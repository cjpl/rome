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
#include <TArrayI.h>
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
#include <TTimeStamp.h>
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
,fRunNumber(0)
,fEventNumber(0)
,fYearMonDay(0)
,fHourMinSec(0)
,fUserInfoFrame(0)
,fUserInfoObjects(new TObjArray(0))
,fListTreeView(kFALSE)
,fListTree(0)
,fNListTreeItem(numberOfTabs)
,fListTreeItem(new TGListTreeItem*[numberOfTabs])
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(-1)
,fTabObjects(new TObjArray(numberOfTabs))
,fCurrentTabIndex(-1)
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
   memset(fListTreeItem, 0, sizeof(TGListTreeItem*) * numberOfTabs);
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
,fRunNumber(0)
,fEventNumber(0)
,fYearMonDay(0)
,fHourMinSec(0)
,fUserInfoFrame(0)
,fUserInfoObjects(new TObjArray(0))
,fListTreeView(kFALSE)
,fListTree(0)
,fNListTreeItem(numberOfTabs)
,fListTreeItem(new TGListTreeItem*[numberOfTabs])
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(-1)
,fTabObjects(new TObjArray(numberOfTabs))
,fCurrentTabIndex(-1)
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
   memset(fListTreeItem, 0, sizeof(TGListTreeItem*) * numberOfTabs);
   fWatchAll.Reset();
}

