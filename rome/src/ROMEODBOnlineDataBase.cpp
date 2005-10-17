// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOnlineDataBase
//
//  Access to the Midas ODB parameters online.
//
//  $Id:$
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEODBOnlineDataBase.h>

ROMEODBOnlineDataBase::ROMEODBOnlineDataBase() {
}

ROMEODBOnlineDataBase::~ROMEODBOnlineDataBase() {
}

bool ROMEODBOnlineDataBase::Init(const char* name,const char* path,const char* connection) {
   fName = name;
   return true;
}

bool ROMEODBOnlineDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber)
{
   return true;
}

bool ROMEODBOnlineDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber)
{
   return true;
}
