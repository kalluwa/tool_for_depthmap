//2015/11/12

#ifndef _DATA_RENDERER_H_
#define _DATA_RENDERER_H_

#include "IBase.h"
#include "FloatDataArray.h"

#include "helper\CArcCamera.h"

namespace EnumSpace
{
	enum RenderingMode
	{
		Point,
		MarchingCube,
		Wireframe,
		MeshPoint,
		Volume,
		VolumeTransparent,

		//其它事件
		CpuDepth,

		//计数
		Count
	};

}

static EnumSpace::RenderingMode g_renderingMode = EnumSpace::Point;

struct Surface {
    unsigned int FboHandle;
    unsigned int TextureHandle[2];
};

class IDataRenderer : public virtual IBase
{
public :
	IDataRenderer():IBase("DataRenderer"),m_bInitialized(false),m_displayData(0),fScale(1.0f),m_camera(0){SetnMaxSize();}

	IDataRenderer(DataArray* rawData,float nMaxSize):IBase("DataRenderer"),m_bInitialized(false),fScale(1.0f),m_camera(0)
	{
		SetData(rawData);
		SetnMaxSize(nMaxSize);
	}

	void SetData(DataArray* rawData)
	{
		m_displayData= rawData;
		m_displayData->Grab();
	}
	
	void SetCamera(kk::scene::CArcCameraNode* cam) { m_camera = cam;}
	~IDataRenderer()
	{
		if(m_displayData)
			m_displayData->Drop();
	}
	
	void SetnMaxSize(float size=50)
	{
		nMaxSize = size;
		float space = 3.3333f/1.20f;
		if(m_displayData)
		{
			space = m_displayData->GetScaleZ()/m_displayData->GetScaleX();

#define Min(a,b) ((a)<(b)?(a):(b))
		fScale = Min((float)nMaxSize/((float)m_displayData->GetSizeZ()*space),
			Min((float)nMaxSize/(float)m_displayData->GetSizeX(),(float)nMaxSize/(float)m_displayData->GetSizeY()));
#undef Min
		}
		else
		{
			fScale = 1.0;
		}
	}

	virtual void Process(){}

	virtual void Draw()=0;
#if 1 //属性
	unsigned char* GetRawData(){ return m_displayData->GetRawData();}
	int GetRawVoxelSize(){return m_displayData->GetVoxelStride();}
#endif

protected:
	bool m_bInitialized;
	DataArray* m_displayData;

	kk::scene::CArcCameraNode* m_camera;
	float nMaxSize;
	float fScale;
};

//shader program
unsigned int CreateProgram(const char* vsKey, const char* gsKey, const char* fsKey);
unsigned int CreateQuad();
unsigned int CreateCube(float scaleX=1.0f,float scaleY=1.0f,float scaleZ=1.0f);
Surface CreateSurface(int width, int height, int numComponents, int numTargets);
Surface CreateVolume(int width, int height, int depth);
unsigned int CreateVolumeFromData(int width, int height, int depth,char* dataInChar);
unsigned int CreateVolumeFromData(int width, int height, int depth,float* dataInFloat);
unsigned int CreateCpuSplat(unsigned int quadVao);
unsigned int CreateNoise();
#endif