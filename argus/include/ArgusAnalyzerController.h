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
#include <TGDockableFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

class ROMENetFolder;
class TGDockableFrame;
class TGNumberEntry;
class TGPictureButton;

class ArgusAnalyzerController : public TGDockableFrame
{
protected:
   ROMENetFolder      *fNetFolder;
   Long64_t            fRunNumber;
   Long64_t            fLastRunNumber;
   Long64_t            fEventNumber;
   Int_t               fEventStep;
   Int_t               fUpdatePeriod;
   TGPictureButton    *fPlayButton;
   TGPictureButton    *fPreviousButton;
   TGPictureButton    *fNextButton;
   TGPictureButton    *fUpdateButton;
   TGPictureButton    *fStopButton;
//   TGPictureButton  *fFrwdButton;
   TGNumberEntry      *fRunNumberEntry;
   TGNumberEntry      *fEventNumberEntry;
   TGNumberEntry      *fEventStepEntry;
   TGNumberEntry      *fUpdatePeriodEntry;

   enum CommandIdentifiers {
      B_Dock = 1,
      B_Hide,
      B_Play,
      B_Previous,
      B_Next,
      B_Update,
      B_Stop,
      B_Frwd,
      T_RunNumber,
      T_EventNumber,
      T_EventStep,
      T_UpdatePeriod
   };

private:
   ArgusAnalyzerController(const ArgusAnalyzerController &controller); // not implemented
   ArgusAnalyzerController &operator=(const ArgusAnalyzerController &rhs); // not implemented

public:
   ArgusAnalyzerController();
   ArgusAnalyzerController(const TGWindow* p, Int_t id, ROMENetFolder* nf, UInt_t options = kVerticalFrame);
   virtual ~ArgusAnalyzerController();

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void           Update();

   ClassDef(ArgusAnalyzerController,0) // Panel to control event loop of analyzer
};

#endif
