// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXML
//
//  XML file access.
//
//  $Log$
//  Revision 1.19  2005/03/23 09:06:11  schneebeli_m
//  libxml replaced by mxml, Bool SP error
//
//  Revision 1.18  2005/02/21 21:29:07  sawada
//
//  Changed OS specifying macros
//  Support for DEC,Ultrix,FreeBSD,Solaris
//
//  Revision 1.17  2004/12/06 16:03:02  sawada
//  code cleanup (tab -> space)
//
//  Revision 1.16  2004/12/02 17:46:43  sawada
//  Macintosh port
//
//  Revision 1.15  2004/11/12 17:35:18  schneebeli_m
//  fast xml database
//
//  Revision 1.14  2004/11/11 12:55:28  schneebeli_m
//  Implemented XML database with new path rules
//
//  Revision 1.13  2004/10/14 09:53:41  schneebeli_m
//  ROME configuration file format changed and extended, Folder Getter changed : GetXYZObject -> GetXYZ, tree compression level and fill flag
//
//  Revision 1.12  2004/09/17 16:21:45  schneebeli_m
//  example updated
//
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
#include <RConfig.h>
#if defined( R__UNIX )
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define O_TEXT 0 
#endif

#if defined( R__VISUAL_CPLUSPLUS )
#include <io.h>
#include <fcntl.h>
#endif

#include <time.h>
#include <stdio.h>

#include <ROMEXML.h>


ROMEXML::ROMEXML() {
   rootNode=NULL;
   currentNode=NULL;

   xmlStack=NULL;
}
ROMEXML::~ROMEXML() {
   if (rootNode!=NULL) {
      mxml_free_tree(rootNode); 
   }

}

// read
bool ROMEXML::OpenFileForRead(const char* file) {
   char error[240];
   rootNode = mxml_parse_file((char*)file, error, sizeof(error));
   if (rootNode == NULL) {
      cout << error << endl;
      return false;
   }
   currentNode = rootNode;
   lastNode = NULL;
   nodeType = 0;
   nodeDepth = 0;
   endTag = false;
   return true;
}
bool ROMEXML::NextLine() {
   if (endTag) {
      if (currentNode==rootNode)
         return false;
      lastNode = currentNode;
      currentNode = currentNode->parent;
      int nextChild = IndexOfChildNode(currentNode,lastNode)+1;
      if (nextChild==currentNode->n_children) {
         nodeDepth--;
      }
      else {
         endTag = false;
         nodeType = 1;
         nodeDepth++;
         currentNode = &currentNode->child[nextChild];
      }
   }
   else if (currentNode->n_children>0) {
      nodeType = 1;
      nodeDepth++;
      currentNode = &currentNode->child[0];
   }
   else {
      endTag = true;
      nodeType = 15;
      nodeDepth--;
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
   value = mxml_get_attribute(currentNode,(char*)name);
   if (value==NULL) {
      value = defaultValue;
      return false;
   }
   return true;
}

bool ROMEXML::GetValue(ROMEString& value,ROMEString& defaultValue) {
   return GetValue(value,(char*)defaultValue.Data());
}
bool ROMEXML::GetValue(ROMEString& value,const char* defaultValue) {
   value = mxml_get_value(currentNode);
   return true;
}

// write
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

// path
bool ROMEXML::OpenFileForPath(const char* file) {
   char error[240];
   rootNode = mxml_parse_file((char*)file, error, sizeof(error));
   if (rootNode == NULL) {
      cout << error << endl;
      return false;
   }
   return true;
}

bool ROMEXML::ExistPath(const char* path) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return true;
}
int ROMEXML::NumberOfOccurrenceOfPath(const char* path) {
   PMXML_NODE *node = NULL;
   int numberOfNodes = mxml_find_nodes(rootNode,(char*)path,&node);
   if (numberOfNodes<0)
      numberOfNodes = 0;
   free(node);
   return numberOfNodes;
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
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL) {
      value = defaultValue;
      return false;
   }
   value = mxml_get_attribute(node,(char*)name);
   if (value==NULL) {
      value = defaultValue;
      return false;
   }
   return true;
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
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL) {
      value = defaultValue;
      return false;
   }
   value = mxml_get_value(node);
   return true;
}
bool ROMEXML::GetPathValues(ROMEString& path,ROMEStrArray* values) {
   return GetPathValues((char*)path.Data(),values);
}
bool ROMEXML::GetPathValues(const char* path,ROMEStrArray* values) {
   PMXML_NODE *node = NULL;
   int numberOfNodes = mxml_find_nodes(rootNode,(char*)path,&node);
   if (numberOfNodes<0)
      return false;
   for (int i=0;i<numberOfNodes;i++) {
      values->AddAtAndExpand(node[i]->value,i);
   }
   free(node);
   return true;
}

bool ROMEXML::ReplacePathAttributeValue(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_replace_attribute_value(node,(char*)name,(char*)value)!=0;
}

bool ROMEXML::ReplacePathAttributeName(const char* path,const char* name,const char* newName) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_replace_attribute_name(node,(char*)name,(char*)newName)!=0;
}

bool ROMEXML::ReplacePathName(const char* path,const char* name) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_replace_node_name(node,(char*)name)!=0;
}

bool ROMEXML::ReplacePathValue(const char* path,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_replace_node_value(node,(char*)value)!=0;
}

bool ROMEXML::NewPathAttribute(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_add_attribute(node,(char*)name,(char*)value)!=0;
}

bool ROMEXML::NewPathNextElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   int nextChild = IndexOfChildNode(node->parent,node)+1;
   return mxml_add_node_at(node->parent,(char*)name,(char*)value,nextChild)!=0;
}

bool ROMEXML::NewPathPrevElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   int nextChild = IndexOfChildNode(node->parent,node);
   return mxml_add_node_at(node->parent,(char*)name,(char*)value,nextChild)!=0;
}
int ROMEXML::NewPathLastElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_add_node(node->parent,(char*)name,(char*)value)!=0;
}

bool ROMEXML::NewPathChildElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_add_node(node,(char*)name,(char*)value)!=0;
}

bool ROMEXML::HasPathChildren(const char* path) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return node->n_children!=0;
}

bool ROMEXML::WritePathFile(const char* file) {
   mxml_write_tree((char*)file,rootNode);
   return true;
}
// auxiliary
int ROMEXML::IndexOfChildNode(PMXML_NODE node,PMXML_NODE childNode)
{
   for (int i=0;i<node->n_children;i++) {
      if (&node->child[i] == childNode) {
         return i;
      }
   }
   return -1;
}

