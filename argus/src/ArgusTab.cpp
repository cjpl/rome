// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusTab                                                             //
//                                                                      //
//  The Base Class for all Tabs in ARGUS.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h> 
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <ArgusTab.h>
#include <ROMEAnalyzer.h>
#include "Riostream.h"

ClassImp(ArgusTab)

ArgusTab::ArgusTab():TGCompositeFrame(NULL,1,1) {
   fActive  = kFALSE;
   fUpdateFrequency  = 0;
   fEventHandlerUserStop  = false;
   fEventHandlerTimer = NULL;
   fEventHandlerWaitTimer = NULL;
   fBusy = false;
}

void ArgusTab::ArgusInit() {
   // Init EventHandler
   fEventHandlerTimer = new TTimer(fUpdateFrequency, kTRUE);
   fEventHandlerTimer->Connect("Timeout()", "ArgusTab", this, "ArgusEventHandler()");
   fEventHandlerWaitTimer = new TTimer(1, kTRUE);
   fEventHandlerWaitTimer->Connect("Timeout()", "ArgusTab", this, "ArgusEventHandler()");
   BaseInit();
   if (GetUpdateFrequency()>0 && !fEventHandlerUserStop) {
      fEventHandlerTimer->TurnOn();
   }
}

void ArgusTab::ArgusEventHandler() {
   if (gROME->IsROMEAndARGUS()) {
      if (fEventHandlerWaitTimer==NULL)
         return;
      if (!gROME->IsEventFilled()) {
         fEventHandlerWaitTimer->TurnOn();
         return;
      }
      fEventHandlerWaitTimer->TurnOff();
   }
   fBusy = true;
   BaseEventHandler();
   fBusy = false;
}

void ArgusTab::StartEventHandler(Int_t milliSeconds) {
   fUpdateFrequency = milliSeconds;
   StartEventHandler();
}
void ArgusTab::StartEventHandler() {
   fEventHandlerTimer->SetTime(fUpdateFrequency);
   fEventHandlerTimer->TurnOn();
}

void ArgusTab::StopEventHandler() {
   fEventHandlerTimer->TurnOff();
   fEventHandlerUserStop  = true;
}

Int_t ArgusTab::GetUpdateFrequency() 
{ 
   return fUpdateFrequency; 
};
void ArgusTab::SetUpdateFrequency(Int_t duration) 
{ 
   fUpdateFrequency = duration;
   if (fEventHandlerTimer!=NULL) {
      fEventHandlerTimer->TurnOff();
      if (GetUpdateFrequency()>0 && !fEventHandlerUserStop) {
         fEventHandlerTimer->SetTime(fUpdateFrequency);
         fEventHandlerTimer->TurnOn();
      }
   }
};

