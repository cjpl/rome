#ifndef ROMEXML_H
#define ROMEXML_H

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>

class ROMEXML
{
protected:
   xmlTextReaderPtr reader;
   xmlTextWriterPtr writer;
   xmlDocPtr doc;
   xmlXPathContextPtr xpathCtx; 
   xmlXPathObjectPtr xpathObj; 

public:
   ROMEXML();
   ~ROMEXML();

   // read
   bool OpenFileForRead(char* file);
   bool NextLine() { return xmlTextReaderRead(reader)!=0; };
   char* GetName() { return (char*)xmlTextReaderConstName(reader); };
   int GetType() { return xmlTextReaderNodeType(reader); };
   int GetDepth() { return xmlTextReaderDepth(reader); };
   bool isEmpty() { return xmlTextReaderIsEmptyElement(reader)!=0; };
   bool GetAttribute(char* name,char* value);
   bool GetValue(char* value);
   // write
   bool OpenFileForWrite(char* file);
   bool StartElement(char* name);
   bool EndElement();
   bool EndDocument();
   bool WriteAttribute(char* name,char* value);
   bool WriteElement(char* name,char* value);
   // path
   bool OpenFileForPath(char* file);
   bool ExistPath(char* path);
   bool GetPathAttribute(char* path,char* name,char* value);
   bool GetPathValue(char* path,char* value);
   bool ReplacePathAttributeValue(char* path,char* name,char* value);
   bool ReplacePathAttributeName(char* path,char* name,char* newName);
   bool ReplacePathName(char* path,char* name);
   bool ReplacePathValue(char* path,char* value);
   bool NewPathAttribute(char* path,char* name,char* value);
   bool NewPathElement(char* path,char* name,char* value);
   bool NewPathChildElement(char* path,char* name,char* value);
   bool WritePathFile(char* file);
};

#endif   // ROMEXML_H
