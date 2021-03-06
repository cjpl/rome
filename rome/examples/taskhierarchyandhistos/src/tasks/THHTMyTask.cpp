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

   Double_t x[200];
   Double_t y[200];
   Int_t i;
   for (i = 0; i < 200; i++) {
      x[i] =  0.5 * i;
      y[i] = gRandom->Rndm();
   }
   GetMyGraph()->Set(200);
   memcpy(GetMyGraph()->GetX(), x, sizeof(Double_t) * 200);
   memcpy(GetMyGraph()->GetY(), y, sizeof(Double_t) * 200);
   GetMyGraph()->GetXaxis()->SetLimits(-200, 200);
}

void THHTMyTask::EndOfRun()
{
}

void THHTMyTask::Terminate()
{
}

