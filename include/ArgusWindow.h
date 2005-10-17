/********************************************************************
  ArgusWindow.h, R.Sawada

  $Id:$

********************************************************************/
#ifndef ArgusWindow_H
#define ArgusWindow_H

#include <TGMenu.h>
#include <TGTab.h>
#include <TGStatusBar.h>

class ArgusWindow : public TGMainFrame 
{
protected:
   TGStatusBar* fStatusBar;        //! status bar
   Bool_t       fStatusBarSwitch;  //! status bar switch
   TGMenuBar*   fMenuBar;          //! menu bar
   TGPopupMenu* fMenuFile;         //! file menu
   TGPopupMenu* fMenuNetFolder;    //! file menu, net folder
   TGTab*       fTab;              //! tabs
   Int_t        fCurrentTabID;     //! ID number of top tab

   enum CommandIdentifiers{
      M_FILE_EXIT
   };
      
public:
   ArgusWindow() {}
   ArgusWindow(const TGWindow* p, Char_t* title) {}
   virtual ~ArgusWindow() {};
   Bool_t         Start();
   virtual Bool_t CreateTabs() = 0;
   virtual Bool_t AddMenuNetFolder(TGPopupMenu* menu) = 0;

   // Status bar
   void           SetStatusBarSwitch(Bool_t sw) { fStatusBarSwitch = sw; };
   Bool_t         GetStatusBarSwitch() { return fStatusBarSwitch; };
   TGStatusBar*   GetStatusBar() { return fStatusBar; };
   
   // Menu
   void           CloseWindow();
   void           ClearStatusBar();
   
   ClassDef(ArgusWindow,1)
};

#endif