//______________________________________________________________________________
ArgusWindow::~ArgusWindow()
{
   Int_t i;
#if 0 // deleting GUI objects may cause error
   if (fSubWindows) {
      fSubWindows->Delete();
   }
   SafeDelete(fStatusBar);
   SafeDelete(fMenuBar);
   SafeDelete(fMenuFile);
   SafeDelete(fTabObjects);
   SafeDelete(fProgress);
   SafeDelete(fRunNumber);
   SafeDelete(fEventNumber);
   SafeDelete(fYearMonDay);
   SafeDelete(fHourMinSec);
   SafeDelete(fListTree);
   SafeDelete(fTab);
   if (fTabObjects) {
      fTabObjects->Delete();
   }
   if (fTGTab) {
      fTGTab->Delete();
   }
#endif
   SafeDelete(fSubWindows);
   SafeDelete(fSubWindowRunning);
   if (fUserInfoObjects) {
      fUserInfoObjects->Delete();
   }
   for (i = 0; i < fNListTreeItem; i++) {
      SafeDelete(fListTreeItem[i]);
   }
   SafeDeleteArray(fListTreeItem);
   SafeDelete(fUserInfoObjects);
   SafeDelete(fTabObjects);
   SafeDelete(fTGTab);

   SafeDeleteArray(fParentIndex);
   SafeDeleteArray(fNumberOfChildren);
//   SafeDelete(fController); // fController can be deleted by clicking closed box.
//   SafeDelete(fControllerNetFolder); // This class is not the owner of net folder
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

   // Create horizontal frame which holds menu bar
   TGHorizontalFrame *hMenuFrame = new TGHorizontalFrame(this, 0, 0);
   AddFrame(hMenuFrame, new TGLayoutHints(kLHintsExpandX, 0, 0, 0, 0));
   {
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

   {
      // separator
      TGHorizontal3DLine *hline = new TGHorizontal3DLine(this);
      AddFrame(hline, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX));
   }

   // Horizontal frame which holds the main canvas
   TGHorizontalFrame *hFrame = new TGHorizontalFrame(this);
   AddFrame(hFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY));

   // Vertical frame which holds the info frame and listtree
   TGVerticalFrame *vFrame = new TGVerticalFrame(hFrame);
   hFrame->AddFrame(vFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY, 0, 1));
   {

      // now analyzer controller is dockable
      if (fTabWindow && !gROME->IsStandAloneARGUS()) {
         fController = new ArgusAnalyzerController(vFrame, -1, fControllerNetFolder);
         fController->Associate(this);
         vFrame->AddFrame(fController, new TGLayoutHints (kLHintsTop | kLHintsLeft));
      }

      if (!gROME->IsStandAloneARGUS()) {
         // run#
         TGHorizontalFrame *runFrame = new TGHorizontalFrame(vFrame);
         vFrame->AddFrame(runFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 15, 10, 2, 2));

         TGLabel *fRunLabel   = new TGLabel(runFrame, "Run : ");
         fRunLabel->SetTextJustify(kTextLeft | kTextCenterY);
         runFrame->AddFrame(fRunLabel, new TGLayoutHints(kLHintsLeft, 0, 0, 0, 0));

         fRunNumber   = new TGLabel(runFrame, "0");
         fRunNumber->SetTextJustify(kTextRight | kTextCenterY);
         runFrame->AddFrame(fRunNumber, new TGLayoutHints(kLHintsRight, 5, 0, 0, 0));

         // evnet#
         TGHorizontalFrame *eventFrame = new TGHorizontalFrame(vFrame);
         vFrame->AddFrame(eventFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 15, 10, 2, 2));

         TGLabel *fEventLabel   = new TGLabel(eventFrame, "Eevent : ");
         fEventLabel->SetTextJustify(kTextLeft | kTextCenterY);
         eventFrame->AddFrame(fEventLabel, new TGLayoutHints(kLHintsLeft, 0, 0, 0, 0));

         fEventNumber   = new TGLabel(eventFrame, "0");
         fEventNumber->SetTextJustify(kTextLeft | kTextCenterY);
         eventFrame->AddFrame(fEventNumber, new TGLayoutHints(kLHintsRight, 5, 0, 0, 0));

         // Year/Mon/Day
         TGHorizontalFrame *YearMonDayFrame = new TGHorizontalFrame(vFrame);
         vFrame->AddFrame(YearMonDayFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 15, 10, 2, 2));

         TGLabel *fYearMonDayLabel = new TGLabel(YearMonDayFrame, "Date(UTC) : ");
         fYearMonDayLabel->SetTextJustify(kTextLeft | kTextCenterY);
         YearMonDayFrame->AddFrame(fYearMonDayLabel, new TGLayoutHints(kLHintsLeft, 0, 0, 0, 0));

         fYearMonDay = new TGLabel(YearMonDayFrame, "1970-Jan-01");
         fYearMonDay->SetTextJustify(kTextLeft | kTextCenterY);
         YearMonDayFrame->AddFrame(fYearMonDay, new TGLayoutHints(kLHintsRight, 5, 0, 0, 0));

         // Hour/Min/Sec
         TGHorizontalFrame *HourMinSecFrame = new TGHorizontalFrame(vFrame);
         vFrame->AddFrame(HourMinSecFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 15, 10, 2, 2));

         TGLabel *fHourMinSecLabel = new TGLabel(HourMinSecFrame, "Time : ");
         fHourMinSecLabel->SetTextJustify(kTextLeft | kTextCenterY);
         HourMinSecFrame->AddFrame(fHourMinSecLabel, new TGLayoutHints(kLHintsLeft, 0, 0, 0, 0));

         fHourMinSec = new TGLabel(HourMinSecFrame, "00:00:00");
         fHourMinSec->SetTextJustify(kTextLeft | kTextCenterY);
         HourMinSecFrame->AddFrame(fHourMinSec, new TGLayoutHints(kLHintsRight, 5, 0, 0, 0));
      }

      // Vertical frame which holds users info frame
      fUserInfoFrame = new TGVerticalFrame(vFrame);
      vFrame->AddFrame(fUserInfoFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0));

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

   ArgusTab   *tab;
   Int_t       lastTabIndex = -1;
   Int_t       iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   if (fListTreeView) {
      ArgusTab   *lastTab = 0;
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

            if (fTabWindow) { // we don't have listtrees for sub-windows
               fListTree->HighlightItem(fListTreeItem[iTab]);
            }
            lastTab = tab;
            lastTabIndex = iTab;
         }
      }
   } else {
      for (iTab = nTabs-1; iTab >=0; iTab--) {
         tab = GetTabObjectAt(iTab);
         if (tab->IsSwitch() && fNumberOfChildren[iTab]<=0) {
            fCurrentTabID = tab->GetID();
            lastTabIndex = iTab;
         }
      }
   }

   Resize(size);
   fInitialWidth  = size.fWidth;
   fInitialHeight = size.fHeight;

   // Adjust size of the sub window so that the main frame has the
   // same size as the parent's. This is not done if the parent window
   // is tab-view, because the size of the main frame is not trivial.
   if (!fTabWindow) {
      if (gROME->GetWindow()->IsListTreeView()) {
         TGDimension size1 = gROME->GetWindow()->GetInitialSize();
         TGDimension size3 = gROME->GetWindow()->GetUserInfoFrame()->GetSize();
         TGDimension size4 = GetUserInfoFrame()->GetSize();

         // Height shall be the same as the parent, but width might be
         // different because of different width of info frame.
         Resize(size1.fWidth - size3.fWidth + size4.fWidth, size1.fHeight);
      }
   }

   gSystem->Sleep(500);
   MapWindow();

   while (!IsMapped()) {
      gSystem->Sleep(20);
   }

   if (!gROME->IsStandAloneARGUS()) {
      // hide run# and event# if argus controller is shown/docked
      if (fController && (!fController->IsHidden() || !fController->IsUndocked())) {
         ((TGCompositeFrame*)(fRunNumber->GetParent()->GetParent()))->HideFrame((TGFrame*)(fRunNumber->GetParent()));
         ((TGCompositeFrame*)(fEventNumber->GetParent()->GetParent()))->HideFrame((TGFrame*)(fEventNumber->GetParent()));
      }
   }

   TriggerTabSelected(lastTabIndex);

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
void ArgusWindow::OnClick(TGListTreeItem* item, Int_t /* btn */)
{
   Int_t selectedTabIndex = GetSelectedItemIndex(item);
   if (selectedTabIndex < 0 || selectedTabIndex == fCurrentTabIndex) {
      return;
   }

   // selecting a branch-node is not allowed
   if (fNumberOfChildren[selectedTabIndex] > 0) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,0,0)
      // For ROOT 4 and earlier, selecting a branch-node might cause
      // inconsistency; a tab being displayed might be different from
      // the selected one in the ListTree view.
      fListTree->SetSelected(fListTreeItem[fCurrentTabIndex]);
      fListTree->HighlightItem(fListTreeItem[fCurrentTabIndex], kTRUE, kTRUE);
      fListTree->HighlightItem(fListTreeItem[selectedTabIndex], kFALSE, kTRUE);
