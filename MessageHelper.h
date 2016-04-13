//2015/11/12

#ifndef _MESSAGE_HELPER_H_
#define _MESSAGE_HELPER_H_

#include<iostream>
#include <string>

#ifdef QT_GUI_LIB

//#include <QtGui>
#include <QtWidgets/qmessagebox.h>
#else
//others:TODO
#endif

//δ���ú�������
#pragma warning(disable:4100)

//��ʾ����
inline void ShowMessage(std::string message)
{
#ifdef QT_GUI_LIB
	QMessageBox::information(NULL,QString::fromLocal8Bit("����"),QString::fromLocal8Bit(message.c_str()),
		QMessageBox::Yes);
#else
	//TODO:

#endif
};


inline void ShowMessage(const char* pStr, ...)
{
    char msg[1024] = {0};

    va_list a;
    va_start(a, pStr);

    _vsnprintf_s(msg, _countof(msg), _TRUNCATE, pStr, a);

#ifdef QT_GUI_LIB
	QMessageBox::information(NULL,QString::fromLocal8Bit("����"),QString::fromLocal8Bit(msg),
		QMessageBox::Yes);
#else
	//TODO:

#endif
};


#endif