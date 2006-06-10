/********************************************************************
  ROMEXML.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEXML_H
#define ROMEXML_H

#if defined( R__VISUAL_CPLUSPLUS )
extern "C" {
#endif
#include "mxml.h"
#if defined( R__VISUAL_CPLUSPLUS )
}
#endif

#include <TObject.h>
#include "ROMEString.h"
class ROMEStrArray;

class ROMEXML : public TObject
{
protected:
   // read & path
   PMXML_NODE rootNode;
   PMXML_NODE currentNode;
   PMXML_NODE lastNode;
   Int_t      nodeType;
   Int_t      nodeDepth;
   Bool_t     endTag;
   ROMEString fFileName;
   ROMEString fPath;

   // write
   MXML_WRITER *writer;

   Int_t   IndexOfChildNode(PMXML_NODE node,PMXML_NODE childNode);
public:
   ROMEXML();
   virtual ~ROMEXML();

   const char *GetFileName() { return fFileName.Data(); };
   const char *GetPath() { return fPath.Data(); };

   Bool_t      isReaderOpen() { return rootNode!=NULL; };
   Bool_t      isWriterOpen() { return writer!=NULL; };
   Bool_t      isPathOpen() { return rootNode!=NULL; };

   // read
   Bool_t      OpenFileForRead(const char* file);
   Bool_t      NextLine();
   char       *GetName() { return currentNode->name; };
   Int_t       GetType() { return nodeType; };
   Int_t       GetDepth() { return nodeDepth; };
   Bool_t      isEmpty() { return currentNode->value[0]==0; };
   Bool_t      GetAttribute(ROMEString& name,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetAttribute(const char* name,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetAttribute(ROMEString& name,ROMEString& value,const char* defaultValue="");
   Bool_t      GetAttribute(const char* name,ROMEString& value,const char* defaultValue="");
   Bool_t      GetValue(ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetValue(ROMEString& value,const char* defaultValue="");
   // write
   Bool_t      OpenFileForWrite(const char* file);
   Int_t       SetTranslate(Int_t flag);
   Bool_t      StartElement(const char* name);
   Bool_t      EndElement();
   Bool_t      EndDocument();
   Bool_t      WriteAttribute(const char* name,const char* value);
   Bool_t      WriteElement(const char* name,const char* value);
   Bool_t      WriteValue(const char* value);
   Bool_t      WriteComment(const char* text);
   // path
   Bool_t      OpenFileForPath(const char* file);
   Bool_t      OpenBufferForPath(char* buffer);
   Bool_t      ExistPath(const char* path);
   Int_t       NumberOfOccurrenceOfPath(const char* path);
   Bool_t      GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,const char* defaultValue="");
   Bool_t      GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,const char* defaultValue="");
   Bool_t      GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,const char* defaultValue="");
   Bool_t      GetPathAttribute(const char* path,const char* name,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetPathAttribute(const char* path,const char* name,ROMEString& value,const char* defaultValue="");
   Bool_t      GetPathValue(ROMEString& path,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetPathValue(ROMEString& path,ROMEString& value,const char* defaultValue="");
   Bool_t      GetPathValue(const char* path,ROMEString& value,ROMEString& defaultValue);
   Bool_t      GetPathValue(const char* path,ROMEString& value,const char* defaultValue="");
   Bool_t      GetPathValues(ROMEString& path,ROMEStrArray* values);
   Bool_t      GetPathValues(const char* path,ROMEStrArray* values);
   Bool_t      ReplacePathAttributeValue(const char* path,const char* name,const char* value);
   Bool_t      ReplacePathAttributeName(const char* path,const char* name,const char* newName);
   Bool_t      ReplacePathName(const char* path,const char* name);
   Bool_t      ReplacePathValue(const char* path,const char* value);
   Bool_t      DeletePathAttribute(const char* path,const char* name);
   Bool_t      DeletePath(const char* path);
   Bool_t      NewPathAttribute(const char* path,const char* name,const char* value);
   Bool_t      NewPathNextElement(const char* path,const char* name,const char* value);
   Bool_t      NewPathPrevElement(const char* path,const char* name,const char* value);
   Int_t       NewPathLastElement(const char* path,const char* name,const char* value);
   Bool_t      NewPathChildElement(const char* path,const char* name,const char* value);
   Bool_t      HasPathChildren(const char* path);
   Bool_t      WritePathFile(const char* file);
   // node
   PMXML_NODE  GetPathNode(const char* path);
   PMXML_NODE  GetRootNode();
   PMXML_NODE  GetSubNode(PMXML_NODE node,Int_t i);
   const char *GetNodeValue(PMXML_NODE node);
   Int_t       GetNodeIntValue(PMXML_NODE node);
   Double_t    GetNodeDoubleValue(PMXML_NODE node);
   const char *GetNodeName(PMXML_NODE node);
   const char *GetNodeAttribute(PMXML_NODE node,const char* name);

   ClassDef(ROMEXML, 0) // Interface to XML files
};

#endif   // ROMEXML_H
