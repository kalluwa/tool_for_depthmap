//2015/11/12

#ifndef _I_DISPOSE_H_
#define _I_DISPOSE_H_

//#include <iostream>
//#include <string>

#include "MessageHelper.h"

//�������г�Ա��base�ӿ�
class IBase
{
public:
	//�ṹ��
	IBase():m_szName(""),ReferenceCount(1){}
	IBase(std::string name):m_szName(name),ReferenceCount(1){}

	//��������
	virtual ~IBase()
	{
	}

	//����
	std::string GetName() const
	{
		return m_szName;
	}
	//���ô�������
	void Grab(){ReferenceCount++;}
	//�������ô���
	bool Drop()
	{
		ReferenceCount--;
		if(ReferenceCount == 0)
		{
			delete this;
			return true;
		}

		return false;
	}
protected:
	//name of this node
	std::string m_szName;
	int ReferenceCount;
};

#endif