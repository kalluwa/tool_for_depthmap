//2015/11/12

#ifndef _I_DATA_PROCESSOR_H_
#define _I_DATA_PROCESSOR_H_

#include "IBase.h"

#include "FloatDataArray.h"

extern std::string g_filePath;
//数据处理器
//只能改变数据值的大小
class IDataProcessor : public virtual IBase
{
public:
	IDataProcessor():IBase("IDataProcessor"){ DefaultInitialization(); }

	IDataProcessor(DataArray* rawData):IBase("IDataProcessor")
	{
		DefaultInitialization();
		SetData(rawData);
	}

	~IDataProcessor()
	{
		if(m_fData)
			m_fData->Drop();
		if(m_originalData)
			m_originalData->Drop();
	}

	//只使用一次,其它用AttachArray
	void SetData(DataArray* rawData)
	{
		if(m_originalData && m_originalData != rawData)
			m_originalData->Drop();

		m_originalData = rawData;
		m_originalData->Grab();
		m_iSizeOfVoxel = rawData->GetVoxelStride();
		m_iSizeX = rawData->GetSizeX();
		m_iSizeY = rawData->GetSizeY();
		m_iSizeZ = rawData->GetSizeZ();

		ConvertDataToFloat();
	}

	void ConvertDataToFloat()
	{
		//if(m_fData)
		{
			//ShowMessage("m_fData已存储其它数据");
			//return;
			//m_fData->Drop();
		}

		float* fData = 0;
		switch(m_iSizeOfVoxel)
		{
		case 1:
			fData = new float[m_iSizeX*m_iSizeY*m_iSizeZ];
			for(int i=0;i<m_iSizeX*m_iSizeY*m_iSizeZ;i++)
				fData[i] = m_originalData->GetRawData()[i] / 255.0f;
			break;
		case 2:
			fData = new float[m_iSizeX*m_iSizeY*m_iSizeZ];
			for(int i=0;i<m_iSizeX*m_iSizeY*m_iSizeZ;i++)
				fData[i] = ((unsigned short*)m_originalData->GetRawData())[i];
			break;
		case 4:
			fData = new float[m_iSizeX*m_iSizeY*m_iSizeZ];
			//for(int i=0;i<m_iSizeX*m_iSizeY*m_iSizeZ;i++)
			//	fData[i] = ((float*)m_originalData->GetRawData())[i];
			memcpy(fData,m_originalData->GetRawData(),m_iSizeX*m_iSizeY*m_iSizeZ*sizeof(float));
			break;
		default:
			::ShowMessage("不支持的voxelSize");
			return;
		}

		//referenceCount = 1
		m_fData = new DataArray(fData,sizeof(float),m_iSizeX,m_iSizeY,m_iSizeZ);
	}

	void AttachArray(DataArray* rawData,DataArray* dataArrayToBeAttached)
	{
		if(dataArrayToBeAttached->GetVoxelStride()!=sizeof(float))
		{
			ShowMessage("数据格式大小不一致，是否是数据设置错误?");
			return;
		}

		if(m_fData && m_fData!= dataArrayToBeAttached)
			m_fData->Drop();
		m_fData = dataArrayToBeAttached;

		if(m_originalData && m_originalData != rawData)
			m_originalData->Drop();
		m_originalData = rawData;

		//add reference count
		m_fData->Grab();
		m_originalData->Grab();
	}
	//在这里处理数据
	virtual void Process()=0;

	void SetnMaxSize(float size=50)
	{
		nMaxSize = size;
		float space = 3.333f/1.20f;
		if(m_fData)
		{
			space = m_fData->GetScaleZ()/m_fData->GetScaleX();
			fScale = std::min( (float)nMaxSize/( (float)m_fData->GetSizeZ()*space ),
			std::min((float)nMaxSize/(float)m_fData->GetSizeX(),(float)nMaxSize/(float)m_fData->GetSizeY()));
		}
		else
		{
			fScale = 1.0f;//invalid value
		}
	}
#if 1 //属性
	DataArray* GetOriginalData(){ return m_originalData;}
	float GetValueAtOriginalData(int x,int y,int z)
	{
		return m_originalData->GetValueAt(x,y,z);
	}
	float* GetRawDataFloat(){ return (float*)m_fData->GetRawData();}
	virtual DataArray* GetProcessDataArray(){ return m_fData;}

	int GetVoxelSize(){return m_iSizeOfVoxel;}
#endif

protected:
	DataArray* m_originalData;
	int m_iSizeOfVoxel;
	int m_iSizeX,m_iSizeY,m_iSizeZ;

	bool m_bProcessed;
	DataArray* m_fData;

	float nMaxSize;
	float fScale;

	void DefaultInitialization()
	{
		m_originalData = 0;
		m_iSizeOfVoxel = 0;
		m_fData=0;
		fScale = 1.0f;
		m_bProcessed = false;

		SetnMaxSize();
	}
};
#endif