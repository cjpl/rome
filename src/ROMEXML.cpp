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

#if defined( _MSC_VER )
#include <io.h>
#include <fcntl.h>
#endif

#include <time.h>
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>
#define MY_ENCODING "ISO-8859-1"

#include <ROMEXML.h>
#include <ROMEString.h>
#include <Riostream.h>

#ifndef HAVE_STRLCPY
/*---- strlcpy and strlcat to avoid buffer overflow ----------------*/

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless size == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
int strlcpy(char *dst, const char *src, int size)
{
   char *d = dst;
   const char *s = src;
   int n = size;

   /* Copy as many bytes as will fit */
   if (n != 0 && --n != 0) {
      do {
         if ((*d++ = *s++) == 0)
            break;
      } while (--n != 0);
   }

   /* Not enough room in dst, add NUL and traverse rest of src */
   if (n == 0) {
      if (size != 0)
         *d = '\0';             /* NUL-terminate dst */
      while (*s++);
   }

   return (s - src - 1);        /* count does not include NUL */
}

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless size <= strlen(dst)).
 * Returns strlen(src) + MIN(size, strlen(initial dst)).
 * If retval >= size, truncation occurred.
 */
int strlcat(char *dst, const char *src, int size)
{
   char *d = dst;
   const char *s = src;
   int n = size;
   int dlen;

   /* Find the end of dst and adjust bytes left but don't go past end */
   while (n-- != 0 && *d != '\0')
      d++;
   dlen = d - dst;
   n = size - dlen;

   if (n == 0)
      return (dlen + strlen(s));
   while (*s != '\0') {
      if (n != 1) {
         *d++ = *s;
         n--;
      }
      s++;
   }
   *d = '\0';

   return (dlen + (s - src));   /* count does not include NUL */
}
#endif


ROMEXML::ROMEXML() {
   reader=NULL;
   xpathCtx=NULL;
   doc=NULL;
   xpathObj=NULL;
   xmlStack=NULL;
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
   char line[256], str[256];
   time_t now;

   hFile = open(file, O_WRONLY | O_CREAT | O_TRUNC | O_TEXT, 0644);

   if (hFile == -1) {
      fprintf(stderr, "Unable to open %s\n", file);
      return false;
   }

   // write XML header
   strcpy(line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
   write(hFile, line, strlen(line));
   time(&now);
   strcpy(str, ctime(&now));
   str[24] = 0; // strip CR
   sprintf(line, "<!-- created by ROME on %s -->\n", str);
   write(hFile, line, strlen(line));

   xmlLevel = 0;
   xmlElementIsOpen = false;

   return true;
}

void ROMEXML::xmlEncode(char *src, int size)
{
   int i;
   char *dst, *p;

   dst = (char *)malloc(size);
   if (dst == NULL)
      return;

   *dst = 0;
   for (i = 0; i < (int) strlen(src); i++) {
      switch (src[i]) {
      case '<':
         strlcat(dst, "&lt;", size);
         break;
      case '>':
         strlcat(dst, "&gt;", size);
         break;
      case '&':
         strlcat(dst, "&amp;", size);
         break;
      case '\"':
         strlcat(dst, "&quot;",size);
         break;
      case '\'':
         strlcat(dst, "&apos;",size);
         break;
      default:
         if ((int)strlen(dst) >= size) {
            free(dst);
            return;
         }
         p = dst+strlen(dst);
         *p = src[i];
         *(p+1) = 0;
      }
   }

   strlcpy(src, dst, size);
}

bool ROMEXML::StartElement(const char* name) 
{
   int i;
   char line[1000], name_enc[256];

   if (xmlElementIsOpen) {
      write(hFile, ">\n", 2);
      xmlElementIsOpen = false;
   }

   line[0] = 0;
   for (i=0 ; i<xmlLevel ; i++)
      strlcat(line, "  ", sizeof(line));
   strlcat(line, "<", sizeof(line));
   strcpy(name_enc, name);
   xmlEncode(name_enc, sizeof(name_enc));
   strlcat(line, name_enc, sizeof(line));

   // put element on stack
   if (xmlStack == NULL)
      xmlStack = (char *)malloc(256);
   else
      xmlStack = (char *)realloc(xmlStack, 256*(xmlLevel+1));
   strlcpy(xmlStack+256*xmlLevel, name_enc, 256);
   xmlLevel++;

   xmlElementIsOpen = true;

   if (write(hFile, line, strlen(line)) != strlen(line))
      return false;
   return true;
}

bool ROMEXML::EndElement() {
   int i;
   char line[1000];

   xmlLevel--;

   if (xmlElementIsOpen) {
      xmlElementIsOpen = false;
      if (write(hFile, "/>\n", 3) != 3)
         return false;
      return true;
   }

   line[0] = 0;
   for (i=0 ; i<xmlLevel ; i++)
      strlcat(line, "  ", sizeof(line));
   strlcat(line, "</", sizeof(line));
   strlcat(line, xmlStack+256*xmlLevel, sizeof(line));
   strlcat(line, ">\n", sizeof(line));

   if (write(hFile, line, strlen(line)) != strlen(line))
      return false;
   return true;
}

bool ROMEXML::EndDocument() {
   int i, level;
   char line[2000];

   if (xmlElementIsOpen) {
      xmlElementIsOpen = false;
      if (write(hFile, ">\n", 2) != 2)
         return false;
   }

   // close remaining open levels
   for (level=xmlLevel-1 ; level>= 0 ; level--) {
      line[0] = 0;
      for (i=0 ; i<level ; i++)
         strlcat(line, "     ", sizeof(line));
      strlcat(line, "</", sizeof(line));
      strlcat(line, xmlStack+256*level, sizeof(line));
      strlcat(line, ">\n", sizeof(line));

      if (write(hFile, line, strlen(line)) != strlen(line))
         return false;
   }

   free(xmlStack);
   xmlStack = NULL;

   close(hFile);
   return true;
}

bool ROMEXML::WriteAttribute(const char* name,const char* value) {
   char name_enc[256];

   if (!xmlElementIsOpen)
      return false;

   write(hFile, " ", 1);
   strlcpy(name_enc, name, sizeof(name_enc));
   xmlEncode(name_enc, sizeof(name_enc));
   write(hFile, name_enc, strlen(name_enc));
   write(hFile, "=\"", 2);

   strlcpy(name_enc, value, sizeof(name_enc));
   xmlEncode(name_enc, sizeof(name_enc));
   write(hFile, name_enc, strlen(name_enc));
   write(hFile, "\"", 1);

   return true;
}

bool ROMEXML::WriteElement(const char* name,const char* value) {
   int i;
   char line[2000], name_enc[256], value_enc[1000];

   if (xmlElementIsOpen) {
      xmlElementIsOpen = false;
      if (write(hFile, ">\n", 2) != 2)
         return false;
   }

   line[0] = 0;
   for (i=0 ; i<xmlLevel ; i++)
      strlcat(line, "     ", sizeof(line));
   strlcat(line, "<", sizeof(line));
   strcpy(name_enc, name);
   xmlEncode(name_enc, sizeof(name_enc));
   strlcat(line, name_enc, sizeof(line));
   strlcat(line, ">", sizeof(line));

   strcpy(value_enc, value);
   xmlEncode(value_enc, sizeof(value_enc));
   strlcat(line, value_enc, sizeof(line));

   strlcat(line, "</", sizeof(line));
   strlcat(line, name_enc, sizeof(line));
   strlcat(line, ">\n", sizeof(line));

   if (write(hFile, line, strlen(line)) != strlen(line))
      return false;

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
