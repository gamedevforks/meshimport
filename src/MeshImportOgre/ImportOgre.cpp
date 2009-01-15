#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#pragma warning(disable:4702) // disabling a warning that only shows up when building VC7

#include "ImportOgre.h"
#include "MeshImport/MeshImport.h"
#include "MeshImport/MeshSystem.h"
#include "common/TinyXML/tinyxml.h"

#pragma warning(disable:4100)
#pragma warning(disable:4996)

#define DEBUG_LOG 1

namespace MESHIMPORT
{

enum NodeElement
{
  NE_MESH,
  NE_SHARED_GEOMETRY,
  NE_VERTEX_BUFFER,
  NE_VERTEX,
  NE_POSITION,
  NE_NORMAL,
	NE_BONEASSIGNMENTS, // boneasssignments
	NE_COLOR_DIFFUSE, // color_diffuse
	NE_FACE,        // face
	NE_TEXCOORD,    // texcoord
	NE_VERTEX_BONE_ASSIGNMENT, // vertexboneassignment
	NE_FACES,
	NE_SKELETON_LINK,
	NE_SUBMESH,
	NE_SUBMESHES,
	NE_LAST
};

enum NodeAttribute
{
  NA_VERTEX_COUNT,
  NA_X,
  NA_Y,
  NA_Z,
	NA_BONE_INDEX, // boneindex
	NA_U,         // u
	NA_USE_32BITINDEXES, // use32bitindexes
	NA_USE_SHARED_VERTICES, // usesahredvertices
	NA_V,         // v
	NA_V1,        // v1
	NA_V2,        // v2
	NA_V3,        // v3
	NA_VALUE,     // value
	NA_VERTEX_INDEX, // vertexindex
	NA_WEIGHT,       // weight
	NA_POSITIONS,    // positions
	NA_NORMALS,      // normals
	NA_COLORS_DIFFUSE, // color_diffuse
	NA_COUNT, // count
	NA_MATERIAL, // material
	NA_NAME, // name
	NA_OPERATION_TYPE, // operationtype
	NA_TEXTURE_COORDS, // texture_coords
	NA_TEXTURE_COORD_DIMENSIONS_0, // texture_coord_dimensions_0

