//2015/11/13

#include "CMarchingCubeProcessor.h"

#include "MathHelper.h"

void CMarchingCubeProcessor::Process()
{
	if(m_bProcessed)
		return;
	//copy data to m_pMarchingCubeArray
	m_pMarchingCubeArray = new DataArray(m_fData->GetRawData(),m_fData->GetVoxelStride(),
			m_fData->GetSizeX(),m_fData->GetSizeY(),m_fData->GetSizeZ());

	int row_vox = m_originalData->GetSizeX();
	int col_vox = m_originalData->GetSizeY();
	int len_vox = m_originalData->GetSizeZ();
	//int tot_vox = row_vox*col_vox*len_vox;

	//float model_radius = 4;
	int index;
	//float center_x=2,center_y=2,center_z=2;
	const int step=2;
	const float divide_number = 1.0f/((3*step)*(3*step));
				
	for(int count_x=step; count_x<row_vox-step; count_x++)
	{
		for(int count_y=step; count_y<col_vox-step; count_y++)
		{
			for(int count_z=step; count_z<len_vox-step; count_z++)
			{
				index = count_z*row_vox*col_vox+count_y*row_vox+count_x;

				float ratio=0.f;
				for(int i=-step;i<=step;i++)
				{
					for(int j=-step;j<=step;j++)
					{
						for(int k=-step;k<=step;k++)
						{
							int tmp_index = (i+count_z)*row_vox*col_vox+(j+count_y)*row_vox+count_x+k;
							if(tmp_index < 0 || (i+count_z) >len_vox || (j+count_y)>col_vox ||
								(count_x+k) >row_vox) 
								continue;

							if(m_originalData->GetValueAt(count_x+k,count_y+j,count_z+i) > 0.0f)
								ratio += divide_number;
						}
					}
				}
				
				m_pMarchingCubeArray->SetValueTo(count_x,count_y,count_z,pow(ratio,2.f));
			}
		}
	}
}