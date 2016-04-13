#ifndef _C_CPU_DEPTH_PROCESSOR_H_
#define _C_CPU_DEPTH_PROCESSOR_H_

#include "IDataProcessor.h"

#include "ImageHelper.h"

class CCpuDepthProcessor : public virtual IDataProcessor
{
public:
	CCpuDepthProcessor()
		:IDataProcessor()
	{
		m_szName = "CpuDepthProcessor";
	}

	void Process();
};
#endif