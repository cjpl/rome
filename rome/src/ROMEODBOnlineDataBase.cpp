// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOnlineDataBase
//
//  Access to the Midas ODB parameters online.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEiostream.h"
#include "ROMEODBOnlineDataBase.h"

ClassImp(ROMEODBOnlineDataBase)

ROMEODBOnlineDataBase::ROMEODBOnlineDataBase() {
}

ROMEODBOnlineDataBase::~ROMEODBOnlineDataBase() {
}

Bool_t ROMEODBOnlineDataBase::Init(const char* name,const char* /*path*/,const char* /*connection*/) {
   fName = name;
   return true;
}

Bool_t ROMEODBOnlineDataBase::Read(ROMEStr2DArray */*values*/,const char */*dataBasePath*/,Long64_t /*runNumber*/,Long64_t /*eventNumber*/)
{
   return true;
}

Bool_t ROMEODBOnlineDataBase::Write(ROMEStr2DArray* /*values*/,const char */*dataBasePath*/,Long64_t /*runNumber*/,Long64_t /*eventNumber*/)
{
   return true;
}
