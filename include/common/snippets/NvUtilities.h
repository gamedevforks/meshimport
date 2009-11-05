// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   

#ifndef NV_UTILITIES_H
#define NV_UTILITIES_H

#include "Nxf.h"
#include "NxVec3.h"
#include "NxQuat.h"

class NxRay;
class NxMat33;
class NxBounds3;
class NxQuat;

// TODO: this is kind of a mess.

namespace NVSHARE
{

	/*!
	Extend an edge along its length by a factor
	*/
	NX_INLINE void makeFatEdge(NxVec3& p0, NxVec3& p1, NxReal fat_coeff)
	{
		NxVec3 Delta = p1 - p0;
		if(Delta.normalize()>0)
		{
			p0 -= Delta * fat_coeff;
			p1 += Delta * fat_coeff;
		}
	}

	//! Compute point as combination of barycentric coordinates
	NX_INLINE NxVec3 computeBarycentricPoint(const NxVec3& p0, const NxVec3& p1, const NxVec3& p2, NxReal u, NxReal v)
	{
		// This seems to confuse the compiler...
//		pt = (1.0f - u - v)*p0 + u*p1 + v*p2;

		const NxF32 w = 1.0f - u - v;
		return NxVec3(	w*p0.x + u*p1.x + v*p2.x,
						w*p0.y + u*p1.y + v*p2.y,
						w*p0.z + u*p1.z + v*p2.z);
	}

	void transformInertiaTensor(const NxVec3 &invD, const NxMat33 & M, NxMat33 &mIInv);

	// generates a pair of quaternions (swing, twist) such that in = swing * twist, with
	// swing.x = 0
	// twist.y = twist.z = 0, and twist is a unit quat

	void separateSwingTwist(const NxQuat &in, NxQuat &swing, NxQuat &twist);

	template<class T>
	NX_INLINE void swap(T& x, T& y)
	{
		T tmp = x;
		x = y;
		y = tmp;
	}

	NX_INLINE void debugBreak()
	{
#ifdef NX_WINDOWS
			__debugbreak();
#elif NX_LINUX
			asm ( "int $3");
#endif
	}

}

#endif
