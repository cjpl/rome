/********************************************************************
  ROMEXML.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEXML_H
#define ROMEXML_H

#if defined( R__VISUAL_CPLUSPLUS )
extern "C" {
#endif
#include "mxml.h"
#if defined( R__VISUAL_CPLUSPLUS )
}
#endif

#include "ROMEString.h"

class ROMEStrArray;

class ROMEXML
{
protected:
   // read & path
   PMXML_NODE   fRootNode;
   PMXML_NODE   fCurrentNode;
   PMXML_NODE   fLastNode;
   Int_t        fNodeType;
   Int_t        fNodeDepth;
   Bool_t       fEndTag;
   ROMEString   fFileName;
   ROMEString   fFullFileName;
   ROMEString   fPath;
   MXML_WRITER *fWriter;   // write

   Int_t   IndexOfChildNode(PMXML_NODE node, PMXML_NODE childNode) const;

private:
   ROMEXML(const ROMEXML &xml); // not implemented
   ROMEXML &operator=(const ROMEXML &rhs); // not implemented

public:
   ROMEXML();
   virtual ~ROMEXML();

   const char *GetFileName() const { return fFileName.Data(); }
   const char *GetFullFileName() const { return fFullFileName.Data(); }
   const char *GetPath() const { return fPath.Data(); }

   Bool_t      isReaderOpen() const { return fRootNode != 0; }
   Bool_t      isWriterOpen() const { return fWriter != 0; }
   Bool_t      isPathOpen() const { return fRootNode != 0; }

   // read
   Bool_t      OpenFileForRead(const char* file);
   Bool_t      NextLine();
   const char *GetName() const { return fCurrentNode->name; }
   Int_t       GetType() const { return fNodeType; }
   Int_t       GetDepth() const { return fNodeDepth; }
   Bool_t      isEmpty() const { return fCurrentNode->value[0]==0; }
   Bool_t      GetValue(ROMEString& value, ROMEString& defaultValue) const { return GetValue(value, defaultValue.Data()); }
   Bool_t      GetValue(ROMEString& value, const char* defaultValue="") const;

   // write
   Bool_t      OpenFileForWrite(const char* file);
   Int_t       SetTranslate(Int_t flag) const { return mxml_set_translate(fWriter, flag); }
   Bool_t      StartElement(const char* name) const { return mxml_start_element(fWriter, name)!=0; }
   Bool_t      EndElement() const { return mxml_end_element(fWriter)!=0; }
   Bool_t      EndDocument() const { return mxml_close_file(fWriter)!=0; }
   Bool_t      WriteAttribute(const char* name, const char* value) const { return mxml_write_attribute(fWriter, name, value)!=0; }
   Bool_t      WriteElement(const char* name, const char* value) const;
   Bool_t      WriteValue(const char* value) const { return mxml_write_value(fWriter, value)!=0; }
   Bool_t      WriteComment(const char* text) const { return mxml_write_comment(fWriter, text)!=0; }
   static void SuppressWritingDate(Bool_t suppress = kTRUE) { mxml_suppress_date(suppress ? 1 : 0); }
   Bool_t      WriteEmptyLine() const { return mxml_write_empty_line(fWriter) != 0; }

   // path
   Bool_t      OpenFileForPath(const char* file);
   Bool_t      OpenBufferForPath(const char* buffer);
   Bool_t      ExistPath(const char* path) const;
   Int_t       NumberOfOccurrenceOfPath(const char* path) const;
   Bool_t      GetPathAttribute(ROMEString& path, ROMEString& name, ROMEString& value,
                                ROMEString& defaultValue) const { return GetPathAttribute(path.Data(), name.Data(), value, defaultValue.Data()); }
   Bool_t      GetPathAttribute(ROMEString& path, ROMEString& name, ROMEString& value,
                                const char* defaultValue="") const { return GetPathAttribute(path.Data(), name.Data(), value, defaultValue); }
   Bool_t      GetPathAttribute(ROMEString& path, const char* name, ROMEString& value,
                                ROMEString& defaultValue) const { return GetPathAttribute(path.Data(), name, value, defaultValue.Data()); }
   Bool_t      GetPathAttribute(const char* path, ROMEString& name, ROMEString& value,
                                ROMEString& defaultValue) const { return GetPathAttribute(path, name.Data(), value, defaultValue.Data()); }
   Bool_t      GetPathAttribute(ROMEString& path, const char* name, ROMEString& value,
                                const char* defaultValue="") const { return GetPathAttribute(path.Data(), name, value, defaultValue); }
   Bool_t      GetPathAttribute(const char* path, ROMEString& name, ROMEString& value,
                                const char* defaultValue="") const { return GetPathAttribute(path, name.Data(), value, defaultValue); }
   Bool_t      GetPathAttribute(const char* path, const char* name, ROMEString& value,
                                ROMEString& defaultValue) const { return GetPathAttribute(path, name, value, defaultValue.Data()); }
   Bool_t      GetPathAttribute(const char* path, const char* name, ROMEString& value,
                                const char* defaultValue="") const;
   Bool_t      GetPathValue(ROMEString& path, ROMEString& value, ROMEString& defaultValue) const { return GetPathValue(path.Data(), value, defaultValue.Data()); }
   Bool_t      GetPathValue(ROMEString& path, ROMEString& value, const char* defaultValue="") const { return GetPathValue(path.Data(), value, defaultValue); }
   Bool_t      GetPathValue(const char* path, ROMEString& value, ROMEString& defaultValue) const { return GetPathValue(path, value, defaultValue.Data()); }
   Bool_t      GetPathValue(const char* path, ROMEString& value, const char* defaultValue="") const;
   Bool_t      GetPathValues(ROMEString& path ,ROMEStrArray* values) const { return GetPathValues(path.Data(), values); }
   Bool_t      GetPathValues(const char* path, ROMEStrArray* values) const;
   Bool_t      ReplacePathAttributeValue(const char* path, const char* name, const char* value) const;
   Bool_t      ReplacePathAttributeName(const char* path, const char* name, const char* newName) const;
   Bool_t      ReplacePathName(const char* path, const char* name) const;
   Bool_t      ReplacePathValue(const char* path, const char* value) const;
   Bool_t      DeletePathAttribute(const char* path, const char* name) const;
   Bool_t      DeletePath(const char* path) const;
   Bool_t      NewPathAttribute(const char* path, const char* name, const char* value) const;
   Bool_t      NewPathNextElement(const char* path, const char* name, const char* value) const;
   Bool_t      NewPathPrevElement(const char* path, const char* name, const char* value) const;
   Int_t       NewPathLastElement(const char* path, const char* name, const char* value) const;
   Bool_t      NewPathChildElement(const char* path ,const char* name ,const char* value) const;
   Bool_t      HasPathChildren(const char* path) const;
   Bool_t      WritePathFile(const char* file);
   // node
   PMXML_NODE  GetPathNode(const char* path) const { return mxml_find_node(fRootNode, const_cast<char*>(path)); }
   PMXML_NODE  GetRootNode() const { return fRootNode; }
   PMXML_NODE  GetSubNode(PMXML_NODE node, Int_t i) const { return mxml_subnode(node, i); }
   const char *GetNodeValue(PMXML_NODE node) const { return mxml_get_value(node); }
   Int_t       GetNodeIntValue(PMXML_NODE node) const;
   Double_t    GetNodeDoubleValue(PMXML_NODE node) const;
   const char *GetNodeName(PMXML_NODE node) const { return mxml_get_name(node); }
   const char *GetNodeAttribute(PMXML_NODE node, const char* name) const { return mxml_get_attribute(node, const_cast<char*>(name)); }

   ClassDef(ROMEXML, 0) // Interface to XML files
};

#endif   // ROMEXML_H
