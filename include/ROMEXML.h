/********************************************************************
  ROMEXML.h, M. Schneebeli PSI

  $Log$
  Revision 1.13  2005/03/23 11:48:47  schneebeli_m
  link error on linux

  Revision 1.12  2005/03/23 09:06:11  schneebeli_m
  libxml replaced by mxml, Bool SP error

  Revision 1.11  2004/11/12 17:35:18  schneebeli_m
  fast xml database

  Revision 1.10  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.9  2004/10/14 09:53:41  schneebeli_m
  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag

  Revision 1.8  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

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

   // write
   int xmlFile, xmlLevel; 
   bool xmlElementIsOpen;
   TObjArray *xmlStack;


   void  XmlEncode(ROMEString &str);
   int   IndexOfChildNode(PMXML_NODE node,PMXML_NODE childNode);
public:
   ROMEXML();
   ~ROMEXML();

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
   bool  StartElement(const char* name);
   bool  EndElement();
   bool  EndDocument();
   bool  WriteAttribute(const char* name,const char* value);
   bool  WriteElement(const char* name,const char* value);
   // path
   bool  OpenFileForPath(const char* file);
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
   bool  NewPathAttribute(const char* path,const char* name,const char* value);
   bool  NewPathNextElement(const char* path,const char* name,const char* value);
   bool  NewPathPrevElement(const char* path,const char* name,const char* value);
   int   NewPathLastElement(const char* path,const char* name,const char* value);
   bool  NewPathChildElement(const char* path,const char* name,const char* value);
   bool  HasPathChildren(const char* path);
   bool  WritePathFile(const char* file);
};

#endif   // ROMEXML_H