#endif
      return;
   }

   // cleanup current tab
   ArgusTab *tab = GetTabObjectAt(fCurrentTabIndex);
   fMainFrame->HideFrame(tab);
   TriggerTabUnSelected(fCurrentTabIndex);
   
   // go to the selected tab
   tab = GetTabObjectAt(selectedTabIndex);
   fMainFrame->ShowFrame(tab);
   TriggerTabSelected(selectedTabIndex);
}

//______________________________________________________________________________
void ArgusWindow::OnDoubleClick(TGListTreeItem* item, Int_t /* btn */)
{
   Int_t selectedTabIndex = GetSelectedItemIndex(item);
   if (selectedTabIndex < 0 || selectedTabIndex == fCurrentTabIndex) {
      return;
   }

   // do not close if current tab is children of selected one
   Bool_t iflag = kFALSE;
   Int_t  parent = fParentIndex[fCurrentTabIndex];
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

   // selecting a branch-node is not allowed.
   if (fNumberOfChildren[selectedTabIndex] > 0) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,0,0)
      // For ROOT 4 and earlier, selecting a branch-node might cause
      // inconsistency; a tab being displayed might be different from
      // the selected one in the ListTree view.
      fListTree->SetSelected(fListTreeItem[fCurrentTabIndex]);
      fListTree->HighlightItem(fListTreeItem[fCurrentTabIndex], kTRUE, kTRUE);
      fListTree->HighlightItem(fListTreeItem[selectedTabIndex], kFALSE, kTRUE);
#endif
      return;
   }
}

