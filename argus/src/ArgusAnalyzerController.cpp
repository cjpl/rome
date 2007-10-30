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


ClassImp(ArgusAnalyzerController)

//______________________________________________________________________________
ArgusAnalyzerController::ArgusAnalyzerController()
:TGTransientFrame(0, 0, 1, 1)
,fNetFolder(0)
,fRunNumber(0)
,fLastRunNumber(0)
,fEventNumber(0)
,fEventStep(1)
,fEventInterval(1)
,fRunNumberEntry(0)
,fEventNumberEntry(0)
,fEventStepEntry(0)
,fEventIntervalEntry(0)
,fRunNumberLabel(0)
,fEventNumberLabel(0)
,fEventStepLabel(0)
,fEventIntervalLabel(0)
,fPlayButton(0)
,fPreviousButton(0)
,fNextButton(0)
,fUpdateButton(0)
,fStopButton(0)
,fOnDisplay(kTRUE)
,fInitialized(kFALSE)
{
   fVerticalFrame[0] = fVerticalFrame[1] = 0;
   fHorizontalFrame[0] = fHorizontalFrame[1] = 0;
}

//______________________________________________________________________________
ArgusAnalyzerController::ArgusAnalyzerController(const TGWindow *p, const TGWindow *main,
                                                 UInt_t w, UInt_t h, ROMENetFolder * nf,
                                                 UInt_t options)
:TGTransientFrame(p, main, w, h, options)
,fNetFolder(nf)
,fRunNumber(0)
,fLastRunNumber(0)
,fEventNumber(0)
,fEventStep(1)
,fEventInterval(1)
//,fVerticalFrame({0, 0})
//,fHorizontalFrame({0, 0})
,fRunNumberEntry(0)
,fEventNumberEntry(0)
,fEventStepEntry(0)
,fEventIntervalEntry(0)
,fRunNumberLabel(0)
,fEventNumberLabel(0)
,fEventStepLabel(0)
,fEventIntervalLabel(0)
,fPlayButton(0)
,fPreviousButton(0)
,fNextButton(0)
,fUpdateButton(0)
,fStopButton(0)
,fOnDisplay(kTRUE)
,fInitialized(kFALSE)
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

   ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);

   // Horizontal frame which contains picture buttons
   fHorizontalFrame[0] = new TGHorizontalFrame(this, 32 * 6 + 4, 32 + 4);

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

   fPreviousButton = new TGPictureButton(fHorizontalFrame[0],
                                         gClient->GetPicturePool()->GetPicture("previous",
                                                                               previousImage->GetPixmap(),
                                                                               previousImage->GetMask()),
                                         B_Previous);
   fPlayButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicturePool()->GetPicture("play",
                                                                           playImage->GetPixmap(),
                                                                           playImage->GetMask()),
                                     B_Play);
   fStopButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicturePool()->GetPicture("stop",
                                                                           stopImage->GetPixmap(),
                                                                           stopImage->GetMask()),
                                     B_Stop);
   fNextButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicturePool()->GetPicture("next",
                                                                           nextImage->GetPixmap(),
                                                                           nextImage->GetMask()),
                                     B_Next);
   fUpdateButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicturePool()->GetPicture("update",
                                                                           updateImage->GetPixmap(),
                                                                           updateImage->GetMask()),
                                       B_Update);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/frwd.xpm"), B_Frwd);
