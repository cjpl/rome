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
     3.4,                  // <-- there is a comma
     2.2                   // <-- no comma
     2.1, 4.5
     ...
     5.0, 0.0
     </valuename>
     Letters between <valuename> and </valuename> are recognized as data.
     In the above example, 3.4 and 2.2 are recognized in the same row.

     If there is + at the end, new data will be added after existing data.
     If there is - at the end, new data will be added before existing data.
     When reading, data which was found first will be used.

   In text files, C like comment is available.

  $Id$

********************************************************************/
#ifndef ROMETextDataBase_H
#define ROMETextDataBase_H

#include <Riostream.h>
#include "ROMEDataBase.h"
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4244 )
#endif // R__VISUAL_CPLUSPLUS
#include <TTimeStamp.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

class ROMETextDataBase : public ROMEDataBase
{
protected:
   ROMEString  fDirectoryPath;
   TTimeStamp *fTime;

public:
   ROMETextDataBase();
   ~ROMETextDataBase();

   Bool_t      Init(const char* name,const char* path,const char* connection);
   Bool_t      Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   Bool_t      Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   char       *GetType() { return "text"; }
   char       *GetDescription() { return "Text data base using the file system to store tables"; }

protected:
   void        RemoveComment(ROMEString &buffer,Bool_t initialize = false);
   Bool_t      ContainsData(const char* buffer);
   const char *StartMark(const char* valueName);
   const char *EndMark(const char* valueName);
   void        AddHeader(ROMEString &buffer,const char* fileName);
   void        RemoveHeader(ROMEString &buffer);

   ClassDef(ROMETextDataBase, 0) // Simple database class
};

#endif   // ROMETexDataBase_H
