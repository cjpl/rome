////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// REFTFillHits                                                               //
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
// This task accesses the following folders :                                 //
//     Hit                                                                    //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/REFTFillHitsGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include <TRandom.h>
#include "generated/REFAnalyzer.h"
#include "tasks/REFTFillHits.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "REFAllFolders.h"


ClassImp(REFTFillHits)

//______________________________________________________________________________
void REFTFillHits::Init()
{
}

//______________________________________________________________________________
void REFTFillHits::BeginOfRun()
{
}

//______________________________________________________________________________
void REFTFillHits::Event()
{
   // This task simulates hits in a detector.
   Int_t nCluster = gRandom->Integer(10);
   Int_t iCluster, iHit;
   Double_t xCluster, yCluster, zCluster, tCluster;
   Int_t nHit;

   for (iCluster = 0; iCluster < nCluster; iCluster++) {
      xCluster = gRandom->Rndm() * 10;
      yCluster = gRandom->Rndm() * 10;
      zCluster = gRandom->Rndm() * 10;
      tCluster = gRandom->Rndm() * 100e-9;
      nHit = gRandom->Poisson(5);
      gAnalyzer->SetHitSize(nHit);
      for (iHit = 0; iHit < nHit; iHit++) {
         gAnalyzer->GetHitAt(iHit)->SetX(xCluster + gRandom->Gaus(0, 0.5));
         gAnalyzer->GetHitAt(iHit)->SetY(yCluster + gRandom->Gaus(0, 0.5));
         gAnalyzer->GetHitAt(iHit)->SetZ(zCluster + gRandom->Gaus(0, 0.5));
         gAnalyzer->GetHitAt(iHit)->SetT(tCluster + gRandom->Gaus(0, 5e-9));
      }
   }
}

//______________________________________________________________________________
void REFTFillHits::EndOfRun()
{
}

//______________________________________________________________________________
void REFTFillHits::Terminate()
{
}
