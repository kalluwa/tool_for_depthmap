#include "CCpuDepthProcessor.h"

//#if 1
#define _DEBUG_SPEED_TEST_

#ifdef  _DEBUG_SPEED_TEST_
#include <Windows.h>
#endif

#include <ap.h>
#include <dataanalysis.h>
#include <vector>
#include "MathHelper.h"

using namespace std;
using namespace alglib;



#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008 
#include <stdlib.h> // Make : g++ -O3 -fopenmp smallpt.cpp -o smallpt 
#include <stdio.h>  //        Remove "-fopenmp" for g++ version < 4.2 
#include <time.h>

#include<iostream>
#include <queue>

#include "helper\CArcCamera.h"
#include "DrawingWrapper.h"
#include "MessageHelper.h"



//#define _ORTH_VIEW_

//#######################################
template<class T>
class data_mask_block
{
	//aabbox[-1,1][-1,1][-1,1]
public:
	bool valid;
	T* data;
	int sx,sy,sz;
	Box data_box;
	//variables
	int nMaxSize;
	double fScale;
	double mmInX,mmInY,mmInZ;
	bool bSimple;

	data_mask_block(const char* filename,bool simple=false,double _mmX=1.2,double _mmY=1.2,double _mmZ=3.333,int _maxSize=50)
	{
		data=0;
		data_box.min_pos = Vec(-1,-1,-1);
		data_box.max_pos = Vec(1,1,1);

		//user settings
		mmInX=_mmX;mmInY=_mmY;mmInZ=_mmZ;
		nMaxSize = _maxSize;
		bSimple = simple;
		read_file_into_data(filename);
	}

	data_mask_block(){}

	void LoadDataFrom(unsigned char* rawData,int _sx,int _sy,int _sz)
	{
		sx = _sx;sy= _sy;sz=_sz;
		data_box.min_pos = Vec(-1,-1,-1);
		data_box.max_pos = Vec(1,1,1);

		//user settings
		mmInX=1.2;mmInY=1.2;mmInZ=3.3333;
		nMaxSize = 50;


		double max_axis = max(sx,max(sy,(int)(sz*mmInZ/mmInX)));
		//use [-1,1][-1,1][-1,1]
		data_box.max_pos.x = sx/(double)max_axis;
		data_box.max_pos.y = sy/(double)max_axis;
		data_box.max_pos.z = sz*(mmInZ/mmInX)/(double)max_axis;

		data_box.min_pos.x = -sx/(double)max_axis;
		data_box.min_pos.y = -sy/(double)max_axis;
		data_box.min_pos.z = -sz*(mmInZ/mmInX)/(double)max_axis;
		set_basic_information();

		//for(int i=0;i<sx; ++i)
		//for(int j=0;j<sy; ++j)
		//for(int k=0;k<sz; ++k) {

		//	int index = i+j*sx+k*sx*sy;
		//	setvalue(i,j,k,rawData[index]);	
		//}

		const int step=2;
		const float divide_number = 1.0f/((3*step)*(3*step));
			
		for(int x=step;x<sx-step;x++)
		for(int y=step;y<sy-step;y++)
		for(int z=step;z<sz-step;z++)
		{
#if 1 
			//if(source_data[x+sx*y+sx*sy*z]!=0)//==(unsigned char)255)
			////if(region_mask[x+sx*y+sx*sy*z]==maxregion_index)
				//setvalue(x,y,z,1);

			float ratio=0.f;
			for(int i=-step;i<=step;i++)
			{
				for( int j=-step;j<=step;j++)
				{
					for( int k=-step;k<=step;k++)
					{
						int tmp_index = (i+z)*sx*sy+(j+y)*sx+x+k;
						if(tmp_index < 0 || (i+z) >sz || (j+y)>sy ||
							(x+k) >sx) 
							continue;

						if(rawData[tmp_index] > 0.0f)
								ratio += divide_number;
					}
				}
			}

			//setvalue(x,y,z,ratio);
			setvalue(x,y,z,pow(ratio,2.f));
#else 
			setvalue(x,y,z,source_data[x+sx*y+sx*sy*z]);
#endif
		}
	}

	//~data_mask_block()
	//{
	//	if(data)
	//		delete[] data;
	//}

	data_mask_block(int _sx,int _sy,int _sz)
	{
		sx = _sx;sy= _sy;sz=_sz;
		data_box.min_pos = Vec(-1,-1,-1);
		data_box.max_pos = Vec(1,1,1);
		
		//user settings
		mmInX=1.2;mmInY=1.2;mmInZ=3.3333;
		nMaxSize = 50;

		double max_axis = max(sx,max(sy,sz));
#if 0
		//use [0,1][0,1][0,1]
		data_box.max_pos.x = sx/(double)max_axis;
		data_box.max_pos.y = sy/(double)max_axis;
		data_box.max_pos.z = sz/(double)max_axis;
		Vec offset = Vec(0.5,0.5,0.5)-data_box.max_pos *0.5;
		data_box.min_pos = data_box.min_pos + offset;
		data_box.max_pos = data_box.max_pos + offset;
#else
		//use [-1,1][-1,1][-1,1]
		data_box.max_pos.x = sx/(double)max_axis;
		data_box.max_pos.y = sy/(double)max_axis;
		data_box.max_pos.z = sz/(double)max_axis;

		data_box.max_pos.x = -sx/(double)max_axis;
		data_box.max_pos.y = -sy/(double)max_axis;
		data_box.max_pos.z = -sz/(double)max_axis;
#endif
		set_basic_information();
		//TODO:remove
		_test_mask();
	}

