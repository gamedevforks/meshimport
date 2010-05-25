rd params /s /q
md params
cd params

md common
cd common
copy \p4\sw\physx\APEXSDK\1.0\trunk\common\src\ConvexHullParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\common\include\ConvexHullParams.h
cd ..

md framework
cd framework
copy \p4\sw\physx\APEXSDK\1.0\trunk\framework\src\RenderMeshAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\framework\src\SubmeshParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\framework\src\VertexBufferParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\framework\include\RenderMeshAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\framework\include\SubmeshParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\framework\include\VertexBufferParameters.h
cd ..

md basicios
cd basicios
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\basicios\src\ApexBasicIOSAssetParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\basicios\include\ApexBasicIOSAssetParams.h
cd ..

md clothing
cd clothing

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\src\ClothingActorParamSchema.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\src\ClothingAssetParamSchema.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\src\ClothingGraphicalLodParamSchema.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\src\ClothingMaterialLibraryParamSchema.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\src\ClothingPhysicalMeshParamSchema.cpp

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\include\ClothingActorParamSchema.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\include\ClothingAssetParamSchema.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\include\ClothingGraphicalLodParamSchema.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\include\ClothingMaterialLibraryParamSchema.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\clothing\include\ClothingPhysicalMeshParamSchema.h

cd ..
md destructible
cd destructible

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\src\ApexDestructibleActorParamSchema.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\src\DestructibleAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\src\SurfaceTraceParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\src\SurfaceTraceSetParameters.cpp

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\include\ApexDestructibleActorParamSchema.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\include\DestructibleAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\include\SurfaceTraceParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\destructible\include\SurfaceTraceSetParameters.h

cd ..
md emitter
cd emitter

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\ApexEmitterActorParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\ApexEmitterAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\EmitterGeomBoxParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\EmitterGeomExplicitParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\EmitterGeomSphereParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\EmitterGeomSphereShellParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\GroundEmitterActorParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\GroundEmitterAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\ImpactEmitterActorParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\src\ImpactEmitterAssetParameters.cpp

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\ApexEmitterActorParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\ApexEmitterAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\EmitterGeomBoxParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\EmitterGeomExplicitParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\EmitterGeomSphereParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\EmitterGeomSphereShellParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\GroundEmitterActorParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\GroundEmitterAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\ImpactEmitterActorParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\emitter\include\ImpactEmitterAssetParameters.h

cd ..
md explosion
cd explosion

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\explosion\src\ApexExplosionAssetParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\explosion\src\ExplosionActorParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\explosion\src\ExplosionEnvParameters.cpp

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\explosion\include\ApexExplosionAssetParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\explosion\include\ExplosionActorParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\explosion\include\ExplosionEnvParameters.h

cd ..

md forcefield
cd forcefield

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\src\FieldBoundaryActorParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\src\FieldBoundaryAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\src\ShapeBoxParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\src\ShapeCapsuleParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\src\ShapeConvexParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\src\ShapeSphereParams.cpp

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\include\FieldBoundaryActorParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\include\FieldBoundaryAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\include\ShapeBoxParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\include\ShapeCapsuleParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\include\ShapeConvexParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\forcefield\include\ShapeSphereParams.h

cd ..

md iofx
cd iofx

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ColorVsDensityModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ColorVsLifeModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\IofxAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\OrientAlongVelocityModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\RandomRotationModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\RandomScaleModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\RandomSubtextureModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\RotationModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ScaleAlongVelocityModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ScaleVsCameraDistanceModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ScaleVsDensityModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ScaleVsLifeModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\SimpleScaleModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\SubtextureVsLifeModifierParams.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\src\ViewDirectionSortingModifierParams.cpp

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ColorVsDensityModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ColorVsLifeModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\IofxAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\OrientAlongVelocityModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\RandomRotationModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\RandomScaleModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\RandomSubtextureModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\RotationModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ScaleAlongVelocityModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ScaleVsCameraDistanceModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ScaleVsDensityModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ScaleVsLifeModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\SimpleScaleModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\SubtextureVsLifeModifierParams.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\iofx\include\ViewDirectionSortingModifierParams.h

cd ..

md particles
cd particles

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\particles\src\NxFluidIosParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\particles\include\NxFluidIosParameters.h

cd ..

md wind
cd wind

copy \p4\sw\physx\APEXSDK\1.0\trunk\module\wind\src\ApexWindAssetParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\wind\src\WindActorParameters.cpp
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\wind\include\ApexWindAssetParameters.h
copy \p4\sw\physx\APEXSDK\1.0\trunk\module\wind\include\WindActorParameters.h

cd ..



cd ..
