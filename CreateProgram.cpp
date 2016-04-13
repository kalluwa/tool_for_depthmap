#include <glew.h>
#include <glsw.h>
#include <string>
#include <iostream>

#include "MessageHelper.h"

#include "IDataRenderer.h"

using namespace std;

GLuint CreateProgram(const char* vsKey, const char* gsKey, const char* fsKey)
{
    static int first = 1;
    if (first) {
        glswInit();
        glswAddPath("../", ".glsl");
        glswAddPath("./", ".glsl");

        std::string qualifiedPath = std::string("./");
        glswAddPath(qualifiedPath.c_str(), ".glsl");
        glswAddDirective("*", "#version 150");

        first = 0;
    }
    
    const char* vsSource = glswGetShader(vsKey);
    const char* gsSource = glswGetShader(gsKey);
    const char* fsSource = glswGetShader(fsKey);

    const char* msg = "Can't find %s shader: '%s'.\n";
    if(vsSource == 0)
	{
		ShowMessage(msg, "vertex", vsKey);
		return -1;
	}
    if(gsKey != 0 && gsSource == 0)
	{
		ShowMessage(msg, "geometry", gsKey);
		return -1;
	}
	if(fsKey != 0 && fsSource == 0)
	{
		ShowMessage(msg, "fragment", fsKey);
		return -1;
	}
    
    GLint compileSuccess;
    GLchar compilerSpew[256];
    GLuint programHandle = glCreateProgram();

    GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vsHandle, 1, &vsSource, 0);
    glCompileShader(vsHandle);
    glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vsHandle, sizeof(compilerSpew), 0, compilerSpew);
    if(!compileSuccess)
	{
		ShowMessage("Can't compile %s:\n%s", vsKey, compilerSpew);
		return -1;
	}
    glAttachShader(programHandle, vsHandle);

    GLuint gsHandle;
    if (gsKey) {
        gsHandle = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gsHandle, 1, &gsSource, 0);
        glCompileShader(gsHandle);
        glGetShaderiv(gsHandle, GL_COMPILE_STATUS, &compileSuccess);
        glGetShaderInfoLog(gsHandle, sizeof(compilerSpew), 0, compilerSpew);
        
		if(!compileSuccess)
		{
			ShowMessage("Can't compile %s:\n%s", gsKey, compilerSpew);
			return -1;
		}
        glAttachShader(programHandle, gsHandle);
    }
    
    GLuint fsHandle;
    if (fsKey) {
        fsHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fsHandle, 1, &fsSource, 0);
        glCompileShader(fsHandle);
        glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &compileSuccess);
        glGetShaderInfoLog(fsHandle, sizeof(compilerSpew), 0, compilerSpew);

		if(!compileSuccess)
		{
			ShowMessage("Can't compile %s:\n%s", fsKey, compilerSpew);
			return -1;
		}
        glAttachShader(programHandle, fsHandle);
    }

    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(programHandle, sizeof(compilerSpew), 0, compilerSpew);

    if (!linkSuccess) {
		std::string errorInformation = "Link error.\n";
		
        if (vsKey) 
		{
			errorInformation+=("Vertex Shader: ");
			errorInformation+=vsKey;
		}
        if (gsKey)
		{
			errorInformation+=("\nGeometry Shader: ");
			errorInformation +=(gsKey);
		}
        if (fsKey)
		{
			errorInformation+=("\nFragment Shader: ");
			errorInformation+=(fsKey);
			errorInformation+="\n";
		}
		errorInformation += compilerSpew;
		ShowMessage(errorInformation);
    }
    
    return programHandle;
};


GLuint CreateQuad()
{
    short positions[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1,
    };
    
    // Create the VAO:
    GLuint vao=-1;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO:
    GLuint vbo;
    GLsizeiptr size = sizeof(positions);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);

    // Set up the vertex layout:
    GLsizeiptr stride = 2 * sizeof(positions[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_SHORT, GL_FALSE, stride, 0);

    return vao;
}