	void set_basic_information()
	{
		
		if(sx==0||sy==0||sz==0)
		{
			data = 0;
			//puts("data size cannot be 0");
			valid = false;
		}
		else
		{
			data = new T[sx*sy*sz];
			valid = true;
		}
	}
	bool read_file_into_data(const char* filename)
	{
		FILE* f;
		fopen_s(&f,filename,"r");
		if(!f)
		{
			puts("###");
			puts(filename);
			puts("###Error\n");
			return false;
		}
		fread(&sx,4,1,f);
		fread(&sy,4,1,f);
		fread(&sz,4,1,f);

		//update nMaxSize And fScale
		fScale = min( (double)nMaxSize/( (double)sz*mmInZ/mmInX ),
min((double)nMaxSize/(double)sx,(double)nMaxSize/(double)sy));

		int sizeInBytes = sx*sy*sz;
		//int max_length = max(sx,max(sy,sz));
		unsigned char* source_data = new unsigned char[sizeInBytes];
		//unsigned char* tmpData = new unsigned char[sizeInBytes];
		int bytes_read = fread(source_data,1,sx*sy*sz,f);
		if(bytes_read != sx*sy*sz)
		{
			ShowMessage("读取错误!");
			delete []source_data;
			return false;
		}
		fclose(f);
		set_basic_information();
		/*for(int x=0;x<1;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}
		for(int x=sx-1;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}

		for(int x=0;x<sx;x++)
		for(int y=0;y<1;y++)
		for(int z=0;z<sz;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}
		for(int x=0;x<sx;x++)
		for(int y=sy-1;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}

		for(int x=0;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<1;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}
		for(int x=0;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=sz-1;z<sz;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}*/

		//sx+=2;
		//sy+=2;
		//sz+=2;
		
		/*for(int x=0;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			source_data[x+y*sx+z*sy*sx]=0;
		}
		
		for(int x=1;x<sx-1;x++)
		for(int y=1;y<sy-1;y++)
		for(int z=1;z<sz-1;z++)
		{
			source_data[x+y*sx+z*sy*sx] = tmpData[x-1+(y-1)*(sx-2)+(z-1)*(sx-2)*(sz-2)];
		}*/
		//delete[] tmpData;
		//valid box

#if 0
		//use [0,1][0,1][0,1]
		double max_axis = max(sx*1.2,max(sy*1.2,sz*3.333));
		data_box.max_pos.x = sx*1.2/(double)max_axis;
		data_box.max_pos.y = sy*1.2/(double)max_axis;
		data_box.max_pos.z = sz*3.333/(double)max_axis;
		Vec offset = Vec(0.5,0.5,0.5)-data_box.max_pos *0.5;
		data_box.min_pos = data_box.min_pos + offset;
		data_box.max_pos = data_box.max_pos + offset;
#else
		//use [-1,1][-1,1][-1,1]
		double max_axis = max(sx*1.2,max(sy*1.2,sz*3.333));
		data_box.max_pos.x = sx*1.2/(double)max_axis;
		data_box.max_pos.y = sy*1.2/(double)max_axis;
		data_box.max_pos.z = sz*3.333/(double)max_axis;
		data_box.min_pos = data_box.max_pos*-1.0f;
#endif

		if(bSimple)
		{
			for(int x=0;x<sx;x++)
			for(int y=0;y<sy;y++)
			for(int z=0;z<sz;z++)
			{
				setvalue(x,y,z,source_data[x+sx*y+sx*sy*z]>0 ? 1:0);
			}

			return true;
		}
		//test
		int count=0;
		for(int x=0;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			setvalue(x,y,z,0);
		}
#if 0
		//gradient
		for(int x=1;x<sx-1;x++)
		for(int y=1;y<sy-1;y++)
		for(int z=1;z<sz-1;z++)
		{
#define get_source_value(x,y,z) source_data[(x)+sx*(y)+sx*sy*(z)]
			Vec gradient(get_source_value(x+1,y,z)-get_source_value(x-1,y,z),
				get_source_value(x,y+1,z)-get_source_value(x,y-1,z),
				get_source_value(x,y,z+1)-get_source_value(x,y,z-1));
			if(gradient.squareLength()>0)
				setvalue(x,y,z,1);
#undef get_source_value(x,y,z)
		}
#else

		const int step=2;
		const float divide_number = 1.0f/((3*step)*(3*step));
			
		for(int x=step;x<sx-step;x++)
		for(int y=step;y<sy-step;y++)
		for(int z=step;z<sz-step;z++)
		{
#if 1 
			//if(source_data[x+sx*y+sx*sy*z]!=0)//==(unsigned char)255)
			////if(region_mask[x+sx*y+sx*sy*z]==maxregion_index)
				//setvalue(x,y,z,1);

			float ratio=0.f;
			for(int i=-step;i<=step;i++)
			{
				for( int j=-step;j<=step;j++)
				{
					for( int k=-step;k<=step;k++)
					{
						int tmp_index = (i+z)*sx*sy+(j+y)*sx+x+k;
						if(tmp_index < 0 || (i+z) >sz || (j+y)>sy ||
							(x+k) >sx) 
							continue;

						if(source_data[tmp_index] > 0.0f)
								ratio += divide_number;
					}
				}
			}

			//setvalue(x,y,z,ratio);
			setvalue(x,y,z,pow(ratio,2.f));
#else 
			setvalue(x,y,z,source_data[x+sx*y+sx*sy*z]);
#endif
		}
#endif
		//for(int i=0;i<sizeInBytes;i++)
		//{
			/*data[i]=(T)0.0;
			if(source_data[i]!=0)
			{
				data[i]=(T)1.0;
				count++;
			}*/
			/*data[i] = (T)source_data[i];
			if(data[i]!=0)
			{
				int a=0;
			}*/
		//}
		printf("count=%d\n",count);
		delete [] source_data;
		return true;
	}
	Box get_box()const{ return data_box;}

