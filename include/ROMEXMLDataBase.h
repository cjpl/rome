/********************************************************************
  ROMEXMLDataBase.h, M. Schneebeli PSI

  $Log$
  Revision 1.5  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.4  2005/01/27 16:21:06  schneebeli_m
  print method & no gROME in path

  Revision 1.3  2004/11/16 16:14:00  schneebeli_m
  implemented task hierarchy

  Revision 1.2  2004/11/11 12:55:27  schneebeli_m
  Implemented XML database with new path rules

  Revision 1.1  2004/09/27 18:25:25  schneebeli_m
  new database classes

********************************************************************/
#ifndef ROMEXMLDataBase_H
#define ROMEXMLDataBase_H

#include <ROMEString.h>
#include <ROMEStrArray.h>
#include <ROMEStr2DArray.h>
#include <ROMEXML.h>
#include <ROMEPath.h>
#include <ROMEDataBase.h>
#include <Riostream.h>

class ROMEXMLDataBase : public ROMEDataBase
{
protected:
   ROMEString    fDirectoryPath;
   ROMEString    fDataBaseName;
   ROMEStrArray  fOrderTableIDs;
   int           fOrderTableIndex;

public:
   ROMEXMLDataBase();
   ~ROMEXMLDataBase();

   bool   Init(const char* name,const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *path,int runNumber);
   bool   Write(ROMEStr2DArray* values,const char *path,int runNumber);
   char*  GetType() { return "xml"; }
   char*  GetDescription() { return "XML data base reflecting the structure of an SQL data base"; }
};

#endif   // ROMEXMLDataBase_H
