//2015/11/13

#ifndef _DRAWING_WRAPPER_H_
#define _DRAWING_WRAPPER_H_

#define _OPEN_GL_

#include <tchar.h>

#ifdef _OPEN_GL_

#include <QtOpenGL/QGLWidget>
#include "glText.h"

#else

#endif

#include "MathHelper.h"

class IDrawingDevice
{
public:
	virtual void Init(){};

	virtual ~IDrawingDevice(){}

	virtual void DrawLine(const Vec& v1,const Vec& v2)=0;
	virtual void DrawLine(const Vec& v1,const Vec& v2,int r,int g,int b)=0;

	virtual void DrawPoint(const Vec&v1)=0;

	virtual void DrawUnitCube(float Size=1.0f)=0;
	//batch drawing
	virtual void DrawBatchBegin()=0;
	virtual void DrawBatchTris(const Vec& v1,const Vec& v2,const Vec& v3)=0;
	virtual void DrawBatchEnd()=0;
	virtual void ClearScreen(int r,int g,int b,int a)=0;
	virtual void DrawText(const std::string& message,const Vec& pos)=0;
};
#ifdef _OPEN_GL_

class OpenGLDevice : public virtual IDrawingDevice
{
private:
	CGlFont glFont;
public:

	void Init()
	{
		glFont.InitFont(_T("ËÎÌו"));
		glFont.m_X = 0.8;
		glFont.m_Y = 0.8;
		glFont.m_dHeight = 0.2;
		glFont.SetColor(255,255,0);
	}

	void DrawLine(const Vec& v1,const Vec& v2)
	{
		glColor3ub(255,255,0);
		glBegin(GL_LINES);
		glVertex3f(v1.x,v1.y,v1.z);
		glVertex3f(v2.x,v2.y,v2.z);
		glEnd();
	};

	void DrawLine(const Vec& v1,const Vec& v2,int r,int g,int b)
	{
		glColor3ub(r,g,b);
		glBegin(GL_LINES);
		glVertex3f(v1.x,v1.y,v1.z);
		glVertex3f(v2.x,v2.y,v2.z);
		glEnd();
	};

	void DrawPoint(const Vec& v1)
	{
		glColor3ub(255,0,0);
		glBegin(GL_POINTS);
		glVertex3f(v1.x,v1.y,v1.z);
		glEnd();
	}

	virtual void DrawBatchBegin(){ glBegin(GL_TRIANGLES);};
	virtual void DrawBatchTris(const Vec& v1,const Vec& v2,const Vec& v3)
	{
		glVertex3f(v1.x,v1.y,v1.z);
		glVertex3f(v2.x,v2.y,v2.z);
		glVertex3f(v3.x,v3.y,v3.z);
	};
	virtual void DrawBatchEnd(){ glEnd();};

	virtual void ClearScreen(int r,int g,int b,int a)
	{
		glClearColor(r/255.0f,g/255.0f,b/255.0f,a/255.0f);
	};

	virtual void DrawUnitCube(float Size)
	{
		
		glColor3ub(255,0,0);
		glBegin(GL_LINES);
		
		glColor3ub(0,255,0);
		glVertex3f(0,0,0);glVertex3f(0,0,Size);//Z axis
		glColor3ub(0,0,0);
		glVertex3f(0,0,Size);glVertex3f(0,Size,Size);
		glVertex3f(0,Size,Size);glVertex3f(0,Size,0);
		glColor3ub(255,0,0);
		glVertex3f(0,Size,0);glVertex3f(0,0,0);//Y axis

		glColor3ub(0,0,0);
		glVertex3f(Size,0,0);glVertex3f(Size,0,Size);
		glVertex3f(Size,0,Size);glVertex3f(Size,Size,Size);
		glVertex3f(Size,Size,Size);glVertex3f(Size,Size,0);
		glVertex3f(Size,Size,0);glVertex3f(Size,0,0);

		glColor3ub(0,0,255);
		glVertex3f(0,0,0);glVertex3f(Size,0,0);//X axis
		glColor3ub(0,0,0);
		glVertex3f(0,0,Size);glVertex3f(Size,0,Size);
		glVertex3f(0,Size,Size);glVertex3f(Size,Size,Size);
		glVertex3f(0,Size,0);glVertex3f(Size,Size,0);
		glEnd();

	}

	void DrawText(const std::string& message,const Vec& position)
	{
		glFont.m_X = position.x;
		glFont.m_Y = position.y;
		
		glFont.m_dHeight = 0.2;
		glFont.Draw2DText((char*)message.c_str());
	}
};

#endif
//struct DrawingDevice
//{
//	union
//	{
//		OpenGLDevice device;
//	};
//};

inline IDrawingDevice* CreateDevice()
{
	return new OpenGLDevice;
};

class DrawingWrapper
{
public:
	static IDrawingDevice* m_device;

	static void Initialize()
	{
		m_device = CreateDevice();
	}

	static void DrawPoint(const Vec& v1)
	{
		m_device->DrawPoint(v1);
	}

	static void ClearScreen(int r,int g,int b,int a)
	{
		m_device->ClearScreen(r,g,b,a);
	}
	static void DrawLine(const Vec& v1,const Vec& v2,int r=0,int g=0,int b=0)
	{
		m_device->DrawLine(v1,v2,r,g,b);
	};

	static void DrawBatchBegin()
	{
		m_device->DrawBatchBegin();
	}
	static void DrawBatchTris(const Vec& v1,const Vec& v2,const Vec& v3)
	{
		m_device->DrawBatchTris(v1,v2,v3);
	}
	static void DrawBatchEnd()
	{
		m_device->DrawBatchEnd();
	}

	static void DrawUnitCube()
	{
		m_device->DrawUnitCube();
	}
	static void DrawSingleTri(const Vec& v1,const Vec& v2,const Vec& v3)
	{
		m_device->DrawBatchBegin();
		m_device->DrawBatchTris(v1,v2,v3);
		m_device->DrawBatchEnd();
	}

	static void DrawText(const std::string& msg,const Vec& pos)
	{
		m_device->DrawText(msg,pos);
	}
	static void Drop()
	{
		if(m_device)
			delete m_device;
	}
};





#endif