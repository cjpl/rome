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
#include <TSystem.h>
#include <TGTab.h>
#include <TGStatusBar.h>
#include <TGProgressBar.h>
#include <TColor.h>
#include <TObjArray.h>
#include <TGMenu.h>
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
ArgusWindow::ArgusWindow(Bool_t statusBarSwitch, TObjArray *tabobj)
:TGMainFrame()
,fWindowId(-1)
,fSubWindows(new TObjArray(0))
,fSubWindowRunning(new TArrayI())
,fSubWindowTimeString(new ROMEStrArray())
,fTabWindow(kTRUE)
,fWatchAll()
,fTimeAllString("")
,fArgusActive(kFALSE)
,fStatusBar(0)
,fStatusBarSwitch(statusBarSwitch)
,fProgress(0)
,fInfoFrame(0)
,fMenuBar(0)
,fMenuFile(0)
,fMenuNetFolder(0)
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(0)
,fTabObjects(tabobj)
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
ArgusWindow::ArgusWindow(const TGWindow* p, Bool_t statusBarSwitch, TObjArray *tabobj, Bool_t tabWindow)
:TGMainFrame(p, 1, 1)
,fWindowId(-1)
,fSubWindows(new TObjArray(0))
,fSubWindowRunning(new TArrayI())
,fSubWindowTimeString(new ROMEStrArray())
,fTabWindow(tabWindow)
,fWatchAll()
,fTimeAllString("")
,fArgusActive(kFALSE)
,fStatusBar(0)
,fStatusBarSwitch(statusBarSwitch)
,fProgress(0)
,fInfoFrame(0)
,fMenuBar(0)
,fMenuFile(0)
,fMenuNetFolder(0)
,fTab(0)
,fMainFrame(0)
,fCurrentTabID(0)
,fTabObjects(tabobj)
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
   SafeDelete(fMenuNetFolder);
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

   // Create menu
   fMenuNetFolder = new TGPopupMenu (fClient->GetRoot());
   fMenuFile = new TGPopupMenu (fClient->GetRoot());
   fMenuFile->Associate(this);

   if (fTabWindow) {
      fMenuFile->AddEntry("New Window", M_FILE_NEW_WINDOW);
      if (AddMenuNetFolder(fMenuNetFolder)) {
         fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
      }
      fMenuFile->AddEntry("Start C&ontroller", M_FILE_CONTROLLER);
      fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   } else {
      fMenuFile->AddEntry("C&lose", M_FILE_EXIT);
   }

   fMenuNetFolder->Associate(this);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));

   // Create info frame
   if (fTabWindow) {
//   fInfoFrame = new ROMECompositeFrame(this, 100, 0, kRaisedFrame | kVerticalFrame | kHorizontalFrame);
//   this->AddFrame(fInfoFrame, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   }

   // Create tab widget
   if (fTabWindow) {
      fTab = new TGTab(this, static_cast<UInt_t>(600 * GetWindowScale()), static_cast<UInt_t>(400 * GetWindowScale()));
      AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   } else {
      fMainFrame = new ROMECompositeFrame(this, static_cast<UInt_t>(600 * GetWindowScale()),
                                          static_cast<UInt_t>(400 * GetWindowScale()));
      AddFrame(fMainFrame, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   }

   ROMEPrint::Debug("Creating Tabs\n");
   if (!CreateTabs()) {
      fWatchAll.Stop();
      return kFALSE;
   }
   ROMEPrint::Debug("Tabs Created\n");

   MapSubwindows();
   Resize(GetDefaultSize());
   gSystem->Sleep(500);
   MapWindow();
   fCurrentTabID = 1;

   while (IsMapped()) {
      gSystem->Sleep(20);
   }

   ProcessMessage(MK_MSG(kC_COMMAND, kCM_TAB), 0, 0);

   gSystem->Sleep(500);

   ROMEPrint::Debug("End of ArgusWindow::Start()\n");
   fWatchAll.Stop();
   return kTRUE;
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
void ArgusWindow::CloseWindow()
{
   gROME->WindowClosed();
}

//______________________________________________________________________________
ArgusTab* ArgusWindow::GetTabObject(const char* tabName)
{
   for (int i = 0; i < fTabObjects->GetEntries(); i++) {
      if (!strcmp(tabName, static_cast<ArgusTab*>(fTabObjects->At(i))->GetTitle())) {
         return static_cast<ArgusTab*>(fTabObjects->At(i));
      }
   }
   Error("GetTabObject", "%s was not found.", tabName);
   return 0;
}

//______________________________________________________________________________
Int_t ArgusWindow::GetActiveTabObjectIndex()
{
   for (int i = 0; i < fTabObjects->GetEntries(); i++) {
      if (static_cast<ArgusTab*>(fTabObjects->At(i))->IsTabActive()) {
         return i;
      }
   }
   Error("GetActiveTabObjectIndex", "no tab active.");
   return -1;
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
const char* ArgusWindow::GetTimeStatisticsString(ROMEString& string)
{
   int i;
   ROMEString str;
   if (fTabWindow) {
      string.SetFormatted("main window................... : %s\n", fWatchAll.GetRealTimeString(str));
   } else {
      string.SetFormatted("sub window %3d................ : %s\n", fWindowId, fWatchAll.GetRealTimeString(str));
   }
   for (i = 0; i < fTabObjects->GetEntriesFast(); i++) {
      static_cast<ArgusTab*>(fTabObjects->At(i))->GetTimeStatisticsString(str);
      string.AppendFormatted(str.Data());
   }
   for (i = 0; i < fSubWindows->GetEntriesFast(); i++) {
      if (IsSubWindowRunningAt(i)) {
         string.AppendFormatted(static_cast<ArgusWindow*>(fSubWindows->At(i))->GetTimeStatisticsString(str));
      } else {
         string.AppendFormatted(GetSubWindowTimeStringAt(i));
      }
   }
   return string.Data();
}

//______________________________________________________________________________
void ArgusWindow::RequestEventHandling()
{
   SetStatus(0, "", 0);
   fRequestEventHandling = true;
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

   fProgress->SetPosition((float)(fProgress->GetMax()*progress));
   fProgress->SetPosition(static_cast<float>(fProgress->GetMax()*progress));
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
Bool_t ArgusWindow::IsSubWindowRunningAt(Int_t i)
{
   return fSubWindowRunning->At(i) != 0;
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
   return fSubWindowTimeString->At(i).Data();
}

//______________________________________________________________________________
void ArgusWindow::SetSubWindowTimeStringAt(Int_t i, const char* timeString)
{
   fSubWindowTimeString->AddAt(timeString, i);
}
