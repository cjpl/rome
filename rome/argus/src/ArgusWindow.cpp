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
#include <TGMsgBox.h>
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
#include "ROMEAnalyzer.h"

ClassImp(ArgusWindow)
ArgusWindow::ArgusWindow()
{
   InitArgus();
}

ArgusWindow::ArgusWindow(const TGWindow* p) : TGMainFrame(p, 1, 1)
{
   InitArgus();
}

void ArgusWindow::InitArgus()
{
   fRequestEventHandling = false;
   fArgusActive = false;
   fWindowScale = 1;
   fStatusBarSwitch = true;
   fControllerActive = false;
   fControllerNetFolder = NULL;
   fUpdateFrequency = 0;

   fStatusBar = 0;
   fMenuBar = 0;
   fMenuFile = 0;
   fMenuNetFolder = 0;
   fTab = 0;
   fTabObjects = 0;
   fController = 0;
   fControllerNetFolder = 0;
   fProgress = 0;

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
   if (AddMenuNetFolder(fMenuNetFolder))
      fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
   fMenuFile->AddEntry("Start C&ontroller", M_FILE_CONTROLLER);
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   fMenuNetFolder->Associate(this);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));

   // Create tab widget
   fTab = new TGTab(this, static_cast<UInt_t>(600 * GetWindowScale()), static_cast<UInt_t>(400 * GetWindowScale()));

   ROMEPrint::Debug("Creating Tabs\n");
   if (!CreateTabs()) {
      fWatchAll.Stop();
      return kFALSE;
   }
   ROMEPrint::Debug("Tabs Created\n");

   AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   ROMEString name;
   name.SetFormatted("ARGUS - %s", gROME->GetProgramName());
   SetWindowName(name.Data());
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

void ArgusWindow::SetControllerNetFolder(const char* folderName)
{
   if (!gROME->IsNetFolderActive(folderName))
      return;
   fControllerNetFolder = gROME->GetNetFolder(folderName);
}

// Time methods
void ArgusWindow::ShowTimeStatistics()
{
   int i;
   ROMEString str;
   ROMEPrint::Print("window........................ : %s\n", fWatchAll.GetRealTimeString(str));
   for (i=0;i<fTabObjects->GetEntriesFast();i++) {
      ((ArgusTab*)fTabObjects->At(i))->ShowTimeStatistics();
   }
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
   gROME->GetWindow()->GetStatusBar()->SetText(text);

   if (mode==0) {
      gROME->GetWindow()->GetStatusBar()->GetBarPart(0)->SetBackgroundColor(TColor::RGB2Pixel(255,150,150));
//      gROME->GetWindow()->GetStatusBar()->Layout();
      fProgress->Reset();
   }
   if (mode==2) {
      gROME->GetWindow()->GetStatusBar()->GetBarPart(0)->SetBackgroundColor(GetDefaultFrameBackground());
   }
   gSystem->ProcessEvents();
   gSystem->Sleep(sleepingTime);
   return;
}

