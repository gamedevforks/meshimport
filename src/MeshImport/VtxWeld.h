#ifndef VTX_WELD_H

#define VFX_WELD_H

#include "UserMemAlloc.h"

/*!
**
** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net
**
** Portions of this source has been released with the PhysXViewer application, as well as
** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.
**
** If you find this code useful or you are feeling particularily generous I would
** ask that you please go to http://www.amillionpixels.us and make a donation
** to Troy DeMolay.
**
** DeMolay is a youth group for young men between the ages of 12 and 21.
** It teaches strong moral principles, as well as leadership skills and
** public speaking.  The donations page uses the 'pay for pixels' paradigm
** where, in this case, a pixel is only a single penny.  Donations can be
** made for as small as $4 or as high as a $100 block.  Each person who donates
** will get a link to their own site as well as acknowledgement on the
** donations blog located here http://www.amillionpixels.blogspot.com/
**
** If you wish to contact me you can use the following methods:
**
** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)
** Skype ID: jratcliff63367
** Yahoo: jratcliff63367
** AOL: jratcliff1961
** email: jratcliff@infiniplex.net
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
** FundRaising Blog: http://amillionpixels.blogspot.com
** Fundraising site: http://www.amillionpixels.us
** New Temple Site:  http://newtemple.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include <set>
#include <vector>

#include "UserMemAlloc.h"
#include "MeshImport.h"

namespace MESHIMPORT
{

template <class Type> class VertexLess
{
public:
	typedef USER_STL::vector< Type > VertexVector;

	bool operator()(NxI32 v1,NxI32 v2) const;

	static void SetSearch(const Type& match,VertexVector *list)
	{
		mFind = match;
		mList = list;
	};

private:
	const Type& Get(NxI32 index) const
	{
		if ( index == -1 ) return mFind;
		VertexVector &vlist = *mList;
		return vlist[index];
	}
	static Type mFind; // vertice to locate.
	static VertexVector  *mList;
};

template <class Type> class VertexPool
{
public:
#if HE_USE_MEMORY_TRACKING
  typedef USER_STL::set<NxI32, USER_STL::GlobalMemoryPool, VertexLess<Type> > VertexSet;
#else
  typedef USER_STL::set<NxI32, VertexLess<Type> > VertexSet;
#endif
	typedef USER_STL::vector< Type > VertexVector;

	NxI32 GetVertex(const Type& vtx)
	{
		VertexLess<Type>::SetSearch(vtx,&mVtxs);
		typename VertexSet::iterator found;
		found = mVertSet.find( -1 );
		if ( found != mVertSet.end() )
		{
			return *found;
		}
		NxI32 idx = (NxI32)mVtxs.size();
		mVtxs.push_back( vtx );
		mVertSet.insert( idx );
		return idx;
	};

	void GetPos(NxI32 idx,NxF32 pos[3]) const
	{
		pos[0] = mVtxs[idx].mPos[0];
    pos[1] = mVtxs[idx].mPos[1];
    pos[2] = mVtxs[idx].mPos[2];
	}

	const Type& Get(NxI32 idx) const
	{
		return mVtxs[idx];
	};

	NxI32 GetSize(void) const
	{
		return (NxI32)mVtxs.size();
	};

	void Clear(NxI32 reservesize)  // clear the vertice pool.
	{
		mVertSet.clear();
		mVtxs.clear();
		mVtxs.reserve(reservesize);
	};

	const VertexVector& GetVertexList(void) const { return mVtxs; };

	void Set(const Type& vtx)
	{
		mVtxs.push_back(vtx);
	}

	NxI32 GetVertexCount(void) const
	{
		return (NxI32)mVtxs.size();
	};

	bool GetVertex(NxI32 i,NxF32 vect[3]) const
	{
		vect[0] = mVtxs[i].mPos[0];
    vect[1] = mVtxs[i].mPos[1];
    vect[2] = mVtxs[i].mPos[2];
		return true;
	};


	Type * GetBuffer(void)
	{
		return &mVtxs[0];
	};
private:
	VertexSet      mVertSet; // ordered list.
	VertexVector   mVtxs;  // set of vertices.
};

}; // end of namespace

#endif
