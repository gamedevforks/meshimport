#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "UserMemAlloc.h"
#include "FloatMath.h"
#include "MeshImport.h"
#include "UserMemAlloc.h"
#include "sutil.h"
#include "ImportARM.h"
#include "NxParamUtils.h"

#include "ConvexHullParams.h"
#include "RenderMeshAssetParameters.h"
#include "SubmeshParameters.h"
#include "VertexBufferParameters.h"
#include "ApexBasicIOSAssetParams.h"
#include "ClothingActorParamSchema.h"
#include "ClothingAssetParamSchema.h"
#include "ClothingGraphicalLodParamSchema.h"
#include "ClothingMaterialLibraryParamSchema.h"
#include "ClothingPhysicalMeshParamSchema.h"
#include "ApexDestructibleActorParamSchema.h"
#include "DestructibleAssetParameters.h"
#include "SurfaceTraceParameters.h"
#include "SurfaceTraceSetParameters.h"
#include "ApexEmitterActorParameters.h"
#include "ApexEmitterAssetParameters.h"
#include "EmitterGeomBoxParams.h"
#include "EmitterGeomExplicitParams.h"
#include "EmitterGeomSphereParams.h"
#include "EmitterGeomSphereShellParams.h"
#include "GroundEmitterActorParameters.h"
#include "GroundEmitterAssetParameters.h"
#include "ImpactEmitterActorParameters.h"
#include "ImpactEmitterAssetParameters.h"
#include "ApexExplosionAssetParams.h"
#include "ExplosionActorParameters.h"
#include "ExplosionEnvParameters.h"
#include "FieldBoundaryActorParameters.h"
#include "FieldBoundaryAssetParameters.h"
#include "ShapeBoxParams.h"
#include "ShapeCapsuleParams.h"
#include "ShapeConvexParams.h"
#include "ShapeSphereParams.h"
#include "ColorVsDensityModifierParams.h"
#include "ColorVsLifeModifierParams.h"
#include "IofxAssetParameters.h"
#include "OrientAlongVelocityModifierParams.h"
#include "RandomRotationModifierParams.h"
#include "RandomScaleModifierParams.h"
#include "RandomSubtextureModifierParams.h"
#include "RotationModifierParams.h"
#include "ScaleAlongVelocityModifierParams.h"
#include "ScaleVsCameraDistanceModifierParams.h"
#include "ScaleVsDensityModifierParams.h"
#include "ScaleVsLifeModifierParams.h"
#include "SimpleScaleModifierParams.h"
#include "SubtextureVsLifeModifierParams.h"
#include "ViewDirectionSortingModifierParams.h"
#include "NxFluidIosParameters.h"
#include "ApexWindAssetParameters.h"
#include "WindActorParameters.h"
#include "NxSerializer.h"
#include "PsUserAllocated.h"
#include "PxUserOutputStream.h"
#include "pxmemorybuffer.h"
#include "VertexFormatParameters.h"

#include "BufferF32x1.h"
#include "BufferF32x2.h"
#include "BufferF32x3.h"
#include "BufferF32x4.h"

#include "BufferU8x1.h"
#include "BufferU8x2.h"
#include "BufferU8x3.h"
#include "BufferU8x4.h"

#include "BufferU16x1.h"
#include "BufferU16x2.h"
#include "BufferU16x3.h"
#include "BufferU16x4.h"

#include "BufferU32x1.h"
#include "BufferU32x2.h"
#include "BufferU32x3.h"
#include "BufferU32x4.h"
#pragma warning(disable:4996)

using namespace physx;
using namespace NxParameterized;

namespace NVSHARE
{
	/**
	\brief Potential semantics of a vertex buffer
	*/
	struct NxRenderVertexSemantic
	{
		enum Enum
		{
			CUSTOM = -1,			//!< User-defined

			POSITION = 0,			//!< Position of vertex
			NORMAL,					//!< Normal at vertex
			TANGENT,				//!< Tangent at vertex
			BINORMAL,				//!< Binormal at vertex
			COLOR,					//!< Color at vertex
			TEXCOORD0,				//!< Texture coord 0 of vertex
			TEXCOORD1,				//!< Texture coord 1 of vertex
			TEXCOORD2,				//!< Texture coord 2 of vertex
			TEXCOORD3,				//!< Texture coord 3 of vertex
			BONE_INDEX,				//!< Bone index of vertex
			BONE_WEIGHT,			//!< Bone weight of vertex

			NUM_SEMANTICS			//!< Count of standard semantics, not a valid semantic
		};
	};


	/**
	\brief Enumeration of possible formats of various buffer semantics

	N.B.: DO NOT CHANGE THE VALUES OF OLD FORMATS.
	*/
	struct NxRenderDataFormat
	{
		/** \brief the enum type */
		enum Enum
		{
			UNSPECIFIED =			0,	//!< No format (semantic not used)

			//!< Integer formats
			UBYTE1 =				1,	//!< One unsigned 8-bit integer (PxU8[1])
			UBYTE2 =				2,	//!< Two unsigned 8-bit integers (PxU8[2])
			UBYTE3 =				3,	//!< Three unsigned 8-bit integers (PxU8[3])
			UBYTE4 =				4,	//!< Four unsigned 8-bit integers (PxU8[4])

			USHORT1 =				5,	//!< One unsigned 16-bit integer (PxU16[1])
			USHORT2 =				6,	//!< Two unsigned 16-bit integers (PxU16[2])
			USHORT3 =				7,	//!< Three unsigned 16-bit integers (PxU16[3])
			USHORT4 =				8,	//!< Four unsigned 16-bit integers (PxU16[4])

			SHORT1 =				9,	//!< One signed 16-bit integer (PxI16[1])
			SHORT2 =				10,	//!< Two signed 16-bit integers (PxI16[2])
			SHORT3 =				11,	//!< Three signed 16-bit integers (PxI16[3])
			SHORT4 =				12,	//!< Four signed 16-bit integers (PxI16[4])

			UINT1 =					13,	//!< One unsigned integer (PxU32[1])
			UINT2 =					14,	//!< Two unsigned integers (PxU32[2])
			UINT3 =					15,	//!< Three unsigned integers (PxU32[3])
			UINT4 =					16,	//!< Four unsigned integers (PxU32[4])

			//!< Color formats
			R8G8B8A8 =				17,	//!< Four unsigned bytes (PxU8[4]) representing red, green, blue, alpha
			B8G8R8A8 =				18,	//!< Four unsigned bytes (PxU8[4]) representing blue, green, red, alpha
			R32G32B32A32_FLOAT =	19,	//!< Four floats (PxF32[4]) representing red, green, blue, alpha
			B32G32R32A32_FLOAT =	20,	//!< Four floats (PxF32[4]) representing blue, green, red, alpha

