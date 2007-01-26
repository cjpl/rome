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
#include "ROMEString.h"
#include "ArgusWindow.h"
#include "ArgusTab.h"
#include "ArgusAnalyzerController.h"
#include "ArgusTextDialog.h"
#include "ROMECompositeFrame.h"
#include "ROMEAnalyzer.h"

ClassImp(ArgusWindow)

ArgusWindow::ArgusWindow()
{
   InitArgus(true);
}

ArgusWindow::ArgusWindow(const TGWindow* p,Bool_t tabWindow) : TGMainFrame(p, 1, 1)
{
   InitArgus(tabWindow);
}

void ArgusWindow::InitArgus(Bool_t tabWindow)
{
   fWindowId = -1;
   fTabWindow = tabWindow;
   fRequestEventHandling = false;
   fArgusActive = false;
   fWindowScale = 1;
   fStatusBarSwitch = true;
   fControllerActive = false;
   fControllerNetFolder = NULL;

   fStatusBar = 0;
   fMenuBar = 0;
   fMenuFile = 0;
   fMenuNetFolder = 0;
   fTab = 0;
   fTabObjects = 0;
   fController = 0;
   fControllerNetFolder = 0;
   fProgress = 0;
   fCurrentEvent = -1;
   fCurrentRun = -1;

   fSubWindows = new TObjArray(0);
   fSubWindowRunning = new TArrayI();
   fSubWindowTimeString = new ROMEStrArray();

   fWatchAll.Reset();
}

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
   SafeDelete(fSubWindowRunning);
   SafeDelete(fSubWindowTimeString);
#endif
//   SafeDelete(fController); // fController can be deleted by clicking closed box.
}

Bool_t ArgusWindow::Start()
{
   // Start Argus Window
   fWatchAll.Start(false);
   ROMEPrint::Debug("ArgusWindow::Start()\n");
   // Initialize Analyzer Controller
   if (fControllerActive)
      fController = new ArgusAnalyzerController(gClient->GetRoot(),this,100,100,fControllerNetFolder);

   // Create status bar
   Int_t parts[2] = { 70,30 };
   fStatusBar = new TGStatusBar(this, 50, 10, kHorizontalFrame);
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   fProgress = new TGHProgressBar(fStatusBar->GetBarPart(1),TGProgressBar::kStandard,20);
   fProgress->SetPosition(0);
   fStatusBar->GetBarPart(1)->AddFrame(fProgress, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 10, 10));
   if (fStatusBarSwitch)
      this->AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 0, 0, 2, 0));

   // Create menu
   fMenuNetFolder = new TGPopupMenu (fClient->GetRoot());
   fMenuFile = new TGPopupMenu (fClient->GetRoot());
   fMenuFile->Associate(this);
   if (fTabWindow) {
      fMenuFile->AddEntry("New Window", M_FILE_NEW_WINDOW);
   }
   if (AddMenuNetFolder(fMenuNetFolder))
      fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
   fMenuFile->AddEntry("Start C&ontroller", M_FILE_CONTROLLER);
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   fMenuNetFolder->Associate(this);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));

   // Create info frame
   if (fTabWindow) {
//   fInfoFrame = new ROMECompositeFrame(this,100,0,kRaisedFrame | kVerticalFrame | kHorizontalFrame);
//   this->AddFrame(fInfoFrame,new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0,0,0,0));
   }

   // Create tab widget
   if (fTabWindow) {
      fTab = new TGTab(this, static_cast<UInt_t>(600 * GetWindowScale()), static_cast<UInt_t>(400 * GetWindowScale()));
      AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   }
   else {
      fMainFrame = new ROMECompositeFrame(this, static_cast<UInt_t>(600 * GetWindowScale()), static_cast<UInt_t>(400 * GetWindowScale()));
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
   ProcessMessage(MK_MSG(kC_COMMAND, kCM_TAB), 0, 0);

   ROMEPrint::Debug("End of ArgusWindow::Start()\n");
   fWatchAll.Stop();
   return kTRUE;
}

