//2015/11/16
#include <Windows.h>
#include <glew.h>
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glew32s.lib")

#include "CRayCastRenderer.h"
#include "MessageHelper.h"
#include "Core\core.h"
#include "Definitions.h"
using namespace kk;



CRayCastRenderer::CRayCastRenderer(int typeOfRendering)
	:IDataRenderer()
{
	m_szName = "RayCastRenderer";
	m_iTypeOfRendering = typeOfRendering;
	//初始化区域为0-1
	m_boundingBox.MinEdge = core::vector3df(0,0,0);
	m_boundingBox.MinEdge = core::vector3df(1,1,1);
}

//bool CRayCastRenderer::Create3DTexture()
//{
//	
//	glGenFramebuffers(1, &m_3dFBOHandle);
//    glBindFramebuffer(GL_FRAMEBUFFER, m_3dFBOHandle);
//
//    unsigned int textureHandle;
//    glGenTextures(1, &textureHandle);
//    glBindTexture(GL_TEXTURE_3D, textureHandle);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        
//    m_3dTextureId = textureHandle;
//
//    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, m_iSizeX, m_iSizeY, m_iSizeZ, 0, GL_RGB, GL_HALF_FLOAT, 0);
//
//    GLint miplevel = 0;
//
//    GLuint colorbuffer;
//    glGenRenderbuffers(1, &colorbuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
//    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureHandle, miplevel);
//    if(GL_NO_ERROR != glGetError())
//	{
//		ShowMessage("Unable to attach color buffer");
//		return false;
//	}
//    
//    if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
//	{
//		ShowMessage("Unable to create FBO.");
//		return false;
//	}
//
//    glClearColor(0, 0, 0, 0);
//    glClear(GL_COLOR_BUFFER_BIT);
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	return true;
//}

