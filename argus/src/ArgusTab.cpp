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

#include "ArgusTab.h"
#include "ArgusWindow.h"
#include "ROMEAnalyzer.h"
#include "ROMEiostream.h"

ClassImp(ArgusTab)

//______________________________________________________________________________
ArgusTab::ArgusTab(ArgusWindow* window, const char* title, ROMEStrArray *drawOpt,
                   TArrayI *logX, TArrayI *logY, TArrayI *logZ, Int_t nUserMenus)
:TGCompositeFrame(NULL,1,1)
,fWindow(window)
,fTitle(title)
,fTabActive(kFALSE)
,fBusy(kFALSE)
,fForeground(kFALSE)
,fCurrentRun(-1)
,fCurrentEvent(-1)
,fWatchAll()
,fWatchUserEvent()
,fCpuTimeAllString("")
,fRealTimeAllString("")
,fTimeUserEventString("")
,fRegisteringActive(kTRUE)
,fSwitch(kFALSE)
,fID(-1)
,fUserPopupMenus(new TObjArray(nUserMenus))
,fDrawOption(drawOpt)
,fLogScaleX(logX)
,fLogScaleY(logY)
,fLogScaleZ(logZ)
{
   // Reset stopwatches
   fWatchAll.Reset();
   fWatchUserEvent.Reset();  

   // Create menus
   Int_t iMenu;
   for (iMenu = 0; iMenu < nUserMenus; iMenu++) {
      fUserPopupMenus->AddAt(new TGPopupMenu(fClient->GetRoot()), iMenu);
      GetUserPopupMenuAt(iMenu)->Associate(fWindow);
   }
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
   while (fBusy) {
      gSystem->Sleep(10);
   }

   fWatchAll.Start(false);
   fWatchUserEvent.Start(false);
   fBusy = true;
   BaseEventHandler();
   fBusy = false;
   fWatchUserEvent.Stop();
   fWatchAll.Stop();
}

//______________________________________________________________________________
Bool_t ArgusTab::RequestNewEvent(Long64_t oldRunNumber, Long64_t oldEventNumber) {

   Bool_t forced = kFALSE;
   Int_t i,j;
   if (gROME->GetWindow()->IsEventHandlingForced()) {
      forced = kTRUE;
      gROME->GetWindow()->ClearEventHandlingForced();
   }

   if (gROME->IsROMEMonitor()) {
      fCurrentRun   = gROME->GetCurrentRunNumber();
      fCurrentEvent = gROME->GetCurrentEventNumber();
      return gROME->GetSocketClientNetFolder()->RequestNewEvent(oldRunNumber,oldEventNumber);
   } else {
      if (oldRunNumber!=gROME->GetCurrentRunNumber()
          || oldEventNumber!=gROME->GetCurrentEventNumber()
          || forced ) {
         // if new run#/event# or event handling is forced
         fCurrentRun   = gROME->GetCurrentRunNumber();
         fCurrentEvent = gROME->GetCurrentEventNumber();
         if (gROME->IsROMEMonitor()) {
            for (i=0;i<gROME->GetTaskObjectEntries();i++) {
               for (j=0;j<gROME->GetTaskObjectAt(i)->GetNumberOfHistos();j++) {
                  gROME->GetTaskObjectAt(i)->SetHistoUpToDateAt(j,kFALSE);
               }
            }
         }
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
      for (i = 0; i < 35 - name.Length() - 5; i++) {
         string += ".";
      }
      string.AppendFormatted(" : %s  %s  %s\n", GetRealTimeOfAll(), GetCpuTimeOfAll(), GetTimeOfUserEvents());
   }
   return string;
}

//______________________________________________________________________________
const char* ArgusTab::GetRealTimeOfAll()
{
   // Returns the elapsed real time in a readable format
   fWatchAll.GetRealTimeString(fRealTimeAllString);
   return fRealTimeAllString.Data();
}

//______________________________________________________________________________
const char* ArgusTab::GetCpuTimeOfAll()
{
   // Returns the elapsed CPU time in a readable format
   fWatchAll.GetCpuTimeString(fCpuTimeAllString);
   return fCpuTimeAllString.Data();
}

//______________________________________________________________________________
const char* ArgusTab::GetTimeOfUserEvents()
{
   // Returns the elapsed CPU time in a readable format
   fWatchUserEvent.GetCpuTimeString(fTimeUserEventString);
   return fTimeUserEventString.Data();
}
