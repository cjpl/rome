////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MRTFillHisto                                                               //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
// The event methods have been written by .                                   //
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
#include "generated/MRTFillHistoGeneratedIncludes.h"

/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/MRAnalyzer.h"

#include "include/tasks/MRTFillHisto.h"
#include <TRandom.h>
#include <Riostream.h>

ClassImp(MRTFillHisto)

void MRTFillHisto::Init()
{
}

void MRTFillHisto::BeginOfRun()
{
}

void MRTFillHisto::Event()
{
   GetMyHisto()->Fill(gRandom->Gaus(0,20/((double)gAnalyzer->GetCurrentRunNumber()*gAnalyzer->GetCurrentRunNumber())));
}

void MRTFillHisto::EndOfRun()
{
}

void MRTFillHisto::Terminate()
{
   GetMyHisto()->Draw();
}

