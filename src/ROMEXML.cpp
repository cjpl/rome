// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXML
//
//  XML file access.
//
//  $Log$
//  Revision 1.11  2004/09/17 16:16:18  schneebeli_m
//  runing on linux
//
//  Revision 1.10  2004/09/17 15:59:16  schneebeli_m
//  xml write not from libxml2
//
//  Revision 1.9  2004/09/16 23:50:47  midas
//  Indent five spaces per level in XML file
//
//  Revision 1.8  2004/09/16 23:48:25  midas
//  Added Log in header
//
//////////////////////////////////////////////////////////////////////////
#if defined ( __linux__ )
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define O_TEXT 0 
#endif

#if defined( _MSC_VER )
#include <io.h>
#include <fcntl.h>
#endif

#include <time.h>
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include <ROMEXML.h>

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
   reader = xmlNewTextReaderFilename(file);
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
   const xmlChar* val = xmlTextReaderValue(reader);
   if (val!=NULL && type==3) {
      value = (char*)val;
      return true;
   }
   value = defaultValue;
   xmlFree((void*)val);
   return false;
}

bool ROMEXML::OpenFileForWrite(const char* file) {
   ROMEString line;
   char *str;
   time_t now;

   xmlFile = open(file, O_WRONLY | O_CREAT | O_TRUNC | O_TEXT, 0644);

   if (xmlFile == -1) {
      fprintf(stderr, "Unable to open %s\n", file);
      return false;
   }

   // write XML header
   line = "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
   write(xmlFile, line.Data(), line.Length());
   time(&now);
   str = ctime(&now);
   line.SetFormatted("<!-- created by ROME on %s -->\n", str);
   write(xmlFile, line.Data(), line.Length());

   xmlLevel = 0;
   xmlElementIsOpen = false;

   xmlStack = new TObjArray(100);
   return true;
}

void ROMEXML::XmlEncode(ROMEString &src)
{
   int i;
   ROMEString dst;
   ROMEString p;

   for (i = 0; i < src.Length(); i++) {
      switch (src[i]) {
      case '<':
         dst.Append("&lt;");
         break;
      case '>':
         dst.Append("&gt;");
         break;
      case '&':
         dst.Append("&amp;");
         break;
      case '\"':
         dst.Append("&quot;");
         break;
      case '\'':
         dst.Append("&apos;");
         break;
      default:
         dst.Append(src[i]);
      }
   }

   src = dst;
}

bool ROMEXML::StartElement(const char* name) 
{
   int i;
   ROMEString line;
   ROMEString name_enc;

   if (xmlElementIsOpen) {
      write(xmlFile, ">\n", 2);
      xmlElementIsOpen = false;
   }

   for (i=0 ; i<xmlLevel ; i++)
      line.Append("     ");
   line.Append("<");
   name_enc = name;
   XmlEncode(name_enc);
   line.Append(name_enc);

   // put element on stack
   xmlStack->AddAt(((TObject*)new ROMEString(name_enc)),xmlLevel);
   xmlLevel++;

   xmlElementIsOpen = true;

   if (write(xmlFile, line.Data(), line.Length()) != line.Length())
      return false;
   return true;
}

bool ROMEXML::EndElement() {
   int i;
   ROMEString line;

   xmlLevel--;

   if (xmlElementIsOpen) {
      xmlElementIsOpen = false;
      if (write(xmlFile, "/>\n", 3) != 3)
         return false;
      return true;
   }

   for (i=0 ; i<xmlLevel ; i++)
      line.Append("     ");
   line.Append("</");
   line.Append(((ROMEString*)xmlStack->At(xmlLevel))->Data());
   delete (ROMEString*)xmlStack->At(xmlLevel);
   line.Append(">\n");

   if (write(xmlFile, line.Data(), line.Length()) != line.Length())
      return false;
   return true;
}

bool ROMEXML::EndDocument() {
   int i, level;
   ROMEString line;

   if (xmlElementIsOpen) {
      xmlElementIsOpen = false;
      if (write(xmlFile, ">\n", 2) != 2)
         return false;
   }

   // close remaining open levels
   for (level=xmlLevel-1 ; level>= 0 ; level--) {
      for (i=0 ; i<level ; i++)
         line.Append("     ");
      line.Append("</");
      line.Append(((ROMEString*)xmlStack->At(level))->Data());
      delete (ROMEString*)xmlStack->At(level);
      line.Append(">\n");

      if (write(xmlFile, line.Data(), line.Length()) != line.Length())
         return false;
   }

   delete xmlStack;

   close(xmlFile);
   return true;
}

bool ROMEXML::WriteAttribute(const char* name,const char* value) {
   ROMEString name_enc;

   if (!xmlElementIsOpen)
      return false;

   write(xmlFile, " ", 1);
   name_enc = name;
   XmlEncode(name_enc);
   write(xmlFile, name_enc.Data(), name_enc.Length());
   write(xmlFile, "=\"", 2);

   name_enc = value;
   XmlEncode(name_enc);
   write(xmlFile, name_enc.Data(), name_enc.Length());
   write(xmlFile, "\"", 1);

   return true;
}

bool ROMEXML::WriteElement(const char* name,const char* value) {
   int i;
   ROMEString line;
   ROMEString name_enc;
   ROMEString value_enc;

   if (xmlElementIsOpen) {
      xmlElementIsOpen = false;
      if (write(xmlFile, ">\n", 2) != 2)
         return false;
   }

   for (i=0 ; i<xmlLevel ; i++)
      line.Append("     ");
   line.Append("<");
   name_enc = name;
   XmlEncode(name_enc);
   line.Append(name_enc);
   line.Append(">");

   value_enc = value;
   XmlEncode(value_enc);
   line.Append(value_enc);

   line.Append("</");
   line.Append(name_enc);
   line.Append(">\n");

   if (write(xmlFile, line.Data(), line.Length()) != line.Length())
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


