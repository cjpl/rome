// $Id$
// Author: Ryu Sawada

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusAnalyzerController                                              //
//                                                                      //
// GUI object which controlls analysis programs                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "ArgusAnalyzerController.h"
#include "ROMEAnalyzer.h"
#include "ROMEEventLoop.h"

ClassImp(ArgusAnalyzerController)
#define AADEBUG
// Constructor
    ArgusAnalyzerController::ArgusAnalyzerController(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, ROMENetFolder * nf, UInt_t options)
:TGTransientFrame(p, main, w, h, options)
{
   fNetFolder = nf;
   fRunNumber = gROME->GetCurrentRunNumber();
   fLastRunNumber = fRunNumber;
   fEventNumber = gROME->GetCurrentEventNumber();
   fEventStep = 1;
   fEventInterval = 5;

#if !defined( AADEBUG )
   if (!fNetFolder->fSocket || !fNetFolder->fSocket->IsValid())
      Error("ArgusAnalyzerController", "NetFolder is not connected to server.");
   return;
#endif

   ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);

// Horizontal frame which contains picture buttons
   fHorizontalFrame[0] = new TGHorizontalFrame(this, 32 * 4 + 4, 32 + 4);

   fPlayButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/play.xpm"), B_Play);
   fNextButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/next.xpm"), B_Next);
   fStopButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/stop.xpm"), B_Stop);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ROMESYS/argus/icons/frwd.xpm"), B_Frwd);

   fPlayButton->AllowStayDown(kTRUE);
   if (static_cast<ROMEEventLoop*>(gROME->GetMainTask())->isContinuousMode()) {
      fPlayButton->SetPicture(gClient->GetPicture("$ROMESYS/argus/icons/pause.xpm"));
      fPlayButton->SetToolTipText("Stop continuous analysis");
      fPlayButton->SetState(kButtonEngaged,kTRUE);
   }
   else {
      fPlayButton->SetToolTipText("Start continuous analysis");
   }
 
   fNextButton->SetToolTipText("Go to next event");
   fStopButton->SetToolTipText("Terminate this run");
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton->SetToolTipText("Go to end of run");

   fPlayButton->Associate(this);
   fNextButton->Associate(this);
   fStopButton->Associate(this);
   // comment out until way to go to EndOfRun is implemented
   //   fFrwdButton->Associate(this);

   fHorizontalFrame[0]->AddFrame(fPlayButton, new TGLayoutHints(kLHintsCenterY, 2, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fNextButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fStopButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   // comment out until way to go to EndOfRun is implemented
   //   fHorizontalFrame[0]->AddFrame(fFrwdButton, new TGLayoutHints(kLHintsCenterY, 0, 2, 2, 2));

// Horizontal frame which contains two vertical frames
   fHorizontalFrame[1] = new TGHorizontalFrame(this, 10, 10);

// Vertical frame which contains labels
   fVerticalFrame[0] = new TGVerticalFrame(fHorizontalFrame[1], 10, 10);
   fRunNumberLabel = new TGLabel(fVerticalFrame[0], "Run number");
   fEventNumberLabel = new TGLabel(fVerticalFrame[0], "Event number");
   // comment out until step and interval is implemented
   //   fEventStepLabel = new TGLabel(fVerticalFrame[0], "Event step");
   //   fEventIntervalLabel = new TGLabel(fVerticalFrame[0], "Event interval");
   fVerticalFrame[0]->AddFrame(fRunNumberLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventNumberLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   // comment out until step and interval is implemented
   //   fVerticalFrame[0]->AddFrame(fEventStepLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   //   fVerticalFrame[0]->AddFrame(fEventIntervalLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

// Vertical frame which contains text entries
   fVerticalFrame[1] = new TGVerticalFrame(fHorizontalFrame[1], 60, 20);
   fRunNumberEntry = new TGNumberEntryField(fVerticalFrame[1], T_RunNumber, (Double_t)fRunNumber, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventNumberEntry = new TGNumberEntryField(fVerticalFrame[1], T_EventNumber, (Double_t)fEventNumber, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   // comment out until step and interval is implemented
   //   fEventStepEntry = new TGNumberEntry(fVerticalFrame[1], fEventStep, 5, T_EventStep, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   //   fEventIntervalEntry = new TGNumberEntry(fVerticalFrame[1], fEventInterval, 5, T_EventInterval, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);

   fRunNumberEntry->Associate(this);
   fEventNumberEntry->Associate(this);
   // comment out until step and interval is implemented
   //   fEventStepEntry->Associate(this);
   //   fEventIntervalEntry->Associate(this);

   fVerticalFrame[1]->AddFrame(fRunNumberEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[1]->AddFrame(fEventNumberEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   // comment out until step and interval is implemented
   //   fVerticalFrame[1]->AddFrame(fEventStepEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   //   fVerticalFrame[1]->AddFrame(fEventIntervalEntry, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

   fHorizontalFrame[1]->AddFrame(fVerticalFrame[0], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fHorizontalFrame[1]->AddFrame(fVerticalFrame[1], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

   AddFrame(fHorizontalFrame[0], new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
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
   }
   else {
      UInt_t root_w, root_h;
      gVirtualX->GetWindowSize(fClient->GetRoot()->GetId(), ax, ay, root_w, root_h);
      ax = (root_w - fWidth) >> 1;
      ay = (root_h - fHeight) >> 1;
   }

   Move(ax, ay);
   SetWMPosition(ax, ay);

   TString window_name = "Control Panel";
#if !defined( AADEBUG )
   window_name += fNetFolder->fSocket->GetUrl();
#endif
   window_name += ".";
   SetWindowName(window_name);

   SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMenu, kMWMFuncAll | kMWMFuncMaximize | kMWMFuncResize, kMWMInputModeless);

   MapWindow();
//   fClient->WaitFor(this);
}

Bool_t ArgusAnalyzerController::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   char str[100];
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (parm1) {
         case B_Play:
            if (fPlayButton->IsDown()) {
               fPlayButton->SetPicture(gClient->GetPicture("$ROMESYS/argus/icons/pause.xpm"));
               fPlayButton->SetToolTipText("Stop continuous analysis");
               if (gROME->IsStandAloneARGUS()) {
                  if (fNetFolder) {
                     fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventC();");
                     fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventR();");
                  }
               }
               else {
                  gROME->SetUserEventC();
                  gROME->SetUserEventR();
               }
            }
            else {
#if defined( R__VISUAL_CPLUSPLUS )
               sprintf(str, "%I64d", gROME->GetCurrentEventNumber()-1);
#else
               sprintf(str, "%lld", gROME->GetCurrentEventNumber()-1);
#endif
               fEventNumberEntry->SetText(str);
               fPlayButton->SetPicture(gClient->GetPicture("$ROMESYS/argus/icons/play.xpm"));
               fPlayButton->SetToolTipText("Start continuous analysis");
               if (gROME->IsStandAloneARGUS()) {
                  if (fNetFolder) {
                     fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventO();");
                  }
               }
               else {
                  gROME->SetUserEventO();
               }
            }
            break;
         case B_Next:
            if (gROME->IsStandAloneARGUS()) {
               if (fNetFolder) {
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventO();");
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventR();");
               }
            }
            else {
#if defined( R__VISUAL_CPLUSPLUS )
               sprintf(str, "%I64d", gROME->GetCurrentEventNumber()-1);
#else
               sprintf(str, "%lld", gROME->GetCurrentEventNumber()-1);
#endif
               fEventNumberEntry->SetText(str);
               gROME->SetUserEventO();
               gROME->SetUserEventR();
            }
            break;
         case B_Stop:
            if (gROME->IsStandAloneARGUS()) {
               if (fNetFolder) {
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventE();");
               }
            }
            else {
               gROME->SetUserEventE();
            }
            break;
         case B_Frwd:
            if (gROME->IsStandAloneARGUS()) {
               if (fNetFolder) {
                  /* do something to go to EndOfRun */
               }
            }
            else {
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
            /* change event number in analyzer */
            break;
            // comment out until step and interval is implemented
            //         case T_EventStep:
            //            fEventStep = fEventStepEntry->GetIntNumber();
            //            break;
            //         case T_EventInterval:
            //            fEventInterval = fEventIntervalEntry->GetIntNumber();
            //            break;
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
            break;
            // comment out until step and interval is implemented
            //         case T_EventStep:
            //            fEventStep = fEventStepEntry->GetIntNumber();
            //            break;
            //         case T_EventInterval:
            //            fEventInterval = fEventIntervalEntry->GetIntNumber();
            //            break;
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
void ArgusAnalyzerController::CloseWindow()
{
   gROME->WindowClosed();
}
