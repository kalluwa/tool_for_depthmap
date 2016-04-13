// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _AABBOX_3D_H_INCLUDED__
#define _AABBOX_3D_H_INCLUDED__


#include "plane3d.h"
#include "line3d.h"

namespace kk
{
namespace core
{

//! Axis aligned bounding box in 3d dimensional space.
/** Has some useful methods used with occlusion culling or clipping.
*/
template <class T>
class aabbox3d
{
	public:

		// Constructors

		aabbox3d(): MinEdge(-1,-1,-1), MaxEdge(1,1,1) {};
		aabbox3d(const aabbox3d<T>& other): MinEdge(other.MinEdge), MaxEdge(other.MaxEdge) {};
		aabbox3d(const vector3d<T>& min, const vector3d<T>& max): MinEdge(min), MaxEdge(max) {};
		aabbox3d(const vector3d<T>& init): MinEdge(init), MaxEdge(init) {};
		aabbox3d(T minx, T miny, T minz, T maxx, T maxy, T maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {};

		// operators

		inline bool operator==(const aabbox3d<T>& other) const { return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);};
		inline bool operator!=(const aabbox3d<T>& other) const { return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);};

		// functions

		//! Adds a point to the bounding box, causing it to grow bigger, 
		//! if point is outside of the box
		//! \param p: Point to add into the box.
		void addInternalPoint(const vector3d<T>& p)
		{
			addInternalPoint(p.X, p.Y, p.Z);
		}

		//! Adds an other bounding box to the bounding box, causing it to grow bigger,
		//! if the box is outside of the box
		//! \param b: Other bounding box to add into this box.
		void addInternalBox(const aabbox3d<T>& b)
		{
			addInternalPoint(b.MaxEdge);
			addInternalPoint(b.MinEdge);
		}

		//! Resets the bounding box.
		void reset(T x, T y, T z)
		{
			MaxEdge.set(x,y,z);
			MinEdge = MaxEdge;
		}

		//! Resets the bounding box.
		void reset(const aabbox3d<T>& initValue)
		{
			*this = initValue;
		}

		//! Resets the bounding box.
		void reset(const vector3d<T>& initValue)
		{
			MaxEdge = initValue;
			MinEdge = initValue;
		}

		//! Adds a point to the bounding box, causing it to grow bigger, 
		//! if point is outside of the box.
		//! \param x: X Coordinate of the point to add to this box.
		//! \param y: Y Coordinate of the point to add to this box.
		//! \param z: Z Coordinate of the point to add to this box.
		void addInternalPoint(T x, T y, T z)
		{
			if (x>MaxEdge.X) MaxEdge.X = x;
			if (y>MaxEdge.Y) MaxEdge.Y = y;
			if (z>MaxEdge.Z) MaxEdge.Z = z;

			if (x<MinEdge.X) MinEdge.X = x;
			if (y<MinEdge.Y) MinEdge.Y = y;
			if (z<MinEdge.Z) MinEdge.Z = z;
		}

		//! Determinates if a point is within this box.
		//! \param p: Point to check.
		//! \return Returns true if the point is withing the box, and false if it is not.
		bool isPointInside(const vector3d<T>& p) const
		{
			return (p.X >= MinEdge.X && p.X <= MaxEdge.X &&
				p.Y >= MinEdge.Y && p.Y <= MaxEdge.Y &&
				p.Z >= MinEdge.Z && p.Z <= MaxEdge.Z);
		};

		//! Determinates if a point is within this box and its borders.
		//! \param p: Point to check.
		//! \return Returns true if the point is withing the box, and false if it is not.
		bool isPointTotalInside(const vector3d<T>& p) const
		{
			return (p.X > MinEdge.X && p.X < MaxEdge.X &&
				p.Y > MinEdge.Y && p.Y < MaxEdge.Y &&
				p.Z > MinEdge.Z && p.Z < MaxEdge.Z);
		};

		//! Determinates if the box intersects with an other box.
		//! \param other: Other box to check a intersection with.
		//! \return Returns true if there is a intersection with the other box, 
		//! otherwise false.
		bool intersectsWithBox(const aabbox3d<T>& other) const
		{
			return (MinEdge <= other.MaxEdge && MaxEdge >= other.MinEdge);
		}

		bool isFullInside(const aabbox3d<T>& other) const
		{
			return MinEdge >= other.MinEdge && MaxEdge <= other.MaxEdge;
		}

