/********************************************************************
  ROMEODBOnlineDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2005/04/14 07:58:10  schneebeli_m
  Implemented odb database (offline)


********************************************************************/
#ifndef ROMEODBOnlineDataBase_H
#define ROMEODBOnlineDataBase_H

#include <ROMEString.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
#include <ROMEXML.h>
#include <ROMEPath.h>
#include <ROMEDataBase.h>
#include <Riostream.h>

class ROMEODBOnlineDataBase : public ROMEDataBase
{
protected:

public:
   ROMEODBOnlineDataBase();
   ~ROMEODBOnlineDataBase();

   bool   Init(const char* name,const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber);
   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber);
   char*  GetType() { return "odb"; };
   char*  GetDescription() { return "Midas ODB database for online use"; };
};

#endif   // ROMEXMLDataBase_H
