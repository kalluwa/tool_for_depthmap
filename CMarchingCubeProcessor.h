//2015/11/13

#ifndef _C_MARCHING_CUBE_PROCESSOR_H_
#define _C_MARCHING_CUBE_PROCESSOR_H_

#include "IDataProcessor.h"

class CMarchingCubeProcessor : public virtual IDataProcessor
{
public:

	CMarchingCubeProcessor()
		:IDataProcessor()
	{
		m_szName = "MarchingCubeProcessor";
		m_pMarchingCubeArray=0;
	}
	~CMarchingCubeProcessor()
	{
		if(m_pMarchingCubeArray)
			m_pMarchingCubeArray->Drop();
	}
	void Process();

	DataArray* GetProcessDataArray(){ return m_pMarchingCubeArray;}
private:
	DataArray* m_pMarchingCubeArray;
};
#endif
