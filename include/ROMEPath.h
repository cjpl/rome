/********************************************************************
  ROMEPath.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2004/11/11 12:58:07  schneebeli_m
  Implemented XML database with new path rules


********************************************************************/
#ifndef ROMEPath_H
#define ROMEPath_H

#include <TArrayI.h>
#include <TObjArray.h>
#include <Riostream.h>
#include <ROMEString.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>

class ROMEPath
{
protected:
   // decode
   ROMEStrArray*    fTableNames;             // Array of TableNames
   ROMEStrArray*    fTableConstraints;       // Array of Constraints on the Table 
   ROMEStrArray*    fTableDBConstraints;     // Array of Constraints read from Data Base
   ROMEStr2DArray*  fTableConnection;        // Connection id's of the Table
   ROMEString       fOrderTableName;         // Name of the Order Table
   ROMEString       fOrderFieldName;         // Name of the Order Field
   TArrayI          fOrderIndex;             // Array indexes of the Order Field Array
   bool             fOrderArray;             // Flags Order Field Arrays
   ROMEString       fFieldName;              // Name of the Field
   TArrayI          fFieldIndex;             // Array indexes of the Field Array
   bool             fFieldArray;             // Flags Field Arrays

   // decode constraint
   ROMEStrArray*    fConstraintField;        // Array of Constraint Fields
   ROMEStrArray*    fConstraintValue;        // Array of Constraint Values

public:
   ROMEPath();
   ~ROMEPath();

   void Print();
   bool Decode(const char* path);
   bool DecodeConstraint(const char* constraint);

   // getters for decode
   int         GetNumberOfTables() { return fTableNames->GetEntriesFast(); };
   const char* GetTableNameAt(int i) { return fTableNames->At(i)->Data(); };
   const char* GetTableConstraintAt(int i)   { return fTableConstraints->At(i)->Data(); };
   const char* GetTableDBConstraintAt(int i) { return fTableDBConstraints->At(i)->Data(); };
   int         GetNumberOfTableConnectionAt(int iTable) { return fTableConnection->GetEntriesFastAt(iTable); };
   const char* GetTableConnectionAt(int iTable,int iConnection) { if (fTableConnection->At(iTable,iConnection)==NULL) return ""; return fTableConnection->At(iTable,iConnection)->Data(); };
   const char* GetOrderTableName() { return fOrderTableName.Data(); };
   const char* GetOrderFieldName() { return fOrderFieldName.Data(); };
   int         GetOrderIndexAt(int i)  { return fOrderIndex[i]; };
   bool        IsOrderArray() { return fOrderArray; };
   const char* GetFieldName() { return fFieldName.Data(); };
   int         GetFieldIndexAt(int i)  { return fFieldIndex[i]; };
   bool        IsFieldArray() { return fFieldArray; };

   // getters for decode constraint
   int         GetNumberOfConstraints() { return fConstraintField->GetEntriesFast(); };
   const char* GetConstraintFieldAt(int i)   { return fConstraintField->At(i)->Data(); };
   const char* GetConstraintValueAt(int i)   { return fConstraintValue->At(i)->Data(); };

protected:
   // setters for decode
   void  SetTableNameAt(int i,const char* name) { fTableNames->AddAtAndExpand(name,i); };
   void  SetTableNameAt(int i,TString& name) { fTableNames->AddAtAndExpand(name,i); };
   void  SetTableNameAt(int i,TSubString& name) { fTableNames->AddAtAndExpand(name,i); };
   void  SetTableConstraintAt(int i,const char* constraint) { fTableConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableConstraintAt(int i,TString& constraint) { fTableConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableConstraintAt(int i,TSubString& constraint) { fTableConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableDBConstraintAt(int i,const char* constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableDBConstraintAt(int i,TString& constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableDBConstraintAt(int i,TSubString& constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableConnectionAt(int iTable,int iConnection,const char* connectionName) { fTableConnection->SetAt(connectionName,iTable,iConnection); };
   void  SetTableConnectionAt(int iTable,int iConnection,TString& connectionName) { fTableConnection->SetAt(connectionName,iTable,iConnection); };
   void  SetTableConnectionAt(int iTable,int iConnection,TSubString& connectionName) { fTableConnection->SetAt(connectionName,iTable,iConnection); };

   void  SetOrderTableName(const char* name) { fOrderTableName = name; };
   void  SetOrderTableName(TString& name) { fOrderTableName = name; };
   void  SetOrderTableName(TSubString& name) { fOrderTableName = name; };
   void  SetOrderFieldName(const char* name) { fOrderFieldName = name; };
   void  SetOrderFieldName(TString& name) { fOrderFieldName = name; };
   void  SetOrderFieldName(TSubString& name) { fOrderFieldName = name; };
   void  SetOrderIndexAt(int i,int index) { fOrderIndex.AddAt(index,i); };
   void  SetOrderArray(bool flag) { fOrderArray = flag; };

   void  SetFieldName(const char* name) { fFieldName = name; };
   void  SetFieldName(TString& name) { fFieldName = name; };
   void  SetFieldName(TSubString& name) { fFieldName = name; };
   void  SetFieldIndexAt(int i,int index) { fFieldIndex.AddAt(index,i); };
   void  SetFieldArray(bool flag) { fFieldArray = flag; };

   // setters for decode constraint
   void  SetConstraintFieldAt(int i,const char* field) { fConstraintField->AddAtAndExpand(field,i); };
   void  SetConstraintFieldAt(int i,TString& field) { fConstraintField->AddAtAndExpand(field,i); };
   void  SetConstraintFieldAt(int i,TSubString& field) { fConstraintField->AddAtAndExpand(field,i); };
   void  SetConstraintValueAt(int i,const char* value) { fConstraintValue->AddAtAndExpand(value,i); };
   void  SetConstraintValueAt(int i,TString& value) { fConstraintValue->AddAtAndExpand(value,i); };
   void  SetConstraintValueAt(int i,TSubString& value) { fConstraintValue->AddAtAndExpand(value,i); };

   // internal methods
   int   MinPosition(int i1=-1,int i2=-1,int i3=-1,int i4=-1);
};

#endif   // ROMEPath_H
