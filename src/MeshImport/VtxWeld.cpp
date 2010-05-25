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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "VtxWeld.h"

namespace NVSHARE
{

template<> MeshVertex VertexLess<MeshVertex>::mFind = MeshVertex();
template<> std::vector<MeshVertex > *VertexLess<MeshVertex>::mList=0;


template<>
bool VertexLess<MeshVertex>::operator()(NxI32 v1,NxI32 v2) const
{

	const MeshVertex& a = Get(v1);
	const MeshVertex& b = Get(v2);

	if ( a.mPos[0] < b.mPos[0] ) return true;
	if ( a.mPos[0] > b.mPos[0] ) return false;

	if ( a.mPos[1] < b.mPos[1] ) return true;
	if ( a.mPos[1] > b.mPos[1] ) return false;

	if ( a.mPos[2] < b.mPos[2] ) return true;
	if ( a.mPos[2] > b.mPos[2] ) return false;


	if ( a.mNormal[0] < b.mNormal[0] ) return true;
	if ( a.mNormal[0] > b.mNormal[0] ) return false;

	if ( a.mNormal[1] < b.mNormal[1] ) return true;
	if ( a.mNormal[1] > b.mNormal[1] ) return false;

	if ( a.mNormal[2] < b.mNormal[2] ) return true;
	if ( a.mNormal[2] > b.mNormal[2] ) return false;

  if ( a.mColor < b.mColor ) return true;
  if ( a.mColor > b.mColor ) return false;


	if ( a.mTexel1[0] < b.mTexel1[0] ) return true;
	if ( a.mTexel1[0] > b.mTexel1[0] ) return false;

	if ( a.mTexel1[1] < b.mTexel1[1] ) return true;
	if ( a.mTexel1[1] > b.mTexel1[1] ) return false;

	if ( a.mTexel2[0] < b.mTexel2[0] ) return true;
	if ( a.mTexel2[0] > b.mTexel2[0] ) return false;

	if ( a.mTexel2[1] < b.mTexel2[1] ) return true;
	if ( a.mTexel2[1] > b.mTexel2[1] ) return false;

	if ( a.mTexel3[0] < b.mTexel3[0] ) return true;
	if ( a.mTexel3[0] > b.mTexel3[0] ) return false;

	if ( a.mTexel3[1] < b.mTexel3[1] ) return true;
	if ( a.mTexel3[1] > b.mTexel3[1] ) return false;


	if ( a.mTexel4[0] < b.mTexel4[0] ) return true;
	if ( a.mTexel4[0] > b.mTexel4[0] ) return false;

	if ( a.mTexel4[1] < b.mTexel4[1] ) return true;
	if ( a.mTexel4[1] > b.mTexel4[1] ) return false;

	if ( a.mTangent[0] < b.mTangent[0] ) return true;
	if ( a.mTangent[0] > b.mTangent[0] ) return false;

	if ( a.mTangent[1] < b.mTangent[1] ) return true;
	if ( a.mTangent[1] > b.mTangent[1] ) return false;

	if ( a.mTangent[2] < b.mTangent[2] ) return true;
	if ( a.mTangent[2] > b.mTangent[2] ) return false;

	if ( a.mBiNormal[0] < b.mBiNormal[0] ) return true;
	if ( a.mBiNormal[0] > b.mBiNormal[0] ) return false;

	if ( a.mBiNormal[1] < b.mBiNormal[1] ) return true;
	if ( a.mBiNormal[1] > b.mBiNormal[1] ) return false;

	if ( a.mBiNormal[2] < b.mBiNormal[2] ) return true;
	if ( a.mBiNormal[2] > b.mBiNormal[2] ) return false;

	if ( a.mWeight[0] < b.mWeight[0] ) return true;
	if ( a.mWeight[0] > b.mWeight[0] ) return false;

	if ( a.mWeight[1] < b.mWeight[1] ) return true;
	if ( a.mWeight[1] > b.mWeight[1] ) return false;

	if ( a.mWeight[2] < b.mWeight[2] ) return true;
	if ( a.mWeight[2] > b.mWeight[2] ) return false;

	if ( a.mWeight[3] < b.mWeight[3] ) return true;
	if ( a.mWeight[3] > b.mWeight[3] ) return false;

	if ( a.mBone[0] < b.mBone[0] ) return true;
	if ( a.mBone[0] > b.mBone[0] ) return false;

	if ( a.mBone[1] < b.mBone[1] ) return true;
	if ( a.mBone[1] > b.mBone[1] ) return false;

	if ( a.mBone[2] < b.mBone[2] ) return true;
	if ( a.mBone[2] > b.mBone[2] ) return false;

	if ( a.mBone[3] < b.mBone[3] ) return true;
	if ( a.mBone[3] > b.mBone[3] ) return false;

  if ( a.mRadius < b.mRadius ) return true;
  if ( a.mRadius > b.mRadius ) return false;

	return false;
};



}; // end of name space
