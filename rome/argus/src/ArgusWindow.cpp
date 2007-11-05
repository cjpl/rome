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
,fTabWindow(kTRUE)
,fWatchAll()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fArgusActive(kFALSE)
,fStatusBar(0)
,fStatusBarSwitch(statusBarSwitch)
,fProgress(0)
,fMenuBar(0)
,fMenuFile(0)
,fRunEventNumber(0)
,fListTreeView(kFALSE)
,fListTree(0)
,fListTreeItem(new TGListTreeItem*[numberOfTabs])
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(-1)
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
,fTabWindow(tabWindow)
,fWatchAll()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fArgusActive(kFALSE)
,fStatusBar(0)
,fStatusBarSwitch(statusBarSwitch)
,fProgress(0)
,fMenuBar(0)
,fMenuFile(0)
,fRunEventNumber(0)
,fListTreeView(kFALSE)
,fListTree(0)
,fListTreeItem(new TGListTreeItem*[numberOfTabs])
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(-1)
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
//   SafeDelete(fController); // fController can be deleted by clicking closed box.
}

//______________________________________________________________________________
Bool_t ArgusWindow::Start()
{
   // Start Argus Window
   fWatchAll.Start(false);
   ROMEPrint::Debug("ArgusWindow::Start()\n");

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

   // Create horizontal frame which holds menu bar and run#/event#
   TGHorizontalFrame *hMenuFrame = new TGHorizontalFrame(this, 0, 0);
   AddFrame(hMenuFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));
   {
      // run# and event#
      fRunEventNumber = new TGLabel(hMenuFrame, "");
      fRunEventNumber->SetTextJustify(kTextCenterX | kTextRight);
      hMenuFrame->AddFrame(fRunEventNumber, new TGLayoutHints(kLHintsRight | kLHintsCenterY, 0, 10, 0, 0));

      // Create menubar
      fMenuBar = new TGMenuBar(hMenuFrame, 1, 1, kHorizontalFrame);
      hMenuFrame->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 0, 0));
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
//            fMenuFile->AddEntry("Start C&ontroller", M_FILE_CONTROLLER);
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

   // Vertical frame which holds the info frame and listtree
   TGVerticalFrame *vFrame = new TGVerticalFrame(hFrame, 0, 0);
   hFrame->AddFrame(vFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY, 0, 1));
   {

      // now analyzer controller is dockable
      if (fTabWindow && !gROME->IsStandAloneARGUS()) {
         fController = new ArgusAnalyzerController(vFrame, -1, fControllerNetFolder);
         vFrame->AddFrame(fController, new TGLayoutHints (kLHintsTop | kLHintsLeft));
      }

//         fControllerDock = new TGDockableFrame(vFrame, 0, kVerticalFrame);
//         vFrame->AddFrame(fControllerDock, new TGLayoutHints(kLHintsTop | kLHintsLeft));
//         fControllerDock->EnableUndock(kTRUE);
//         fControllerDock->EnableHide(kTRUE);
//         fControllerDock->Associate(this);

      if (fListTreeView && fTabWindow) {
         // Create ListTree
         TGCanvas *canvas = new TGCanvas(vFrame, 120, 400);
         vFrame->AddFrame(canvas, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY));
         fListTree = new TGListTree(canvas, kHorizontalFrame);
         fListTree->Connect("Clicked(TGListTreeItem*,Int_t)", "ArgusWindow", this, "OnClick(TGListTreeItem*,Int_t)");
         fListTree->Connect("DoubleClicked(TGListTreeItem*,Int_t)", "ArgusWindow", this, "OnDoubleClick(TGListTreeItem*,Int_t)");
         fListTree->Associate(this);
      }

      // Create the main frame
      const UInt_t width  = static_cast<UInt_t>(640 * GetWindowScale());
      const UInt_t height = static_cast<UInt_t>(480 * GetWindowScale());
      if (!fListTreeView && fTabWindow) {
         fTab = new TGTab(hFrame, width, height);
         fTab->Associate(this);
         hFrame->AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
      } else {
         fMainFrame = new ROMECompositeFrame(hFrame, width, height);
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

   ArgusTab *lastTab = 0;
   if (fListTreeView) {
      Int_t iTab;
      const Int_t nTabs = fTabObjects->GetEntriesFast();
      ArgusTab* tab;
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
//            fCurrentTabID = iTab;
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
      lastTab->BaseTabSelected();
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
   Int_t currentTabIndex = GetCurrentTabObjectIndex();
   Int_t selectedTabIndex = GetSelectedItemIndex(item);
   if (selectedTabIndex < 0 || selectedTabIndex == currentTabIndex) {
      return;
   }

   // selecting a branch-node is not allowed. Buttons other than button1 are not allowed, either.
   if (fNumberOfChildren[selectedTabIndex] > 0 || btn != kButton1) {
      fListTree->SetSelected(fListTreeItem[currentTabIndex]);
      fListTree->HighlightItem(fListTreeItem[currentTabIndex], kTRUE, kTRUE);
      fListTree->HighlightItem(fListTreeItem[selectedTabIndex], kFALSE, kTRUE);
      return;
   }
   
   // cleanup current tab
   ArgusTab *tab = GetTabObjectAt(currentTabIndex);
   fMainFrame->HideFrame(tab);
   tab->BaseTabUnSelected();
   
   // go to the selected tab
   tab = GetTabObjectAt(selectedTabIndex);
   fMainFrame->ShowFrame(tab);
   tab->BaseTabSelected();
}

//______________________________________________________________________________
void ArgusWindow::OnDoubleClick(TGListTreeItem* item, Int_t btn)
{
   Int_t currentTabIndex = GetCurrentTabObjectIndex();
   Int_t selectedTabIndex = GetSelectedItemIndex(item);
   if (selectedTabIndex < 0 || selectedTabIndex == currentTabIndex) {
      return;
   }

   // do not close if current tab is children of selected one
   Bool_t iflag = kFALSE;
   Int_t  parent = fParentIndex[currentTabIndex];
   while (parent >=0) {
      if (selectedTabIndex == parent) {
         iflag = kTRUE;
         break;
      }
      parent = fParentIndex[parent];
   }
   if (iflag) {
      fListTree->OpenItem(item);
   }

   // selecting a branch-node is not allowed. Buttons other than button1 are not allowed, either.
   if (fNumberOfChildren[selectedTabIndex] > 0 || btn != kButton1) {
      fListTree->SetSelected(fListTreeItem[currentTabIndex]);
      fListTree->HighlightItem(fListTreeItem[currentTabIndex], kTRUE, kTRUE);
      fListTree->HighlightItem(fListTreeItem[selectedTabIndex], kFALSE, kTRUE);
      return;
   }
}

//______________________________________________________________________________
void ArgusWindow::CloseWindow()
{
   if (fTabWindow) {
      gROME->WindowClosed();
   } else {
      ArgusTab* tab = GetCurrentTabObject();
      ROMEString str;
      gROME->GetWindow()->SetSubWindowRunningAt(fWindowId, kFALSE);
      ClearEventHandlingRequest();
      ClearEventHandlingForced();
      
      // When a sub window is newly opened or it is closed,
      // calling TGMainFrame::CloseWindow() causes BadWindow
      // and/or BadDrawable error on X11, which terminates
      // the program. In order avoid this, just unmap the sub
      // window rather than closing it. This also makes it
      // possible to map the sub window again instead of
      // creating a new sub window.
      
//               TGMainFrame::CloseWindow();
      UnmapWindow();

      tab->SetRegisteringActive(true);
      if (!tab->IsTabActive()) {
         tab->UnRegisterObjects();
      }
   }
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
const char* ArgusWindow::GetTimeStatisticsString(ROMEString& string)
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   Int_t iSub;
   const Int_t nSubs = fSubWindows->GetEntriesFast();
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
   for (iSub = 0; iSub < nSubs; iSub++) {
      string.AppendFormatted(static_cast<ArgusWindow*>(fSubWindows->At(iSub))->GetTimeStatisticsString(str));
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
      if (ptr->IsForeground()) {
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
   int iSub;
   const Int_t nSub = fSubWindows->GetEntriesFast();
   char str[128];

   SetStatus(0, "", 0);
   fWatchAll.Start(false);
   if (fController) {
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
      // call the parent frame's Layout() method to force updating of size of labels.
      ((TGCompositeFrame*)(fRunEventNumber->GetParent()))->Layout();
   }
   ArgusTab *tab = GetCurrentTabObject();
   if (tab && tab->IsSwitch()) {
      tab->ArgusEventHandler();
   }

   for (iSub = 0; iSub < nSub; iSub++) {
      if (IsSubWindowRunningAt(iSub)) {
         static_cast<ArgusWindow*>(fSubWindows->At(iSub))->TriggerEventHandler();
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
   ArgusWindow *subWindow = 0;
   Int_t        iSub;
   Int_t        nSub;

//   cout << "ArgusWindow::ProcessMessage " << GET_MSG(msg) << " " << GET_SUBMSG(msg) << " " << param1 << endl;

   // Process messages coming from widgets associated with the dialog.  
   switch (GET_MSG(msg)) {
   case kC_COMMAND:    
      switch (GET_SUBMSG(msg)) {
      case kCM_MENU:
         iTab = GetCurrentTabObjectIndex();
         tab  = GetTabObjectAt(iTab);
         tab->BaseMenuClicked(0, param1);
         switch (param1) {
         case M_FILE_NEW_WINDOW:
            tab->SetRegisteringActive(kFALSE);

            // Reuse the unmapped sub window with the selected tab, if any.
            nSub = fSubWindows->GetEntriesFast();
            for (iSub = 0; iSub < nSub; iSub++) {
               if (!IsSubWindowRunningAt(iSub)) {
                  subWindow = static_cast<ArgusWindow*>(fSubWindows->At(iSub));
                  if (subWindow->GetCurrentTabObjectIndex() == iTab) {
                     subWindow->MapWindow();
                     SetSubWindowRunningAt(iSub, kTRUE);
                     return kTRUE;
                  }
               }
            }

            newTitle = "ARGUS - ";
            newTitle += tab->GetTitle();

            subWindow = CreateSubWindow();
            subWindow->SetStatusBarSwitch(fStatusBarSwitch);
//            subWindow->SetListTreeView(kFALSE);
            subWindow->SetWindowScale(fWindowScale);
            subWindow->SetWindowName(newTitle.Data());
            subWindow->SetWindowId(fSubWindows->GetEntriesFast());
            subWindow->ClearEventHandlingRequest();
            subWindow->ClearEventHandlingForced();
            newTab = subWindow->GetTabObjectAt(iTab);
            newTab->SetTabActive(kTRUE);
            newTab->SetSwitch(kTRUE);

            subWindow->Start();
            SetSubWindowRunningAt(fSubWindows->GetEntriesFast(),kTRUE);
            fSubWindows->Add(subWindow); 
            break;
         case M_FILE_EXIT:
            CloseWindow();
            break;
         case M_FILE_CONTROLLER:
            if (fController->IsHidden()) {
               fController->ShowContainer();
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
            tab = GetTabObject(param1);
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
   case kC_DOCK:
      switch (GET_SUBMSG(msg)) {
      case kDOCK_DOCK:
         break;
      case kDOCK_UNDOCK:
         break;
      case kDOCK_SHOW:
         break;
      case kDOCK_HIDE:
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

