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

bool ROMEXML::OpenFileForRead(char* file) {
   reader = xmlReaderForFile(file, NULL, 0);
   if (reader == NULL) {
      fprintf(stderr, "Unable to open %s\n", file);
      return false;
   }
   return true;
}

int ROMEXML::GetAttribute(char* name,char* value,unsigned int valueSize) {
   const xmlChar* val = xmlTextReaderGetAttribute(reader,(xmlChar*)name);
   if (val==NULL) {
      xmlFree((void*)val);
      return 0;
   }
   if (valueSize<strlen((const char*)val)) {
      xmlFree((void*)val);
      return -1;
   }
   strcpy(value,(char*)val);
   xmlFree((void*)val);
   return 1;
}

char* ROMEXML::GetAttribute(char* name,char* defaultValue) {
   const xmlChar* val = xmlTextReaderGetAttribute(reader,(xmlChar*)name);
   if (val==NULL) {
      if (defaultValue==0) return NULL;
      char* value = new char[strlen(defaultValue)+1];
      strcpy(value,defaultValue);
      xmlFree((void*)val);
      return value;
   }
   char* value = new char[strlen((char*)val)+1];
   strcpy(value,(char*)val);
   xmlFree((void*)val);
   return value;
}

int ROMEXML::GetValue(char* value,unsigned int valueSize) {
   if (xmlTextReaderRead(reader)==0)
      return 0;
   int type = xmlTextReaderNodeType(reader);
   const xmlChar* val = xmlTextReaderConstValue(reader);
   if (val!=NULL && type==3) {
      if (valueSize<strlen((const char*)val)) {
         xmlFree((void*)val);
         return -1;
      }
      strcpy(value,(char*)val);
      return 1;
   }
   xmlFree((void*)val);
   return 0;
}

char* ROMEXML::GetValue(char* defaultValue) {
   if (xmlTextReaderRead(reader)==0) {
      char* value = new char[strlen(defaultValue)+1];
      strcpy(value,defaultValue);
      return value;
   }
   int type = xmlTextReaderNodeType(reader);
   const xmlChar* val = xmlTextReaderConstValue(reader);
   if (val!=NULL && type==3) {
      char* value = new char[strlen((char*)val)+1];
      strcpy(value,(char*)val);
      return value;
   }
   char* value = new char[strlen(defaultValue)+1];
   strcpy(value,defaultValue);
   xmlFree((void*)val);
   return value;
}

bool ROMEXML::OpenFileForWrite(char* file) {
   writer = xmlNewTextWriterFilename(file, 0);
   if (writer == NULL) {
      fprintf(stderr, "Unable to open %s\n", file);
      return false;
   }
   xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
   xmlTextWriterWriteFormatComment(writer,"%s"," edited with ROME");
   return true;
}

bool ROMEXML::StartElement(char* name) {
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

bool ROMEXML::WriteAttribute(char* name,char* value) {
   xmlTextWriterWriteAttribute(writer, BAD_CAST name,BAD_CAST value);
   return true;
}

bool ROMEXML::WriteElement(char* name,char* value) {
   xmlTextWriterWriteElement(writer, BAD_CAST name, BAD_CAST value);
   return true;
}

bool ROMEXML::OpenFileForPath(char* file) {
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

bool ROMEXML::ExistPath(char* path) {
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

bool ROMEXML::GetPathAttribute(char* path,char* name,char* value) {
   char *pathI = new char[strlen(path)+20];
   sprintf(pathI,"%s/attribute::*",path);
   xpathObj = xmlXPathEvalExpression((const xmlChar*)pathI, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", pathI);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc);
      delete pathI;
      return false;
   }
   if (xpathObj->nodesetval==NULL) {
      delete pathI;
      return false;
   }
   for (int i=0;i<xpathObj->nodesetval->nodeNr;i++) {
      if (!strcmp((char*)xpathObj->nodesetval->nodeTab[i]->name,name)) {
         strcpy(value,(char*)xpathObj->nodesetval->nodeTab[i]->children->content);
         delete pathI;
         return true;
      }
   }
   delete pathI;
   return false;
}

bool ROMEXML::GetPathValue(char* path,char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   strcpy(value,(char*)xpathObj->nodesetval->nodeTab[0]->children->content);
   return true;
}

bool ROMEXML::ReplacePathAttributeValue(char* path,char* name,char* value) {
   char *pathI = new char[strlen(path)+20];
   sprintf(pathI,"%s/attribute::*",path);
   xpathObj = xmlXPathEvalExpression((const xmlChar*)pathI, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", pathI);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   for (int i=0;i<xpathObj->nodesetval->nodeNr;i++) {
      if (!strcmp((char*)xpathObj->nodesetval->nodeTab[i]->name,name)) {
         strcpy((char*)xpathObj->nodesetval->nodeTab[i]->children->content,value);
         delete pathI;
         return true;
      }
   }
   delete pathI;
   return false;
}

bool ROMEXML::ReplacePathAttributeName(char* path,char* name,char* newName) {
   char *pathI = new char[strlen(path)+20];
   sprintf(pathI,"%s/attribute::*",path);
   xpathObj = xmlXPathEvalExpression((const xmlChar*)pathI, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", pathI);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   for (int i=0;i<xpathObj->nodesetval->nodeNr;i++) {
      if (!strcmp((char*)xpathObj->nodesetval->nodeTab[i]->name,name)) {
         strcpy((char*)xpathObj->nodesetval->nodeTab[i]->name,newName);
         delete pathI;
         return true;
      }
   }
   delete pathI;
   return false;
}

bool ROMEXML::ReplacePathName(char* path,char* name) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   strcpy((char*)xpathObj->nodesetval->nodeTab[0]->name,name);
   return true;
}

bool ROMEXML::ReplacePathValue(char* path,char* value) {
   xpathObj = xmlXPathEvalExpression((const xmlChar*)path, xpathCtx);
   if(xpathObj == NULL) {
      fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", path);
      xmlXPathFreeContext(xpathCtx); 
      xmlFreeDoc(doc); 
      return false;
   }
   strcpy((char*)xpathObj->nodesetval->nodeTab[0]->children->content,value);
   return true;
}

bool ROMEXML::NewPathAttribute(char* path,char* name,char* value) {
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

bool ROMEXML::NewPathNextElement(char* path,char* name,char* value) {
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

bool ROMEXML::NewPathPrevElement(char* path,char* name,char* value) {
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

bool ROMEXML::NewPathChildElement(char* path,char* name,char* value) {
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

bool ROMEXML::HasPathChildren(char* path) {
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

bool ROMEXML::WritePathFile(char* file) {
   xmlSaveFormatFileEnc(file,doc,"ISO-8859-1",1);
   return true;
}
