//2015/11/12

#ifndef _VOLUME_DATA_
#define _VOLUME_DATA_

#include "IBase.h"
#include "FloatDataArray.h"
#include "IDataProcessor.h"
#include "IDataRenderer.h"
#include "Core\core.h"

#include<vector>
//定义体数据内存结构
template<class T>
class VolumeData : public virtual IBase
{
public:
	//构造函数
	VolumeData():IBase("VolumeData"){ DefaultInitialization(); }

	VolumeData(std::string fileName,int sizeX,int sizeY,int sizeZ)
		:IBase("VolumeData"),
		m_szFileName(fileName),m_bLoadFromFile(true),
		m_iSizeX(sizeX),m_iSizeY(sizeY),m_iSizeZ(sizeZ)
	{
		LoadRawDataFromFile(fileName,sizeX,sizeY,sizeZ);
	}

	~VolumeData()
	{
		if(m_rawData)
		{
			m_rawData->Drop();
		}
		if(m_fileRawData)
			m_fileRawData->Drop();
	}

	void SetNMaxSize(int _maxSize)
	{
		m_nMaxSize = _maxSize;
	}

	void SetSpace(float x,float y,float z)
	{
		m_spacing = core::vector3df(x,y,z);
	}
	//read file[.3ddata]
	bool LoadDataFromFile(const std::string& fileName)
	{
		//检查存在与否
		FILE* filePointer;
		fopen_s(&filePointer,fileName.c_str(),"r");
		if(!filePointer)
		{
			ShowMessage("文件不存在");
			return false;
		}
		m_szFileName = fileName;

		int sizeX,sizeY,sizeZ;
		fread(&sizeX,sizeof(int),1,filePointer);
		fread(&sizeY,sizeof(int),1,filePointer);
		fread(&sizeZ,sizeof(int),1,filePointer);

		//读取文件
		int originalRawDataCount = sizeX * sizeY * sizeZ;

		int sizeOfType = sizeof(T);
		int memoryCountInBytes = originalRawDataCount * sizeOfType;

		T* rawDataTmp = new T[originalRawDataCount];
		if(memoryCountInBytes != fread(rawDataTmp,sizeOfType,originalRawDataCount,filePointer))
		{
			ShowMessage("数据读取不正确");
			delete []rawDataTmp;

			return false;
		}
		
		m_fileRawData = new DataArray(rawDataTmp,sizeof(T),
			sizeX,//new size
			sizeY,
			sizeZ);
		//m_nMaxSize=50;
		int oldSizeX = sizeX;
		int oldSizeY = sizeY;
		int oldSizeZ = sizeZ;
		m_rawDataSize.X = sizeX;
		m_rawDataSize.Y = sizeY;
		m_rawDataSize.Z = sizeZ;
#if 1
		

		int tmpX = sizeX,tmpY = sizeY,tmpZ = (int)((float)sizeZ * m_spacing.Z/m_spacing.X);
		float ratio = ((float)m_nMaxSize)/std::max(tmpX,std::max(tmpY,tmpZ));
		sizeX = (int)(ratio * sizeX);
		sizeY = (int)(ratio * sizeY);
		sizeZ = (int)(ratio * sizeZ);
		float ratioX = oldSizeX / (float)sizeX;
		float ratioY = oldSizeY / (float)sizeY;
		float ratioZ = oldSizeZ / (float)sizeZ;

		int voxelCount = (sizeX+2*m_iPadSize)*(m_iPadSize*2+sizeY)*(m_iPadSize*2+sizeZ);
		
		T* rawData = new T[voxelCount];
		memset(rawData,0,voxelCount*sizeof(T));
		int indexInPaddedData = 0;
		int indexInRawData = 0;
		for(int k=0;k<sizeZ;k++)
		for(int j=0;j<sizeY;j++)
		for(int i=0;i<sizeX;i++)
		{
			indexInRawData =(int)( (int)(i*ratioX)+(int)(j*oldSizeX*ratioY)+(int)(k*oldSizeY*oldSizeX*ratioZ) );
			indexInPaddedData = 
				(m_iPadSize+i)+
				(m_iPadSize+j)*(m_iPadSize*2+sizeX)+
				(m_iPadSize+k)*(m_iPadSize*2+sizeY)*(m_iPadSize*2+sizeX);

			rawData[indexInPaddedData] = GetSampleValueAtRawData(i*ratioX,j*ratioY,k*ratioZ);//rawDataTmp[indexInRawData];
		}
		
#else
		int voxelCount = (sizeX+2*m_iPadSize)*(m_iPadSize*2+sizeY)*(m_iPadSize*2+sizeZ);
		T* rawData = new T[voxelCount];
		memset(rawData,0,voxelCount*sizeof(T));

		int indexInPaddedData = 0;
		int indexInRawData = 0;
		for(int k=0;k<sizeZ;k++)
		for(int j=0;j<sizeY;j++)
		for(int i=0;i<sizeX;i++)
		{
			indexInRawData =(int)( (int)(i)+(int)(j*oldSizeX)+(int)(k*oldSizeY*oldSizeX) );
			indexInPaddedData = 
				(m_iPadSize+i)+
				(m_iPadSize+j)*(m_iPadSize*2+sizeX)+
				(m_iPadSize+k)*(m_iPadSize*2+sizeY)*(m_iPadSize*2+sizeX);

			rawData[indexInPaddedData] = rawDataTmp[indexInRawData];
		}
#endif
		//free data
		//delete [] rawDataTmp;
		
		//reference count = 1
		m_rawData = new DataArray(rawData,sizeof(T),
			sizeX+m_iPadSize*2,//new size
			sizeY+m_iPadSize*2,
			sizeZ+m_iPadSize*2);

		//delete []rawData;
		//m_rawData = new DataArray(rawDataTmp,sizeof(T),
		//	sizeX,sizeY,sizeZ);
		return true;
	}
	//读取数据
	bool LoadRawDataFromFile(const std::string& fileName,
		int sizeX,int sizeY,int sizeZ)
	{
		//检查存在与否
		FILE* filePointer = fopen(fileName,"r");
		if(!filePoitner)
		{
			return false;
		}
		m_szFileName = fileName;

		if(m_rawData)
		{
			m_rawData->ForceClear();
		}
		//读取文件
		int voxelCount = sizeX*sizeY*sizeZ;
		int sizeOfType = sizeof(T);
		int memoryCountInBytes = voxelCount * sizeOfType;

		T* rawData = new T[voxelCount];
		if(memoryCountInBytes != fread(rawData,sizeOfType,voxelCount,filePointer))
		{
			ShowMessage("数据读取不正确");
			delete []rawData;

			return false;
		}
		//reference count = 1
		m_rawData = new DataArray(rawData,sizeof(T),sizeX,sizeY,sizeZ);

		return true;
	}

//属性
#if 1
	//[只有当从文件读取时才有意义]
	bool GetFileName(std::string& fileName) const
	{
		fileName = m_szFileName;

		return m_bLoadFromFile;
	}

