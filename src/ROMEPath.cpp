// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEPath
//
//  Data base path decoding.
//
//  $Log$
//  Revision 1.11  2005/04/01 14:56:23  schneebeli_m
//  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ
//
//  Revision 1.10  2005/03/17 15:44:51  schneebeli_m
//  GetAbsolutePath error removed
//
//  Revision 1.9  2005/03/13 08:43:14  sawada
//  removed or comment out unused variables.
//  made virtual destructor of ROMEConfig and ROMEDataBase.
//
//  Revision 1.8  2005/03/03 19:24:32  sawada
//  compatibility with SQL and XML database.
//
//  Revision 1.7  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.6  2004/11/19 13:29:55  schneebeli_m
//  added stuff for sample
//
//  Revision 1.5  2004/11/16 18:53:44  sawada
//  SQL Write
//
//  Revision 1.4  2004/11/16 12:11:06  sawada
//  SQL Init,Read
//
//  Revision 1.3  2004/11/12 17:35:18  schneebeli_m
//  fast xml database
//
//  Revision 1.2  2004/11/11 14:07:15  schneebeli_m
//  ROMEStrArray and ROMEStr2DArray change
//
//  Revision 1.1  2004/11/11 12:56:58  schneebeli_m
//  Implemented XML database with new path rules
//
//
//////////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include <ROMEPath.h>

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
   this->SetOrderIndexAt(0,0);
   this->SetOrderIndexAt(1,-1);
   this->SetOrderIndexAt(2,1);
   this->SetFieldIndexAt(0,0);
   this->SetFieldIndexAt(1,-1);
   this->SetFieldIndexAt(2,1);
   
   fConstraintField = new ROMEStrArray(0);
   fConstraintValue = new ROMEStrArray(0);
}

ROMEPath::~ROMEPath() {
   delete fTableNames;
   delete fTableAbsolutePaths;
   delete fTableConstraints;
   delete fTableDBConstraints;
   delete fTableIDName;
   delete fTableIDXName;

   fConstraintField->Delete();
   fConstraintValue->Delete();
   delete fConstraintField;
   delete fConstraintValue;
}


bool ROMEPath::DecodeConstraint(const char* contraint)
{
   int istart,iend,iequal,iconst=0;
   ROMEString str = contraint;
   ROMEString substr;
   fConstraintField->RemoveAll();
   fConstraintValue->RemoveAll();
   if (str.Index(" or ",4,0,TString::kIgnoreCase)!=-1) {
      cout << "\n'or' not handled in DecodeConstraint." << endl;
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
         cout << "\nInvalid constraint expression : " << substr.Data() << "." << endl;
         return false;
      }
      this->SetConstraintFieldAt(iconst,((TString)substr(0,iequal)).Data());
      this->SetConstraintValueAt(iconst,((TString)substr(iequal+1,substr.Length()-iequal-1)).Data());
      iconst++;
   }
   return true;
}

