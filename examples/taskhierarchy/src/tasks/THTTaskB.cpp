////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// THTTaskB                                                                   //
//                                                                            //
//                                                                            //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tasks/THTTaskB.h"
#include "Riostream.h"

ClassImp(THTTaskB)

void THTTaskB::Init()
{
   cout << "B:" << GetSP()->Getoutput().Data() << endl;
}

void THTTaskB::BeginOfRun()
{
//   gAnalyzer->SetTerminationFlag();
}

void THTTaskB::Event()
{
}

void THTTaskB::EndOfRun()
{
}

void THTTaskB::Terminate()
{
}

