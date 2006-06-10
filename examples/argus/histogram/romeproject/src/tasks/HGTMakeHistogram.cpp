// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGTMakeHistogram                                                           //
//                                                                            //
// Create and fill histograms at the begin of run                             //
//                                                                            //
// The event methods have been written by Ryu Sawada.                         //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
//                                                                            //
//  This task contains the following histgrams :                              //
//     hAdc                                                                   //
//                                                                            //
// The histograms are created and saved automaticaly by the task.             //
//                                                                            //
// The following method can be used to fill a histogram :                     //
//                                                                            //
// Fill<Histogram Name>(Double_t value,Double_t weight)                       //
//                                                                            //
// For histogram arrays use :                                                 //
//                                                                            //
// Fill<Histogram Name>At(Int_t index,Double_t value,Double_t weight)         //
//                                                                            //
// If more histogram functions are needed use the following function the get  //
//  a handle to the histogram and use the root functions.                     //
//                                                                            //
// Get<Histogram Name>()                                                      //
//                                                                            //
// For histogram arrays use :                                                 //
//                                                                            //
// Get<Histogram Name>At(Int_t index)                                         //
//                                                                            //
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
   FillhAdc(rndm->Gaus(2000, 200));
}

void HGTMakeHistogram::EndOfRun()
{
}

void HGTMakeHistogram::Terminate()
{
}
