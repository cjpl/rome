////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// NFTFillHistoAndFolder                                                      //
//                                                                            //
//                                                                            //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//                                                                            //
//  This task contains the following histgrams :                              //
//     Histo1                                                                 //
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
// Followings are include files of folders. ROMEBuilder will update it with   //
// reading this source code when it is executed next time.                    //
#include "generated/NFGaussData.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include <TRandom.h>
#include "include/tasks/NFTFillHistoAndFolder.h"
#include "Riostream.h"

ClassImp(NFTFillHistoAndFolder)

void NFTFillHistoAndFolder::Init()
{
}

void NFTFillHistoAndFolder::BeginOfRun()
{
   gAnalyzer->GetGaussData()->SetBinSize(200);
}

void NFTFillHistoAndFolder::Event()
{
   double value = gRandom->Gaus(0,20);
   FillHisto1(value);
   int ivalue = (int)(value+100);
   if (ivalue>=0 && ivalue<200)
      gAnalyzer->GetGaussData()->SetBinAt(ivalue,gAnalyzer->GetGaussData()->GetBinAt(ivalue)+1);
}

void NFTFillHistoAndFolder::EndOfRun()
{
}

void NFTFillHistoAndFolder::Terminate()
{
}

