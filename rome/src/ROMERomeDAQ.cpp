// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMERomeDAQ
//
//  Interface to the output of ROME.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800)
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TBranchElement.h>
#include <TMath.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMEAnalyzer.h"
#include "ROMERomeDAQ.h"
#include "ROMETree.h"
#include "ROMETreeInfo.h"

ClassImp(ROMERomeDAQ)

ROMERomeDAQ::ROMERomeDAQ() {
   fTreeInfo = new ROMETreeInfo();
   fTreeIndex = 0;
   fInputFileNameIndex = -1;
   fRootFiles = 0;
   fROMETrees = 0;
   fTreePosition = 0;
   fTreePositionArray = 0;
   fTreeNEntries = 0;
}

ROMERomeDAQ::~ROMERomeDAQ() {
   SafeDelete(fTreeInfo);
   SafeDeleteArray(fTreePosition);
   SafeDeleteArray(fTreePositionArray);
   SafeDeleteArray(fTreeNEntries);
   Int_t j;
   const Int_t nTree = gROME->GetTreeObjectEntries();
   for (j=0;j<nTree;j++) {
      SafeDelete(fROMETrees[j]);
   }
   SafeDeleteArray(fROMETrees);

}

Bool_t ROMERomeDAQ::Init() {
   if (gROME->isOnline()) {
      ROMEPrint::Error("Rome mode is not supported for online analysis.\n");
      return false;
   }
   if (gROME->isOffline()) {
      int i;
      ROMEString filename;
      const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
      ROMEPrint::Print("Program is running offline.\n");
      if ((gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
         fRootFiles = new TFile*[nInputFile];
         for (i=0;i<nInputFile;i++) {
            filename.SetFormatted("%s%s",gROME->GetInputDir(),gROME->GetInputFileNameAt(i).Data());
            gROME->ReplaceWithRunAndEventNumber(filename);
            fRootFiles[i] = new TFile(filename.Data(),"READ");
            if (fRootFiles[i]->IsZombie()) {
               ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
               return false;
            }
         }
      }
   }

   const Int_t nTree = gROME->GetTreeObjectEntries();
   fTreePosition = new Long64_t[nTree];
   fTreePositionArray = new Long64_t*[nTree];
   fTreeNEntries = new Long64_t[nTree];

   Int_t j;
   fROMETrees = new ROMETree*[nTree];
   for (j=0;j<nTree;j++) {
      fROMETrees[j] = new ROMETree(new TTree());
   }
   return true;
}

Bool_t ROMERomeDAQ::BeginOfRun() {
   const Int_t nTree = gROME->GetTreeObjectEntries();
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();

   int i,j,k;
   for (j=0;j<nTree;j++) {
      fROMETrees[j]->SetFileName(gROME->GetTreeObjectAt(j)->GetFileName());
      fROMETrees[j]->SetConfigFileName(gROME->GetTreeObjectAt(j)->GetConfigFileName());
      fROMETrees[j]->SetRead(gROME->GetTreeObjectAt(j)->isRead());
      fROMETrees[j]->SetName(gROME->GetTreeObjectAt(j)->GetName());
      fROMETrees[j]->AllocateBranchActive(gROME->GetTreeObjectAt(j)->GetNBranchActive());
      memcpy(fROMETrees[j]->GetBranchActive(),
             gROME->GetTreeObjectAt(j)->GetBranchActive(),
             gROME->GetTreeObjectAt(j)->GetNBranchActive() * sizeof(Bool_t));
   }

   Int_t nKey;
   if (gROME->isOffline()) {
      this->SetRunning();
      // Read Trees
      ROMEString filename;
      ROMEString treename;
      if (gROME->IsRunNumberBasedIO())
         fRootFiles = new TFile*[nTree];
      TTree *tree;
      ROMETree *romeTree;
      ROMEString runNumberString;
      if ((gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()))
         gROME->GetCurrentRunNumberString(runNumberString);
      bool treeRead = false;
      for (j=0;j<nTree;j++) {
         romeTree = fROMETrees[j];
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            treeRead = true;

            if (gROME->IsRunNumberBasedIO()) {
               if(romeTree->GetConfigFileName().Length()) {
                  filename.SetFormatted("%s%s",gROME->GetInputDir(),romeTree->GetConfigFileName().Data());
                  gROME->ReplaceWithRunAndEventNumber(filename);
               }
               else {
                  filename.SetFormatted("%s%s%s.root",gROME->GetInputDir(),romeTree->GetName(),runNumberString.Data());
               }
               gSystem->ExpandPathName(filename);
               fRootFiles[j] = new TFile(filename.Data(),"READ");
               if (fRootFiles[j]->IsZombie()) {
                  ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
                  return false;
               }
               ROMEPrint::Print("Reading %s\n", filename.Data());
               tree->Read(romeTree->GetName());
               romeTree->SetFile(fRootFiles[j]);
            }
            else if (gROME->IsFileNameBasedIO()) {
               if (fTreeIndex>0) {
                  if (fTreeIndex>=fRootFiles[fInputFileNameIndex]->GetNkeys())
                     fTreeIndex = 0;
                  else {
                     treename.SetFormatted("%s_%d",fCurrentTreeName.Data(),fTreeIndex);
                     fRootFiles[fInputFileNameIndex]->cd();
                     tree->Read(treename.Data());
                     romeTree->SetFile(fRootFiles[fInputFileNameIndex]);
                     fTreeIndex++;
                     tree->SetName(fCurrentTreeName.Data());
                  }
               }
               if (fTreeIndex==0) {
                  fInputFileNameIndex++;
                  if (nInputFile<=fInputFileNameIndex) {
                     this->SetTerminate();
                     return true;
                  }
                  gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(fInputFileNameIndex).Data());
                  fRootFiles[fInputFileNameIndex]->cd();
                  ROMEPrint::Print("Reading %s%s\n",gROME->GetInputDir(),gROME->GetCurrentInputFileName().Data());
                  tree->Read(romeTree->GetName());
                  romeTree->SetFile(fRootFiles[fInputFileNameIndex]);
                  fCurrentTreeName = romeTree->GetName();
                  fTreeIndex++;
               }
            }
            else if (gROME->IsRunNumberAndFileNameBasedIO()) {
               fInputFileNameIndex = -1;
               fCurrentTreeName = romeTree->GetName();
               for (i=0;i<nInputFile;i++) {
                  nKey = fRootFiles[i]->GetNkeys();
                  for (k=0;k<nKey;k++) {
                     treename.SetFormatted("%s",fCurrentTreeName.Data());
                     if (k>0)
                        treename.AppendFormatted("_%d",k);
                     fRootFiles[i]->cd();
                     tree->Read(treename.Data());
                     romeTree->SetFile(fRootFiles[i]);
                     ((TBranchElement*)tree->FindBranch("Info"))->SetAddress(&fTreeInfo);
                     tree->GetBranch("Info")->GetEntry(0);
                     if (fTreeInfo->GetRunNumber()==gROME->GetCurrentRunNumber()) {
                        gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(i));
                        ROMEPrint::Print("Reading %s%s\n",gROME->GetInputDir(),gROME->GetCurrentInputFileName().Data());
                        tree->SetName(fCurrentTreeName.Data());
                        fInputFileNameIndex = i;
                        break;
                     }
                  }
               }
               if (fInputFileNameIndex==-1) {
#if defined( R__VISUAL_CPLUSPLUS )
                  ROMEPrint::Warning("Run %I64d not found in the specified input files !\n",gROME->GetCurrentRunNumber());
                  ROMEPrint::Warning("Skipping run %I64d.\n",gROME->GetCurrentRunNumber());
#else
                  ROMEPrint::Warning("Run %lld not found in the specified input files !\n",gROME->GetCurrentRunNumber());
                  ROMEPrint::Warning("Skipping run %lld.\n",gROME->GetCurrentRunNumber());
#endif
                  this->SetEndOfRun();
                  return true;
               }
            }
            romeTree->SetTree(tree);
            fTreePosition[j] = 0;
            fTreeNEntries[j] = tree->GetEntries();
         }
         else {
            fRootFiles[j] = 0;
         }
      }
      if (!treeRead) {
         ROMEPrint::Error("No input tree specified for running in rome mode.\n");
         return false;
      }
      this->ConnectTrees();

      // Get run number
      for (j=0;j<nTree;j++) {
         romeTree = fROMETrees[j];
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            if (tree->GetEntriesFast()>0) {
               tree->GetBranch("Info")->GetEntry(0);
               gROME->SetCurrentRunNumber(fTreeInfo->GetRunNumber());
            }
         }
      }

      // Prepare event number array
      Int_t iEvent;
      fMaxEventNumber = 0;
      fTreeInfo->SetEventNumber(0);
      for (j=0;j<nTree;j++) {
         romeTree = fROMETrees[j];
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            fTreePositionArray[j] = new Long64_t[(int)fTreeNEntries[j]]; // Warning : potential loss of data (int)
            for(iEvent = 0; iEvent < fTreeNEntries[j]; iEvent++) {
               tree->GetBranch("Info")->GetEntry(iEvent);
               fTreePositionArray[j][iEvent] = fTreeInfo->GetEventNumber();
            }
            if (fMaxEventNumber < fTreeInfo->GetEventNumber())
               fMaxEventNumber = fTreeInfo->GetEventNumber();
         }
         else {
            fTreePositionArray[j] = 0;
         }
      }
   }
   return true;
}

