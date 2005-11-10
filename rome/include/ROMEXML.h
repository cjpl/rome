/********************************************************************
  ROMEXML.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEXML_H
#define ROMEXML_H

#if defined( R__VISUAL_CPLUSPLUS )
extern "C" {
#endif
#include <mxml.h>
#if defined( R__VISUAL_CPLUSPLUS )
}
#endif

#include <TObjArray.h>
#include <Riostream.h>

#include <ROMEString.h>
#include <ROMEStrArray.h>

class ROMEXML
{
protected:
   // read & path
   PMXML_NODE rootNode;
   PMXML_NODE currentNode;
   PMXML_NODE lastNode;
   int nodeType;
   int nodeDepth;
   bool endTag;
   ROMEString fFileName;
   ROMEString fPath;

   // write
   MXML_WRITER *writer;

   int   IndexOfChildNode(PMXML_NODE node,PMXML_NODE childNode);
public:
   ROMEXML();
   ~ROMEXML();

   const char* GetFileName() { return fFileName.Data(); };
   const char* GetPath() { return fPath.Data(); };

   bool  isReaderOpen() { return rootNode!=NULL; };
   bool  isWriterOpen() { return writer!=NULL; };
   bool  isPathOpen() { return rootNode!=NULL; };

   // read
   bool  OpenFileForRead(const char* file);
   bool  NextLine();
   char* GetName() { return currentNode->name; };
   int   GetType() { return nodeType; };
   int   GetDepth() { return nodeDepth; };
   bool  isEmpty() { return currentNode->value[0]==0; };
   bool  GetAttribute(ROMEString& name,ROMEString& value,ROMEString& defaultValue);
   bool  GetAttribute(const char* name,ROMEString& value,ROMEString& defaultValue);
   bool  GetAttribute(ROMEString& name,ROMEString& value,const char* defaultValue="");
   bool  GetAttribute(const char* name,ROMEString& value,const char* defaultValue="");
   bool  GetValue(ROMEString& value,ROMEString& defaultValue);
   bool  GetValue(ROMEString& value,const char* defaultValue="");
   // write
   bool  OpenFileForWrite(const char* file);
   int   SetTranslate(int flag);
   bool  StartElement(const char* name);
   bool  EndElement();
   bool  EndDocument();
   bool  WriteAttribute(const char* name,const char* value);
   bool  WriteElement(const char* name,const char* value);
   bool  WriteValue(const char* value);
   bool  WriteComment(const char* text);
   // path
   bool  OpenFileForPath(const char* file);
   bool  OpenBufferForPath(char* buffer);
   bool  ExistPath(const char* path);
   int   NumberOfOccurrenceOfPath(const char* path);
   bool  GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,ROMEString& defaultValue);
   bool  GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,const char* defaultValue="");
   bool  GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,ROMEString& defaultValue);
   bool  GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,ROMEString& defaultValue);
   bool  GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,const char* defaultValue="");
   bool  GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,const char* defaultValue="");
   bool  GetPathAttribute(const char* path,const char* name,ROMEString& value,ROMEString& defaultValue);
   bool  GetPathAttribute(const char* path,const char* name,ROMEString& value,const char* defaultValue="");
   bool  GetPathValue(ROMEString& path,ROMEString& value,ROMEString& defaultValue);
   bool  GetPathValue(ROMEString& path,ROMEString& value,const char* defaultValue="");
   bool  GetPathValue(const char* path,ROMEString& value,ROMEString& defaultValue);
   bool  GetPathValue(const char* path,ROMEString& value,const char* defaultValue="");
   bool  GetPathValues(ROMEString& path,ROMEStrArray* values);
   bool  GetPathValues(const char* path,ROMEStrArray* values);
   bool  ReplacePathAttributeValue(const char* path,const char* name,const char* value);
   bool  ReplacePathAttributeName(const char* path,const char* name,const char* newName);
   bool  ReplacePathName(const char* path,const char* name);
   bool  ReplacePathValue(const char* path,const char* value);
   bool  DeletePathAttribute(const char* path,const char* name);
   bool  DeletePath(const char* path);
   bool  NewPathAttribute(const char* path,const char* name,const char* value);
   bool  NewPathNextElement(const char* path,const char* name,const char* value);
   bool  NewPathPrevElement(const char* path,const char* name,const char* value);
   int   NewPathLastElement(const char* path,const char* name,const char* value);
   bool  NewPathChildElement(const char* path,const char* name,const char* value);
   bool  HasPathChildren(const char* path);
   bool  WritePathFile(const char* file);
   // node
   PMXML_NODE  GetPathNode(const char* path);
   PMXML_NODE  GetRootNode();
   PMXML_NODE  GetSubNode(PMXML_NODE node,int i);
   const char* GetNodeValue(PMXML_NODE node);
   int         GetNodeIntValue(PMXML_NODE node);
   double      GetNodeDoubleValue(PMXML_NODE node);
   const char* GetNodeName(PMXML_NODE node);
   const char* GetNodeAttribute(PMXML_NODE node,const char* name);
};

#endif   // ROMEXML_H
