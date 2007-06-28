// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGTMakeHistogram                                                           //
//                                                                            //
// Begin_Html <!--
/*-->

Description:
<p>
Create and fill histograms at the begin of run
</p>

<!--*/
// --> End_Html
//                                                                            //
// The event methods have been written by Ryu Sawada.                         //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
//                                                                            //
// This task contains the following histgrams :                               //
//    hAdc                                                                    //
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
#include "generated/HGTMakeHistogramGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/HGAnalyzer.h"

#include <include/tasks/HGTMakeHistogram.h>
#include <Riostream.h>
#include <TRandom.h>

ClassImp(HGTMakeHistogram)

TRandom *rndm;

void HGTMakeHistogram::Init()
{
   rndm = new TRandom();
}

void HGTMakeHistogram::BeginOfRun()
{
}

void HGTMakeHistogram::Event()
{
   GethAdc()->Fill(rndm->Gaus(2000, 200));
}

void HGTMakeHistogram::EndOfRun()
{
}

void HGTMakeHistogram::Terminate()
{
}
