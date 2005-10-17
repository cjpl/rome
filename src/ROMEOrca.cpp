// Author: Jan Wouters
//////////////////////////////////////////////////////////////////////////
//
//  ROMEOrca
//
//  Interface to the Orca System.
//
//  $Id:$
//
//////////////////////////////////////////////////////////////////////////

#include <ROMEOrca.h>

ROMEOrca::ROMEOrca() {
}

bool ROMEOrca::Init() {
   cout << "This is Orca" << endl;
   return true;
}
bool ROMEOrca::BeginOfRun() {
   return true;
}
bool ROMEOrca::Event(int event) {
   return true;
}
bool ROMEOrca::EndOfRun() {
   return true;
}
bool ROMEOrca::Terminate() {
   return true;
}
