// $Id$
// Author: Ryu Sawada

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusWindow                                                          //
//                                                                      //
// Base class for the main window of Argus projects                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 4244)
#endif                          // R__VISUAL_CPLUSPLUS
#include <TColor.h>
#include <TG3DLine.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGListTree.h>
#include <TGMenu.h>
#include <TGProgressBar.h>
#include <TGStatusBar.h>
#include <TGTab.h>
#include <TObjArray.h>
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif                          // R__VISUAL_CPLUSPLUS
#include "ROMEUtilities.h"
#include "ROMEString.h"
#include "ArgusWindow.h"
#include "ArgusTab.h"
#include "ArgusAnalyzerController.h"
#include "ArgusTextDialog.h"
#include "ROMECompositeFrame.h"
#include "ROMEAnalyzer.h"

ClassImp(ArgusWindow)

//______________________________________________________________________________
ArgusWindow::ArgusWindow(Bool_t statusBarSwitch, Int_t numberOfTabs)
:TGMainFrame()
,fWindowId(-1)
,fSubWindows(new TObjArray(0))
,fSubWindowRunning(new TArrayI())
,fSubWindowTimeString(new ROMEStrArray())
,fTabWindow(kTRUE)
,fWatchAll()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fArgusActive(kFALSE)
,fStatusBar(0)
,fStatusBarSwitch(statusBarSwitch)
,fProgress(0)
,fInfoFrame(0)
,fMenuBar(0)
,fMenuFile(0)
,fRunEventNumber(0)
,fListTreeView(kFALSE)
,fListTree(0)
,fListTreeItem(new TGListTreeItem*[numberOfTabs])
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(0)
,fTabObjects(new TObjArray(numberOfTabs))
,fParentIndex(new Int_t[numberOfTabs])
,fNumberOfChildren(new Int_t[numberOfTabs])
,fTGTab(new TObjArray(numberOfTabs))
,fControllerActive(kFALSE)
,fController(0)
,fControllerNetFolder(0)
,fWindowScale(1)
,fRequestEventHandling(kFALSE)
,fForceEventHandling(kFALSE)
{
   fWatchAll.Reset();
}

//______________________________________________________________________________
ArgusWindow::ArgusWindow(const TGWindow* p, Bool_t statusBarSwitch, Int_t numberOfTabs, Bool_t tabWindow)
:TGMainFrame(p, 0, 0)
,fWindowId(-1)
,fSubWindows(new TObjArray(0))
,fSubWindowRunning(new TArrayI())
,fSubWindowTimeString(new ROMEStrArray())
,fTabWindow(tabWindow)
,fWatchAll()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fArgusActive(kFALSE)
,fStatusBar(0)
,fStatusBarSwitch(statusBarSwitch)
,fProgress(0)
,fInfoFrame(0)
,fMenuBar(0)
,fMenuFile(0)
,fRunEventNumber(0)
,fListTreeView(kFALSE)
,fListTree(0)
,fListTreeItem(new TGListTreeItem*[numberOfTabs])
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(0)
,fTabObjects(new TObjArray(numberOfTabs))
,fParentIndex(new Int_t[numberOfTabs])
,fNumberOfChildren(new Int_t[numberOfTabs])
,fTGTab(new TObjArray(numberOfTabs))
,fControllerActive(kFALSE)
,fController(0)
,fControllerNetFolder(0)
,fWindowScale(1)
,fRequestEventHandling(kFALSE)
,fForceEventHandling(kFALSE)
{
   fWatchAll.Reset();
}

//______________________________________________________________________________
ArgusWindow::~ArgusWindow()
{
#if 0 // deleting GUI objects may cause error
   SafeDelete(fStatusBar);
   SafeDelete(fMenuBar);
   SafeDelete(fMenuFile);
   SafeDelete(fTab);
   SafeDelete(fTabObjects);
   SafeDelete(fProgress);
#endif
   SafeDelete(fSubWindowRunning);
   SafeDelete(fSubWindowTimeString);
//   SafeDelete(fController); // fController can be deleted by clicking closed box.
}

