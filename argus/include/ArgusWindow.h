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

#include <stdlib.h>
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

#include "ROME.h"
#include "ROMENetFolder.h"
#include "ROMEStopwatch.h"
#include "ROMEString.h"
#include "ArgusAnalyzerController.h"
#include "ArgusTab.h"

class ROMECompositeFrame;
class ROMEStrArray;
class TArrayI;
class TGHProgressBar;
class TGDockableFrame;
class TGLabel;
class TGListTree;
class TGListTreeItem;
class TGMenuBar;
class TGStatusBar;
class TGTab;
class TGTextEntry;
class TGVerticalFrame;
class TObjArray;

class ArgusWindow : public TGMainFrame
{
protected:
   Int_t                    fWindowId;             //! Window id
   TObjArray               *fSubWindows;           //! Handles to sub windows
   TArrayI                 *fSubWindowRunning;     //! Flags running sub windows
   Bool_t                   fTabWindow;            //! Flags main window
   ROMEStopwatch            fWatchAll;             //! Records time used by window
   ROMEString               fCpuTimeAllString;     //! Elapsed CPU Time of all in a readable format
   ROMEString               fRealTimeAllString;    //! Elapsed Real Time of all in a readable format
   Bool_t                   fArgusActive;          //! active flag
   TGStatusBar             *fStatusBar;            //! status bar
   Bool_t                   fStatusBarSwitch;      //! status bar switch
   TGHProgressBar          *fProgress;             //!
   TGMenuBar               *fMenuBar;              //! menu bar
   TGPopupMenu             *fMenuFile;             //! file menu
   TGLabel                 *fRunNumber;            //! run number
   TGLabel                 *fEventNumber;          //! event number
   TGLabel                 *fYearMonDay;           //! date
   TGLabel                 *fHourMinSec;           //! time
   TGVerticalFrame         *fUserInfoFrame;        //!
   TObjArray               *fUserInfoObjects;      //!
   Bool_t                   fListTreeView;         //!
   TGListTree              *fListTree;             //!
   Int_t                    fNListTreeItem;        //!
   TGListTreeItem         **fListTreeItem;         //!
   TGTab                   *fTab;                  //! tabs
   ROMECompositeFrame      *fMainFrame;            //! main frame
   Int_t                    fCurrentTabID;         //! ID number of current tab, supposed to be used only for tab-view
   TObjArray               *fTabObjects;           //! Handle to Tab Objects
   Int_t                    fCurrentTabIndex;      //! Index of the current Tab Objects
   Int_t                   *fParentIndex;          //! Index of the parent tab. -1 if no parent tab.
   Int_t                   *fNumberOfChildren;     //! Number of sub-tabs.
   TObjArray               *fTGTab;                //! array of TGTab* which holds sub-tabs
   Bool_t                   fControllerActive;     //!
   ArgusAnalyzerController *fController;           //!
   ROMENetFolder           *fControllerNetFolder;  //!
   Float_t                  fWindowScale;          //! Window scale
   UInt_t                   fInitialWidth;         //! Default width of the window at startup
   UInt_t                   fInitialHeight;        //! Default height of the window at startup
   Bool_t                   fRequestEventHandling; //! Event handling request flag
   Bool_t                   fForceEventHandling;   //! Force event handling even if it is the same event
   ULong_t                  fScreenShotPeriod;     //!
   ULong_t                  fScreenShotLastTime;   //!
   TString                  fTimeZone;             //!

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

   Bool_t           Start();
   void             CloseWindow();

   // Sub Windows
private:
   virtual ArgusWindow *CreateSubWindow() = 0;
public:
   void             SetWindowId(int id) { fWindowId = id; }
   int              GetWindowId() const { return fWindowId; }
   void             SetSubWindowRunningAt(Int_t i, Bool_t running);
   Bool_t           IsSubWindowRunningAt(Int_t i);

   // Stopwatch
   const char*      GetTimeStatisticsString(ROMEString& string);

   // Status bar
   void             SetStatusBarSwitch(Bool_t sw) { fStatusBarSwitch = sw; }
   Bool_t           GetStatusBarSwitch() const { return fStatusBarSwitch; }
   TGStatusBar     *GetStatusBar() const { return fStatusBar; }
   void             SetStatus(Int_t mode,const char *text,double progress=0.,Int_t sleepingTime=10);
   void             ClearStatusBar();