		//! Tests if the box intersects with a line
		//! \param line: Line to test intersection with.
		//! \return Returns true if there is an intersection and false if not.
		bool intersectsWithLine(const line3d<T>& line) const
		{
			return intersectsWithLine(line.getMiddle(), line.getVector().normalize(), 
					(T)(line.getLength() * 0.5));
		}

		//! Tests if the box intersects with a line
		//! \return Returns true if there is an intersection and false if not.
		bool intersectsWithLine(const vector3d<T>& linemiddle, 
					const vector3d<T>& linevect,
					T halflength) const
		{
			const vector3d<T> e = getExtent() * (T)0.5;
			const vector3d<T> t = getCenter() - linemiddle;
			T r;

			if ((fabs(t.X) > e.X + halflength * fabs(linevect.X)) || 
				(fabs(t.Y) > e.Y + halflength * fabs(linevect.Y)) ||
				(fabs(t.Z) > e.Z + halflength * fabs(linevect.Z)) )
				return false;

			r = e.Y * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.Y);
			if (fabs(t.Y*linevect.Z - t.Z*linevect.Y) > r )
				return false;

			r = e.X * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.X);
			if (fabs(t.Z*linevect.X - t.X*linevect.Z) > r )
				return false;

			r = e.X * (T)fabs(linevect.Y) + e.Y * (T)fabs(linevect.X);
			if (fabs(t.X*linevect.Y - t.Y*linevect.X) > r)
				return false;

