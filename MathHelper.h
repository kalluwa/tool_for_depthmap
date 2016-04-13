//2015/11/12

#ifndef _MATH_HELPER_H_
#define _MATH_HELPER_H_

#include <math.h>   // smallpt, a Path Tracer by Kevin Beason, 2008 

#if !defined(M_PI)
#define M_PI 3.141592
#endif
//############helper##############
inline void swap(double& a,double& b)
{
	double c = a;
	a = b;
	b= c;
};
inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; };
inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }; 
//###############3d vector and ray and box###############################
struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm 
   double x, y, z;                  // position, also color (r,g,b) 
   Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; } 
   Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); } 
   Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); } 
   Vec operator*(double b) const { return Vec(x*b,y*b,z*b); } 
   Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); } 
   Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); } 
   double squareLength(){ return (x*x+y*y+z*z);}
   double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross: 
   Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);} 
}; 
static Vec operator*(double a,const Vec& b){return b*a;};

struct Ray { Vec o, d; Ray(Vec o_, Vec d_) : o(o_), d(d_) {} }; 

struct Box{
	Vec min_pos,max_pos;
	Box(double _minX=0,double _minY=0,double _minZ=0,
		double _maxX=1,double _maxY=1,double _maxZ=1)
	{ 
		min_pos.x = _minX;min_pos.y = _minY;min_pos.z = _minZ;
		max_pos.x = _maxX;max_pos.y = _maxY;max_pos.z = _maxZ;
		
		if(min_pos.x>max_pos.x) swap(min_pos.x,max_pos.x);
		if(min_pos.y>max_pos.y) swap(min_pos.y,max_pos.y);
		if(min_pos.z>max_pos.z) swap(min_pos.z,max_pos.z);
	}
	Box(Vec _min,Vec _max)
	{
		min_pos = _min;max_pos = _max;
		if(min_pos.x>max_pos.x) swap(min_pos.x,max_pos.x);
		if(min_pos.y>max_pos.y) swap(min_pos.y,max_pos.y);
		if(min_pos.z>max_pos.z) swap(min_pos.z,max_pos.z);
	}
	bool contain(Vec p)
	{
		if(p.x <min_pos.x||p.x > max_pos.x ||
		p.y <min_pos.y||p.y > max_pos.z ||
		p.z <min_pos.z||p.z > max_pos.z )
			return false;
			
		return true;
	}
	//test intersection with ray[line3d]
	//http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
	bool intersect_with_ray(const Ray& ray,double &t)
	{
		//initilization
		Vec direction=ray.d;//(ray.d.x,ray.d.y,ray.d.z);
		double Infinity = 100000000;
		double Tnear = -Infinity,Tfar = Infinity;
		double T1,T2;
		//for each pair of planes
		//plane normal along x-axis
		if(direction.x==0)
		{
			if(ray.o.x<min_pos.x || ray.o.x>max_pos.x)
				return false;//parallel to the yz plane
		}
		//calculate the x intersect pos
		T1 = (min_pos.x - ray.o.x)/direction.x;
		T2 = (max_pos.x - ray.o.x)/direction.x;
		//make sure T1 is the smaller one
		if(T1 > T2)//{ double tmp = T1; T1 = T2; T2 = tmp; }
			swap(T1,T2);
		if(T1 > Tnear) Tnear = T1;
		if(T2 < Tfar) Tfar = T2;
		if(Tnear > Tfar || Tfar <0 ) return false;
		//plane normal along y-axis
		if(direction.y==0)
		{
			if(ray.o.y<min_pos.y || ray.o.y>max_pos.y)
				return false;//parallel to the zx plane
		}
		//calculate the y intersect pos
		T1 = (min_pos.y - ray.o.y)/direction.y;
		T2 = (max_pos.y - ray.o.y)/direction.y;
		//make sure T1 is the smaller one
		if(T1 > T2)//{ f32 tmp = T1; T1 = T2; T2 = tmp; }
			swap(T1,T2);
		if(T1 > Tnear) Tnear = T1;
		if(T2 < Tfar) Tfar = T2;
		if(Tnear > Tfar || Tfar <0 ) return false;
		//plane normal along Z-axis
		//Tnear = -Infinity;Tfar = Infinity;
		if(direction.z==0)
		{
			if(ray.o.z<min_pos.z || ray.o.z>max_pos.z)
				return false;//parallel to the zx plane
		}
		//calculate the Z intersect pos
		T1 = (min_pos.z - ray.o.z)/direction.z;
		T2 = (max_pos.z - ray.o.z)/direction.z;
		//make sure T1 is the smaller one
		if(T1 > T2)//{ f32 tmp = T1; T1 = T2; T2 = tmp; }
			swap(T1,T2);
		if(T1 > Tnear) Tnear = T1;
		if(T2 < Tfar) Tfar = T2;
		if(Tnear > Tfar || Tfar <0 ) return false;
		//intersectPoint = ray.start + Tnear * direction;
		t = Tnear;
		return true;
	}
};

struct Triangle
{
	Vec V0,V1,V2;

	#define SMALL_NUM   0.000001 // anything that avoids division overflow
	// dot product (3D) which allows vector operations in arguments
	#define dot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
	// intersect3D_RayTriangle(): find the 3D intersection of a ray with a triangle
	//    Input:  a ray R, and a triangle T
	//    Output: *I = intersection point (when it exists)
	//    Return: -1 = triangle is degenerate (a segment or point)
	//             0 =  disjoint (no intersect)
	//             1 =  intersect in unique point I1
	//             2 =  are in the same plane
	int intersect3D_RayTriangle( Ray R, Vec* I )
	{
		Vec    u, v, n;              // triangle vectors
		Vec    dir, w0, w;           // ray vectors
		double     r, a, b;              // params to calc ray-plane intersect

		// get triangle edge vectors and plane normal
		u = V1 - V0;
		v = V2 - V0;
		n = u % v;              // cross product
		if (abs(n.x)<SMALL_NUM && abs(n.y)<SMALL_NUM && abs(n.z)<SMALL_NUM )             // triangle is degenerate
			return -1;                  // do not deal with this case

		dir = R.d;              // ray direction vector
		w0 = R.o - V0;
		a = -dot(n,w0);
		b = dot(n,dir);
		if (fabs(b) < SMALL_NUM) {     // ray is  parallel to triangle plane
			if (a == 0)                 // ray lies in triangle plane
			{
				*I = R.o;
				return 2;
			}
			else return 0;              // ray disjoint from plane
		}

		// get intersect point of ray with triangle plane
		r = a / b;
		if (r < 0.0)                    // ray goes away from triangle
			return 0;                   // => no intersect
		// for a segment, also test if (r > 1.0) => no intersect

		*I = R.o + dir*r;            // intersect point of ray and plane

		// is I inside T?
		float    uu, uv, vv, wu, wv, D;
		uu = dot(u,u);
		uv = dot(u,v);
		vv = dot(v,v);
		w = *I - V0;
		wu = dot(w,u);
		wv = dot(w,v);
		D = uv * uv - uu * vv;

		// get and test parametric coords
		float s, t;
		s = (uv * wv - vv * wu) / D;
		if (s < 0.0 || s > 1.0)         // I is outside T
			return 0;
		t = (uv * wu - uu * wv) / D;
		if (t < 0.0 || (s + t) > 1.0)  // I is outside T
			return 0;

		return 1;                       // I is in T
	}

#undef dot

};

#endif