	T getvalue_atworld(double x,double y,double z) const
	{
		if(z>=data_box.min_pos.z&&z<data_box.max_pos.z &&
			y>=data_box.min_pos.y&&y<data_box.max_pos.y&&
			x>=data_box.min_pos.x&&x<data_box.max_pos.x)
		{
			double rx = (sx*(x - data_box.min_pos.x)/(data_box.max_pos.x -data_box.min_pos.x));
			double ry = (sy*(y - data_box.min_pos.y)/(data_box.max_pos.y -data_box.min_pos.y));
			double rz = (sz*(z - data_box.min_pos.z)/(data_box.max_pos.z -data_box.min_pos.z));

			int x_in_image = (int)rx;
			int y_in_image = (int)ry;
			int z_in_image = (int)rz;

			rx -=x_in_image;
			ry -=y_in_image;
			rz -=z_in_image;

			T v000 = getvalue(x_in_image,y_in_image,z_in_image);
			T v100 = getvalue(x_in_image+1,y_in_image,z_in_image);
			T v010 = getvalue(x_in_image,y_in_image+1,z_in_image);
			T v110 = getvalue(x_in_image+1,y_in_image+1,z_in_image);

			T v001 = getvalue(x_in_image,y_in_image,z_in_image+1);
			T v101 = getvalue(x_in_image+1,y_in_image,z_in_image+1);
			T v011 = getvalue(x_in_image,y_in_image+1,z_in_image+1);
			T v111 = getvalue(x_in_image+1,y_in_image+1,z_in_image+1);

			double judge_value = 
				v000*(1-rx)*(1-ry)*(1-rz)+
				v100*rx*(1-ry)*(1-rz)+
				v010*(1-rx)*ry*(1-rz)+
				v110*rx*ry*(1-rz)+

				v001*(1-rx)*(1-ry)*rz+
				v101*rx*(1-ry)*rz+
				v011*(1-rx)*ry*rz+
				v111*rx*ry*rz;

#if 1
			return judge_value>0.95?(T)1:(T)0;
#elif 0
			return (judge_value>0.6 & judge_value>0.4)?(T)1:(T)0;
#else

			if(judge_value!=0)
			{
				int a=0;
			}
			return data[x_in_image+y_in_image*sx+z_in_image*sy*sx];
			if(data[x_in_image+y_in_image*sx+z_in_image*sy*sx] == 1)
				return (T)1;
			
			return 0;
#endif
		}
		
		//puts("coordinate error");
		return (T)0;
	}