  NA_LAST
};

NodeElement getNodeElement(const char *e)
{
  NodeElement ret = NE_LAST;
  if ( strcmp(e,"mesh") == 0 )
    ret = NE_MESH;
  else if ( strcmp(e,"sharedgeometry") == 0 )
    ret = NE_SHARED_GEOMETRY;
  else if ( strcmp(e,"vertexbuffer") == 0 )
    ret = NE_VERTEX_BUFFER;
  else if ( strcmp(e,"vertex") == 0 )
    ret = NE_VERTEX;
  else if ( strcmp(e,"position") == 0 )
    ret = NE_POSITION;
  else if ( strcmp(e,"normal") == 0 )
    ret = NE_NORMAL;
  else if ( strcmp(e,"boneassignments") == 0 )
    ret = NE_BONEASSIGNMENTS;
  else if ( strcmp(e,"color_diffuse") == 0 )
    ret = NE_COLOR_DIFFUSE;
  else if ( strcmp(e,"colour_diffuse") == 0 )
    ret = NE_COLOR_DIFFUSE;
  else if ( strcmp(e,"face") == 0 )
    ret = NE_FACE;
  else if (strcmp(e,"texcoord") == 0 )
    ret = NE_TEXCOORD;
  else if ( strcmp(e,"vertexboneassignment") == 0 )
    ret = NE_VERTEX_BONE_ASSIGNMENT;
  else if ( strcmp(e,"faces") == 0 )
    ret = NE_FACES;
  else if ( strcmp(e,"skeletonlink") == 0 )
    ret = NE_SKELETON_LINK;
  else if ( strcmp(e,"submesh") == 0 )
    ret = NE_SUBMESH;
  else if ( strcmp(e,"submeshes") == 0 )
    ret = NE_SUBMESHES;
  else
    printf("Unknown element: %s\r\n", e );

  return ret;
};

NodeAttribute getNodeAttribute(const char *a)
{
  NodeAttribute ret = NA_LAST;
  if ( strcmp(a,"vertexcount") == 0 )
    ret = NA_VERTEX_COUNT;
  else if (strcmp(a,"x") == 0 )
    ret = NA_X;
  else if ( strcmp(a,"y") == 0 )
    ret = NA_Y;
  else if ( strcmp(a,"z") == 0 )
    ret = NA_Z;
  else if ( strcmp(a,"boneindex") == 0 )
    ret = NA_BONE_INDEX;
  else if ( strcmp(a,"u") == 0 )
    ret = NA_U;
	else if ( strcmp(a,"v") == 0 )
		ret = NA_V;
  else if ( strcmp(a,"v1") == 0 )
    ret = NA_V1;
  else if ( strcmp(a,"v2") == 0 )
    ret = NA_V2;
  else if ( strcmp(a,"v3") == 0 )
    ret = NA_V3;
  else if ( strcmp(a,"value") == 0 )
    ret = NA_VALUE;
  else if ( strcmp(a,"vertexindex") == 0 )
    ret = NA_VERTEX_INDEX;
  else if ( strcmp(a,"weight") == 0 )
    ret = NA_WEIGHT;
	else if ( strcmp(a,"positions") == 0 )
		ret = NA_POSITIONS;
	else if ( strcmp(a,"normals") == 0 )
		ret = NA_NORMALS;
	else if ( strcmp(a,"colors_diffuse") == 0 )
		ret = NA_COLORS_DIFFUSE;
	else if ( strcmp(a,"colours_diffuse") == 0 )
		ret = NA_COLORS_DIFFUSE;
  else if ( strcmp(a,"count") == 0 )
    ret = NA_COUNT;
  else if ( strcmp(a,"material") == 0 )
    ret = NA_MATERIAL;
  else if ( strcmp(a,"name") == 0 )
    ret = NA_NAME;
  else if ( strcmp(a,"operationtype") == 0 )
    ret = NA_OPERATION_TYPE;
  else if ( strcmp(a,"texture_coord_dimensions_0") == 0 )
    ret = NA_TEXTURE_COORD_DIMENSIONS_0;
	else if ( strcmp(a,"texture_coords") == 0 )
		ret = NA_TEXTURE_COORDS;
	else if ( strcmp(a,"usesharedvertices") == 0 )
		ret = NA_USE_SHARED_VERTICES;
	else if ( strcmp(a,"use32bitindexes") == 0 )
		ret = NA_USE_32BITINDEXES;
  else
    printf("Unknown attribute: %s\r\n", a );

  return ret;
}



class MeshImportOgre : public MeshImporter
{
public:
  MeshImportOgre(void)
  {
    mCallback = 0;
  }

  virtual const char * getExtension(int index)  // report the default file name extension for this mesh type.
  {
    return ".xml";
  }

  virtual const char * getDescription(int index)  // report the default file name extension for this mesh type.
  {
    return "Ogre3d XML Mesh Files";
  }


  virtual bool importMesh(const char *fname,const void *data,unsigned int dlen,MeshImportInterface *callback,const char *options)
  {
    bool ret = false;

    mDisplay = false;
    for (int i=0; i<NE_LAST; i++)
      mShow[i] = true;
    mCallback = callback;

    if ( data && mCallback )
    {
  		TINYXML::TiXmlDocument *doc = MEMALLOC_NEW(TINYXML::TiXmlDocument);
  		bool ok = doc->LoadFile(fname,data,dlen);
  		if ( ok )
  		{
        mCallback->importAssetName(fname,0);
  			Traverse(doc,0);
  			ret = true;
  		}

      MEMALLOC_DELETE(TINYXML::TiXmlDocument,doc);

    }

    return ret;
  }

	void Traverse(TINYXML::TiXmlNode *node,int depth)
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

