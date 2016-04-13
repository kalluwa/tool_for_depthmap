/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "glwidget.h"

#include <QtGui/QMouseEvent>
#include <QtCore/QTimer>
#include <QtCore/qmimedata.h>
#include "Definitions.h"

#include "CMarchingCubeRenderer.h"
#include <math.h>


GLWidget::GLWidget(ApplicationCenter* app,QWidget *parent)
    : QGLWidget(parent)
{
	//init
	m_application = app;
	cam =  new kk::scene::CArcCameraNode();

	app->SetCamera(cam);
    gear1 = 0;
    gear2 = 0;
    gear3 = 0;
    xRot = 0;
    yRot = 0;
    zRot = 0;
    gear1Rot = 0;

	m_bEnable = false;
	m_bKeyRightClick = false;
    //QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(advanceGears()));
    //timer->start(20);

	//设置焦点
	setFocusPolicy(Qt::StrongFocus);
	//enable drop event
	setAcceptDrops(true);
	
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(gear1, 1);
    glDeleteLists(gear2, 1);
    glDeleteLists(gear3, 1);

	if(cam)
		delete cam;

}

void GLWidget::setXRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::DefaultState()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPointSize(2.0f);
	//cam->Default();
}

void GLWidget::ChangeRenderingMode(EnumSpace::RenderingMode mode)
{
	if(!Enabled())
		return;

	DefaultState();
	switch(mode)
	{
	case EnumSpace::Point:
		m_application->SetRendererAndProcessor(0);//和ApplicationCenter中的排列对应
		break;
	case EnumSpace::MarchingCube:
		m_application->SetRendererAndProcessor(1);
		break;
	case EnumSpace::Wireframe:
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		if(g_renderingMode != EnumSpace::MeshPoint&&
			g_renderingMode != EnumSpace::Wireframe&&
			g_renderingMode != EnumSpace::MarchingCube)
			m_application->SetRendererAndProcessor(1);
		break;
	case EnumSpace::MeshPoint:
		glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
		if(g_renderingMode != EnumSpace::Wireframe&&
			g_renderingMode != EnumSpace::MarchingCube)
			m_application->SetRendererAndProcessor(1);
		break;
	case EnumSpace::Volume:
		m_application->SetRendererAndProcessor(2);
		break;
	case EnumSpace::VolumeTransparent:
		m_application->SetRendererAndProcessor(3);
		break;
	//其它事件
	case EnumSpace::CpuDepth:
		m_application->SetRendererAndProcessor(4);
		break;
	default:
		ShowMessage("不支持的显示模式");
		return;
	}

	g_renderingMode = mode;
	updateGL();
}

void GLWidget::OnSaveMesh(const std::string& filePath)
{
	if(!Enabled())
		return;

	if(g_renderingMode != EnumSpace::MarchingCube &&
		g_renderingMode != EnumSpace::Wireframe)
	{
		ShowMessage("需先切换到到网格/面片显示");
		return;
	}

	auto marchingCubeRenderer = dynamic_cast<CMarchingCubeRenderer*>(m_application->GetCurrentRenderer());

	if(!marchingCubeRenderer)
	{
		ShowMessage("转换渲染器错误");
	}

	marchingCubeRenderer->SaveTriangleMesh(filePath);
}
void GLWidget::dragEnterEvent(QDragEnterEvent* event)
{
	//图片 文件(夹)之类
	if(event->mimeData()->hasUrls())
	{
		event->accept();
	}
	else
		event->ignore();
}
void GLWidget::dropEvent(QDropEvent *event)
{
	if(event->mimeData()->hasUrls())
	{
		//get files' path
		QList<QUrl> filePathList = event->mimeData()->urls();

		QString fileName =filePathList[0].toLocalFile();

		//ShowMessage(fileName.toStdString());

		//reload
		g_filePath = fileName.toStdString();

		m_application->LoadContent(fileName.toStdString());

		m_bEnable = true;
		
		g_renderingMode = EnumSpace::Point;

		updateGL();
	}
}
void GLWidget::initializeGL()
{
    static const GLfloat lightPos[4] = { 5.0f, 5.0f, 10.0f, 1.0f };
    static const GLfloat reflectance1[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
    static const GLfloat reflectance2[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
    static const GLfloat reflectance3[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    gear1 = makeGear(reflectance1, 1.0, 4.0, 1.0, 0.7, 20);
    gear2 = makeGear(reflectance2, 0.5, 2.0, 2.0, 0.7, 10);
    gear3 = makeGear(reflectance3, 1.3, 2.0, 0.5, 0.7, 10);

    glEnable(GL_NORMALIZE);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
	
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//TODO:change the code to move camera
	//glPushMatrix();
 //   glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
 //   glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
 //   glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
	//if(cam)
	cam->setUpMatrices();
    //drawGear(gear1, -3.0, -2.0, 0.0, gear1Rot / 16.0);
    //drawGear(gear2, +3.1, -2.0, 0.0, -2.0 * (gear1Rot / 16.0) - 9.0);

    //glRotated(+90.0, 1.0, 0.0, 0.0);
    //drawGear(gear3, -3.1, -1.8, -2.2, +2.0 * (gear1Rot / 16.0) - 2.0);
	//rendering code
	m_application->Draw();

    //glPopMatrix();
}

void GLWidget::resizeGL(int width, int height)
{
	
    //int side = qMin(width, height);
	
	//VIEWPORT_WIDTH = side;
	//VIEWPORT_HEIGHT = side;
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
	VIEWPORT_WIDTH = width;
	VIEWPORT_HEIGHT = height;
	cam->OnSize();
#if 1
	glViewport(0, 0, width, height);
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-1.0, +1.0, -1.0, 1.0, 1.0, 100.0);
	gluPerspective(45.0, width/(float)height, 0.01f, 500.0f);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glTranslated(0.0, 0.0, -5.0);
	//gluLookAt(m_eye[0],m_eye[1],m_eye[2],m_target[0],m_target[1],m_target[2],0,1,0);
#else
    glViewport((width - side) / 2, (height - side) / 2, side, side);
	
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.0, +1.0, -1.0, 1.0, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(0.0, 0.0, -20.0);
#endif
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
	if(event->button() & Qt::LeftButton)//LeftButton)
		cam->OnMouseDown(0,lastPos.x(),lastPos.y());
	else if(event->button() & Qt::RightButton)
		cam->OnMouseDown(2,lastPos.x(),lastPos.y());
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() & Qt::LeftButton)//LeftButton)
		cam->OnMouseRelease(0,event->pos().x(),event->pos().y());
	else if(event->button() & Qt::RightButton)
		cam->OnMouseRelease(2,lastPos.x(),lastPos.y());
}

void GLWidget::wheelEvent(QWheelEvent* event)
{
	cam->HandleMouseWheelChange(event->x(),event->y(),
		event->delta());
	updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);

		cam->OnMouseMove(event->x(),event->y());

		updateGL();
    } else if (event->buttons() & Qt::RightButton) {
        //setXRotation(xRot + 8 * dy);
        //setZRotation(zRot + 8 * dx);
		cam->OnMouseMove(event->x(),event->y());

		updateGL();
    }
    lastPos = event->pos();
	
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Right)
		m_bKeyRightClick = true;
	else if(event->key() == Qt::Key_Left)
		m_bKeyLeftClick = true;

	if(event->key() == Qt::Key_W)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		updateGL();
	}
	else if(event->key() == Qt::Key_S)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		updateGL();
	}
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	//right
	if(event->key() == Qt::Key_Right)
		if(m_bKeyRightClick == true)
		{
			m_bKeyRightClick = false;
			m_application->SetNextRendererAndProcessor();
			//cam->Default();
			updateGL();
		}
	//left
	if(event->key() == Qt::Key_Left)
		if(m_bKeyLeftClick == true)
		{
			m_bKeyLeftClick = false;
			m_application->SetPreviousRendererAndProcessor();
			//cam->Default();
			updateGL();
		}

}

