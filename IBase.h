//2015/11/12

#ifndef _I_DISPOSE_H_
#define _I_DISPOSE_H_

//#include <iostream>
//#include <string>

#include "MessageHelper.h"

//定义所有成员的base接口
class IBase
{
public:
	//结构体
	IBase():m_szName(""),ReferenceCount(1){}
	IBase(std::string name):m_szName(name),ReferenceCount(1){}

	//析构函数
	virtual ~IBase()
	{
	}

	//属性
	std::string GetName() const
	{
		return m_szName;
	}
	//引用次数增加
	void Grab(){ReferenceCount++;}
	//减少引用次数
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