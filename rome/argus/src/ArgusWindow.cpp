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
#include <TGTab.h>
#include <TGStatusBar.h>
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
   fArgusActive = false;
   InitArgus();
}

ArgusWindow::ArgusWindow(const TGWindow* p) : TGMainFrame(p, 1, 1)
{
   fArgusActive = true;
   InitArgus();
}

void ArgusWindow::InitArgus()
{
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
#endif
//   SafeDelete(fController); // fController can be deleted by clicking closed box.
}

Bool_t ArgusWindow::Start()
{
   ROMEPrint::Debug("ArgusWindow::Start()\n");
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

   ROMEPrint::Debug("Creating Tabs\n");
   if (!CreateTabs())
      return kFALSE;
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