	T getvalue_atworld2(double x,double y,double z,double prex,double prey,double prez) const
	{

		if(z>=data_box.min_pos.z&&z<data_box.max_pos.z &&
			y>=data_box.min_pos.y&&y<data_box.max_pos.y&&
			x>=data_box.min_pos.x&&x<data_box.max_pos.x)
		{
			double rx = (sx*(x - data_box.min_pos.x)/(data_box.max_pos.x -data_box.min_pos.x));
			double ry = (sy*(y - data_box.min_pos.y)/(data_box.max_pos.y -data_box.min_pos.y));
			double rz = (sz*(z - data_box.min_pos.z)/(data_box.max_pos.z -data_box.min_pos.z));

			prex = (sx*(prex - data_box.min_pos.x)/(data_box.max_pos.x -data_box.min_pos.x));
			prey = (sy*(prey - data_box.min_pos.y)/(data_box.max_pos.y -data_box.min_pos.y));
			prez = (sz*(prez - data_box.min_pos.z)/(data_box.max_pos.z -data_box.min_pos.z));

			int x_in_image = (int)(rx+0.5);
			int y_in_image = (int)(ry+0.5);
			int z_in_image = (int)(rz+0.5);

			rx -=x_in_image;
			ry -=y_in_image;
			rz -=z_in_image;

			prex -= (int)x_in_image;
			prey -= (int)y_in_image;
			prez -= (int)z_in_image;

			//isosurface
			//GRIDCELL grid;
			//grid.p[0] = XYZ(0,0,0);
			//grid.val[0] = getvalue(x_in_image,y_in_image,z_in_image);
			//grid.p[1] = XYZ(1,0,0);
			//grid.val[1] = getvalue(x_in_image+1,y_in_image,z_in_image);
			//grid.p[2] = XYZ(1,0,1);
			//grid.val[2] = getvalue(x_in_image+1,y_in_image,z_in_image+1);
			//grid.p[3] = XYZ(0,0,1);
			//grid.val[3] = getvalue(x_in_image,y_in_image,z_in_image+1);

			//grid.p[4] = XYZ(0,1,0);
			//grid.val[4] = getvalue(x_in_image,y_in_image+1,z_in_image);
			//grid.p[5] = XYZ(1,1,0);
			//grid.val[5] = getvalue(x_in_image+1,y_in_image+1,z_in_image);
			//grid.p[6] = XYZ(1,1,1);
			//grid.val[6] = getvalue(x_in_image+1,y_in_image+1,z_in_image+1);
			//grid.p[7] = XYZ(0,1,1);
			//grid.val[7] = getvalue(x_in_image,y_in_image+1,z_in_image+1);

			//int numof1s = 0;
			//for(int i=0;i<8;i++)
			//{
			//	numof1s += abs(grid.val[i] - 0)<0.00001?0:1;
			//}
			//if(numof1s == 8)
			//	return T(1);


			//if(numof1s<=0)
			//	return T(0);

			//TRIANGLE triangles[5];
			//double isolevel = 0.5;
			//int numTriangle = Polygonise(grid,isolevel,triangles);
			//if(numTriangle>0)
			//{
			//	Vec raydir(rx-prex,ry-prey,rz-prez);
			//	Vec rayorigin2isosurface(triangles[0].p[0].x-rx,triangles[0].p[0].y-ry,triangles[0].p[0].z-rz);
			//	if(raydir.dot(rayorigin2isosurface)<=0.001)
			//		return (T)1.0;
			//}
			
		}
		
		return (T)0;
	}

	bool getIsoSurface(double isoValue)
	{
		//point position in 3D 
		float3* model_vox = (float3 *)malloc(sizeof(float3)*sx*sy*sz);
		uint index=0;
		double ratio = 2.0/(max(sx,max(sy,sz)));
		for(int x=0;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			index = z*sx*sy+y*sx+x;
			model_vox[index].x = (x-sx/2.0)*(ratio);
			model_vox[index].y = (y-sy/2.0)*(ratio);
			model_vox[index].z = (z-sz/2.0)*(ratio);
		}
		float3 world_origin;
		world_origin.x = 0.0f;
		world_origin.y = 0.0f;
		world_origin.z = 0.0f;
		//float3 world_side;
		//world_side.x = 2.0f;
		//world_side.y = 2.0f;
		//world_side.z = 2.0f;
		float vox_size = 0.5f;
		MarchingCube* mc = new MarchingCube(sx, sy, sz,data, model_vox,world_origin, vox_size, isoValue);

		vector<Triangle> tris;
		mc->run(tris);
		//output triangles
		FILE* f = fopen("triangles.txt","w");
		for(uint i=0;i<tris.size();i++)
		{
			fprintf(f,"%.2lf %.2lf %.2lf ",tris[i].V0.x,tris[i].V0.y,tris[i].V0.z);
			fprintf(f,"%.2lf %.2lf %.2lf ",tris[i].V1.x,tris[i].V1.y,tris[i].V1.z);
			fprintf(f,"%.2lf %.2lf %.2lf\n",tris[i].V2.x,tris[i].V2.y,tris[i].V2.z);
		}
		fflush(f);
		fclose(f);
		f=NULL;
		//---------------
		free(model_vox);
		delete mc;

		return true;
	}
	void setvalue(int x,int y,int z,T value)
	{
		if(z>=0&&z<sz &&
			y>=0&&y<sy&&
			x>=0&&x<sx)
			data[x+sx*y+sx*sy*z] = (T)value;
	}
	