//______________________________________________________________________________
void ArgusWindow::CloseWindow()
{
   if (fTabWindow) {
      gROME->WindowClosed();
   } else {
      ArgusTab* tab = GetTabObjectAt(fCurrentTabIndex);
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
const char* ArgusWindow::GetTimeStatisticsString(ROMEString& str1)
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   Int_t iSub;
   const Int_t nSubs = fSubWindows->GetEntriesFast();
   ROMEString str2, str3;
   if (fTabWindow) {
      str1.SetFormatted("main window........................ : %s  %s\n",
                          fWatchAll.GetRealTimeString(str2), fWatchAll.GetCpuTimeString(str3));
   } else {
      str1.SetFormatted("sub window %3d..................... : %s  %s\n", fWindowId,
                          fWatchAll.GetRealTimeString(str2), fWatchAll.GetCpuTimeString(str3));
   }
   for (iTab = 0; iTab < nTabs; iTab++) {
      static_cast<ArgusTab*>(fTabObjects->At(iTab))->GetTimeStatisticsString(str2);
      str1.AppendFormatted(str2.Data());
   }
   for (iSub = 0; iSub < nSubs; iSub++) {
      str1.AppendFormatted(static_cast<ArgusWindow*>(fSubWindows->At(iSub))->GetTimeStatisticsString(str2));
   }
   return str1.Data();
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
Int_t ArgusWindow::GetTabObjectIndex(const int id) const
{
   Int_t iTab;
   const Int_t nTabs = fTabObjects->GetEntriesFast();
   ArgusTab* ptr;
   for (iTab = 0; iTab < nTabs; iTab++) {
      ptr = static_cast<ArgusTab*>(fTabObjects->At(iTab));
      if (ptr->GetID() == id) {
         return iTab;
      }
   }
   Error("GetTabObject", "id %d was not found.", id);
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
   Int_t             iParent;
//   Bool_t            isSwitch;   

   for (iTab = 0; iTab < nTabs; iTab++) {
      tab = GetTabObjectAt(iTab);
//      isSwitch = tab->IsSwitch();

      // switch off the tab if its ancestor is off
      iParent = fParentIndex[iTab];
      while (iParent != -1) {
         if (! GetTabObjectAt(iParent)->IsSwitch()) {
            tab->SetSwitch(kFALSE);
            break;
         }
         iParent = fParentIndex[iParent];
      }
//      cout << "CreateTabs : " << iTab << " " << tab->IsSwitch() << " " << isSwitch << " " << tab->GetName() << endl;

      if (!tab->IsSwitch()) {
         continue;
      }

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
         if (fTabWindow) { // we don't have listtrees for sub-windows
            item = (fParentIndex[iTab]==-1) ? 0 : fListTreeItem[fParentIndex[iTab]];
            fListTreeItem[iTab] = fListTree->AddItem(item, tab->GetTitle());
            fListTree->OpenItem(fListTreeItem[iTab]);
         }
         if (fNumberOfChildren[iTab]<=0) {
            tab->ReparentWindow(fMainFrame, 60, 20);
            tab->ArgusInit();
            fMainFrame->AddFrame(tab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));
            if (!fTabWindow) {
               return kTRUE;
            }
         } else {
            tab->ArgusInit();
         }
      }
   }

   return kTRUE;
}

//______________________________________________________________________________
void ArgusWindow::TriggerTabSelected(Int_t index)
{
   Int_t        tabLevel = 0;
   const Int_t  nTabs = fTabObjects->GetEntriesFast();
   Int_t       *tabStack = new Int_t[nTabs];

   fCurrentTabIndex = index;

   // push parent tab's index to stack until reaching the top-level tab
   tabStack[tabLevel] = index;
   while (tabLevel+1<nTabs && (tabStack[tabLevel+1]=fParentIndex[tabStack[tabLevel]]) != -1) {
      tabLevel++;
   }

   // call BaseTabSelected() thru ancestry to descendant
   for ( ; tabLevel>=0; tabLevel--) {
      GetTabObjectAt(tabStack[tabLevel])->BaseTabSelected();
   }

   // call the parent frame's Layout() method so that menus added in BaseTabSelected() will be displayed.
   ((TGCompositeFrame*)(fMenuBar->GetParent()))->Layout();

   delete [] tabStack;
}

