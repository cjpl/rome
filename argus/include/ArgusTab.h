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
#include "ROMEString.h"

class TTimer;

class ArgusTab : public TGCompositeFrame {

protected:
   ROMEString fTitle;              //! Tab Title
   Bool_t     fActive;               //! is Active
   Int_t      fUpdateFrequency;      //! Update Frequency
   TTimer    *fEventHandlerTimer;    //! Timer for the EventHandler function
   TTimer    *fEventHandlerWaitTimer;//! Timer for the EventHandler function while waiting
   Bool_t     fEventHandlerUserStop; //! True if the user stopped the EventHandler
   Bool_t     fBusy;                 //! Busy flag

public:
   ArgusTab();
   ~ArgusTab() {}

   void ArgusInit();
   void ArgusEventHandler();
   void StartEventHandler(Int_t milliSeconds);
   void StartEventHandler();
   void StopEventHandler();

   // User Methods
   const char* GetTitle() { return fTitle.Data(); }
   void        SetTitle(const char* title) { fTitle = title; }
   Bool_t      IsActive() { return fActive; }
   void        SetActive(Bool_t active) { fActive = active; }
   Int_t       GetUpdateFrequency();
   void        SetUpdateFrequency(Int_t duration);
   Bool_t      IsBusy() { return fBusy; }

   virtual void BaseInit() = 0;
   virtual void Init() = 0;
   virtual void EndInit() = 0;
   virtual void BaseEventHandler() = 0;
   virtual void EventHandler() = 0;

   ClassDef(ArgusTab,0) // Base class of ARGUS tabs
};

#endif   // ArgusTab_H
