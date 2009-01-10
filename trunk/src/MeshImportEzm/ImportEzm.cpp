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
** Permission is hereby granted, free of charge, to any person obtaining a copy
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


#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "MeshImport/MeshSystem.h"
#include "ImportEZM.h"

#include "common/snippets/stable.h"
#include "common/snippets/asc2bin.h"
#include "common/TinyXML/tinyxml.h"

#pragma warning(disable:4100)
#pragma warning(disable:4996)

using namespace TINYXML;

namespace MESHIMPORT
{

#define DEBUG_LOG 0

enum NodeType
{
	NT_NONE,
	NT_SCENE_GRAPH,
	NT_MESH,
	NT_ANIMATION,
	NT_SKELETON,
	NT_BONE,
	NT_MESH_SECTION,
	NT_VERTEX_BUFFER,
	NT_INDEX_BUFFER,
	NT_NODE_TRIANGLE,
	NT_NODE_INSTANCE,
	NT_ANIM_TRACK,
	NT_LAST
};

enum AttributeType
{
	AT_NONE,
	AT_NAME,
	AT_COUNT,
	AT_PARENT,
	AT_MATERIAL,
	AT_CTYPE,
	AT_SEMANTIC,
	AT_POSITION,
	AT_ORIENTATION,
	AT_DURATION,
	AT_DTIME,
	AT_TRACK_COUNT,
	AT_FRAME_COUNT,
	AT_LAST
};

class GeometryDeformVertex
{
public:
	float        mPos[3];
	float        mNormal[3];
	float        mTexel1[2];
	float        mTexel2[2];
	float        mWeight[4];
	unsigned short mBone[4];
};

class MaxVertex
{
public:
	float         mPos[3];
	float         mWeight[4];
	unsigned char mBone[4];
	float         mNormal[3];
	float         mTexel[2];
	float         mTangent[3];
	float         mBinormal[3];
};

static void getVertex(const float *buffer,MeshImportVertex &vtx)
{
	vtx.mPos[0] = buffer[0];
	vtx.mPos[1] = buffer[1];
  vtx.mPos[2] = buffer[2];
	vtx.mNormal[0] = buffer[3];
	vtx.mNormal[1] = buffer[4];
	vtx.mNormal[2] = buffer[5];
	vtx.mTexel1[0] = buffer[6];
	vtx.mTexel1[1] = buffer[7];
	vtx.mTexel2[0] = buffer[8];
	vtx.mTexel2[0] = buffer[9];
}

struct tempVertex
{
  float pos[3];
  float normal[3];
  float tex1[2];
  float tex2[2];
  float w[4];
  unsigned short b[4];
};

static void getVertex(const tempVertex &s,MeshImportVertex &d)
{
  d.mPos[0] = s.pos[0];
  d.mPos[1] = s.pos[1];
  d.mPos[2] = s.pos[2];

  d.mNormal[0] = s.normal[0];
  d.mNormal[1] = s.normal[1];
  d.mNormal[2] = s.normal[2];

  d.mTexel1[0] = s.tex1[0];
  d.mTexel1[1] = s.tex1[1];

  d.mTexel2[0] = s.tex2[0];
  d.mTexel2[1] = s.tex2[1];

  d.mWeight[0] = s.w[0];
  d.mWeight[1] = s.w[1];
  d.mWeight[2] = s.w[2];
  d.mWeight[3] = s.w[3];

  d.mBone[0] = s.b[0];
  d.mBone[1] = s.b[1];
  d.mBone[2] = s.b[2];
  d.mBone[3] = s.b[3];
}

struct GeometryVertex
{
  float mPos[3];
  float mNormal[3];
  float mTexel1[2];
  float mTexel2[2];
};

class MeshImportEZM : public MeshImporter
{
public:
	MeshImportEZM(void)
	{
//		mCallback = 0;
		mType     = NT_NONE;
		mBone     = 0;
		mFrameCount = 0;
		mDuration   = 0;
		mTrackCount = 0;
		mDtime      = 0;
		mTrackIndex = 0;

		mToElement.SetCaseSensitive(false);
		mToAttribute.SetCaseSensitive(false);

		mToElement.Add("SceneGraph",                       NT_SCENE_GRAPH);
		mToElement.Add("Mesh",                             NT_MESH);
		mToElement.Add("Animation",                        NT_ANIMATION);
		mToElement.Add("Skeleton",                         NT_SKELETON);
		mToElement.Add("Bone",                             NT_BONE);
		mToElement.Add("MeshSection",                      NT_MESH_SECTION);
		mToElement.Add("VertexBuffer",                     NT_VERTEX_BUFFER);
		mToElement.Add("IndexBuffer",                      NT_INDEX_BUFFER);
		mToElement.Add("NodeTriangle",                     NT_NODE_TRIANGLE);
		mToElement.Add("NodeInstance",                     NT_NODE_INSTANCE);
		mToElement.Add("AnimTrack",                        NT_ANIM_TRACK);

		mToAttribute.Add("name",                           AT_NAME);
		mToAttribute.Add("count",                          AT_COUNT);
		mToAttribute.Add("parent",                         AT_PARENT);
		mToAttribute.Add("material",                       AT_MATERIAL);
		mToAttribute.Add("ctype",                          AT_CTYPE);
		mToAttribute.Add("semantic",                       AT_SEMANTIC);
		mToAttribute.Add("position",                       AT_POSITION);
		mToAttribute.Add("orientation",                    AT_ORIENTATION);
		mToAttribute.Add("duration",                       AT_DURATION);
		mToAttribute.Add("dtime",                          AT_DTIME);
		mToAttribute.Add("trackcount",                     AT_TRACK_COUNT);
		mToAttribute.Add("framecount",                     AT_FRAME_COUNT);

		mName          = 0;
		mCount         = 0;
		mParent        = 0;
		mCtype         = 0;
		mSemantic      = 0;
		mSkeleton      = 0;
		mBoneIndex     = 0;
		mIndexBuffer   = 0;
		mVertexBuffer  = 0;
		mVertexCount   = 0;
		mIndexCount    = 0;
		mAnimTrack     = 0;
		mAnimation     = 0;

	}


