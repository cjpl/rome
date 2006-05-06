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

#include <ROMEOrcaDAQ.h>

ClassImp(ROMEOrcaDAQ)

ROMEOrcaDAQ::ROMEOrcaDAQ() {
}

Bool_t ROMEOrcaDAQ::Init() {
   cout << "This is Orca" << endl;
   return true;
}

Bool_t ROMEOrcaDAQ::BeginOfRun() {
   return true;
}

Bool_t ROMEOrcaDAQ::Event(Long64_t event) {
   return true;
}

Bool_t ROMEOrcaDAQ::EndOfRun() {
   return true;
}

Bool_t ROMEOrcaDAQ::Terminate() {
   return true;
}
