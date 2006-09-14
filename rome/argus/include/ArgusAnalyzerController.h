// $Id$
// Author: Ryu Sawada
#ifndef ArgusAnalyzerController_H
#define ArgusAnalyzerController_H


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusAnalyzerController                                              //
//                                                                      //
// GUI object which controlls analysis programs                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

class ROMENetFolder;
class TGNumberEntryField;
class TGNumberEntry;
class TGLabel;
class TGPictureButton;

class ArgusAnalyzerController : public TGTransientFrame
{
protected:
   ROMENetFolder      *fNetFolder;
   Long64_t            fRunNumber;
   Long64_t            fLastRunNumber;
   Long64_t            fEventNumber;
   Int_t               fEventStep;
   Int_t               fEventInterval;
   TGVerticalFrame    *fVerticalFrame[2];
   TGHorizontalFrame  *fHorizontalFrame[2];
   TGNumberEntryField *fRunNumberEntry;
   TGNumberEntryField *fEventNumberEntry;
   TGNumberEntry      *fEventStepEntry;
   TGNumberEntry      *fEventIntervalEntry;
   TGLabel            *fRunNumberLabel;
   TGLabel            *fEventNumberLabel;
   TGLabel            *fEventStepLabel;
   TGLabel            *fEventIntervalLabel;
   TGPictureButton    *fPlayButton;
   TGPictureButton    *fPreviousButton;
   TGPictureButton    *fNextButton;
   TGPictureButton    *fStopButton;
//   TGPictureButton  *fFrwdButton;
   Bool_t              fOnDisplay;
   Bool_t              fInitialized;

   enum CommandIdentifiers{
      B_Play,
      B_Previous,
      B_Next,
      B_Stop,
      B_Frwd,
      T_RunNumber,
      T_EventNumber,
      T_EventStep,
      T_EventInterval
   };
      
public:
   ArgusAnalyzerController():TGTransientFrame(NULL, NULL, 1, 1){}
   ArgusAnalyzerController(const TGWindow* p,const TGWindow* main,UInt_t w,UInt_t h,ROMENetFolder* nf,UInt_t options = kVerticalFrame);
   virtual ~ArgusAnalyzerController();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void           CloseWindow();
   void           Update();

   ClassDef(ArgusAnalyzerController,0) // Panel to control event loop of analyzer
};

#endif
