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
#include "ROMENetFolder.h"
#include "ROMEStopwatch.h"
#include "ROMEString.h"
#include "ArgusAnalyzerController.h"
#include "ArgusTab.h"

class TGTab;
class TObjArray;
class TGHProgressBar;
class TGTextEntry;
class TGVerticalFrame;
class ROMECompositeFrame;
class ROMEStrArray;

class ArgusWindow : public TGMainFrame
{
protected:
   Int_t                    fWindowId;             //! Window id
   TObjArray               *fSubWindows;           //! Handles to sub windows
   TArrayI                 *fSubWindowRunning;     //! Flags running sub windows
   ROMEStrArray            *fSubWindowTimeString;  //! Run time of sub window
   Bool_t                   fTabWindow;            //! Flags Monitor with Tabs
   ROMEStopwatch            fWatchAll;             //! Records time used by window
   ROMEString               fCpuTimeAllString;     //! Elapsed CPU Time of all in a readable format
   ROMEString               fRealTimeAllString;    //! Elapsed Real Time of all in a readable format
   Bool_t                   fArgusActive;          //! active flag
   TGStatusBar             *fStatusBar;            //! status bar
   Bool_t                   fStatusBarSwitch;      //! status bar switch
   TGHProgressBar          *fProgress;             //!
   TGHorizontalFrame       *fInfoFrame;            //! info frame
   TGMenuBar               *fMenuBar;              //! menu bar
   TGPopupMenu             *fMenuFile;             //! file menu
   TGPopupMenu             *fMenuNetFolder;        //! file menu, net folder
   TGLabel                 *fRunEventNumber;       //! run number and event number
   TGTab                   *fTab;                  //! tabs
   ROMECompositeFrame      *fMainFrame;            //! main frame
   Int_t                    fCurrentTabID;         //! ID number of current tab
   TObjArray               *fTabObjects;           //! Handle to Tab Objects
   Int_t                   *fParentIndex;          //! Index of the parent tab. -1 if no parent tab.
   Int_t                   *fNumberOfChildren;     //! Number of sub-tabs.
   TObjArray               *fTGTab;                //! array of TGTab* which holds sub-tabs
   Bool_t                   fControllerActive;     //!
   ArgusAnalyzerController *fController;           //!
   ROMENetFolder           *fControllerNetFolder;  //!
   Float_t                  fWindowScale;          //! Window scale
   Bool_t                   fRequestEventHandling; //! Event handling request flag
   Bool_t                   fForceEventHandling;   //! Force event handling even if it is the same event

public:
   enum CommandIdentifiers {
      M_FILE_NEW_WINDOW,
      M_FILE_CONTROLLER,
      M_FILE_EXIT
   };

private:
   ArgusWindow(const ArgusWindow &window); // not implemented
   ArgusWindow &operator=(const ArgusWindow &rhs); // not implemented

public:
   ArgusWindow(Bool_t statusBarSwitch = kTRUE, Int_t numberOfTabs = 0);
   ArgusWindow(const TGWindow* p, Bool_t statusBarSwitch = kTRUE, Int_t numberOfTabs = 0, Bool_t tabWindow = kTRUE);
   virtual ~ArgusWindow();

   Bool_t          Start();
   void            CloseWindow();

   // Sub Windows
private:
   virtual ArgusWindow *CreateSubWindow() = 0;
public:
   void            SetWindowId(int id) { fWindowId = id; };
   int             GetWindowId() const { return fWindowId; };
   void            SetSubWindowRunningAt(Int_t i, Bool_t running);
   Bool_t          IsSubWindowRunningAt(Int_t i);

   // Stopwatch
   void            SetSubWindowTimeStringAt(Int_t i, const char* timeString);
   const char*     GetSubWindowTimeStringAt(Int_t i);
   const char*     GetTimeStatisticsString(ROMEString& string);

   // Status bar
   void            SetStatusBarSwitch(Bool_t sw) { fStatusBarSwitch = sw; }
   Bool_t          GetStatusBarSwitch() const { return fStatusBarSwitch; }
   TGStatusBar    *GetStatusBar() const { return fStatusBar; }
   void            SetStatus(Int_t mode,const char *text,double progress=0.,Int_t sleepingTime=10);
   void            ClearStatusBar();

   // Menu
   TGMenuBar*      GetMenuBar() const { return fMenuBar; }
   virtual Bool_t  AddMenuNetFolder(TGPopupMenu* menu) = 0;

   // Tab methods
   void            AddTab(TObject *tab) { fTabObjects->AddLast(tab); }
   ArgusTab       *GetTabObjectAt(Int_t index) const { return static_cast<ArgusTab*>(fTabObjects->At(index)); }
   ArgusTab       *GetTabObject(const char* tabTitle) const;
   ArgusTab       *GetTabObject(const int id) const;
   Int_t           GetTabObjectEntries() const { return fTabObjects ? fTabObjects->GetEntries() : 0; }
   Int_t           GetTabObjectEntriesFast() const { return fTabObjects ? fTabObjects->GetEntriesFast() : 0; }
   Int_t           GetCurrentTabObjectIndex() const;
private:
   Bool_t          CreateTabs();

public:   
   // Analyzer Controller
   ArgusAnalyzerController *GetAnalyzerController() const { return fController; }
   void            SetControllerActive(bool flag) { fControllerActive = flag; }
   Bool_t          IsControllerActive() const { return fControllerActive; }
   void            SetControllerNetFolder(const char* folderName);
   ROMENetFolder  *GetControllerNetFolder() const { return fControllerNetFolder; }

   // Window Scale
   Float_t         GetWindowScale() const { return fWindowScale; }
   void            SetWindowScale(Float_t scale) { fWindowScale = scale; }
   void            SetWindowScale(const char* scale) { Char_t* cstop; fWindowScale = static_cast<Float_t>(strtod(scale,&cstop)); }
   void            SetWindowScale(ROMEString& scale) { SetWindowScale(scale.Data()); }

   // Event Handling
   void            RequestEventHandling();
   void            ClearEventHandlingRequest() { fRequestEventHandling = false; }
   Bool_t          IsEventHandlingRequested() const { return fRequestEventHandling; }
   void            ForceEventHandling() { fForceEventHandling = true; }
   void            ClearEventHandlingForced() { fForceEventHandling = false; }
   Bool_t          IsEventHandlingForced() const { return fForceEventHandling; }

   // Event Handler
   void            TriggerEventHandler();

   // Message handling
   Bool_t          ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
protected:
   virtual Bool_t  ProcessMessageNetFolder(Long_t param1) = 0;

public:
   // Active. This might be dangerouse because it overload TGFrame::IsActive
   Bool_t          IsActive() const { return fArgusActive; }
//   Int_t           GetActiveTabObjectIndex();
   void            CheckActiveFlags();

   ClassDef(ArgusWindow,0) // Base class of ARGUS main window
};

#endif
