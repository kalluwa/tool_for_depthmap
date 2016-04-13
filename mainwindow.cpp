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
#include "mainwindow.h"

#include <QtWidgets/QtWidgets>
#include "helper/PictureBox.h"
#include "ImageHelper.h"

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);
	
	Application = new ApplicationCenter();

    glWidget = new GLWidget(Application);
    pixmapLabel = new QLabel;

    glWidgetArea = new QScrollArea;
    glWidgetArea->setWidget(glWidget);
    glWidgetArea->setWidgetResizable(true);
    glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glWidgetArea->setMinimumSize(200, 200);
	
    pixmapLabelArea = new QScrollArea;
    pixmapLabelArea->setWidget(pixmapLabel);
    pixmapLabelArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    pixmapLabelArea->setMinimumSize(20, 20);
    //xSlider = createSlider(SIGNAL(xRotationChanged(int)),
    //                       SLOT(setXRotation(int)));
    //ySlider = createSlider(SIGNAL(yRotationChanged(int)),
    //                       SLOT(setYRotation(int)));
    //zSlider = createSlider(SIGNAL(zRotationChanged(int)),
    //                       SLOT(setZRotation(int)));

    createActions();
    createMenus();

	
	QGridLayout *centralLayout = new QGridLayout;
#if 0
    centralLayout->addWidget(glWidgetArea, 0, 0);
    centralLayout->addWidget(pixmapLabelArea, 0, 1);
	//centralLayout->addWidget(xSlider, 1, 0, 1, 2);
    //centralLayout->addWidget(ySlider, 2, 0, 1, 2);
    //centralLayout->addWidget(zSlider, 3, 0, 1, 2);
    
#else
	QSplitter* splitter= new QSplitter(Qt::Horizontal);
	splitter->addWidget(glWidgetArea);
	splitter->addWidget(pixmapLabelArea);

	QList<int> sizes;
	sizes<<500<<200;
	splitter->setSizes(sizes);
	centralLayout->addWidget(splitter);
	//setCentralWidget(splitter);
#endif
    
	centralWidget->setLayout(centralLayout);
    //xSlider->setValue(15 * 16);
    //ySlider->setValue(345 * 16);
    //zSlider->setValue(0 * 16);

    setWindowTitle(tr("tool_for_depthmap"));
    resize(800, 600);

	//初始化
	Application->Initialize();
	//读取默认文件
	//Application->LoadContent();
	
}

void MainWindow::renderIntoPixmap()
{
    QSize size = getSize();
    if (size.isValid()) {
        QPixmap pixmap = glWidget->renderPixmap(size.width(), size.height());
        setPixmap(pixmap);
    }
}

void MainWindow::grabFrameBuffer()
{
    QImage image = glWidget->grabFrameBuffer();
    setPixmap(QPixmap::fromImage(image));
}

void MainWindow::changeMaxSize()
{
	bool ok = false;
	int nMaxSize = QInputDialog::getInt(this,QString::fromLocal8Bit("nMaxSize"),
		QString::fromLocal8Bit("10~200"),
		50,10,200,1,&ok);

	if(!ok)
		return;
	if(nMaxSize >= 10 && nMaxSize <=200)
	{
		Application->LoadContent(g_filePath.c_str(),nMaxSize);
		glWidget->updateGL();
	}
	else
	{
		ShowMessage("设置无效,有效范围为(10~200)");
	}
}
void MainWindow::clearPixmap()
{
    setPixmap(QPixmap());
}

void MainWindow::about()
{
	QMessageBox::about(this, QString::fromLocal8Bit("关于"),
            tr("The <b>Grabber</b> example demonstrates two approaches for "
               "rendering OpenGL into a Qt pixmap."));
}

//显示事件
void MainWindow::OnShowPointCloud()
{
	m_aSaveMesh->setEnabled(false);
	glWidget->ChangeRenderingMode(EnumSpace::Point);
};
void MainWindow::OnShowMarchingCube()
{
	m_aSaveMesh->setEnabled(true);
	glWidget->ChangeRenderingMode(EnumSpace::MarchingCube);
};
void MainWindow::OnShowWireframe()
{
	m_aSaveMesh->setEnabled(true);
	glWidget->ChangeRenderingMode(EnumSpace::Wireframe);
}
void MainWindow::OnShowMeshPoint()
{
	m_aSaveMesh->setEnabled(true);
	glWidget->ChangeRenderingMode(EnumSpace::MeshPoint);
}
void MainWindow::OnShowVolumeSolid()
{
	m_aSaveMesh->setEnabled(false);
	glWidget->ChangeRenderingMode(EnumSpace::Volume);
}
void MainWindow::OnShowVolumeTransparent()
{
	m_aSaveMesh->setEnabled(false);
	glWidget->ChangeRenderingMode(EnumSpace::VolumeTransparent);
}
//生成事件
void MainWindow::OnGenerateCpuDepth()
{
	glWidget->ChangeRenderingMode(EnumSpace::CpuDepth);
}

