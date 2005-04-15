// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOfflineDataBase
//
//  Access to the Midas ODB parameters in the header of a midas file.
//
//  $Log$
//  Revision 1.2  2005/04/15 16:44:26  schneebeli_m
//  odb, zlib
//
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
   ROMEString value;
   if (!xml->isPathOpen())
      return false;
   xml->GetPathValue(dataBasePath,value);
   values->SetAt(value.Data(),0,0);
   return true; 
}

bool ROMEODBOfflineDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber)
{
   return true; 
}