bool CRayCastRenderer::Create3DShader()
{
	return true;
}
bool CRayCastRenderer::Init()
{
	if(!m_bInitialized)
	{
		auto err = glewInit();
		if(GLEW_OK!=err)
		{
			//ShowMessage((char*)glewGetErrorString(err));
			return false;
		}
		m_bInitialized = true;
	}
	//cube
	QuadVao = CreateQuad();
	CubeVao = CreateCube();
	//shader
	RayEndpointsProgram = CreateProgram("Volume.Vertex", 0, "Volume.Endpoints");
    switch(m_iTypeOfRendering)
	{
	case 0:
		RaycastProgram = CreateProgram("Volume.Quad", 0, "Volume.Lighting");
		break;
	case 1:
		RaycastProgram = CreateProgram("Volume.Quad", 0, "Volume.Semitransparent");
		break;
	default:
		ShowMessage("unsupported rendering mode");
		return false;
	}

	RayEndPointsSurface = CreateSurface(VIEWPORT_WIDTH,VIEWPORT_HEIGHT, 3, 2);
#if 0
	//fill data here
/*	int SizeX = 64;//m_displayData->GetSizeX();
	int SizeY = 61;//m_displayData->GetSizeY();
	int SizeZ = 45;//m_displayData->GetSizeZ();
*/	
	int SizeX =128;61;64+rand()%32; m_displayData->GetSizeX();
	int SizeY =128;50;64+rand()%32; m_displayData->GetSizeY();
	int SizeZ =128;50;64+rand()%32; m_displayData->GetSizeZ();

	char* pixels = new char[SizeX*SizeY*SizeZ];
    {
        // http://www.stat.wisc.edu/~mchung/teaching/MIA/reading/diffusion.gaussian.kernel.pdf.pdf
        //float doubleVariance = 2.0f * InnerScale * InnerScale;
        //float normalizationConstant = 1.0f / std::pow(std::sqrt(6.2832f) * InnerScale, 3.0f);
        char* pDest = pixels;
		for (int z = 0; z < SizeZ; ++z) {
            for (int y = 0; y < SizeY; ++y) {
                for (int x = 0; x < SizeX; ++x) {
					*pDest++=(char)0;
					//pDest[x+y*Size+z*Size*Size]=255;
				}
			}
		}
		pDest = pixels;
        for (int z = 1; z < SizeZ-1; ++z) {
            for (int y = 1; y < SizeY-1; ++y) {
                for (int x = 1; x < SizeX-1; ++x) {
                    int cx = x - SizeX / 2;
                    int cy = y - SizeY / 2;
                    int cz = z - SizeZ / 2;
					float r2 = 0.001f*(cx*cx+cy*cy+cz*cz);
                    float density =0.4f * std::exp(-r2 / 1.0f);
                    //if(density>0.5)
					if(x>SizeX/2&&y>SizeY/2&&z>SizeZ/2)
						density = 1.0f;
					else
						density = 0.0f;
					pDest[x+y*SizeX+z*SizeY*SizeX]= (char)(255.0f * density);
                }
            }
        }

    }

	SplatTexture = CreateVolumeFromData(SizeX,SizeY,SizeZ,pixels);
	delete []pixels;


	//SplatTexture = CreateCpuSplat(QuadVao);
#else
	//create volume texture
	switch(m_displayData->GetVoxelStride())
	{
	case 1:
		SplatTexture = CreateVolumeFromData(m_displayData->GetSizeX(),
			m_displayData->GetSizeY(),m_displayData->GetSizeZ(),(char*)m_displayData->GetRawData());
		break;
	case 4://float data
		SplatTexture = CreateVolumeFromData(m_displayData->GetSizeX(),
			m_displayData->GetSizeY(),m_displayData->GetSizeZ(),(float*)m_displayData->GetRawData());
		break;
	default:
		ShowMessage("source data not supported");
		return false;
	}

	NoiseTexture = CreateNoise();
#endif
	glBlendFunc(GL_ONE, GL_ONE);
    glDisable(GL_DEPTH_TEST);

	//计算boundingbox的实际区域
#define Max(a,b) ((a)>(b)?(a):(b))
	int sizeX = m_displayData->GetSizeX();
	int sizeY = m_displayData->GetSizeY();
	int sizeZ = m_displayData->GetSizeZ();

	int widthP2 = 1;
	while(widthP2<sizeX) widthP2<<=1;
	int heightP2 = 1;
	while(heightP2<sizeY) heightP2<<=1;
	int depthP2 = 1;
	while(depthP2<sizeZ) depthP2<<=1;

	float maxAxisLength = (Max(widthP2*m_displayData->GetScaleX(),Max(heightP2*m_displayData->GetScaleY(),depthP2*m_displayData->GetScaleZ())));
	m_boundingBox.MinEdge = core::vector3df(0,0,0);
	m_boundingBox.MaxEdge = core::vector3df(widthP2*m_displayData->GetScaleX(),heightP2*m_displayData->GetScaleY(),depthP2*m_displayData->GetScaleZ())/maxAxisLength;

	core::vector3df targetCenter = core::vector3df(sizeX*m_displayData->GetScaleX(),sizeY*m_displayData->GetScaleY(),sizeZ*m_displayData->GetScaleZ())/maxAxisLength;
	targetCenter /=2.0f;
	
	core::vector3df offsetToUnitCubeCenter = core::vector3df(0.5f,0.5f,0.5f) - targetCenter;
	//move boundingBox
	m_boundingBox.MinEdge += offsetToUnitCubeCenter;
	m_boundingBox.MaxEdge += offsetToUnitCubeCenter;
	//包围盒的最大长度为1.0f
#undef Max
	return true;
}

void CRayCastRenderer::Draw()
{
	glDisable(GL_CULL_FACE);
	if(!m_bInitialized)
	{
		Init();
		return;
	}

	// Clear the texture bindings:
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_3D, 0);

	// Update the ray start & stop surfaces:
    BindProgram(RayEndpointsProgram,m_boundingBox);
    glBindFramebuffer(GL_FRAMEBUFFER, RayEndPointsSurface.FboHandle);
    glEnable(GL_BLEND);
    glDrawBuffers(2, EnumArray(GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1));
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(CubeVao);
    //glEnable(GL_SCISSOR_TEST);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
    //glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);

	
    // Perform the raycast:
    BindProgram(RaycastProgram,m_boundingBox);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffers(2, EnumArray(GL_BACK_LEFT, GL_NONE));
    glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
	//glClearColor(0,0,0,0);
	//glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glDisable(GL_BLEND);

    glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_BLEND);
    //glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, RayEndPointsSurface.TextureHandle[0]);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, RayEndPointsSurface.TextureHandle[1]);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_3D, SplatTexture);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, NoiseTexture);
	glBindVertexArray(QuadVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(0);
	/*glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, 0);
	*/
}

