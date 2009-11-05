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

#ifndef NV_ATOMIC_H
#define NV_ATOMIC_H

#include "UserMemAlloc.h"
#include "NxSimpleTypes.h"

namespace NVSHARE
{
	/* set *dest equal to val. Return the old value of *dest */
	NxI32	atomicExchange(volatile NxI32 *dest, NxI32 val);

	/* if *dest == comp, replace with exch. Return original value of *dest */
	NxI32	atomicCompareExchange(volatile NxI32 *dest, NxI32 exch, NxI32 comp);

	/* if *dest == comp, replace with exch. Return original value of *dest */
	void *	atomicCompareExchangePointer(volatile void **dest, void *exch, void *comp);

	/* increment the specified location. Return the incremented value */
	NxI32	atomicIncrement(volatile NxI32 *val);

	/* increment the specified location. Return the decremented value */
	NxI32	atomicDecrement(volatile NxI32 *val);

	/* add delta to *val. Return the new value */
	NxI32	atomicAdd(volatile NxI32 *val, NxI32 delta);

	/* compute the maximum of dest and val. Return the new value */
	NxI32	atomicMax(volatile NxI32 *val, NxI32 val2);

}


#endif
