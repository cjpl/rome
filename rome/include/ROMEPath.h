/********************************************************************
  ROMEPath.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEPath_H
#define ROMEPath_H

#include <TArrayI.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"

class ROMEPath
{
protected:
   // decode
   ROMEStrArray *fTableNames;             // Array of TableNames
   ROMEStrArray *fTableAbsolutePaths;     // Absolute path to the table
   ROMEStrArray *fTableConstraints;       // Array of Constraints on the Table
   ROMEStrArray *fTableDBConstraints;     // Array of Constraints read from Data Base
   ROMEStrArray *fTableIDName;            // ID name of the Table
   ROMEStrArray *fTableIDXName;           // IDX name of the Table
   ROMEString    fOrderTableName;         // Name of the Order Table
   ROMEString    fOrderFieldName;         // Name of the Order Field
   TArrayI       fOrderIndex;             // Array indexes of the Order Field Array
   Bool_t        fOrderArray;             // Flags Order Field Arrays
   ROMEString    fFieldName;              // Name of the Field
   ROMEString    fFieldConstraints;       // Constraints on the Field
   TArrayI       fFieldIndex;             // Array indexes of the Field Array
   Bool_t        fFieldArray;             // Flags Field Arrays

   // decode constraint
   ROMEStrArray *fConstraintField;        // Array of Constraint Fields
   ROMEStrArray *fConstraintValue;        // Array of Constraint Values

private:
   ROMEPath(const ROMEPath &path); // not implemented
   ROMEPath &operator=(const ROMEPath &rhs); // not implemented

public:
   ROMEPath();
   virtual ~ROMEPath();

   void        Print() const;
   Bool_t      Decode(const char* path,Long64_t runNumber,Long64_t eventNumber);
   Bool_t      DecodeConstraint(const char* constraint);

   // getters for decode
   Int_t       GetNumberOfTables() const { return fTableNames->GetEntriesFast(); }
   const char *GetTableNameAt(Int_t i) const { return fTableNames->At(i).Data(); }
//   const char *GetTableAbsolutePathAt(Int_t i) const { return fTableAbsolutePaths->At(i).Data(); }
   const char *GetTableConstraintAt(Int_t i) const   { return fTableConstraints->At(i).Data(); }
   const char *GetTableDBConstraintAt(Int_t i) const { return fTableDBConstraints->At(i).Data(); }
   const char *GetTableIDNameAt(Int_t i) const { return fTableIDName->At(i).Data(); }
   const char *GetTableIDXNameAt(Int_t i) const { return fTableIDXName->At(i).Data(); }
   const char *GetOrderTableName() const { return fOrderTableName.Data(); }
   const char *GetOrderFieldName() const { return fOrderFieldName.Data(); }
   Int_t       GetOrderIndexAt(Int_t i) const  { return fOrderIndex[i]; }
   Bool_t      IsOrderArray() const { return fOrderArray; }
   const char *GetFieldName() const { return fFieldName.Data(); }
   const char *GetFieldConstraints() const { return fFieldConstraints.Data(); }
   Int_t       GetFieldIndexAt(Int_t i) const  { return fFieldIndex[i]; }
   Bool_t      IsFieldArray() const { return fFieldArray; }

   void        GetAbsolutePath( ROMEString& path,const char* tablename ) const;

   // getters for decode constraint
   Int_t       GetNumberOfConstraints() const { return fConstraintField->GetEntriesFast(); }
   const char *GetConstraintFieldAt(Int_t i) const   { return fConstraintField->At(i).Data(); }
   const char *GetConstraintValueAt(Int_t i) const   { return fConstraintValue->At(i).Data(); }

protected:
   // setters for decode
   void        SetTableNameAt(Int_t i,const char* name) { fTableNames->AddAtAndExpand(name,i); }
   void        SetTableNameAt(Int_t i,TString& name) { fTableNames->AddAtAndExpand(name,i); }
   void        SetTableAbsolutePathAt(Int_t i,const char* name) { fTableAbsolutePaths->AddAtAndExpand(name,i); }
   void        SetTableAbsolutePathAt(Int_t i,TString& name) { fTableAbsolutePaths->AddAtAndExpand(name,i); }
   void        SetTableConstraintAt(Int_t i,const char* constraint) { fTableConstraints->AddAtAndExpand(constraint,i); }
   void        SetTableConstraintAt(Int_t i,TString& constraint) { fTableConstraints->AddAtAndExpand(constraint,i); }
   void        SetTableDBConstraintAt(Int_t i,const char* constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); }
   void        SetTableDBConstraintAt(Int_t i,TString& constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); }
   void        SetTableIDNameAt(Int_t i,const char* name) { fTableIDName->AddAtAndExpand(name,i); }
   void        SetTableIDNameAt(Int_t i,TString& name) { fTableIDName->AddAtAndExpand(name,i); }
   void        SetTableIDXNameAt(Int_t i,const char* name) { fTableIDXName->AddAtAndExpand(name,i); }
   void        SetTableIDXNameAt(Int_t i,TString& name) { fTableIDXName->AddAtAndExpand(name,i); }

   void        SetOrderTableName(const char* name) { fOrderTableName = name; }
   void        SetOrderTableName(TString& name) { fOrderTableName = name; }
   void        SetOrderFieldName(const char* name) { fOrderFieldName = name; }
   void        SetOrderFieldName(TString& name) { fOrderFieldName = name; }
   void        SetOrderIndexAt(Int_t i,Int_t index) { fOrderIndex.AddAt(index,i); }
   void        SetOrderArray(Bool_t flag) { fOrderArray = flag; }

   void        SetFieldName(const char* name) { fFieldName = name; }
   void        SetFieldName(TString& name) { fFieldName = name; }
   void        SetFieldConstraints(const char* constraints) { fFieldConstraints = constraints; }
   void        SetFieldConstraints(TString& constraints) { fFieldConstraints = constraints; }
   void        SetFieldIndexAt(Int_t i,Int_t index) { fFieldIndex.AddAt(index,i); }
   void        SetFieldArray(Bool_t flag) { fFieldArray = flag; }

   // setters for decode constraint
   void        SetConstraintFieldAt(Int_t i,const char* field) { fConstraintField->AddAtAndExpand(field,i); }
   void        SetConstraintFieldAt(Int_t i,TString& field) { fConstraintField->AddAtAndExpand(field,i); }
   void        SetConstraintValueAt(Int_t i,const char* value) { fConstraintValue->AddAtAndExpand(value,i); }
   void        SetConstraintValueAt(Int_t i,TString& value) { fConstraintValue->AddAtAndExpand(value,i); }

   ClassDef(ROMEPath, 0) // Class for analysing database path
};

#endif   // ROMEPath_H
