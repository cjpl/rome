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
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TArrayI.h>
#include <TGMenu.h>
#include <TGStatusBar.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

#include "ROME.h"
#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "ROMENetFolder.h"
#include "ROMEStopwatch.h"
#include "ArgusAnalyzerController.h"
#include "ArgusTab.h"

class TGTab;
class TObjArray;
class TGHProgressBar;
class ROMECompositeFrame;

class ArgusWindow : public TGMainFrame
{
protected:
   Int_t                    fWindowId;             //! Window id
   TObjArray               *fSubWindows;           //! Handles to sub windows
   TArrayI                 *fSubWindowRunning;     //! Flags running sub windows
   ROMEStrArray            *fSubWindowTimeString;  //! Run time of sub window
   Bool_t                   fTabWindow;            //! Flags Monitor with Tabs
   ROMEStopwatch            fWatchAll;             //! Records time used by window
   ROMEString               fTimeAllString;        //! Elapsed Time of all in a readable format
   Bool_t                   fArgusActive;          //! active flag
   TGStatusBar             *fStatusBar;            //! status bar
   Bool_t                   fStatusBarSwitch;      //! status bar switch
   TGHProgressBar          *fProgress;             //!
   ROMECompositeFrame      *fInfoFrame;            //! info frame
   TGMenuBar               *fMenuBar;              //! menu bar
   TGPopupMenu             *fMenuFile;             //! file menu
   TGPopupMenu             *fMenuNetFolder;        //! file menu, net folder
   TGTab                   *fTab;                  //! tabs
   ROMECompositeFrame      *fMainFrame;            //! main frame
   Int_t                    fCurrentTabID;         //! ID number of top tab
   TObjArray               *fTabObjects;           //! Handle to Tab Objects
   Bool_t                   fControllerActive;     //!
   ArgusAnalyzerController *fController;           //!
   ROMENetFolder           *fControllerNetFolder;  //!
   Float_t                  fWindowScale;          //! Window scale
   Bool_t                   fRequestEventHandling; //! Event handling request flag
   Long64_t                 fCurrentEvent;         //! Currently displayed event
   Long64_t                 fCurrentRun;           //! Currently displayed run
   Bool_t                   fUpdateButtonClicked ; //! Flag to check status of update button

   enum CommandIdentifiers{
      M_FILE_NEW_WINDOW,
      M_FILE_CONTROLLER,
      M_FILE_EXIT
   };

public:
   ArgusWindow();
   ArgusWindow(const TGWindow* p,Bool_t tabWindow=kTRUE);
   virtual ~ArgusWindow();

   Bool_t          Start();
   virtual Bool_t  CreateTabs() = 0;
   virtual Bool_t  AddMenuNetFolder(TGPopupMenu* menu) = 0;
   const char* GetTimeStatisticsString(ROMEString& string);

   // Sub Windows
   int  GetWindowId() { return fWindowId; };
   void SetWindowId(int id) { fWindowId = id; };

   Bool_t IsSubWindowRunningAt(Int_t i);
   void   SetSubWindowRunningAt(Int_t i,Bool_t running);

   const char* GetSubWindowTimeStringAt(Int_t i);
   void        SetSubWindowTimeStringAt(Int_t i,const char* timeString);

   // Analyzer Controller
   ArgusAnalyzerController   *GetAnalyzerController() { return fController; }
   Bool_t          IsControllerActive() { return fControllerActive; }
   void            SetControllerActive(bool flag) { fControllerActive = flag; }
   ROMENetFolder  *GetControllerNetFolder() { return fControllerNetFolder; }
   void            SetControllerNetFolder(const char* folderName);

   // Window Scale
   Float_t         GetWindowScale() { return fWindowScale; }
   void            SetWindowScale(Float_t scale) { fWindowScale = scale; }
   void            SetWindowScale(Char_t* scale) { Char_t* cstop; fWindowScale = (Float_t)strtod(scale,&cstop); }
   void            SetWindowScale(ROMEString& scale) { SetWindowScale((Char_t*)scale.Data()); }

   // Status bar
   void            SetStatusBarSwitch(Bool_t sw) { fStatusBarSwitch = sw; }
   Bool_t          GetStatusBarSwitch() { return fStatusBarSwitch; }
   TGStatusBar*    GetStatusBar() { return fStatusBar; }
   void            ClearStatusBar();
   void            SetStatus(Int_t mode,const char *text,double progress=0.,Int_t sleepingTime=10);

   // Event Handling
   void            RequestEventHandling();
   void            ClearEventHandlingRequest() { fRequestEventHandling = false; }
   Bool_t          IsEventHandlingRequested() { return fRequestEventHandling; }

   // Menu
   TGMenuBar*      GetMenuBar() { return fMenuBar; }
   void            CloseWindow();

   // Tabs
   void            AddTab(TObject *tab) { fTabObjects->AddLast(tab); }
   ArgusTab       *GetTabObjectAt(Int_t index) { return (ArgusTab*)fTabObjects->At(index); }
   ArgusTab       *GetTabObject(const char* tabName);
   Int_t           GetTabObjectEntries() { return fTabObjects ? fTabObjects->GetEntries() : 0; }
   // Active. This might be dangerouse because it overload TGFrame::IsActive
   Bool_t          IsActive() const { return fArgusActive; }
   Int_t           GetActiveTabObjectIndex();

   // Event Handler
   virtual void    TriggerEventHandler() = 0;

   // Event & Run Numbers
   Long64_t        GetCurrentEvent() { return fCurrentEvent; };
   void            SetCurrentEvent(Long64_t event) { fCurrentEvent = event; };
   Long64_t        GetCurrentRun() { return fCurrentRun; };
   void            SetCurrentRun(Long64_t run) { fCurrentRun = run; };

   // Update Button
   Bool_t          IsUpdateButtonClicked() const { return fUpdateButtonClicked; }
   void            SetUpdateButtonClicked(Bool_t flag) {fUpdateButtonClicked = flag; }

protected:
   void            InitArgus(Bool_t tabWindow);

   ClassDef(ArgusWindow,0) // Base class of ARGUS main window
};

#endif