Bool_t ROMERomeDAQ::Event(Long64_t event) {
   if (gROME->isOffline()) {
      int j;
      ROMETree *romeTree;
      TTree *tree;
      bool found = false;
      const Int_t nTree = gROME->GetTreeObjectEntries();

      if (event > fMaxEventNumber) {
         this->SetEndOfRun();
         return true;
      }

      // read event
      for (j=0;j<nTree;j++) {
         romeTree = fROMETrees[j];
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            fTreePosition[j] = TMath::BinarySearch(fTreeNEntries[j], fTreePositionArray[j], event);
            if (fTreePosition[j] >= 0
                && fTreePosition[j] < fTreeNEntries[j]
                && fTreePositionArray[j][fTreePosition[j]] == event) {
               found = true;
               if (gROME->IsRunNumberBasedIO())
                  tree->SetDirectory(fRootFiles[j]);
               else if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())
                  tree->SetDirectory(fRootFiles[fInputFileNameIndex]);
               tree->GetEntry(fTreePosition[j]);
               gROME->SetCurrentEventNumber(fTreeInfo->GetEventNumber());
            }
         }
      }

      if (!found) {
         this->SetContinue();
         return true;
      }
   }

   return true;
}

Long64_t ROMERomeDAQ::Seek(Long64_t event)
{
   if (gROME->isOffline()) {
      if (event < 0 || event > fMaxEventNumber) {
#if defined(R__UNIX)
         Warning("Seek", "Event number %lld was not found.", event);
#else
         Warning("Seek", "Event number %I64d was not found.", event);
#endif
         event = gROME->GetCurrentEventNumber();
         gROME->SetDontReadNextEvent();
         SetContinue();
      }
   }
   return event;
}

Bool_t ROMERomeDAQ::EndOfRun() {
   if (gROME->isOffline()) {
      const Int_t nTree = gROME->GetTreeObjectEntries();
      int j;
      if (gROME->IsRunNumberBasedIO()) {
         for (j=0;j<nTree;j++) {
            if (fROMETrees[j]->isRead()) {
               fROMETrees[j]->GetTree()->Delete("");
               fRootFiles[j]->Close();
               fROMETrees[j]->SetTree(new TTree());
            }
            SafeDelete(fRootFiles[j]);
         }
         SafeDeleteArray(fRootFiles);
      }
      // delete sequential number array
      for (j=0;j<nTree;j++) {
         SafeDeleteArray(fTreePositionArray[j]);
      }
   }
   return true;
}

Bool_t ROMERomeDAQ::Terminate() {
   if (gROME->isOffline()) {
   }
   return true;
}



