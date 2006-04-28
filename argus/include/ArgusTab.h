/********************************************************************
  ArgusTab.h, M. Schneebeli PSI

  $Id: ArgusTab.h 1000 2006-04-03 05:54:56Z schneebeli_m $

********************************************************************/
#ifndef ArgusTab_H
#define ArgusTab_H

#include "RConfig.h"
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include "TGFrame.h"
#include "TTimer.h"
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif
#include "ROMEString.h"

class ArgusTab : public TGCompositeFrame {

protected:
   ROMEString fTitle;              //! Tab Title
   Bool_t   fActive;               //! is Active
   Int_t    fUpdateFrequency;      //! Update Frequency
   TTimer  *fEventHandlerTimer;    //! Timer for the EventHandler function
   TTimer  *fEventHandlerWaitTimer;//! Timer for the EventHandler function while waiting
   Bool_t   fEventHandlerUserStop; //! True if the user stopped the EventHandler
   Bool_t   fBusy;                 //! Busy flag
public:
   ArgusTab();
   ~ArgusTab() {};

   void InitTab();
   virtual void Init() = 0;
   void TabEventHandler();
   virtual void BaseTabEventHandler() = 0;
   virtual void EventHandler() = 0;
   void StartEventHandler(Int_t milliSeconds);
   void StartEventHandler();
   void StopEventHandler();

   // User Methods
   const char* GetTitle() { return fTitle.Data(); };
   void      SetTitle(const char* title) { fTitle = title; };
   Bool_t    GetActive() { return fActive; };
   void      SetActive(Bool_t active) { fActive = active; };
   Int_t     GetUpdateFrequency();
   void      SetUpdateFrequency(Int_t duration);
   Bool_t    IsBusy() { return fBusy; };

   ClassDef(ArgusTab,1)
};

#endif   // ArgusTab_H
