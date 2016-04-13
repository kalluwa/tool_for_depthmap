//2015//11/13

#include "CDotRenderer.h"
#include "DrawingWrapper.h"

void CDotRenderer::Process()
{
	if(m_bInitialized)
		return;
	m_bInitialized = true;
	//copy data to m_pMarchingCubeArray
	m_pDotCloudData  = new DataArray(m_displayData->GetRawData(),m_displayData->GetVoxelStride(),
			m_displayData->GetSizeX(),m_displayData->GetSizeY(),m_displayData->GetSizeZ(),true);

	m_pDotCloudData->Clear();
	int sizeX = m_displayData->GetSizeX();
	int sizeY = m_displayData->GetSizeY();
	int sizeZ = m_displayData->GetSizeZ();
	//float scaleX = m_displayData->GetScaleX();
	//float scaleY = m_displayData->GetScaleY();
	//float scaleZ = m_displayData->GetScaleZ();
	int inteval = m_displayData->GetVoxelStride();

	
	int count = 0;
	//float scaleZHere = scaleZ;
	for(int i = 1;i<sizeX-1;i++)
	{
		for(int j=1;j<sizeY-1;j++)
		{
			for(int k=1;k<sizeZ-1;k++)
			{
				int index = i+sizeX*j+sizeX*sizeY*k;
				
				switch(inteval)
				{
				case 4:
					count = 0;
					for(int x=-1;x<2;x++)
					for(int y=-1;y<2;y++)
					for(int z=-1;z<2;z++)
					{
						if(((float*)m_displayData->GetRawData())[(i+x)+sizeX*(y+j)+sizeX*sizeY*(k+z)] > 0.5f)
							count++;
					}
					if(count!=0&&count!=27)
					if(((float*)m_displayData->GetRawData())[index]>0.5f)
					{
						//scaleZHere = scaleZ+(scaleZ-1.0f) * (rand()%10-5.0f)*0.04f;
						//DrawingWrapper::DrawPoint(Vec(i/((double)sizeX)*scaleX,j/(float)sizeY*scaleY,k/(float)sizeZ*scaleZHere));
						m_pDotCloudData->SetValueTo(i,j,k,1.0f);
					}
					break;
				case 1:
					count=0;
					for(int x=-1;x<2;x++)
					for(int y=-1;y<2;y++)
					for(int z=-1;z<2;z++)
					{
						if((m_displayData->GetRawData())[(i+x)+sizeX*(y+j)+sizeX*sizeY*(k+z)] > 1)
							count++;
					}
					if(count!=0&&count!=27)
					if((m_displayData->GetRawData())[index]>1)
						//DrawingWrapper::DrawPoint(Vec(i/((double)sizeX)*scaleX,j/(float)sizeY*scaleY,k/(float)sizeZ*scaleZ));
						m_pDotCloudData->SetValueTo(i,j,k,1.0f);
					break;
				}
			}
		}
	}

	//计算boundingbox的实际区域
	float maxAxisLength = (std::max(sizeX*m_displayData->GetScaleX(),std::max(sizeY*m_displayData->GetScaleY(),sizeZ*m_displayData->GetScaleZ())));
	m_boundingBox.MinEdge = core::vector3df(0,0,0);
	m_boundingBox.MaxEdge = core::vector3df(sizeX*m_displayData->GetScaleX(),sizeY*m_displayData->GetScaleY(),sizeZ*m_displayData->GetScaleZ())/maxAxisLength;
	core::vector3df boundingBoxCenter = m_boundingBox.getCenter();
	core::vector3df offsetToUnitCubeCenter = core::vector3df(0.5f,0.5f,0.5f) - boundingBoxCenter;
	//move boundingBox
	m_boundingBox.MinEdge += offsetToUnitCubeCenter;
	m_boundingBox.MaxEdge += offsetToUnitCubeCenter;
	//包围盒的最大长度为1.0f
	m_voxelSpace = m_boundingBox.getExtent();
	m_voxelSpace.X /= (sizeX-1);
	m_voxelSpace.Y /= (sizeY-1);
	m_voxelSpace.Z /= (sizeZ-1);
}
void CDotRenderer::Draw()
{
	DrawingWrapper::ClearScreen(255,255,255,255);
	//int size = m_pDotCloudData->GetSizeX()*m_pDotCloudData->GetSizeY()*m_pDotCloudData->GetSizeZ();
	DrawingWrapper::DrawUnitCube();
	core::vector2df unitZ = m_camera->convertScreenPosToMinusOne2One(m_camera->getScreenPosFor(core::vector3df(0,0,1)));
	core::vector2df unitY = m_camera->convertScreenPosToMinusOne2One(m_camera->getScreenPosFor(core::vector3df(0,1,0)));
	core::vector2df unitX = m_camera->convertScreenPosToMinusOne2One(m_camera->getScreenPosFor(core::vector3df(1,0,0)));

	DrawingWrapper::DrawText("Z",Vec(unitZ.X,unitZ.Y,0));
	DrawingWrapper::DrawText("Y",Vec(unitY.X,unitY.Y,0));
	DrawingWrapper::DrawText("X",Vec(unitX.X,unitX.Y,0));
	int sizeX = m_displayData->GetSizeX();
	int sizeY = m_displayData->GetSizeY();
	int sizeZ = m_displayData->GetSizeZ();
	//float scaleX = m_displayData->GetScaleX();
	//float scaleY = m_displayData->GetScaleY();
	//float scaleZ = m_displayData->GetScaleZ();
	const float range = 0.1f;
	float startPosX = m_boundingBox.MinEdge.X,
		startPosY = m_boundingBox.MinEdge.Y,
		startPosZ = m_boundingBox.MinEdge.Z;
	srand(0);
	if(m_displayData->GetVoxelStride()==4)
	{
		for(int i=0;i<sizeX;i++)
		for(int j=0;j<sizeY;j++)
		for(int k=0;k<sizeZ;k++)
		{
			if(m_pDotCloudData->GetValueAt(i,j,k)>0.5f)
				//DrawingWrapper::DrawPoint(Vec(i/((double)sizeX)*scaleX,j/(float)sizeY*scaleY,k/(float)sizeZ*scaleZ));
				DrawingWrapper::DrawPoint(Vec(startPosX + i*m_voxelSpace.X,
				startPosY + j*m_voxelSpace.Y,
				startPosZ + k*m_voxelSpace.Z+range*1/abs((1+k/(float)sizeZ-0.5f))*(rand()/(float)RAND_MAX)));
		}
		
	}
	else if(m_displayData->GetVoxelStride()==1)
	{
		for(int i=0;i<sizeX;i++)
		for(int j=0;j<sizeY;j++)
		for(int k=0;k<sizeZ;k++)
		{
			if(m_pDotCloudData->GetValueAt(i,j,k)>0)
				//DrawingWrapper::DrawPoint(Vec(i/((double)sizeX)*scaleX,j/(float)sizeY*scaleY,k/(float)sizeZ*scaleZ));
				DrawingWrapper::DrawPoint(Vec(startPosX + i*m_voxelSpace.X,
				startPosY + j*m_voxelSpace.Y,
				startPosZ + k*m_voxelSpace.Z+range*1/abs((1+k/(float)sizeZ-0.5f))*(rand()/(float)RAND_MAX)));
		}
	}
	
	
}