			return true;
		}

		//! Classifies a relation with a plane.
		//! \param plane: Plane to classify relation to.
		//! \return Returns ISREL3D_FRONT if the box is in front of the plane,
		//! ISREL3D_BACK if the box is back of the plane, and
		//! ISREL3D_CLIPPED if is on both sides of the plane.
		EIntersectionRelation3D classifyPlaneRelation(const plane3d<T>& plane) const
		{
			vector3d<T> nearPoint(MaxEdge);
			vector3d<T> farPoint(MinEdge);

			if (plane.Normal.X > (T)0)
			{
				nearPoint.X = MinEdge.X;
				farPoint.X = MaxEdge.X;
			}

			if (plane.Normal.Y > (T)0)
			{
				nearPoint.Y = MinEdge.Y;
				farPoint.Y = MaxEdge.Y;
			}

			if (plane.Normal.Z > (T)0)
			{
				nearPoint.Z = MinEdge.Z;
				farPoint.Z = MaxEdge.Z;
			}

			if (plane.Normal.dotProduct(nearPoint) + plane.D > (T)0)
				return ISREL3D_FRONT;

			if (plane.Normal.dotProduct(farPoint) + plane.D > (T)0)
				return ISREL3D_CLIPPED;

			return ISREL3D_BACK;
		}


		//! returns center of the bounding box
		vector3d<T> getCenter() const
		{
			return (MinEdge + MaxEdge) / 2;
		}

		//! returns extend of the box
		vector3d<T> getExtent() const
		{
			return MaxEdge - MinEdge;
		}


		//! stores all 8 edges of the box into a array
		//! \param edges: Pointer to array of 8 edges
		void getEdges(vector3d<T> *edges) const
		{
			const core::vector3d<T> middle = getCenter();
			const core::vector3d<T> diag = middle - MaxEdge;

			/*
			Edges are stored in this way:
			Hey, am I an ascii artist, or what? :) niko.
                  /4--------/0
                 /  |      / |
                /   |     /  |
                6---------2  |
                |   5- - -| -1
                |  /      |  /
                |/        | /
                7---------3/ 
			*/

			edges[0].set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
			edges[1].set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
			edges[2].set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
			edges[3].set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
			edges[4].set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
			edges[5].set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
			edges[6].set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
			edges[7].set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		}


		//! returns if the box is empty, which means that there is
		//! no space within the min and the max edge.
		bool isEmpty() const
		{
			return MinEdge.equals ( MaxEdge );
		}

		//! repairs the box, if for example MinEdge and MaxEdge are swapped.
		void repair()
		{
			T t;

			if (MinEdge.X > MaxEdge.X)
				{ t=MinEdge.X; MinEdge.X = MaxEdge.X; MaxEdge.X=t; }
			if (MinEdge.Y > MaxEdge.Y)
				{ t=MinEdge.Y; MinEdge.Y = MaxEdge.Y; MaxEdge.Y=t; }
			if (MinEdge.Z > MaxEdge.Z)
				{ t=MinEdge.Z; MinEdge.Z = MaxEdge.Z; MaxEdge.Z=t; }
		}

		//! Calculates a new interpolated bounding box.
		//! \param other: other box to interpolate between
		//! \param d: value between 0.0f and 1.0f.
		aabbox3d<T> getInterpolated(const aabbox3d<T>& other, f32 d) const
		{
			f32 inv = 1.0f - d;
			return aabbox3d<T>((other.MinEdge*inv) + (MinEdge*d),
				(other.MaxEdge*inv) + (MaxEdge*d));
		}

		//test intersection with ray[line3d]
		//http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
		bool getIntersectionPoint(const line3d<T>& ray,vector3d<T>& intersectPoint)
		{
			//initilization
			vector3d<T> direction =ray.end ;

			T Infinity = 100000000;
			T Tnear = -Infinity,Tfar = Infinity;
			T T1,T2;
			//for each pair of planes

			//plane normal along x-axis
			if(direction.X==0)
			{
				if(ray.start.X<MinEdge.X || ray.start.X>MaxEdge.X)
					return false;//parallel to the yz plane
			}
			//calculate the x intersect pos
			T1 = (MinEdge.X - ray.start.X)/direction.X;
			T2 = (MaxEdge.X - ray.start.X)/direction.X;
			//make sure T1 is the smaller one
			if(T1 > T2){ f32 tmp = T1; T1 = T2; T2 = tmp; }
			if(T1 > Tnear) Tnear = T1;
			if(T2 < Tfar) Tfar = T2;
			if(Tnear > Tfar || Tfar <0 ) return false;
			
			//plane normal along y-axis
			if(direction.Y==0)
			{
				if(ray.start.Y<MinEdge.Y || ray.start.Y>MaxEdge.Y)
					return false;//parallel to the zx plane
			}
			//calculate the y intersect pos
			T1 = (MinEdge.Y - ray.start.Y)/direction.Y;
			T2 = (MaxEdge.Y - ray.start.Y)/direction.Y;
			//make sure T1 is the smaller one
			if(T1 > T2){ f32 tmp = T1; T1 = T2; T2 = tmp; }
			if(T1 > Tnear) Tnear = T1;
			if(T2 < Tfar) Tfar = T2;
			if(Tnear > Tfar || Tfar <0 ) return false;
			
			//plane normal along Z-axis
			//Tnear = -Infinity;Tfar = Infinity;
			if(direction.Z==0)
			{
				if(ray.start.Z<MinEdge.Z || ray.start.Z>MaxEdge.Z)
					return false;//parallel to the zx plane
			}
			//calculate the Z intersect pos
			T1 = (MinEdge.Z - ray.start.Z)/direction.Z;
			T2 = (MaxEdge.Z - ray.start.Z)/direction.Z;
			//make sure T1 is the smaller one
			if(T1 > T2){ f32 tmp = T1; T1 = T2; T2 = tmp; }
			if(T1 > Tnear) Tnear = T1;
			if(T2 < Tfar) Tfar = T2;
			if(Tnear > Tfar || Tfar <0 ) return false;
			
			intersectPoint = ray.start + Tnear * direction;
			return true;
		}

		//MinX:1,MinY=3,MinZ=5
		//MaxX:2,MaxY=4,MaxZ=6
		bool getPickedPlane(const line3d<T>& ray, s32& planeIndex)
		{
			core::vector3df intersectionPos;
			if(getIntersectionPoint(ray,intersectionPos))
			{
				f32 x = intersectionPos.X;
				f32 y = intersectionPos.Y;
				f32 z = intersectionPos.Z;

				s32 index =0;
				f32 epsion = 0.001f;
				//find the plane
				if(x>MinEdge.X + epsion && x< MaxEdge.X - epsion)
				{
					index+=2;
					if(y>MinEdge.Y + epsion && y< MaxEdge.Y - epsion)
						index+=2;
				}
				if( abs(x - MaxEdge.X) < epsion || abs(y - MaxEdge.Y) < epsion ||
					abs(z - MaxEdge.Z) < epsion)
					index += 1;

				//if(index == 0)
				//	return false;

				index += 1;
				planeIndex = index;
				return true;
			}
			return false;
		}
		// member variables
		
		vector3d<T> MinEdge;
		vector3d<T> MaxEdge;
};

	//! Typedef for a f32 3d bounding box.
	typedef aabbox3d<f32> aabbox3df;
	//! Typedef for an integer 3d bounding box.
	typedef aabbox3d<s32> aabbox3di;


} // end namespace core
} // end namespace kk

#endif

