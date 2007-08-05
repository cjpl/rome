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

namespace {
Int_t MinPosition(Int_t i1 = -1, Int_t i2 = -1, Int_t i3 = -1, Int_t i4 = -1)
{
   if (i1==-1 && i2==-1 && i3==-1 && i4==-1)
      return -1;
   if (i1==-1) {
      i1 = 1000000;
   }
   if (i2==-1) {
      i2 = 1000000;
   }
   if (i3==-1) {
      i3 = 1000000;
   }
   if (i4==-1) {
      i4 = 1000000;
   }
   return TMath::Min(i1,TMath::Min(i2,TMath::Min(i3,i4)));
}
}

//______________________________________________________________________________
ROMEPath::ROMEPath()
:fTableNames(new ROMEStrArray())
,fTableAbsolutePaths(new ROMEStrArray())
,fTableConstraints(new ROMEStrArray())
,fTableDBConstraints(new ROMEStrArray())
,fTableIDName(new ROMEStrArray())
,fTableIDXName(new ROMEStrArray())
,fOrderTableName("")
,fOrderFieldName("")
,fOrderIndex()
,fOrderArray(kFALSE)
,fFieldName("")
,fFieldConstraints("")
,fFieldIndex()
,fFieldArray(kFALSE)
,fConstraintField(new ROMEStrArray())
,fConstraintValue(new ROMEStrArray())
{
   // decode
   fFieldIndex.Set(3);
   fOrderIndex.Set(3);
   SetOrderIndexAt(0,0);
   SetOrderIndexAt(1,-1);
   SetOrderIndexAt(2,1);
   SetFieldIndexAt(0,0);
   SetFieldIndexAt(1,-1);
   SetFieldIndexAt(2,1);
}

//______________________________________________________________________________
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

//______________________________________________________________________________
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
      SetConstraintFieldAt(iconst, static_cast<TString>(substr(0,iequal)).Data());
      SetConstraintValueAt(iconst, static_cast<TString>(substr(iequal+1,substr.Length()-iequal-1)).Data());
      iconst++;
   }
   return true;
}

