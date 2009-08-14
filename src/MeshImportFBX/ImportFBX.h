#ifndef IMPORT_FBX_H
#define IMPORT_FBX_H

#include <fbxsdk.h>
#include <fbxfilesdk/fbxfilesdk_nsuse.h>

#include <map>
#include <vector>

#include "FBXReader.h"
#include "Texture.h"
#include "MeshImport.h"
#include "stringdict.h"

#include <iostream>
#include <fstream>
#include <string.h>

#include <vector>

#include "fmi_math.h"

using namespace std;

#pragma warning(disable: 4565)

#ifdef MESHIMPORTFBX_EXPORTS
#define FBX_DLL_API extern "C" __declspec(dllexport)
#else
#define FBX_DLL_API extern "C" __declspec(dllimport)
#endif


#define MAX_BONES	4

#define DTIME_INTERVAL (1/30.0f)

struct PolygonMaterial
{
      	int materialIndex;
 //    	int diffuseIndex;
//	    int normalIndex;
	    int polygonIndex;

	    bool submeshesEqual( const PolygonMaterial& t )
	    {
		    return	materialIndex == t.materialIndex
//				&& diffuseIndex == t.diffuseIndex
//				&& normalIndex == t.normalIndex
            ;
	    }
};

// PH: Always use the same version as in the header. Everything else is just a pain
#define MESHIMPORTFBX_VERSION MESHIMPORT_VERSION  // version 0.01  increase this version number whenever an interface change occurs.



struct BoneRelation
{
	const char* bone;
	const char* parent;
};

struct ClusterBoneMap
{
public:
	ClusterBoneMap(): clusterID(-1),
		              meshBone(NULL),
					  pMesh(NULL),
					  globalPosition(),
					  boneDataInitialized(false),
					  clusterInfoInitialized(false),
					  clusterName(""),
					  isRoot(false),
					  boneName(""){}

	int clusterID;
	const char* clusterName;
	const char* boneName;
	KFbxXMatrix bindPose;
	MESHIMPORT::MeshBone* meshBone;
	KFbxMesh *pMesh;
	KFbxXMatrix globalPosition;
	bool boneDataInitialized;
	bool clusterInfoInitialized;
	bool isRoot;

	bool IsIntialized() { return (boneDataInitialized && clusterInfoInitialized);}

};


class MeshImportFBX: public MESHIMPORT::MeshImporter
{

public:
	MeshImportFBX();
	~MeshImportFBX();

	virtual const char * getExtension(int index);
	virtual const char * getDescription(int index);
	virtual bool importMesh(const char *fname,
		                    const void *data,unsigned int dlen,
							MESHIMPORT::MeshImportInterface *callback,
							const char *options,
							MESHIMPORT::MeshImportApplicationResource *appResource);


	void ProcessScene(KFbxNode *subScene = NULL);

	void ImportMesh();


	void ImportSkeleton();
	bool importSkeletonRecursive(KFbxNode* node, int parentBone, int& boneAllocator);	
	void ImportAnimation();

	//void AddSkeletonNode(KFbxNode* pNode, KTime& pTime, KFbxXMatrix& pParentGlobalPosition);
    void AddMeshNode(KFbxNode* pNode);
	

	int	mNumBones;

    // Utility

	KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, KTime& pTime, KFbxPose* pPose, KFbxXMatrix* pParentGlobalPosition = 0);
	KFbxXMatrix GetGlobalPosition(KFbxNode* pNode, KTime& pTime, KFbxXMatrix& pParentGlobalPosition );
	KFbxXMatrix GetPoseMatrix(KFbxPose* pPose, int pNodeIndex);
	KFbxXMatrix GetGeometry(KFbxNode* pNode);
	
	inline void MatrixScale(KFbxXMatrix& pMatrix, double pValue);
	inline void MatrixAddToDiagonal(KFbxXMatrix& pMatrix, double pValue);
	inline void MatrixAdd(KFbxXMatrix& pDstMatrix, KFbxXMatrix& pSrcMatrix);
	
	const char* getFileName( const char* fullPath);

	bool Import( const char* filename, MESHIMPORT::MeshImportInterface* callBack );

	void outputMessage( const char* message );


	StringDict									meshStrings;
	MESHIMPORT::MeshSkeleton					meshSkeleton;
	std::vector<MESHIMPORT::MeshBone>			meshBones;
	std::vector<KFbxNode*>						meshNodes;
	std::vector<KFbxXMatrix>					meshWorldBindPoseXforms;
	std::vector<KFbxXMatrix>					meshWorldBindShapeXforms;
	std::vector<KFbxXMatrix>					meshWorldAnimXforms;
	MESHIMPORT::MeshAnimation					meshAnimation;
	std::vector<MESHIMPORT::MeshAnimTrack*>		meshTrackPtrs;
	std::vector<MESHIMPORT::MeshAnimTrack>		meshTracks;
	std::vector<MESHIMPORT::MeshAnimPose>		meshPoses;


	inline void GetBindPoseMatrix(KFbxCluster *lCluster, KFbxXMatrix& bindPose);
	inline void GetBindShapeMatrix( KFbxCluster *lCluster, KFbxXMatrix& bindShape);
	void ApplyVertexTransform(KFbxMesh* pMesh, MESHIMPORT::MeshVertex *pVerts);
	
	void ConvertNurbsAndPatch(KFbxSdkManager* pSdk, KFbxScene* pScene);

    void ConvertNurbsAndPatchRecursive(KFbxSdkManager* pSdk, 
		                                      KFbxScene* pScene, KFbxNode* pNode);


    //void LoadTexture(KFbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray);

	//void LoadSupportedTexturesRecursive(KFbxNode *pNode, KArrayTemplate<VSTexture*>& pTextureArray);

	//void LoadSupportedTextures(KFbxScene *pScene, KArrayTemplate<VSTexture*>& pTextureArray);

	bool getClusterByIndex(int j, ClusterBoneMap** pose)
	{
		
		bool ret = false;

		map<const char * , ClusterBoneMap*>::iterator it = m_boneClusterMap.begin();

		while(it != m_boneClusterMap.end())
		{
			ClusterBoneMap* tempPose = it->second;
			
			if(tempPose->clusterID == j)
			{
				*pose = tempPose;
				ret = true;
				break;
			}

			it++;
		}

		return ret;

	}


	bool getCluster(const char * name, ClusterBoneMap** cluster)
	{
		bool ret = false;

		map<const char *, ClusterBoneMap*>::iterator it = m_boneClusterMap.begin();

		while(it != m_boneClusterMap.end())
		{
			ClusterBoneMap* pose = it->second;
			if(0 == strcmp(pose->clusterName,name))
			{
				*cluster = pose;
				ret = true;
				break;
			}
			
			it++;
		}
		return ret;

	}
	bool getClusterByBone(const char* name, ClusterBoneMap** cluster)
	{
		bool ret = false;


		map<const char * , ClusterBoneMap*>::iterator it = m_boneClusterMap.begin();
			

		char clusterName[128];
		strcpy(clusterName, "Cluster ");
		strcat(clusterName, name);
		strcat(clusterName, "\0");

		while(it != m_boneClusterMap.end())
		{
			ClusterBoneMap* pose  = it->second;
			//@@DW Bad string termination???
			if(0 == strcmp(clusterName,it->first))
			{
				*cluster = pose;
				ret = true;
				break;

			}
			
			it++;
		}
		//find doesnt work??
		/*if((it = m_boneClusterMap.find(name)) != m_boneClusterMap.end())
		{
			cluster = it->second;
			ret = true;
		}*/

		
		return ret;

	}
	bool getBoneBindPose(const char *name, KFbxMatrix& bindPose)
	{
		bool ret = false;

	

		ClusterBoneMap* bone;

		if(getClusterByBone(name, &bone))
		{
			bindPose = bone->bindPose;
			ret = true;
		}

		return ret;
	}

	void AddClusterInfo(ClusterBoneMap* pCluster)
	{
		ClusterBoneMap *cluster;
		if(getCluster(pCluster->clusterName, &cluster))
		{
			cluster->clusterID = pCluster->clusterID;
			cluster->pMesh =  pCluster->pMesh;
			cluster->clusterName = pCluster->clusterName;
			cluster->clusterInfoInitialized = true;
			delete pCluster;

		} else
		{
		    m_boneClusterMap.insert(m_boneClusterMap.begin(), pair<const char *, ClusterBoneMap*>(pCluster->clusterName, pCluster));
			pCluster->clusterInfoInitialized = true;
		}

		
	}

	int getNumBones()
	{
		return (int)m_boneClusterMap.size();
	}
	 bool getBone(const char *name, MESHIMPORT::MeshBone& bone)
	{
		bool ret = false;

		char clusterName[128];
		strcpy(clusterName, "Cluster ");
		strcat(clusterName, name);
		strcat(clusterName, "\0");

		map<const char *, ClusterBoneMap*>::iterator it;

		if((it = m_boneClusterMap.find(clusterName)) != m_boneClusterMap.end())
		{
			ClusterBoneMap* cluster = it->second;

			bone = *cluster->meshBone;
			ret = true;

		}

		return ret;

	}

	void updateBone(const char* name, MESHIMPORT::MeshBone *newBone, KFbxXMatrix& pGlobalPosition)
	{
		ClusterBoneMap* pose;

		char clusterName[128];
		strcpy(clusterName, "Cluster ");
		strcat(clusterName, name);
		strcat(clusterName, "\0");

		if(getClusterByBone(clusterName, &pose))
		{
			if(pose->meshBone && (newBone != pose->meshBone))
				delete pose->meshBone;

			pose->meshBone = newBone;
			pose->globalPosition = pGlobalPosition;
		    

		}

	}
	
	//void addBoneInfo(const char *name, MESHIMPORT::MeshBone* newBone, KFbxXMatrix& globalPos, bool isRoot)
	//{
	//	
	//	
	//	mVertexFormat |= MIVF_BONE_WEIGHTING;

	//	ClusterBoneMap* curPose;

	//	//@@DW Tidy this
	//	char clusterName[128];
	//	strcpy(clusterName, "Cluster ");
	//	strcat(clusterName, name);
	//	strcat(clusterName, "\0");

	//	char *newName = (char *)malloc(strlen(clusterName) + 1 * sizeof(char));

	//	strcpy(newName, clusterName);

	//	if(!getCluster(clusterName, &curPose))
	//	{
	//		curPose = new ClusterBoneMap();
	//		m_boneClusterMap.insert(m_boneClusterMap.begin(), pair<const char*, ClusterBoneMap*>(newName, curPose));
	//	} 
	//	
	//	
	//	curPose->meshBone = newBone;
	//	curPose->globalPosition = globalPos;
	//	curPose->boneName = name;
	//	curPose->boneDataInitialized = true;
	//	curPose->isRoot = isRoot;

	//	
	//}

	int mBoneCount;
	int mVertexCount;
	long mVertexFormat;
	
	
	map<const char * , ClusterBoneMap* >m_boneClusterMap; // Map to cluster ID
	vector<BoneRelation*>  m_boneHierarchy;

protected:

	MESHIMPORT::MeshImportInterface *m_callback;

	void Release();

	


	 
private:


	
	KString						m_fileName;
	KString						m_filePath;

	

	KFbxSdkManager*				m_sdkManager;
	KFbxImporter*				m_importer;
	KFbxScene*					m_scene;
	KFbxTakeInfo*				m_takeInfo;
	KString*					m_takeName;

	KArrayTemplate<KFbxNode*>	m_cameraArray;
	KArrayTemplate<KString*>	m_takeNameArray;
	KArrayTemplate<KFbxPose*>	m_poseArray;
	KArrayTemplate<VSTexture*>	m_textureArray;
    KArrayTemplate<KFbxSurfaceMaterial*> m_MaterialArray;


	KTime						m_period;

	unsigned __int32			m_vertexCount;
	unsigned __int32			m_vertexUVCount;
	unsigned __int32			m_triangleCount;
	unsigned __int32			m_meshDataTypes;

};


//MeshImporter * createMeshImportFBX(void);
void           releaseMeshImportFBX(MESHIMPORT::MeshImporter *iface);



#endif