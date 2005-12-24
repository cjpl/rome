// $Id: ArgusAnalyzerController.cpp 785 2005-12-18 18:43:19Z sawada $
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

ClassImp(ArgusAnalyzerController)
#define DEBUG
// Constructor
    ArgusAnalyzerController::ArgusAnalyzerController(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, TNetFolder * nf, UInt_t options)
:TGTransientFrame(p, main, w, h, options)
{
   fNetFolder = nf;
   fRunNumber = gROME->GetCurrentRunNumber();
   fLastRunNumber = fRunNumber;
   fEventNumber = gROME->GetCurrentEventNumber();
   fEventStep = 1;
   fEventInterval = 5;

#if !defined( DEBUG )
   if (!fNetFolder->fSocket || !fNetFolder->fSocket->IsValid())
      Error("ArgusAnalyzerController", "NetFolder is not connected to server.");
   return;
#endif

   ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);

// Horizontal frame which contains picture buttons
   fHorizontalFrame[0] = new TGHorizontalFrame(this, 32 * 4 + 4, 32 + 4);

   fPlayButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ARGUSSYS/icons/play.xpm"), B_Play);
   fNextButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ARGUSSYS/icons/next.xpm"), B_Next);
   fStopButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ARGUSSYS/icons/stop.xpm"), B_Stop);
   fFrwdButton = new TGPictureButton(fHorizontalFrame[0], gClient->GetPicture("$ARGUSSYS/icons/frwd.xpm"), B_Frwd);

   fPlayButton->AllowStayDown(kTRUE);

   fPlayButton->SetToolTipText("Start continuous analysis");
   fNextButton->SetToolTipText("Go to next event");
   fStopButton->SetToolTipText("Terminate this run");
   fFrwdButton->SetToolTipText("Go to end of run");

   fPlayButton->Associate(this);
   fNextButton->Associate(this);
   fStopButton->Associate(this);
   fFrwdButton->Associate(this);

   fHorizontalFrame[0]->AddFrame(fPlayButton, new TGLayoutHints(kLHintsCenterY, 2, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fNextButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fStopButton, new TGLayoutHints(kLHintsCenterY, 0, 0, 2, 2));
   fHorizontalFrame[0]->AddFrame(fFrwdButton, new TGLayoutHints(kLHintsCenterY, 0, 2, 2, 2));

// Horizontal frame which contains two vertical frames
   fHorizontalFrame[1] = new TGHorizontalFrame(this, 10, 10);

// Vertical frame which contains labels
   fVerticalFrame[0] = new TGVerticalFrame(fHorizontalFrame[1], 10, 10);
   fRunNumberLabel = new TGLabel(fVerticalFrame[0], "Run number");
   fEventNumberLabel = new TGLabel(fVerticalFrame[0], "Event number");
   fEventStepLabel = new TGLabel(fVerticalFrame[0], "Event step");
   fEventIntervalLabel = new TGLabel(fVerticalFrame[0], "Event interval");
   fVerticalFrame[0]->AddFrame(fRunNumberLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventNumberLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventStepLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));
   fVerticalFrame[0]->AddFrame(fEventIntervalLabel, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 2, 2, 2, 2));

// Vertical frame which contains text entries
   fVerticalFrame[1] = new TGVerticalFrame(fHorizontalFrame[1], 60, 20);
   fRunNumberEntry = new TGNumberEntryField(fVerticalFrame[1], T_RunNumber, fRunNumber, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
   fEventNumberEntry = new TGNumberEntryField(fVerticalFrame[1], T_EventNumber, fEventNumber, TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
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
#if !defined( DEBUG )
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
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (parm1) {
         case B_Play:
            if (fPlayButton->IsDown()) {
               fPlayButton->SetPicture(gClient->GetPicture("$ARGUSSYS/icons/pause.xpm"));
               fPlayButton->SetToolTipText("Stop continuous analysis");
               if (fNetFolder) {        // control analyzer
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventC();");
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventR();");
               }
               else {           // control active DAQ
                  // to be implemented something
               }
            }
            else {
               fPlayButton->SetPicture(gClient->GetPicture("$ARGUSSYS/icons/play.xpm"));
               fPlayButton->SetToolTipText("Start continuous analysis");
               if (fNetFolder) {        // control analyzer
                  fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventS();");
               }
               else {           // control active DAQ
                  // to be implemented something
               }
            }
            break;
         case B_Next:
            if (fNetFolder) {   // control analyzer
               fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventO();");
               fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventR();");
            }
            else {              // control active DAQ
               if (fRunNumber != fLastRunNumber) {
                  fLastRunNumber = fRunNumber;
                  gROME->GetActiveDAQ()->EndOfRun();
                  gROME->SetCurrentRunNumber(fRunNumber);
                  if (!gROME->GetActiveDAQ()->BeginOfRun()) {
                     // currently, only sequensial read from event-0 is supported. Random access will be supported soon.
                     fEventNumber = 0;
                     gROME->SetCurrentEventNumber(fEventNumber);
                     return false;
                  }
                  // currently, only sequensial read from event-0 is supported. Random access will be supported soon.
                  fEventNumber = 0;
                  gROME->SetCurrentEventNumber(fEventNumber);
                  gROME->SetCurrentRunNumber(fRunNumber);
               }
               // read data from file
               gROME->GetActiveDAQ()->Event(fEventNumber);

               // increment event number
               fEventNumber++;
               gROME->SetCurrentEventNumber(fEventNumber);
            }
            break;
         case B_Stop:
            if (fNetFolder) {   // control analyzer
               fNetFolder->ExecuteCommand("gAnalyzer->SetUserEventS();");
            }
            else {              // control active DAQ
               // to be implemented something
            }
            break;
         case B_Frwd:
            if (fNetFolder) {   // control analyzer
               // to be implemented something
            }
            else {              // control active DAQ
               gROME->GetActiveDAQ()->EndOfRun();
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
            break;
         case T_EventNumber:
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
      default:
         break;
      }
      break;
   default:
      break;
   }
   return kTRUE;
}
