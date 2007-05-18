////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// DOTFillHisto                                                               //
//                                                                            //
// Begin_Html                                                                 //
//                                                                            //
// Description:                                                               //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Usage:                                                                     //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Status:                                                                    //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// To Do:                                                                     //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Known Problems:                                                            //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// End_Html                                                                   //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
//                                                                            //
// This task contains the following histgrams :                               //
//    MyHisto                                                                 //
//    MyOtherHisto                                                            //
//    MySingleHisto                                                           //
//    My2DHisto                                                               //
// This task contains the following graphs :                                  //
//    MyGraph                                                                 //
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
// Generated header file containing necessary includes                        //
#include "generated/DOTFillHistoGeneratedIncludes.h"
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/DOAnalyzer.h"

#include "include/tasks/DOTFillHisto.h"
#include <TRandom.h>
#include "Riostream.h"

ClassImp(DOTFillHisto)

void DOTFillHisto::Init()
{
}

void DOTFillHisto::BeginOfRun()
{
}

void DOTFillHisto::Event()
{
   GetMyHistoAt(0)->Fill(gRandom->Gaus(0,10));
   GetMyHistoAt(1)->Fill(gRandom->Gaus(0,20));
   GetMyHistoAt(2)->Fill(gRandom->Gaus(0,30));
   GetMyOtherHistoAt(0)->Fill(gRandom->Gaus(10,10));
   GetMyOtherHistoAt(1)->Fill(gRandom->Gaus(10,20));
   GetMyOtherHistoAt(2)->Fill(gRandom->Gaus(10,30));
   GetMySingleHisto()->Fill(gRandom->Gaus(0,2));
   GetMy2DHistoAt(0)->Fill(gRandom->Gaus(10,10),gRandom->Gaus(10,10));
   GetMy2DHistoAt(1)->Fill(gRandom->Gaus(10,20),gRandom->Gaus(10,20));
   GetMy2DHistoAt(2)->Fill(gRandom->Gaus(10,30),gRandom->Gaus(10,30));
   for (int i=0;i<3;i++)
      for (int j=0;j<6;j++)
         GetMyGraphAt(i)->SetPoint(j,j,gRandom->Gaus(0,1));
}

void DOTFillHisto::EndOfRun()
{
}

void DOTFillHisto::Terminate()
{
}