void ArgusWindow::ClearStatusBar()
{
   Int_t parts[2] = { 70,30 };
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   fStatusBar->SetText("", 0);
}

void ArgusWindow::CloseWindow()
{
   gROME->WindowClosed();
}

ArgusTab* ArgusWindow::GetTabObject(const char* tabName)
{
   for (int i=0;i<fTabObjects->GetEntries();i++) {
      if (!strcmp(tabName,((ArgusTab*)fTabObjects->At(i))->GetTitle()))
         return (ArgusTab*)fTabObjects->At(i);
   }
   Error("GetTabObject", "%s was not found.", tabName);
   return 0;
}

Int_t ArgusWindow::GetActiveTabObjectIndex()
{
   for (int i=0;i<fTabObjects->GetEntries();i++) {
      if (((ArgusTab*)fTabObjects->At(i))->IsTabActive())
         return i;
   }
   Error("GetActiveTabObjectIndex", "no tab active.");
   return -1;
}

void ArgusWindow::SetControllerNetFolder(const char* folderName)
{
   if (!gROME->IsNetFolderActive(folderName))
      return;
   fControllerNetFolder = gROME->GetNetFolder(folderName);
}

const char* ArgusWindow::GetTimeStatisticsString(ROMEString& string)
{
   int i;
   ROMEString str;
   if (fTabWindow)
      string.SetFormatted("main window................... : %s\n", fWatchAll.GetRealTimeString(str));
   else
      string.SetFormatted("sub window %3d................ : %s\n", fWindowId, fWatchAll.GetRealTimeString(str));
   for (i=0;i<fTabObjects->GetEntriesFast();i++) {
      ((ArgusTab*)fTabObjects->At(i))->GetTimeStatisticsString(str);
      string.AppendFormatted(str.Data());
   }
   for (i=0;i<fSubWindows->GetEntriesFast();i++) {
      if (IsSubWindowRunningAt(i)) {
         string.AppendFormatted(((ArgusWindow*)fSubWindows->At(i))->GetTimeStatisticsString(str));
      }
      else {
         string.AppendFormatted(GetSubWindowTimeStringAt(i));
      }
   }
   return string.Data();
}

void ArgusWindow::RequestEventHandling()
{
   SetStatus(0,"",0);
   fRequestEventHandling = true;
}

void ArgusWindow::SetStatus(Int_t mode,const char *text,double progress,Int_t sleepingTime)
{
   // Set status bar
   // mode 0 : initialization
   // mode 1 : set progress
   // mode 2 : finish

   fProgress->SetPosition((float)(fProgress->GetMax()*progress));
   fStatusBar->SetText(text);

   if (mode==0) {
      fStatusBar->GetBarPart(0)->SetBackgroundColor(TColor::RGB2Pixel(255,150,150));
//      fStatusBar->Layout();
      fProgress->Reset();
   }
   if (mode==2) {
      fStatusBar->GetBarPart(0)->SetBackgroundColor(GetDefaultFrameBackground());
   }
   gSystem->ProcessEvents();
   gSystem->Sleep(sleepingTime);
   return;
}

Bool_t ArgusWindow::IsSubWindowRunningAt(Int_t i)
{
   return fSubWindowRunning->At(i)!=0;
}

void ArgusWindow::SetSubWindowRunningAt(Int_t i,Bool_t running)
{
   if (fSubWindowRunning->GetSize()<=i) {
      if (fSubWindowRunning->GetSize()*2>i)
         fSubWindowRunning->Set(fSubWindowRunning->GetSize()*2);
      else
         fSubWindowRunning->Set(i+1);
   }
   fSubWindowRunning->AddAt((Int_t)running,i);
}

const char* ArgusWindow::GetSubWindowTimeStringAt(Int_t i)
{
   return fSubWindowTimeString->At(i).Data();
}

void ArgusWindow::SetSubWindowTimeStringAt(Int_t i,const char* timeString)
{
   fSubWindowTimeString->AddAt(timeString,i);
}
