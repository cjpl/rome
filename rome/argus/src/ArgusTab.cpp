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

//______________________________________________________________________________
ArgusTab::ArgusTab(ArgusWindow* window, ROMEStrArray *drawOpt, TArrayI *logX, TArrayI *logY, TArrayI *logZ)
:TGCompositeFrame(NULL,1,1)
,fWindow(window)
,fTitle("")
,fTabActive(kFALSE)
,fBusy(kFALSE)
,fForeground(kFALSE)
,fCurrentRun(-1)
,fCurrentEvent(-1)
,fWatchAll()
,fTimeAllString("")
,fWatchUser()
,fTimeUserString("")
,fWatchUserEvent()
,fTimeUserEventString("")
,fRegisteringActive(kTRUE)
,fDrawOption(drawOpt)
,fLogScaleX(logX)
,fLogScaleY(logY)
,fLogScaleZ(logZ)
{
}

//______________________________________________________________________________
void ArgusTab::ArgusInit() {
   // Init EventHandler
   fWatchAll.Start(false);
   fCurrentRun   = -1;
   fCurrentEvent = -1;
   BaseInit();
   fWatchAll.Stop();
}

//______________________________________________________________________________
ArgusTab::~ArgusTab() {
   SafeDelete(fDrawOption);
   SafeDelete(fLogScaleX);
   SafeDelete(fLogScaleY);
   SafeDelete(fLogScaleZ);
}

//______________________________________________________________________________
void ArgusTab::ArgusEventHandler() {
   while (fBusy)
      gSystem->Sleep(10);
   fWatchAll.Start(false);
   fBusy = true;
   BaseEventHandler();
   fBusy = false;
   fWatchAll.Stop();
}

//______________________________________________________________________________
Bool_t ArgusTab::RequestNewEvent(Long64_t oldRunNumber, Long64_t oldEventNumber) {

   Bool_t forced = kFALSE;
   if(gROME->GetWindow()->IsEventHandlingForced()) {
      forced = kTRUE;
      gROME->GetWindow()->ClearEventHandlingForced();
   }

   if (gROME->IsROMEMonitor()) {
      fCurrentRun   = gROME->GetCurrentRunNumber();
      fCurrentEvent = gROME->GetCurrentEventNumber();
      return gROME->GetSocketClientNetFolder()->RequestNewEvent(oldRunNumber,oldEventNumber);
   }
   else {
      if (oldRunNumber!=gROME->GetCurrentRunNumber()
          || oldEventNumber!=gROME->GetCurrentEventNumber()
          || forced ) {
         // if new run#/event# or event handling is forced
         fCurrentRun   = gROME->GetCurrentRunNumber();
         fCurrentEvent = gROME->GetCurrentEventNumber();
         return kTRUE;
      }
   }
   return kFALSE;
}

//______________________________________________________________________________
Bool_t ArgusTab::RequestEvent() {
   if (gROME->IsROMEMonitor()) {
      return gROME->GetSocketClientNetFolder()->RequestEvent();
   }
   return kTRUE;
}

// Time methods
//______________________________________________________________________________
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

//______________________________________________________________________________
const char* ArgusTab::GetTimeOfAll()
{
   // Returns the elapsed time in a readable format
   fWatchAll.GetRealTimeString(fTimeAllString);
   return fTimeAllString.Data();
}

//______________________________________________________________________________
const char* ArgusTab::GetTimeOfUser()
{
   // Returns the elapsed time in a readable format
   fWatchUser.GetRealTimeString(fTimeUserString);
   return fTimeUserString.Data();
}

//______________________________________________________________________________
const char* ArgusTab::GetTimeOfUserEvents()
{
   // Returns the elapsed time in a readable format
   fWatchUserEvent.GetRealTimeString(fTimeUserEventString);
   return fTimeUserEventString.Data();
}
