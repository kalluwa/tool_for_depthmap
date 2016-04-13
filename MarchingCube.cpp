#include <Windows.h>
#include <glew.h>
#include <glsw.h>
#include "MarchingCube.h"
#include "MarchingCubeTable.h"
#include <stdio.h>
#include "MessageHelper.h"
#include "DrawingWrapper.h"

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"glew32s.lib")

float light_ambient[]  = { 0.5f,0.5f,0.5f,0.5f };
float light_diffuse[]  = { 0.5f,0.5f,0.5f,0.5f };
float light_position[] = { -10.0f, -20.0f, -10.0f, 1.0f };
float light_position_2[]={-10.0f, 20.0f, -10.0f, 1.0f };

MarchingCube::MarchingCube(int _row_vox, int _col_vox, int _len_vox, float*_scalar,vector3df*_pos,vector3df _origin, float _step, float _isovalue)
{
	row_vox = _row_vox;
	col_vox = _col_vox;
	len_vox = _len_vox;
	tot_vox	= row_vox*col_vox*len_vox;

	scalar  = _scalar;

	pos		= _pos;
	origin	= _origin;

	isovalue=_isovalue;
	
	normal  = (vector3df*)malloc(sizeof(vector3df)*tot_vox);
	memset(normal, 0, sizeof(vector3df)*tot_vox);

	m_glewInited = false;
}

MarchingCube::~MarchingCube()
{
	free(normal);
	if(pos)
		delete []pos;

	Positions.clear();
	Normals.clear();
}

void MarchingCube::CreateVBO(const std::vector<vector3df>& positions,
	const std::vector<vector3df>& normals)
{
	//important
	m_vao=-1;
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

	//create vbo
	GLuint vertexBufferID,normalBufferID;
	glGenBuffers(1,&vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferID);
	GLsizeiptr size = positions.size()*sizeof(vector3df);
	glBufferData(GL_ARRAY_BUFFER, size, &positions[0].X, GL_STATIC_DRAW);
	
	glGenBuffers(1,&normalBufferID);
	glBindBuffer(GL_ARRAY_BUFFER,normalBufferID);
	size = normals.size()*sizeof(vector3df);
	glBufferData(GL_ARRAY_BUFFER, size, &normals[0].X, GL_STATIC_DRAW);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,vertexBufferID);
	glVertexPointer(3,GL_FLOAT,0,0);
	glBindBuffer(GL_ARRAY_BUFFER,normalBufferID);
	glNormalPointer(GL_FLOAT,0,0);
}

void MarchingCube::Draw()
{
	DrawingWrapper::DrawUnitCube();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	if(!m_glewInited)
	{
		auto err = glewInit();
		if(GLEW_OK!=err)
		{
			//ShowMessage((char*)glewGetErrorString(err));
			return;
		}
		m_glewInited = true;
		run();
	}
	glDisable(GL_BLEND);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,light_position);
	glEnable(GL_LIGHT1);

	//glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT2, GL_POSITION,light_position_2);
	//glEnable(GL_LIGHT2);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnable(GL_LIGHTING);
	glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_iTriCount);

	glDisable(GL_LIGHTING);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}
