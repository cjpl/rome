/********************************************************************
  ROMETextDataBase.h, R. Sawada

  Text format non-relational database.

  Arguments of Init
     name       : name of database
     path       : directory of files
     connection : not used

  Rules of dataBasePath
     Data with following dataBasePath
        filename/valuename+
        filename/valuename
        filename/valuename-
     will be stored in text file like
     <valuename>
     1.0, 1.0
     2.0, 1.0
     ...
     5.0, 0.0
     </valuename>
     Letters between <valuename> and </valuename> are recognized as data.

     If there is + at the end, new data will be added after existing data.
     If there is - at the end, new data will be added before existing data.
     When reading, data which was found first will be used.

   In text files, C like comment is available.

  $Id:$

********************************************************************/
#ifndef ROMETextDataBase_H
#define ROMETextDataBase_H

#include <Riostream.h>
#include <ROMEDataBase.h>
#include <TTimeStamp.h>

class ROMETextDataBase : public ROMEDataBase
{
protected:
   ROMEString fDirectoryPath;
   TTimeStamp* fTime;

public:
   ROMETextDataBase();
   ~ROMETextDataBase();

   bool   Init(const char* name,const char* path,const char* connection);
   bool   Read(ROMEStr2DArray *values,const char *dataBasePath,int runNumber);
   bool   Write(ROMEStr2DArray* values,const char *dataBasePath,int runNumber);
   char*  GetType() { return "text"; }
   char*  GetDescription() { return "Text data base using the file system to store tables"; }

protected:
   void   RemoveComment(ROMEString &buffer,bool initialize = false);
   bool   ContainsData(const char* buffer);
   const char* StartMark(const char* valueName);
   const char* EndMark(const char* valueName);
   void   AddHeader(ROMEString &buffer,const char* fileName);
   void   RemoveHeader(ROMEString &buffer);
};

#endif   // ROMETexDataBase_H