			//!< Normalized formats
			BYTE_UNORM1 =			21,	//!< One unsigned normalized value in the range [0,1], packed into 8 bits (PxU8[1])
			BYTE_UNORM2 =			22,	//!< Two unsigned normalized value in the range [0,1], each packed into 8 bits (PxU8[2])
			BYTE_UNORM3 =			23,	//!< Three unsigned normalized value in the range [0,1], each packed into bits (PxU8[3])
			BYTE_UNORM4 =			24,	//!< Four unsigned normalized value in the range [0,1], each packed into 8 bits (PxU8[4])

			SHORT_UNORM1 =			25,	//!< One unsigned normalized value in the range [0,1], packed into 16 bits (PxU16[1])
			SHORT_UNORM2 =			26,	//!< Two unsigned normalized value in the range [0,1], each packed into 16 bits (PxU16[2])
			SHORT_UNORM3 =			27,	//!< Three unsigned normalized value in the range [0,1], each packed into 16 bits (PxU16[3])
			SHORT_UNORM4 =			28,	//!< Four unsigned normalized value in the range [0,1], each packed into 16 bits (PxU16[4])

			BYTE_SNORM1 =			29,	//!< One signed normalized value in the range [-1,1], packed into 8 bits (PxU8[1])
			BYTE_SNORM2 =			30,	//!< Two signed normalized value in the range [-1,1], each packed into 8 bits (PxU8[2])
			BYTE_SNORM3 =			31,	//!< Three signed normalized value in the range [-1,1], each packed into bits (PxU8[3])
			BYTE_SNORM4 =			32,	//!< Four signed normalized value in the range [-1,1], each packed into 8 bits (PxU8[4])

			SHORT_SNORM1 =			33,	//!< One signed normalized value in the range [-1,1], packed into 16 bits (PxU16[1])
			SHORT_SNORM2 =			34,	//!< Two signed normalized value in the range [-1,1], each packed into 16 bits (PxU16[2])
			SHORT_SNORM3 =			35,	//!< Three signed normalized value in the range [-1,1], each packed into 16 bits (PxU16[3])
			SHORT_SNORM4 =			36,	//!< Four signed normalized value in the range [-1,1], each packed into 16 bits (PxU16[4])

			//!< Float formats
			HALF1 =					37,	//!< One 16-bit floating point value
			HALF2 =					38,	//!< Two 16-bit floating point values
			HALF3 =					39,	//!< Three 16-bit floating point values
			HALF4 =					40,	//!< Four 16-bit floating point values

			FLOAT1 =				41,	//!< One 32-bit floating point value
			FLOAT2 =				42,	//!< Two 32-bit floating point values
			FLOAT3 =				43,	//!< Three 32-bit floating point values
			FLOAT4 =				44,	//!< Four 32-bit floating point values

			FLOAT3x4 =				45,	//!< A 3x4 matrix (see PxMat34)
			FLOAT3x3 =				46,	//!< A 3x3 matrix (see PxMat33)
			QUAT =					47,	//!< A quaternion (see PxQuat)
		};

		/// Get byte size of format type
		static PX_INLINE physx::PxU32 getFormatDataSize( Enum format )
		{
			switch( format )
			{
			default:
				PX_ALWAYS_ASSERT();
			case UNSPECIFIED:
				return 0;

			case UBYTE1:
			case BYTE_UNORM1:
			case BYTE_SNORM1:
				return sizeof(physx::PxU8);
			case UBYTE2:
			case BYTE_UNORM2:
			case BYTE_SNORM2:
				return sizeof(physx::PxU8) * 2;
			case UBYTE3:
			case BYTE_UNORM3:
			case BYTE_SNORM3:
				return sizeof(physx::PxU8) * 3;
			case UBYTE4:
			case BYTE_UNORM4:
			case BYTE_SNORM4:
				return sizeof(physx::PxU8) * 4;

			case USHORT1:
			case SHORT1:
			case HALF1:
			case SHORT_UNORM1:
			case SHORT_SNORM1:
				return sizeof(physx::PxU16);
			case USHORT2:
			case SHORT2:
			case HALF2:
			case SHORT_UNORM2:
			case SHORT_SNORM2:
				return sizeof(physx::PxU16) * 2;
			case USHORT3:
			case SHORT3:
			case HALF3:
			case SHORT_UNORM3:
			case SHORT_SNORM3:
				return sizeof(physx::PxU16) * 3;
			case USHORT4:
			case SHORT4:
			case HALF4:
			case SHORT_UNORM4:
			case SHORT_SNORM4:
				return sizeof(physx::PxU16) * 4;

			case UINT1:
				return sizeof(physx::PxU32);
			case UINT2:
				return sizeof(physx::PxU32) * 2;
			case UINT3:
				return sizeof(physx::PxU32) * 3;
			case UINT4:
				return sizeof(physx::PxU32) * 4;

			case R8G8B8A8:
			case B8G8R8A8:
				return sizeof(physx::PxU8) * 4;

			case R32G32B32A32_FLOAT:
			case B32G32R32A32_FLOAT:
				return sizeof(physx::PxF32) * 4;

			case FLOAT1:
				return sizeof(physx::PxF32);
			case FLOAT2:
				return sizeof(physx::PxF32) * 2;
			case FLOAT3:
				return sizeof(physx::PxF32) * 3;
			case FLOAT4:
				return sizeof(physx::PxF32) * 4;

			case FLOAT3x4:
				return sizeof(physx::PxMat34Legacy);

			case FLOAT3x3:
				return sizeof(physx::PxMat33);

			case QUAT:
				return sizeof(physx::PxQuat);
			}
		}
	};



class AppUserAllocator : public physx::PxUserAllocator
{
public:

	virtual void* allocate(size_t size, physx::PxU32 , const char* , int )
	{
		return ::malloc(size);
	}

	virtual void deallocate(void* ptr)
	{
		::free(ptr);
	}

};


class AppUserOutputStream : public physx::PxUserOutputStream
{
public:
	virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		printf("%s : %s : %d\r\n", message, file, line );
	}

};

struct ParamFactory : public UserAllocated
{
	ParamFactory(const char *name,NxParameterized::Factory *f)
	{
		mName = name;
		mFactory = f;
	}
	const char *mName;
	NxParameterized::Factory *mFactory;
};

typedef Array< ParamFactory >  ParamFactoryVector;

class AppTraits : public NxParameterized::Traits
{
public:

	virtual void registerFactory(NxParameterized::Factory &ptr)
	{
		ParamFactory p(ptr.getClassName(),&ptr);
		mFactories.pushBack(p);
	}

