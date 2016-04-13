//2015/11/13
#ifndef _C_DOT_RENDERER_H_
#define _C_DOT_RENDERER_H_

#include "IDataRenderer.h"
#include "Core\core.h"
using namespace kk;

class CDotRenderer : public virtual IDataRenderer
{
public:
	CDotRenderer()
		:IDataRenderer()
	{
		m_szName = "DotRenderer";
		m_pDotCloudData=(0);
		//初始化区域为0-1
		m_boundingBox.MinEdge = core::vector3df(0,0,0);
		m_boundingBox.MinEdge = core::vector3df(1,1,1);
	}

	virtual ~CDotRenderer()
	{
		if(m_pDotCloudData)
			m_pDotCloudData->Drop();
	}
	virtual void Process();

	virtual void Draw();
private:
	DataArray* m_pDotCloudData;
	core::aabbox3df m_boundingBox;
	core::vector3df m_voxelSpace;
};
#endif