//______________________________________________________________________________
Bool_t ArgusWindow::Start()
{
   // Start Argus Window
   fWatchAll.Start(false);
   ROMEPrint::Debug("ArgusWindow::Start()\n");
   // Initialize Analyzer Controller
   if (fTabWindow && fControllerActive) {
      fController = new ArgusAnalyzerController(gClient->GetRoot(), this, 100, 100, fControllerNetFolder);
   }

   // Create status bar
   Int_t parts[2] = {70, 30};
   fStatusBar = new TGStatusBar(this, 50, 10, kHorizontalFrame);
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   fProgress = new TGHProgressBar(fStatusBar->GetBarPart(1), TGProgressBar::kStandard, 20);
   fProgress->SetPosition(0);
   fStatusBar->GetBarPart(1)->AddFrame(fProgress, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 10, 10));
   if (fStatusBarSwitch) {
      this->AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 0, 0, 2, 0));
   }

   // Create info frame
   fInfoFrame = new TGHorizontalFrame(this, 0, 0);
   AddFrame(fInfoFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));
   {
      // run# and event#
      fRunEventNumber = new TGLabel(fInfoFrame, "");
      fRunEventNumber->SetTextJustify(kTextCenterX | kTextRight);
      fInfoFrame->AddFrame(fRunEventNumber, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 10, 0, 0));

      // Create menubar
      fMenuBar = new TGMenuBar(fInfoFrame, 1, 1, kHorizontalFrame);
      fInfoFrame->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0));
      {
         // Create popup menu
         fMenuFile = new TGPopupMenu(fClient->GetRoot());
         fMenuFile->Associate(this);
         fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

         if (fTabWindow) {
            fMenuFile->AddEntry("New Window", M_FILE_NEW_WINDOW);
            if (gROME->GetNumberOfNetFolders()>0) {
               TGPopupMenu *fMenuNetFolder = new TGPopupMenu(fClient->GetRoot());
               if (AddMenuNetFolder(fMenuNetFolder)) {
                  fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
                  fMenuNetFolder->Associate(this);
               }
            }
            fMenuFile->AddEntry("Start C&ontroller", M_FILE_CONTROLLER);
            fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
         } else {
            fMenuFile->AddEntry("C&lose", M_FILE_EXIT);
         }
      }
   }

   // separator
   TGHorizontal3DLine *hline = new TGHorizontal3DLine(this);
   AddFrame(hline, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX));

   // Horizontal frame which holds the main canvas
   TGHorizontalFrame *hFrame = new TGHorizontalFrame(this, 0, 0);
   AddFrame(hFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY));
   {
      UInt_t fWidth  = static_cast<UInt_t>(640 * GetWindowScale());
      UInt_t fHeight = static_cast<UInt_t>(480 * GetWindowScale());

      if (fListTreeView && fTabWindow) {
         // Create ListTree
         TGCanvas *canvas = new TGCanvas(hFrame, 120, 400);
         hFrame->AddFrame(canvas, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 1));
         fListTree = new TGListTree(canvas, kHorizontalFrame);
         fListTree->Connect("Clicked(TGListTreeItem*,Int_t)", "ArgusWindow", this, "OnClick(TGListTreeItem*,Int_t)");
         fListTree->Connect("DoubleClicked(TGListTreeItem*,Int_t)", "ArgusWindow", this, "OnDoubleClick(TGListTreeItem*,Int_t)");
         fListTree->Associate(this);
      }

      // Create the main frame
      if (!fListTreeView && fTabWindow) {
         fTab = new TGTab(hFrame, fWidth, fHeight);
         fTab->Associate(this);
         hFrame->AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
      } else {
         fMainFrame = new ROMECompositeFrame(hFrame, fWidth, fHeight);
         hFrame->AddFrame(fMainFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY));
      }
   }

   ROMEPrint::Debug("Creating Tabs\n");
   CheckActiveFlags();
   if (!CreateTabs()) {
      fWatchAll.Stop();
      return kFALSE;
   }
   ROMEPrint::Debug("Tabs Created\n");

   MapSubwindows();

   TGDimension size = GetDefaultSize();

   if (fListTreeView) {
      Int_t iTab;
      const Int_t nTabs = fTabObjects->GetEntriesFast();
      ArgusTab* tab;
      ArgusTab *lastTab = 0;
      TGDimension framesize(0,0);

      // first, hide all the tabs
      for (iTab = 0; iTab < nTabs; iTab++) {
         tab = GetTabObjectAt(iTab);
         fMainFrame->HideFrame(tab);
      }

      // show all the tabs one by one to get the maximum window size
      size = GetDefaultSize();
      for (iTab = nTabs-1; iTab >=0; iTab--) {
         tab = GetTabObjectAt(iTab);
         if (tab->IsSwitch() && fNumberOfChildren[iTab]<=0) {
            fMainFrame->HideFrame(lastTab);
            fMainFrame->ShowFrame(tab);

            framesize = GetDefaultSize();
            if (size.fWidth < framesize.fWidth) {
               size.fWidth = framesize.fWidth;
            }
            if (size.fHeight < framesize.fHeight) {
               size.fHeight = framesize.fHeight;
            }

            fListTree->HighlightItem(fListTreeItem[iTab]);
            fCurrentTabID = iTab;
            lastTab = tab;
         }
      }
   }

   Resize(size);
   gSystem->Sleep(500);
   MapWindow();

   while (!IsMapped()) {
      gSystem->Sleep(20);
   }

   if (!fListTreeView) {
      fCurrentTabID = 1;
      ProcessMessage(MK_MSG(kC_COMMAND, kCM_TAB), 0, 0);
   } else {
      GetTabObject(fCurrentTabID)->BaseTabSelected();
   }

   gSystem->Sleep(500);

   ROMEPrint::Debug("End of ArgusWindow::Start()\n");
   fWatchAll.Stop();
   return kTRUE;
}

