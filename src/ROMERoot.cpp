// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMERoot
//
//  Interface to the Root Output of ROME.
//
//  $Log$
//  Revision 1.6  2005/08/15 09:48:46  schneebeli_m
//  improved input file based IO
//
//  Revision 1.5  2005/08/12 15:37:02  schneebeli_m
//  added input file based IO
//
//  Revision 1.4  2005/07/12 06:42:23  sawada
//  Bug fix. Matched the name of method (IsActiveID and IsActiveEventID)
//
//  Revision 1.3  2005/06/13 15:49:04  schneebeli_m
//  changed name of DAQ user functions
//
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
   fTreeIndex = 0;
   fInputFileNameIndex = -1;
}

bool ROMERoot::Init() {
   if (gROME->isOnline()) {
      gROME->Println("Root mode is not supported for online analysis.\n");
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
bool ROMERoot::BeginOfRun() {
   if (gROME->isOffline()) {
      this->SetRunning();
      // Read Trees
      int j;
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
            if (fTreeIndex>0 && (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
               treename.SetFormatted("%s_%d",tree->GetName(),fTreeIndex);
               fRootFiles[fInputFileNameIndex]->cd();
               tree->Read(treename.Data());
               if (tree==NULL)
                  fTreeIndex = 0;
               else {
                  fTreeIndex++;
                  tree->SetName(romeTree->GetTree()->GetName());
                  tree->SetDirectory(romeTree->GetFile());
               }
            }
            if (gROME->IsRunNumberBasedIO()) {
               filename.SetFormatted("%s%s%s.root",gROME->GetInputDir(),tree->GetName(),runNumberString.Data());
               fRootFiles[j] = new TFile(filename.Data(),"READ");
               if (fRootFiles[j]->IsZombie()) {
                  gROME->Print("Inputfile '");
                  gROME->Print(filename.Data());
                  gROME->Println("' not found.");
                  return false;
               }
               tree->Read(tree->GetName());
            }
            else if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
               if (fTreeIndex==0) {
                  fInputFileNameIndex++;
                  if (gROME->GetNumberOfInputFileNames()<=fInputFileNameIndex) {
                     this->SetTerminate();
                     return true;
                  }
                  gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(fInputFileNameIndex));
                  fRootFiles[fInputFileNameIndex]->cd();
                  tree->Read(tree->GetName());
               }
               fTreeIndex++;
            }
            romeTree->SetTree(tree);
            fTreePosition[j] = 0;
         }
      }
      if (!treeRead) {
         gROME->Println("No input root file specified for running in root mode.\n");
         return false;
      }
      this->ConnectTrees();
   TFile* outputFile = new TFile("xxx.root","RECREATE");
   gROME->GetTreeObjectAt(0)->GetTree()->Write(0,TObject::kOverwrite);
   outputFile->Close();
   outputFile->Delete();

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
bool ROMERoot::Event(int event) {
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
bool ROMERoot::EndOfRun() {
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
bool ROMERoot::Terminate() {
   if (gROME->isOffline()) {
   }
   return true;
}



