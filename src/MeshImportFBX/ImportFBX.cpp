#pragma warning(disable:4265)
#include "UserMemAlloc.h"
#include "ImportFBX.h"

namespace NVSHARE
{

#define DEBUG_LOG 1

	// Local functions and definitions
#define MESSAGE_BUFFER_SIZE 1000
char messageBuffer[MESSAGE_BUFFER_SIZE];

MeshImportFBX::MeshImportFBX() :
m_sdkManager(NULL),
m_importer(NULL),
m_scene(NULL),
m_takeInfo(NULL),
m_takeName(NULL)
{
	mNumBones = 0;
	mVertexFormat = 0;
}

MeshImportFBX::~MeshImportFBX(void)
{
	Release();
}

void MeshImportFBX::Release(void)
{
	m_takeInfo = NULL;
	m_takeName = NULL;
	
	m_cameraArray.Clear();
	m_takeNameArray.Clear();
	m_poseArray.Clear();
	m_textureArray.Clear();
	m_MaterialArray.Clear();

	for(std::map<const char * , ClusterBoneMap* >::iterator itr = m_boneClusterMap.begin();
		itr != m_boneClusterMap.end();
		itr++ )
	{
		if(itr->second)
			delete itr->second;
	}
	m_boneClusterMap.clear();

	for(int i = 0; i < m_boneHierarchy.size(); i++)
	{
		if(m_boneHierarchy[i])
			delete m_boneHierarchy[i];
	}
	m_boneHierarchy.clear();
	

	if (m_scene != NULL)
	{
		m_scene->Destroy(true, true); 
		m_scene = NULL;
	}

	if (m_importer != NULL)
	{
		m_importer->Destroy(true, true);
		m_importer = NULL;
	}

	if (m_sdkManager != NULL)
	{
		m_sdkManager->Destroy();
		m_sdkManager = NULL;	
	}
}
 const char* MeshImportFBX::getFileName( const char* fullPath )
{
	if( !fullPath )
	{
        return NULL;
	}

	const char* fileName = fullPath + strlen( fullPath );
	while( fileName > fullPath )
	{
		char c = fileName[-1];
		if( c == '\\' || c == '/' )
		{
			break;
		}
		--fileName;
	}

	return fileName;
}

void MeshImportFBX::outputMessage( const char* message )
{
	printf("%s\r\n", message);
}

const char * MeshImportFBX::getExtension(int index)  // report the default file name extension for this mesh type.
{
	return ".fbx";
}

const char * MeshImportFBX::getDescription(int index)  // report the default file name extension for this mesh type.
{
	return "Autodesk FBX Scene File";
}

bool MeshImportFBX::importMesh(const char *fname,
							   const void *data,
							   unsigned int dlen,
							   NVSHARE::MeshImportInterface *callback,
							   const char *options, 
							   NVSHARE::MeshImportApplicationResource *appResource)
{	
	bool ret = false;

	// Must save the data to a temporary file, as FBX importer *only* reads from files!
	FILE *fph = fopen("@temp.fbx","wb");
	if ( fph )
	{
		fwrite(data,dlen,1,fph);
		fclose(fph);
	    ret = Import("@temp.fbx", callback);
	}

    return ret;
}

bool MeshImportFBX::Import( const char* filename, NVSHARE::MeshImportInterface *callback  )
{
	char message[OUTPUT_TEXT_BUFFER_SIZE+1] = "";
	message[OUTPUT_TEXT_BUFFER_SIZE] = '\0';
	
    const char* localName = getFileName( filename );
	KString fileName = KString( filename );
	KString filePath = fileName.Left( localName - filename );

	m_sdkManager = KFbxSdkManager::Create();

	if(m_sdkManager == NULL)
		return false;


    // Create the importer.
    int fileFormat = -1;
    //int registeredCount;
    //int pluginId;
    //m_sdkManager->GetIOPluginRegistry()->RegisterReader( CreateFBXImporterReader, GetFBXImporterReaderInfo,
    //             pluginId, registeredCount, FillFBXImporterReaderIOSettings );

    m_importer = KFbxImporter::Create( m_sdkManager, "" );
	if( !m_sdkManager->GetIOPluginRegistry()->DetectFileFormat( filename, fileFormat ) )
    {
        // Unrecognizable file format. Try to fall back to KFbxImporter::eFBX_BINARY
        fileFormat = m_sdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
    }

    m_importer->SetFileFormat( fileFormat );


    // Initialize the importer by providing a filename.
    if( !m_importer->Initialize( filename ) )
		return false;

    // Create the scene.
    m_scene = KFbxScene::Create( m_sdkManager, "" );


    if (m_importer->IsFBX())
    {
        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOSREF.SetBoolProp(IMP_FBX_MATERIAL,        true);
        IOSREF.SetBoolProp(IMP_FBX_TEXTURE,         true);
        IOSREF.SetBoolProp(IMP_FBX_LINK,            true);
        IOSREF.SetBoolProp(IMP_FBX_SHAPE,           true);
        IOSREF.SetBoolProp(IMP_FBX_GOBO,            true);
        IOSREF.SetBoolProp(IMP_FBX_ANIMATION,       true);
        IOSREF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }



	sprintf_s( message, OUTPUT_TEXT_BUFFER_SIZE, "Importing file %s", filename );
	outputMessage( message );
	if( !m_importer->Import(m_scene) )
		return false;

	//// Convert Axis System to what is used in this example, if needed
	//KFbxAxisSystem sceneAxisSystem = m_scene->GetGlobalSettings().GetAxisSystem();
	//KFbxAxisSystem ourAxisSystem(sceneAxisSystem.KFbxAxisSystem::ZAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::LeftHanded);
	//if( sceneAxisSystem != ourAxisSystem )
	//{
	//     ourAxisSystem.ConvertScene(m_scene);
	//}

	//// Convert Unit System to what is used in this example, if needed
	//KFbxSystemUnit sceneSystemUnit = m_scene->GetGlobalSettings().GetSystemUnit();
	//if( sceneSystemUnit.GetScaleFactor() != 1.0 )
	//{
	//	
	//    KFbxSystemUnit ourSystemUnit(1.0);
	//	ourSystemUnit.ConvertScene(m_scene);
	//	
	//}

	m_callback = callback;
	
	ImportSkeleton();

	m_takeName = NULL;
	m_takeInfo = NULL;
	m_takeNameArray.Clear();

    int takeCount = m_importer->GetTakeCount();
	int tSelected = -1;

	for(int t = 0; t < takeCount; t++ )
	{
		m_takeInfo = m_importer->GetTakeInfo(t);
		m_takeNameArray.Add( &m_takeInfo->mName );
		if(m_takeInfo->mSelect)
			tSelected = t;
	}        
	if(tSelected == -1 && takeCount > 0)
		tSelected = 0;

	if(tSelected >= 0)
	{
		m_takeInfo = m_importer->GetTakeInfo(tSelected);
		m_takeName = m_takeNameArray[tSelected];
		m_scene->SetCurrentTake( m_takeName->Buffer() );

		if (!ImportAnimation())
		{
			Release();
			return false;
		}
	}

    m_scene->FillMaterialArray(m_MaterialArray);


    ProcessScene();
				
	//// Load the texture data in memory (for supported formats)
	//LoadSupportedTextures(m_scene, m_textureArray);
				
	sprintf_s( message, OUTPUT_TEXT_BUFFER_SIZE, "done!" );
	outputMessage( message );


	m_scene->Destroy(true, true);
	m_scene = NULL;

	m_importer->Destroy(true, true);
	m_importer = NULL;

	m_sdkManager->Destroy();
	m_sdkManager = NULL;

	return true;
}



void releaseMeshImportFBX(NVSHARE::MeshImporter *iface)
{
    MeshImportFBX *m = static_cast< MeshImportFBX *>(iface);
    delete m;
}



/*
void FBXImporter::nodeCollectMaterials( ApexDefaultMaterialLibrary& materialLibrary, KFbxNode* pNode )
{
	KFbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

	if (lNodeAttribute)
	{
		KFbxLayerContainer* lLayerContainer = NULL;

		switch (lNodeAttribute->GetAttributeType())
		{
		case KFbxNodeAttribute::eNURB:
			lLayerContainer = pNode->GetNurb();
			break;

		case KFbxNodeAttribute::ePATCH:
			lLayerContainer = pNode->GetPatch();
			break;

		case KFbxNodeAttribute::eMESH:
			lLayerContainer = pNode->GetMesh();
			break;
		}

		if (lLayerContainer){
			int lMaterialIndex;
			int lTextureIndex;
			KFbxProperty lProperty;
			int lNbTex;
			KFbxTexture* lTexture = NULL; 
			KFbxSurfaceMaterial *lMaterial = NULL;
			int lNbMat = pNode->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
			KString materialNamePrefix = m_fileName + KString( "#" );
			for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++)
			{
				lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex));
				if(lMaterial)
				{ 
					bool created;
					ApexDefaultMaterial* material = (ApexDefaultMaterial*)materialLibrary.getMaterial( materialNamePrefix+lMaterial->GetName(), created );
					if( created )
					{
						lProperty = lMaterial->FindProperty(KFbxSurfaceMaterial::sDiffuse);
						if(lProperty.IsValid())
						{
							lNbTex = lProperty.GetSrcObjectCount(KFbxTexture::ClassId);
							for (lTextureIndex = 0; lTextureIndex < lNbTex; lTextureIndex++)
							{
								lTexture = KFbxCast <KFbxTexture> (lProperty.GetSrcObject(KFbxTexture::ClassId, lTextureIndex)); 
								if(lTexture)
								{
									ApexDefaultTextureMap* textureMap = new ApexDefaultTextureMap();
									ApexDefaultTextureMap::FileErrorType error = textureMap->load( lTexture->GetFileName() );
									// Attempt several different ways to find the file:
									if( error == ApexDefaultTextureMap::FILE_NOT_FOUND )
									{
										KString localName = getFileName( lTexture->GetFileName() );
										KString filename = m_filePath + localName;
										error = textureMap->load( filename );
										if( error == ApexDefaultTextureMap::FILE_NOT_FOUND )
										{
											error = textureMap->load( localName );
										}
									}
									switch( error )
									{
									case ApexDefaultTextureMap::NO_FILE_ERROR:
										material->setTextureMap( DIFFUSE_MAP, textureMap );
										break;
									case ApexDefaultTextureMap::BAD_FILENAME:
										sprintf_s( messageBuffer, MESSAGE_BUFFER_SIZE, "Bad texture filename: %s.", lTexture->GetFileName() ? lTexture->GetFileName() : "NULL" );
										outputMessage( messageBuffer );
										break;
									case ApexDefaultTextureMap::FILE_NOT_FOUND:
										sprintf_s( messageBuffer, MESSAGE_BUFFER_SIZE, "Failed to open texture file %s.", lTexture->GetFileName() );
										outputMessage( messageBuffer );
										break;
									case ApexDefaultTextureMap::FILE_NOT_READABLE:
										sprintf_s( messageBuffer, MESSAGE_BUFFER_SIZE, "Failed to load texture map from file %s.  Current supported formats are .bmp, .dds, and .tga.", lTexture->GetFileName() );
										outputMessage( messageBuffer );
										break;
									}
								}
								break;	// For now, only one diffuse texture
							}
						}
					}
				}
			}
		} 
	}

	int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		nodeCollectMaterials( materialLibrary, pNode->GetChild(i) );
	}
}
*/

}; // end of namespace
