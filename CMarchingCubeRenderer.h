//2015/11/13

#ifndef _C_MARCHING_CUBE_RENDERER_H_
#define _C_MARCHING_CUBE_RENDERER_H_

#include "IDataRenderer.h"
#include "MarchingCube.h"

class CMarchingCubeRenderer : public virtual IDataRenderer
{
public:
	CMarchingCubeRenderer()
		:IDataRenderer()
	{
		m_szName = "CMarchingCubeRenderer";
		mc = 0;
	}

	virtual void Process();
	virtual void Draw();

	void SaveTriangleMesh(const std::string& filePath);
private:
	MarchingCube* mc;
};
#endif