/********************************************************************
  ArgusTab.h, M. Schneebeli PSI

  $Id: ArgusTab.h 1000 2006-04-03 05:54:56Z schneebeli_m $

********************************************************************/
#ifndef ArgusTab_H
#define ArgusTab_H

#include "RConfig.h"
#pragma warning( push )
#pragma warning( disable : 4800 )
#include "TGFrame.h"
#include "TTimer.h"
#pragma warning( pop )

class ArgusTab : public TGCompositeFrame {

protected:
   Bool_t   fActive;               //! is Active
   Int_t    fUpdateFrequency;      //! Update Frequency
   TTimer  *fEventHandlerTimer;    //! Timer for the EventHandler function
   Bool_t   fEventHandlerUserStop; //! True if the user stopped the EventHandler
public:
   ArgusTab();
   ~ArgusTab() {};

   void InitTab();
   virtual void Init() = 0;
   void TabEventHandler();
   virtual void EventHandler() = 0;
   void StartEventHandler(Int_t milliSeconds);
   void StartEventHandler();
   void StopEventHandler();

   // User Methods
   Bool_t    GetActive() { return fActive; };
   void      SetActive(Bool_t active) { fActive = active; };
   Int_t     GetUpdateFrequency();
   void      SetUpdateFrequency(Int_t duration);

   ClassDef(ArgusTab,1)
};

#endif   // ArgusTab_H
