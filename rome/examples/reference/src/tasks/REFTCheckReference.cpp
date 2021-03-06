////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// REFTCheckReference                                                         //
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
//     Cluster                                                                //
//     MainCluster                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/REFTCheckReferenceGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/REFAnalyzer.h"
#include "tasks/REFTCheckReference.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "REFAllFolders.h"

ClassImp(REFTCheckReference)

namespace {
   Bool_t CheckCluster(REFCluster *a, REFCluster *b) {
      return a && b &&
            a->GetX() == b->GetX() &&
            a->GetY() == b->GetY() &&
            a->GetZ() == b->GetZ() &&
            a->GetT() == b->GetT() &&
            a->GetHitSize() == b->GetHitSize();
   }
   Bool_t CheckHit(REFHit *a, REFHit *b) {
      return a && b &&
            a->GetX() == b->GetX() &&
            a->GetY() == b->GetY() &&
            a->GetZ() == b->GetZ() &&
            a->GetT() == b->GetT();
   }
}

//______________________________________________________________________________
void REFTCheckReference::Init()
{
}

//______________________________________________________________________________
void REFTCheckReference::BeginOfRun()
{
}

//______________________________________________________________________________
void REFTCheckReference::Event()
{
   // this task check if TRef and TRefArray work.

   Int_t id;
   Int_t iCluster, iHit;

   try {
      // check the largest cluster
      if (gAnalyzer->GetMainCluster()->GetLargestClusterObj()) {
         id = gAnalyzer->GetMainCluster()->GetLargestClusterID();
         if (!CheckCluster(dynamic_cast<REFCluster*>(gAnalyzer->GetMainCluster()->GetLargestClusterObj()),
                           gAnalyzer->GetClusterAt(id))) throw 1;
      }

      // check the first hits
      for (iCluster = 0; iCluster < gAnalyzer->GetClusterSize(); iCluster++) {
         if(gAnalyzer->GetClusterAt(iCluster)->GetHitSize()) {
            id = gAnalyzer->GetClusterAt(iCluster)->GetFirstHitID();
            if (!CheckHit(dynamic_cast<REFHit*>(gAnalyzer->GetClusterAt(iCluster)->GetFirstHitObj()),
                          gAnalyzer->GetHitAt(id))) throw 1;
         }
      }

      // check all hit entries
      for (iCluster = 0; iCluster < gAnalyzer->GetClusterSize(); iCluster++) {
         for (iHit = 0; iHit < gAnalyzer->GetClusterAt(iCluster)->GetHitSize(); iHit++) {
            id = gAnalyzer->GetClusterAt(iCluster)->GetHitIDAt(iHit);
            if (!CheckHit(dynamic_cast<REFHit*>(gAnalyzer->GetClusterAt(iCluster)->GetHitAt(iHit)),
                          gAnalyzer->GetHitAt(id))) throw 1;
         }
      }
   }
   catch (Int_t err) {
      Report(R_ERROR, "TRef or TRefArray does not work correctly.");
      gAnalyzer->GetApplication()->Terminate(err);
   }
}

//______________________________________________________________________________
void REFTCheckReference::EndOfRun()
{
}

//______________________________________________________________________________
void REFTCheckReference::Terminate()
{
}
