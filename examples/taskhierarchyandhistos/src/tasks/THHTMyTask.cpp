////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// THHTMyTask                                                                 //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
//                                                                            //
// This task contains the following histgrams :                               //
//    MyHisto                                                                 //
//                                                                            //
// The histograms/graph are created and saved automaticaly by the task.       //
//                                                                            //
// The following method can be used to get a handle of the histogram/graph :  //
//                                                                            //
// Get<Histogram/Graph Name>()                                                //
//                                                                            //
// For histogram/graph arrays use :                                           //
//                                                                            //
// Get<Histogram/Graph Name>At(Int_t index)                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/THHTMyTaskGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/THHAnalyzer.h"


#include "tasks/THHTMyTask.h"
#include <TRandom.h>
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "THHAllFolders.h"


ClassImp(THHTMyTask)

void THHTMyTask::Init()
{
}

void THHTMyTask::BeginOfRun()
{
}

void THHTMyTask::Event()
{
   GetMyHistoAt(0)->Fill(gRandom->Gaus(GetSP()->GetMeanValue(),10));
   GetMyHistoAt(1)->Fill(gRandom->Gaus(GetSP()->GetMeanValue(),20));
   GetMyHistoAt(2)->Fill(gRandom->Gaus(GetSP()->GetMeanValue(),30));
}

void THHTMyTask::EndOfRun()
{
}

void THHTMyTask::Terminate()
{
}

