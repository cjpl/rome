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

using namespace std;

ClassImp(ROMERomeDAQ)

//______________________________________________________________________________
ROMERomeDAQ::ROMERomeDAQ()
:ROMEDAQSystem()
,fRootFiles(0)
,fROMETrees(0)
,fTreeIndex(0)
,fInputFileNameIndex(-1)
,fCurrentTreeName("")
,fMaxEventNumber(-1)
,fTreeInfo(new ROMETreeInfo())
,fSkipReadTree(0)
,fCurrentTreePosition(0)
,fTreePositionLookup()
,fTreePositionMap()
,fTreeNEntries(0)
,fTimeStamp(0)
{
}

//______________________________________________________________________________
ROMERomeDAQ::~ROMERomeDAQ()
{
   SafeDelete(fTreeInfo);
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
   Int_t j;

   fTreePositionLookup.resize(nTree);
   fTreePositionMap.resize(nTree);
   fSkipReadTree.Set(nTree);
   fSkipReadTree.Reset();
   fCurrentTreePosition.Set(nTree);
   fCurrentTreePosition.Reset();
   fTreeNEntries = new Long64_t[nTree];

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
   vector<map<Long64_t, Long64_t> > treePositionMap(nTree); // key:event#, value:position
   vector<Bool_t> skipBuildPositionLookup(nTree, kFALSE);

   int i, j, k;
   ROMETree *romeTree;
   for (j = 0; j < nTree; j++) {
      romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
      romeTree->SetFileName(gROME->GetTreeObjectAt(j)->GetFileName());
      romeTree->SetConfigInputFileName(gROME->GetTreeObjectAt(j)->GetConfigInputFileName());
      romeTree->SetRead(gROME->GetTreeObjectAt(j)->isRead());
      romeTree->SetName(gROME->GetTreeObjectAt(j)->GetName());
      romeTree->SetCacheSize(gROME->GetTreeObjectAt(j)->GetCacheSize());
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
               if (!fSkipReadTree[j]) {
                  ROMEPrint::Print("Reading %s\n", filename.Data());
                  if (!tree->Read(romeTree->GetName())) {
                     return false;
                  }
               } else {
                  fSkipReadTree[j] = 0;
                  skipBuildPositionLookup[j] = kTRUE;
               }
               romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(j)));
               romeTree->SetFileName(static_cast<TFile*>(fRootFiles->At(j))->GetName());
               gROOT->cd();
            } else if (gROME->IsFileNameBasedIO()) {
               if (fTreeIndex > 0) {
                  if (fTreeIndex >= static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->GetNkeys()) {
                     fTreeIndex = 0;
                  } else {
                     treename.SetFormatted("%s_%d", fCurrentTreeName.Data(), fTreeIndex);
                     static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->cd();
                     if (static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->GetKey(treename)) {
                        if (!fSkipReadTree[j]) {
                           if (!tree->Read(treename.Data())) {
                              return false;
                           }
                        } else {
                           fSkipReadTree[j] = 0;
                           skipBuildPositionLookup[j] = kTRUE;
                        }
                        romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex)));
                        romeTree->SetFileName(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->GetName());
                        fTreeIndex++;
                        tree->SetName(fCurrentTreeName.Data());
                     } else {
                        fTreeIndex = 0;
                     }
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
                  gROME->ConstructFilePath(gROME->GetRawInputDirString(),
                                           gROME->GetCurrentInputFileName(), filename);
                  if (!fSkipReadTree[j]) {
                     ROMEPrint::Print("Reading %s\n", filename.Data());
                     if (!tree->Read(romeTree->GetName())) {
                        return false;
                     }
                  } else {
                     fSkipReadTree[j] = 0;
                     skipBuildPositionLookup[j] = kTRUE;
                  }
                  romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex)));
                  romeTree->SetFileName(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex))->GetName());
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
                     if (static_cast<TFile*>(fRootFiles->At(i))->GetKey(treename)) {
                        gROME->ConstructFilePath(gROME->GetRawInputDirString(),
                                                 gROME->GetInputFileNameAt(i), filename);
                        if (!fSkipReadTree[j]) {
                           ROMEPrint::Print("Reading %s\n", filename.Data());
                           if (!tree->Read(treename.Data())) {
                              return false;
                           }
                        } else {
                           skipBuildPositionLookup[j] = kTRUE;
                        }
                        romeTree->SetFile(static_cast<TFile*>(fRootFiles->At(i)));
                        romeTree->SetFileName(static_cast<TFile*>(fRootFiles->At(i))->GetName());
                        static_cast<TBranchElement*>(tree->FindBranch("Info"))->SetAddress(&fTreeInfo);
//                        tree->GetBranch("Info")->GetEntry(0);
//                        if (fTreeInfo->GetRunNumber() == gROME->GetCurrentRunNumber()) {
                        gROME->SetCurrentInputFileName(gROME->GetInputFileNameAt(i));
                        tree->SetName(fCurrentTreeName.Data());
                        fInputFileNameIndex = i;
                        break;
//                        }
                     }
                  }
               }
               fSkipReadTree[j] = 1;
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
            if (gROME->IsRunNumberBasedIO()) {
               // doesn't cuase memory leak ?
               fRootFiles->AddAt(0, j);
            }
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
                  tree->GetBranch("Info")->GetEntry(fCurrentTreePosition[j]);
                  gROME->SetCurrentRunNumber(fTreeInfo->GetRunNumber());
                  run = static_cast<Int_t>(gROME->GetCurrentRunNumber());
               }
            }
         }
      }

      // Prepare event number array
      Int_t iEvent;
      fTreeInfo->SetEventNumber(0);

      // Check max event number
      Bool_t eventFound = kFALSE;
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         tree = romeTree->GetTree();
         treePositionMap[j].clear();
         if (romeTree->isRead()) {
            if (!skipBuildPositionLookup[j]) {
#if 0 /* this cause crash with ROOT v5.32.03 */
               Long64_t autoFlush = tree->GetAutoFlush(); 
               tree->SetAutoFlush(0); 
#endif
               for (iEvent = 0; iEvent < fTreeNEntries[j]; iEvent++) {
                  tree->GetBranch("Info")->GetEntry(iEvent);
                  fTreePositionMap[j][pair<Long64_t, Long64_t>(fTreeInfo->GetRunNumber(), fTreeInfo->GetEventNumber())] =
                     iEvent;
                  if (fTreeInfo->GetRunNumber() == run) {
                     treePositionMap[j][fTreeInfo->GetEventNumber()] = iEvent;
                     if (!eventFound) {
                        eventFound = kTRUE;
                     }
                  }
               }
#if 0 /* this cause crash with ROOT v5.32.03 */
               tree->SetAutoFlush(autoFlush); 
#endif
            } else {
               std::map<std::pair<Long64_t, Long64_t>, Long64_t>::iterator posmap;
               for (posmap = fTreePositionMap[j].begin();
                    posmap != fTreePositionMap[j].end();
                    ++posmap) {
                  if (posmap->first.first == run) {
                     treePositionMap[j][posmap->first.second] = posmap->second;
                     if (!eventFound) {
                        eventFound = kTRUE;
                     }
                  }
               }
            }
         }
      }

      map<Long64_t, Long64_t>::iterator treePosition;
      fMaxEventNumber = -1;
      for (j = 0; j < nTree; j++) {
         for (treePosition = treePositionMap[j].begin();
              treePosition != treePositionMap[j].end();
              ++treePosition) {
            if (fMaxEventNumber < treePosition->first) {
               fMaxEventNumber = treePosition->first;
            }
         }
      }
      for (j = 0; j < nTree; j++) {
         fTreePositionLookup[j].resize(fMaxEventNumber + 1, -1);
         fill(fTreePositionLookup[j].begin(), fTreePositionLookup[j].end(), -1);
         for (treePosition = treePositionMap[j].begin();
              treePosition != treePositionMap[j].end();
              ++treePosition) {
            fTreePositionLookup[j][treePosition->first] = treePosition->second;
         }
      }

      if (!eventFound) {
         this->SetEndOfRun();
         return kTRUE;
      }
      this->SetAnalyze();

      // read ODB (when there are multiple ODB, the first one is actually used)
      const char *odbbuffer = 0;
      TObjString *odbstr    = 0;
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         if (romeTree->isRead()) {
            odbstr = static_cast<TObjString*>(romeTree->GetFile()->Get("odb"));
            if (odbstr) {
               odbbuffer = odbstr->String().Data();
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
Bool_t ROMERomeDAQ::Event(Long64_t event)
{
   if (gROME->isOffline()) {
      int       j;
      ROMETree *romeTree;
      TTree    *tree;
      bool      found = false;
      int       treePosition;

      gROME->SetEventID(kTriggerEventID);

      // read event
      const Int_t nTree = gROME->GetTreeObjectEntries();
      for (j = 0; j < nTree; j++) {
         romeTree = static_cast<ROMETree*>(fROMETrees->At(j));
         tree = romeTree->GetTree();
         if (romeTree->isRead()) {
            if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
               treePosition = fCurrentTreePosition[j];
               if (treePosition >= fTreeNEntries[j]) {
                  // next file
                  fCurrentTreePosition.Reset();
                  this->SetEndOfRun();
                  return true;
               }
            } else {
               if (event < static_cast<Long64_t>(fTreePositionLookup[j].size()) &&
				   (treePosition = fTreePositionLookup[j][event]) != -1) {
                  fCurrentTreePosition[j] = treePosition;
               } else {
                  treePosition = -1;
               }
            }
            if (treePosition >= 0 && treePosition < fTreeNEntries[j]) {
               found = true;
               if (gROME->IsRunNumberBasedIO()) {
                  tree->SetDirectory(static_cast<TFile*>(fRootFiles->At(j)));
               } else if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
                  tree->SetDirectory(static_cast<TFile*>(fRootFiles->At(fInputFileNameIndex)));
               }
               tree->GetEntry(treePosition);
               UpdateVariableSize(j);
               gROME->SetCurrentEventNumber((event = fTreeInfo->GetEventNumber()));
               fTimeStamp = fTreeInfo->GetTimeStamp();
               if (fTreeInfo->GetRunNumber() != gROME->GetCurrentRunNumber()) {
                  // read this file again.
                  fTreeIndex--;
                  fInputFileNameIndex--;
                  fSkipReadTree[j] = 1;
                  this->SetEndOfRun();
                  return true;
               }
            }
            if (event > fMaxEventNumber) {
               this->SetEndOfRun();
               return true;
            }
            fCurrentTreePosition[j]++;
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
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMERomeDAQ::Terminate()
{
   if (gROME->isOffline()) {
      const Int_t nTree      = gROME->GetTreeObjectEntries();
      const Int_t nInputFile = gROME->GetNumberOfInputFileNames();
      Int_t j;
      if (gROME->IsFileNameBasedIO() || gROME->IsRunNumberAndFileNameBasedIO()) {
         for (j = 0; j < nTree; j++) {
            static_cast<ROMETree*>(fROMETrees->At(j))->DeleteTree();
         }
         for (j = 0; j < nInputFile; j++) {
            static_cast<TFile*>(fRootFiles->At(j))->Close();
         }
         if (fRootFiles) {
            fRootFiles->Delete();
         }
         SafeDelete(fRootFiles);
      }
   }
   return true;
}
