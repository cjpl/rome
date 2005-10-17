// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOfflineDataBase
//
//  Access to the Midas ODB parameters in the header of a midas file.
//
//  $Id:$
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