	virtual NxParameterized::Factory * removeFactory( const char * name )
	{
		PX_ALWAYS_ASSERT(); // not expected/implemented
		return NULL;
	}

	virtual NxParameterized::Factory * removeFactory( const char * name,physx::PxU32 versionNumber )
	{
		PX_ALWAYS_ASSERT(); // not expected/implemented
		return NULL;
	}

	virtual NxParameterized::Interface * createNxParameterized( const char * name )
	{
		NxParameterized::Interface *ret = NULL;

		NxParameterized::Factory *f = getFactory(name);
		PX_ASSERT(f);
		if ( f )
		{
			ret = f->create(this);
		}

		return ret;
	}

	virtual NxParameterized::Interface * createNxParameterized( const char * name, physx::PxU32 ver ) 
	{
		PX_ALWAYS_ASSERT();
		return NULL;
	}

	virtual NxParameterized::Interface * finishNxParameterized( const char * name, void *ptr, void *memoryStart, physx::PxI32 *pRefCount )
	{
		NxParameterized::Interface *ret = NULL;

		NxParameterized::Factory *f = getFactory(name);
		PX_ASSERT(f);
		if ( f )
		{
			ret = f->finish(this,ptr,memoryStart,pRefCount);
		}


		return ret;
	}

	virtual NxParameterized::Interface * finishNxParameterized( const char * name, physx::PxU32 ver, void *obj, void *buf, physx::PxI32 *refCount ) 
	{
		PX_ALWAYS_ASSERT();
		return NULL;
	}

	virtual physx::PxU32 getCurrentVersion(const char *className) const 
	{
		PX_ALWAYS_ASSERT();
		return 1;
	}

	NxParameterized::Factory * getFactory(const char *name)
	{
		NxParameterized::Factory *ret = NULL;
		for (ParamFactoryVector::Iterator i=mFactories.begin(); i!=mFactories.end(); ++i)
		{
			ParamFactory &p = (*i);
			if ( strcmp(p.mName,name) == 0 )
			{
				ret = p.mFactory; 
				break;
			}
		}
		return ret;
	}

	virtual bool getNxParameterizedNames( const char ** names, physx::PxU32 &outCount, physx::PxU32 inCount) const
	{
		bool ret = false;
		return ret;
	}

	virtual void *alloc(physx::PxU32 nbytes)
	{
		return ::malloc(nbytes);
	}

    virtual void free(void *buf)
    {
    	::free(buf);
    }

	virtual physx::PxI32 incRefCount(physx::PxI32 *pRefCount)
	{
		*pRefCount++;
		return *pRefCount;
	}

	virtual physx::PxI32 decRefCount(physx::PxI32 *pRefCount)
	{
		*pRefCount--;
		return *pRefCount;
	}

	ParamFactoryVector mFactories;

};

typedef Array< NxParameterized::Interface * > InterfaceVector;


class MeshImporterARM : public MeshImporter, public Memalloc, public AppTraits
{
public:
	MeshImporterARM(void)
	{
		registerFactory(mNxApexBasicIOSAssetParamFactory);
		registerFactory(mNxApexClothingActorParamFactory);
		registerFactory(mClothingAssetParametersFactory);
		registerFactory(mClothingGraphicalLodParametersFactory);
		registerFactory(mClothingMaterialLibraryParametersFactory);
		registerFactory(mClothingPhysicalMeshParametersFactory);
		registerFactory(mConvexHullParametersFactory);
		registerFactory(mNxApexDestructibleActorParamFactory);
		registerFactory(mDestructibleAssetParametersFactory);
		registerFactory(mSurfaceTraceParametersFactory);
		registerFactory(mSurfaceTraceSetParametersFactory);
		registerFactory(mApexEmitterActorParametersFactory);
		registerFactory(mApexEmitterAssetParametersFactory);
		registerFactory(mEmitterGeomBoxParamsFactory);
		registerFactory(mEmitterGeomExplicitParamsFactory);
		registerFactory(mEmitterGeomSphereParamsFactory);
		registerFactory(mEmitterGeomSphereShellParamsFactory);
		registerFactory(mGroundEmitterActorParametersFactory);
		registerFactory(mGroundEmitterAssetParametersFactory);
		registerFactory(mImpactEmitterActorParametersFactory);
		registerFactory(mImpactEmitterAssetParametersFactory);
		registerFactory(mNxApexExplosionAssetParamFactory);
		registerFactory(mExplosionActorParametersFactory);
		registerFactory(mExplosionEnvParametersFactory);
		registerFactory(mFieldBoundaryActorParametersFactory);
		registerFactory(mFieldBoundaryAssetParametersFactory);
		registerFactory(mShapeBoxParamsFactory);
		registerFactory(mShapeCapsuleParamsFactory);
		registerFactory(mShapeConvexParamsFactory);
		registerFactory(mShapeSphereParamsFactory);
		registerFactory(mRenderMeshAssetParametersFactory);
		registerFactory(mSubmeshParametersFactory);
		registerFactory(mVertexBufferParametersFactory);
		registerFactory(mColorVsDensityModifierParamsFactory);
		registerFactory(mColorVsLifeModifierParamsFactory);
		registerFactory(mIofxAssetParametersFactory);
		registerFactory(mOrientAlongVelocityModifierParamsFactory);
		registerFactory(mRandomRotationModifierParamsFactory);
		registerFactory(mRandomScaleModifierParamsFactory);
		registerFactory(mRandomSubtextureModifierParamsFactory);
		registerFactory(mRotationModifierParamsFactory);
		registerFactory(mScaleAlongVelocityModifierParamsFactory);
		registerFactory(mScaleVsCameraDistanceModifierParamsFactory);
		registerFactory(mScaleVsDensityModifierParamsFactory);
		registerFactory(mScaleVsLifeModifierParamsFactory);
		registerFactory(mSimpleScaleModifierParamsFactory);
		registerFactory(mSubtextureVsLifeModifierParamsFactory);
		registerFactory(mViewDirectionSortingModifierParamsFactory);
		registerFactory(mNxFluidIosParametersFactory);
		registerFactory(mNxApexWindAssetParamFactory);
		registerFactory(mWindActorParametersFactory);
		registerFactory(mVertexFormatParametersFactory);

		registerFactory(mBufferF32x1Factory);
		registerFactory(mBufferF32x2Factory);
		registerFactory(mBufferF32x3Factory);
		registerFactory(mBufferF32x4Factory);

		registerFactory(mBufferU8x1Factory);
		registerFactory(mBufferU8x2Factory);
		registerFactory(mBufferU8x3Factory);
		registerFactory(mBufferU8x4Factory);

		registerFactory(mBufferU16x1Factory);
		registerFactory(mBufferU16x2Factory);
		registerFactory(mBufferU16x3Factory);
		registerFactory(mBufferU16x4Factory);

		registerFactory(mBufferU32x1Factory);
		registerFactory(mBufferU32x2Factory);
		registerFactory(mBufferU32x3Factory);
		registerFactory(mBufferU32x4Factory);

	}
  	virtual NxI32              getExtensionCount(void) { return 2; }; // most importers support just one file name extension.