	T getvalue(int x,int y,int z) const
	{
		if(z>=0&&z<sz &&
			y>=0&&y<sy&&
			x>=0&&x<sx)
		return data[x+sx*y+sx*sy*z];
		
		//puts("invaid position");
		return (T)0;	
	}
	void _test_mask()
	{
		if(!valid)
		return;
		
		for(int x=0;x<sx;x++)
		for(int y=0;y<sy;y++)
		for(int z=0;z<sz;z++)
		{
			setvalue(x,y,z,0);
		}
#if 0
		/*for(int x=sx/2;x<sx;x++)
		for(int y=sy/2;y<sy;y++)
		for(int z=sz/2;z<sz;z++)
		{
			setvalue(x,y,z,0);
		}*/
		setvalue(0,0,0,1);
		setvalue(1,1,0,1);
		setvalue(1,0,1,1);
		setvalue(0,1,1,1);
		/*for(int x=10;x<sx;x++)
		for(int y=10;y<sy;y++)
		for(int z=10;z<sz;z++)
		{
			setvalue(x,y,z,1);
		}
*/
		/*for(int i=0;i<sx*sy*sz*0.1;i++)
		{
			int x = rand()%sx;
			int y = rand()%sy;
			int z = rand()%sz;
			setvalue(x,y,z,1);
		}*/
		//for(int x=sx/4;x<(sx*3/4);x++)
		//for(int y=sy/4;y<(sy*3/4);y++)
		//for(int z=sz/4;z<(sz*3/4);z++)
		//{
		//	setvalue(x,y,z,1);
		//}
#else
		for(int i=0;i<sx; ++i)
		for(int j=0;j<sy; ++j)
			for(int k=0;k<sz; ++k) {

				Vec voxelPosition;
				voxelPosition.x = (float)i / (sx-1) * 2.f + -1.f;
				voxelPosition.y = (float)j / (sy-1) * 2.f + -1.f;
				voxelPosition.z = (float)k / (sz-1) * 2.f + -1.f;

				// Fill the grid with a solid sphere with a very dense inner sphere
				double dfield2 = voxelPosition.squareLength();
				double value = dfield2 < 0.7f ? 1.f : 0.f;

				//if(dfield2<.25f)
				//	value = 20.f;

				setvalue(i,j,k,value);	
			}
#endif
	}
	
};

