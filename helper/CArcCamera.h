#ifndef _C_ARC_CAMERA_H_
#define _C_ARC_CAMERA_H_

#include "../Core/core.h"

using namespace kk::core;
namespace kk
{
namespace scene
{
	class CArcCameraNode
	{
	public:
		CArcCameraNode();

		virtual ~CArcCameraNode();
		//type:0-left,1-middle,2-right
		void OnSize();
		void OnMouseDown(int type,int x,int y);
		void OnMouseMove(int x,int y);
		void OnMouseRelease(int type,int x,int y);
		void Default();

		virtual void HandleMouseLeftDownEvent(int x,int y);
		virtual void HandleMouseWheelChange(int x,int y,float delta);
		virtual void HandleMouseRightDownEvent(int x,int y);

		//math functions
		matrix4 getRotatonMatrix(vector2df posDown,vector2df posNow);
		quaternion getQuaternion(vector3df vecFrom, vector3df vecTo);
		vector3df convertMouseToSphere(vector2df vec2Mouse);
		//set up view and projection and world
		virtual void setUpMatrices();
		//set camera to 3 axis
		void SetCameraToAxis(int type);
		//Getter
		core::vector3df getPosition(){ return pos;};
		core::vector3df getTarget(){ return target;}
		core::vector3df getUp(){ return up;}
		core::vector2df getScreenPosFor(const core::vector3df& pos3D);
		core::vector2df convertScreenPosToMinusOne2One(const core::vector2df& pos);

		static vector3df XAxisWorld,YAxisWorld,ZAxisWorld;
	private:
		vector2df ScreenCenter;
		float OffsetToCenter;
		matrix4 RotationMatrixInverse;


		quaternion QuatDown,QuatNow;
		vector2df DownPos,CurrentPos;
		core::vector3df DownPos3D,CurrentPos3D;
		float fRadius;

		core::vector3df _up,_dir;
		//base
		kk::core::vector3df initPosition;
		kk::core::vector3df pos;
		kk::core::vector3df target;
		kk::core::vector3df up;
		//we need driver to update our view matrix
		//kk::video::IVideoDriver* driver;
		//store some data for tmp use
		int lastX,lastY;
		bool isFirstUpdate;
		bool mouseLeftDown,mouseRightDown;
		bool m_bCamMoving,m_bDragging;

	};//end of class 

}//end scene
}//end kk
#endif