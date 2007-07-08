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

#include <TObjArray.h>
#include "ROMEiostream.h"

#include "ROMEXML.h"
#include "ROMEStrArray.h"

ClassImp(ROMEXML)

//______________________________________________________________________________
ROMEXML::ROMEXML()
:rootNode(0)
,currentNode(0)
,lastNode(0)
,nodeType(0)
,nodeDepth(0)
,endTag(kFALSE)
,fFileName("")
,fFullFileName("")
,fPath("")
,writer(0)
{
}

//______________________________________________________________________________
ROMEXML::~ROMEXML() {
   if (rootNode!=NULL) {
      mxml_free_tree(rootNode);
      rootNode = NULL;
   }
}

// read
//______________________________________________________________________________
Bool_t ROMEXML::OpenFileForRead(const char* file) {
   char error[240];
   rootNode = mxml_parse_file(const_cast<char*>(file), error, sizeof(error));
   if (rootNode == NULL) {
      ROMEPrint::Error("%s\n", error);
      return false;
   }
   fFullFileName = file;
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

//______________________________________________________________________________
Bool_t ROMEXML::NextLine() {
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

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetAttribute(name,value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetAttribute(name.Data(),value,defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetAttribute(name.Data(),value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(const char* name,ROMEString& value,const char* defaultValue) {
   value = mxml_get_attribute(currentNode, const_cast<char*>(name));
   if (value==NULL) {
      value = defaultValue;
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetValue(ROMEString& value,ROMEString& defaultValue) {
   return GetValue(value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetValue(ROMEString& value,const char* /*defaultValue*/) {
   value = mxml_get_value(currentNode);
   return true;
}

// write
//______________________________________________________________________________
Bool_t ROMEXML::OpenFileForWrite(const char* file) {
   writer = mxml_open_file(file);
   if (writer==NULL)
      return false;
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\','/');
   if (fFileName.Last('/')==-1)
      fPath = "";
   else
      fPath = fFileName(0,fFileName.Last('/'));
   fFileName = fFileName(fFileName.Last('/')+1,fFileName.Length());
   return true;
}

//______________________________________________________________________________
Int_t ROMEXML::SetTranslate(Int_t flag) {
   return mxml_set_translate(writer,flag);
}

//______________________________________________________________________________
Bool_t ROMEXML::StartElement(const char* name)
{
   if (!mxml_start_element(writer,name))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::EndElement() {
   if (!mxml_end_element(writer))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::EndDocument() {
   if (!mxml_close_file(writer))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteAttribute(const char* name,const char* value) {
   if (!mxml_write_attribute(writer,name,value))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteEmptyLine() {
   return mxml_write_empty_line(writer)!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteElement(const char* name,const char* value) {
   if (!mxml_start_element(writer,name))
      return false;
   if (!mxml_write_value(writer,value))
      return false;
   if (!mxml_end_element(writer))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteValue(const char* value) {
   if (!mxml_write_value(writer,value))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteComment(const char* text) {
   if (!mxml_write_comment(writer,text))
      return false;
   return true;
}

// path
//______________________________________________________________________________
Bool_t ROMEXML::OpenFileForPath(const char* file) {
   char error[240];
   rootNode = mxml_parse_file(const_cast<char*>(file), error, sizeof(error));
   if (rootNode == NULL) {
      ROMEPrint::Error("%s\n", error);
      return false;
   }
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\','/');
   if (fFileName.Last('/')==-1)
      fPath = "";
   else
      fPath = fFileName(0,fFileName.Last('/'));
   fFileName = fFileName(fFileName.Last('/')+1,fFileName.Length());
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::OpenBufferForPath(char* buffer) {
   if (rootNode!=NULL) {
      mxml_free_tree(rootNode);
      rootNode = NULL;
   }
   char error[240];
   rootNode = mxml_parse_buffer(buffer, error, sizeof(error));
   if (rootNode == NULL) {
      ROMEPrint::Error("%s\n", error);
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::ExistPath(const char* path) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return true;
}

//______________________________________________________________________________
Int_t ROMEXML::NumberOfOccurrenceOfPath(const char* path) {
   PMXML_NODE *node = NULL;
   int numberOfNodes = mxml_find_nodes(rootNode, const_cast<char*>(path), &node);
   if (numberOfNodes<0)
      numberOfNodes = 0;
   free(node);
   return numberOfNodes;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path.Data(),name.Data(),value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path,ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path.Data(),name.Data(),value,defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path.Data(),name,value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path,name.Data(),value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path,const char* name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path.Data(),name,value,defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path,ROMEString& name,ROMEString& value,const char* defaultValue) {
   return GetPathAttribute(path,name.Data(),value,defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path,const char* name,ROMEString& value,ROMEString& defaultValue) {
   return GetPathAttribute(path,name,value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path,const char* name,ROMEString& value,const char* defaultValue) {
   char *tmp;
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL) {
      value = defaultValue;
      return false;
   }
   tmp = mxml_get_attribute(node, const_cast<char*>(name));
   if (tmp==NULL) {
      value = defaultValue;
      return false;
   }
   value = tmp;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(ROMEString& path,ROMEString& value,ROMEString& defaultValue) {
   return GetPathValue(path.Data(),value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(ROMEString& path,ROMEString& value,const char* defaultValue) {
   return GetPathValue(path.Data(),value,defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(const char* path,ROMEString& value,ROMEString& defaultValue) {
   return GetPathValue(path,value,defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(const char* path,ROMEString& value,const char* defaultValue) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL) {
      value = defaultValue;
      return false;
   }
   value = mxml_get_value(node);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValues(ROMEString& path,ROMEStrArray* values) {
   return GetPathValues(path.Data(),values);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValues(const char* path,ROMEStrArray* values) {
   PMXML_NODE *node = NULL;
   int numberOfNodes = mxml_find_nodes(rootNode, const_cast<char*>(path), &node);
   if (numberOfNodes<0)
      return false;
   for (int i=0;i<numberOfNodes;i++) {
      values->AddAtAndExpand(node[i]->value,i);
   }
   free(node);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathAttributeValue(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_replace_attribute_value(node, const_cast<char*>(name), const_cast<char*>(value))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathAttributeName(const char* path,const char* name,const char* newName) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_replace_attribute_name(node, const_cast<char*>(name), const_cast<char*>(newName))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathName(const char* path,const char* name) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_replace_node_name(node, const_cast<char*>(name))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathValue(const char* path,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_replace_node_value(node, const_cast<char*>(value))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::DeletePathAttribute(const char* path,const char* name) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_delete_attribute(node, const_cast<char*>(name))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::DeletePath(const char* path) {
   PMXML_NODE node = mxml_find_node(rootNode,const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_delete_node(node)!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathAttribute(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_add_attribute(node, const_cast<char*>(name), const_cast<char*>(value))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathNextElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   int nextChild = IndexOfChildNode(node->parent,node)+1;
   return mxml_add_node_at(node->parent, const_cast<char*>(name), const_cast<char*>(value), nextChild)!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathPrevElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   int nextChild = IndexOfChildNode(node->parent,node);
   return mxml_add_node_at(node->parent, const_cast<char*>(name), const_cast<char*>(value), nextChild)!=0;
}

//______________________________________________________________________________
Int_t ROMEXML::NewPathLastElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_add_node(node->parent, const_cast<char*>(name), const_cast<char*>(value))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathChildElement(const char* path,const char* name,const char* value) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return mxml_add_node(node, const_cast<char*>(name), const_cast<char*>(value))!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::HasPathChildren(const char* path) {
   PMXML_NODE node = mxml_find_node(rootNode, const_cast<char*>(path));
   if (node==NULL)
      return false;
   return node->n_children!=0;
}

//______________________________________________________________________________
Bool_t ROMEXML::WritePathFile(const char* file) {
   mxml_write_tree(const_cast<char*>(file), rootNode);
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\','/');
   if (fFileName.Last('/')==-1)
      fPath = "";
   else
      fPath = fFileName(0,fFileName.Last('/'));
   fFileName = fFileName(fFileName.Last('/')+1,fFileName.Length());
   return true;
}

// node
//______________________________________________________________________________
PMXML_NODE ROMEXML::GetPathNode(const char* path) {
   return mxml_find_node(rootNode, const_cast<char*>(path));
}

//______________________________________________________________________________
PMXML_NODE ROMEXML::GetRootNode() {
   return rootNode;
}

//______________________________________________________________________________
PMXML_NODE ROMEXML::GetSubNode(PMXML_NODE node,Int_t i) {
   return mxml_subnode(node,i);
}

//______________________________________________________________________________
const char* ROMEXML::GetNodeValue(PMXML_NODE node) {
   return mxml_get_value(node);
}

//______________________________________________________________________________
Int_t ROMEXML::GetNodeIntValue(PMXML_NODE node) {
   char *cstop;
   return strtol(mxml_get_value(node),&cstop,10);
}

//______________________________________________________________________________
Double_t ROMEXML::GetNodeDoubleValue(PMXML_NODE node) {
   char *cstop;
   return strtod(mxml_get_value(node),&cstop);
}

//______________________________________________________________________________
const char* ROMEXML::GetNodeName(PMXML_NODE node) {
   return mxml_get_name(node);
}

//______________________________________________________________________________
const char* ROMEXML::GetNodeAttribute(PMXML_NODE node,const char* name) {
   return mxml_get_attribute(node, const_cast<char*>(name));
}

// auxiliary
//______________________________________________________________________________
Int_t ROMEXML::IndexOfChildNode(PMXML_NODE node,PMXML_NODE childNode)
{
   for (int i=0;i<node->n_children;i++) {
      if (&node->child[i] == childNode) {
         return i;
      }
   }
   return -1;
}

