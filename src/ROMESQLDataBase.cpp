// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMESQLDataBase
//
//  SQLDataBase access.
//
//  $Log$
//  Revision 1.1  2004/09/27 18:26:05  schneebeli_m
//  new database classes
//
//
//////////////////////////////////////////////////////////////////////////
#include <ROMEString.h>
#include <ROMEStatic.h>
#include <TObjString.h>

#include <ROMESQLDataBase.h>

ROMESQLDataBase::ROMESQLDataBase() {
}

ROMESQLDataBase::~ROMESQLDataBase() {
}

bool ROMESQLDataBase::Init(const char* path,const char* runTableName) {
   return true;
}

void ROMESQLDataBase::Read(TObjArray *values,const char *pathI,int runNumber,int numberOfFolders,int numberOfFields)
{
}
void ROMESQLDataBase::Write(const char *path,TObjArray* values)
{
}
