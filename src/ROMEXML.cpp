// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEXML                                                             //
//                                                                      //
//  XML file access.
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#if defined ( __linux__ )
#include <string.h>
#endif

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <libxml/xpath.h>
#define MY_ENCODING "ISO-8859-1"

#include <ROMEXML.h>
#include <ROMEString.h>
#include <Riostream.h>

ROMEXML::ROMEXML() {
   reader=NULL;
   xpathCtx=NULL;
   doc=NULL;
   xpathObj=NULL;
}
ROMEXML::~ROMEXML() {
   if (reader!=NULL) {
      xmlFreeTextReader(reader); 
   }
   if (xpathObj!=NULL) {
      xmlXPathFreeObject(xpathObj);
   }
   if (xpathCtx!=NULL) {
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc);
   }
}

bool ROMEXML::OpenFileForRead(const char* file) {
   reader = xmlReaderForFile(file, NULL, 0);
   if (reader == NULL) {
      fprintf(stderr, "Unable to open %s\n", file);
      return false;
   }
   return true;
}

bool ROMEXML::GetAttribute(const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetAttribute(name,value,(char*)defaultValue.Data());
}
bool ROMEXML::GetAttribute(ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetAttribute((char*)name.Data(),value,defaultValue);
}
bool ROMEXML::GetAttribute(ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetAttribute((char*)name.Data(),value,(char*)defaultValue.Data());
}
bool ROMEXML::GetAttribute(const char* name,ROMEString& value,const char* defaultValue) {
   const xmlChar* val = xmlTextReaderGetAttribute(reader,(xmlChar*)name);
   if (val==NULL) {
      value = defaultValue;
      xmlFree((void*)val);
      return false;
   }
   value = (char*)val;
   xmlFree((void*)val);
   return true;
}

bool ROMEXML::GetValue(ROMEString& value,ROMEString& defaultValue) {
   return GetValue(value,(char*)defaultValue.Data());
}
bool ROMEXML::GetValue(ROMEString& value,const char* defaultValue) {
   if (xmlTextReaderRead(reader)==0) {
      value = defaultValue;
      return false;
   }
   int type = xmlTextReaderNodeType(reader);
   const xmlChar* val = xmlTextReaderConstValue(reader);
   if (val!=NULL && type==3) {
      value = (char*)val;
      return true;
   }
   value = defaultValue;
   xmlFree((void*)val);
   return false;
}

bool ROMEXML::OpenFileForWrite(const char* file) {
   writer = xmlNewTextWriterFilename(file, 0);
   if (writer == NULL) {
      fprintf(stderr, "Unable to open %s\n", file);
      return false;
   }
   xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
   xmlTextWriterWriteFormatComment(writer,"%s"," edited with ROME");
   return true;
}

bool ROMEXML::StartElement(const char* name) {
   xmlTextWriterStartElement(writer, BAD_CAST name);
   return true;
}

bool ROMEXML::EndElement() {
   xmlTextWriterEndElement(writer);
   return true;
}

bool ROMEXML::EndDocument() {
   xmlTextWriterEndDocument(writer);
   xmlFreeTextWriter(writer);
   xmlCleanupParser();
   xmlMemoryDump();
   return true;
}

bool ROMEXML::WriteAttribute(const char* name,const char* value) {
   xmlTextWriterWriteAttribute(writer, BAD_CAST name,BAD_CAST value);
   return true;
}

bool ROMEXML::WriteElement(const char* name,const char* value) {
   xmlTextWriterWriteElement(writer, BAD_CAST name, BAD_CAST value);
   return true;
}

bool ROMEXML::OpenFileForPath(const char* file) {
   // Load XML document
   doc = xmlParseFile(file);
   if (doc == NULL) {
  	   fprintf(stderr, "Error: unable to parse file \"%s\"\n", file);
  	   return false;
   }

   // Create xpath evaluation context
   xpathCtx = xmlXPathNewContext(doc);
   if(xpathCtx == NULL) {
      fprintf(stderr,"Error: unable to create new XPath context\n");
      xmlFreeDoc(doc); 
      return false;
   }
   return true;
}

bool ROMEXML::ExistPath(const char* path) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   if (xpathObj->nodesetval->nodeNr==0)
      return false;
   return true;
}

bool ROMEXML::GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute((char*)path.Data(),(char*)name.Data(),value,(char*)defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute((char*)path.Data(),(char*)name.Data(),value,defaultValue);
}
bool ROMEXML::GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute((char*)path.Data(),name,value,(char*)defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path,(char*)name.Data(),value,(char*)defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute((char*)path.Data(),name,value,defaultValue);
}
bool ROMEXML::GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path,(char*)name.Data(),value,defaultValue);
}
bool ROMEXML::GetPathAttribute(const char* path,const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path,name,value,(char*)defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(const char* path,const char* name,ROMEString& value,const char* defaultValue) {
   ROMEString pathI(path);
   pathI.Append("/attribute::*");
   xpathObj = xmlXPathEvalExpression((const xmlChar*)pathI.Data(), xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", pathI.Data());
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc);
      value = defaultValue;
      return false;
   }
   if (xpathObj->nodesetval==NULL) {
      value = defaultValue;
      return false;
   }
   for (int i=0;i<xpathObj->nodesetval->nodeNr;i++) {
      if (!strcmp((char*)xpathObj->nodesetval->nodeTab[i]->name,name)) {
         value = (char*)xpathObj->nodesetval->nodeTab[i]->children->content;
         return true;
      }
   }
   value = defaultValue;
   return false;
}

