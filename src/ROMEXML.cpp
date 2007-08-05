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
:fRootNode(0)
,fCurrentNode(0)
,fLastNode(0)
,fNodeType(0)
,fNodeDepth(0)
,fEndTag(kFALSE)
,fFileName("")
,fFullFileName("")
,fPath("")
,fWriter(0)
{
}

//______________________________________________________________________________
ROMEXML::~ROMEXML()
{
   if (fRootNode != 0) {
      mxml_free_tree(fRootNode);
      fRootNode = 0;
   }
}

// read
//______________________________________________________________________________
Bool_t ROMEXML::OpenFileForRead(const char* file)
{
   char error[240];
   fRootNode = mxml_parse_file(const_cast<char*>(file), error, sizeof(error));
   if (fRootNode == 0) {
      ROMEPrint::Error("%s\n", error);
      return false;
   }
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\', '/');
   if (fFileName.Last('/') == -1) {
      fPath = "";
   } else {
      fPath = fFileName(0, fFileName.Last('/'));
   }
   fFileName = fFileName(fFileName.Last('/') + 1, fFileName.Length());
   fCurrentNode = fRootNode;
   fLastNode = 0;
   fNodeType = 0;
   fNodeDepth = 0;
   fEndTag = false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::NextLine()
{
   if (fEndTag) {
      if (fCurrentNode == fRootNode) {
         return false;
      }
      fLastNode = fCurrentNode;
      fCurrentNode = fCurrentNode->parent;
      int nextChild = IndexOfChildNode(fCurrentNode, fLastNode) + 1;
      if (nextChild == fCurrentNode->n_children) {
         fNodeDepth--;
      } else {
         fEndTag = false;
         fNodeType = 1;
         fNodeDepth++;
         fCurrentNode = &fCurrentNode->child[nextChild];
      }
   } else if (fCurrentNode->n_children>0) {
      fNodeType = 1;
      fNodeDepth++;
      fCurrentNode = &fCurrentNode->child[0];
   } else {
      fEndTag = true;
      fNodeType = 15;
      fNodeDepth--;
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(const char* name, ROMEString& value, ROMEString& defaultValue) const
{
   return GetAttribute(name, value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(ROMEString& name, ROMEString& value, const char* defaultValue) const
{
   return GetAttribute(name.Data(), value, defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(ROMEString& name, ROMEString& value, ROMEString& defaultValue) const
{
   return GetAttribute(name.Data(), value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetAttribute(const char* name, ROMEString& value, const char* defaultValue) const
{
   value = mxml_get_attribute(fCurrentNode, const_cast<char*>(name));
   if (value == 0) {
      value = defaultValue;
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetValue(ROMEString& value, ROMEString& defaultValue) const
{
   return GetValue(value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetValue(ROMEString& value, const char* /*defaultValue*/) const
{
   value = mxml_get_value(fCurrentNode);
   return true;
}

// write
//______________________________________________________________________________
Bool_t ROMEXML::OpenFileForWrite(const char* file)
{
   fWriter = mxml_open_file(file);
   if (fWriter == 0) {
      return false;
   }
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\', '/');
   if (fFileName.Last('/') == -1) {
      fPath = "";
   } else {
      fPath = fFileName(0, fFileName.Last('/'));
   }
   fFileName = fFileName(fFileName.Last('/') + 1, fFileName.Length());
   return true;
}

//______________________________________________________________________________
Int_t ROMEXML::SetTranslate(Int_t flag) const
{
   return mxml_set_translate(fWriter, flag);
}

//______________________________________________________________________________
Bool_t ROMEXML::StartElement(const char* name) const
{
   if (!mxml_start_element(fWriter, name)) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::EndElement() const
{
   if (!mxml_end_element(fWriter)) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::EndDocument() const
{
   if (!mxml_close_file(fWriter)) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteAttribute(const char* name, const char* value) const
{
   if (!mxml_write_attribute(fWriter, name, value)) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteEmptyLine() const
{
   return mxml_write_empty_line(fWriter) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteElement(const char* name, const char* value) const
{
   if (!mxml_start_element(fWriter, name)) {
      return false;
   }
   if (!mxml_write_value(fWriter, value)) {
      return false;
   }
   if (!mxml_end_element(fWriter)) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteValue(const char* value) const
{
   if (!mxml_write_value(fWriter, value)) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::WriteComment(const char* text) const
{
   if (!mxml_write_comment(fWriter, text)) {
      return false;
   }
   return true;
}

// path
//______________________________________________________________________________
Bool_t ROMEXML::OpenFileForPath(const char* file)
{
   char error[240];
   fRootNode = mxml_parse_file(const_cast<char*>(file), error, sizeof(error));
   if (fRootNode == 0) {
      ROMEPrint::Error("%s\n", error);
      return false;
   }
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\', '/');
   if (fFileName.Last('/') == -1) {
      fPath = "";
   } else {
      fPath = fFileName(0, fFileName.Last('/'));
   }
   fFileName = fFileName(fFileName.Last('/') + 1, fFileName.Length());
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::OpenBufferForPath(const char* buffer)
{
   if (fRootNode != 0) {
      mxml_free_tree(fRootNode);
      fRootNode = 0;
   }
   char error[240];
   fRootNode = mxml_parse_buffer(buffer, error, sizeof(error));
   if (fRootNode == 0) {
      ROMEPrint::Error("%s\n", error);
      return false;
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::ExistPath(const char* path) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return true;
}

//______________________________________________________________________________
Int_t ROMEXML::NumberOfOccurrenceOfPath(const char* path) const
{
   PMXML_NODE *node = 0;
   int numberOfNodes = mxml_find_nodes(fRootNode, const_cast<char*>(path), &node);
   if (numberOfNodes < 0) {
      numberOfNodes = 0;
   }
   free(node);
   return numberOfNodes;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path, ROMEString& name, ROMEString& value, ROMEString& defaultValue) const
{
   return GetPathAttribute(path.Data(), name.Data(), value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path, ROMEString& name, ROMEString& value, const char* defaultValue) const
{
   return GetPathAttribute(path.Data(), name.Data(), value, defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path, const char* name, ROMEString& value, ROMEString& defaultValue) const
{
   return GetPathAttribute(path.Data(), name, value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path, ROMEString& name, ROMEString& value, ROMEString& defaultValue) const
{
   return GetPathAttribute(path, name.Data(), value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(ROMEString& path, const char* name, ROMEString& value, const char* defaultValue) const
{
   return GetPathAttribute(path.Data(), name, value, defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path, ROMEString& name, ROMEString& value, const char* defaultValue) const
{
   return GetPathAttribute(path, name.Data(), value, defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path, const char* name, ROMEString& value, ROMEString& defaultValue) const
{
   return GetPathAttribute(path, name, value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathAttribute(const char* path, const char* name, ROMEString& value, const char* defaultValue) const
{
   char *tmp;
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      value = defaultValue;
      return false;
   }
   tmp = mxml_get_attribute(node, const_cast<char*>(name));
   if (tmp == 0) {
      value = defaultValue;
      return false;
   }
   value = tmp;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(ROMEString& path, ROMEString& value, ROMEString& defaultValue) const
{
   return GetPathValue(path.Data(), value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(ROMEString& path, ROMEString& value, const char* defaultValue) const
{
   return GetPathValue(path.Data(), value, defaultValue);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(const char* path, ROMEString& value, ROMEString& defaultValue) const
{
   return GetPathValue(path, value, defaultValue.Data());
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValue(const char* path, ROMEString& value, const char* defaultValue) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      value = defaultValue;
      return false;
   }
   value = mxml_get_value(node);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValues(ROMEString& path, ROMEStrArray* values) const
{
   return GetPathValues(path.Data(), values);
}

//______________________________________________________________________________
Bool_t ROMEXML::GetPathValues(const char* path, ROMEStrArray* values) const
{
   PMXML_NODE *node = 0;
   int numberOfNodes = mxml_find_nodes(fRootNode, const_cast<char*>(path), &node);
   if (numberOfNodes < 0) {
      return false;
   }
   for (int i = 0; i < numberOfNodes; i++) {
      values->AddAtAndExpand(node[i]->value, i);
   }
   free(node);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathAttributeValue(const char* path, const char* name, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_replace_attribute_value(node, const_cast<char*>(name), const_cast<char*>(value)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathAttributeName(const char* path, const char* name, const char* newName) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_replace_attribute_name(node, const_cast<char*>(name), const_cast<char*>(newName)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathName(const char* path, const char* name) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_replace_node_name(node, const_cast<char*>(name)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::ReplacePathValue(const char* path, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_replace_node_value(node, const_cast<char*>(value)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::DeletePathAttribute(const char* path, const char* name) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_delete_attribute(node, const_cast<char*>(name)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::DeletePath(const char* path) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_delete_node(node) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathAttribute(const char* path, const char* name, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_add_attribute(node, const_cast<char*>(name), const_cast<char*>(value)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathNextElement(const char* path, const char* name, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   int nextChild = IndexOfChildNode(node->parent, node) + 1;
   return mxml_add_node_at(node->parent, const_cast<char*>(name), const_cast<char*>(value), nextChild) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathPrevElement(const char* path, const char* name, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   int nextChild = IndexOfChildNode(node->parent, node);
   return mxml_add_node_at(node->parent, const_cast<char*>(name), const_cast<char*>(value), nextChild) != 0;
}

//______________________________________________________________________________
Int_t ROMEXML::NewPathLastElement(const char* path, const char* name, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_add_node(node->parent, const_cast<char*>(name), const_cast<char*>(value)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::NewPathChildElement(const char* path, const char* name, const char* value) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return mxml_add_node(node, const_cast<char*>(name), const_cast<char*>(value)) != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::HasPathChildren(const char* path) const
{
   PMXML_NODE node = mxml_find_node(fRootNode, const_cast<char*>(path));
   if (node == 0) {
      return false;
   }
   return node->n_children != 0;
}

//______________________________________________________________________________
Bool_t ROMEXML::WritePathFile(const char* file)
{
   mxml_write_tree(const_cast<char*>(file), fRootNode);
   fFullFileName = file;
   fFileName = file;
   fFileName.ReplaceAll('\\', '/');
   if (fFileName.Last('/') == -1) {
      fPath = "";
   } else {
      fPath = fFileName(0, fFileName.Last('/'));
   }
   fFileName = fFileName(fFileName.Last('/') + 1, fFileName.Length());
   return true;
}

// node
//______________________________________________________________________________
PMXML_NODE ROMEXML::GetPathNode(const char* path) const
{
   return mxml_find_node(fRootNode, const_cast<char*>(path));
}

//______________________________________________________________________________
PMXML_NODE ROMEXML::GetRootNode() const
{
   return fRootNode;
}

//______________________________________________________________________________
PMXML_NODE ROMEXML::GetSubNode(PMXML_NODE node, Int_t i) const
{
   return mxml_subnode(node, i);
}

//______________________________________________________________________________
const char* ROMEXML::GetNodeValue(PMXML_NODE node) const
{
   return mxml_get_value(node);
}

//______________________________________________________________________________
Int_t ROMEXML::GetNodeIntValue(PMXML_NODE node) const
{
   char *cstop;
   return strtol(mxml_get_value(node), &cstop, 10);
}

//______________________________________________________________________________
Double_t ROMEXML::GetNodeDoubleValue(PMXML_NODE node) const
{
   char *cstop;
   return strtod(mxml_get_value(node), &cstop);
}

//______________________________________________________________________________
const char* ROMEXML::GetNodeName(PMXML_NODE node) const
{
   return mxml_get_name(node);
}

//______________________________________________________________________________
const char* ROMEXML::GetNodeAttribute(PMXML_NODE node, const char* name) const
{
   return mxml_get_attribute(node, const_cast<char*>(name));
}

// auxiliary
//______________________________________________________________________________
Int_t ROMEXML::IndexOfChildNode(PMXML_NODE node, PMXML_NODE childNode) const
{
   for (int i = 0; i < node->n_children; i++) {
      if (&node->child[i] == childNode) {
         return i;
      }
   }
   return -1;
}
