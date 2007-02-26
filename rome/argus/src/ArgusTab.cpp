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

#include "ROMEiostream.h"
#include "ArgusTab.h"
#include "ArgusWindow.h"
#include "ROMEAnalyzer.h"

ClassImp(ArgusTab)

ArgusTab::ArgusTab(ArgusWindow* window):TGCompositeFrame(NULL,1,1) {
   fWindow  = window;
   fTabActive  = kFALSE;
   fBusy = false;
   fWatchAll.Reset();
   fWatchUser.Reset();
   fWatchUserEvent.Reset();
   fRegisteringActive = true;
}

void ArgusTab::ArgusInit() {
   // Init EventHandler
   fWatchAll.Start(false);
   BaseInit();
   fWatchAll.Stop();
}

ArgusTab::~ArgusTab() {
}

void ArgusTab::ArgusEventHandler() {
   if (fBusy)
      return;
   fWatchAll.Start(false);
   fBusy = true;
   BaseEventHandler();
   fBusy = false;
   fWatchAll.Stop();
}

Bool_t ArgusTab::RequestNewEvent(Long64_t oldRunNumber,Long64_t oldEventNumber) {
   gROME->GetWindow()->SetCurrentRun(gROME->GetCurrentRunNumber());
   gROME->GetWindow()->SetCurrentEvent(gROME->GetCurrentEventNumber());
   if (gROME->IsROMEMonitor()) {
      return gROME->GetSocketClientNetFolder()->RequestNewEvent(oldRunNumber,oldEventNumber);
   }
   else {
      if (oldRunNumber!=gROME->GetCurrentRunNumber() || oldEventNumber!=gROME->GetCurrentEventNumber())
         return kTRUE;
   }
   return kFALSE;
}

Bool_t ArgusTab::RequestEvent() {
   gROME->GetWindow()->SetCurrentRun(gROME->GetCurrentRunNumber());
   gROME->GetWindow()->SetCurrentEvent(gROME->GetCurrentEventNumber());
   if (gROME->IsROMEMonitor()) {
      return gROME->GetSocketClientNetFolder()->RequestEvent();
   }
   return kTRUE;
}

// Time methods
const char* ArgusTab::GetTimeStatisticsString(ROMEString& string)
{
   string = "";
   if (fTabActive) {
      int i;
      ROMEString name;
      int nchars;
      nchars = 0;
      string = " Tab ";
      nchars = 5;
      name = this->GetName();
      string += name.Data();
      for (i=0;i<30-name.Length()-5;i++)
         string += ".";
      string.AppendFormatted(" : %s  %s  %s\n", GetTimeOfAll(), GetTimeOfUser(), GetTimeOfUserEvents());
   }
   return string;
}

const char* ArgusTab::GetTimeOfAll()
{
   // Returns the elapsed time in a readable format
   fWatchAll.GetRealTimeString(fTimeAllString);
   return fTimeAllString.Data();
}
const char* ArgusTab::GetTimeOfUser()
{
   // Returns the elapsed time in a readable format
   fWatchUser.GetRealTimeString(fTimeUserString);
   return fTimeUserString.Data();
}
const char* ArgusTab::GetTimeOfUserEvents()
{
   // Returns the elapsed time in a readable format
   fWatchUserEvent.GetRealTimeString(fTimeUserEventString);
   return fTimeUserEventString.Data();
}
