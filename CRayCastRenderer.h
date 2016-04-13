//2015/11/16

#ifndef _C_RAYCAST_RENDERER_H_
#define _C_RAYCAST_RENDERER_H_

#include "Core\core.h"

#include "IDataRenderer.h"

static void BindProgram(GLuint program,const kk::core::aabbox3df& boundingBox);
static GLenum* EnumArray(GLenum a, GLenum b);

class CRayCastRenderer: public virtual IDataRenderer
{
public:
	CRayCastRenderer(int typeOfRendering);

	//virtual method
	virtual void Process();

	virtual void Draw();

	bool Init();
private:
	//private methods
	bool LoadShader(const std::string& shaderName);

	//bool Create3DTexture();

	bool Create3DShader();
private:
	//private variables
	unsigned int m_3dFBOHandle,m_3dTextureId,m_3dTextureColorBuffer;
	int m_iTypeOfRendering;
	unsigned int QuadVao,CubeVao;//frame
	unsigned int RayEndpointsProgram,RaycastProgram;//shader

	Surface RayEndPointsSurface;
	unsigned int SplatTexture;//test[volume data]
	unsigned int NoiseTexture;

	//œ‘ æ«¯”Ú
	kk::core::aabbox3df m_boundingBox;
};
#endif