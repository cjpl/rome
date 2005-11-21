// Author: Jan Wouters
//////////////////////////////////////////////////////////////////////////
//
//  ROMEOrcaDAQ
//
//  Interface to the Orca System.
//
//  $Id: ROMEOrcaDAQ.cpp 480 2005-10-17 12:12:54Z schneebeli_m $
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEOrcaDAQ.h>

ROMEOrcaDAQ::ROMEOrcaDAQ() {
}

bool ROMEOrcaDAQ::Init() {
   cout << "This is Orca" << endl;
   return true;
}
bool ROMEOrcaDAQ::BeginOfRun() {
   return true;
}
bool ROMEOrcaDAQ::Event(int event) {
   return true;
}
bool ROMEOrcaDAQ::EndOfRun() {
   return true;
}
bool ROMEOrcaDAQ::Terminate() {
   return true;
}
