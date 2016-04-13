#ifndef UseDx
#include <Windows.h>
#include <gl/gl.h>
#include <gl/GLU.h>
//#include <QtOpenGL\qgl.h>
#endif
#include "CArcCamera.h"

#include "../Definitions.h"
#include "../MessageHelper.h"

#pragma warning(disable:4100)

namespace kk
{
namespace scene
{
	core::vector3df CArcCameraNode::XAxisWorld = vector3df(1,0,0);
	core::vector3df CArcCameraNode::YAxisWorld = vector3df(0,1,0);
	core::vector3df CArcCameraNode::ZAxisWorld = vector3df(0,0,1);

	CArcCameraNode::CArcCameraNode()
		:QuatNow(0,0,0,1),QuatDown(0,0,0,1),ScreenCenter(VIEWPORT_WIDTH/2.0f,VIEWPORT_HEIGHT/2.0f),m_bCamMoving(false),m_bDragging(false)
		//fRadius(0.9f)
	{
		target = vector3df(0,0,0);
		pos = initPosition=target + core::vector3df(0,0,1);
		up = vector3df(0,1,0);
		//1x1x1 cube
		//f32 radius = 1.0f*sqrtf(3);
		OffsetToCenter = 1.0f;//radius / tan(DEG2RAD(45)/2);
		fRadius = 0.5f*sqrtf((float)(VIEWPORT_WIDTH*VIEWPORT_WIDTH+VIEWPORT_HEIGHT*VIEWPORT_HEIGHT));

	}

	CArcCameraNode::~CArcCameraNode()
	{
	}

	void CArcCameraNode::Default()
	{
		
		target = vector3df(0,0,0);
		pos = initPosition=target + core::vector3df(0,0,1);
		up = vector3df(0,1,0);
		//1x1x1 cube
		//f32 radius = 1.0f*sqrtf(3);
		OffsetToCenter = 1.0f;//radius / tan(DEG2RAD(45)/2);
		fRadius = 0.5f*sqrtf((float)(VIEWPORT_WIDTH*VIEWPORT_WIDTH+VIEWPORT_HEIGHT*VIEWPORT_HEIGHT));
		OffsetToCenter = (target-pos).getLength();
	}

	void CArcCameraNode::OnMouseDown(int type,int x,int y)
	{
		switch(type)
		{
		case 0://left
			HandleMouseLeftDownEvent(x,y);
			break;
		case 1://middle

			break;
		case 2://right
			HandleMouseRightDownEvent(x,y);
			break;
		}
	}

	void CArcCameraNode::OnMouseRelease(int type,int x,int y)
	{
		switch(type)
		{
		case 0://left
			m_bDragging = false;
			break;
		case 1://middle

			break;
		case 2://right
			m_bCamMoving = false;
			break;
		}

	}

	core::vector2df CArcCameraNode::getScreenPosFor(const core::vector3df& pos3D)
	{
		#ifndef UseDx
		//opengl
		double screenX,screenY,screenZ;
		double ModelView[16];
		glGetDoublev(GL_MODELVIEW_MATRIX,ModelView);
		double Projection[16];
		glGetDoublev(GL_PROJECTION_MATRIX,Projection);
		int Viewport[4];
		glGetIntegerv(GL_VIEWPORT,Viewport);
		gluProject(pos3D.X,pos3D.Y,pos3D.Z,ModelView,Projection,
			Viewport,&screenX,&screenY,&screenZ);

		return core::vector2df((float)screenX,(float)screenY);
		#endif
	}
	core::vector2df CArcCameraNode::convertScreenPosToMinusOne2One(const core::vector2df& pos)
	{
		core::vector2df result;
		int Viewport[4];
		glGetIntegerv(GL_VIEWPORT,Viewport);
		result.X = ((pos.X / (float)Viewport[2])-0.5f)*2.0f;
		result.Y = ((pos.Y / (float)Viewport[3]) -0.5f)*2.0f;

		//result.X = pos.X;
		//result.Y = pos.Y;
		return result;
	}
	void CArcCameraNode::OnMouseMove(int x,int y)
	{
		if(m_bDragging)
		{
			CurrentPos = vector2df((f32)x,(f32)y);
			CurrentPos3D = convertMouseToSphere(CurrentPos);
				//update view matrix
			getRotatonMatrix(DownPos,CurrentPos);
		}
		if(m_bCamMoving)
		{
			static core::vector3df dir;
			static core::vector3df xDir;
			static core::vector3df yDir;

			dir = target - pos;
			dir.normalize();
			xDir = dir.crossProduct(up);xDir.normalize();
			yDir = up;

			CurrentPos = core::vector2df((f32)x,(f32)y);

			//update target location
			float xDelta = -CurrentPos.X + DownPos.X ;
			float yDelta = CurrentPos.Y - DownPos.Y;
			core::vector3df moveDir = xDir * xDelta*0.01f + yDir * yDelta*0.01f;
			target += moveDir;
			pos += moveDir;

			DownPos = CurrentPos;
		}
	}
	void CArcCameraNode::OnSize()
	{
		f32 squaredMagnitude = (f32)((VIEWPORT_WIDTH*VIEWPORT_WIDTH)+(VIEWPORT_HEIGHT*VIEWPORT_HEIGHT)); 
		fRadius = 0.5f*sqrtf(squaredMagnitude);
		ScreenCenter = vector2df(VIEWPORT_WIDTH/2.0f,VIEWPORT_HEIGHT/2.0f);
	}
	void CArcCameraNode::HandleMouseLeftDownEvent(int x,int y)
	{
		m_bDragging = true;
		
		QuatDown = QuatNow;
		DownPos = core::vector2df((f32)x,(f32)y);
		DownPos3D = convertMouseToSphere(DownPos);

		//
		_up = up;
		_dir = pos - target;
		_dir.normalize();
		
	}

