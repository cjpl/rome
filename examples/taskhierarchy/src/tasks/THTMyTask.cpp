//// Author: 
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// THTMyTask                                                                  //
//                                                                            //
// 
// 
// This header has been generated by the ROMEBuilder.
// The event methods have been written by .
// 
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include <include/generated/THTMyTask.h>
#include <Riostream.h>

ClassImp(THTMyTask)

void THTMyTask::Init()
{
   cout << GetSP()->Getoutput().Data() << endl;
}

void THTMyTask::BeginOfRun()
{
   gAnalyzer->SetTerminationFlag();
}

void THTMyTask::Event()
{
}

void THTMyTask::EndOfRun()
{
}

void THTMyTask::Terminate()
{
}