//##################interset##############################
bool radiance(const data_mask_block<float>& data_mask,const Ray& ray,double& t,double& ctvalue)
{
	const double ray_search_end = 2.0;
	double ray_search_step = 0.05;
	//double Infinity = 1.0f;
	t = 0;
	bool result = false;
	if(data_mask.get_box().intersect_with_ray(ray,t))//.data_box.intersect_with_ray(ray,t))
	{
		Vec dir = ray.d;
		dir.norm();
		//search
		
		while(t<ray_search_end)
		{
			Vec pre_pos = ray.o;
			Vec test_pos = ray.o+ray.d*t;
#if 1
			ctvalue = data_mask.getvalue_atworld(test_pos.x,test_pos.y,test_pos.z);
#else
			ctvalue = data_mask.getvalue_atworld2(test_pos.x,test_pos.y,test_pos.z,
				pre_pos.x,pre_pos.y,pre_pos.z);
#endif

#if 1
			if(0!= ctvalue)
			{
				t -= ray_search_step;
				ray_search_step/=2.0;
				result = true;
			}

			if(ray_search_step <0.0005)
			{
				//t is the distance for this ray

				break;//exit loop
			}
#else
			if(ctvalue > 0.5)
			{
				result = true;
				break;
			}
#endif
			t+= ray_search_step;
		}
	}
	return result;
};
//different camera
void save_default_camera(const data_mask_block<float>& data_mask,int img_width,int img_height,Vec camPos,Vec camTarget,Vec camRight,const char* filename="depth.ppm")
{
#ifdef _DEBUG_SPEED_TEST_
	unsigned int tickCount = ::GetTickCount();
#endif
	//preset camera 
	Vec target=camTarget;
	Vec camera=camPos;
	Ray cam(camera, (target-camera).norm());
	//offset in project plane[x,y]
	double theta = (90/2)/180.0*3.14159;
	//theta=90===>cx=2d===<d=1>===>scale=2
	//scale = 2.0;//1.0*2*tan(delta/180.0*3.14159);
	//we use d=1.0
	double scale = 1.0*tan(theta)*2;

	Vec cx = camRight;
	Vec test = cx%cam.d;//up
	if(abs(test.x)<0.001&&abs(test.y)<0.001&&abs(test.z)<0.001)
	{
		cx = Vec(0,1,0);
		test = cx%cam.d;
		if(abs(test.x)<0.001&&abs(test.y)<0.001&&abs(test.z)<0.001)
			cx = Vec(1,0,0);
	}
	
	//cx = (cx%cam.d).norm()*scale;
	//Vec cy = (cx%cam.d).norm()*-scale*(img_height/(double)img_width);
	Vec cy = (cx%cam.d).norm()*(scale);//*(img_height/(double)img_width);
	cx = (cam.d%cy).norm()*(scale);
	double* depth_values = new double[img_width*img_height];

	//int count = 0;
	for(int x=0;x<img_width;x++)
	{
		for(int y=0;y<img_height;y++)
		{
			int index = (img_height-y-1)*img_width+x;
			//for each pixel x=>[-1,1] y=>[-1,1]
#ifndef _ORTH_VIEW_
			//ray for each pixel 
			Vec d = cx*( x/(double)(img_width-1) - .5) + 
                     cy*( y/(double)(img_height-1) - .5) + 
					 cam.d;//.norm(); 

   			Ray ray(cam.o,d.norm());
#else
			Vec d = 1.1f*cx*( x/(double)(img_width-1) - .5) + 
				1.1f*cy*(y/(double)(img_height-1) - .5)+
				cam.o;
			Ray ray(d,cam.d.norm());
#endif

   			//cast this ray and get depth value
			double t=0,ctvalue;
			depth_values[index]=0;
			if(radiance(data_mask,ray,t,ctvalue))
			{
#ifndef _ORTH_VIEW_
				depth_values[index]=t * (d.dot(cam.d));
#else 
				depth_values[index]=t;
#endif
			}
		}
	}
#ifdef _DEBUG_SPEED_TEST_
	//171 ms
	unsigned int genDepthTime = ::GetTickCount() - tickCount;
	std::cout<<genDepthTime;
#endif
	//printf("%d",count);
	//scale depth texture
	double min_depth =100000.0;
	double max_depth = 0;
	for(int i=0;i<img_width*img_height;i++)
	{
		if(depth_values[i] >0.01)
		{
			if(depth_values[i] > max_depth)
				max_depth = depth_values[i];
			if(depth_values[i] < min_depth)
				min_depth = depth_values[i];
		}
	}
#if 0
	double scale_ratio  =1.0;
	if(max_depth != min_depth)
		scale_ratio  =1.0 / (max_depth - min_depth);

	for(int i=0;i<img_width*img_height;i++)
	{
		if(depth_values[i] > 0)
		{
			depth_values[i] = (depth_values[i] - min_depth)*scale_ratio;
		}
	}
#else
	for(int i=0;i<img_width*img_height;i++)
	{
		if(depth_values[i]<0.01)
			depth_values[i]=min_depth + (max_depth-min_depth)*2;
	}
	max_depth = min_depth + (max_depth-min_depth)*2;
	double scale_ratio  =1.0;
	if(max_depth != min_depth)
		scale_ratio  =1.0 / (max_depth - min_depth);
	for(int i=0;i<img_width*img_height;i++)
	{
		if(depth_values[i] > 0)
		{
			depth_values[i] = (depth_values[i] - min_depth)*scale_ratio;
		}
	}
#endif
	unsigned char* depthData = new unsigned char[img_width*img_height];
	//write depth to image
	FILE *f ;
	fopen_s(&f,filename, "w");         // Write image to PPM file. 
    fprintf(f, "P3\n%d %d\n%d\n", img_width,img_height, 255); 
    for (int i=0; i<img_width*img_height; i++) 
	{
		fprintf(f,"%d %d %d ", toInt(depth_values[i]), toInt(depth_values[i]), toInt(depth_values[i]));
		depthData[i] = toInt(depth_values[i]);
	}
	fflush(f);
	fclose(f);

	ShowImage(CreateImageFrom(depthData,img_width,img_height),filename);
	//free data
	delete[] depth_values;
}

