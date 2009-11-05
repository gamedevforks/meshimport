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

#ifndef NV_FOUNDATION_BITUTILS_H
#define NV_FOUNDATION_BITUTILS_H

#include "Nx.h"
#include "UserMemAlloc.h"
#ifdef WIN32
#include "NvIntrinsics_Windows.h"
#endif

#ifdef _XBOX
#include "NvIntrinsics_Xbox360.h"
#endif

#ifdef __CELLOS_LV2__
#include "NvIntrinsics_ps3.h"
#endif

namespace NVSHARE
{
	NX_INLINE NxU32 bitCount32(NxU32 v)
	{
		// from http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
		NxU32 const w = v - ((v >> 1) & 0x55555555);
		NxU32 const x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
		return ((x + (x >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
	}

	/*!
	Return the index of the highest set bit. Or 0 if no bits are set.
	*/
	NX_INLINE NxU32 highestSetBit32(NxU32 v)
	{
		for(NxU32 j = 32; j-- > 0;)
		{
			if(v&(1<<j))
				return j;
		}
		return 0;
	}

	NX_INLINE bool isPowerOfTwo(NxU32 x)
	{
		return x!=0 && (x & x-1) == 0;
	}

	// "Next Largest Power of 2
	// Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
	// that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
	// the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
	// largest power of 2. For a 32-bit value:"
	NX_INLINE NxU32 nextPowerOfTwo(NxU32 x)
	{
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		return x+1;
	}

	// Helper function to approximate log2 of an integer value (assumes that the input is actually power of two)
	NX_INLINE NxU32 ilog2(NxU32 num)
	{
		for (NxU32 i=0; i<32; i++)
		{
			num >>= 1;
			if (num == 0) return i;
		}

		NX_ASSERT(0);
		return (NxU32)-1;
	}

	NX_INLINE int intChop(const NxF32& f)
	{ 
		NxI32 a			= *reinterpret_cast<const NxI32*>(&f);			// take bit pattern of float into a register
		NxI32 sign		= (a>>31);										// sign = 0xFFFFFFFF if original value is negative, 0 if positive
		NxI32 mantissa	= (a&((1<<23)-1))|(1<<23);						// extract mantissa and add the hidden bit
		NxI32 exponent	= ((a&0x7fffffff)>>23)-127;						// extract the exponent
		NxI32 r			= ((NxU32)(mantissa)<<8)>>(31-exponent);		// ((1<<exponent)*mantissa)>>24 -- (we know that mantissa > (1<<24))
		return ((r ^ (sign)) - sign ) &~ (exponent>>31);				// add original sign. If exponent was negative, make return value 0.
	}

	NX_INLINE int intFloor(const NxF32& f)
	{ 
		NxI32 a			= *reinterpret_cast<const NxI32*>(&f);									// take bit pattern of float into a register
		NxI32 sign		= (a>>31);																// sign = 0xFFFFFFFF if original value is negative, 0 if positive
		a&=0x7fffffff;																			// we don't need the sign any more
		NxI32 exponent	= (a>>23)-127;															// extract the exponent
		NxI32 expsign   = ~(exponent>>31);														// 0xFFFFFFFF if exponent is positive, 0 otherwise
		NxI32 imask		= ( (1<<(31-(exponent))))-1;											// mask for true integer values
		NxI32 mantissa	= (a&((1<<23)-1));														// extract mantissa (without the hidden bit)
		NxI32 r			= ((NxU32)(mantissa|(1<<23))<<8)>>(31-exponent);						// ((1<<exponent)*(mantissa|hidden bit))>>24 -- (we know that mantissa > (1<<24))
		r = ((r & expsign) ^ (sign)) + ((!((mantissa<<8)&imask)&(expsign^((a-1)>>31)))&sign);	// if (fabs(value)<1.0) value = 0; copy sign; if (value < 0 && value==(int)(value)) value++; 
		return r;
	}

	NX_INLINE int intCeil(const NxF32& f)
	{ 
		NxI32 a			= *reinterpret_cast<const NxI32*>(&f) ^ 0x80000000;						// take bit pattern of float into a register
		NxI32 sign		= (a>>31);																// sign = 0xFFFFFFFF if original value is negative, 0 if positive
		a&=0x7fffffff;																			// we don't need the sign any more
		NxI32 exponent	= (a>>23)-127;															// extract the exponent
		NxI32 expsign   = ~(exponent>>31);														// 0xFFFFFFFF if exponent is positive, 0 otherwise
		NxI32 imask		= ( (1<<(31-(exponent))))-1;											// mask for true integer values
		NxI32 mantissa	= (a&((1<<23)-1));														// extract mantissa (without the hidden bit)
		NxI32 r			= ((NxU32)(mantissa|(1<<23))<<8)>>(31-exponent);						// ((1<<exponent)*(mantissa|hidden bit))>>24 -- (we know that mantissa > (1<<24))
		r = ((r & expsign) ^ (sign)) + ((!((mantissa<<8)&imask)&(expsign^((a-1)>>31)))&sign);	// if (fabs(value)<1.0) value = 0; copy sign; if (value < 0 && value==(int)(value)) value++; 
		return -r;
	}

}

#endif
