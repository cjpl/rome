////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// THTTaskB                                                                   //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/THTTaskBGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/THAnalyzer.h"

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

