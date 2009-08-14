#pragma once;

#include "ImportFBX.h"

#define OUTPUT_TEXT_BUFFER_SIZE	1000


class FBXImporterReader : public KFbxReader
{
public:


    FBXImporterReader(KFbxSdkManager &pFbxSdkManager, int pID);

    virtual ~FBXImporterReader();

    virtual void GetVersion(int& pMajor, int& pMinor, int& pRevision) const;
    virtual bool FileOpen(char* pFileName);
    virtual bool FileClose();
    virtual bool IsFileOpen();

    virtual KFbxStreamOptions* GetReadOptions(bool pParseFileAsNeeded = true);
    virtual bool Read(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions);

private:
    FILE *mFilePointer;
    KFbxSdkManager *mManager;
};

extern  KFbxReader* CreateFBXImporterReader( KFbxSdkManager& pManager, KFbxImporter& pImporter, int pSubID, int pPluginID );
extern void* GetFBXImporterReaderInfo( KFbxReader::KInfoRequest pRequest, int pId );
extern void FillFBXImporterReaderIOSettings( KFbxIOSettings& pIOS );