#else
   fPreviousButton = new TGPictureButton(fHorizontalFrame[0],
                                         gClient->GetPicture("$ROMESYS/argus/icons/previous.xpm"), B_Previous);
   fPlayButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicture("$ROMESYS/argus/icons/play.xpm"), B_Play);
   fStopButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicture("$ROMESYS/argus/icons/stop.xpm"), B_Stop);
   fNextButton = new TGPictureButton(fHorizontalFrame[0],
                                     gClient->GetPicture("$ROMESYS/argus/icons/next.xpm"), B_Next);
   fUpdateButton = new TGPictureButton(fHorizontalFrame[0],
                                       gClient->GetPicture("$ROMESYS/argus/icons/update.xpm"), B_Update);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/frwd.xpm"), B_Frwd);
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
   fNextButton->SetToolTipText("Go to next event");
   fUpdateButton->SetToolTipText("Update display");
   fStopButton->SetToolTipText("Terminate this run");
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton->SetToolTipText("Go to end of run");

   fPreviousButton->Associate(this);
   fPlayButton->Associate(this);
   fStopButton->Associate(this);
   fNextButton->Associate(this);
   fUpdateButton->Associate(this);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton->Associate(this);

   fHorizontalFrame[0]->AddFrame(fPreviousButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fPlayButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fStopButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fNextButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fUpdateButton, new TGLayoutHints(kLHintsCenterY | kLHintsRight, 0, 0, 2, 2));
   // comment out until way to go to EndOfRun is implemented
   //   fHorizontalFrame[0]->AddFrame(fFrwdButton, new TGLayoutHints(kLHintsCenterY, 0, 2, 2, 2));

   // Horizontal frame which contains two vertical frames
   fHorizontalFrame[1] = new TGHorizontalFrame(this, 10, 10);

   // Vertical frame which contains labels
   fVerticalFrame[0] = new TGVerticalFrame(fHorizontalFrame[1], 10, 10);
   fRunNumberLabel = new TGLabel(fVerticalFrame[0], "Run number");
   fEventNumberLabel = new TGLabel(fVerticalFrame[0], "Event number");
   fEventStepLabel = new TGLabel(fVerticalFrame[0], "Event step");
   fEventIntervalLabel = new TGLabel(fVerticalFrame[0], "Update frequency");

   fVerticalFrame[0]->AddFrame(fRunNumberLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventNumberLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventStepLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventIntervalLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

   // Vertical frame which contains text entries
   fVerticalFrame[1] = new TGVerticalFrame(fHorizontalFrame[1], 60, 20);
   fRunNumberEntry = new TGNumberEntryField(fVerticalFrame[1], T_RunNumber, static_cast<Double_t>(fRunNumber),
                                            TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventNumberEntry = new TGNumberEntryField(fVerticalFrame[1], T_EventNumber, 0.,
                                              TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventStepEntry = new TGNumberEntry(fVerticalFrame[1], fEventStep, 5, T_EventStep, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventIntervalEntry = new TGNumberEntry(fVerticalFrame[1], fEventInterval, 5, T_EventInterval, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);

   fRunNumberEntry->Associate(this);
   fEventNumberEntry->Associate(this);
   fEventStepEntry->Associate(this);
   fEventIntervalEntry->Associate(this);

   fVerticalFrame[1]->AddFrame(fRunNumberEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[1]->AddFrame(fEventNumberEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[1]->AddFrame(fEventStepEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[1]->AddFrame(fEventIntervalEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

   fHorizontalFrame[1]->AddFrame(fVerticalFrame[0], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fHorizontalFrame[1]->AddFrame(fVerticalFrame[1], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

   AddFrame(fHorizontalFrame[0], new TGLayoutHints(kLHintsExpandX | kLHintsLeft, 2, 2, 2, 2));
   AddFrame(fHorizontalFrame[1], new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Int_t ax, ay;
   if (main) {
      Window_t wdum;
      Int_t mx, my; // geometry of main
      UInt_t mw, mh; // geometry of main
      gVirtualX->GetWindowSize(main->GetId(), mx, my, mw, mh);
      gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(), mw >> 1, mh >> 1, ax, ay, wdum);
   } else {
      UInt_t root_w, root_h;
      gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay, root_w, root_h);
      ax = (root_w - fWidth) >> 1;
      ay = (root_h - fHeight) >> 1;
   }

   Move(ax, ay);
   SetWMPosition(ax, ay);

   TString window_name = "Analyzer Controller";
#if 0 // fSocket is protected
   window_name += fNetFolder->fSocket->GetUrl();
#endif
   window_name += ".";
   SetWindowName(window_name);

   SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMenu, kMWMFuncAll | kMWMFuncMaximize | kMWMFuncResize, kMWMInputModeless);

   MapWindow();
//   fClient->WaitFor(this);
   fInitialized = true;
}

//______________________________________________________________________________
ArgusAnalyzerController::~ArgusAnalyzerController()
{
#if 0 // deleting GUI objects may cause error
   SafeDelete(fHorizontalFrame[0]);
   SafeDelete(fHorizontalFrame[1]);
   SafeDelete(fVerticalFrame[0]);
   SafeDelete(fVerticalFrame[1]);
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
Bool_t ArgusAnalyzerController::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (parm1) {
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
            fEventNumber = fEventNumberEntry->GetIntNumber();
            break;
         case T_EventStep:
            fEventStep = fEventStepEntry->GetIntNumber();
            break;
         case T_EventInterval:
            fEventInterval = fEventIntervalEntry->GetIntNumber();
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
            if (fInitialized) {
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

//______________________________________________________________________________
void ArgusAnalyzerController::CloseWindow()
{
   // Close the dialog. On close the dialog will be deleted and cannot be
   // re-used.
   gROME->GetWindow()->SetControllerActive(kFALSE);
   DeleteWindow();
}
