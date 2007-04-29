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
#include <TArrayI.h>
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif
#include "ROME.h"
#include "ROMEStrArray.h"
#include "ROMEStopwatch.h"

class ArgusWindow;

class ArgusTab : public TGCompositeFrame {

protected:
   ArgusWindow   *fWindow;                //! The window holding the tab
   ROMEString     fTitle;                 //! Tab Title
   Bool_t         fTabActive;             //! is Active
   Bool_t         fBusy;                  //! Busy flag
   Bool_t         fForeground;            //! Flag if foreground
   Long64_t       fCurrentRun;            //! Records current run# displayed in the tab 
   Long64_t       fCurrentEvent;          //! Records current event# displayed in the tab 
   ROMEStopwatch  fWatchAll;              //! Records time used by tab
   ROMEString     fTimeAllString;         //! Elapsed Time of all in a readable format
   ROMEStopwatch  fWatchUser;             //! Records time used by user
   ROMEString     fTimeUserString;        //! Elapsed Time of user in a readable format
   ROMEStopwatch  fWatchUserEvent;        //! Records time used by user events
   ROMEString     fTimeUserEventString;   //! Elapsed Time of user events in a readable format
   Bool_t         fRegisteringActive;     //! Registering active flag

   // General Histo/Graph parameters (object displays, single objects)
   ROMEStrArray        *fDrawOption;      //!
   TArrayI             *fLogScaleX;       //!
   TArrayI             *fLogScaleY;       //!
   TArrayI             *fLogScaleZ;       //!

private:
   ArgusTab(const ArgusTab &tab); // not implemented
   ArgusTab &operator=(const ArgusTab &rhs); // not implemented

public:
   ArgusTab(ArgusWindow* window);
   virtual ~ArgusTab();

   void ArgusInit();
   void ArgusEventHandler();
   Bool_t RequestNewEvent(Long64_t oldRunNumber,Long64_t oldEventNumber);
   Bool_t RequestEvent();

   const char* GetTimeStatisticsString(ROMEString& string);
   const char *GetTimeOfAll();
   const char *GetTimeOfUser();
   const char *GetTimeOfUserEvents();

   // General Histo/Graph parameter methods (object displays, single objects)
   void  SetLogScaleXAt(Int_t displayTypeIndex,Int_t flag=1) { fLogScaleX->AddAt(flag,displayTypeIndex); };
   Int_t IsLogScaleXAt(Int_t displayTypeIndex) { return fLogScaleX->At(displayTypeIndex); };
   void  SetLogScaleYAt(Int_t displayTypeIndex,Int_t flag=1) { fLogScaleY->AddAt(flag,displayTypeIndex); };
   Int_t IsLogScaleYAt(Int_t displayTypeIndex) { return fLogScaleY->At(displayTypeIndex); };
   void  SetLogScaleZAt(Int_t displayTypeIndex,Int_t flag=1) { fLogScaleZ->AddAt(flag,displayTypeIndex); };
   Int_t IsLogScaleZAt(Int_t displayTypeIndex) { return fLogScaleZ->At(displayTypeIndex); };

   void SetDrawOptionAt(Int_t displayTypeIndex,const char* option) { fDrawOption->AddAt(option,displayTypeIndex); };
   const char* GetDrawOptionAt(Int_t displayTypeIndex) { return fDrawOption->At(displayTypeIndex); };

   // User Methods
   const char* GetTitle() const { return fTitle.Data(); }
   void        SetTitle(const char* title) { fTitle = title; }
   Bool_t      IsTabActive() const { return fTabActive; }
   void        SetTabActive(Bool_t active) { fTabActive = active; }
   Bool_t      IsBusy() { return fBusy; }
   Bool_t      IsForeground() { return fForeground; }
   void        SetForeground(Bool_t foreground) { fForeground = foreground; }

   Bool_t      IsRegisteringActive() { return fRegisteringActive; }
   void        SetRegisteringActive(Bool_t registeringActive) { fRegisteringActive = registeringActive; }

   virtual void BaseInit() = 0;
   virtual void Init() = 0;
   virtual void EndInit() = 0;
   virtual void BaseEventHandler() = 0;
   virtual void EventHandler() = 0;

   virtual void RegisterObjects() = 0;
   virtual void UnRegisterObjects() = 0;

   ClassDef(ArgusTab,0) // Base class of ARGUS tabs
};

#endif   // ArgusTab_H