GLuint CreateCube(float scaleX,float scaleY,float scaleZ)
{
    #define O 0.0f,
    #define X +1.0f,
    float positions[] = { 
		scaleX*O scaleY*O scaleZ*O 
		scaleX*O scaleY*O scaleZ*X 
		scaleX*O scaleY*X scaleZ*O 
		scaleX*O scaleY*X scaleZ*X 

		scaleX*X scaleY*O scaleZ*O 
		scaleX*X scaleY*O scaleZ*X 
		scaleX*X scaleY*X scaleZ*O 
		scaleX*X scaleY*X scaleZ*X };
    #undef O
    #undef X
    
    short indices[] = {
        7, 3, 1, 1, 5, 7, // Z+
        0, 2, 6, 6, 4, 0, // Z-
        6, 2, 3, 3, 7, 6, // Y+
        1, 0, 4, 4, 5, 1, // Y-
        3, 2, 0, 0, 1, 3, // X-
        4, 6, 7, 7, 5, 4, // X+
    };

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    {
        GLuint vbo;
        GLsizeiptr size = sizeof(positions);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
    }

    // Create the VBO for indices:
    {
        GLuint vbo;
        GLsizeiptr size = sizeof(indices);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
    }

    // Set up the vertex layout:
    GLsizeiptr stride = 3 * sizeof(positions[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);

    return vao;
}


Surface CreateSurface(GLsizei width, GLsizei height, int numComponents, int numTargets)
{
    Surface surface;
    glGenFramebuffers(1, &surface.FboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, surface.FboHandle);

    for (int attachment = 0; attachment < numTargets; ++attachment) {

        GLuint textureHandle;
        glGenTextures(1, &textureHandle);
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        surface.TextureHandle[attachment] = textureHandle;

        const int UseHalfFloats = 1;
        if (UseHalfFloats) {
            switch (numComponents) {
                case 1: glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_HALF_FLOAT, 0); break;
                case 2: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_HALF_FLOAT, 0); break;
                case 3: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_HALF_FLOAT, 0); break;
                case 4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, 0); break;
                default: ShowMessage("Illegal slab format.");
            }
        } else {
            switch (numComponents) {
                case 1: glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0); break;
                case 2: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, 0); break;
                case 3: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0); break;
                case 4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0); break;
                default: ShowMessage("Illegal slab format.");
            }
        }

        if(GL_NO_ERROR != glGetError())
		{
			ShowMessage("Unable to attach color buffer");
		}
        GLuint colorbuffer;
        glGenRenderbuffers(1, &colorbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, GL_TEXTURE_2D, textureHandle, 0);
        
    }
    
    if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		ShowMessage("Unable to create FBO.");
	}
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return surface;
}

Surface CreateVolume(GLsizei width, GLsizei height, GLsizei depth)
{
    Surface surface;
    glGenFramebuffers(1, &surface.FboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, surface.FboHandle);

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_3D, textureHandle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
    surface.TextureHandle[0] = textureHandle;

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, width, height, depth, 0, GL_RGB, GL_HALF_FLOAT, 0);

    GLint miplevel = 0;

    GLuint colorbuffer;
    glGenRenderbuffers(1, &colorbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureHandle, miplevel);
    if(GL_NO_ERROR != glGetError())
	{
		ShowMessage("Unable to attach color buffer");
	}
    if(GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
		ShowMessage("Unable to create FBO.");
	}

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return surface;
}

GLuint CreateVolumeFromData(GLsizei width, GLsizei height, GLsizei depth,char* dataInChar)
{
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_3D, textureHandle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, dataInChar);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_R8, width, height, depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,(unsigned char*)dataInChar);

	if(glGetError() != GL_NO_ERROR)
	{
		ShowMessage("Tex3D error");
	}
    return textureHandle;
}