//______________________________________________________________________________
Int_t ArgusWindow::GetSelectedItemIndex(TGListTreeItem* item) const
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();

   for (iTab = 0; iTab < nTabs; iTab++ ){
      if (item == fListTreeItem[iTab]) {
         return iTab;
      }
   }

   Error("GetSelectedItemIndex", "item was not found.");
   return -1;
}

//______________________________________________________________________________
void ArgusWindow::OnClick(TGListTreeItem* item, Int_t btn)
{
   ArgusTab* tab;
   Int_t iTab = GetSelectedItemIndex(item);
   if (iTab < 0 || iTab == fCurrentTabID) {
      return;
   }

   // selecting a branch-node is not allowed. Buttons other than button1 are not allowed, either.
   if (fNumberOfChildren[iTab] > 0 || btn != kButton1) {
      fListTree->SetSelected(fListTreeItem[fCurrentTabID]);
      fListTree->HighlightItem(fListTreeItem[fCurrentTabID], kTRUE, kTRUE);
      fListTree->HighlightItem(fListTreeItem[iTab], kFALSE, kTRUE);
      return;
   }
   
   // cleanup current tab
   tab = GetTabObject(fCurrentTabID);
   fMainFrame->HideFrame(tab);
   tab->BaseTabUnSelected();
   
   // go to the selected tab
   fCurrentTabID = iTab;
   tab = GetTabObject(fCurrentTabID);
   fMainFrame->ShowFrame(tab);
   tab->BaseTabSelected();
}

