/********************************************************************
  ArgusTab.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ArgusTab_H
#define ArgusTab_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif
#include "ROME.h"
#include "ROMEString.h"
#include "ROMEStopwatch.h"

class ArgusTab : public TGCompositeFrame {

protected:
   ROMEString     fTitle;            //! Tab Title
   Bool_t         fActive;           //! is Active
   Bool_t         fBusy;             //! Busy flag
   ROMEStopwatch  fWatchAll;         //! Records time used by tab
   ROMEString     fTimeAllString;    //! Elapsed Time of all in a readable format
   ROMEStopwatch  fWatchUser;        //! Records time used by user
   ROMEString     fTimeUserString;   //! Elapsed Time of user in a readable format
   ROMEStopwatch  fWatchUserEvent;   //! Records time used by user events
   ROMEString     fTimeUserEventString;//! Elapsed Time of user events in a readable format

public:
   ArgusTab();
   virtual ~ArgusTab();

   void ArgusInit();
   void ArgusEventHandler();

   void        ShowTimeStatistics();
   const char *GetTimeOfAll();
   const char *GetTimeOfUser();
   const char *GetTimeOfUserEvents();

   // User Methods
   const char* GetTitle() const { return fTitle.Data(); }
   void        SetTitle(const char* title) { fTitle = title; }
   Bool_t      IsActive() const { return fActive; } // This might be dangerouse because it overload TGFrame::IsActive
   void        SetActive(Bool_t active) { fActive = active; }
   Bool_t      IsBusy() { return fBusy; }

   virtual void BaseInit() = 0;
   virtual void Init() = 0;
   virtual void EndInit() = 0;
   virtual void BaseEventHandler() = 0;
   virtual void EventHandler() = 0;

   ClassDef(ArgusTab,0) // Base class of ARGUS tabs
};

#endif   // ArgusTab_H