	void CArcCameraNode::HandleMouseWheelChange(int x,int y,float delta)
	{
		//float step = 0.1f*(0.5f*sqrtf((Driver->Width*Driver->Width)+(Driver->Height*Driver->Height)));
		OffsetToCenter *= (delta < 0? 1.2f:0.8f);
		if(OffsetToCenter <0.2f)
			OffsetToCenter = 0.2f;
		pos = target + QuatNow * initPosition * OffsetToCenter;
		

		setUpMatrices();
	}
	
	void CArcCameraNode::HandleMouseRightDownEvent(int x,int y)
	{
		
		m_bCamMoving = true;
		DownPos = core::vector2df((f32)x,(f32)y);
			//get x y plane
	}
	matrix4 CArcCameraNode::getRotatonMatrix(vector2df posDown,vector2df posNow) 
	{
		QuatNow = getQuaternion(DownPos3D,CurrentPos3D) * QuatDown;

		up = QuatNow * vector3df(0,1,0);//_up;
		pos = target + QuatNow * initPosition * OffsetToCenter;

		return QuatNow.getMatrix();
	}

	//convert 2d pos to 3d unit sphere coordinate
	vector3df CArcCameraNode::convertMouseToSphere(vector2df vec2Mouse)
	{
		vec2Mouse.X = VIEWPORT_WIDTH - vec2Mouse.X;

		vector2df vec2UnitMouse = (vec2Mouse - ScreenCenter) / fRadius;
		
		double dDragRadius = vec2UnitMouse.dotProduct(vec2UnitMouse);

		vector3df vec3BallMouse(vec2UnitMouse.X, vec2UnitMouse.Y, 0.0);

		if (dDragRadius > 1.0) {
			// the mouse position was outside the sphere
			// -> map the mouse position to the circle
			vec3BallMouse /= (f32)sqrt(dDragRadius);
			vec3BallMouse.Z = 0.0f;
		} else {
			// compute the z-value of the unit sphere
			vec3BallMouse.Z = (f32)sqrt(1.0 - dDragRadius);
		}
		//vec3BallMouse.normalize();
		return (vec3BallMouse);
	}

	quaternion CArcCameraNode::getQuaternion(vector3df vecFrom, vector3df vecTo)
	{
		float dot = vecFrom.dotProduct(vecTo);
		core::vector3df qPart = vecFrom.crossProduct(vecTo);
		return quaternion(qPart.X,qPart.Y,qPart.Z,dot);
		//return quaternion(
		//	vecFrom.Z * vecTo.Z - vecFrom.Y*vecTo.Y,
		//	vecFrom.Z * vecTo.X - vecFrom.X*vecTo.Z,
		//	vecFrom.X * vecTo.Y - vecFrom.Y*vecTo.X,
		//	vecFrom.X * vecTo.X + vecFrom.Y*vecTo.Y + vecFrom.Z*vecTo.Z);
	}


	void CArcCameraNode::SetCameraToAxis(int type)
	{
		target = core::vector3df(0.5f,0.5f,0.5f);
		switch(type)
		{
		case 0:
			pos = target - 2.0f*XAxisWorld;
			up = YAxisWorld;
			//反转
			XAxisWorld *= -1.0f;
			break;
		case 1:
			pos = target - 2.0f*YAxisWorld;
			up = ZAxisWorld;
			//反转
			YAxisWorld *= -1.0f;
			break;
		case 2:
			pos = target - 2.0f*ZAxisWorld;
			up = XAxisWorld;
			//反转
			ZAxisWorld *= -1.0f;
			break;
		default:
			ShowMessage("不支持的坐标类型");
			return;
		}
		QuatNow = core::quaternion(0,0,0,1);
		QuatDown = core::quaternion(0,0,0,1);
		//f32 radius = 1.0f*sqrtf(3);
		//OffsetToCenter = 1.0f;//radius / tan(DEG2RAD(45)/2);
		//fRadius = 0.5f*sqrtf((float)(VIEWPORT_WIDTH*VIEWPORT_WIDTH+VIEWPORT_HEIGHT*VIEWPORT_HEIGHT));
		//OffsetToCenter = (target-pos).getLength();

		setUpMatrices();
	}
	void CArcCameraNode::setUpMatrices()
	{
		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();
		//gluPerspective(45,VIEWPORT_WIDTH/(double)VIEWPORT_HEIGHT,0.1,1000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(pos.X,pos.Y,pos.Z,target.X,target.Y,target.Z,
			up.X,up.Y,up.Z);
		//gluLookAt(3,3,3,0,0,0,0,1,0);

	}

	// view , projection and world
//	void CArcCameraNode::setUpMatrices()
//	{
//#ifndef UseDX
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		gluPerspective(45,Driver->getViewport().X/(double)Driver->getViewport().Y,0.1f,1000.0f);
//		
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		glTranslatef(0,0,-OffsetToCenter);
//		glMultMatrixf(&RotationMatrixInverse[0]);
//		glTranslatef(0,0,1);
//#endif
//	}
}//end scene
}//end kk