////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// THTTaskA                                                                   //
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

#include "include/tasks/THTTaskA.h"
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

