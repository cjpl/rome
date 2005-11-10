// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEXML
//
//  XML file access.
//
//  $Id$
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
}
ROMEXML::~ROMEXML() {
   if (rootNode!=NULL) {
      mxml_free_tree(rootNode);
      rootNode = NULL;
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
   fFileName = file;
   fFileName.ReplaceAll('\\','/');
   if (fFileName.Last('/')==-1)
      fPath = "";
   else
      fPath = fFileName(0,fFileName.Last('/'));
   fFileName = fFileName(fFileName.Last('/')+1,fFileName.Length());
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
   return GetAttribute(name,value,defaultValue.Data());
}
bool ROMEXML::GetAttribute(ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetAttribute(name.Data(),value,defaultValue);
}
bool ROMEXML::GetAttribute(ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetAttribute(name.Data(),value,defaultValue.Data());
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
   return GetValue(value,defaultValue.Data());
}
bool ROMEXML::GetValue(ROMEString& value,const char* defaultValue) {
   value = mxml_get_value(currentNode);
   return true;
}

// write
bool ROMEXML::OpenFileForWrite(const char* file) {
   writer = mxml_open_file(file);
   if (writer==NULL)
      return false;
   fFileName = file;
   fFileName.ReplaceAll('\\','/');
   if (fFileName.Last('/')==-1)
      fPath = "";
   else
      fPath = fFileName(0,fFileName.Last('/'));
   fFileName = fFileName(fFileName.Last('/')+1,fFileName.Length());
   return true;
}

int ROMEXML::SetTranslate(int flag) {
   return mxml_set_translate(writer,flag);
}

bool ROMEXML::StartElement(const char* name)
{
   if (!mxml_start_element(writer,name))
      return false;
   return true;
}

bool ROMEXML::EndElement() {
   if (!mxml_end_element(writer))
      return false;
   return true;
}

bool ROMEXML::EndDocument() {
   if (!mxml_close_file(writer))
      return false;
   return true;
}

bool ROMEXML::WriteAttribute(const char* name,const char* value) {
   if (!mxml_write_attribute(writer,name,value))
      return false;
   return true;
}

bool ROMEXML::WriteElement(const char* name,const char* value) {
   if (!mxml_start_element(writer,name))
      return false;
   if (!mxml_write_value(writer,value))
      return false;
   if (!mxml_end_element(writer))
      return false;
   return true;
}
bool ROMEXML::WriteValue(const char* value) {
   if (!mxml_write_value(writer,value))
      return false;
   return true;
}
bool ROMEXML::WriteComment(const char* text) {
   if (!mxml_write_comment(writer,text))
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
   fFileName = file;
   fFileName.ReplaceAll('\\','/');
   if (fFileName.Last('/')==-1)
      fPath = "";
   else
      fPath = fFileName(0,fFileName.Last('/'));
   fFileName = fFileName(fFileName.Last('/')+1,fFileName.Length());
   return true;
}

bool ROMEXML::OpenBufferForPath(char* buffer) {
   if (rootNode!=NULL) {
      mxml_free_tree(rootNode);
      rootNode = NULL;
   }
   char error[240];
   rootNode = mxml_parse_buffer(buffer, error, sizeof(error));
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
   return GetPathAttribute(path.Data(),name.Data(),value,defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path.Data(),name.Data(),value,defaultValue);
}
bool ROMEXML::GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path.Data(),name,value,defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path,name.Data(),value,defaultValue.Data());
}
bool ROMEXML::GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path.Data(),name,value,defaultValue);
}
bool ROMEXML::GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path,name.Data(),value,defaultValue);
}
bool ROMEXML::GetPathAttribute(const char* path,const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path,name,value,defaultValue.Data());
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
   return GetPathValue(path.Data(),value,defaultValue.Data());
}
bool ROMEXML::GetPathValue(ROMEString& path,ROMEString& value,const char* defaultValue) {
   return GetPathValue(path.Data(),value,defaultValue);
}
bool ROMEXML::GetPathValue(const char* path,ROMEString& value,ROMEString& defaultValue) {
   return GetPathValue(path,value,defaultValue.Data());
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
   return GetPathValues(path.Data(),values);
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

bool ROMEXML::DeletePathAttribute(const char* path,const char* name) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_delete_attribute(node,(char*)name)!=0;
}
bool ROMEXML::DeletePath(const char* path) {
   PMXML_NODE node = mxml_find_node(rootNode,(char*)path);
   if (node==NULL)
      return false;
   return mxml_delete_node(node)!=0;
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

// node
PMXML_NODE ROMEXML::GetPathNode(const char* path) {
   return mxml_find_node(rootNode,(char*)path);
}

PMXML_NODE ROMEXML::GetRootNode() {
   return rootNode;
}
PMXML_NODE ROMEXML::GetSubNode(PMXML_NODE node,int i) {
   return mxml_subnode(node,i);
}
const char* ROMEXML::GetNodeValue(PMXML_NODE node) {
   return mxml_get_value(node);
}
int ROMEXML::GetNodeIntValue(PMXML_NODE node) {
   char *cstop;
   return strtol(mxml_get_value(node),&cstop,10);
}
double ROMEXML::GetNodeDoubleValue(PMXML_NODE node) {
   char *cstop;
   return strtod(mxml_get_value(node),&cstop);
}
const char* ROMEXML::GetNodeName(PMXML_NODE node) {
   return mxml_get_name(node);
}
const char* ROMEXML::GetNodeAttribute(PMXML_NODE node,const char* name) {
   return mxml_get_attribute(node,(char*)name);
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

