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
#endif                          // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TROOT.h>
#include <TBranchElement.h>
#include <TMath.h>
#include <TObjString.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif                          // R__VISUAL_CPLUSPLUS
#include "ROMEAnalyzer.h"
#include "ROMERomeDAQ.h"
#include "ROMETree.h"
#include "ROMETreeInfo.h"
#include "ROMEODBOfflineDataBase.h"

ClassImp(ROMERomeDAQ)

//______________________________________________________________________________
ROMERomeDAQ::ROMERomeDAQ()
:ROMEDAQSystem()
,fRootFiles(0)
,fROMETrees(0)
,fTreeIndex(0)
,fInputFileNameIndex(-1)
,fCurrentTreeName("")
,fMaxEventNumber(0)
,fTreeInfo(new ROMETreeInfo())
,fTreePositionLookup(0)
,fTreeNEntries(0)
,fTimeStamp(0)
{
}

//______________________________________________________________________________
ROMERomeDAQ::~ROMERomeDAQ()
{
   SafeDelete(fTreeInfo);
   SafeDeleteArray(fTreePositionLookup);
   SafeDeleteArray(fTreeNEntries);
   if (fROMETrees) {
      fROMETrees->Delete();
   }
   SafeDelete(fROMETrees);
}

//______________________________________________________________________________
Bool_t ROMERomeDAQ::Init()
{
   if (gROME->isOnline()) {
      ROMEPrint::Error("Rome mode is not supported for online analysis.\n");
      return false;
   }
   if (gROME->isOffline()) {
      int i;
      ROMEString filename;
      const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
      ROMEPrint::Print("Program is running offline.\n");
      if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
         fRootFiles = new TObjArray(nInputFile);
         for (i = 0; i < nInputFile; i++) {
            gROME->ConstructFilePath(gROME->GetRawInputDirString(), gROME->GetInputFileNameAt(i), filename);
            fRootFiles->AddAt(new TFile (filename.Data(), "READ"), i);
            gROOT->cd();
            if (static_cast<TFile*>(fRootFiles->At(i))->IsZombie()) {
               ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
               return false;
            }
         }
      }
   }

   const Int_t nTree = gROME->GetTreeObjectEntries();
   fTreePositionLookup = new Long64_t*[nTree];
   fTreeNEntries = new Long64_t[nTree];

   Int_t j;
   fROMETrees = new TObjArray(nTree);
   for (j = 0; j < nTree; j++) {
      fROMETrees->AddAt(new ROMETree(new TTree()), j);
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMERomeDAQ::BeginOfRun()
{
   const Int_t nTree      = gROME->GetTreeObjectEntries();
   const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
   Int_t       run        = static_cast<Int_t>(gROME->GetCurrentRunNumber());

   int i, j, k;
   ROMETree *romeTree;
   for (j = 0; j < nTree; j++) {
      romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
      romeTree->SetFileName(gROME->GetTreeObjectAt(j)->GetFileName());
      romeTree->SetConfigInputFileName(gROME->GetTreeObjectAt(j)->GetConfigInputFileName());
      romeTree->SetRead(gROME->GetTreeObjectAt(j)->isRead());
      romeTree->SetName(gROME->GetTreeObjectAt(j)->GetName());
      romeTree->AllocateBranchRead(gROME->GetTreeObjectAt(j)->GetNBranchRead());
      memcpy(romeTree->GetBranchRead(),
             gROME->GetTreeObjectAt(j)->GetBranchRead(),
             gROME->GetTreeObjectAt(j)->GetNBranchRead() * sizeof(Bool_t));
   }

   Int_t nKey;
   if (gROME->isOffline()) {
      this->SetRunning();
      // Read Trees
      ROMEString filename;
      ROMEString treename;
      if (gROME->IsRunNumberBasedIO()) {
         fRootFiles = new TObjArray(nTree);
      }
      TTree *tree;
      ROMEString runNumberString;
      if ((gROME->IsRunNumberBasedIO() || gROME->IsRunNumberAndFileNameBasedIO())) {
         gROME->GetCurrentRunNumberString(runNumberString);
      }
      bool treeRead = false;
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            treeRead = true;

            if (gROME->IsRunNumberBasedIO()) {
               if (romeTree->GetConfigInputFileName().Length()) {
                  gROME->ConstructFilePath(gROME->GetRawInputDirString(), romeTree->GetConfigInputFileName(), filename);
               } else {
                  ROMEString tmpfilename;
                  tmpfilename.SetFormatted("%s%s.root", romeTree->GetName(), runNumberString.Data());
                  gROME->ConstructFilePath(gROME->GetRawInputDirString(), tmpfilename, filename);
               }
               gSystem->ExpandPathName(filename);
               fRootFiles->AddAt(new TFile (filename.Data(), "READ"), j);
               if (static_cast<TFile*>(fRootFiles->At(j))->IsZombie()) {
                  ROMEPrint::Warning("Inputfile '%s' not found.\n", filename.Data());
                  return false;
               }
               ROMEPrint::Print("Reading %s\n", filename.Data());
               if (!tree->Read(romeTree->GetName())) {
                  return false;
               }
               romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(j)));
               gROOT->cd();
            } else if (gROME->IsFileNameBasedIO()) {
               if (fTreeIndex > 0) {
                  if (fTreeIndex >= static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->GetNkeys()) {
                     fTreeIndex = 0;
                  } else {
                     treename.SetFormatted("%s_%d", fCurrentTreeName.Data(), fTreeIndex);
                     static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->cd();
                     if (!tree->Read(treename.Data())) {
                        return false;
                     }
                     romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex)));
                     fTreeIndex++;
                     tree->SetName(fCurrentTreeName.Data());
                  }
               }
               if (fTreeIndex == 0) {
                  fInputFileNameIndex++;
                  if (nInputFile <= fInputFileNameIndex) {
                     this->SetTerminate();
                     return true;
                  }
                  gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(fInputFileNameIndex).Data());
                  static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->cd();
                  ROMEPrint::Print("Reading %s\n",
                                   gROME->ConstructFilePath(gROME->GetRawInputDirString(),
                                                            gROME->GetCurrentInputFileName(), filename).Data());
                  if (!tree->Read(romeTree->GetName())) {
                     return false;
                  }
                  romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex)));
                  fCurrentTreeName = romeTree->GetName();
                  fTreeIndex++;
               }
            } else if (gROME->IsRunNumberAndFileNameBasedIO()) {
               fInputFileNameIndex = -1;
               fCurrentTreeName = romeTree->GetName();
               for (i = 0; i < nInputFile; i++) {
                  nKey = static_cast<TFile*>(fRootFiles->At(i))->GetNkeys();
                  for (k = 0; k < nKey; k++) {
                     treename.SetFormatted("%s", fCurrentTreeName.Data());
                     if (k > 0) {
                        treename.AppendFormatted("_%d", k);
                     }
                     static_cast<TFile*>(fRootFiles->At(i))->cd();
                     if (!tree->Read(treename.Data())) {
                        return false;
                     }
                     romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(i)));
                     static_cast<TBranchElement*>(tree->FindBranch("Info"))->SetAddress(&fTreeInfo);
                     tree->GetBranch("Info")->GetEntry(0);
                     if (fTreeInfo->GetRunNumber() == gROME->GetCurrentRunNumber()) {
                        gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(i));
                        ROMEPrint::Print("Reading %s\n",
                                         gROME->ConstructFilePath(gROME->GetRawInputDirString(),
                                                                  gROME->GetCurrentInputFileName(), filename).Data());
                        tree->SetName(fCurrentTreeName.Data());
                        fInputFileNameIndex = i;
                        break;
                     }
                  }
               }
               if (fInputFileNameIndex == -1) {
                  ROMEPrint::Warning("Run " R_LLD " not found in the specified input files !\n",
                                     gROME->GetCurrentRunNumber());
                  ROMEPrint::Warning("Skipping run " R_LLD ".\n", gROME->GetCurrentRunNumber());
                  this->SetEndOfRun();
                  return true;
               }
            }
            romeTree->SetTree(tree);
            fTreeNEntries[j] = tree->GetEntries();
         } else {
            fRootFiles->AddAt(0, j);
         }
      }
      if (!treeRead) {
         ROMEPrint::Error("No input tree specified for running in rome mode.\n");
         return false;
      }
      this->ConnectTrees();

      // Get run number
      if ((gROME->IsFileNameBasedIO())) {
         for (j = 0; j < nTree; j++) {
            romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
            tree = romeTree->GetTree();
            if (romeTree->isRead()) {
               if (tree->GetEntriesFast() > 0) {
                  tree->GetBranch("Info")->GetEntry(0);
                  gROME->SetCurrentRunNumber(fTreeInfo->GetRunNumber());
                  run = static_cast<Int_t>(gROME->GetCurrentRunNumber());
               }
            }
         }
      }

      // Prepare event number array
      Int_t iEvent;
      fMaxEventNumber = 0;
      fTreeInfo->SetEventNumber(0);

      // Check max event number
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            for (iEvent = 0; iEvent < fTreeNEntries[j]; iEvent++) {
               tree->GetBranch("Info")->GetEntry(iEvent);
               if (fMaxEventNumber < fTreeInfo->GetEventNumber()) {
                  fMaxEventNumber = fTreeInfo->GetEventNumber();
               }
            }
         }
      }
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            fTreePositionLookup[j] = new Long64_t[static_cast<Int_t>(fMaxEventNumber + 1)];    // Warning : potential loss of data (int)
            for (iEvent = 0; iEvent < fMaxEventNumber + 1; iEvent++) {
               fTreePositionLookup[j][iEvent] = -1;
            }
            for (iEvent = 0; iEvent < fTreeNEntries[j]; iEvent++) {
               tree->GetBranch("Info")->GetEntry(iEvent);
               if (fTreeInfo->GetRunNumber() == run) {
                  fTreePositionLookup[j][fTreeInfo->GetEventNumber()] = iEvent;
               }
            }
         } else {
            fTreePositionLookup[j] = 0;
         }
      }

      // read ODB (when there are multiple ODB, the first one is actually used)
      const char *odbbuffer = 0;
      TObjString *odbstr    = 0;
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         if (romeTree->isRead()) {
            odbstr = static_cast<TObjString*>(romeTree->GetFile()->Get("odb"));
            if (odbstr) {
               odbbuffer = odbstr->GetString().Data();
               break;
            }
         }
      }
      if (gROME->isDataBaseActive("ODB")) {
         static_cast<ROMEODBOfflineDataBase*>(gROME->GetDataBase("ODB"))->
               SetBuffer(odbbuffer);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMERomeDAQ::Event(Long64_t event) {
   if (gROME->isOffline()) {
      int       j;
      ROMETree *romeTree;
      TTree    *tree;
      bool      found = false;
      int       treePosition;

      if (event > fMaxEventNumber) {
         this->SetEndOfRun();
         return true;
      }

      gROME->SetEventID(kTriggerEventID);

      // read event
      const Int_t nTree = gROME->GetTreeObjectEntries();
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            treePosition = static_cast<Int_t>(fTreePositionLookup[j][event]);
            if (treePosition >= 0 && treePosition < fTreeNEntries[j]) {
               found = true;
               if (gROME->IsRunNumberBasedIO()) {
                  tree->SetDirectory(static_cast<TFile*>(fRootFiles->At(j)));
               } else if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
                  tree->SetDirectory(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex)));
               }
               tree->GetEntry(treePosition);
               gROME->SetCurrentEventNumber(fTreeInfo->GetEventNumber());
               fTimeStamp = fTreeInfo->GetTimeStamp();
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

//______________________________________________________________________________
Long64_t ROMERomeDAQ::Seek(Long64_t event)
{
   if (gROME->isOffline()) {
      if (event < 0 || event > fMaxEventNumber) {
         Warning("Seek", "Event number " R_LLD " was not found.", event);
         event = gROME->GetCurrentEventNumber();
         gROME->SetDontReadNextEvent();
         SetContinue();
      }
   }
   return event;
}

//______________________________________________________________________________
Bool_t ROMERomeDAQ::EndOfRun()
{
   if (gROME->isOffline()) {
      const Int_t nTree = gROME->GetTreeObjectEntries();
      int j;
      if (gROME->IsRunNumberBasedIO()) {
         for (j = 0; j < nTree; j++) {
            if (static_cast<ROMETree*>(fROMETrees->At(j))->isRead()) {
               static_cast<ROMETree*>(fROMETrees->At(j))->DeleteTree();
               static_cast<TFile*>(fRootFiles->At(j))->Close();
               static_cast<ROMETree*>(fROMETrees->At(j))->SetTree(new TTree());
            }
         }
         if (fRootFiles) {
            fRootFiles->Delete();
         }
         SafeDelete(fRootFiles);
      }
      // delete sequential number array
      for (j = 0; j < nTree; j++) {
         SafeDeleteArray(fTreePositionLookup[j]);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMERomeDAQ::Terminate()
{
   if (gROME->isOffline()) {
      const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
      Int_t j;
      if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
         for (j = 0; j < nInputFile; j++) {
            static_cast<ROMETree*>(fROMETrees->At(j))->DeleteTree();
            static_cast<TFile*>(fRootFiles->At(j))->Close();
         }
         if (fRootFiles) {
            fRootFiles->Delete();
         }
         SafeDeleteArray(fRootFiles);
      }
   }
   return true;
}