//pca
bool pca_filter(const data_mask_block<float> data_mask,vector<Vec>& baseVector)
{
	vector<Vec> positions;
	//int pixelCount=0;
	for(int x=0;x<data_mask.sx;x++)
	{
		for(int y=0;y<data_mask.sy;y++)
		{
			for(int z=0;z<data_mask.sz;z++)
			{
				//pixelCount=0;
				//for(int i=-1;i<=1;i++)
				//for(int j=-1;j<=1;j++)
				//for(int k=-1;k<=1;k++)
				//{
				//	if(data_mask.getvalue(x+i,y+j,z+k)!=0)
				//		pixelCount++;
				//}

				//if(data_mask.getvalue(x,y,z)!=0&&pixelCount<9)
				//if(pixelCount>0&&pixelCount<9)
				if(data_mask.getvalue(x,y,z)!=0)
					positions.push_back(Vec(1.2f*x,1.2f*y,3.33333f*z));
			}
		}
	}

	//3 unit point for test 
	int num_components = 3;
	int point_size = (int)positions.size();
	//double* data = new double[point_size*num_components];
	//data[0]=0.1;data[1]=0.1;data[2]=0.1;
	//data[3]=0.4;data[4]=0.4;data[5]=0.4;
	//data[6]=0.8;data[7]=0.8;data[8]=0.8;
	//data[9]=1.2;data[10]=1.2;data[11]=1.2;
	double* data = &positions[0].x;	
	//for(int i=0;i<point_size;i++)
	//{
	//	data[i*num_components+0] = i==0?1:0;
	//	data[i*num_components+1] = i==1?1:0;
	//	data[i*num_components+2] = i==2?1:0;

	//	printf("%lf %lf %lf\n",data[i*num_components+0],data[i*num_components+1],data[i*num_components+2]);
	//}
	
	real_2d_array arr;
	arr.setcontent(point_size,num_components,data);
	//free data
	//delete[] data;
	positions.clear();
	ae_int_t info;
	real_1d_array s2;
	real_2d_array v;
	pcabuildbasis(arr,point_size,num_components,info,s2,v);

	if(info==-4)
	{
		puts("SVD subrotine haven't converged\n");
		return false;
	}
	else if(info == -1)
	{
		puts("wrong parameters has been passed(NPoints<0,NVars<1)\n");
		return false;
	}

	double* result_parameters = s2.getcontent();
	int parameters_length = s2.length();
	
	puts("Values:\n");
	for(int i=0;i<parameters_length;i++)
	{
		printf("%lf\t",result_parameters[i]);
	}
	puts("\n");
	int columns = v.cols();//should be 3
	//int rows = v.rows();//should be 3
	
	puts("base vectors:\n");
	for(int i=0;i<columns;i++)
	{
		printf("%lf %lf %lf\n",v[0][i],v[1][i],v[2][i]);
		baseVector.push_back(Vec(v[0][i],v[1][i],v[2][i]));
	}
	puts("\n");

	return true;
}
bool pca_filter_2D_xz(const data_mask_block<float>& data_mask,vector<Vec>& baseVector)
{
	vector<Vec> positions;
	//int pixelCount=0;
	for(int x=0;x<data_mask.sx;x++)
	{
		for(int y=0;y<data_mask.sy;y++)
		{
			for(int z=0;z<data_mask.sz;z++)
			{
				//pixelCount=0;
				//for(int i=-1;i<=1;i++)
				//for(int j=-1;j<=1;j++)
				//for(int k=-1;k<=1;k++)
				//{
				//	if(data_mask.getvalue(x+i,y+j,z+k)!=0)
				//		pixelCount++;
				//}

				//if(data_mask.getvalue(x,y,z)!=0&&pixelCount<9)
				//if(pixelCount>0&&pixelCount<9)
				if(data_mask.getvalue(x,y,z)!=0)
					positions.push_back(Vec(1.2*x,0,3.33*z));
			}
		}
	}

	//3 unit point for test 
	int num_components = 3;
	int point_size = (int)positions.size();
	//double* data = new double[point_size*num_components];
	//data[0]=0.1;data[1]=0.1;data[2]=0.1;
	//data[3]=0.4;data[4]=0.4;data[5]=0.4;
	//data[6]=0.8;data[7]=0.8;data[8]=0.8;
	//data[9]=1.2;data[10]=1.2;data[11]=1.2;
	double* data = &positions[0].x;	
	//for(int i=0;i<point_size;i++)
	//{
	//	data[i*num_components+0] = i==0?1:0;
	//	data[i*num_components+1] = i==1?1:0;
	//	data[i*num_components+2] = i==2?1:0;

	//	printf("%lf %lf %lf\n",data[i*num_components+0],data[i*num_components+1],data[i*num_components+2]);
	//}
	
	real_2d_array arr;
	arr.setcontent(point_size,num_components,data);
	//free data
	//delete[] data;
	positions.clear();
	ae_int_t info;
	real_1d_array s2;
	real_2d_array v;
	pcabuildbasis(arr,point_size,num_components,info,s2,v);

	if(info==-4)
	{
		puts("SVD subrotine haven't converged\n");
		return false;
	}
	else if(info == -1)
	{
		puts("wrong parameters has been passed(NPoints<0,NVars<1)\n");
		return false;
	}

	double* result_parameters = s2.getcontent();
	int parameters_length = s2.length();
	
	puts("Values:\n");
	for(int i=0;i<parameters_length;i++)
	{
		printf("%lf\t",result_parameters[i]);
	}
	puts("\n");
	int columns = v.cols();//should be 3
	//int rows = v.rows();//should be 3
	
	puts("base vectors:\n");
	for(int i=0;i<columns;i++)
	{
		printf("%lf %lf %lf\n",v[0][i],v[1][i],v[2][i]);
		baseVector.push_back(Vec(v[0][i],v[1][i],v[2][i]));
	}
	puts("\n");

	return true;
}

