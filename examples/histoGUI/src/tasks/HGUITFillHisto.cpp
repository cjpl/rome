////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGUITFillHisto                                                             //
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
//    MyOtherHisto                                                            //
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
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/HGUITFillHistoGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/HGUIAnalyzer.h"

#include "include/tasks/HGUITFillHisto.h"
#include <TRandom.h>
#include "Riostream.h"

ClassImp(HGUITFillHisto)

void HGUITFillHisto::Init()
{
}

void HGUITFillHisto::BeginOfRun()
{
}

void HGUITFillHisto::Event()
{
   GetMyHistoAt(0)->Fill(gRandom->Gaus(0,10));
   GetMyHistoAt(1)->Fill(gRandom->Gaus(0,20));
   GetMyHistoAt(2)->Fill(gRandom->Gaus(0,30));
   GetMyOtherHisto()->Fill(gRandom->Gaus(0,40));
   GetMyGraph()->SetPoint(0,0,gRandom->Gaus(0,1));
   GetMyGraph()->SetPoint(1,1,gRandom->Gaus(0,1));
   GetMyGraph()->SetPoint(2,2,gRandom->Gaus(0,1));
   GetMyGraph()->SetPoint(3,3,gRandom->Gaus(0,1));
   GetMyGraph()->SetPoint(4,4,gRandom->Gaus(0,1));
}

void HGUITFillHisto::EndOfRun()
{
}

void HGUITFillHisto::Terminate()
{
}