  	virtual const char *     getExtension(NxI32 index)  // report the default file name extension for this mesh type.
  	{
  		return index == 0 ? ".apx" : ".apb";
	}

  	virtual const char *     getDescription(NxI32 index) // report the ascii description of the import type.
  	{
  		return index == 0 ? "APEX Render Mesh XML" : "APEX Render Mesh Binary";
	}

  	virtual bool             importMesh(const char *meshName,const void *data,NxU32 dlen,MeshImportInterface *callback,const char *options,MeshImportApplicationResource *appResource)
  	{
  		bool ret = false;

		PxMemoryBuffer mb(data,dlen);
		NxParameterized::NxSerializer *s = internalCreateNxSerializer(NxSerializer::NST_XML,this);
		NxSerializer::DeserializedData desdata;
		NxSerializer::ErrorType err = s->deserialize(mb,desdata);
		if ( err == NxSerializer::ERROR_NONE )
		{
			for (PxU32 i=0; i<desdata.size(); i++)
			{
				NxParameterized::Interface *iface = desdata[i];
				PxU32 count;
				const NxParameterized::ParamResult *result = getParamList(*iface,"RenderMeshAssetParameters",NULL,count,true,true,this);
				if ( result )
				{
					for (PxU32 i=0; i<count; i++)
					{
						importRenderMeshAsset(meshName, (NxParameterized::Interface *)result[i].mHandle.getInterface(), callback );
						ret = true;
					}
					releaseParamList(count,result,this);
				}
			}
		}
		s->release();

  		return ret;
	}

	void importRenderMeshAsset(const char *meshName,NxParameterized::Interface *renderMesh,MeshImportInterface *callback)
	{
		if ( renderMesh )
		{
			NxParameterized::Handle handle(*renderMesh);
			NxParameterized::Interface *iface = NxParameterized::findParam(*renderMesh,"submeshes",handle);
			if ( iface )
			{
				physx::PxI32 size=0;
				if ( handle.getArraySize(size,0) == ::NxParameterized::ERROR_NONE )
				{
					for (physx::PxI32 i=0; i<size; i++)
					{
						handle.set(i);
						NxParameterized::Interface *paramPtr = 0;
						handle.getParamRef(paramPtr);
						if ( paramPtr )
						{
							const char *materialName = "material";
							char scratch[512];
							NxParameterized::Handle materialHandle(*renderMesh);
							physx::string::sprintf_s(scratch,512,"materialNames[%d]",i);
							NxParameterized::Interface *mat = NxParameterized::findParam(*renderMesh,scratch,materialHandle);
							PX_ASSERT(mat);
							if ( mat )
							{
								materialHandle.getParamString(materialName);
							}
							importSubMesh(meshName, paramPtr, callback, materialName );
						}

						handle.popIndex();
					}
				}
			}
		}
	}

	void getBufferVec3(const void *_src,physx::PxU32 stride,NxParameterized::Interface *subMesh,physx::PxU32 vcount,physx::PxU32 index)
	{
		const physx::PxU8 *src = (physx::PxU8 *)_src;
		NxParameterized::Handle handle(*subMesh);
		char scratch[512];
		physx::string::sprintf_s(scratch,512,"vertexBuffer.buffers[%d]",index);
		NxParameterized::Interface *buffer = NxParameterized::findParam(*subMesh,scratch,handle);
		PX_ASSERT(buffer);
		if ( buffer )
		{
			NxParameterized::Interface *paramPtr = this->createNxParameterized("BufferF32x3");
			handle.setParamRef(paramPtr);
			PX_ASSERT(paramPtr);
			if ( paramPtr )
			{
				NxParameterized::Handle bufferHandle(*paramPtr);
				NxParameterized::Interface *f = NxParameterized::findParam(*paramPtr,"data",bufferHandle);
				PX_ASSERT(f);
				if ( f )
				{
					if ( bufferHandle.resizeArray(vcount) == ::NxParameterized::ERROR_NONE )
					{
						for (physx::PxU32 i=0; i<vcount; i++)
						{
							bufferHandle.setParamVec3Array((const physx::PxVec3 *)src,1,i);
							src+=stride;
						}
					}
					else
					{
						PX_ALWAYS_ASSERT();
					}
				}
			}
		}
	}

	void getBufferVec2(const void *_src,physx::PxU32 stride,NxParameterized::Interface *subMesh,physx::PxU32 vcount,physx::PxU32 index)
	{
		const physx::PxU8 *src = (physx::PxU8 *)_src;
		NxParameterized::Handle handle(*subMesh);
		char scratch[512];
		physx::string::sprintf_s(scratch,512,"vertexBuffer.buffers[%d]",index);
		NxParameterized::Interface *buffer = NxParameterized::findParam(*subMesh,scratch,handle);
		PX_ASSERT(buffer);
		if ( buffer )
		{
			NxParameterized::Interface *paramPtr = this->createNxParameterized("BufferF32x2");
			handle.setParamRef(paramPtr);
			PX_ASSERT(paramPtr);
			if ( paramPtr )
			{
				NxParameterized::Handle bufferHandle(*paramPtr);
				NxParameterized::Interface *f = NxParameterized::findParam(*paramPtr,"data",bufferHandle);
				PX_ASSERT(f);
				if ( f )
				{
					if ( bufferHandle.resizeArray(vcount) == ::NxParameterized::ERROR_NONE )
					{
						NxParameterized::ErrorType err;
						for (physx::PxU32 i=0; i<vcount; i++)
						{
							err = bufferHandle.set(i);
							PX_ASSERT( err == NxParameterized::ERROR_NONE );

							const physx::PxF32 *coord = (const physx::PxF32 *)src;
							err = bufferHandle.set(0);
							PX_ASSERT( err == NxParameterized::ERROR_NONE );
							err = bufferHandle.setParamF32(coord[0]);
							PX_ASSERT( err == NxParameterized::ERROR_NONE );
							bufferHandle.popIndex();

							err = bufferHandle.set(1);
							PX_ASSERT( err == NxParameterized::ERROR_NONE );
							err = bufferHandle.setParamF32(coord[1]);
							PX_ASSERT( err == NxParameterized::ERROR_NONE );
							bufferHandle.popIndex();

							bufferHandle.popIndex();

							src+=stride;
						}
					}
					else
					{
						PX_ALWAYS_ASSERT();
					}
				}
			}
		}
	}



