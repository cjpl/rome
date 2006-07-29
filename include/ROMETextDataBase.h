// $Id$
// Author: Ryu Sawada

#ifndef ROMETextDataBase_H
#define ROMETextDataBase_H

//////////////////////////////////////////////////////////////////////////
//
//  Text format non-relational database.
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEDataBase.h"

class TTimeStamp;

class ROMETextDataBase : public ROMEDataBase
{
protected:
   ROMEString  fDirectoryPath;
   TTimeStamp *fTime;

public:
   ROMETextDataBase();
   virtual ~ROMETextDataBase();

   Bool_t      Init(const char* name,const char* path,const char* connection);
   Bool_t      Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   Bool_t      Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber);
   const char *GetType() { return "text"; }
   const char *GetDescription() { return "Text data base using the file system to store tables"; }

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
