// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEPath
//
//  Data base path decoding.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <TObjArray.h>
#include "ROMEiostream.h"
#include "ROMEStr2DArray.h"
#include "ROMEPath.h"

ClassImp(ROMEPath)

ROMEPath::ROMEPath() {
   fOrderArray = false;
   fFieldArray = false;
   fTableAbsolutePaths = new ROMEStrArray(0);
   fTableNames = new ROMEStrArray(0);
   fTableConstraints = new ROMEStrArray(0);
   fTableDBConstraints = new ROMEStrArray(0);
   fTableIDName = new ROMEStrArray(0);
   fTableIDXName = new ROMEStrArray(0);
   fFieldIndex.Set(3);
   fOrderIndex.Set(3);
   SetOrderIndexAt(0,0);
   SetOrderIndexAt(1,-1);
   SetOrderIndexAt(2,1);
   SetFieldIndexAt(0,0);
   SetFieldIndexAt(1,-1);
   SetFieldIndexAt(2,1);

   fConstraintField = new ROMEStrArray(0);
   fConstraintValue = new ROMEStrArray(0);
}

ROMEPath::~ROMEPath() {
   SafeDelete(fTableNames);
   SafeDelete(fTableAbsolutePaths);
   SafeDelete(fTableConstraints);
   SafeDelete(fTableDBConstraints);
   SafeDelete(fTableIDName);
   SafeDelete(fTableIDXName);

   fConstraintField->Delete();
   fConstraintValue->Delete();
   SafeDelete(fConstraintField);
   SafeDelete(fConstraintValue);
}

Bool_t ROMEPath::DecodeConstraint(const char* contraint)
{
   int istart,iend,iequal,iconst=0;
   ROMEString str = contraint;
   ROMEString substr;
   fConstraintField->RemoveAll();
   fConstraintValue->RemoveAll();
   if (str.Index(" or ",4,0,TString::kIgnoreCase)!=-1) {
      ROMEPrint::Error("\n'or' not handled in DecodeConstraint.\n");
      return false;
   }
   str.ReplaceAll("(","");
   str.ReplaceAll(")","");
   str.ReplaceAll(" and ",",");
   str.ReplaceAll(" AND ",",");
   str.ReplaceAll(" And ",",");
   str += ",";
   istart = 0;
   while ((iend=str.Index(",",1,istart,TString::kIgnoreCase))!=-1) {
      substr = str(istart,iend-istart);
      substr.Strip(TString::kBoth,' ');
      istart = iend+1;
      if ((iequal=substr.Index("=",1,0,TString::kExact))==-1) {
         ROMEPrint::Error("\nInvalid constraint expression : %s.\n", substr.Data());
         return false;
      }
      SetConstraintFieldAt(iconst,((TString)substr(0,iequal)).Data());
      SetConstraintValueAt(iconst,((TString)substr(iequal+1,substr.Length()-iequal-1)).Data());
      iconst++;
   }
   return true;
}