  virtual const char * getExtension(void)  // report the default file name extension for this mesh type.
  {
    return ".ezm";
  }

  virtual bool  importMesh(const char *meshName,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    mCallback = callback;

    if ( data && mCallback )
    {
  		TiXmlDocument *doc = MEMALLOC_NEW(TiXmlDocument);
  		bool ok = doc->LoadFile(meshName,data,dlen);
  		if ( ok )
  		{
        mCallback->importAssetName(meshName,0);
  			Traverse(doc,0);
  			ret = true;
  		}

  		if ( mAnimation )
  		{
  			mCallback->importAnimation(*mAnimation);
        MEMALLOC_DELETE(MeshImportAnimation,mAnimation);
  			mAnimation = 0;
  		}

      MEMALLOC_DELETE(TiXmlDocument,doc);

    }

    return ret;
  }

	void Traverse(TiXmlNode *node,int depth)
	{

		Process(node,depth);

		node = node->FirstChild();

		while (node )
		{

			if ( node->NoChildren() )
			{
				Process(node,depth);
			}
			else
			{
				Traverse(node,depth+1);
			}

			node = node->NextSibling();
		}

	}

	void Process(TiXmlNode *node,int depth)
	{

		const char *value = node->Value();

		ProcessNode(node->Type(),value,depth);

		TiXmlElement *element = node->ToElement(); // is there an element?  Yes, traverse it's attribute key-pair values.

		if ( element )
		{
			TiXmlAttribute *atr = element->FirstAttribute();
			while ( atr )
			{
				const char *aname  = atr->Name();
				const char *avalue = atr->Value();
				ProcessAttribute( node->Type(), value, depth, aname, avalue );
				atr = atr->Next();
			}
		}
	}


