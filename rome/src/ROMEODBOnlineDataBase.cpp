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
#include "ROMEPath.h"
#include "ROMEAnalyzer.h"
#include "ROMEODBOnlineDataBase.h"
#include "ROMEiostream.h"
#if defined (HAVE_MIDAS)
#   include "ROMEMidasDAQ.h"
#endif

ClassImp(ROMEODBOnlineDataBase)

//______________________________________________________________________________
ROMEODBOnlineDataBase::ROMEODBOnlineDataBase()
:ROMEDataBase()
{
}

//______________________________________________________________________________
ROMEODBOnlineDataBase::~ROMEODBOnlineDataBase() {
}

//______________________________________________________________________________
Bool_t ROMEODBOnlineDataBase::Init(const char* name, const char* /*path*/, const char* /*connection*/) {
   fName = name;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOnlineDataBase::Read(ROMEStr2DArray *values, const char *dataBasePath, Long64_t runNumber, Long64_t eventNumber)
{
#if defined (HAVE_MIDAS)
   if (gROME && gROME->GetActiveDAQ() && gROME->isOnline() && gROME->IsActiveDAQ("midas")) {
      return static_cast<ROMEMidasDAQ*>(gROME->GetActiveDAQ())->
            ReadODBOnline(values, dataBasePath, runNumber, eventNumber);
   } else {
      ROMEPrint::Error("Please select MIDA DAQ system in the ROME configuration\n");
      return kFALSE;
   }
#else
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
   return kTRUE;
#endif
}

//______________________________________________________________________________
Bool_t ROMEODBOnlineDataBase::Write(ROMEStr2DArray* values, const char * dataBasePath, Long64_t runNumber, Long64_t eventNumber)
{
#if defined( HAVE_MIDAS )
   if (gROME && gROME->GetActiveDAQ() && gROME->isOnline() && gROME->IsActiveDAQ("midas")) {
      return static_cast<ROMEMidasDAQ*>(gROME->GetActiveDAQ())->
            WriteODBOnline(values, dataBasePath, runNumber, eventNumber);
   } else {
      ROMEPrint::Error("Please select MIDA DAQ system in the ROME configuration\n");
      return kFALSE;
   }
#else
   WarningSuppression(values);
   WarningSuppression(dataBasePath);
   WarningSuppression(runNumber);
   WarningSuppression(eventNumber);
   return kTRUE;
#endif
}