//########################################################
void CCpuDepthProcessor::Process()
{
	int img_width=256,img_height=256;	

	//test
#define DontUseFile

#ifdef DontUseFile
	data_mask_block<float> data_mask_whole(g_filePath.c_str(),true);
	data_mask_block<float> data_mask;
	data_mask.LoadDataFrom(m_originalData->GetRawData(),
		m_originalData->GetSizeX(),m_originalData->GetSizeY(),
		m_originalData->GetSizeZ());
#else
	data_mask_block<float> data_mask(g_filePath.c_str());
#endif

#ifdef _DEBUG_SPEED_TEST_
	std::string speed_string="";

	unsigned int TickCount = ::GetTickCount();
#endif
	//data_mask.getIsoSurface(0.5f);
	//depth default
	float scale =0.75f;
	Vec camPos = Vec(0,2,0)*scale;
	Vec camTarget(0,0,0);
	//默认视图：从上朝下看
	vector<Vec> base_vectors_raw_2D;
#ifdef DontUseFile
	pca_filter_2D_xz(data_mask_whole,base_vectors_raw_2D);
#else
	pca_filter_2D_xz(data_mask,base_vectors_raw);
#endif

	camPos = base_vectors_raw_2D[2]*-2.0f*scale;
	if(camPos.y < 0)//摄像机位置在Y上方
		camPos = camPos*-1.0f;
	Vec xz_right = Vec(0,-1,0)%base_vectors_raw_2D[0];
	if(xz_right.z < 0)//摄像机右侧为Z+
		xz_right = xz_right*-1.0f;
	save_default_camera(data_mask,img_width,img_height,camPos,camTarget,xz_right);

	SetnMaxSize(50);
	//return 0;
	//pca
	vector<Vec> base_vectors_raw;
#ifdef DontUseFile
	pca_filter(data_mask_whole,base_vectors_raw);
#else
	pca_filter(data_mask,base_vectors_raw);
#endif

	vector<Vec> base_vectors;
	//if(base_vectors_raw[2].y < 0)
	//	base_vectors_raw[2] = base_vectors_raw[2];
	base_vectors.push_back(base_vectors_raw[2]);//最短轴，面积最大
	base_vectors.push_back(base_vectors_raw[1]);//短轴
	base_vectors.push_back(base_vectors_raw[0]);//长轴

	//1 判断长轴Z值是否小于0，如果不是将其取反
	int maxIndex=0;
	float floatValue=0.0f;
	maxIndex = abs(base_vectors[2].x) > abs(base_vectors[2].y)?0:1;
	if(maxIndex == 0)
	{
		maxIndex = abs(base_vectors[2].x) > abs(base_vectors[2].z)?0:2;
		floatValue = maxIndex==0?base_vectors[2].x:base_vectors[2].z;
	}
	else
	{
		maxIndex = abs(base_vectors[2].y) > abs(base_vectors[2].z)?1:2;
		floatValue = maxIndex==1?base_vectors[2].y:base_vectors[2].z;
	}

	if(floatValue > 0)
	{
		base_vectors[2] = base_vectors[2]*-1.0f;
	}
	//2 检查是否是右手坐标系，否的话将最小轴取反
	if((base_vectors[0]%base_vectors[1]).dot(base_vectors[2]) <0)
	{
		base_vectors[0] = base_vectors[0] * -1.0f;
	}
	
	//set camera pca axis
	kk::scene::CArcCameraNode::XAxisWorld = kk::core::vector3df(base_vectors[0].x,base_vectors[0].y,base_vectors[0].z);
	kk::scene::CArcCameraNode::YAxisWorld = kk::core::vector3df(base_vectors[1].x,base_vectors[1].y,base_vectors[1].z);
	kk::scene::CArcCameraNode::ZAxisWorld = kk::core::vector3df(base_vectors[2].x,base_vectors[2].y,base_vectors[2].z);
	vector<Vec> cam_positions;

	vector<Vec> base_vectors_right;
	base_vectors_right.push_back(base_vectors[2]*-1.0f);
	base_vectors_right.push_back(base_vectors[2]*-1.0f);
	base_vectors_right.push_back(base_vectors[1]*-1.0f);

	//in diffenct angle
	for(int i=0;i<(int)base_vectors.size();i++)
	{
		//与fScale对应的值--->1.0f替代下面的-2.0*scale
		if(i!=2)
			cam_positions.push_back( base_vectors[i]*2.0f*scale);
		else
			cam_positions.push_back( base_vectors[i]*-2.0f*scale);
	}
	
	for(int i=0;i<(int)base_vectors.size();i++)
	{
		char filename[256]={0};
		sprintf_s(filename,256,"depth_pca_%d.ppm",i);
		puts("pca image: ");
		puts(filename);
		puts("\n");
		save_default_camera(data_mask,img_width,img_height,cam_positions[i],Vec(0,0,0),/*base_vectors[(i-1)%base_vectors.size()]*/base_vectors_right[i],filename);
	}
#ifdef _DEBUG_SPEED_TEST_

	double result = 0.001*(::GetTickCount() - TickCount);
	char tmp[100];sprintf_s(tmp,"PCA,生成,存储时间=%lf s",result);
	speed_string.append(tmp);

	ShowMessage(speed_string.c_str());
#endif

}

