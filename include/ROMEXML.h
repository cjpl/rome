#ifndef ROMEXML_H
#define ROMEXML_H

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>

#include <TObjArray.h>
#include <Riostream.h>

#include <ROMEString.h>

#define ENCODING "ISO-8859-1"

class ROMEXML
{
protected:
   xmlTextReaderPtr reader;
   xmlDocPtr doc;
   xmlXPathContextPtr xpathCtx; 
   xmlXPathObjectPtr xpathObj; 

   // variables for "manual" implmentation of TextWriter
   int xmlFile, xmlLevel; 
   bool xmlElementIsOpen;
   TObjArray *xmlStack;


   void  XmlEncode(ROMEString &str);
public:
   ROMEXML();
   ~ROMEXML();

   // read
   bool  OpenFileForRead(const char* file);
   bool  NextLine() { return xmlTextReaderRead(reader)!=0; };
   char* GetName() { return (char*)xmlTextReaderName(reader); };
   int   GetType() { return xmlTextReaderNodeType(reader); };
   int   GetDepth() { return xmlTextReaderDepth(reader); };
   bool  isEmpty() { return xmlTextReaderIsEmptyElement(reader)!=0; };
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
   bool  ReplacePathAttributeValue(const char* path,const char* name,const char* value);
   bool  ReplacePathAttributeName(const char* path,const char* name,const char* newName);
   bool  ReplacePathName(const char* path,const char* name);
   bool  ReplacePathValue(const char* path,const char* value);
   bool  NewPathAttribute(const char* path,const char* name,const char* value);
   bool  NewPathNextElement(const char* path,const char* name,const char* value);
   bool  NewPathPrevElement(const char* path,const char* name,const char* value);
   bool  NewPathChildElement(const char* path,const char* name,const char* value);
   bool  HasPathChildren(const char* path);
   bool  WritePathFile(const char* file);
};

#endif   // ROMEXML_H