//______________________________________________________________________________
void ArgusWindow::OnDoubleClick(TGListTreeItem* item, Int_t btn)
{
   ArgusTab* tab;
   Int_t iTab = GetSelectedItemIndex(item);
   if (iTab < 0 || iTab == fCurrentTabID) {
      return;
   }

   // do not close if current tab is children of selected one
   Bool_t iflag = kFALSE;
   Int_t  parent = fParentIndex[fCurrentTabID];
   while (parent >=0) {
      if (iTab == parent) {
         iflag = kTRUE;
         break;
      }
      parent = fParentIndex[parent];
   }
   if (iflag) {
      fListTree->OpenItem(item);
   }

   // selecting a branch-node is not allowed. Buttons other than button1 are not allowed, either.
   if (fNumberOfChildren[iTab] > 0 || btn != kButton1) {
      fListTree->SetSelected(fListTreeItem[fCurrentTabID]);
      fListTree->HighlightItem(fListTreeItem[fCurrentTabID], kTRUE, kTRUE);
      fListTree->HighlightItem(fListTreeItem[iTab], kFALSE, kTRUE);
      return;
   }
}

//______________________________________________________________________________
void ArgusWindow::CloseWindow()
{
   gROME->WindowClosed();
}

//______________________________________________________________________________
Bool_t ArgusWindow::IsSubWindowRunningAt(Int_t i)
{
   const Int_t n = fSubWindowRunning->GetSize();
   if (i<n) {
      return fSubWindowRunning->At(i) != 0;
   } else {
      return kFALSE;
   }
}

//______________________________________________________________________________
void ArgusWindow::SetSubWindowRunningAt(Int_t i, Bool_t running)
{
   if (fSubWindowRunning->GetSize() <= i) {
      if (fSubWindowRunning->GetSize() * 2 > i) {
         fSubWindowRunning->Set(fSubWindowRunning->GetSize() * 2);
      } else {
         fSubWindowRunning->Set(i + 1);
      }
   }
   fSubWindowRunning->AddAt(running ? 1 : 0, i);
}

//______________________________________________________________________________
const char* ArgusWindow::GetSubWindowTimeStringAt(Int_t i)
{
   const Int_t n = fSubWindowTimeString->GetEntriesFast();
   if (i<n) {
      return fSubWindowTimeString->At(i).Data();
   } else {
      return "";
   }
}

//______________________________________________________________________________
void ArgusWindow::SetSubWindowTimeStringAt(Int_t i, const char* timeString)
{
   fSubWindowTimeString->AddAt(timeString, i);
}

//______________________________________________________________________________
const char* ArgusWindow::GetTimeStatisticsString(ROMEString& string)
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   ROMEString str;
   if (fTabWindow) {
      string.SetFormatted("main window........................ : %s  %s\n",
                          fWatchAll.GetRealTimeString(str), fWatchAll.GetCpuTimeString(str));
   } else {
      string.SetFormatted("sub window %3d..................... : %s  %s\n", fWindowId,
                          fWatchAll.GetRealTimeString(str), fWatchAll.GetCpuTimeString(str));
   }
   for (iTab = 0; iTab < nTabs; iTab++) {
      static_cast<ArgusTab*>(fTabObjects->At(iTab))->GetTimeStatisticsString(str);
      string.AppendFormatted(str.Data());
   }
   for (iTab = 0; iTab < nTabs; iTab++) {
      if (IsSubWindowRunningAt(iTab)) {
         string.AppendFormatted(static_cast<ArgusWindow*>(fSubWindows->At(iTab))->GetTimeStatisticsString(str));
      } else {
         string.AppendFormatted(GetSubWindowTimeStringAt(iTab));
      }
   }
   return string.Data();
}

//______________________________________________________________________________
void ArgusWindow::SetStatus(Int_t mode, const char *text, Double_t progress, Int_t sleepingTime)
{
   // Set status bar
   // mode 0 : initialization
   // mode 1 : set progress
   // mode 2 : finish

   if (!fStatusBar) {
      return;
   }

   fProgress->SetPosition(static_cast<Float_t>(fProgress->GetMax()*progress));
   fProgress->SetPosition(static_cast<Float_t>(fProgress->GetMax()*progress));
   fStatusBar->SetText(text);

   if (mode == 0) {
      fStatusBar->GetBarPart(0)->SetBackgroundColor(TColor::RGB2Pixel(255, 150, 150));
//      fStatusBar->Layout();
      fProgress->Reset();
   } else  if (mode == 2) {
      fStatusBar->GetBarPart(0)->SetBackgroundColor(GetDefaultFrameBackground());
   }
   gROME->GetApplication()->DisableFPETrap();
   gSystem->ProcessEvents();
   gROME->GetApplication()->EnableFPETrap();
   gSystem->Sleep(sleepingTime);
   return;
}

