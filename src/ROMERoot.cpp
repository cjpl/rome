// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMERoot
//
//  Interface to the Root Output of ROME.
//
//  $Log$
//  Revision 1.2  2005/01/27 16:21:06  schneebeli_m
//  print method & no gROME in path
//
//  Revision 1.1  2005/01/24 15:25:09  schneebeli_m
//  Seperated DAQ classes
//
//
//////////////////////////////////////////////////////////////////////////

#include <ROMERoot.h>

ROMERoot::ROMERoot() {
   fTreeInfo = new ROMETreeInfo();
}

bool ROMERoot::Initialize() {
   if (gROME->isOnline()) {
      gROME->Println("Root mode is not supported for online analysis.\n");
      return false;
   }
   if (gROME->isOffline()) {
      gROME->Println("Program is running offline.\n");
   }
   return true;
}
bool ROMERoot::Connect() {
   if (gROME->isOffline()) {
      // Read Trees
      int j;
      ROMEString filename;
      fRootFiles = new TFile*[gROME->GetTreeObjectEntries()];
      TTree *tree;
      ROMETree *romeTree;
      ROMEString runNumberString;
      gROME->GetCurrentRunNumberString(runNumberString);
      bool treeRead = false;
      fTreePosition = new int[gROME->GetTreeObjectEntries()];
      fTreeNextSeqNumber = new int[gROME->GetTreeObjectEntries()];
      for (j=0;j<gROME->GetTreeObjectEntries();j++) {
         romeTree = gROME->GetTreeObjectAt(j);
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            treeRead = true;
            filename.SetFormatted("%s%s%s.root",gROME->GetInputDir(),tree->GetName(),runNumberString.Data());
            fRootFiles[j] = new TFile(filename.Data(),"READ");
            if (fRootFiles[j]->IsZombie()) {
               gROME->Print("Inputfile '");
               gROME->Print(filename.Data());
               gROME->Println("' not found.");
               return false;
            }
            tree = (TTree*)fRootFiles[j]->FindObjectAny(tree->GetName());
            romeTree->SetTree(tree);
            fTreePosition[j] = 0;
         }
      }
      if (!treeRead) {
         gROME->Println("No input root file specified for running in root mode.\n");
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
            }
            else {
               fTreeNextSeqNumber[j] = -1;
            }
         }
      }
   }
   return true;
}
bool ROMERoot::ReadEvent(int event) {
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
               tree->GetEntry(fTreePosition[j]);
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
bool ROMERoot::Disconnect() {
   if (gROME->isOffline()) {
      for (int j=0;j<gROME->GetTreeObjectEntries();j++) {
         if (gROME->GetTreeObjectAt(j)->isRead()) fRootFiles[j]->Close();
      }
      delete [] fRootFiles;
   }
   return true;
}
bool ROMERoot::Termination() {
   if (gROME->isOffline()) {
   }
   return true;
}

