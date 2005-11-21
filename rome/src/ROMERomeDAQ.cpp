// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMERomeDAQ
//
//  Interface to the output of ROME.
//
//  $Id: ROMERomeDAQ.cpp 680 2005-10-31 11:32:46Z ritt $
//
//////////////////////////////////////////////////////////////////////////

#include <TBranchElement.h>
#include <ROMERomeDAQ.h>

ROMERomeDAQ::ROMERomeDAQ() {
   fTreeInfo = new ROMETreeInfo();
   fTreeIndex = 0;
   fInputFileNameIndex = -1;
}

bool ROMERomeDAQ::Init() {
   if (gROME->isOnline()) {
      gROME->Println("Rome mode is not supported for online analysis.\n");
      return false;
   }
   if (gROME->isOffline()) {
      int i;
      ROMEString filename;
      gROME->Println("Program is running offline.\n");
      if ((gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
         fRootFiles = new TFile*[gROME->GetNumberOfInputFileNames()];
         for (i=0;i<gROME->GetNumberOfInputFileNames();i++) {
            filename.SetFormatted("%s%s",gROME->GetInputDir(),gROME->GetInputFileNameAt(i).Data());
            fRootFiles[i] = new TFile(filename.Data(),"READ");
            if (fRootFiles[i]->IsZombie()) {
               gROME->Print("Inputfile '");
               gROME->Print(filename.Data());
               gROME->Println("' not found.");
               return false;
            }
         }
      }
   }
   return true;
}
bool ROMERomeDAQ::BeginOfRun() {
   if (gROME->isOffline()) {
      this->SetRunning();
      // Read Trees
      int i,j,k;
      ROMEString filename;
      ROMEString treename;
      if (gROME->IsRunNumberBasedIO())
         fRootFiles = new TFile*[gROME->GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *romeTree;
      ROMEString runNumberString;
      if ((gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()))
         gROME->GetCurrentRunNumberString(runNumberString);
      bool treeRead = false;
      fTreePosition = new int[gROME->GetTreeObjectEntries()];
      fTreeNextSeqNumber = new int[gROME->GetTreeObjectEntries()];
      for (j=0;j<gROME->GetTreeObjectEntries();j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            treeRead = true;
            if (gROME->IsRunNumberBasedIO()) {
               filename.SetFormatted("%s%s%s.root",gROME->GetInputDir(),tree->GetName(),runNumberString.Data());
               fRootFiles[j] = new TFile(filename.Data(),"READ");
               if (fRootFiles[j]->IsZombie()) {
                  gROME->Print("Inputfile '");
                  gROME->Print(filename.Data());
                  gROME->Println("' not found.");
                  return false;
               }
               gROME->Print("Reading ");
               gROME->Print(filename.Data());
               tree->Read(tree->GetName());
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
                     tree->SetDirectory(romeTree->GetFile());
                  }
               }
               if (fTreeIndex==0) {
                  fInputFileNameIndex++;
                  if (gROME->GetNumberOfInputFileNames()<=fInputFileNameIndex) {
                     this->SetTerminate();
                     return true;
                  }
                  gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(fInputFileNameIndex).Data());
                  fRootFiles[fInputFileNameIndex]->cd();
                  gROME->Print("Reading ");
                  gROME->Print(gROME->GetInputDir());
                  gROME->Println(gROME->GetCurrentInputFileName().Data());
                  tree->Read(tree->GetName());
                  fCurrentTreeName = tree->GetName();
                  fTreeIndex++;
               }
            }
            else if (gROME->IsRunNumberAndFileNameBasedIO()) {
               fInputFileNameIndex = -1;
               fCurrentTreeName = tree->GetName();
               for (i=0;i<gROME->GetNumberOfInputFileNames();i++) {
                  for (k=0;k<fRootFiles[i]->GetNkeys();k++) {
                     treename.SetFormatted("%s",fCurrentTreeName.Data());
                     if (k>0)
                        treename.AppendFormatted("_%d",k);
                     fRootFiles[i]->cd();
                     tree->Read(treename.Data());
                     ((TBranchElement*)tree->FindBranch("Info"))->SetAddress(&fTreeInfo);
                     tree->GetBranch("Info")->GetEntry(0);
                     if (fTreeInfo->GetRunNumber()==gROME->GetCurrentRunNumber()) {
                        gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(i));
                        gROME->Print("Reading ");
                        gROME->Print(gROME->GetInputDir());
                        gROME->Println(gROME->GetCurrentInputFileName().Data());
                        tree->SetName(fCurrentTreeName.Data());
                        fInputFileNameIndex = i;
                        break;
                     }
                  }
               }
               if (fInputFileNameIndex==-1) {
                  ROMEString buf;
                  buf.SetFormatted("Run %d not found in the specified input files !",gROME->GetCurrentRunNumber());
                  gROME->Println(buf);
                  buf.SetFormatted("Skipping run %d.",gROME->GetCurrentRunNumber());
                  gROME->Println(buf);
                  this->SetEndOfRun();
                  return true;
               }
            }
            romeTree->SetTree(tree);
            fTreePosition[j] = 0;
         }
      }
      if (!treeRead) {
         gROME->Println("No input tree specified for running in rome mode.\n");
         return false;
      }
      this->ConnectTrees();

      // Get sequential number
      for (j=0;j<gROME->GetTreeObjectEntries();j++) {
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
bool ROMERomeDAQ::Event(int event) {
   if (gROME->isOffline()) {
      ROMETree *romeTree;
      TTree *tree;
      bool found = false;
      // read event
      for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            if (fTreeNextSeqNumber[j]==event) {
               found = true;
               if (tree->GetEntriesFast()>fTreePosition[j]+1) {
                  tree->GetBranch("Info")->GetEntry(fTreePosition[j]+1);
                  fTreeNextSeqNumber[j] = fTreeInfo->GetSequentialNumber();
               }
               else {
                  fTreeNextSeqNumber[j] = -1;
               }
               gROME->SetCurrentEventNumber(fTreeInfo->GetEventNumber());
               if (gROME->IsRunNumberBasedIO())
                  tree->SetDirectory(fRootFiles[j]);
               else if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())
                  tree->SetDirectory(fRootFiles[fInputFileNameIndex]);
               tree->GetEntry(fTreePosition[j]);
               tree->SetDirectory(romeTree->GetFile());
               fTreePosition[j]++;
            }
         }
      }
      if (!found) {
         this->SetEndOfRun();
         return true;
      }
   }

   return true;
}
bool ROMERomeDAQ::EndOfRun() {
   if (gROME->isOffline()) {
      if (gROME->IsRunNumberBasedIO()) {
         for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
            if (gROME->GetTreeObjectAt(j)->isRead()) fRootFiles[j]->Close();
         }
         delete [] fRootFiles;
      }
   }
   return true;
}
bool ROMERomeDAQ::Terminate() {
   if (gROME->isOffline()) {
   }
   return true;
}



