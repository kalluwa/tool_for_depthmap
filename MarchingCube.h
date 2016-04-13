#ifndef __MARCHINGCUBE_H__
#define __MARCHINGCUBE_H__

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "Core\core.h"
#include <vector>

typedef unsigned int uint;
using namespace kk;
using namespace kk::core;
//struct float3 
//{
//	float x,y,z;
//};

class MarchingCube
{
private:

	int row_vox;
	int col_vox;
	int len_vox;
	int tot_vox;
	float step;

	float *scalar;
	vector3df* normal;
	vector3df* pos;//体素点在空间中的位置
	vector3df origin;

	float isovalue;

	unsigned int m_vao;
	unsigned int m_vbo;

	std::vector<vector3df> Positions;
	std::vector<vector3df> Normals;
public:

	MarchingCube(int _row_vox, int _col_vox, int _len_vox, float *_scalar, vector3df *_pos, vector3df _origin, float _step, float _isovalue);
	~MarchingCube();
	void CreateVBO(const std::vector<vector3df>& positions,
		const std::vector<vector3df>& normals);

	std::vector<vector3df>& GetTriangles() { return Positions;}
	void run();
	void Draw();

	bool m_glewInited;
	int m_iTriCount;
};

#endif