	void copyBufferVec3(void *_dest,physx::PxU32 stride,NxParameterized::Interface *subMesh,physx::PxU32 vcount,physx::PxU32 index)
	{
		physx::PxU8 *dest = (physx::PxU8 *)_dest;
		NxParameterized::Handle handle(*subMesh);
		char scratch[512];
		physx::string::sprintf_s(scratch,512,"vertexBuffer.buffers[%d]",index);
		NxParameterized::Interface *buffer = NxParameterized::findParam(*subMesh,scratch,handle);
		PX_ASSERT(buffer);
		if ( buffer )
		{
			NxParameterized::Interface *paramPtr = 0;
			handle.getParamRef(paramPtr);
			PX_ASSERT(paramPtr);
			if ( paramPtr )
			{
				NxParameterized::Handle bufferHandle(*paramPtr);
				NxParameterized::Interface *f = NxParameterized::findParam(*paramPtr,"data",bufferHandle);
				PX_ASSERT(f);
				if ( f )
				{
					physx::PxI32 bufferSize=0;
					if ( bufferHandle.getArraySize(bufferSize,0) == ::NxParameterized::ERROR_NONE )
					{
						PX_ASSERT( bufferSize == (physx::PxI32)vcount );
						if ( bufferSize == (physx::PxI32)vcount )
						{
							for (physx::PxU32 i=0; i<vcount; i++)
							{
								NxParameterized::ErrorType err = bufferHandle.getParamVec3Array((physx::PxVec3 *)dest,1,i);
								PX_FORCE_PARAMETER_REFERENCE(err);
								PX_ASSERT( err == NxParameterized::ERROR_NONE );
								dest+=stride;
							}
						}
					}
					else
					{
						PX_ALWAYS_ASSERT();
					}
				}
			}
		}
	}


	void copyBufferVec2(void *_dest,physx::PxU32 stride,NxParameterized::Interface *subMesh,physx::PxU32 vcount,physx::PxU32 index)
	{
		physx::PxU8 *dest = (physx::PxU8 *)_dest;
		NxParameterized::Handle handle(*subMesh);
		char scratch[512];
		physx::string::sprintf_s(scratch,512,"vertexBuffer.buffers[%d]",index);
		NxParameterized::Interface *buffer = NxParameterized::findParam(*subMesh,scratch,handle);
		PX_ASSERT(buffer);
		if ( buffer )
		{
			NxParameterized::Interface *paramPtr = 0;
			handle.getParamRef(paramPtr);
			PX_ASSERT(paramPtr);
			if ( paramPtr )
			{
				NxParameterized::Handle bufferHandle(*paramPtr);
				NxParameterized::Interface *f = NxParameterized::findParam(*paramPtr,"data",bufferHandle);
				PX_ASSERT(f);
				if ( f )
				{
					physx::PxI32 bufferSize=0;
					if ( bufferHandle.getArraySize(bufferSize,0) == ::NxParameterized::ERROR_NONE )
					{
						PX_ASSERT( bufferSize == (physx::PxI32)vcount );
						if ( bufferSize == (physx::PxI32)vcount )
						{
							NxParameterized::ErrorType err;
							for (physx::PxU32 i=0; i<vcount; i++)
							{
								bufferHandle.set(i);

								physx::PxF32 *texel = (physx::PxF32 *)dest;

								// 
								// read first float
								err = bufferHandle.set(0);
								PX_ASSERT( err == NxParameterized::ERROR_NONE );
								err = bufferHandle.getParamF32(texel[0]);
								PX_ASSERT( err == NxParameterized::ERROR_NONE );
								bufferHandle.popIndex();

								// read second float
								err = bufferHandle.set(1);
								PX_ASSERT( err == NxParameterized::ERROR_NONE );
								err = bufferHandle.getParamF32(texel[1]);
								PX_ASSERT( err == NxParameterized::ERROR_NONE );
								bufferHandle.popIndex();


								bufferHandle.popIndex();
								dest+=stride;
							}
						}
					}
					else
					{
						PX_ALWAYS_ASSERT();
					}
				}
			}
		}
	}


