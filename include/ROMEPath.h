/********************************************************************
  ROMEPath.h, M. Schneebeli PSI

  $Log$
  Revision 1.6  2005/03/03 19:24:32  sawada
  compatibility with SQL and XML database.

  Revision 1.5  2005/01/27 16:21:06  schneebeli_m
  print method & no gROME in path

  Revision 1.4  2004/11/12 17:35:18  schneebeli_m
  fast xml database

  Revision 1.3  2004/11/11 14:07:15  schneebeli_m
  ROMEStrArray and ROMEStr2DArray change

  Revision 1.2  2004/11/11 13:17:37  schneebeli_m
  detail

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
   ROMEStrArray*    fTableAbsolutePaths;     // Absolute path to the table
   ROMEStrArray*    fTableConstraints;       // Array of Constraints on the Table 
   ROMEStrArray*    fTableDBConstraints;     // Array of Constraints read from Data Base
   ROMEStrArray*    fTableIDName;            // ID name of the Table
   ROMEStrArray*    fTableIDXName;           // IDX name of the Table
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
   bool Decode(const char* path,int runNumber);
   bool DecodeConstraint(const char* constraint);

   // getters for decode
   int         GetNumberOfTables() { return fTableNames->GetEntriesFast(); };
   const char* GetTableNameAt(int i) { return fTableNames->At(i).Data(); };
//   const char* GetTableAbsolutePathAt(int i) { return fTableAbsolutePaths->At(i).Data(); };
   const char* GetTableConstraintAt(int i)   { return fTableConstraints->At(i).Data(); };
   const char* GetTableDBConstraintAt(int i) { return fTableDBConstraints->At(i).Data(); };
   const char* GetTableIDNameAt(int i) { return fTableIDName->At(i).Data(); };
   const char* GetTableIDXNameAt(int i) { return fTableIDXName->At(i).Data(); };
   const char* GetOrderTableName() { return fOrderTableName.Data(); };
   const char* GetOrderFieldName() { return fOrderFieldName.Data(); };
   int         GetOrderIndexAt(int i)  { return fOrderIndex[i]; };
   bool        IsOrderArray() { return fOrderArray; };
   const char* GetFieldName() { return fFieldName.Data(); };
   int         GetFieldIndexAt(int i)  { return fFieldIndex[i]; };
   bool        IsFieldArray() { return fFieldArray; };
   const char* GetAbsolutePath( const char* tablename );

   // getters for decode constraint
   int         GetNumberOfConstraints() { return fConstraintField->GetEntriesFast(); };
   const char* GetConstraintFieldAt(int i)   { return fConstraintField->At(i).Data(); };
   const char* GetConstraintValueAt(int i)   { return fConstraintValue->At(i).Data(); };

protected:
   // setters for decode
   void  SetTableNameAt(int i,const char* name) { fTableNames->AddAtAndExpand(name,i); };
   void  SetTableNameAt(int i,TString& name) { fTableNames->AddAtAndExpand(name,i); };
   void  SetTableAbsolutePathAt(int i,const char* name) { fTableAbsolutePaths->AddAtAndExpand(name,i); };
   void  SetTableAbsolutePathAt(int i,TString& name) { fTableAbsolutePaths->AddAtAndExpand(name,i); };
   void  SetTableConstraintAt(int i,const char* constraint) { fTableConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableConstraintAt(int i,TString& constraint) { fTableConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableDBConstraintAt(int i,const char* constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableDBConstraintAt(int i,TString& constraint) { fTableDBConstraints->AddAtAndExpand(constraint,i); };
   void  SetTableIDNameAt(int i,const char* name) { fTableIDName->AddAtAndExpand(name,i); };
   void  SetTableIDNameAt(int i,TString& name) { fTableIDName->AddAtAndExpand(name,i); };
   void  SetTableIDXNameAt(int i,const char* name) { fTableIDXName->AddAtAndExpand(name,i); };
   void  SetTableIDXNameAt(int i,TString& name) { fTableIDXName->AddAtAndExpand(name,i); };

   void  SetOrderTableName(const char* name) { fOrderTableName = name; };
   void  SetOrderTableName(TString& name) { fOrderTableName = name; };
   void  SetOrderFieldName(const char* name) { fOrderFieldName = name; };
   void  SetOrderFieldName(TString& name) { fOrderFieldName = name; };
   void  SetOrderIndexAt(int i,int index) { fOrderIndex.AddAt(index,i); };
   void  SetOrderArray(bool flag) { fOrderArray = flag; };

   void  SetFieldName(const char* name) { fFieldName = name; };
   void  SetFieldName(TString& name) { fFieldName = name; };
   void  SetFieldIndexAt(int i,int index) { fFieldIndex.AddAt(index,i); };
   void  SetFieldArray(bool flag) { fFieldArray = flag; };

   // setters for decode constraint
   void  SetConstraintFieldAt(int i,const char* field) { fConstraintField->AddAtAndExpand(field,i); };
   void  SetConstraintFieldAt(int i,TString& field) { fConstraintField->AddAtAndExpand(field,i); };
   void  SetConstraintValueAt(int i,const char* value) { fConstraintValue->AddAtAndExpand(value,i); };
   void  SetConstraintValueAt(int i,TString& value) { fConstraintValue->AddAtAndExpand(value,i); };

   // internal methods
   int   MinPosition(int i1=-1,int i2=-1,int i3=-1,int i4=-1);
};

#endif   // ROMEPath_H
