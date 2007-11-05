// $Id$
// Author: Ryu Sawada

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
#include <TGNumberEntry.h>
#include <TGButton.h>
#include <TGLabel.h>
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,2,0))
#   include <TImage.h>
#endif
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ArgusAnalyzerController.h"
#include "ArgusWindow.h"
#include "ROMEAnalyzer.h"
#include "ROMEEventLoop.h"
#include "ROMENetFolder.h"
#include "ROMETreeInfo.h"
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,2,0))
#   include "../icons/previous.xpm"
#   include "../icons/play.xpm"
#   include "../icons/stop.xpm"
#   include "../icons/next.xpm"
#   include "../icons/update.xpm"
#endif

const int   kDigitWidth = 8;

ClassImp(ArgusAnalyzerController)

//______________________________________________________________________________
ArgusAnalyzerController::ArgusAnalyzerController()
:TGDockableFrame(0)
,fNetFolder(0)
,fRunNumber(0)
,fLastRunNumber(0)
,fEventNumber(0)
,fEventStep(1)
,fEventInterval(1)
,fPlayButton(0)
,fPreviousButton(0)
,fNextButton(0)
,fUpdateButton(0)
,fStopButton(0)
,fRunNumberEntry(0)
,fEventNumberEntry(0)
,fEventStepEntry(0)
,fEventIntervalEntry(0)
{
}

