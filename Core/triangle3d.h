// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _KK_TRIANGLE_3D_
#define _KK_TRIANGLE_3D_

#include "vector3d.h"
#include "line3d.h"
#include "plane3d.h"
#include "aabbox3d.h"

namespace kk
{
namespace core
{
	
	template <class T>
	class triangle3d  
	{
	public:

		bool isTotalInsideBox(const kk::core::aabbox3d<T>& box) const
		{
			return (box.isPointInside(pointA) && 
				box.isPointInside(pointB) &&
				box.isPointInside(pointC));
		}

		bool operator==(const triangle3d<T>& other) const { return other.pointA==pointA && other.pointB==pointB && other.pointC==pointC; }
		bool operator!=(const triangle3d<T>& other) const { return other.pointA!=pointA || other.pointB!=pointB || other.pointC!=pointC; }

		
		kk::core::vector3d<T> closestPointOnTriangle(const kk::core::vector3d<T>& p) const
		{
			
			core::vector3d<T> rab = kk::core::line3d<T>(pointA, pointB).getClosestPoint(p);
			core::vector3d<T> rbc = kk::core::line3d<T>(pointB, pointC).getClosestPoint(p);
			core::vector3d<T> rca = kk::core::line3d<T>(pointC, pointA).getClosestPoint(p);

			T d1 = rab.getDistanceFrom(p);
			T d2 = rbc.getDistanceFrom(p);
			T d3 = rca.getDistanceFrom(p);

			if (d1 < d2)
				return d1 < d3 ? rab : rca;
            
			return d2 < d3 ? rbc : rca;
		}


		bool isPointInside(const kk::core::vector3d<T>& p) const
		{
			return (isOnSameSide(p, pointA, pointB, pointC) &&
				isOnSameSide(p, pointB, pointA, pointC) &&
				isOnSameSide(p, pointC, pointA, pointB));
		}


		bool isPointInsideFast(const kk::core::vector3d<T>& p) const
		{
			kk::core::vector3d<T> f = pointB - pointA;
			kk::core::vector3d<T> g = pointC - pointA;

			f32 a = f.dotProduct(f);
			f32 b = f.dotProduct(g);
			f32 c = g.dotProduct(g);

			f32 ac_bb = (a*c)-(b*b);
			kk::core::vector3d<T> vp = p - pointA;

			f32 d = vp.dotProduct(f);
			f32 e = vp.dotProduct(g);
			f32 x = (d*c)-(e*b);
			f32 y = (e*a)-(d*b);
			f32 z = x+y-ac_bb;

			return (( ((u32&)z)& ~(((u32&)x)|((u32&)y))) & 0x80000000)!=0;
		}


		bool isPointInsideBarycentricTechnique(const kk::core::vector3d<T>& p) const
		{
			core::vector3d<T> v0 = pointC - pointA;
			core::vector3d<T> v1 = pointB - pointA;
			core::vector3d<T> v2 = p - pointA;

			//compute dot products
			f32 dot00 = v0.dotProduct(v0);
			f32 dot01 = v0.dotProduct(v1);
			f32 dot02 = v0.dotProduct(v2);
			f32 dot11 = v1.dotProduct(v1);
			f32 dot12 = v1.dotProduct(v2);

			//compute barycentric cooridnates
			f32 invDenom = 1.0f/(dot00 * dot11 - dot01*dot01);
			f32 u = (dot11 * dot02 - dot01 * dot12) * invDenom;
			f32 v = (dot00 * dot12 - dot01 * dot02) * invDenom;

			//check if point is in this triangle
			return (u >=0) && (v>=0) && (u+v<1); 
		}

		bool isOnSameSide(const kk::core::vector3d<T>& p1, const kk::core::vector3d<T>& p2, 
			const kk::core::vector3d<T>& a, const kk::core::vector3d<T>& b) const
		{
			kk::core::vector3d<T> bminusa = b - a;
			kk::core::vector3d<T> cp1 = bminusa.crossProduct(p1 - a);
			kk::core::vector3d<T> cp2 = bminusa.crossProduct(p2 - a);
			return (cp1.dotProduct(cp2) >= core::ROUNDING_ERROR_32);
		}


		bool getIntersectionWithLimitedLine(const kk::core::line3d<T>& line,
			kk::core::vector3d<T>& outIntersection) const
		{
			return getIntersectionWithLine(line.start,
				line.getVector(), outIntersection) &&
				outIntersection.isBetweenPoints(line.start, line.end);
		}


		bool getIntersectionWithLine(const kk::core::vector3d<T>& linePoint,
			const kk::core::vector3d<T>& lineVect, kk::core::vector3d<T>& outIntersection) const
		{
			if (getIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
				return isPointInside(outIntersection);

			return false;			
		}


		bool getIntersectionOfPlaneWithLine(const kk::core::vector3d<T>& linePoint,
			const kk::core::vector3d<T>& lineVect, kk::core::vector3d<T>& outIntersection) const
		{
			const kk::core::vector3d<T> normal = getNormal().normalize();
			T t2;
            
			if ( core::iszero ( t2 = normal.dotProduct(lineVect) ) )
				return false;

			T d = pointA.dotProduct(normal);
			T t = -(normal.dotProduct(linePoint) - d) / t2;
			outIntersection = linePoint + (lineVect * t);
			return true;
		}

		
		kk::core::vector3d<T> getNormal() const
		{
			return (pointB - pointA).crossProduct(pointC - pointA);
		}

		bool isFrontFacing(const kk::core::vector3d<T>& lookDirection) const
		{
			kk::core::vector3d<T> n = getNormal();
			n.normalize();
			return n.dotProduct(lookDirection) <= 0.0f;
		}

		//! Returns the plane of this triangle.
		kk::core::plane3d<T> getPlane() const
		{
			return kk::core::plane3d<T>(pointA, pointB, pointC);
		}

		
		T getArea() const
		{
			return (pointB - pointA).crossProduct(pointC - pointA).getLength() * 0.5;

		}

		
		void set(const kk::core::vector3d<T>& a, const kk::core::vector3d<T>& b, const kk::core::vector3d<T>& c)
		{
			pointA = a;
			pointB = b;
			pointC = c;
		}

		
		
		
		kk::core::vector3d<T> pointA; 
		kk::core::vector3d<T> pointB; 
		kk::core::vector3d<T> pointC; 
	};


	//! Typedef for a f32 3d triangle.
	typedef triangle3d<f32> triangle3df;

	//! Typedef for an integer 3d triangle.
	typedef triangle3d<s32> triangle3di;

} // end namespace core
} // end namespace kk

#endif