bool ROMEPath::Decode(const char* dataBasePath,int runNumber)
{
   this->SetOrderTableName("");
   this->SetOrderFieldName("idx");
   char* cstop;
   int value;
   int abspathposition = 0;
   ROMEString originalPath;
   ROMEString orderPath;
   ROMEString path = dataBasePath;
   // replace # with the current run number
   ROMEString runNumberString;
   runNumberString.SetFormatted("%d",runNumber);
   path.ReplaceAll("#",runNumberString);
   originalPath = path;
   // check path
   if (path.Length()<=0) {
      cout << "\nInvalid path." << endl;
      return false;
   }
   if (path[0]!='/') {
      cout << "\nPath statment has to start with '/'." << endl;
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
      this->SetTableIDNameAt(nTable,"id");
      this->SetTableIDXNameAt(nTable,"");
      this->SetTableConstraintAt(nTable,"");
      this->SetTableDBConstraintAt(nTable,"");
      subPath = path(0,index);
      path = path(index+1,path.Length());
      abspathposition += index+1;
      // brackets
      i1=subPath.Index("(",1,0,TString::kExact);
      i2=subPath.Index("[",1,0,TString::kExact);
      i3=subPath.Index("{",1,0,TString::kExact);
      index = this->MinPosition(i1,i2,i3);
      if (index==-1) {
         this->SetTableNameAt(nTable,subPath);
         this->SetTableAbsolutePathAt(nTable,((TString)originalPath(0,abspathposition-1)).Data());
         continue;
      }
      else {
         if (index==0) {
            cout << "\nNo table name specified." << endl;
            return false;
         }
         this->SetTableNameAt(nTable,((TString)subPath(0,index)).Data());
         this->SetTableAbsolutePathAt(nTable,((TString)originalPath(0,abspathposition-1)).Data());
      }
      // handle '('
      if (i1!=-1) {
         if ((iat1=subPath.Index("@",1,i1,TString::kExact))!=-1) {
            if ((iat2=subPath.Index(")",1,iat1,TString::kExact))==-1) {
               cout << "\nData base constraint statement not closed in table '" << this->GetTableNameAt(nTable) << "'." << endl;
               return false;
            }
            this->SetTableDBConstraintAt(nTable,((TString)subPath(iat1+1,iat2-iat1-1)).Data());
         }
         else {
            if ((index=subPath.Index(")",1,i1,TString::kExact))==-1) {
               cout << "\nConstraint statement not closed in table '" << this->GetTableNameAt(nTable) << "'." << endl;
               return false;
            }
            this->SetTableConstraintAt(nTable,((TString)subPath(i1+1,index-i1-1)).Data());
         }
      }
      // handle '['
      if (i2!=-1) {
         if ((index=subPath.Index("]",1,i2,TString::kExact))==-1) {
            cout << "\nArray statement not closed in table '" << this->GetTableNameAt(nTable) << "'." << endl;
            return false;
         }
         this->SetOrderTableName(this->GetTableNameAt(nTable));
         this->SetOrderArray(true);
         this->SetOrderIndexAt(0,0);
         this->SetOrderIndexAt(1,-1);
         this->SetOrderIndexAt(2,1);
         ROMEString temp = subPath(i2+1,index-i2);
         for (int i=0;i<3;i++) {
            value = strtol(temp.Data(),&cstop,10);
            if (cstop==NULL)
               return true;
            this->SetOrderIndexAt(i,value);
            if (*cstop==']')
               break;
            temp = temp((int)(cstop+1-temp.Data()),temp.Length());
         }
      }
      // handle '{'
      if (i3!=-1) {
         if ((ie=subPath.Index("}",1,i3,TString::kExact))==-1) {
            cout << "\nID statement not closed in table '" << this->GetTableNameAt(nTable) << "'." << endl;
            return false;
         }
         itemp=subPath.Index(",",1,i3,TString::kExact);
         if (itemp==-1 || itemp>=ie) {
            cout << "\nConnection id statment in table '" << this->GetTableNameAt(nTable) << "' must contain a comma." << endl;
            return false;
         }
         ROMEString temp = subPath(i3+1,itemp-i3-1);
         temp.Strip(TString::kBoth,' ');
         temp.Strip(TString::kBoth,'"');
         this->SetTableIDNameAt(nTable,temp.Data());
         temp = subPath(itemp+1,ie-itemp-1);
         temp.Strip(TString::kBoth,' ');
         temp.Strip(TString::kBoth,'"');
         this->SetTableIDXNameAt(nTable,temp.Data());
      }
   }
   if (strlen(this->GetOrderTableName())<=0)
      this->SetOrderTableName(this->GetTableNameAt(nTable));

   // handle order
   if (orderPath.Length()>0) {
      ROMEPath *order = new ROMEPath();
      orderPath.Insert(0,"/");
      order->Decode(orderPath.Data(),runNumber);
      if (order->GetNumberOfTables()!=1) {
         cout << "\nOrder statment has to look like this : 'Table'/'Field'['start','end','step']." << endl;
         delete order;
         return false;
      }
      this->SetOrderArray(order->IsFieldArray());
      this->SetOrderTableName(order->GetTableNameAt(0));
      this->SetOrderFieldName(order->GetFieldName());
      for (int i=0;i<3;i++)
         this->SetOrderIndexAt(i,order->GetFieldIndexAt(i));
      delete order;
   }

   // extract field
   index=path.Index("[",1,0,TString::kExact);
   this->SetFieldIndexAt(0,0);
   this->SetFieldIndexAt(1,-1);
   this->SetFieldIndexAt(2,1);
   if (index==-1) {
      this->SetFieldName(path);
      this->SetFieldArray(false);
      return true;
   }
   else {
      this->SetFieldName(((TString)path(0,index)).Data());
      this->SetFieldArray(true);
   }
   path = path(index+1,path.Length());
   abspathposition += index+1;

   for (int i=0;i<3;i++) {
      value = strtol(path.Data(),&cstop,10);
      if (cstop==NULL)
         return true;
      if (*cstop!=',' && *cstop!=']') {
         cout << "\nError in array statement." << endl;
         return false;
      }
      this->SetFieldIndexAt(i,value);
      if (*cstop==']')
         return true;
      path = path((int)(cstop+1-path.Data()),path.Length());
   }
   return true;
}

void ROMEPath::Print() {
   cout << "Tables : " << endl;
   for (int i=0;i<this->GetNumberOfTables();i++) {
      cout << "   Name : " << this->GetTableNameAt(i) << endl;
      cout << "      Constraint     : " << this->GetTableConstraintAt(i) << endl;
      cout << "      DB Constraint  : " << this->GetTableDBConstraintAt(i) << endl;
      cout << "      Connection ID  : " << this->GetTableIDNameAt(i) << endl;
      cout << "      Connection IDX : " << this->GetTableIDXNameAt(i) << endl;
      cout << endl;
   }
   cout << "\nOrder : " << endl;
   cout << "   Table Name : " << this->GetOrderTableName() << endl;
   cout << "   Field Name : " << this->GetOrderFieldName() << endl;
   if (this->IsOrderArray()) {
      cout << "\n   Array : " << endl;
      cout << "      start : " << this->GetOrderIndexAt(0) << endl;
      cout << "      end   : " << this->GetOrderIndexAt(1) << endl;
      cout << "      step  : " << this->GetOrderIndexAt(2) << endl;
      cout << endl;
   }
   cout << "\nField : " << endl;
   cout << "   Name : " << this->GetFieldName() << endl;
   if (this->IsFieldArray()) {
      cout << "\n   Array : " << endl;
      cout << "      start : " << this->GetFieldIndexAt(0) << endl;
      cout << "      end   : " << this->GetFieldIndexAt(1) << endl;
      cout << "      step  : " << this->GetFieldIndexAt(2) << endl;
   }
}

void ROMEPath::GetAbsolutePath( ROMEString& path, const char* tablename ){
   int i;
   for(i=1;i<fTableNames->GetEntriesFast();i++){
      if(fTableNames->At(i)==tablename){
         path = fTableAbsolutePaths->At(i-1).Data();
         path += "/";
         path += tablename;
         return;
      }
   }
   path = tablename;
   return;
}

int ROMEPath::MinPosition(int i1,int i2,int i3,int i4) {
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