	int GetTypeSize() const
	{
		return sizeof(T);
	}

	DataArray* GetDataArray(){ return m_rawData;}
	T* GetRawData(){ return m_rawData.GetRawData(); }
	int GetSizeX()const{return m_rawData.GetSizeX(); }
	int GetSizeY()const{return m_rawData.GetSizeY(); }
	int GetSizeZ()const{return m_rawData.GetSizeZ(); }

	double GetSampleValueAtRawData(float x,float y,float z)
	{
		if(x <= 0 || y<=0 || z<=0 ||
			x >= m_rawDataSize.X-1 ||
			y >= m_rawDataSize.Y-1 ||
			z >= m_rawDataSize.Z-1)
			return 0.0f;


		//get neighbor
		
		double rx = x;
		double ry = y;
		double rz = z;

		int x_in_image = (int)rx;
		int y_in_image = (int)ry;
		int z_in_image = (int)rz;

		rx -=x_in_image;
		ry -=y_in_image;
		rz -=z_in_image;

		T v000 = m_fileRawData->GetValueAt(x_in_image,y_in_image,z_in_image);
		T v100 = m_fileRawData->GetValueAt(x_in_image+1,y_in_image,z_in_image);
		T v010 = m_fileRawData->GetValueAt(x_in_image,y_in_image+1,z_in_image);
		T v110 = m_fileRawData->GetValueAt(x_in_image+1,y_in_image+1,z_in_image);

		T v001 = m_fileRawData->GetValueAt(x_in_image,y_in_image,z_in_image+1);
		T v101 = m_fileRawData->GetValueAt(x_in_image+1,y_in_image,z_in_image+1);
		T v011 = m_fileRawData->GetValueAt(x_in_image,y_in_image+1,z_in_image+1);
		T v111 = m_fileRawData->GetValueAt(x_in_image+1,y_in_image+1,z_in_image+1);

		double judge_value = 
			v000*(1-rx)*(1-ry)*(1-rz)+
			v100*rx*(1-ry)*(1-rz)+
			v010*(1-rx)*ry*(1-rz)+
			v110*rx*ry*(1-rz)+

			v001*(1-rx)*(1-ry)*rz+
			v101*rx*(1-ry)*rz+
			v011*(1-rx)*ry*rz+
			v111*rx*ry*rz; 

		return judge_value;
	}
#endif

private:
	std::string m_szFileName;
	bool m_bLoadFromFile;
	//int m_iSizeX,m_iSizeY,m_iSizeZ;
	//T* m_rawData;
	DataArray* m_fileRawData;
	DataArray* m_rawData;
	int m_iPadSize;
	int m_nMaxSize;
	//vector<IDataProcessor*> m_processorList;
	//vector<IDataRenderer*> m_rendererList;

	kk::core::vector3df m_spacing;
	kk::core::vector3di m_rawDataSize;

	void DefaultInitialization()
	{
		m_szFileName = "";
		m_bLoadFromFile = false;
		m_rawData = 0;
		m_fileRawData=0;
		m_iPadSize = 3;
		m_nMaxSize = 50;
		m_spacing = core::vector3df(1.2f,1.2f,3.333f);
	}
};

#endif