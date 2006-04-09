/********************************************************************
  ROMEOrcaDAQ.h, J. Wouters 

  $Id$

********************************************************************/
#ifndef ROMEOrcaDAQ_H
#define ROMEOrcaDAQ_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMEOrcaDAQ : public ROMEDAQSystem {

public:
   ROMEOrcaDAQ::ROMEOrcaDAQ();

   // Additional Getters
   Int_t       GetTimeStamp() { return 0; };  // TODO
   const char *GetName() { return "orca"; };

   Bool_t      Init();
   Bool_t      BeginOfRun();
   Bool_t      Event(Long64_t event);
   Bool_t      EndOfRun();
   Bool_t      Terminate();
};

#endif   // ROMEOrcaDAQ_H
