// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOfflineDataBase
//
//  Access to the Midas ODB parameters in the header of a midas file.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEODBOfflineDataBase.h>

ROMEODBOfflineDataBase::ROMEODBOfflineDataBase() {
   xml = new ROMEXML();
}

ROMEODBOfflineDataBase::~ROMEODBOfflineDataBase() {
   delete xml;
}

Bool_t ROMEODBOfflineDataBase::Init(const char* name,const char* path,const char* connection) {
   fName = name;
   return true;
}

Bool_t ROMEODBOfflineDataBase::SetBuffer(char *buffer) {
   return xml->OpenBufferForPath(buffer);
}

Bool_t ROMEODBOfflineDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)
{
   ROMEString value;
   if (!xml->isPathOpen())
      return false;
   xml->GetPathValue(dataBasePath,value);
   values->SetAt(value.Data(),0,0);
   return true;
}

Bool_t ROMEODBOfflineDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)
{
   return true;
}
