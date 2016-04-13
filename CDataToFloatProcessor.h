//2015/11/13

#ifndef _C_DATA_TO_FLOAT_PROCESSOR_H_
#define _C_DATA_TO_FLOAT_PROCESSOR_H_

#include "IDataProcessor.h"

class CDataToFloatProcessor : public virtual IDataProcessor
{
public:

	CDataToFloatProcessor()
		:IDataProcessor()
	{
		m_szName = "DataToFloatProcessor";
	}

	void Process()
	{
		//ConvertDataToFloat();
	};
};

#endif