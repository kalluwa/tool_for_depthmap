#ifndef _IMAGE_HELPER_H_
#define _IMAGE_HELPER_H_

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QtWidgets>
#include "helper\PictureBox.h"

inline QImage CreateImageFrom(unsigned char* rawData,int width,int height)
{
	QImage img(rawData,width,height,QImage::Format_Indexed8);

	return img;
};

inline void ShowImage(const QImage& img,const std::string& nameOfWindow)
{
	QDialog* dig = new QDialog;
	QGridLayout* centralLayout = new QGridLayout;
	PictureBox* imageBox = new PictureBox();
	
	//QImage imageSample(QString("C:\\Chrysanthemum.jpg"));
	imageBox->setImage(img);
    centralLayout->addWidget(imageBox, 0, 0);
	dig->setLayout(centralLayout);
	dig->setMinimumSize(300,300);
	dig->setWindowTitle(nameOfWindow.c_str());
	dig->show();
};
#endif
