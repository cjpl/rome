/********************************************************************
  ArgusWindow.cpp, R.Sawada

  $Log$
  Revision 1.1  2005/05/26 14:26:55  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.


********************************************************************/
#include <TGMsgBox.h>
#include "ArgusWindow.h"
#include "ArgusTextDialog.h"
#include "ArgusMonitor.h"

ClassImp(ArgusWindow)

Bool_t ArgusWindow::Start()
{
   // Create status bar
   Int_t parts[] = {5};
   fStatusBar = new TGStatusBar(this,50,10,kHorizontalFrame);
   fStatusBar->SetParts(parts,sizeof(parts)/sizeof(Int_t));
   if(fStatusBarSwitch)
      this->AddFrame(fStatusBar,new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX,0,0,2,0));

   // Create menu
   fMenuNetFolder = new TGPopupMenu(fClient->GetRoot());
   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->Associate(this);
   if(AddMenuNetFolder(fMenuNetFolder))
      fMenuFile->AddPopup("&Connect NetFolder", fMenuNetFolder);
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
   fMenuNetFolder->Associate(this);
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1));
   
   // Create tab widget
   fTab = new TGTab(this, (UInt_t)(600*gArgus->GetWindowScale()), (UInt_t)(400*gArgus->GetWindowScale()));
   
   if(!CreateTabs())
      return kFALSE;

   AddFrame(fTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   SetWindowName(gArgus->GetProgramName());
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
   fCurrentTabID = 1;
   ProcessMessage(MK_MSG(kC_COMMAND,kCM_TAB),0,0);

   return kTRUE;
}

void ArgusWindow::ClearStatusBar()
{
   Int_t parts[] = {5};
   fStatusBar->SetParts(parts,sizeof(parts)/sizeof(Int_t));
   fStatusBar->SetText("",0);
}

void ArgusWindow::CloseWindow()
{
#ifdef USE_TRINT
   gArgus->GetApplication()->Terminate(0);
#else 
   gArgus->GetApplication()->Terminate();
#endif
}