	void importSubMesh(const char *meshName,NxParameterized::Interface *subMesh,MeshImportInterface *callback,const char *materialName)
	{
		physx::PxU32 vcount;
		NxParameterized::Handle handle(*subMesh);

		if ( NxParameterized::getParamU32(*subMesh,"vertexBuffer.vertexCount",vcount) )
		{
			physx::PxU32 flags = 0;
			MeshVertex *vertices = new MeshVertex[vcount];
			NxParameterized::Interface *iface = NxParameterized::findParam(*subMesh,"vertexBuffer.vertexFormat.bufferFormats",handle);
			if ( iface )
			{
				physx::PxI32 size=0;
				if ( handle.getArraySize(size,0) == ::NxParameterized::ERROR_NONE )
				{
					for (physx::PxI32 i=0; i<size; i++)
					{
						physx::PxU32 format;
						char scratch[512];
						physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].name",i);
						NxParameterized::Interface *f = NxParameterized::findParam(*subMesh,scratch,handle);
						PX_ASSERT(f);
						if ( f )
						{
							const char *val;
							handle.getParamString(val);
							physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].format",i);
							f = NxParameterized::findParam(*subMesh,scratch,handle);
							PX_ASSERT(f);
							if ( f )
							{
								handle.getParamU32(format);
								// ready to copy the data now!
								if ( strcmp(val,"SEMANTIC_POSITION") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT3 );
									if ( format == NxRenderDataFormat::FLOAT3 )
									{
										flags|=MIVF_POSITION;
										copyBufferVec3(&vertices[0].mPos,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_NORMAL") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT3 );
									if ( format == NxRenderDataFormat::FLOAT3 )
									{
										flags|=MIVF_NORMAL;
										copyBufferVec3(&vertices[0].mNormal,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_TANGENT") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT3 );
									if ( format == NxRenderDataFormat::FLOAT3 )
									{
										flags|=MIVF_TANGENT;
										copyBufferVec3(&vertices[0].mTangent,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_BINORMAL") == 0  )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT3 );
									if ( format == NxRenderDataFormat::FLOAT3 )
									{
										flags|=MIVF_BINORMAL;
										copyBufferVec3(&vertices[0].mTangent,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_TEXCOORD0") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT2 );
									if ( format == NxRenderDataFormat::FLOAT2 )
									{
										flags|=MIVF_TEXEL1;
										copyBufferVec2(&vertices[0].mTexel1,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_TEXCOORD1") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT2 );
									if ( format == NxRenderDataFormat::FLOAT2 )
									{
										flags|=MIVF_TEXEL2;
										copyBufferVec2(&vertices[0].mTexel2,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_TEXCOORD2") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT2 );
									if ( format == NxRenderDataFormat::FLOAT2 )
									{
										flags|=MIVF_TEXEL3;
										copyBufferVec2(&vertices[0].mTexel3,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_TEXCOORD3") == 0 )
								{
									PX_ASSERT( format == NxRenderDataFormat::FLOAT2 );
									if ( format == NxRenderDataFormat::FLOAT2 )
									{
										flags|=MIVF_TEXEL4;
										copyBufferVec2(&vertices[0].mTexel4,sizeof(MeshVertex),subMesh,vcount,i);
									}
								}
								else if ( strcmp(val,"SEMANTIC_BONE_INDEX") == 0 )
								{
									printf("Not yet supported.\r\n");
								}
								else
								{
									printf("Format: %s not yet supported.\r\n", val );
								}
							}
						}

					}
				}
			}

			// ok..now grab the index buffer...
			iface = NxParameterized::findParam(*subMesh,"indexBuffer",handle);
			if ( iface )
			{

				physx::PxI32 size=0;
				if ( handle.getArraySize(size,0) == ::NxParameterized::ERROR_NONE )
				{
					physx::PxI32 tcount = size/3;
					for (physx::PxI32 i=0; i<tcount; i++)
					{
						physx::PxU32 tri[3];
						handle.getParamU32Array(tri,3,i*3);
						physx::PxU32 i1 = tri[0];
						physx::PxU32 i2 = tri[1];
						physx::PxU32 i3 = tri[2];

						PX_ASSERT( i1 < vcount );
						PX_ASSERT( i2 < vcount );
						PX_ASSERT( i3 < vcount );
						callback->importTriangle(meshName,materialName,flags,vertices[i1],vertices[i2],vertices[i3]);
					}
				}
			}
			delete []vertices;
		}
	}

	PX_INLINE physx::PxU32 hash( const char *string )
	{
		// "DJB" string hash 
		physx::PxU32 h = 5381;
		char c;
		while( (c = *string++) != '\0' )
		{
			h = ((h<<5)+h)^c;
		}
		return h;
	}


	void getVertexBuffers(NxParameterized::Interface *subMesh,const MeshVertex *vertices,physx::PxU32 vcount,physx::PxU32 vflags)
	{
		char *formatNames[256];
		NxRenderDataFormat::Enum formatTypes[256];
		physx::PxI32 semanticTypes[256];
		physx::PxU32 hashId[256];

		physx::PxU32 formatCount=0;
		if ( vflags & MIVF_POSITION ) 
		{
			formatNames[formatCount] = "SEMANTIC_POSITION";
			semanticTypes[formatCount] = NxRenderVertexSemantic::POSITION;
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT3;
			formatCount++;
		}

		if ( vflags & MIVF_NORMAL ) 
		{
			formatNames[formatCount] = "SEMANTIC_NORMAL";
			semanticTypes[formatCount] = NxRenderVertexSemantic::NORMAL;
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT3;
			formatCount++;
		}
		if ( vflags & MIVF_TANGENT ) 
		{
			formatNames[formatCount] = "SEMANTIC_TANGENT";
			semanticTypes[formatCount] = NxRenderVertexSemantic::TANGENT;
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT3;
			formatCount++;
		}
		if ( vflags & MIVF_BINORMAL ) 
		{
			formatNames[formatCount] = "SEMANTIC_BINORMAL";
			semanticTypes[formatCount] = NxRenderVertexSemantic::BINORMAL;
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT3;
			formatCount++;
		}
		if ( vflags & MIVF_TEXEL1 ) 
		{
			formatNames[formatCount] = "SEMANTIC_TEXCOORD0";
			semanticTypes[formatCount] = NxRenderVertexSemantic::TEXCOORD0;
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT2;
			formatCount++;
		}
		if ( vflags & MIVF_TEXEL2 ) 
		{
			formatNames[formatCount] = "SEMANTIC_TEXCOORD1";
			semanticTypes[formatCount] = NxRenderVertexSemantic::TEXCOORD1;
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT2;
			formatCount++;
		}
		if ( vflags & MIVF_TEXEL3 ) 
		{
			formatNames[formatCount] = "SEMANTIC_TEXCOORD2";
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT2;
			semanticTypes[formatCount] = NxRenderVertexSemantic::TEXCOORD2;
			formatCount++;
		}
		if ( vflags & MIVF_TEXEL4 ) 
		{
			formatNames[formatCount] = "SEMANTIC_TEXCOORD3";
			formatTypes[formatCount] = NxRenderDataFormat::FLOAT2;
			semanticTypes[formatCount] = NxRenderVertexSemantic::TEXCOORD3;
			formatCount++;
		}
		for (physx::PxU32 i=0; i<formatCount; i++)
		{
			hashId[i] = hash( formatNames[i] );
		}

		NxParameterized::Handle handle(*subMesh);
		NxParameterized::Interface *iface = NxParameterized::findParam(*subMesh,"vertexBuffer",handle);
		if ( iface )
		{
			NxParameterized::Interface *vbuff = this->createNxParameterized("VertexBufferParameters");
			PX_ASSERT(vbuff);
			if ( vbuff )
			{
				handle.setParamRef(vbuff);
			}
		}
		NxParameterized::setParamU32(*subMesh,"vertexBuffer.vertexCount",vcount);

		iface = NxParameterized::findParam(*subMesh,"vertexBuffer.vertexFormat",handle);
		if ( iface )
		{
			NxParameterized::Interface *vf = this->createNxParameterized("VertexFormatParameters");
			PX_ASSERT(vf);
			if ( vf )
			{
				handle.setParamRef(vf);
			}
		}

		iface = NxParameterized::findParam(*subMesh,"vertexBuffer.vertexFormat.bufferFormats",handle);
		if ( iface )
		{
			if ( handle.resizeArray(formatCount) == ::NxParameterized::ERROR_NONE )
			{
				for (physx::PxU32 i=0; i<formatCount; i++)
				{
					char scratch[512];
					physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].name",i);
					NxParameterized::Interface *f = NxParameterized::findParam(*subMesh,scratch,handle);
					PX_ASSERT(f);
					if ( f )
					{
						handle.setParamString(formatNames[i]);
					}
					physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].semantic",i);
					f = NxParameterized::findParam(*subMesh,scratch,handle);
					PX_ASSERT(f);
					if ( f )
					{
						handle.setParamI32( semanticTypes[i] );
					}
					physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].id",i);
					f = NxParameterized::findParam(*subMesh,scratch,handle);
					PX_ASSERT(f);
					if ( f )
					{
						handle.setParamU32(hashId[i]);
					}
					physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].format",i);
					f = NxParameterized::findParam(*subMesh,scratch,handle);
					PX_ASSERT(f);
					if ( f )
					{
						handle.setParamU32( formatTypes[i] );
					}
					physx::string::sprintf_s(scratch,512,"vertexBuffer.vertexFormat.bufferFormats[%d].serialize",i);
					f = NxParameterized::findParam(*subMesh,scratch,handle);
					PX_ASSERT(f);
					if ( f )
					{
						handle.setParamBool(true);
					}
				}
			}
		}

		NxParameterized::Interface *buffers = NxParameterized::findParam(*subMesh,"vertexBuffer.buffers",handle);
		if ( buffers )
		{
			handle.resizeArray(formatCount);
		}


		for (physx::PxU32 i=0; i<formatCount; i++)
		{
			switch ( semanticTypes[i] )
			{
				case NxRenderVertexSemantic::POSITION:
					getBufferVec3(&vertices[0].mPos,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::NORMAL:
					getBufferVec3(&vertices[0].mNormal,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::TANGENT:
					getBufferVec3(&vertices[0].mNormal,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::BINORMAL:
					getBufferVec3(&vertices[0].mNormal,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::TEXCOORD0:
					getBufferVec2(&vertices[0].mTexel1,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::TEXCOORD1:
					getBufferVec2(&vertices[0].mTexel2,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::TEXCOORD2:
					getBufferVec2(&vertices[0].mTexel3,sizeof(MeshVertex),subMesh,vcount,i);
					break;
				case NxRenderVertexSemantic::TEXCOORD3:
					getBufferVec2(&vertices[0].mTexel4,sizeof(MeshVertex),subMesh,vcount,i);
					break;



			}
		}
	}

	void getSubMesh(NxParameterized::Interface *subMesh,SubMesh *sm,Mesh *m)
	{
		NxParameterized::Handle handle(*subMesh);

		physx::PxU32 vcount=0;
		MeshVertex *vertices = new MeshVertex[m->mVertexCount];
		physx::PxU8 *indexRemapIn = new physx::PxU8[m->mVertexCount];
		memset(indexRemapIn,0,sizeof(physx::PxU8)*m->mVertexCount);

		physx::PxU32 *indexRemapOut = new physx::PxU32[m->mVertexCount];

		for (physx::PxU32 i=0; i<sm->mTriCount*3; i++)
		{
			physx::PxU32 index = sm->mIndices[i];
			if ( indexRemapIn[index] == 0 )
			{
				indexRemapIn[index] = 1; // represented
				indexRemapOut[index] = vcount;
				vertices[vcount] = m->mVertices[index];
				vcount++;
			}
		}

		getVertexBuffers(subMesh,vertices,vcount,m->mVertexFlags);

		delete []indexRemapIn;

		NxParameterized::Interface *iface = NxParameterized::findParam(*subMesh,"indexBuffer",handle);
		if ( iface )
		{
			if ( handle.resizeArray(sm->mTriCount*3) == ::NxParameterized::ERROR_NONE )
			{
				for (physx::PxU32 i=0; i<sm->mTriCount; i++)
				{
					physx::PxU32 tri[3];
					tri[0] = indexRemapOut[sm->mIndices[i*3+0]];
					tri[1] = indexRemapOut[sm->mIndices[i*3+1]];
					tri[2] = indexRemapOut[sm->mIndices[i*3+2]];
					handle.setParamU32Array(tri,3,i*3);
				}
			}
		}
		iface = NxParameterized::findParam(*subMesh,"vertexPartition",handle);
		if ( iface )
		{
			if ( handle.resizeArray(2) == ::NxParameterized::ERROR_NONE )
			{
				physx::PxU32 store[2] = { 0, vcount };
				handle.setParamU32Array(store,2,0);
			}
		}
		iface = NxParameterized::findParam(*subMesh,"indexPartition",handle);
		if ( iface )
		{
			if ( handle.resizeArray(2) == ::NxParameterized::ERROR_NONE )
			{
				physx::PxU32 store[2] = { 0, sm->mTriCount*3 };
				handle.setParamU32Array(store,2,0);
			}
		}


		delete []vertices;
		delete []indexRemapOut;

	}

	NxParameterized::Interface * getApexRenderMesh(Mesh *mesh)
	{
		NxParameterized::Interface *renderMesh = this->createNxParameterized("RenderMeshAssetParameters");

		NxParameterized::Handle handle(*renderMesh);
		NxParameterized::Interface *iface = NxParameterized::findParam(*renderMesh,"materialNames",handle);
		if ( iface )
		{
			handle.resizeArray(mesh->mSubMeshCount);
		}
		iface = NxParameterized::findParam(*renderMesh,"submeshes",handle);
		if ( iface )
		{
			handle.resizeArray(mesh->mSubMeshCount);
			for (physx::PxU32 i=0; i<mesh->mSubMeshCount; i++)
			{
				SubMesh *subMesh = mesh->mSubMeshes[i];
				handle.set(i);
				NxParameterized::Interface *paramPtr = this->createNxParameterized("SubmeshParameters");
				handle.setParamRef(paramPtr);
				if ( paramPtr )
				{
					NxParameterized::Handle materialHandle(*renderMesh);
					char scratch[512];
					physx::string::sprintf_s(scratch,512,"materialNames[%d]",i);
					NxParameterized::Interface *mat = NxParameterized::findParam(*renderMesh,scratch,materialHandle);
					PX_ASSERT(mat);
					if ( mat )
					{
						materialHandle.setParamString(subMesh->mMaterialName);
					}
					getSubMesh(paramPtr,subMesh,mesh);
				}
				handle.popIndex();
			}
		}
		return renderMesh;
	}

	virtual const void * saveMeshSystem(MeshSystem *ms,NxU32 &dlen,bool binary)
	{

		const void *ret = NULL;

		NxParameterized::Interface ** meshes = new NxParameterized::Interface *[ms->mMeshCount];
		for (physx::PxU32 i=0; i<ms->mMeshCount; i++)
		{
			meshes[i] = getApexRenderMesh(ms->mMeshes[i] );
		}
		NxParameterized::NxSerializer *ser = internalCreateNxSerializer(binary ? NxSerializer::NST_BINARY : NxSerializer::NST_XML, this );
		PX_ASSERT(ser);
		physx::PxMemoryBuffer mbuff;
		ser->serialize(mbuff,(const NxParameterized::Interface **)meshes,ms->mMeshCount);
		if ( mbuff.getWriteBufferSize() > 0 )
		{
			void *mem = ::malloc(mbuff.getWriteBufferSize());
			PX_ASSERT(mem);
			memcpy(mem,mbuff.getWriteBuffer(),mbuff.getWriteBufferSize());
			dlen = mbuff.getWriteBufferSize();
			ret = mem;
		}
		for (physx::PxU32 i=0; i<ms->mMeshCount; i++)
		{
			meshes[i]->destroy();
		}
		delete []meshes;

		return ret;
	}

	virtual void releaseSavedMeshSystem(const void *mem)
	{
		::free((void *)mem);
	}


private:

	InterfaceVector				mObjects;

	NxApexBasicIOSAssetParamFactory mNxApexBasicIOSAssetParamFactory;
	NxApexClothingActorParamFactory mNxApexClothingActorParamFactory;
	ClothingAssetParametersFactory mClothingAssetParametersFactory;
	ClothingGraphicalLodParametersFactory mClothingGraphicalLodParametersFactory;
	ClothingMaterialLibraryParametersFactory mClothingMaterialLibraryParametersFactory;
	ClothingPhysicalMeshParametersFactory mClothingPhysicalMeshParametersFactory;
	ConvexHullParametersFactory mConvexHullParametersFactory;
	NxApexDestructibleActorParamFactory mNxApexDestructibleActorParamFactory;
	DestructibleAssetParametersFactory mDestructibleAssetParametersFactory;
	SurfaceTraceParametersFactory mSurfaceTraceParametersFactory;
	SurfaceTraceSetParametersFactory mSurfaceTraceSetParametersFactory;
	ApexEmitterActorParametersFactory mApexEmitterActorParametersFactory;
	ApexEmitterAssetParametersFactory mApexEmitterAssetParametersFactory;
	EmitterGeomBoxParamsFactory mEmitterGeomBoxParamsFactory;
	EmitterGeomExplicitParamsFactory mEmitterGeomExplicitParamsFactory;
	EmitterGeomSphereParamsFactory mEmitterGeomSphereParamsFactory;
	EmitterGeomSphereShellParamsFactory mEmitterGeomSphereShellParamsFactory;
	GroundEmitterActorParametersFactory mGroundEmitterActorParametersFactory;
	GroundEmitterAssetParametersFactory mGroundEmitterAssetParametersFactory;
	ImpactEmitterActorParametersFactory mImpactEmitterActorParametersFactory;
	ImpactEmitterAssetParametersFactory mImpactEmitterAssetParametersFactory;
	NxApexExplosionAssetParamFactory mNxApexExplosionAssetParamFactory;
	ExplosionActorParametersFactory mExplosionActorParametersFactory;
	ExplosionEnvParametersFactory mExplosionEnvParametersFactory;
	FieldBoundaryActorParametersFactory mFieldBoundaryActorParametersFactory;
	FieldBoundaryAssetParametersFactory mFieldBoundaryAssetParametersFactory;
	ShapeBoxParamsFactory mShapeBoxParamsFactory;
	ShapeCapsuleParamsFactory mShapeCapsuleParamsFactory;
	ShapeConvexParamsFactory mShapeConvexParamsFactory;
	ShapeSphereParamsFactory mShapeSphereParamsFactory;
	RenderMeshAssetParametersFactory mRenderMeshAssetParametersFactory;
	SubmeshParametersFactory mSubmeshParametersFactory;
	VertexBufferParametersFactory mVertexBufferParametersFactory;
	ColorVsDensityModifierParamsFactory mColorVsDensityModifierParamsFactory;
	ColorVsLifeModifierParamsFactory mColorVsLifeModifierParamsFactory;
	IofxAssetParametersFactory mIofxAssetParametersFactory;
	OrientAlongVelocityModifierParamsFactory mOrientAlongVelocityModifierParamsFactory;
	RandomRotationModifierParamsFactory mRandomRotationModifierParamsFactory;
	RandomScaleModifierParamsFactory mRandomScaleModifierParamsFactory;
	RandomSubtextureModifierParamsFactory mRandomSubtextureModifierParamsFactory;
	RotationModifierParamsFactory mRotationModifierParamsFactory;
	ScaleAlongVelocityModifierParamsFactory mScaleAlongVelocityModifierParamsFactory;
	ScaleVsCameraDistanceModifierParamsFactory mScaleVsCameraDistanceModifierParamsFactory;
	ScaleVsDensityModifierParamsFactory mScaleVsDensityModifierParamsFactory;
	ScaleVsLifeModifierParamsFactory mScaleVsLifeModifierParamsFactory;
	SimpleScaleModifierParamsFactory mSimpleScaleModifierParamsFactory;
	SubtextureVsLifeModifierParamsFactory mSubtextureVsLifeModifierParamsFactory;
	ViewDirectionSortingModifierParamsFactory mViewDirectionSortingModifierParamsFactory;
	NxFluidIosParametersFactory mNxFluidIosParametersFactory;
	NxApexWindAssetParamFactory mNxApexWindAssetParamFactory;
	WindActorParametersFactory mWindActorParametersFactory;
	VertexFormatParametersFactory mVertexFormatParametersFactory;

	BufferF32x1Factory mBufferF32x1Factory;
	BufferF32x2Factory mBufferF32x2Factory;
	BufferF32x3Factory mBufferF32x3Factory;
	BufferF32x4Factory mBufferF32x4Factory;

	BufferU8x1Factory mBufferU8x1Factory;
	BufferU8x2Factory mBufferU8x2Factory;
	BufferU8x3Factory mBufferU8x3Factory;
	BufferU8x4Factory mBufferU8x4Factory;

	BufferU16x1Factory mBufferU16x1Factory;
	BufferU16x2Factory mBufferU16x2Factory;
	BufferU16x3Factory mBufferU16x3Factory;
	BufferU16x4Factory mBufferU16x4Factory;

	BufferU32x1Factory mBufferU32x1Factory;
	BufferU32x2Factory mBufferU32x2Factory;
	BufferU32x3Factory mBufferU32x3Factory;
	BufferU32x4Factory mBufferU32x4Factory;


};


static AppUserAllocator appAlloc;
static AppUserOutputStream appOutput;

MeshImporter * createMeshImportARM(void)
{
	static bool first = true;
	if ( first )
	{
		physx::Foundation::createInstance(PX_PUBLIC_FOUNDATION_VERSION, appOutput, appAlloc);
		first = false;
	}
    MeshImporterARM *m = MEMALLOC_NEW(MeshImporterARM);
    return static_cast< MeshImporter *>(m);
}

void           releaseMeshImportARM(MeshImporter *iface)
{
    MeshImporterARM *p = static_cast< MeshImporterARM *>(iface);
    delete p;
}


};  // end of namespace
