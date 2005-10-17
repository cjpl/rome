// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOnlineDataBase
//
//  Access to the Midas ODB parameters online.
//
//  $Log$
//  Revision 1.2  2005/07/12 06:42:22  sawada
//  Bug fix. Matched the name of method (IsActiveID and IsActiveEventID)
//
//  Revision 1.1  2005/04/14 07:58:26  schneebeli_m
//  Implemented odb database (offline)
//
//
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