	void ProcessNode(int ntype,const char *svalue,int depth)
	{
		char value[43];
		value[39] = '.';
		value[40] = '.';
		value[41] = '.';
		value[42] = 0;

		strncpy(value,svalue,39);

		switch ( ntype )
		{
			case TiXmlNode::ELEMENT:
			case TiXmlNode::DOCUMENT:
				{
					if ( ntype == TiXmlNode::DOCUMENT )
						Display(depth,"Node(DOCUMENT): %s\n", value);
					else
					{
						mType = (NodeType)mToElement.Get(svalue);
						switch ( mType )
						{
							case NT_ANIMATION:
								if ( mAnimation )
								{
									mCallback->importAnimation(*mAnimation);
									MEMALLOC_DELETE(MeshImportAnimation,mAnimation);
									mAnimation = 0;
								}
								mName       = 0;
								mFrameCount = 0;
								mDuration   = 0;
								mTrackCount = 0;
								mDtime      = 0;
								mTrackIndex = 0;
								break;
							case NT_ANIM_TRACK:
								if ( mAnimation == 0 )
								{
									if ( mName && mFrameCount && mDuration && mTrackCount && mDtime )
									{
										int framecount = atoi( mFrameCount );
										float duration = (float) atof( mDuration );
										int trackcount = atoi(mTrackCount);
										float dtime = (float) atof(mDtime);
										if ( trackcount >= 1 && framecount >= 1 )
										{
											mAnimation = MEMALLOC_NEW(MeshImportAnimation)(mName, trackcount, framecount, duration, dtime );
										}
									}
								}
								if ( mAnimation )
								{
									mAnimTrack = mAnimation->GetTrack(mTrackIndex);
									mTrackIndex++;
								}
								break;
							case NT_SKELETON:
								{
									MEMALLOC_DELETE(MeshImportSkeleton,mSkeleton);
									mSkeleton = MEMALLOC_NEW(MeshImportSkeleton)("bip01");
								}
							case NT_BONE:
								if ( mSkeleton )
								{
									mBone = mSkeleton->GetBonePtr(mBoneIndex);
								}
								break;
						}
						Display(depth,"Node(ELEMENT): %s\n", value);
					}
				}
				break;
			case TiXmlNode::TEXT:
				Display(depth,"Node(TEXT): %s\n", value);
				switch ( mType )
				{
					case NT_ANIM_TRACK:
						if ( mAnimTrack )
						{
							mAnimTrack->SetName(mName);
							int count = atoi( mCount );
							if ( count == mAnimTrack->GetFrameCount() )
							{
								float *buff = (float *) ::malloc(sizeof(float)*7*count);
								Asc2Bin(svalue, count, "fff ffff", buff );
								for (int i=0; i<count; i++)
								{
									MeshImportAnimPose *p = mAnimTrack->GetPose(i);
									const float *src = &buff[i*7];
									p->mPos[0]  = src[0];
									p->mPos[1]  = src[1];
									p->mPos[2]  = src[2];
									p->mQuat[0] = src[3];
									p->mQuat[1] = src[4];
									p->mQuat[2] = src[5];
									p->mQuat[3] = src[6];
								}

							}
						}
						break;
					case NT_NODE_INSTANCE:
						if ( mName )
						{
							float transform[4*4];
							Asc2Bin(svalue, 4, "ffff", transform );
              MeshImportBone b;
              b.SetTransform(transform);
              float pos[3];
              float quat[3];
              float scale[3] = { 1, 1, 1 };
              b.ExtractOrientation(quat);
              b.GetPos(pos);
							mCallback->importMeshInstance(mName,pos,quat,scale);
							mName = 0;
						}
						break;
					case NT_NODE_TRIANGLE:
						if ( mCtype && mSemantic )
						{
							if ( stricmp(mSemantic,"position normal texcoord texcoord blendweights blendindices") == 0 )
							{
                tempVertex tvtx[3];
								MeshImportVertex vtx[3];

								Asc2Bin(svalue, 3, mCtype, tvtx );

                getVertex(tvtx[0],vtx[0]);
                getVertex(tvtx[1],vtx[1]);
                getVertex(tvtx[2],vtx[2]);

                mCallback->importTriangle(MIVF_POSITION | MIVF_NORMAL | MIVF_TEXEL1 | MIVF_TEXEL2 | MIVF_BONE_WEIGHTING,vtx);

							}
							else if ( stricmp(mSemantic,"position normal texcoord texcoord") == 0 )
							{
								MeshImportVertex vtx[3];
								float buffer[10*3];
								Asc2Bin(svalue, 3, mCtype, buffer );
								getVertex(buffer,vtx[0]);
								getVertex(&buffer[10],vtx[1]);
								getVertex(&buffer[20],vtx[2]);

                mCallback->importTriangle(MIVF_POSITION | MIVF_NORMAL | MIVF_TEXEL1 | MIVF_TEXEL2,vtx);
							}
							mCtype = 0;
							mSemantic = 0;

						}
						break;
					case NT_VERTEX_BUFFER:
						if ( mCtype && mCount )
						{
							mVertexCount  = atoi(mCount);
							if ( mVertexCount > 0 )
							{
								mVertexBuffer = Asc2Bin(svalue, mVertexCount, mCtype, 0 );
							}
							mCtype = 0;
							mCount = 0;
						}
						break;
					case NT_INDEX_BUFFER:
						if ( mCount )
						{
							mIndexCount = atoi(mCount);
							if ( mIndexCount > 0 )
							{
								mIndexBuffer = Asc2Bin(svalue, mIndexCount, "ddd", 0 );
							}
						}

						if ( mIndexBuffer && mVertexBuffer )
						{
							if ( stricmp(mSemantic,"position normal texcoord texcoord blendweights blendindices") == 0 )
							{
								MeshImportVertex *vtx = MEMALLOC_NEW_ARRAY(MeshImportVertex,mVertexCount)[mVertexCount];
								GeometryDeformVertex            *source = (GeometryDeformVertex *) mVertexBuffer;
								MeshImportVertex *dest = vtx;

								for (int i=0; i<mVertexCount; i++)
								{

									dest->mPos[0]     = source->mPos[0];
									dest->mPos[1]     = source->mPos[1];
									dest->mPos[2]     = source->mPos[2];

									dest->mNormal[0]  = source->mNormal[0];
									dest->mNormal[1]  = source->mNormal[1];
									dest->mNormal[2]  = source->mNormal[2];

									dest->mTexel1[0]  = source->mTexel1[0];
									dest->mTexel1[1]  = source->mTexel1[1];

									dest->mTexel2[0]  = source->mTexel2[0];
									dest->mTexel2[1]  = source->mTexel2[1];

									dest->mWeight[0] = source->mWeight[0];
									dest->mWeight[1] = source->mWeight[1];
									dest->mWeight[2] = source->mWeight[2];
									dest->mWeight[3] = source->mWeight[3];

									dest->mBone[0]   = source->mBone[0];
									dest->mBone[1]   = source->mBone[1];
									dest->mBone[2]   = source->mBone[2];
									dest->mBone[3]   = source->mBone[3];

									dest++;
									source++;

								}

								unsigned int *idx = (unsigned int *) mIndexBuffer;

								for (int i=0; i<mIndexCount; i++)
								{
                  MeshImportVertex v[3];
                  v[0] = vtx[*idx++];
                  v[1] = vtx[*idx++];
                  v[2] = vtx[*idx++];
									mCallback->importTriangle(MIVF_POSITION | MIVF_NORMAL | MIVF_TEXEL1 | MIVF_TEXEL2 | MIVF_BONE_WEIGHTING,v);

								}
                MEMALLOC_DELETE_ARRAY(MeshImportVertex,vtx);
							}
							else if ( stricmp(mSemantic,"position blendweights blendindices normal texcoord tangent binormal") == 0 )
							{
								MeshImportVertex *vtx = MEMALLOC_NEW_ARRAY(MeshImportVertex,mVertexCount)[mVertexCount];
								MaxVertex            *source = (MaxVertex *) mVertexBuffer;
								MeshImportVertex *dest = vtx;

								for (int i=0; i<mVertexCount; i++)
								{

									dest->mPos[0]     = source->mPos[0];
									dest->mPos[1]     = source->mPos[1];
									dest->mPos[2]     = source->mPos[2];

									dest->mNormal[0]  = source->mNormal[0];
									dest->mNormal[1]  = source->mNormal[1];
									dest->mNormal[2]  = source->mNormal[2];

									dest->mTangent[0] = source->mTangent[0];
									dest->mTangent[1] = source->mTangent[1];
									dest->mTangent[2] = source->mTangent[2];

									dest->mBiNormal[0] = source->mBinormal[0];
									dest->mBiNormal[1] = source->mBinormal[1];
									dest->mBiNormal[2] = source->mBinormal[2];

									dest->mTexel1[0]  = source->mTexel[0];
									dest->mTexel1[1]  = source->mTexel[1];

									dest->mTexel2[0]  = source->mTexel[0];
									dest->mTexel2[1]  = source->mTexel[1];

									dest->mWeight[0] = source->mWeight[0];
									dest->mWeight[1] = source->mWeight[1];
									dest->mWeight[2] = source->mWeight[2];
									dest->mWeight[3] = source->mWeight[3];

									dest->mBone[0]   = (unsigned short) source->mBone[0];
									dest->mBone[1]   = (unsigned short) source->mBone[1];
									dest->mBone[2]   = (unsigned short) source->mBone[2];
									dest->mBone[3]   = (unsigned short) source->mBone[3];

									dest++;
									source++;

								}

								unsigned int *idx = (unsigned int *) mIndexBuffer;

								for (int i=0; i<mIndexCount; i++)
								{
                  MeshImportVertex v[3];
                  v[0] = vtx[*idx++];
                  v[1] = vtx[*idx++];
                  v[2] = vtx[*idx++];
									mCallback->importTriangle(MIVF_ALL,v);

								}
                MEMALLOC_DELETE_ARRAY(MeshImportVertex,vtx);
							}
							else if ( stricmp(mSemantic,"position normal texcoord texcoord") == 0 )
							{
								MeshImportVertex *vtx = MEMALLOC_NEW_ARRAY(MeshImportVertex,mVertexCount)[mVertexCount];
								GeometryVertex            *source = (GeometryVertex *) mVertexBuffer;
								MeshImportVertex *dest = vtx;

								for (int i=0; i<mVertexCount; i++)
								{

									dest->mPos[0]     = source->mPos[0];
									dest->mPos[1]     = source->mPos[1];
									dest->mPos[2]     = source->mPos[2];

									dest->mNormal[0]  = source->mNormal[0];
									dest->mNormal[1]  = source->mNormal[1];
									dest->mNormal[2]  = source->mNormal[2];


									dest->mTexel1[0]  = source->mTexel1[0];
									dest->mTexel1[1]  = source->mTexel1[1];

									dest->mTexel2[0]  = source->mTexel2[0];
									dest->mTexel2[1]  = source->mTexel2[1];


									dest++;
									source++;

								}

								unsigned int *idx = (unsigned int *) mIndexBuffer;

								for (int i=0; i<mIndexCount; i++)
								{
									MeshImportVertex v[3];
                  v[0] = vtx[*idx++];
									v[1] = vtx[*idx++];
									v[2] = vtx[*idx++];
									mCallback->importTriangle(MIVF_POSITION | MIVF_NORMAL | MIVF_TEXEL1 | MIVF_TEXEL2,v);
								}
                MEMALLOC_DELETE_ARRAY(MeshImportVertex,vtx);
							}
						}

						::free( mIndexBuffer);
			      ::free( mVertexBuffer);

						mIndexBuffer = 0;
						mVertexBuffer = 0;
						mIndexCount = 0;
						mVertexCount = 0;
						break;
				}
				break;
			case TiXmlNode::COMMENT:
				Display(depth,"Node(COMMENT): %s\n", value);
				break;
			case TiXmlNode::DECLARATION:
				Display(depth,"Node(DECLARATION): %s\n", value);
				break;
			case TiXmlNode::UNKNOWN:
				Display(depth,"Node(UNKNOWN): %s\n", value);
				break;
			default:
				Display(depth,"Node(?????): %s\n", value);
				break;
		}
	}

