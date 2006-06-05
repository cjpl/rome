// $Id$
// Author: Ryu Sawada
#ifndef ArgusWindow_H
#define ArgusWindow_H


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
#endif // R__VISUAL_CPLUSPLUS
#include <TGMenu.h>
#include <TGTab.h>
#include <TGStatusBar.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <ArgusAnalyzerController.h>
#include <ArgusTab.h>
#include <ROMEAnalyzer.h>
#include <ROMEString.h>

class ArgusWindow : public TGMainFrame 
{
protected:
   Bool_t       fArgusActive;      //! active flag
   TGStatusBar* fStatusBar;        //! status bar
   Bool_t       fStatusBarSwitch;  //! status bar switch
   int          fUpdateFrequency;  //! update frequency
   TGMenuBar*   fMenuBar;          //! menu bar
   TGPopupMenu* fMenuFile;         //! file menu
   TGPopupMenu* fMenuNetFolder;    //! file menu, net folder
   TGTab*       fTab;              //! tabs
   Int_t        fCurrentTabID;     //! ID number of top tab
   TObjArray   *fTabObjects;       //! Handle to Tab Objects

   // Analyzer Controller
   bool fControllerActive;
   ArgusAnalyzerController *fController;
   ROMENetFolder *fControllerNetFolder;


   // Window
   Float_t          fWindowScale;                  //! Window scale
   
   enum CommandIdentifiers{
      M_FILE_EXIT
   };
      
public:
   ArgusWindow(); 
   ArgusWindow(const TGWindow* p, Char_t* title);
   virtual ~ArgusWindow() {};
   Bool_t         Start();
   virtual Bool_t CreateTabs() = 0;
   virtual Bool_t AddMenuNetFolder(TGPopupMenu* menu) = 0;

   // Analyzer Controller
   ArgusAnalyzerController *GetAnalyzerController() { return fController; };
   bool IsControllerActive() { return fControllerActive; };
   void SetControllerActive(bool flag) { fControllerActive = flag; };
   ROMENetFolder *GetControllerNetFolder() { return fControllerNetFolder; };
   void        SetControllerNetFolder(const char* folderName) { if (!gROME->IsNetFolderActive(folderName)) return; fControllerNetFolder = gROME->GetNetFolder(folderName); };

   // Window Scale
   Float_t         GetWindowScale() { return fWindowScale; }
   void            SetWindowScale(Float_t scale) { fWindowScale = scale; }
   void            SetWindowScale(Char_t* scale) { Char_t* cstop; fWindowScale = (Float_t)strtod(scale,&cstop); }
   void            SetWindowScale(ROMEString& scale) { SetWindowScale((Char_t*)scale.Data()); }
   
   // Status bar
   void           SetStatusBarSwitch(Bool_t sw) { fStatusBarSwitch = sw; };
   Bool_t         GetStatusBarSwitch() { return fStatusBarSwitch; };
   TGStatusBar*   GetStatusBar() { return fStatusBar; };

   // Update Frequency
   void           SetUpdateFrequency(Int_t duration)
   {
      fUpdateFrequency = duration;
      if (fArgusActive) {
         ArgusTab *tab;
         for (int i=0;i<GetTabObjectEntries();i++) {
            tab = GetTabObjectAt(i);
            if (tab)
               tab->SetUpdateFrequency(duration);
         }
      }
   };
   Int_t          GetUpdateFrequency() { return fUpdateFrequency; };
   
   // Menu
   TGMenuBar*     GetMenuBar() { return fMenuBar; };
   void           CloseWindow();
   void           ClearStatusBar();
   
   // Tabs
   void            AddTab(ArgusTab *tab) { fTabObjects->AddLast(tab); };
   ArgusTab       *GetTabObjectAt(Int_t index) { return (ArgusTab*)fTabObjects->At(index); };
   ArgusTab       *GetTabObject(const char* tabName);
   Int_t           GetTabObjectEntries() {
      if(fTabObjects)
         return fTabObjects->GetEntries();
      return 0;
   };
   // Active
   Bool_t IsActive() { return fArgusActive; };

   // Event Handler
   virtual void TriggerEventHandler() = 0;
protected:
   void InitArgus();

   ClassDef(ArgusWindow,1) // Base class of ARGUS main window
};

#endif