	void Process(TINYXML::TiXmlNode *node,int depth)
	{

		const char *value = node->Value();

		ProcessNode(node->Type(),value,depth);

		TINYXML::TiXmlElement *element = node->ToElement(); // is there an element?  Yes, traverse it's attribute key-pair values.

		if ( element )
		{
			TINYXML::TiXmlAttribute *atr = element->FirstAttribute();
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

    if ( ntype != TINYXML::TiXmlNode::ELEMENT )
    {
      mDisplay = true;
    }
    else
    {
      mDisplay = false;
    }

		switch ( ntype )
		{
			case TINYXML::TiXmlNode::ELEMENT:
			case TINYXML::TiXmlNode::DOCUMENT:
				{
					if ( ntype == TINYXML::TiXmlNode::DOCUMENT )
						Display(depth,"Node(DOCUMENT): %s\n", value);
					else
					{
						NodeElement e = getNodeElement(value);
            if ( e != NE_LAST )
            {
              mDisplay = mShow[e];
              mShow[e] = false;
            }
            else
            {
              mDisplay = true;
            }
            // insert code here...
						Display(depth,"Node(ELEMENT): %s\n", value);
            
            switch ( e )
            {
              case NE_MESH:
                break;
              case NE_SHARED_GEOMETRY:
                break;
              case NE_VERTEX_BUFFER:
                break;
              case NE_VERTEX:
                break;
              case NE_POSITION:
                break;
              case NE_NORMAL:
                break;
          	  case NE_BONEASSIGNMENTS:
                break;
          	  case NE_COLOR_DIFFUSE:
                break;
          	  case NE_FACE:
                break;
          	  case NE_TEXCOORD:
                break;
          	  case NE_VERTEX_BONE_ASSIGNMENT:
                break;
            }
					}
				}
				break;
			case TINYXML::TiXmlNode::TEXT:
				Display(depth,"Node(TEXT): %s\n", value);
				break;
			case TINYXML::TiXmlNode::COMMENT:
				Display(depth,"Node(COMMENT): %s\n", value);
				break;
			case TINYXML::TiXmlNode::DECLARATION:
				Display(depth,"Node(DECLARATION): %s\n", value);
				break;
			case TINYXML::TiXmlNode::UNKNOWN:
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

    NodeAttribute a = getNodeAttribute(aname);

    switch ( a )
    {
      case NA_VERTEX_COUNT:
        break;
      case NA_X:
        break;
      case NA_Y:
        break;
      case NA_Z:
        break;
	    case NA_BONE_INDEX:
        break;
	    case NA_U:
        break;
	    case NA_USE_32BITINDEXES:
        break;
	    case NA_USE_SHARED_VERTICES:
        break;
	    case NA_V:
        break;
	    case NA_V1:
        break;
	    case NA_V2:
        break;
	    case NA_V3:
        break;
	    case NA_VALUE:
        break;
	    case NA_VERTEX_INDEX:
        break;
	    case NA_WEIGHT:
        break;
			case NA_POSITIONS:    // positions
				break;
			case NA_NORMALS:
				break;
			case NA_COLORS_DIFFUSE:
				break;
    }

	}

	void Display(int depth,const char * fmt,...)
	{
#if DEBUG_LOG
    if ( mDisplay )
    {
  		for (int i=0; i<depth; i++)
  		{
  			printf("  ");
  		}
  		char wbuff[8192];
  		_vsnprintf(wbuff, 8191, fmt, (char *)(&fmt+1));
  		printf("%s", wbuff);
    }
#endif
	}





private:
  bool                 mDisplay;
  bool                 mShow[NE_LAST];
  MeshImportInterface *mCallback;

};

MeshImporter * createMeshImportOgre(void)
{
  MeshImportOgre *m = MEMALLOC_NEW(MeshImportOgre);
  return static_cast< MeshImporter *>(m);
}

void         releaseMeshImportOgre(MeshImporter *iface)
{
  MeshImportOgre *m = static_cast< MeshImportOgre *>(iface);
  MEMALLOC_DELETE(MeshImportOgre,m);
}

};// end of namespace
