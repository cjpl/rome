//// Author: 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// THTTaskA                                                                   //
//                                                                            //
// 
// 
// This header has been generated by the ROMEBuilder.
// The event methods have been written by .
// 
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/THTTaskA.h"
#include "Riostream.h"

ClassImp(THTTaskA)

void THTTaskA::Init()
{
   cout << "A:" << GetSP()->Getoutput().Data() << endl;

}
void THTTaskA::BeginOfRun()
{
//   this->StartRootInterpreter();
//   gAnalyzer->SetTerminationFlag();
}

void THTTaskA::Event()
{
}

void THTTaskA::EndOfRun()
{
}

void THTTaskA::Terminate()
{
}