void CRayCastRenderer::Process()
{
	Init();
}

bool CRayCastRenderer::LoadShader(const std::string& shaderName)
{

	return true;
}



//static functions
static void BindProgram(GLuint program,const kk::core::aabbox3df& boundingBox)
{
	
    glUseProgram(program);
	float ModelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX,ModelView);
	float Projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX,Projection);

	core::matrix4 identity;
    GLint modelview = glGetUniformLocation(program, "Modelview");
    if (modelview > -1) {
        glUniformMatrix4fv(modelview, 1, 0, (float*) ModelView);
    }
	
    //GLint modelviewProjection = glGetUniformLocation(program, "ModelviewProjection");
    //if (modelviewProjection > -1) {
		//core::matrix4 view(ModelView);
		//view = view.getTransposed();
		//core::matrix4 proj(identity);
		//proj = proj.getTransposed();
		//core::matrix4 wvp = view*proj;
		//wvp = wvp.getTransposed();
		
	
        //glUniformMatrix4fv(modelviewProjection, 1, 0, (float*) &wvp);
    //}

    GLint normalMatrix = glGetUniformLocation(program, "NormalMatrix");
    if (normalMatrix > -1) {
		core::matrix4 nm = identity;//(*(core::matrix4*)ModelView);
		nm.getTransposed();
        float packed[9] = {
            nm(0,0), nm(0,1), nm(0,2),
            nm(1,0), nm(1,1), nm(1,2),
            nm(2,0), nm(2,1), nm(2,2) };
        glUniformMatrix3fv(normalMatrix, 1, 0, (float*) &packed);
    }
    
    GLint projection = glGetUniformLocation(program, "Projection");
    if (projection > -1)
        glUniformMatrix4fv(projection, 1, 0, (float*) &Projection);

    GLint size = glGetUniformLocation(program, "InverseSize");
    if (size > -1)
        glUniform2f(size, 1.0f / VIEWPORT_WIDTH, 1.0f / VIEWPORT_HEIGHT);

    GLint diffuseMaterial = glGetUniformLocation(program, "DiffuseMaterial");
    if (diffuseMaterial > -1)
        glUniform3f(diffuseMaterial, 1, 1, 0.5f);

    GLint lightPosition = glGetUniformLocation(program, "LightPosition");
    if (lightPosition > -1) {
        core::vector3df v(0.25f, 0.25f, 1);
		v.normalize();
        glUniform3fv(lightPosition, 1, (float*) &v);
    }

    GLint rayStart = glGetUniformLocation(program, "RayStart");
    if (rayStart > -1)
        glUniform1i(rayStart, 0);

    GLint rayStop = glGetUniformLocation(program, "RayStop");
    if (rayStop > -1)
        glUniform1i(rayStop, 1);

    GLint volume = glGetUniformLocation(program, "Volume");
    if (volume > -1)
        glUniform1i(volume, 2);

	//bounding box
	GLint boxMinCorner = glGetUniformLocation(program,"BoxMinCorner");
	if(boxMinCorner >-1)
		glUniform3fv(boxMinCorner,1,&boundingBox.MinEdge.X);
	GLint boxExtent = glGetUniformLocation(program,"BoxExtent");
	if(boxMinCorner >-1)
	{
		static core::vector3df extent = boundingBox.getExtent();
		glUniform3fv(boxExtent,1,&extent.X);
	}

    GLint noise = glGetUniformLocation(program, "Noise");
    if (noise > -1)
        glUniform1i(noise, 3);
}

static GLenum* EnumArray(GLenum a, GLenum b)
{
    static GLenum enums[2];
    enums[0] = a;
    enums[1] = b;
    return &enums[0];
}