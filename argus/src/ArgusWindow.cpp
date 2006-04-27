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
#pragma warning( disable : 4800 )
#endif                          // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TGMsgBox.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif                          // R__VISUAL_CPLUSPLUS
#include "ArgusWindow.h"
#include "ArgusTextDialog.h"
#include "ROMEAnalyzer.h"

ClassImp(ArgusWindow)
ArgusWindow::ArgusWindow(const TGWindow* p, Char_t* title) : TGMainFrame(p, 1, 1)
{
   fWindowScale = 1;
   fStatusBarSwitch = true;
   fControllerActive = false;
   fControllerNetFolder = NULL;
   fUpdateFrequency = 0;
}

Bool_t ArgusWindow::Start()
{
   // Initialize Analyzer Controller
   if (fControllerActive)
      fController = new ArgusAnalyzerController(gClient->GetRoot(),this,100,100,fControllerNetFolder);

   // Create status bar
   Int_t parts[] = { 5 };
   fStatusBar = new TGStatusBar(this, 50, 10, kHorizontalFrame);
   fStatusBar->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   if (fStatusBarSwitch)
      this->AddFrame(fStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 0, 0, 2, 0));

   // Create menu
   fMenuNetFolder = new TGPopupMenu (fClient->GetRoot());
   fMenuFile = new TGPopupMenu (fClient->GetRoot());
   fMenuFile->Associate(this);
   if (AddMenuNetFolder(fMenuNetFolder))
      fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   fMenuNetFolder->Associate(this);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));

   // Create tab widget
   fTab = new TGTab(this, static_cast<UInt_t>(600 * GetWindowScale()), static_cast<UInt_t>(400 * GetWindowScale()));

   if (!CreateTabs())
      return kFALSE;

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
   gROME->WindowClosed();
}
ArgusTab* ArgusWindow::GetTabObject(const char* tabName) 
{ 
   for (int i=0;i<fTabObjects->GetEntries();i++) {
      cout << ((ArgusTab*)fTabObjects->At(i))->GetName() << endl;
      return (ArgusTab*)fTabObjects->At(i);
   }
};
