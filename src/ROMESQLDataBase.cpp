// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBase
//
//  SQLDataBase access.
//
//  $Log$
//  Revision 1.4  2004/11/11 12:55:28  schneebeli_m
//  Implemented XML database with new path rules
//
//  Revision 1.3  2004/10/05 07:52:44  schneebeli_m
//  dyn. Folders, TRef Objects, XML format changed, ROMEStatic removed
//
//  Revision 1.2  2004/09/30 13:08:21  schneebeli_m
//  ...
//
//  Revision 1.1  2004/09/27 18:26:05  schneebeli_m
//  new database classes
//
//
//////////////////////////////////////////////////////////////////////////
#include <ROMEString.h>
#include <TObjString.h>

#include <ROMESQLDataBase.h>

ROMESQLDataBase::ROMESQLDataBase() {
}

ROMESQLDataBase::~ROMESQLDataBase() {
}

bool ROMESQLDataBase::Init(const char* path,const char* runTableName) {
   return true;
}

bool ROMESQLDataBase::Read(ROMEStr2DArray *values,const char *path){
  return true;
}

bool ROMESQLDataBase::Write(ROMEStr2DArray* values,const char *path) {
  return true;
}