//______________________________________________________________________________
ArgusAnalyzerController::ArgusAnalyzerController(const TGWindow *p, Int_t id, ROMENetFolder * nf, UInt_t options)
:TGDockableFrame(p, id, options)
,fNetFolder(nf)
,fRunNumber(0)
,fLastRunNumber(0)
,fEventNumber(0)
,fEventStep(1)
,fEventInterval(1)
,fPlayButton(0)
,fPreviousButton(0)
,fNextButton(0)
,fUpdateButton(0)
,fStopButton(0)
,fRunNumberEntry(0)
,fEventNumberEntry(0)
,fEventStepEntry(0)
,fEventIntervalEntry(0)
{
   fRunNumber = gROME->GetCurrentRunNumber();
   fLastRunNumber = fRunNumber;
   fEventNumber = gROME->GetCurrentEventNumber();
   fEventStep = 1;
   fEventInterval = gROME->GetWindowUpdateFrequency();

#if 0 // fSocket is protected
   if (!fNetFolder->fSocket || !fNetFolder->fSocket->IsValid())
      Error("ArgusAnalyzerController", "NetFolder is not connected to server.");
   return;
#endif

   TGHorizontalFrame  *hFrame[2];
   TGVerticalFrame    *vFrame[2];

   // Horizontal frame which contains picture buttons
   hFrame[0] = new TGHorizontalFrame(fContainer, 0, 0);

#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,2,0))
   TImage *previousImage = TImage::Create();
   TImage *playImage = TImage::Create();
   TImage *stopImage = TImage::Create();
   TImage *nextImage = TImage::Create();
   TImage *updateImage = TImage::Create();
   previousImage->SetImageBuffer(const_cast<char**>(previous_xpm), TImage::kXpm);
   playImage->SetImageBuffer(const_cast<char**>(play_xpm), TImage::kXpm);
   stopImage->SetImageBuffer(const_cast<char**>(stop_xpm), TImage::kXpm);
   nextImage->SetImageBuffer(const_cast<char**>(next_xpm), TImage::kXpm);
   updateImage->SetImageBuffer(const_cast<char**>(update_xpm), TImage::kXpm);

   fPreviousButton =
         new TGPictureButton(hFrame[0], gClient->GetPicturePool()->GetPicture("previous",
                                                                   previousImage->GetPixmap(),
                                                                   previousImage->GetMask()),
                             B_Previous);
   fPlayButton =
         new TGPictureButton(hFrame[0], gClient->GetPicturePool()->GetPicture("play",
                                                                              playImage->GetPixmap(),
                                                                              playImage->GetMask()),
                             B_Play);
   fStopButton =
         new TGPictureButton(hFrame[0], gClient->GetPicturePool()->GetPicture("stop",
                                                                              stopImage->GetPixmap(),
                                                                              stopImage->GetMask()),
                             B_Stop);
   fNextButton =
         new TGPictureButton(hFrame[0], gClient->GetPicturePool()->GetPicture("next",
                                                                              nextImage->GetPixmap(),
                                                                              nextImage->GetMask()),
                             B_Next);
   fUpdateButton =
         new TGPictureButton(hFrame[0], gClient->GetPicturePool()->GetPicture("update",
                                                                              updateImage->GetPixmap(),
                                                                              updateImage->GetMask()),
                             B_Update);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton = new TGPictureButton(hFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/frwd.xpm"), B_Frwd);
#else
   fPreviousButton = new TGPictureButton(hFrame[0],
                                         gClient->GetPicture("$ROMESYS/argus/icons/previous.xpm"), B_Previous);
   fPlayButton = new TGPictureButton(hFrame[0],
                                     gClient->GetPicture("$ROMESYS/argus/icons/play.xpm"), B_Play);
   fStopButton = new TGPictureButton(hFrame[0],
                                     gClient->GetPicture("$ROMESYS/argus/icons/stop.xpm"), B_Stop);
   fNextButton = new TGPictureButton(hFrame[0],
                                     gClient->GetPicture("$ROMESYS/argus/icons/next.xpm"), B_Next);
   fUpdateButton = new TGPictureButton(hFrame[0],
                                       gClient->GetPicture("$ROMESYS/argus/icons/update.xpm"), B_Update);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton = new TGPictureButton(hFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/frwd.xpm"), B_Frwd);
#endif

   fPlayButton->AllowStayDown(kTRUE);
   if (static_cast<ROMEEventLoop*>(gROME->GetMainTask())->isContinuousMode()) {
      fPlayButton->SetPicture(gClient->GetPicture("$ROMESYS/argus/icons/pause.xpm"));
      fPlayButton->SetToolTipText("Stop continuous analysis");
      fPlayButton->SetState(kButtonEngaged,kTRUE);
   } else {
      fPlayButton->SetToolTipText("Start continuous analysis");
   }

   fPreviousButton->SetToolTipText("Go to previous event");
   fNextButton    ->SetToolTipText("Go to next event");
   fUpdateButton  ->SetToolTipText("Update display");
   fStopButton    ->SetToolTipText("Terminate this run");
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton->SetToolTipText("Go to end of run");

   fPreviousButton->Associate(this);
   fPlayButton->Associate(this);
   fStopButton->Associate(this);
   fNextButton->Associate(this);
   fUpdateButton->Associate(this);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton->Associate(this);

   hFrame[0]->AddFrame(fPreviousButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));
   hFrame[0]->AddFrame(fPlayButton,     new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));
   hFrame[0]->AddFrame(fStopButton,     new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));
   hFrame[0]->AddFrame(fNextButton,     new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));
   hFrame[0]->AddFrame(fUpdateButton,   new TGLayoutHints(kLHintsCenterY, 5, 0, 0, 0)); // put a small gap
   // comment out until way to go to EndOfRun is implemented
   //   hFrame[0]->AddFrame(fFrwdButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 0, 0));

   // Horizontal frame which contains two vertical frames
   hFrame[1] = new TGHorizontalFrame(fContainer, 10, 10);

   // Vertical frame which contains labels
   vFrame[0] = new TGVerticalFrame(hFrame[1], 10, 10);
   TGLabel *fRunNumberLabel     = new TGLabel(vFrame[0], "Run : ");
   TGLabel *fEventNumberLabel   = new TGLabel(vFrame[0], "Event : ");
   TGLabel *fEventStepLabel     = new TGLabel(vFrame[0], "Event step");
   TGLabel *fEventIntervalLabel = new TGLabel(vFrame[0], "Update freq.");

   vFrame[0]->AddFrame(fRunNumberLabel,     new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   vFrame[0]->AddFrame(fEventNumberLabel,   new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   vFrame[0]->AddFrame(fEventStepLabel,     new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   vFrame[0]->AddFrame(fEventIntervalLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));

   // Vertical frame which contains text entries
   vFrame[1]   = new TGVerticalFrame(hFrame[1], 60, 20);
   fRunNumberEntry     = new TGNumberEntry(vFrame[1], 0., kDigitWidth, T_RunNumber, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventNumberEntry   = new TGNumberEntry(vFrame[1], 0., kDigitWidth, T_EventNumber, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventStepEntry     = new TGNumberEntry(vFrame[1], fEventStep, kDigitWidth, T_EventStep, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventIntervalEntry = new TGNumberEntry(vFrame[1], fEventInterval, kDigitWidth, T_EventInterval, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);

   fRunNumberEntry    ->SetButtonToNum(kFALSE);
   fEventNumberEntry  ->SetButtonToNum(kFALSE);
   fEventStepEntry    ->SetButtonToNum(kFALSE);
   fEventIntervalEntry->SetButtonToNum(kFALSE);

   fRunNumberEntry    ->Associate(this);
   fEventNumberEntry  ->Associate(this);
   fEventStepEntry    ->Associate(this);
   fEventIntervalEntry->Associate(this);

   vFrame[1]->AddFrame(fRunNumberEntry,     new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   vFrame[1]->AddFrame(fEventNumberEntry,   new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   vFrame[1]->AddFrame(fEventStepEntry,     new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));
   vFrame[1]->AddFrame(fEventIntervalEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 0, 0, 0, 0));

   hFrame[1]->AddFrame(vFrame[0], new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 5, 5, 0, 0));
   hFrame[1]->AddFrame(vFrame[1], new TGLayoutHints(kLHintsRight | kLHintsExpandY, 0, 0, 0, 0));

   fContainer->AddFrame(hFrame[0], new TGLayoutHints(kLHintsExpandX | kLHintsLeft, 2, 2, 2, 2));
   fContainer->AddFrame(hFrame[1], new TGLayoutHints(kLHintsExpandX | kLHintsLeft, 2, 2, 2, 2));

   TString window_name = "ARGUS Controller";
#if 0 // fSocket is protected
   window_name += ".";
   window_name += fNetFolder->fSocket->GetUrl();
#endif
   SetWindowName(window_name);

//   GetUndocked()->SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMenu, kMWMFuncAll | kMWMFuncMaximize | kMWMFuncResize, kMWMInputModeless);

}

//______________________________________________________________________________
ArgusAnalyzerController::~ArgusAnalyzerController()
{
#if 0 // deleting GUI objects may cause error
   SafeDelete(fPlayButton);
   SafeDelete(fPreviousButton);
   SafeDelete(fNextButton);
   SafeDelete(fUpdateButton);
   SafeDelete(fStopButton);
   SafeDelete(fRunNumberLabel);
   SafeDelete(fEventNumberLabel);
   SafeDelete(fRunNumberEntry);
   SafeDelete(fEventNumberEntry);
#endif
}

//______________________________________________________________________________
void ArgusAnalyzerController::Update()
{
   char str[100];
   sprintf(str, R_LLD, TMath::Max(gROME->GetCurrentEventNumber(), static_cast<Long64_t>(0)));
   if(fEventNumberEntry) {
      fEventNumberEntry->SetText(str);
   }
   sprintf(str, R_LLD, TMath::Max(gROME->GetCurrentRunNumber(), static_cast<Long64_t>(0)));
   if(fRunNumberEntry) {
      fRunNumberEntry->SetText(str);
   }
}

//______________________________________________________________________________
Bool_t ArgusAnalyzerController::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (parm1) {
         case 1:
            if (!fHidden) {
               UndockContainer();
            }
            break;
         case 2:
            if (!fHidden) {
               HideContainer();
            } else {
               ShowContainer();
            }
            break;
         case B_Play:
            if (fPlayButton->IsDown()) {
               fPlayButton->SetPicture(gClient->GetPicture("$ROMESYS/argus/icons/pause.xpm"));
               fPlayButton->SetToolTipText("Stop continuous analysis");
               if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
                  if (fNetFolder) {
                     fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventC();");
                     fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventR();");
                  }
               } else {
                  gROME->SetUserEventC();
                  gROME->SetUserEventR();
               }
            } else {
               fPlayButton->SetPicture(gClient->GetPicture("$ROMESYS/argus/icons/play.xpm"));
               fPlayButton->SetToolTipText("Start continuous analysis");
               if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
                  if (fNetFolder) {
                     fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventS();");
                  }
               } else {
                  gROME->SetUserEventS();
               }
            }
            break;
         case B_Previous:
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
               if (fNetFolder) {
                  ROMEString command;
                  command.SetFormatted("gAnalyzer->SetUserEventJ("R_LLD");", gROME->GetCurrentEventNumber() - fEventStep);
                  fNetFolder->ExecuteCommand(command.Data());
               }
            } else {
               gROME->SetUserEventJ(gROME->GetCurrentEventNumber() - fEventStep);
            }
            break;
         case B_Next:
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
               if (fNetFolder) {
                  ROMEString command;
                  command.SetFormatted("gAnalyzer->SetUserEventJ("R_LLD");", gROME->GetCurrentEventNumber() + fEventStep);
                  fNetFolder->ExecuteCommand(command.Data());
               }
            } else {
                  gROME->SetUserEventJ(gROME->GetCurrentEventNumber() + fEventStep);
            }
            break;

         case B_Update:
            gROME->GetWindow()->ForceEventHandling();      
            gROME->GetWindow()->RequestEventHandling();
            break;
         case B_Stop:
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
               if (fNetFolder) {
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventE();");
               }
            } else {
               gROME->SetUserEventE();
            }
            break;
         case B_Frwd:
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
               if (fNetFolder) {
                  /* do something to go to EndOfRun */
               }
            } else {
               /* do something to go to EndOfRun */
            }
            break;
         case T_RunNumber:
            /* change run number in analyzer */
            break;
         case T_EventNumber:
            fEventNumberEntry->GetNumberEntry()
                  ->IncreaseNumber((TGNumberFormat::EStepSize)(parm2%100), parm2/10000?-1:1, (parm2%10000/100));
            fEventNumber = fEventNumberEntry->GetIntNumber();
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
               if (fNetFolder) {
                  ROMEString str;
                  str.SetFormatted("gAnalyzer->SetUserEventJ("R_LLD");",fEventNumber);
                  fNetFolder->ExecuteCommand(str.Data());
               }
            } else {
               gROME->SetUserEventJ(fEventNumber);
            }
            break;
         case T_EventStep:
            fEventStepEntry->GetNumberEntry()
                  ->IncreaseNumber((TGNumberFormat::EStepSize)(parm2%100), parm2/10000?-1:1, (parm2%10000/100));
            fEventStep = fEventStepEntry->GetIntNumber();
            break;
         case T_EventInterval:
            fEventIntervalEntry->GetNumberEntry()
                  ->IncreaseNumber((TGNumberFormat::EStepSize)(parm2%100), parm2/10000?-1:1, (parm2%10000/100));
            fEventInterval = fEventIntervalEntry->GetIntNumber();
            break;
         default:
            break;
         }
         break;
      default:
         break;
      }
      break;
   case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) {
      case kTE_TEXTCHANGED:
         switch (parm1) {
         case T_RunNumber:
            /* change run number in analyzer */
            break;
         case T_EventNumber:
            /* do nothing until ENTER is presses */
            break;
         case T_EventStep:
            /* do nothing until ENTER is presses */
            break;
         case T_EventInterval:
            /* do nothing until ENTER is presses */
            break;
         default:
            break;
         }
         break;
      case kTE_ENTER:
         switch (parm1) {
         case T_RunNumber:
            fRunNumber = fRunNumberEntry->GetIntNumber();
            break;
         case T_EventNumber:
            fEventNumber = fEventNumberEntry->GetIntNumber();
            if (gROME->IsStandAloneARGUS() || gROME->IsROMEMonitor()) {
               if (fNetFolder) {
                  ROMEString str;
                  str.SetFormatted("gAnalyzer->SetUserEventJ("R_LLD");",fEventNumber);
                  fNetFolder->ExecuteCommand(str.Data());
               }
            } else {
               gROME->SetUserEventJ(fEventNumber);
            }
            break;
         case T_EventStep:
            fEventStep = fEventStepEntry->GetIntNumber();
            break;
         case T_EventInterval:
            fEventInterval = fEventIntervalEntry->GetIntNumber();
            gROME->SetWindowUpdateFrequency(fEventInterval);
            break;
         default:
            break;
         }
         break;
      default:
         break;
      }
      break;
   default:
      break;
   }
   return kTRUE;
}