//______________________________________________________________________________
void ArgusWindow::ClearStatusBar()
{
/*
   Int_t parts[2] = {70, 30};
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   fStatusBar->SetText("", 0);
   fStatusBar->SetText("", 1);
*/
}

//______________________________________________________________________________
ArgusTab* ArgusWindow::GetTabObject(const char* tabTitle) const
{
   // This method might return wrong tab if there are two tabs with same title
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   ArgusTab* ptr;
   for (iTab = 0; iTab < nTabs; iTab++) {
      ptr = static_cast<ArgusTab*>(fTabObjects->At(iTab));
      if (!strcmp(tabTitle, ptr->GetTitle())) {
         return ptr;
      }
   }
   Error("GetTabObject", "title %s was not found.", tabTitle);
   return 0;
}

//______________________________________________________________________________
ArgusTab* ArgusWindow::GetTabObject(const int id) const
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   ArgusTab* ptr;
   for (iTab = 0; iTab < nTabs; iTab++) {
      ptr = static_cast<ArgusTab*>(fTabObjects->At(iTab));
      if (ptr->GetID() == id) {
         return ptr;
      }
   }
   Error("GetTabObject", "id %d was not found.", id);
   return 0;
}

//______________________________________________________________________________
Int_t ArgusWindow::GetCurrentTabObjectIndex() const
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   ArgusTab *ptr;
   for (iTab = 0; iTab < nTabs; iTab++) {
      ptr = static_cast<ArgusTab*>(fTabObjects->At(iTab));
      if (ptr->IsCurrentTab()) {
         return iTab;
      }
   }
//   Don't print the error, because there is no active tab at the very beginning
//   Error("GetCurrentTabObjectIndex", "no tab active.");
   return -1;
}

//______________________________________________________________________________
Bool_t ArgusWindow::CreateTabs()
{
   ArgusTab         *tab;
   Int_t             iTab;
   const Int_t       nTabs = fTabObjects->GetEntriesFast();
   TGCompositeFrame *tabFrame;
   Int_t             newID;
   TGTab            *parentTab;
   TGTab            *newTab;
   TGListTreeItem   *item;
   ROMEString        command = "";

   for (iTab = 0; iTab < nTabs; iTab++) {
      tab = GetTabObjectAt(iTab);
      if (tab->IsSwitch()) {
         if (!fListTreeView) {
            parentTab = (fParentIndex[iTab]==-1) ? fTab : static_cast<TGTab*>(fTGTab->At(fParentIndex[iTab]));
            if (fNumberOfChildren[iTab]<=0) {
               if (fTabWindow) {
                  if (parentTab) {
                     newID = parentTab->GetNumberOfTabs() + ( (fParentIndex[iTab]==-1) ? 0 : 1000*fParentIndex[iTab] );
                     tabFrame = parentTab->AddTab(tab->GetTitle());
                     tab->ReparentWindow(tabFrame, 60, 20);
                     tab->ArgusInit();
                     tab->SetID(newID);
                     tabFrame->AddFrame(tab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));
                  }
               } else {
                  tab->ReparentWindow(fMainFrame, 60, 20);
                  tab->ArgusInit();
                  tab->SetID(0);
                  fMainFrame->AddFrame(tab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));
                  return kTRUE;
               }
            } else {
               newID = parentTab->GetNumberOfTabs() + ( (fParentIndex[iTab]==-1) ? 0 : 1000*fParentIndex[iTab] );
               tabFrame = parentTab->AddTab(tab->GetTitle());
               tab->ReparentWindow(tabFrame, 60, 20);
               tab->ArgusInit();
               tab->SetID(newID);
               newTab = new TGTab(tabFrame);
               command.SetFormatted("gAnalyzer->GetWindow()->ProcessMessage($MSG, $PARM1 + %d, $PARM2)", iTab*1000);
               newTab->SetCommand(command.Data());
               tabFrame->AddFrame(newTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
               fTGTab->AddAt(newTab, iTab);
            }
         } else {
            item = (fParentIndex[iTab]==-1) ? 0 : fListTreeItem[fParentIndex[iTab]];
            fListTreeItem[iTab] = fListTree->AddItem(item, tab->GetTitle());
            fListTree->OpenItem(fListTreeItem[iTab]);
            if (fNumberOfChildren[iTab]<=0) {
               tab->ReparentWindow(fMainFrame, 60, 20);
               tab->ArgusInit();
               tab->SetID(iTab);
               fMainFrame->AddFrame(tab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));
               if (!fTabWindow) {
                  return kTRUE;
               }
            } else {
               tab->ArgusInit();
               tab->SetID(iTab);
            }
         }
      }
   }
   
   return kTRUE;
}