//______________________________________________________________________________
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

   int indx;
   int i1,i2,i3,iat1,iat2,itemp,ie;
   ROMEString subPath;
   ROMEString constraint;
   int nTable = -1;
   // extract order path
   if ((indx=path.Index(";",1,0,TString::kExact))!=-1) {
      orderPath = path(indx+1,path.Length()-indx-1);
      path = path(0,indx);
   }
   // extract tables
   while ((indx=path.Index("/",1,0,TString::kExact))!=-1) {
      constraint.Resize(0);
      nTable++;
      SetTableIDNameAt(nTable,"id");
      SetTableIDXNameAt(nTable,"");
      SetTableConstraintAt(nTable,"");
      SetTableDBConstraintAt(nTable,"");
      subPath = path(0,indx);
      path = path(indx+1,path.Length());
      abspathposition += indx+1;
      // brackets
      i1=subPath.Index("[",1,0,TString::kExact);
      i2=subPath.Index("(",1,0,TString::kExact);
      i3=subPath.Index("{",1,0,TString::kExact);
      indx = MinPosition(i1,i2,i3);
      if (indx==-1) {
         SetTableNameAt(nTable,subPath);
         SetTableAbsolutePathAt(nTable, static_cast<TString>(originalPath(0,abspathposition-1)).Data());
         continue;
      }
      else {
         if (indx==0) {
            ROMEPrint::Error("\nNo table name specified.\n");
            return false;
         }
         SetTableNameAt(nTable, static_cast<TString>(subPath(0,indx)).Data());
         SetTableAbsolutePathAt(nTable, static_cast<TString>(originalPath(0,abspathposition-1)).Data());
      }
      // handle '[' (Constraints)
      if (i1!=-1) {
         if ((iat1=subPath.Index("@",1,i1,TString::kExact))!=-1) {
            if ((iat2=subPath.Index("]",1,iat1,TString::kExact))==-1) {
               ROMEPrint::Error("\nData base constraint statement not closed in table '%s'.\n", GetTableNameAt(nTable));
return false;
            }
            SetTableDBConstraintAt(nTable, static_cast<TString>(subPath(iat1+1,iat2-iat1-1)).Data());
         }
         else {
            if ((indx=subPath.Index("]",1,i1,TString::kExact))==-1) {
               ROMEPrint::Error("\nConstraint statement not closed in table '%s'.\n", GetTableNameAt(nTable));
               return false;
            }
            SetTableConstraintAt(nTable, static_cast<TString>(subPath(i1+1,indx-i1-1)).Data());
         }
      }
      // handle '('  (Arrays)
      if (i2!=-1) {
         if ((indx=subPath.Index(")",1,i2,TString::kExact))==-1) {
            ROMEPrint::Error("\nArray statement not closed in table '%s'.\n", GetTableNameAt(nTable));
            return false;
         }
         SetOrderTableName(GetTableNameAt(nTable));
         SetOrderArray(true);
         SetOrderIndexAt(0,0);
         SetOrderIndexAt(1,-1);
         SetOrderIndexAt(2,1);
         ROMEString temp = subPath(i2+1,indx-i2);
         for (int i=0;i<3;i++) {
            value = strtol(temp.Data(),&cstop,10);
            if (cstop==NULL) {
               return true;
            }
            if (*cstop!=',' && *cstop!=')') {
               ROMEPrint::Error("\nError in array statement.\n%s\n", dataBasePath);
               return false;
            }
            SetOrderIndexAt(i,value);
            if (*cstop==')') {
               break;
            }
            temp = temp(static_cast<int>(cstop+1-temp.Data()),temp.Length());
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
   if (strlen(GetOrderTableName())<=0) {
      SetOrderTableName(GetTableNameAt(nTable));
   }

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
      for (int i=0;i<3;i++) {
         SetOrderIndexAt(i,order->GetFieldIndexAt(i));
      }
      SafeDelete(order);
   }

   // extract field
   SetFieldIndexAt(0,0);
   SetFieldIndexAt(1,-1);
   SetFieldIndexAt(2,1);
   i1=path.Index("[",1,0,TString::kExact);
   i2=path.Index("(",1,0,TString::kExact);
   indx = MinPosition(i1,i2);
   if (indx==-1) {
      SetFieldName(path);
      SetFieldArray(false);
      return true;
   }
   else {
      if (indx==0) {
         ROMEPrint::Error("\nNo field name specified.\n");
         return false;
      }
      SetFieldName(static_cast<TString>(path(0,indx)).Data());
   }
   // handle '[' (Constraints)
   if (i1!=-1) {
      if ((indx=path.Index("]",1,i1,TString::kExact))==-1) {
         ROMEPrint::Error("\nConstraint statement not closed in field '%s'.\n", GetFieldName());
         return false;
      }
      SetFieldConstraints(static_cast<TString>(path(i1+1,indx-i1-1)).Data());
   }

   // handle '('  (Arrays)
   if (i2!=-1) {
      if ((indx=path.Index(")",1,i2,TString::kExact))==-1) {
         ROMEPrint::Error("\nArray statement not closed in field '%s'.\n", GetFieldName());
         return false;
      }
      SetFieldArray(true);
      path = path(i2+1,indx-i2);
      for (int i=0;i<3;i++) {
         value = strtol(path.Data(),&cstop,10);
         if (cstop==NULL) {
            return true;
         }
         if (*cstop!=',' && *cstop!=')') {
            ROMEPrint::Error("\nError in array statement.\n%s\n", dataBasePath);
            return false;
         }
         SetFieldIndexAt(i,value);
         if (*cstop==')') {
            if (GetFieldIndexAt(1)==-1) {
               SetFieldIndexAt(1,GetFieldIndexAt(0)+1);
            }
            return true;
         }
         path = path(static_cast<int>(cstop+1-path.Data()),path.Length());
      }
   }
   return true;
}

//______________________________________________________________________________
void ROMEPath::Print() const
{
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
      ROMEPrint::Print("      start : %d\n", GetOrderIndexAt(0));
      ROMEPrint::Print("      end   : %d\n", GetOrderIndexAt(1));
      ROMEPrint::Print("      step  : %d\n", GetOrderIndexAt(2));
      ROMEPrint::Print("\n");
   }
   ROMEPrint::Print("\nField :\n");
   ROMEPrint::Print("   Name : %s\n", GetFieldName());
   ROMEPrint::Print("      Constraint     : %s\n", GetFieldConstraints());
   if (IsFieldArray()) {
      ROMEPrint::Print("\n   Array :\n");
      ROMEPrint::Print("      start : %d\n", GetFieldIndexAt(0));
      ROMEPrint::Print("      end   : %d\n", GetFieldIndexAt(1));
      ROMEPrint::Print("      step  : %d\n", GetFieldIndexAt(2));
   }
}

void ROMEPath::GetAbsolutePath( ROMEString& path, const char* tablename ) const
{
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