void MainWindow::OnSaveMesh()
{
	QString path = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存文件"),".",QString("stl(*.stl)"));

	if(path.length() > 0)
		glWidget->OnSaveMesh(path.toStdString().c_str());
}

void MainWindow::OnBindCameraToAxis(int type)
{
	//ShowMessage("%d",type);
	glWidget->GetCamera()->SetCameraToAxis(type);

	glWidget->updateGL();
}



void MainWindow::createActions()
{
    renderIntoPixmapAct = new QAction(tr("&Render into Pixmap..."), this);
    renderIntoPixmapAct->setShortcut(tr("Ctrl+R"));
    connect(renderIntoPixmapAct, SIGNAL(triggered()),
            this, SLOT(renderIntoPixmap()));

    grabFrameBufferAct = new QAction(tr("&Grab Frame Buffer"), this);
    grabFrameBufferAct->setShortcut(tr("Ctrl+G"));
    connect(grabFrameBufferAct, SIGNAL(triggered()),
            this, SLOT(grabFrameBuffer()));

    clearPixmapAct = new QAction(tr("&Clear Pixmap"), this);
    clearPixmapAct->setShortcut(tr("Ctrl+L"));
    connect(clearPixmapAct, SIGNAL(triggered()), this, SLOT(clearPixmap()));

	changeNMaxSize =  new QAction(QString::fromLocal8Bit("设置n&MaxSize"),this);
	changeNMaxSize->setShortcut(tr("Ctrl+M"));
	connect(changeNMaxSize, SIGNAL(triggered()), this, SLOT(changeMaxSize()));
	
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	//创建事件响应 
	m_aShowPoint = new QAction(QString::fromLocal8Bit("点云"),this);
	connect(m_aShowPoint,SIGNAL(triggered()),this,SLOT(OnShowPointCloud()));
	m_aShowMarchingCubeMesh = new QAction(QString::fromLocal8Bit("面片"),this);
	connect(m_aShowMarchingCubeMesh,SIGNAL(triggered()),this,SLOT(OnShowMarchingCube()));
	m_aShowMarchingCubeMeshWireframe = new QAction(QString::fromLocal8Bit("面片网格"),this);
	connect(m_aShowMarchingCubeMeshWireframe,SIGNAL(triggered()),this,SLOT(OnShowWireframe()));
	m_aShowMarchingCubeMeshPoint = new QAction(QString::fromLocal8Bit("面片点"),this);
	connect(m_aShowMarchingCubeMeshPoint,SIGNAL(triggered()),this,SLOT(OnShowMeshPoint()));
	m_aShowVolumeRendering = new QAction(QString::fromLocal8Bit("体渲染"),this);
	connect(m_aShowVolumeRendering,SIGNAL(triggered()),this,SLOT(OnShowVolumeSolid()));
	m_aShowVolumeRenderingTransparent = new QAction(QString::fromLocal8Bit("体渲染(透明)"),this);
	connect(m_aShowVolumeRenderingTransparent,SIGNAL(triggered()),this,SLOT(OnShowVolumeTransparent()));
	m_aGenerateCpuDepthBuffer = new QAction(QString::fromLocal8Bit("CPU深度图"),this);
	connect(m_aGenerateCpuDepthBuffer,SIGNAL(triggered()),this,SLOT(OnGenerateCpuDepth()));
	m_aSaveMesh = new QAction(QString::fromLocal8Bit("保存网格"),this);
	m_aSaveMesh->setEnabled(false);
	connect(m_aSaveMesh,SIGNAL(triggered()),this,SLOT(OnSaveMesh()));
	
	QSignalMapper* camSingalMapper = new QSignalMapper();
	m_aSetCameraToAxisTypeOne = new QAction(QString::fromLocal8Bit("方向一"),this);
	//m_aSetCameraToAxisTypeOne->setEnabled(false);
	m_aSetCameraToAxisTypeTwo = new QAction(QString::fromLocal8Bit("方向二"),this);
	//m_aSetCameraToAxisTypeTwo->setEnabled(false);
	m_aSetCameraToAxisTypeThree = new QAction(QString::fromLocal8Bit("方向三"),this);
	//m_aSetCameraToAxisTypeThree->setEnabled(false);
	connect(m_aSetCameraToAxisTypeOne,SIGNAL(triggered()),camSingalMapper,SLOT(map()));
	connect(m_aSetCameraToAxisTypeTwo,SIGNAL(triggered()),camSingalMapper,SLOT(map()));
	connect(m_aSetCameraToAxisTypeThree,SIGNAL(triggered()),camSingalMapper,SLOT(map()));

	camSingalMapper->setMapping(m_aSetCameraToAxisTypeOne,0);
	camSingalMapper->setMapping(m_aSetCameraToAxisTypeTwo,1);
	camSingalMapper->setMapping(m_aSetCameraToAxisTypeThree,2);
	connect(camSingalMapper,SIGNAL(mapped(int)),this,SLOT(OnBindCameraToAxis(int)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(renderIntoPixmapAct);
    fileMenu->addAction(grabFrameBufferAct);
    fileMenu->addAction(clearPixmapAct);
	fileMenu->addAction(changeNMaxSize);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

	//create menu bar
	QToolBar* toolBar = addToolBar(tr("&File"));
	toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_aShowPoint->setIcon(QIcon("ui_data/pointCloud.ico"));
	toolBar->addAction(m_aShowPoint);
	m_aShowMarchingCubeMesh->setIcon(QIcon("ui_data/MarchingCube.ico"));
	toolBar->addAction(m_aShowMarchingCubeMesh);
	m_aShowMarchingCubeMeshWireframe->setIcon(QIcon("ui_data/wireframe.ico"));
	toolBar->addAction(m_aShowMarchingCubeMeshWireframe);
	m_aShowMarchingCubeMeshPoint->setIcon(QIcon("ui_data/pointCloud.ico"));
	toolBar->addAction(m_aShowMarchingCubeMeshPoint);
	m_aShowVolumeRendering->setIcon(QIcon("ui_data/volume.ico"));
	toolBar->addAction(m_aShowVolumeRendering);
	m_aShowVolumeRenderingTransparent->setIcon(QIcon("ui_data/volume.ico"));
	toolBar->addAction(m_aShowVolumeRenderingTransparent);

	toolBar->addSeparator();
	m_aGenerateCpuDepthBuffer->setIcon(QIcon("ui_data/distance.ico"));
	toolBar->addAction(m_aGenerateCpuDepthBuffer);
	m_aSaveMesh->setIcon(QIcon("ui_data/save.ico"));
	toolBar->addAction(m_aSaveMesh);

	//camera
	m_aSetCameraToAxisTypeOne->setIcon(QIcon("ui_data/axisx.ico"));
	toolBar->addAction(m_aSetCameraToAxisTypeOne);
	m_aSetCameraToAxisTypeTwo->setIcon(QIcon("ui_data/axisy.ico"));
	toolBar->addAction(m_aSetCameraToAxisTypeTwo);
	m_aSetCameraToAxisTypeThree->setIcon(QIcon("ui_data/axisz.ico"));
	toolBar->addAction(m_aSetCameraToAxisTypeThree);

}

QSlider *MainWindow::createSlider(const char *changedSignal,
                                  const char *setterSlot)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    connect(slider, SIGNAL(valueChanged(int)), glWidget, setterSlot);
    connect(glWidget, changedSignal, slider, SLOT(setValue(int)));
    return slider;
}

void MainWindow::setPixmap(const QPixmap &pixmap)
{
    pixmapLabel->setPixmap(pixmap);
    QSize size = pixmap.size() / pixmap.devicePixelRatio();
    if (size - QSize(1, 0) == pixmapLabelArea->maximumViewportSize())
        size -= QSize(1, 0);
    pixmapLabel->resize(size);
}

QSize MainWindow::getSize()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Grabber"),
                                         tr("Enter pixmap size:"),
                                         QLineEdit::Normal,
                                         tr("%1 x %2").arg(glWidget->width())
                                                      .arg(glWidget->height()),
                                         &ok);
    if (!ok)
        return QSize();

    QRegExp regExp(tr("([0-9]+) *x *([0-9]+)"));
    if (regExp.exactMatch(text)) {
        int width = regExp.cap(1).toInt();
        int height = regExp.cap(2).toInt();
        if (width > 0 && width < 2048 && height > 0 && height < 2048)
            return QSize(width, height);
    }

    return glWidget->size();
}