	void ProcessAttribute(int         /* ntype */,          // enumerated type of the node
												const char * /* nvalue */, // The node value / key
												int         depth,          // how deeply nested we are in the XML hierachy
												const char *aname,  // the name of the attribute
												const char *savalue) // the value of the attribute
	{
		char avalue[43];

		avalue[39] = '.';
		avalue[40] = '.';
		avalue[41] = '.';
		avalue[42] = 0;

		strncpy(avalue,savalue,39);
		Display(depth,"  ### Attribute(%s,%s)\n", aname, avalue );

		AttributeType attrib = (AttributeType) mToAttribute.Get(aname);
		switch ( attrib )
		{
			case AT_POSITION:
				if ( mType == NT_BONE && mBone )
				{
					Asc2Bin(savalue,1,"fff", mBone->mPosition );
					mBone->ComposeTransform();
					mBoneIndex++;

					if ( mBoneIndex == mSkeleton->GetBoneCount() )
					{
						mCallback->importSkeleton(*mSkeleton);
						MEMALLOC_DELETE(MeshImportSkeleton,mSkeleton);
						mSkeleton = 0;
						mBoneIndex = 0;
					}

				}
				break;
			case AT_ORIENTATION:
				if ( mType == NT_BONE && mBone )
				{
					Asc2Bin(savalue,1,"ffff", mBone->mOrientation );
				}
				break;

			case AT_DURATION:
				mDuration = savalue;
				break;

			case AT_DTIME:
				mDtime = savalue;
				break;

			case AT_TRACK_COUNT:
				mTrackCount = savalue;
				break;

			case AT_FRAME_COUNT:
				mFrameCount = savalue;
				break;

			case AT_NAME:
				mName = savalue;

				switch ( mType )
				{
					case NT_MESH:
						mCallback->createMeshRef(savalue);
						break;
					case NT_SKELETON:
						if ( mSkeleton )
						{
							mSkeleton->SetName(savalue);
						}
						break;
					case NT_BONE:
						if ( mBone )
						{
							mBone->SetName(savalue);
						}
						break;
				}
				break;
			case AT_COUNT:
				mCount = savalue;
				if ( mType == NT_SKELETON )
				{
					if ( mSkeleton )
					{
						int count = atoi( savalue );
						if ( count > 0 )
						{
							MeshImportBone *bones;
							bones = MEMALLOC_NEW_ARRAY(MeshImportBone,count)[count];
							mSkeleton->SetBones(count,bones);
							mBoneIndex = 0;
						}
					}
				}
				break;
			case AT_PARENT:
				mParent = savalue;
				if ( mBone )
				{
					for (int i=0; i<mBoneIndex; i++)
					{
						const MeshImportBone &b = mSkeleton->GetBone(i);
						if ( strcmp(mParent,b.mName) == 0 )
						{
							mBone->mParentIndex = i;
							break;
						}
					}
				}
				break;
			case AT_MATERIAL:
				if ( mType == NT_MESH_SECTION )
				{
					mCallback->importMaterial(savalue,0);
				}
				break;
			case AT_CTYPE:
				mCtype = savalue;
				break;
			case AT_SEMANTIC:
				mSemantic = savalue;
				break;
		}

	}

