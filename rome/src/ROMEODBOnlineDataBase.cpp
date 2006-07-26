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

#if defined ( HAVE_MIDAS ) && !defined ( __MAKECINT__ )
#   include "midas.h"
#endif
#include "ROMEiostream.h"
#include "ROMEAnalyzer.h"
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

Bool_t ROMEODBOnlineDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t /*runNumber*/,Long64_t /*eventNumber*/)
{
   if (gROME->isOnline()) {
      HNDLE hKey;
      char name[500];
      int type,num_values,item_size;
      char buffer[500];
      int buffersize = 500;
      char value[500];
      if (db_find_key(gROME->GetMidasOnlineDataBase(), 0, (char*)dataBasePath, &hKey)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      if (db_get_key_info(gROME->GetMidasOnlineDataBase(), hKey,name,sizeof(name), &type, &num_values,&item_size)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      if (db_get_data(gROME->GetMidasOnlineDataBase(),hKey,&buffer,&buffersize,type)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      if (db_sprintf(value, buffer, buffersize, 0, type)!= CM_SUCCESS) {
         ROMEPrint::Error("\nCannot read online database\n");
         return false;
      }
      values->SetAt(value,0,0);
   }
   return true;
}

Bool_t ROMEODBOnlineDataBase::Write(ROMEStr2DArray* /*values*/,const char * /*dataBasePath*/,Long64_t /*runNumber*/,Long64_t /*eventNumber*/)
{
   return true;
}
