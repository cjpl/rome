// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusTab                                                             //
//                                                                      //
//  The Base Class for all Tabs in ARGUS.
//                                                                      //
//  $Id: ARGUSTab.cpp 1005 2006-04-06 08:18:31Z schneebeli_m $
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <ArgusTab.h>
#include "Riostream.h"

ClassImp(ArgusTab)

ArgusTab::ArgusTab():TGCompositeFrame(NULL,1,1) {
   fActive  = kFALSE;
   fUpdateFrequency  = 0;
   fEventHandlerUserStop  = false;
   fEventHandlerTimer = NULL;
}

void ArgusTab::InitTab() {
   // Init EventHandler
   fEventHandlerTimer = new TTimer(fUpdateFrequency, kTRUE);
   fEventHandlerTimer->Connect("Timeout()", "MEGTDRS", this, "TabEventHandler()");
   Init();
   if (GetUpdateFrequency()>0 && !fEventHandlerUserStop) {
      fEventHandlerTimer->TurnOn();
   }
}

void ArgusTab::TabEventHandler() {
   EventHandler();
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

