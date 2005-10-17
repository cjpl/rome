// Author: Jan Wouters
//////////////////////////////////////////////////////////////////////////
//
//  ROMEOrca
//
//  Interface to the Orca System.
//
//  $Log$
//  Revision 1.1  2005/07/01 08:21:33  schneebeli_m
//  added to cvs
//
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