Bool_t ROMEPath::Decode(const char* dataBasePath,Long64_t runNumber,Long64_t eventNumber)
{
   SetOrderTableName("");
   SetOrderFieldName("idx");
   char* cstop;
   int value;
   int abspathposition = 0;
   ROMEString tmpString;
   ROMEString originalPath;
   ROMEString orderPath;
   ROMEString path = dataBasePath;
   // replace ## with the current run number
#if defined( R__VISUAL_CPLUSPLUS )
   tmpString.SetFormatted("%I64d",eventNumber);
#else
   tmpString.SetFormatted("%lld",eventNumber);
#endif
   path.ReplaceAll("##",tmpString);
   originalPath = path;
   // replace # with the current run number
#if defined( R__VISUAL_CPLUSPLUS )
   tmpString.SetFormatted("%I64d",runNumber);
#else
   tmpString.SetFormatted("%lld",runNumber);
#endif
   path.ReplaceAll("#",tmpString);
   // check path
   if (path.Length()<=0) {
      ROMEPrint::Error("\nInvalid path.\n");
      return false;
   }
   if (path[0]!='/') {
      ROMEPrint::Error("\nPath statment has to start with '/'.\n");
      return false;
   }
   path = path(1,path.Length());
   abspathposition += 1;

   int index;
   int i1,i2,i3,iat1,iat2,itemp,ie;
   ROMEString subPath;
   ROMEString constraint;
   int nTable = -1;
   // extract order path
   if ((index=path.Index(";",1,0,TString::kExact))!=-1) {
      orderPath = path(index+1,path.Length()-index-1);
      path = path(0,index);
   }
   // extract tables
   while ((index=path.Index("/",1,0,TString::kExact))!=-1) {
      constraint.Resize(0);
      nTable++;
      SetTableIDNameAt(nTable,"id");
      SetTableIDXNameAt(nTable,"");
      SetTableConstraintAt(nTable,"");
      SetTableDBConstraintAt(nTable,"");
      subPath = path(0,index);
      path = path(index+1,path.Length());
      abspathposition += index+1;
      // brackets
      i1=subPath.Index("[",1,0,TString::kExact);
      i2=subPath.Index("(",1,0,TString::kExact);
      i3=subPath.Index("{",1,0,TString::kExact);
      index = MinPosition(i1,i2,i3);
      if (index==-1) {
         SetTableNameAt(nTable,subPath);
         SetTableAbsolutePathAt(nTable,((TString)originalPath(0,abspathposition-1)).Data());
         continue;
      }
      else {
         if (index==0) {
            ROMEPrint::Error("\nNo table name specified.\n");
            return false;
         }
         SetTableNameAt(nTable,((TString)subPath(0,index)).Data());
         SetTableAbsolutePathAt(nTable,((TString)originalPath(0,abspathposition-1)).Data());
      }
      // handle '[' (Constraints)
      if (i1!=-1) {
         if ((iat1=subPath.Index("@",1,i1,TString::kExact))!=-1) {
            if ((iat2=subPath.Index("]",1,iat1,TString::kExact))==-1) {
               ROMEPrint::Error("\nData base constraint statement not closed in table '%s'.\n", GetTableNameAt(nTable));
return false;
            }
            SetTableDBConstraintAt(nTable,((TString)subPath(iat1+1,iat2-iat1-1)).Data());
         }
         else {
            if ((index=subPath.Index("]",1,i1,TString::kExact))==-1) {
               ROMEPrint::Error("\nConstraint statement not closed in table '%s'.\n", GetTableNameAt(nTable));
               return false;
            }
            SetTableConstraintAt(nTable,((TString)subPath(i1+1,index-i1-1)).Data());
         }
      }
      // handle '('  (Arrays)
      if (i2!=-1) {
         if ((index=subPath.Index(")",1,i2,TString::kExact))==-1) {
            ROMEPrint::Error("\nArray statement not closed in table '%s'.\n", GetTableNameAt(nTable));
            return false;
         }
         SetOrderTableName(GetTableNameAt(nTable));
         SetOrderArray(true);
         SetOrderIndexAt(0,0);
         SetOrderIndexAt(1,-1);
         SetOrderIndexAt(2,1);
         ROMEString temp = subPath(i2+1,index-i2);
         for (int i=0;i<3;i++) {
            value = strtol(temp.Data(),&cstop,10);
            if (cstop==NULL)
               return true;
            if (*cstop!=',' && *cstop!=')') {
               ROMEPrint::Error("\nError in array statement.\n%s\n", dataBasePath);
               return false;
            }
            SetOrderIndexAt(i,value);
            if (*cstop==')')
               break;
            temp = temp((int)(cstop+1-temp.Data()),temp.Length());
         }
      }
      // handle '{'
      if (i3!=-1) {
         if ((ie=subPath.Index("}",1,i3,TString::kExact))==-1) {
            ROMEPrint::Error("\nID statement not closed in table '%s'.\n", GetTableNameAt(nTable));
            return false;
         }
         itemp=subPath.Index(",",1,i3,TString::kExact);
         if (itemp==-1 || itemp>=ie) {
            ROMEPrint::Error("\nConnection id statment in table '%s' must contain a comma.\n", GetTableNameAt(nTable));
            return false;
         }
         ROMEString temp = subPath(i3+1,itemp-i3-1);
         temp.Strip(TString::kBoth,' ');
         temp.Strip(TString::kBoth,'"');
         SetTableIDNameAt(nTable,temp.Data());
         temp = subPath(itemp+1,ie-itemp-1);
         temp.Strip(TString::kBoth,' ');
         temp.Strip(TString::kBoth,'"');
         SetTableIDXNameAt(nTable,temp.Data());
      }
   }
   if (strlen(GetOrderTableName())<=0)
      SetOrderTableName(GetTableNameAt(nTable));

   // handle order
   if (orderPath.Length()>0) {
      ROMEPath *order = new ROMEPath();
      orderPath.Insert(0,"/");
      order->Decode(orderPath.Data(),runNumber,eventNumber);
      if (order->GetNumberOfTables()!=1) {
         ROMEPrint::Error("\nOrder statment has to look like this : 'Table'/'Field'['start','end','step'].\n");
         SafeDelete(order);
         return false;
      }
      SetOrderArray(order->IsFieldArray());
      SetOrderTableName(order->GetTableNameAt(0));
      SetOrderFieldName(order->GetFieldName());
      for (int i=0;i<3;i++)
         SetOrderIndexAt(i,order->GetFieldIndexAt(i));
      SafeDelete(order);
   }

   // extract field
   SetFieldIndexAt(0,0);
   SetFieldIndexAt(1,-1);
   SetFieldIndexAt(2,1);
   i1=path.Index("[",1,0,TString::kExact);
   i2=path.Index("(",1,0,TString::kExact);
   index = MinPosition(i1,i2);
   if (index==-1) {
      SetFieldName(path);
      SetFieldArray(false);
      return true;
   }
   else {
      if (index==0) {
         ROMEPrint::Error("\nNo field name specified.\n");
         return false;
      }
      SetFieldName(((TString)path(0,index)).Data());
   }
   // handle '[' (Constraints)
   if (i1!=-1) {
      if ((index=path.Index("]",1,i1,TString::kExact))==-1) {
         ROMEPrint::Error("\nConstraint statement not closed in field '%s'.\n", GetFieldName());
         return false;
      }
      SetFieldConstraints(((TString)path(i1+1,index-i1-1)).Data());
   }

   // handle '('  (Arrays)
   if (i2!=-1) {
      if ((index=path.Index(")",1,i2,TString::kExact))==-1) {
         ROMEPrint::Error("\nArray statement not closed in field '%s'.\n", GetFieldName());
         return false;
      }
      SetFieldArray(true);
      path = path(i2+1,index-i2);
      for (int i=0;i<3;i++) {
         value = strtol(path.Data(),&cstop,10);
         if (cstop==NULL)
            return true;
         if (*cstop!=',' && *cstop!=')') {
            ROMEPrint::Error("\nError in array statement.\n%s\n", dataBasePath);
            return false;
         }
         SetFieldIndexAt(i,value);
         if (*cstop==')')
            return true;
         path = path((int)(cstop+1-path.Data()),path.Length());
      }
   }
   return true;
}

