// Author: Jan Wouters
//////////////////////////////////////////////////////////////////////////
//
//  ROMEOrcaDAQ
//
//  Interface to the Orca System.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEAnalyzer.h"
#include "ROMEOrcaDAQ.h"

ClassImp(ROMEOrcaDAQ)

//______________________________________________________________________________
ROMEOrcaDAQ::ROMEOrcaDAQ() {
}

//______________________________________________________________________________
Bool_t ROMEOrcaDAQ::Init() {
   ROMEPrint::Print("This is Orca\n");
   return true;
}

//______________________________________________________________________________
Bool_t ROMEOrcaDAQ::BeginOfRun() {
   return true;
}

//______________________________________________________________________________
Bool_t ROMEOrcaDAQ::Event(Long64_t /*event*/) {
   return true;
}

//______________________________________________________________________________
Bool_t ROMEOrcaDAQ::EndOfRun() {
   return true;
}

//______________________________________________________________________________
Bool_t ROMEOrcaDAQ::Terminate() {
   return true;
}