void MarchingCube::run()
{
	float cube_value[8];
	vector3df cube_pos[8];
	vector3df cube_norm[8];

	vector3df edge_vertex[12];
	vector3df edge_norm[12];

	uint global_index;
	uint index;
	uint prev;
	uint next;
	uint x;
	uint y;
	uint z;

	int flag_index;
	int edge_flags;

	for(auto count_x=0; count_x<row_vox; count_x++)
	{
		for(auto count_y=0; count_y<col_vox; count_y++)
		{
			for(auto count_z=0; count_z<len_vox; count_z++)
			{
				global_index=count_z*row_vox*col_vox + count_y*row_vox + count_x;

				if(count_x == 0)
				{
					prev=count_z*row_vox*col_vox + count_y*row_vox + count_x+1;
					normal[global_index].X=(scalar[prev]-0.0f)/step;
				}

				if(count_x == row_vox-1)
				{
					next=count_z*row_vox*col_vox+ count_y*row_vox + count_x-1;
					normal[global_index].X=(0.0f-scalar[next])/step;
				}

				if(count_x!=0 && count_x!=row_vox-1)
				{
					prev=count_z*row_vox*col_vox + count_y*row_vox + count_x+1;
					next=count_z*row_vox*col_vox + count_y*row_vox + count_x-1;
					normal[global_index].X=(scalar[prev]-scalar[next])/step;
				}

				if(count_y == 0)
				{
					prev=count_z*row_vox*col_vox + (count_y+1)*row_vox + count_x;
					normal[global_index].Y=(scalar[prev]-0.0f)/step;
				}

				if(count_y == col_vox-1)
				{
					next=count_z*row_vox*col_vox + (count_y-1)*row_vox + count_x;
					normal[global_index].Y=(0.0f-scalar[next])/step;
				}

				if(count_y!=0 && count_y!=col_vox-1)
				{
					prev=count_z*row_vox*col_vox + (count_y+1)*row_vox + count_x;
					next=count_z*row_vox*col_vox + (count_y-1)*row_vox + count_x;
					normal[global_index].Y=(scalar[prev]-scalar[next])/step;
				}

				if(count_z == 0)
				{
					prev=(count_z+1)*row_vox*col_vox + count_y*row_vox + count_x;
					normal[global_index].Z=(scalar[prev]-0.0f)/step;
				}

				if(count_z == len_vox-1)
				{
					next=(count_z-1)*row_vox*col_vox + count_y*row_vox + count_x;
					normal[global_index].Z=(0.0f-scalar[next])/step;
				}

				if(count_z!=0 && count_z!=len_vox-1)
				{
					prev=(count_z+1)*row_vox*col_vox + count_y*row_vox + count_x;
					next=(count_z-1)*row_vox*col_vox + count_y*row_vox + count_x;
					normal[global_index].Z=(scalar[prev]-scalar[next])/step;
				}

				float norm=-sqrt(normal[global_index].X*normal[global_index].X+normal[global_index].Y*normal[global_index].Y+normal[global_index].Z*normal[global_index].Z);

				if(norm == 0.0f)
				{
					normal[global_index].X=0.0f;
					normal[global_index].Y=0.0f;
					normal[global_index].Z=0.0f;
				}
				else
				{
					normal[global_index].X=normal[global_index].X/norm;
					normal[global_index].Y=normal[global_index].Y/norm;
					normal[global_index].Z=normal[global_index].Z/norm;
				}
			}
		}
	}
	
	//std::vector<float3> positions;
	//std::vector<float3> normals;
	Positions.clear();
	Normals.clear();
	//glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT1, GL_POSITION,light_position);
	//glEnable(GL_LIGHT1);

	//glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
	//glLightfv(GL_LIGHT2, GL_POSITION,light_position_2);
	//glEnable(GL_LIGHT2);

	//glEnable(GL_LIGHTING);

	//glBegin(GL_TRIANGLES);

	for(auto count_x=0; count_x<row_vox-1; count_x++)
	{
		for(auto count_y=0; count_y<col_vox-1; count_y++)
		{
			for(auto count_z=0; count_z<len_vox-1; count_z++)
			{
				global_index=count_z*row_vox*col_vox + count_y*row_vox + count_x;

				for(uint count=0; count<8; count++)
				{
					x=count_x+vertex_offset[count][0];
					y=count_y+vertex_offset[count][1];
					z=count_z+vertex_offset[count][2];

					index = z*row_vox*col_vox + y*row_vox + x;
					cube_value[count]=scalar[index];
					cube_pos[count]=pos[index];
					cube_norm[count]=normal[index];
				}

				flag_index=0;
				for(uint count=0; count<8; count++)
				{
					if(cube_value[count] < isovalue) 
					{
						flag_index |= 1<<count;
					}
				}

				edge_flags=cube_edge_flags[flag_index];
				if(edge_flags == 0) 
				{
					continue;
				}

				for(uint count=0; count<12; count++)
				{
					if(edge_flags & (1<<count))
					{
						float diff = (isovalue - cube_value[edge_conn[count][0]]) / (cube_value[edge_conn[count][1]] - cube_value[edge_conn[count][0]]);

						edge_vertex[count].X = cube_pos[edge_conn[count][0]].X + (cube_pos[edge_conn[count][1]].X - cube_pos[edge_conn[count][0]].X) * diff;
						edge_vertex[count].Y = cube_pos[edge_conn[count][0]].Y + (cube_pos[edge_conn[count][1]].Y - cube_pos[edge_conn[count][0]].Y) * diff;
						edge_vertex[count].Z = cube_pos[edge_conn[count][0]].Z + (cube_pos[edge_conn[count][1]].Z - cube_pos[edge_conn[count][0]].Z) * diff;

						edge_norm[count].X = cube_norm[edge_conn[count][0]].X + (cube_norm[edge_conn[count][1]].X - cube_norm[edge_conn[count][0]].X) * diff;
						edge_norm[count].Y = cube_norm[edge_conn[count][0]].Y + (cube_norm[edge_conn[count][1]].Y - cube_norm[edge_conn[count][0]].Y) * diff;
						edge_norm[count].Z = cube_norm[edge_conn[count][0]].Z + (cube_norm[edge_conn[count][1]].Z - cube_norm[edge_conn[count][0]].Z) * diff;
					}
				}

				for(uint count_triangle = 0; count_triangle < 5; count_triangle++)
				{
					if(triangle_table[flag_index][3*count_triangle] < 0)
					{
						break;
					}

					//glBegin(GL_TRIANGLES);
						for(uint count_point = 0; count_point < 3; count_point++)
						{
							index = triangle_table[flag_index][3*count_triangle+count_point];
							vector3df normal;
							normal.X =edge_norm[index].X;
							normal.Y =edge_norm[index].Y;
							normal.Z =edge_norm[index].Z;
							glNormal3f(edge_norm[index].X, edge_norm[index].Y, edge_norm[index].Z);
							
							vector3df pos;
							//const float scale = 0.1f;
							pos.X = edge_vertex[index].X+origin.X;
							pos.Y = edge_vertex[index].Y+origin.Y;
							pos.Z = edge_vertex[index].Z+origin.Z;
							//glVertex3f(edge_vertex[index].x+origin.x,
							//			edge_vertex[index].y+origin.y, 
							//			(edge_vertex[index].z+origin.z)*2.7775f);
							glVertex3f(pos.X,pos.Y,pos.Z);
							Positions.push_back(pos);
							Normals.push_back(normal);
						}
				}
			}
		}
	}

	
	//glEnd();

	//glDisable(GL_LIGHTING);

	//glColor3f(1.0f, 1.0f, 0.0f);
	
	//glColor3f(1.0, 1.0, 0.0f);
	//glPointSize(20.0f);
	//glBegin(GL_POINTS);
	//	glVertex3f( 10.0f, 10.0f, 10.0f);
	//glEnd();

	//positions.clear();
	//float3 _11;
	//_11.x = 0;_11.y=0;_11.z=0;
	//positions.push_back(_11);
	//_11.x = 1;_11.y=0;_11.z=0;
	//positions.push_back(_11);
	//_11.x = 0;_11.y=0;_11.z=1;
	//positions.push_back(_11);
	//
	//_11.x = 1;_11.y=0;_11.z=1;
	//positions.push_back(_11);
	//_11.x = 1;_11.y=0;_11.z=0;
	//positions.push_back(_11);
	//_11.x = 0;_11.y=0;_11.z=1;
	//positions.push_back(_11);

	//normals.clear();
	//_11.x = 0;_11.y=1;_11.z=0;
	//normals.push_back(_11);
	//_11.x = 1;_11.y=0;_11.z=0;
	//normals.push_back(_11);
	//_11.x = 0;_11.y=0;_11.z=1;
	//normals.push_back(_11);
	//
	//_11.x = 1;_11.y=0;_11.z=1;
	//normals.push_back(_11);
	//_11.x = 1;_11.y=0;_11.z=0;
	//normals.push_back(_11);
	//_11.x = 0;_11.y=0;_11.z=1;
	//normals.push_back(_11);

	m_iTriCount = Positions.size();
	CreateVBO(Positions,Normals);
}

