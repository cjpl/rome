// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOfflineDataBase
//
//  Access to the Midas ODB parameters in the header of a midas file.
//
//  $Log$
//  Revision 1.1  2005/04/14 07:58:26  schneebeli_m
//  Implemented odb database (offline)
//
//
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEODBOfflineDataBase.h>

ROMEODBOfflineDataBase::ROMEODBOfflineDataBase() {
   xml = new ROMEXML();
}

ROMEODBOfflineDataBase::~ROMEODBOfflineDataBase() {
   delete xml;
}

bool ROMEODBOfflineDataBase::Init(const char* name,const char* path,const char* connection) {
   fName = name;
   return true;
}
bool ROMEODBOfflineDataBase::SetBuffer(char *buffer) {
   return xml->OpenBufferForPath(buffer);
}

bool ROMEODBOfflineDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber)
{
   if (!xml->isPathOpen())
      return false;
   int i,j;
   ROMEPath *path = new ROMEPath();
   ROMEString odbPath = "/";
   ROMEString value;

   // decode path
   if (!path->Decode(dataBasePath,runNumber)) {
      cout << "\nPath decode error : " << dataBasePath << endl;
      return false;
   }
   
//   path->Print();

   for (i=0;i<path->GetNumberOfTables();i++) {
      odbPath += path->GetTableNameAt(i);
      if (strcmp(path->GetTableConstraintAt(i),"")) {
         odbPath += "[";
         odbPath += path->GetTableConstraintAt(i);
         odbPath += "]";
      }
      if (strcmp(path->GetTableDBConstraintAt(i),"")) {
         odbPath += "[@";
         odbPath += path->GetTableDBConstraintAt(i);
         odbPath += "]";
      }
      odbPath += "/";
   }
   odbPath += path->GetFieldName();
   if (strcmp(path->GetFieldConstraints(),"")) {
      odbPath += "[";
      odbPath += path->GetFieldConstraints();
      odbPath += "]";
   }
   xml->GetPathValue(odbPath.Data(),value);
   values->SetAt(value.Data(),0,0);

   delete path;
   return true; 
}

bool ROMEODBOfflineDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber)
{
   return true; 
}
