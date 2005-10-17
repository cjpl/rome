/********************************************************************
  ArgusAnalyzerController.h, R.Sawada

  $Id:$

********************************************************************/
#ifndef ArgusAnalyzerController_H
#define ArgusAnalyzerController_H

#include <TGNumberEntry.h>
#include <TGButton.h>
#include <TGLabel.h>
#include "TNetFolder.h"

class ArgusAnalyzerController : public TGTransientFrame
{
protected:
   TNetFolder*         fNetFolder;
   Int_t               fRunNumber;
   Int_t               fEventNumber;
   Int_t               fEventStep;
   Int_t               fEventInterval;
   TGVerticalFrame*    fVerticalFrame[2];
   TGHorizontalFrame*  fHorizontalFrame[2];
   TGNumberEntryField* fRunNumberEntry;
   TGNumberEntryField* fEventNumberEntry;
   TGNumberEntry*      fEventStepEntry;
   TGNumberEntry*      fEventIntervalEntry;
   TGLabel*            fRunNumberLabel;
   TGLabel*            fEventNumberLabel;
   TGLabel*            fEventStepLabel;
   TGLabel*            fEventIntervalLabel;
   TGPictureButton*    fPlayButton;
   TGPictureButton*    fNextButton;
   TGPictureButton*    fStopButton;
   TGPictureButton*    fFrwdButton;

   enum CommandIdentifiers{
      B_Play,
      B_Next,
      B_Stop,
      B_Frwd,
      T_RunNumber,
      T_EventNumber,
      T_EventStep,
      T_EventInterval
   };
      
public:
   ArgusAnalyzerController():TGTransientFrame(){};
   ArgusAnalyzerController(const TGWindow* p,const TGWindow* main,UInt_t w,UInt_t h,TNetFolder* nf,UInt_t options = kVerticalFrame);
   virtual ~ArgusAnalyzerController() { DeleteWindow(); }
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   
   ClassDef(ArgusAnalyzerController,1)
};

#endif