//______________________________________________________________________________
void ArgusWindow::SetControllerNetFolder(const char* folderName)
{
   if (!gROME->IsNetFolderActive(folderName)) {
      return;
   }
   fControllerNetFolder = gROME->GetNetFolder(folderName);
}

//______________________________________________________________________________
void ArgusWindow::RequestEventHandling()
{
   SetStatus(0, "", 0);
   fRequestEventHandling = true;
}

//______________________________________________________________________________
void ArgusWindow::TriggerEventHandler()
{
   int i;
   char str[128];
   SetStatus(0, "", 0);
   fWatchAll.Start(false);
   if (fControllerActive && fController) {
      fController->Update();
   }

   if (!gROME->IsStandAloneARGUS()) {
#if defined(R__UNIX)
      sprintf(str, "Run : %6lld     Event : %6lld",
              gROME->GetCurrentRunNumber(), gROME->GetCurrentEventNumber());
#else
      sprintf(str, "Run : %6I64d     Event : %6I64d",
              gROME->GetCurrentRunNumber(), gROME->GetCurrentEventNumber());
#endif
      fRunEventNumber->SetText(str);
      fInfoFrame->Layout(); // call the parent frame's Layout() method to force updating of size of labels.
   }
   ArgusTab *tab = GetTabObject(fCurrentTabID);
   if (tab && tab->IsSwitch()) {
      tab->ArgusEventHandler();
   }

   for (i = 0; i < fSubWindows->GetEntriesFast(); i++) {
      if (IsSubWindowRunningAt(i)) {
         static_cast<ArgusWindow*>(fSubWindows->At(i))->TriggerEventHandler();
      }
   }
   fWatchAll.Stop();
   SetStatus(2,"", 0);
}