   // Info frame
   TGVerticalFrame *GetUserInfoFrame() const { return fUserInfoFrame; }
   Int_t            GetUserInfoObjectEntriesFast() const { return fUserInfoObjects ? fUserInfoObjects->GetEntriesFast() : 0; }
//   void             GetUserInfoObjectAddAt(TObject *obj, Int_t idx) { fUserInfoObjects->AddAt(obj, idx); }
//   TObject         *GetUserInfoObjectAt(Int_t idx) const { return fUserInfoObjects->At(idx); }
   TObjArray       *GetUserInfoObjects() const { return fUserInfoObjects; }

   // Menu
   TGMenuBar*       GetMenuBar() const { return fMenuBar; }
   virtual Bool_t   AddMenuNetFolder(TGPopupMenu* menu) = 0;

   // ListTree
   void             SetListTreeView(Bool_t sw) { fListTreeView = sw; }
   Bool_t           IsListTreeView() const { return fListTreeView; }
   void             OnClick(TGListTreeItem* item, Int_t btn);
   void             OnDoubleClick(TGListTreeItem* item, Int_t btn);
private:
   Int_t            GetSelectedItemIndex(TGListTreeItem*) const;

   // Tab/ListTree methods
public:
   void             AddTab(TObject *tab) { fTabObjects->AddLast(tab); }
   Int_t            GetTabObjectEntries() const { return fTabObjects ? fTabObjects->GetEntries() : 0; }
   Int_t            GetTabObjectEntriesFast() const { return fTabObjects ? fTabObjects->GetEntriesFast() : 0; }
   ArgusTab        *GetTabObjectAt(Int_t index) const { return static_cast<ArgusTab*>(fTabObjects->At(index)); }
   Int_t            GetCurrentTabObjectIndex() const { return fCurrentTabIndex; }
private:
   ArgusTab        *GetTabObject(const char* tabTitle) const;
   ArgusTab        *GetTabObject(const int id) const;
   Int_t            GetTabObjectIndex(const int id) const;
   Bool_t           CreateTabs();
   void             TriggerTabSelected(Int_t index);
   void             TriggerTabUnSelected(Int_t index);

   // Analyzer Controller
public:   
   ArgusAnalyzerController *GetAnalyzerController() const { return fController; }
   void             SetControllerActive(bool flag) { fControllerActive = flag; }
   Bool_t           IsControllerActive() const { return fControllerActive; }
   void             SetControllerNetFolder(const char* folderName);
   ROMENetFolder   *GetControllerNetFolder() const { return fControllerNetFolder; }

   // Window Scale
   Float_t          GetWindowScale() const { return fWindowScale; }
   void             SetWindowScale(Float_t scale) { fWindowScale = scale; }
   void             SetWindowScale(const char* scale) { Char_t* cstop; fWindowScale = static_cast<Float_t>(strtod(scale,&cstop)); }
   void             SetWindowScale(ROMEString& scale) { SetWindowScale(scale.Data()); }

   //
   TGDimension      GetInitialSize() const { return TGDimension(fInitialWidth, fInitialHeight); }

   // Screen shot
   void             SetScreenShotPeriod(Int_t period) { fScreenShotPeriod = period; }
   Int_t            GetScreenShotPeriod() const { return fScreenShotPeriod; }

   // Event Handling
public:
   void             RequestEventHandling();
   Bool_t           IsEventHandlingRequested() const { return fRequestEventHandling; }
   void             ForceEventHandling() { fForceEventHandling = true; }
   Bool_t           IsEventHandlingForced() const { return fForceEventHandling; }
private:
   void             ClearEventHandlingRequest() { fRequestEventHandling = false; }
   void             ClearEventHandlingForced() { fForceEventHandling = false; }

   // Event Handler
public:
   void             TriggerEventHandler();

   // Message handling
   Bool_t           ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
protected:
   virtual Bool_t   ProcessMessageNetFolder(Long_t param1) = 0;
   Bool_t           OpenNewWindow(Int_t tabIndex);
   void             ChangeTab(Int_t tabIndex);

   // Active. This might be dangerouse because it overload TGFrame::IsActive
public:
   Bool_t           IsActive() const { return fArgusActive; }
//   Int_t            GetActiveTabObjectIndex();
   void             CheckActiveFlags();

   void             SetTimeZone(const char* timezone) { fTimeZone = timezone; fTimeZone.ToLower(); }
   const char*      GetTimeZone() { return fTimeZone; }

   ClassDef(ArgusWindow,0) // Base class of ARGUS main window
};

#endif
