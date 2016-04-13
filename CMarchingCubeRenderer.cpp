//2015/11/13

#include "CMarchingCubeRenderer.h"
#include "MathHelper.h"
#include <iostream>
#include "Core\core.h"

#include <sstream>
#include <fstream>
#include "DrawingWrapper.h"

void CMarchingCubeRenderer::Process()
{
	if(m_bInitialized) return;
		m_bInitialized = true;

	int sizeX = m_displayData->GetSizeX();
	int sizeY = m_displayData->GetSizeY();
	int sizeZ = m_displayData->GetSizeZ();

	//计算三个方向的长度，将它缩放到 1x1x1的范围内
	//中心在（0.5,0.5,0.5）
	vector3df * _3dPosition = new vector3df[sizeX*sizeY*sizeZ];
	int posIndex = 0;
//#define max(a,b) ((a)>(b)?(a):(b))
	//float voxelSize = 1.0f/(max(max(sizeX,sizeY),sizeZ));
	float voxelSize = 1.0f;
//#undef max

	float fscale = 1.0f/(std::max(sizeX*m_displayData->GetScaleX(),std::max(sizeY*m_displayData->GetScaleY(),sizeZ*m_displayData->GetScaleZ())));
	
	kk::core::vector3df min_pos(0,0,0);
	kk::core::vector3df max_pos(sizeX*m_displayData->GetScaleX()*fscale,
		sizeY*m_displayData->GetScaleY()*fscale,
		sizeZ*m_displayData->GetScaleZ()*fscale);
	float maxValue = 1.0f/std::max(std::max(max_pos.X,max_pos.Y),max_pos.Z);
	max_pos *= maxValue;
	//中心为
	kk::core::vector3df center = (min_pos + max_pos)*0.5f;
	//是要移动到（0.5,0.5,0.5），所以偏移量为：
	kk::core::vector3df offset=kk::core::vector3df(0.5,0.5,0.5) - center;
	for(int x=0;x<sizeX;x++)
	for(int y=0;y<sizeY;y++)
	for(int z=0;z<sizeZ;z++)
	{
		posIndex = x+y*sizeX+z*sizeX*sizeY;
		//最大轴变为了1
		_3dPosition[posIndex].X = x*m_displayData->GetScaleX()*fscale   +offset.X;
		_3dPosition[posIndex].Y = y*m_displayData->GetScaleY()*fscale   +offset.Y;
		_3dPosition[posIndex].Z = z*m_displayData->GetScaleZ()*fscale   +offset.Z;
		//移动到中心来
		//就加上了偏移
		//posIndex++;
	}
	vector3df origin;//(-10,-10,-10);
	origin.X=0;
	origin.Y=0;
	origin.Z=0;
	//if(mc)
	//	delete mc;
	mc = new MarchingCube(sizeX, sizeY, sizeZ, (float*)(m_displayData->GetRawData()),
		_3dPosition, origin, voxelSize, 0.95f);
	
	//delete[] _3dPosition;
}
void CMarchingCubeRenderer::Draw()
{
	mc->Draw();
	core::vector2df unitZ = m_camera->convertScreenPosToMinusOne2One(m_camera->getScreenPosFor(core::vector3df(0,0,1)));
	core::vector2df unitY = m_camera->convertScreenPosToMinusOne2One(m_camera->getScreenPosFor(core::vector3df(0,1,0)));
	core::vector2df unitX = m_camera->convertScreenPosToMinusOne2One(m_camera->getScreenPosFor(core::vector3df(1,0,0)));

	DrawingWrapper::DrawText("Z",Vec(unitZ.X,unitZ.Y,0));
	DrawingWrapper::DrawText("Y",Vec(unitY.X,unitY.Y,0));
	DrawingWrapper::DrawText("X",Vec(unitX.X,unitX.Y,0));
}

void CMarchingCubeRenderer::SaveTriangleMesh(const std::string& filePath)
{
	if(!mc)
	{
		ShowMessage("尚未初始化！");
		return;
	}

	std::ofstream file(filePath.c_str());
	if(!file)
	{
		ShowMessage("打开文件失败！");
		return;
	}
#if 0
	std::vector<kk::core::vector3df>& tris = mc->GetTriangles();
	file<<tris.size()<<"\n";
	for(int i=0;i<(int)tris.size();i++)
	{
		file<<tris[i].X<<" "<<tris[i].Y<<" "<<tris[i].Z<<"\n";
	}
	file.flush();
	file.close();
#else
	std::vector<kk::core::vector3df>& trisPoints = mc->GetTriangles();
	file<<"solid ascii\n";
	//file<<tris.size()<<"\n";
	int triStartIndex=0,nextPoint=1,thirdPoint = 2;

	for(int i=0;i<(int)trisPoints.size();i+=3)
	{
		triStartIndex = i;
		nextPoint = i+1;
		thirdPoint = i+2;

		//normal
		auto normal = (trisPoints[nextPoint] - trisPoints[triStartIndex]).crossProduct(
			trisPoints[thirdPoint] - trisPoints[triStartIndex]);

		normal.normalize();
		//facet normal 0.686406 -0.001236 0.727218
		file<<"facet normal "<<normal.X<<" "<<normal.Y<<" "<<normal.Z<<"\n";
		//	 outer loop
		file<<"outer loop \n";
		//	 vertex -0.247873 -0.360891 -0.342410
		file<<"vertex "<<trisPoints[triStartIndex].X<<" "<<trisPoints[triStartIndex].Y<<" "<<trisPoints[triStartIndex].Z<<"\n";
		//	 vertex -0.260913 -0.352391 -0.330088
		file<<"vertex "<<trisPoints[nextPoint].X<<" "<<trisPoints[nextPoint].Y<<" "<<trisPoints[nextPoint].Z<<"\n";
		//	 vertex -0.256184 -0.375570 -0.334590
		file<<"vertex "<<trisPoints[thirdPoint].X<<" "<<trisPoints[thirdPoint].Y<<" "<<trisPoints[thirdPoint].Z<<"\n";
		// endloop
		file<<"endloop\n";
	 //endfacet
		file<<"endfacet\n";
		
	}
	file<<"endsolid";
	file.flush();
	file.close();
#endif

}