bool ROMEXML::GetPathValue(ROMEString& path,ROMEString& value,ROMEString& defaultValue) {
   return GetPathValue((char*)path.Data(),value,(char*)defaultValue.Data());
}
bool ROMEXML::GetPathValue(ROMEString& path,ROMEString& value,const char* defaultValue) {
   return GetPathValue((char*)path.Data(),value,defaultValue);
}
bool ROMEXML::GetPathValue(const char* path,ROMEString& value,ROMEString& defaultValue) {
   return GetPathValue(path,value,(char*)defaultValue.Data());
}
bool ROMEXML::GetPathValue(const char* path,ROMEString& value,const char* defaultValue) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      value = defaultValue;
      return false;
   }
   value = (char*)xpathObj->nodesetval->nodeTab[0]->children->content;
   return true;
}

bool ROMEXML::ReplacePathAttributeValue(const char* path,const char* name,const char* value) {
   ROMEString pathI(path);
   pathI.Append("/attribute::*");
   xpathObj = xmlXPathEvalExpression((const xmlChar*)pathI.Data(), xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", pathI.Data());
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   for (int i=0;i<xpathObj->nodesetval->nodeNr;i++) {
      if (!strcmp((char*)xpathObj->nodesetval->nodeTab[i]->name,name)) {
         if (sizeof((char*)xpathObj->nodesetval->nodeTab[i]->children->content)<=strlen(value))
            return false;
         strcpy((char*)xpathObj->nodesetval->nodeTab[i]->children->content,value);
         return true;
      }
   }
   return false;
}

bool ROMEXML::ReplacePathAttributeName(const char* path,const char* name,const char* newName) {
   ROMEString pathI(path);
   pathI.Append("/attribute::*");
   xpathObj = xmlXPathEvalExpression((const xmlChar*)pathI.Data(), xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", pathI.Data());
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   for (int i=0;i<xpathObj->nodesetval->nodeNr;i++) {
      if (!strcmp((char*)xpathObj->nodesetval->nodeTab[i]->name,name)) {
         if (sizeof((char*)xpathObj->nodesetval->nodeTab[i]->name)<=strlen(newName))
            return false;
         strcpy((char*)xpathObj->nodesetval->nodeTab[i]->name,newName);
         return true;
      }
   }
   return false;
}

bool ROMEXML::ReplacePathName(const char* path,const char* name) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   if (sizeof((char*)xpathObj->nodesetval->nodeTab[0]->name)<=strlen(name))
      return false;
   strcpy((char*)xpathObj->nodesetval->nodeTab[0]->name,name);
   return true;
}

bool ROMEXML::ReplacePathValue(const char* path,const char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   if (sizeof((char*)xpathObj->nodesetval->nodeTab[0]->children->content)<=strlen(value))
      return false;
   strcpy((char*)xpathObj->nodesetval->nodeTab[0]->children->content,value);
   return true;
}

bool ROMEXML::NewPathAttribute(const char* path,const char* name,const char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   xmlNewProp(xpathObj->nodesetval->nodeTab[0],(xmlChar*)name,(xmlChar*)value);
   return true;
}

bool ROMEXML::NewPathNextElement(const char* path,const char* name,const char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   xmlNodePtr node = xmlNewNode(NULL,(xmlChar*)name);
   xmlNodeSetContent(node,(xmlChar*)value);
   xmlAddNextSibling(xpathObj->nodesetval->nodeTab[0],node);
   return true;
}

bool ROMEXML::NewPathPrevElement(const char* path,const char* name,const char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   xmlNodePtr node = xmlNewNode(NULL,(xmlChar*)name);
   xmlNodeSetContent(node,(xmlChar*)value);
   xmlAddPrevSibling(xpathObj->nodesetval->nodeTab[0],node);
   return true;
}

bool ROMEXML::NewPathChildElement(const char* path,const char* name,const char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   xmlNewChild(xpathObj->nodesetval->nodeTab[0],NULL,(xmlChar*)name,(xmlChar*)value);
   return true;
}

bool ROMEXML::HasPathChildren(const char* path) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   if (xpathObj->nodesetval->nodeTab[0]->children==NULL) return false;
   return true;
}

bool ROMEXML::WritePathFile(const char* file) {
   xmlSaveFormatFileEnc(file,doc,"ISO-8859-1",1);
   return true;
}
