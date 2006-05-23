// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// PMTMixed                                                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/PMTMixed.h"
#include "include/generated/PMWindow.h"

ClassImp(PMTMixed)

void PMTMixed::Init()
{
   fHorz = new TGHorizontalFrame(this, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   fButton1 = new TGTextButton(fHorz, "w/o Thread", 0);
   fButton2 = new TGTextButton(fHorz, "w/  Thread", 1);
   fButton1->Associate(this);
   fButton2->Associate(this);
   fHorz->AddFrame(fButton1, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   fHorz->AddFrame(fButton2, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));

   AddFrame(fHorz, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void PMTMixed::MenuClicked(Long_t param)
{
}

Bool_t PMTMixed::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1) {
         case 0:
            withOutMakingThread();
            break;
         }
         break;
      }
      break;
   }
   return RunProcessMessageThread(msg, param1, param2);
}

Bool_t PMTMixed::ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1) {
         case 1:
            withMakingThread();
            break;
         }
         break;
      }
      break;
   }
   return true;
}

void PMTMixed::withOutMakingThread()
{
   cout << "This function is executed without making new thread" << endl;
   int i;
   for (i = 10; i > 0; i--) {
      cout << i << endl;
      gSystem->Sleep(1000);
   }
   cout << "PMTMixed::withOutMakingThread is finished" << endl;
   cout << endl;
}

void PMTMixed::withMakingThread()
{
   cout << "This function is executed with making new thread" << endl;
   cout << " When you have both types of ProcessMessage," << endl;
   cout << "you need to call RunProcessMessageThread form ProcessMessage." << endl;
   int i;
   for (i = 10; i > 0; i--) {
      cout << i << endl;
      gSystem->Sleep(1000);
   }
   cout << "PMTMixed::withMakingThread is finished" << endl;
   cout << endl;
}
