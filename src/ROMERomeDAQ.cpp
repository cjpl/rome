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

#include <TBranchElement.h>
#include "ROMEAnalyzer.h"
#include "ROMERomeDAQ.h"
#include "ROMETreeInfo.h"

ClassImp(ROMERomeDAQ)

ROMERomeDAQ::ROMERomeDAQ() {
   fTreeInfo = new ROMETreeInfo();
   fTreeIndex = 0;
   fInputFileNameIndex = -1;
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
            fRootFiles[i] = new TFile(filename.Data(),"READ");
            if (fRootFiles[i]->IsZombie()) {
               ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
               return false;
            }
         }
      }
   }
   return true;
}

Bool_t ROMERomeDAQ::BeginOfRun() {
   const Int_t nTree = gROME->GetTreeObjectEntries();
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
   Int_t nKey;
   if (gROME->isOffline()) {
      this->SetRunning();
      // Read Trees
      int i,j,k;
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
      fTreePosition = new Long64_t[nTree];
      fTreeNextSeqNumber = new Long64_t[nTree];
      for (j=0;j<nTree;j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            treeRead = true;
            if (gROME->IsRunNumberBasedIO()) {
               filename.SetFormatted("%s%s%s.root",gROME->GetInputDir(),romeTree->GetName(),runNumberString.Data());
               fRootFiles[j] = new TFile(filename.Data(),"READ");
               if (fRootFiles[j]->IsZombie()) {
                  ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
                  return false;
               }
               ROMEPrint::Print("Reading %s\n", filename.Data());
               tree->Read(romeTree->GetName());
            }
            else if (gROME->IsFileNameBasedIO()) {
               if (fTreeIndex>0) {
                  if (fTreeIndex>=fRootFiles[fInputFileNameIndex]->GetNkeys())
                     fTreeIndex = 0;
                  else {
                     treename.SetFormatted("%s_%d",fCurrentTreeName.Data(),fTreeIndex);
                     fRootFiles[fInputFileNameIndex]->cd();
                     tree->Read(treename.Data());
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

      // Get sequential number
      for (j=0;j<nTree;j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            if (tree->GetEntriesFast()>0) {
               tree->GetBranch("Info")->GetEntry(0);
               fTreeNextSeqNumber[j] = fTreeInfo->GetSequentialNumber();
               gROME->SetCurrentRunNumber(fTreeInfo->GetRunNumber());
            }
            else {
               fTreeNextSeqNumber[j] = -1;
            }
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
      bool endfound = true;
      const Int_t nTree = gROME->GetTreeObjectEntries();
      // read event
      for (j=0;j<nTree;j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            while (fTreeNextSeqNumber[j]>0 && fTreeNextSeqNumber[j]<event) {
               fTreePosition[j]++;
               if (tree->GetEntriesFast()>fTreePosition[j]) {
                  tree->GetBranch("Info")->GetEntry(fTreePosition[j]);
                  fTreeNextSeqNumber[j] = fTreeInfo->GetSequentialNumber();
               }
               else {
                  fTreeNextSeqNumber[j] = -1;
               }
            }
            if (fTreeNextSeqNumber[j]==event) {
               found = true;
               if (tree->GetEntriesFast()>fTreePosition[j]+1) {
                  tree->GetBranch("Info")->GetEntry(fTreePosition[j]+1);
                  fTreeNextSeqNumber[j] = fTreeInfo->GetSequentialNumber();
               }
               else {
                  fTreeNextSeqNumber[j] = -1;
               }
               tree->GetBranch("Info")->GetEntry(fTreePosition[j]);
               gROME->SetCurrentEventNumber(fTreeInfo->GetEventNumber());
               if (gROME->IsRunNumberBasedIO())
                  tree->SetDirectory(fRootFiles[j]);
               else if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())
                  tree->SetDirectory(fRootFiles[fInputFileNameIndex]);
               tree->GetEntry(fTreePosition[j]);
               fTreePosition[j]++;
            }
         }
      }
      for (j=0;j<nTree;j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead())
            if (fTreeNextSeqNumber[j] != -1)
               endfound = false;
      }
      if (endfound) {
         this->SetEndOfRun();
         return true;
      }
      if (!found) {
         this->SetContinue();
         return true;
      }
   }

   return true;
}

Bool_t ROMERomeDAQ::EndOfRun() {
   if (gROME->isOffline()) {
      if (gROME->IsRunNumberBasedIO()) {
         const Int_t nTree = gROME->GetTreeObjectEntries();
         for (int j=0;j<nTree;j++) {
            if (gROME->GetTreeObjectAt(j)->isRead()) {
               gROME->GetTreeObjectAt(j)->GetTree()->Delete("");
               fRootFiles[j]->Close();
               gROME->GetTreeObjectAt(j)->SetTree(new TTree());
            }
         }
         delete [] fRootFiles;
      }
   }
   return true;
}

Bool_t ROMERomeDAQ::Terminate() {
   if (gROME->isOffline()) {
   }
   return true;
}