GLuint CreateVolumeFromData(GLsizei width, GLsizei height, GLsizei depth,float* dataInFloat)
{

	int widthP2 = 1;
	while(widthP2<width) widthP2<<=1;
	int heightP2 = 1;
	while(heightP2<height) heightP2<<=1;
	int depthP2 = 1;
	while(depthP2<depth) depthP2<<=1;

	//int maxSize = std::max(std::max(widthP2,heightP2),depthP2);
	int SizeX =widthP2;
	int SizeY =heightP2;
	int SizeZ =depthP2;

	char* pixels = new char[SizeX*SizeY*SizeZ];
    {
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
        for (int z = 1; z < depth-1; ++z) {
            for (int y = 1; y < height-1; ++y) {
                for (int x = 1; x < width-1; ++x) {
                    int cx = x - SizeX / 2;
                    int cy = y - SizeY / 2;
                    int cz = z - SizeZ / 2;
					float r2 = 0.001f*(cx*cx+cy*cy+cz*cz);
                    float density =0.4f * std::exp(-r2 / 1.0f);
#define valueAt(x,y,z) dataInFloat[(x)+(y)*width+(z)*width*height]
					float count=0.0f;
					for(int i=-1;i<2;i++)
					for(int j=-1;j<2;j++)
					for(int k=-1;k<2;k++)
					{
						if(valueAt(x+i,y+j,z+k)>0.1)
							count+=1/27.0f;
					}
					density = count;//dataInFloat[x+y*width+z*width*height];

					//if(dataInFloat[x+y*width+z*width*height] > 0.5f)
					//	density = 1.0f;
					//else
					//	density = 0.0f;
					pDest[x+y*SizeX+z*SizeY*SizeX]= (char)(255.0f * density);
                }
            }
        }

    }

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_3D, textureHandle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, width,height,depth, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_R8, SizeX, SizeY, SizeZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,(unsigned char*)pixels);

    //delete [] pixels;

    return textureHandle;
}

GLuint CreateCpuSplat(GLuint quadVao)
{

    const int Size = 64;
    const float InnerScale = 0.4f;
    const float RadiusScale = 0.001f;

    char* pixels = new char[Size*Size*Size];
    {
        // http://www.stat.wisc.edu/~mchung/teaching/MIA/reading/diffusion.gaussian.kernel.pdf.pdf
        float doubleVariance = 2.0f * InnerScale * InnerScale;
        float normalizationConstant = 1.0f / std::pow(std::sqrt(6.2832f) * InnerScale, 3.0f);
        char* pDest = pixels;
        float maxDensity = 0;
        float sumDensity = 0;
        float minDensity = 100.0f;
        for (int z = 0; z < Size; ++z) {
            for (int y = 0; y < Size; ++y) {
                for (int x = 0; x < Size; ++x) {
                    int cx = x - Size / 2;
                    int cy = y - Size / 2;
                    int cz = z - Size / 2;
                    float r2 = RadiusScale * float(cx*cx + cy*cy + cz*cz);
                    float density = normalizationConstant * std::exp(-r2 / doubleVariance);
                    maxDensity = std::max(maxDensity, density);
                    minDensity = std::min(minDensity, density);
                    sumDensity += density;
                    *pDest++ = (char)(255.0f * density);
                }
            }
        }

		//test
		//pDest = pixels;
		//for (int z = 0; z < Size; ++z) {
  //          for (int y = 0; y < Size; ++y) {
  //              for (int x = 0; x < Size; ++x) {
		//			//if(y>Size/4&&y<Size/4*3
		//			//	&&x>Size/4&&x<Size/4*3
		//			//	&&z>Size/4&&z<Size/4*3)
		//			//	*pDest++ =255;
		//			////else if(y<Size/2)
		//			////	*pDest++ =1;
		//			//else
		//			//	*pDest++ = 0;

		//			*pDest++ = rand();
		//		}
		//	}
		//}
    }

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_3D, textureHandle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, Size, Size, Size, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);

    delete [] pixels;

    return textureHandle;
}

GLuint CreateNoise()
{
    int Width = 256;
    int Height = 256;

    char* pixels = new char[Width * Height];
    
    char* pDest = pixels;
    for (int i = 0; i < Width * Height; i++) {
        *pDest++ = rand() % 256;
    }

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Width, Height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);

    delete [] pixels;

    return textureHandle;
}