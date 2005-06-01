/********************************************************************
  TGROMEController.h, R.Sawada

  $Log$
  Revision 1.1  2005/06/01 22:41:22  sawada
  added TGROMEController for development.


********************************************************************/
#ifndef TGROMECONTROLLER_H
#define TGROMECONTROLLER_H

#include <TGNumberEntry.h>
#include <TGButton.h>
#include <TGLabel.h>
#include "TNetFolder.h"

class TGROMEController : public TGTransientFrame
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
   TGROMEController():TGTransientFrame(){};
   TGROMEController(const TGWindow* p,const TGWindow* main,UInt_t w,UInt_t h,TNetFolder* nf,UInt_t options = kVerticalFrame);
   virtual ~TGROMEController() { DeleteWindow(); }
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   
   ClassDef(TGROMEController,1)
};

#endif
