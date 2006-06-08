////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MRTFillHisto                                                               //
//                                                                            //
//                                                                            //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
//                                                                            //
//  This task contains the following histgrams :                              //
//     MyHisto                                                                //
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

