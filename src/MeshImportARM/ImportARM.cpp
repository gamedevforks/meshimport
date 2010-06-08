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

#pragma warning(disable:4996)

using namespace physx;
using namespace NxParameterized;

namespace NVSHARE
{


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

	virtual void removeFactory( const char * name )
	{
		PX_ALWAYS_ASSERT(); // not expected/implemented
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

  		return ret;
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