void GLWidget::advanceGears()
{
    gear1Rot += 2 * 16;
    updateGL();
}

GLuint GLWidget::makeGear(const GLfloat *reflectance, GLdouble innerRadius,
                          GLdouble outerRadius, GLdouble thickness,
                          GLdouble toothSize, GLint toothCount)
{
    const double Pi = 3.14159265358979323846;

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance);

    GLdouble r0 = innerRadius;
    GLdouble r1 = outerRadius - toothSize / 2.0;
    GLdouble r2 = outerRadius + toothSize / 2.0;
    GLdouble delta = (2.0 * Pi / toothCount) / 4.0;
    GLdouble z = thickness / 2.0;

    glShadeModel(GL_FLAT);

    for (int i = 0; i < 2; ++i) {
        GLdouble sign = (i == 0) ? +1.0 : -1.0;

        glNormal3d(0.0, 0.0, sign);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= toothCount; ++j) {
            GLdouble angle = 2.0 * Pi * j / toothCount;
            glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
            glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
            glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
            glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta), sign * z);
        }
        glEnd();

        glBegin(GL_QUADS);
        for (int j = 0; j < toothCount; ++j) {
            GLdouble angle = 2.0 * Pi * j / toothCount;
            glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
            glVertex3d(r2 * cos(angle + delta), r2 * sin(angle + delta), sign * z);
            glVertex3d(r2 * cos(angle + 2 * delta), r2 * sin(angle + 2 * delta), sign * z);
            glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta), sign * z);
        }
        glEnd();
    }

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < toothCount; ++i) {
        for (int j = 0; j < 2; ++j) {
            GLdouble angle = 2.0 * Pi * (i + j / 2.0) / toothCount;
            GLdouble s1 = r1;
            GLdouble s2 = r2;
            if (j == 1)
                qSwap(s1, s2);

            glNormal3d(cos(angle), sin(angle), 0.0);
            glVertex3d(s1 * cos(angle), s1 * sin(angle), +z);
            glVertex3d(s1 * cos(angle), s1 * sin(angle), -z);

            glNormal3d(s2 * sin(angle + delta) - s1 * sin(angle),
                       s1 * cos(angle) - s2 * cos(angle + delta), 0.0);
            glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), +z);
            glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), -z);
        }
    }
    glVertex3d(r1, 0.0, +z);
    glVertex3d(r1, 0.0, -z);
    glEnd();

    glShadeModel(GL_SMOOTH);

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= toothCount; ++i) {
        GLdouble angle = i * 2.0 * Pi / toothCount;
        glNormal3d(-cos(angle), -sin(angle), 0.0);
        glVertex3d(r0 * cos(angle), r0 * sin(angle), +z);
        glVertex3d(r0 * cos(angle), r0 * sin(angle), -z);
    }
    glEnd();

    glEndList();

    return list;
}

void GLWidget::drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
                        GLdouble angle)
{
    glPushMatrix();
    glTranslated(dx, dy, dz);
    glRotated(angle, 0.0, 0.0, 1.0);
    glCallList(gear);
    glPopMatrix();
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}
