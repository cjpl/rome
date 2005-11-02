/********************************************************************
  ArgusWindow.cpp, R.Sawada

  $Id$

********************************************************************/
#include <TGMsgBox.h>
#include "ArgusWindow.h"
#include "ArgusTextDialog.h"
#include "ArgusMonitor.h"

ClassImp(ArgusWindow)

Bool_t ArgusWindow::Start()
{
   // Initialize DAQ
   if (!gArgus->DAQInit()) {
      gArgus->SetTerminationFlag();
      gArgus->Println("\n\nTerminating Program !");
      return kFALSE;
   }
   // DAQ begion of run
   if (!gArgus->DAQBeginOfRun()) {
      gArgus->SetTerminationFlag();
      gArgus->Println("\n\nTerminating Program !");
      return kFALSE;
   }
   // Create status bar
   Int_t parts[] = { 5 };
   fStatusBar = new TGStatusBar(this, 50, 10, kHorizontalFrame);
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   if (fStatusBarSwitch)
      this->AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 0, 0, 2, 0));

   // Create menu
   fMenuNetFolder = new TGPopupMenu(fClient->GetRoot());
   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->Associate(this);
   if (AddMenuNetFolder(fMenuNetFolder))
      fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   fMenuNetFolder->Associate(this);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));

   // Create tab widget
   fTab = new TGTab(this, (UInt_t) (600 * gArgus->GetWindowScale()), (UInt_t) (400 * gArgus->GetWindowScale()));

   if (!CreateTabs())
      return kFALSE;

   AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   SetWindowName(gArgus->GetProgramName());
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
   fCurrentTabID = 1;
   ProcessMessage(MK_MSG(kC_COMMAND, kCM_TAB), 0, 0);

   return kTRUE;
}

void ArgusWindow::ClearStatusBar()
{
   Int_t parts[] = { 5 };
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   fStatusBar->SetText("", 0);
}

void ArgusWindow::CloseWindow()
{
   // Disconnect
   if (!gArgus->DAQEndOfRun()) {
      gArgus->SetTerminationFlag();
      gArgus->Println("\n\nTerminating Program !");
   }
   // Terminate
   if (!gArgus->DAQTerminate()) {
      gArgus->SetTerminationFlag();
      gArgus->Println("\n\nTerminating Program !");
   }

   gArgus->GetApplication()->Terminate();
}