//______________________________________________________________________________
Bool_t ArgusWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t /*param2*/)
{
   Int_t        iTab     = 0;
   ArgusTab    *tab      = 0;
   ArgusTab    *newTab   = 0;
   ROMEString   newTitle = "";
   ArgusWindow *newWindow = 0;

   // Process messages coming from widgets associated with the dialog.  
   switch (GET_MSG(msg)) {
   case kC_COMMAND:    
      switch (GET_SUBMSG(msg)) {
      case kCM_MENU:
         tab = GetTabObject(fCurrentTabID);
         tab->BaseMenuClicked(0, param1);
         switch (param1) {
         case M_FILE_NEW_WINDOW:
            tab->SetRegisteringActive(kFALSE);
            newTitle = "ARGUS - ";
            newTitle += tab->GetTitle();

            newWindow = CreateSubWindow();
            newWindow->SetStatusBarSwitch(fStatusBarSwitch);
//            newWindow->SetListTreeView(kFALSE);
            newWindow->SetControllerActive(fControllerActive);
            newWindow->SetWindowScale(fWindowScale);
            newWindow->SetWindowName(newTitle.Data());
            newWindow->SetWindowId(fSubWindows->GetEntriesFast());
            newWindow->ClearEventHandlingRequest();

            newTab = newWindow->GetTabObjectAt(GetCurrentTabObjectIndex());
            newTab->SetTabActive(kTRUE);
            newTab->SetCurrentTab(kTRUE);
            newTab->SetSwitch(kTRUE);

            newWindow->Start();
            SetSubWindowRunningAt(fSubWindows->GetEntriesFast(),kTRUE);
            fSubWindows->Add(newWindow);
            break;
         case M_FILE_EXIT:
            if (fTabWindow) {
               CloseWindow();
            } else {
               ROMEString str;
               gROME->GetWindow()->SetSubWindowRunningAt(fWindowId,kFALSE);
               gROME->GetWindow()->SetSubWindowTimeStringAt(fWindowId,GetTimeStatisticsString(str));
               TGMainFrame::CloseWindow();
               tab->SetRegisteringActive(true);
               if (!tab->IsTabActive()) {
                  tab->UnRegisterObjects();
               }
            }
            break;
         case M_FILE_CONTROLLER:
            if (!fControllerActive) {
               fController = new ArgusAnalyzerController(gClient->GetRoot(),this, 100, 100, fControllerNetFolder);
               fControllerActive = kTRUE;
            }
            break;
         default:
            ProcessMessageNetFolder(param1);
            break;
         }
         break;
      case kCM_BUTTON:
         break;
      case kCM_LISTBOX:
         break;      
      case kCM_TAB:
         if (param1 != fCurrentTabID) {

            // cleanup current tab
            iTab = GetCurrentTabObjectIndex();
            if (iTab>=0) {
               tab = static_cast<ArgusTab*>(fTabObjects->At(iTab));
               tab->BaseTabUnSelected();
            }

            // go to the selected tab
            fCurrentTabID = param1;
            tab = GetTabObject(fCurrentTabID);
            tab->BaseTabSelected();
            iTab = GetCurrentTabObjectIndex();
            if (fNumberOfChildren[iTab] >0) {
               // special treatment for tabs with sub-tab
               ProcessMessage(MK_MSG(kC_COMMAND, kCM_TAB), 1000*iTab + static_cast<TGTab*>(fTGTab->At(iTab))->GetCurrent(), 0);
            }

            // draw windows
            MapSubwindows();
            Layout();
            MapWindow();
         }
         break;
      }
      break;
   }
   return kTRUE;
}

//______________________________________________________________________________
//Int_t ArgusWindow::GetActiveTabObjectIndex()
//{
//   for (int i = 0; i < fTabObjects->GetEntriesFast(); i++) {
//      if (static_cast<ArgusTab*>(fTabObjects->At(i))->IsTabActive()) {
//         return i;
//      }
//   }
//   Error("GetActiveTabObjectIndex", "no tab active.");
//   return -1;
//}

//______________________________________________________________________________
void ArgusWindow::CheckActiveFlags()
{
   if (gROME->IsROMEMonitor()) {
      int i,j;
      ROMETask *ptr;
      Bool_t flag;
      for (i = 0; i < gROME->GetTaskObjectEntries(); i++) {
         ptr = gROME->GetTaskObjectAt(i);
         if (gROME->IsTaskActive(i)) {
            ptr->SetActive(true);
            for (j = 0; j < ptr->GetNumberOfHistos(); j++) {
               flag = gROME->IsHistoActive(i,j);
               ptr->GetHistoParameterAt(j)->SetActive(flag);
            }
            for (j = 0; j < ptr->GetNumberOfGraphs(); j++) {
               flag = gROME->IsGraphActive(i,j);
               ptr->GetHistoParameterAt(j)->SetActive(flag);
            }
         } else {
            ptr->SetActive(false);
            for (j = 0; j < ptr->GetNumberOfHistos(); j++) {
               ptr->GetHistoParameterAt(j)->SetActive(false);
            }
            for (j = 0; j < ptr->GetNumberOfGraphs(); j++) {
               ptr->GetGraphParameterAt(j)->SetActive(false);
            }
         }
      }
   }
}