	void Display(int /* depth */,const char * /* fmt */,...)
	{
#if DEBUG_LOG
		for (int i=0; i<depth; i++)
		{
			printf("  ");
		}
		char wbuff[8192];
		vsnprintf(wbuff, 8191, fmt, (char *)(&fmt+1));
		printf("%s", wbuff);
#endif
	}





private:
  MeshImportInterface     *mCallback;

	StringTableInt         mToElement;         // convert string to element enumeration.
	StringTableInt         mToAttribute;       // convert string to attribute enumeration
	NodeType               mType;

	const char * mName;
	const char * mCount;
	const char * mParent;
	const char * mCtype;
	const char * mSemantic;

	const char * mFrameCount;
	const char * mDuration;
	const char * mTrackCount;
	const char * mDtime;

	int          mTrackIndex;
	int          mBoneIndex;
	MeshImportBone       * mBone;

	MeshImportAnimation  * mAnimation;
	MeshImportAnimTrack  * mAnimTrack;
	MeshImportSkeleton   * mSkeleton;
	int          mVertexCount;
	int          mIndexCount;
	void       * mVertexBuffer;
	void       * mIndexBuffer;


};


MeshImporter * createMeshImportEZM(void)
{
  MeshImportEZM *m = MEMALLOC_NEW(MeshImportEZM);
  return static_cast< MeshImporter *>(m);
}

void         releaseMeshImportEZM(MeshImporter *iface)
{
  MeshImportEZM *m = static_cast< MeshImportEZM *>(iface);
  MEMALLOC_DELETE(MeshImportEZM,m);
}

}; // end of namepsace