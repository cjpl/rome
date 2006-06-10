// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TTTTab1                                                                    //
//                                                                            //
//                                                                            //
//                                                                            //
// This tab has following thread functions.                                   //
//    func_1_1                                                                //
//    func_1_2                                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/TTAnalyzer.h"
#include "include/generated/TTWindow.h"
#include "include/tabs/TTTTab1.h"

// uncomment if you want to include headers of all folders
//#include "TTAllFolders.h"


ClassImp(TTTTab1)

void TTTTab1::Init()
{
   fVert = new TGVerticalFrame(this, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   fCanvas = new TRootEmbeddedCanvas("Sample Canvas", fVert, (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()));

   fHorz = new TGHorizontalFrame(fVert, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (100 * gAnalyzer->GetWindow()->GetWindowScale()));

   fBStartVertical = new TGTextButton(fHorz, "Start vertical", B_STARTV);
   fBStopVertical = new TGTextButton(fHorz, "Stop vertical", B_STOPV);
   fBStartHorizontal = new TGTextButton(fHorz, "Start horizontal", B_STARTH);
   fBStopHorizontal = new TGTextButton(fHorz, "Stop horizontal", B_STOPH);

   fBStartVertical->Associate(this);
   fBStopVertical->Associate(this);
   fBStartHorizontal->Associate(this);
   fBStopHorizontal->Associate(this);

   fHorz->AddFrame(fBStartVertical, new TGLayoutHints(kLHintsCenterY, 10, 10, 4, 4));
   fHorz->AddFrame(fBStopVertical, new TGLayoutHints(kLHintsCenterY, 10, 10, 4, 4));
   fHorz->AddFrame(fBStartHorizontal, new TGLayoutHints(kLHintsCenterY, 10, 10, 4, 4));
   fHorz->AddFrame(fBStopHorizontal, new TGLayoutHints(kLHintsCenterY, 10, 10, 4, 4));

   fText = new TLatex(0.5, 0.5, "thread test.");
   fText->SetTextAlign(22);

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));

   fVert->AddFrame(fHorz, new TGLayoutHints(kLHintsExpandX | kLHintsBottom, 4, 4, 4, 4));

   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));

   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

void TTTTab1::EndInit()
{
}

void TTTTab1::EventHandler()
{
}

void TTTTab1::MenuClicked(TGPopupMenu *menu,Long_t param)
{
}

void TTTTab1::TabSelected()
{
}
void TTTTab1::TabUnSelected()
{
}

void TTTTab1::func_1_1()
{
#if defined (ROOT_TThread)
   TThread::Lock();
#endif
   fText->SetY(fText->GetY() < 1 ? fText->GetY() + 0.01 : 0);
#if defined (ROOT_TThread)
   TThread::UnLock();
#endif
   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

void TTTTab1::func_1_2()
{
#if defined (ROOT_TThread)
   TThread::Lock();
#endif
   fText->SetX(fText->GetX() < 1 ? fText->GetX() + 0.01 : 0);
#if defined (ROOT_TThread)
   TThread::UnLock();
#endif
   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

bool TTTTab1::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1) {
         case B_STARTV:
            Startfunc_1_1(200, 100);
            break;
         case B_STOPV:
            Stopfunc_1_1();
            break;
         case B_STARTH:
            Startfunc_1_2(1000, 10);
            break;
         case B_STOPH:
            Stopfunc_1_2();
            break;
         }
         break;
      }
      break;
   }
   return true;
}