//______________________________________________________________________________
void ArgusWindow::TriggerTabUnSelected(Int_t index)
{
   GetTabObjectAt(index)->BaseTabUnSelected();

   // trace the ancestry and call BaseTabUnSelected()
   while (fParentIndex[index] != -1) {
      index = fParentIndex[index];
      GetTabObjectAt(index)->BaseTabUnSelected();
   }
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
   int          iSub;
   const Int_t  nSub = fSubWindows->GetEntriesFast();
   Int_t        tabLevel = 0;
   const Int_t  nTabs = fTabObjects->GetEntriesFast();
   Int_t       *tabStack = new Int_t[nTabs];

   char         str[64];
   UInt_t       year, mon, day, hour, min, sec;
   const char   month[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
   const Bool_t iUTC = kTRUE;

   SetStatus(0, "", 0);
   fWatchAll.Start(false);
   if (fController) {
      fController->Update();
   }

   if (!gROME->IsStandAloneARGUS()) {
      // run#
#if defined(R__UNIX)
      sprintf(str, "%7lld", gROME->GetCurrentRunNumber());
#else
      sprintf(str, "%7I64d", gROME->GetCurrentRunNumber());
#endif
      fRunNumber  ->SetText(str);

      // event#
#if defined(R__UNIX)
      sprintf(str, "%7lld", gROME->GetCurrentEventNumber());
#else
      sprintf(str, "%7I64d", gROME->GetCurrentEventNumber());
#endif
      fEventNumber->SetText(str);

      // date and time
      TTimeStamp timeStamp(gROME->GetActiveDAQ()->GetTimeStamp(), 0);
      timeStamp.GetDate(iUTC, 0, &year, &mon, &day);
      timeStamp.GetTime(iUTC, 0, &hour, &min, &sec);

      sprintf(str, "%4d-%3s-%02d", year, month[mon-1], day);
      fYearMonDay->SetText(str);

      sprintf(str, "%02d:%02d:%02d", hour, min, sec);
      fHourMinSec->SetText(str);
   }

   // push parent tab's index to stack until reaching the top-level tab
   tabStack[tabLevel] = fCurrentTabIndex;
   while (tabLevel+1<nTabs && (tabStack[tabLevel+1]=fParentIndex[tabStack[tabLevel]]) != -1) {
      tabLevel++;
   }

   // call ArgusEventHandler() thru ancestry to descendant
   for ( ; tabLevel>=0; tabLevel--) {
      GetTabObjectAt(tabStack[tabLevel])->ArgusEventHandler();
   }
   delete [] tabStack;

   // call event handlers for sub-windows
   for (iSub = 0; iSub < nSub; iSub++) {
      if (IsSubWindowRunningAt(iSub)) {
         static_cast<ArgusWindow*>(fSubWindows->At(iSub))->TriggerEventHandler();
      }
   }

   // call Layout() method to force updating of size of labels in the infoFrame, including UserInfoFrame
   ((TGCompositeFrame*)(fUserInfoFrame->GetParent()))->Layout();

   // clear requests
   ClearEventHandlingRequest();
   ClearEventHandlingForced();

   fWatchAll.Stop();
   SetStatus(2, "", 0);
}

//______________________________________________________________________________
Bool_t ArgusWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t /*param2*/)
{
   Int_t        selectedTabIndex = 0;
   ArgusTab    *tab       = 0;
   ArgusTab    *newTab    = 0;
   ROMEString   newTitle  = "";
   ArgusWindow *subWindow = 0;
   Int_t        iTab      = 0;
   Int_t        iParent   = 0;
   Int_t        iSub;
   Int_t        nSub;

//   cout << "ArgusWindow::ProcessMessage " << GET_MSG(msg) << " " << GET_SUBMSG(msg) << " " << param1 << endl;

   // Process messages coming from widgets associated with the dialog.  
   switch (GET_MSG(msg)) {
   case kC_COMMAND:    
      switch (GET_SUBMSG(msg)) {
      case kCM_MENU:
         tab  = GetTabObjectAt(fCurrentTabIndex);
         tab->BaseMenuClicked(0, param1);
         switch (param1) {
         case M_FILE_NEW_WINDOW:
            tab->SetRegisteringActive(kFALSE);

            // Reuse the unmapped sub window with the selected tab, if any.
            nSub = fSubWindows->GetEntriesFast();
            for (iSub = 0; iSub < nSub; iSub++) {
               if (!IsSubWindowRunningAt(iSub)) {
                  subWindow = static_cast<ArgusWindow*>(fSubWindows->At(iSub));
                  if (subWindow->fCurrentTabIndex == fCurrentTabIndex) {
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
            subWindow->SetListTreeView(kTRUE); // sub-windows are always in list-tree mode, which uses fMainFrame rather than fTab.
            subWindow->SetWindowScale(fWindowScale);
            subWindow->SetWindowName(newTitle.Data());
            subWindow->SetWindowId(fSubWindows->GetEntriesFast());
            subWindow->ClearEventHandlingRequest();
            subWindow->ClearEventHandlingForced();
            newTab = subWindow->GetTabObjectAt(fCurrentTabIndex);
            newTab->SetTabActive(kTRUE);
            newTab->SetSwitch(kTRUE);

            // switch on all the ancestor tabs
            iTab = fCurrentTabIndex;
            while ((iParent=fParentIndex[iTab]) != -1) {
               subWindow->GetTabObjectAt(iParent)->SetSwitch(kTRUE);
               iTab = iParent;
            }

            SetSubWindowRunningAt(fSubWindows->GetEntriesFast(),kTRUE);
            fSubWindows->Add(subWindow); 
            subWindow->Start();

            // force event handler for new window
            ForceEventHandling();
            subWindow->TriggerEventHandler();
            ClearEventHandlingForced();

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
            TriggerTabUnSelected(fCurrentTabIndex);

            // go to the terminal-node if selected one is a branch-node
            fCurrentTabID = param1;
            selectedTabIndex = GetTabObjectIndex(fCurrentTabID);
            Int_t nChildren = fNumberOfChildren[selectedTabIndex];
            while (nChildren > 0) {
               fCurrentTabID = 1000*selectedTabIndex + static_cast<TGTab*>(fTGTab->At(selectedTabIndex))->GetCurrent();
               selectedTabIndex = GetTabObjectIndex(fCurrentTabID);
               nChildren = fNumberOfChildren[fCurrentTabIndex];
            }

            // go to the selected tab
            TriggerTabSelected(selectedTabIndex);
         }
         break;
      }
      break;
   case kC_DOCK:
      switch (GET_SUBMSG(msg)) {
         // hide/show run# and event#
      case kDOCK_DOCK:
         ((TGCompositeFrame*)(fRunNumber->GetParent()->GetParent()))->HideFrame((TGFrame*)(fRunNumber->GetParent()));
         ((TGCompositeFrame*)(fEventNumber->GetParent()->GetParent()))->HideFrame((TGFrame*)(fEventNumber->GetParent()));
         break;
      case kDOCK_UNDOCK:
         ((TGCompositeFrame*)(fRunNumber->GetParent()->GetParent()))->ShowFrame((TGFrame*)(fRunNumber->GetParent()));
         ((TGCompositeFrame*)(fEventNumber->GetParent()->GetParent()))->ShowFrame((TGFrame*)(fEventNumber->GetParent()));
         break;
      case kDOCK_SHOW:
         ((TGCompositeFrame*)(fRunNumber->GetParent()->GetParent()))->HideFrame((TGFrame*)(fRunNumber->GetParent()));
         ((TGCompositeFrame*)(fEventNumber->GetParent()->GetParent()))->HideFrame((TGFrame*)(fEventNumber->GetParent()));
         break;
      case kDOCK_HIDE:
         ((TGCompositeFrame*)(fRunNumber->GetParent()->GetParent()))->ShowFrame((TGFrame*)(fRunNumber->GetParent()));
         ((TGCompositeFrame*)(fEventNumber->GetParent()->GetParent()))->ShowFrame((TGFrame*)(fEventNumber->GetParent()));
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
               ptr->GetGraphParameterAt(j)->SetActive(flag);
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

