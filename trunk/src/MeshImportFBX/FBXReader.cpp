// Local definitions

#include "FBXReader.h"

FBXImporterReader::FBXImporterReader(KFbxSdkManager &pFbxSdkManager, int pID):
KFbxReader(pFbxSdkManager, pID),
mFilePointer(NULL),
mManager(&pFbxSdkManager)
{
}

FBXImporterReader::~FBXImporterReader()
{
    FileClose();
}

void FBXImporterReader::GetVersion(int& pMajor, int& pMinor, int& pRevision) const
{
    pMajor = 1;
    pMinor = 0;
    pRevision=0;
}

bool FBXImporterReader::FileOpen(char* pFileName)
{
    if(mFilePointer != NULL)
        FileClose();
    if(fopen_s(&mFilePointer, pFileName, "r") != 0)
        return false;
    return true;
}
bool FBXImporterReader::FileClose()
{
    if(mFilePointer!=NULL)
        fclose(mFilePointer);
    return true;
    
}
bool FBXImporterReader::IsFileOpen()
{
    if(mFilePointer != NULL)
        return true;
    return false;
}

KFbxStreamOptions* FBXImporterReader::GetReadOptions(bool pParseFileAsNeeded)
{
    //No need to worry, the importer that calls this, will call destroy
    //on these stream options at the end of import.
    KFbxStreamOptions* lStreamOptions = KFbxStreamOptions::Create(mManager, "");
    return lStreamOptions;
}

//Read the custom file and reconstruct node hierarchy.
bool FBXImporterReader::Read(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions)
{
    if (!pDocument)
    {
        GetError().SetLastErrorID(eINVALID_DOCUMENT_HANDLE);
        return false;
    }
    KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
    bool            lIsAScene = (lScene != NULL);
    bool            lResult = false;

    if(lIsAScene)
    {
        KFbxNode* lRootNode = lScene->GetRootNode();
        KFbxNodeAttribute * lRootNodeAttribute = KFbxNull::Create(mManager,"");
        lRootNode->SetNodeAttribute(lRootNodeAttribute);

        int lSize;
        char* lBuffer;    
        if(mFilePointer != NULL)
        {
            //To obtain file size
            fseek (mFilePointer , 0 , SEEK_END);
            lSize = ftell (mFilePointer);
            rewind (mFilePointer);

            //Read file content to a string.
            lBuffer = (char*) malloc (sizeof(char)*lSize);
            size_t lRead = fread(lBuffer, 1, lSize, mFilePointer);
            lBuffer[lRead]='\0';
            KString lString(lBuffer);

            //Parse the string to get name and relation of Nodes. 
            KString lSubString, lChildName, lParentName;
            KFbxNode* lChildNode;
            KFbxNode* lParentNode;
            KFbxNodeAttribute* lChildAttribute;
            int lEndTokenCount = lString.GetTokenCount("\n");

            for (int i = 0; i < lEndTokenCount; i++)
            {
                lSubString = lString.GetToken(i, "\n");
                KString lNodeString;
                lChildName = lSubString.GetToken(0, "\"");
                lParentName = lSubString.GetToken(2, "\"");

                //Build node hierarchy.
                if(lParentName == "RootNode")
                {
                    lChildNode = KFbxNode::Create(mManager,lChildName.Buffer());
                    lChildAttribute = KFbxNull::Create(mManager,"");
                    lChildNode->SetNodeAttribute(lChildAttribute);

                    lRootNode->AddChild(lChildNode);
                }
                else
                {
                    lChildNode = KFbxNode::Create(mManager,lChildName.Buffer());
                    lChildAttribute = KFbxNull::Create(mManager,"");
                    lChildNode->SetNodeAttribute(lChildAttribute);

                    lParentNode = lRootNode->FindChild(lParentName.Buffer());
                    lParentNode->AddChild(lChildNode);
                }
            }
        }
        lResult = true;
    }    
    return lResult;
}


KFbxReader* CreateFBXImporterReader( KFbxSdkManager& pManager, KFbxImporter& pImporter, int pSubID, int pPluginID )
{
    return new FBXImporterReader( pManager, pPluginID );
}


// Get extension, description or version info about MyOwnReader
void* GetFBXImporterReaderInfo( KFbxReader::KInfoRequest pRequest, int pId )
{
    static char const* sExt[] = 
    {
        0
    };

    static char const* sDesc[] = 
    {
        0
    };

    switch (pRequest)
    {
    case KFbxReader::eInfoExtension:
        return sExt;
    case KFbxReader::eInfoDescriptions:
        return sDesc;
    default:
        return 0;
    }
}

void FillFBXImporterReaderIOSettings( KFbxIOSettings& pIOS )
{    
}