void ROMEPath::Print() {
   const int nTable = GetNumberOfTables();
   ROMEPrint::Print("Tables : \n");
   for (int i=0;i<nTable;i++) {
      ROMEPrint::Print("   Name : %s\n", GetTableNameAt(i));
      ROMEPrint::Print("      Constraint     : %s\n", GetTableConstraintAt(i));
      ROMEPrint::Print("      DB Constraint  : %s\n", GetTableDBConstraintAt(i));
      ROMEPrint::Print("      Connection ID  : %s\n", GetTableIDNameAt(i));
      ROMEPrint::Print("      Connection IDX : %s\n", GetTableIDXNameAt(i));
      ROMEPrint::Print("\n");
   }
   ROMEPrint::Print("\nOrder :\n");
   ROMEPrint::Print("   Table Name : %s\n", GetOrderTableName());
   ROMEPrint::Print("   Field Name : %s\n", GetOrderFieldName());
   if (IsOrderArray()) {
      ROMEPrint::Print("\n   Array :\n");
      ROMEPrint::Print("      start : %s\n", GetOrderIndexAt(0));
      ROMEPrint::Print("      end   : %s\n", GetOrderIndexAt(1));
      ROMEPrint::Print("      step  : %s\n", GetOrderIndexAt(2));
      ROMEPrint::Print("\n");
   }
   ROMEPrint::Print("\nField :\n");
   ROMEPrint::Print("   Name : %s\n", GetFieldName());
   ROMEPrint::Print("      Constraint     : %s\n", GetFieldConstraints());
   if (IsFieldArray()) {
      ROMEPrint::Print("\n   Array :\n");
      ROMEPrint::Print("      start : %s\n", GetFieldIndexAt(0));
      ROMEPrint::Print("      end   : %s\n", GetFieldIndexAt(1));
      ROMEPrint::Print("      step  : %s\n", GetFieldIndexAt(2));
   }
}

void ROMEPath::GetAbsolutePath( ROMEString& path, const char* tablename ) {
   int i;
   const int nTableName = fTableNames->GetEntriesFast();
   for (i=1;i<nTableName;i++) {
      if(fTableNames->At(i)==tablename) {
         path = fTableAbsolutePaths->At(i-1).Data();
         path += "/";
         path += tablename;
         return;
      }
   }
   path = tablename;
   return;
}

Int_t ROMEPath::MinPosition(Int_t i1,Int_t i2,Int_t i3,Int_t i4) {
   if (i1==-1 && i2==-1 && i3==-1 && i4==-1)
      return -1;
   if (i1==-1)
      i1 = 1000000;
   if (i2==-1)
      i2 = 1000000;
   if (i3==-1)
      i3 = 1000000;
   if (i4==-1)
      i4 = 1000000;
   return TMath::Min(i1,TMath::Min(i2,TMath::Min(i3,i4)));
}