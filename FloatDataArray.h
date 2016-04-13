//2015/11/13

#ifndef _FLOAT_DATA_ARRAY_H_
#define _FLOAT_DATA_ARRAY_H_


class DataArray : public virtual IBase
{
public:
	DataArray():
	  m_rawData(0),m_iVoxelStride(0),m_iSizeX(0),m_iSizeY(0),m_iSizeZ(0)
	{
		m_scaleX=1.2f;m_scaleY=1.2f;m_scaleZ=3.333f;
	}

	//create a filled float array first before constructor
	DataArray(void* rawData,int voxelStride,int sizeX,int sizeY,int sizeZ,bool copy = false)
		:m_iVoxelStride(voxelStride),m_iSizeX(sizeX),m_iSizeY(sizeY),m_iSizeZ(sizeZ)
	{
		m_scaleX=1.2f;m_scaleY=1.2f;m_scaleZ=3.333f;

		if(!copy)
			m_rawData = (unsigned char*)rawData;
		else
		{
			switch(voxelStride)
			{
			case 1:
				m_rawData = new unsigned char[sizeX*sizeY*sizeZ];
				break;
			case 4:
				m_rawData = (unsigned char*)(new float[sizeX*sizeY*sizeZ]);
				break;
			}
			
			memcpy(m_rawData,rawData,sizeX*sizeY*sizeZ*voxelStride);

		}
	}

	virtual ~DataArray()
	{
		if(m_rawData)
			delete []m_rawData;
	}


	void SetDataTo(void* rawData,int voxelStride,int sizeX,int sizeY,int sizeZ)
	{
		//clear old data
		if(m_rawData)
			delete [] m_rawData;

		m_rawData = (unsigned char*)rawData;
		m_iVoxelStride=voxelStride;
		m_iSizeX=(sizeX);m_iSizeY=(sizeY);m_iSizeZ=(sizeZ);
	}

	void ForceClear()
	{
		if(m_rawData)
			delete [] m_rawData;
	}
#if 1 // Ù–‘
	unsigned char* GetRawData(){ return m_rawData; }
	int GetVoxelStride(){ 
		return m_iVoxelStride; 
	}

	void Clear(){ memset(m_rawData,0,m_iVoxelStride*m_iSizeX*m_iSizeY*m_iSizeZ);}
	int GetSizeX()const{return m_iSizeX;}
	int GetSizeY()const{return m_iSizeY;}
	int GetSizeZ()const{return m_iSizeZ;} 
	inline float GetScaleX()const{ return m_scaleX;}
	inline float GetScaleY()const{ return m_scaleY;}
	inline float GetScaleZ()const{ return m_scaleZ;}

	float GetValueAt(int x,int y,int z)
	{
		switch(m_iVoxelStride)
		{
		case 1:
			return m_rawData[x+y*m_iSizeX+z*m_iSizeX*m_iSizeY];
		case 2:
			return ((unsigned short*)m_rawData)[x+y*m_iSizeX+z*m_iSizeX*m_iSizeY];
		case 4://TODO:int ? float
			return ((float*)m_rawData)[x+y*m_iSizeX+z*m_iSizeX*m_iSizeY];
		}

		return 0;
	}

	void SetValueTo(int x,int y,int z,float value)
	{
		switch(m_iVoxelStride)
		{
		case 1:
			m_rawData[x+y*m_iSizeX+z*m_iSizeX*m_iSizeY] = (unsigned char)value;
		case 2:
			((unsigned short*)m_rawData)[x+y*m_iSizeX+z*m_iSizeX*m_iSizeY] = (unsigned short)value;
		case 4://TODO:int ? float
			((float*)m_rawData)[x+y*m_iSizeX+z*m_iSizeX*m_iSizeY] = value;
		}
	}
#endif

private:
	unsigned char* m_rawData;
	int m_iVoxelStride;
	int m_iSizeX,m_iSizeY,m_iSizeZ;
	float m_scaleX,m_scaleY,m_scaleZ;
};
#endif
