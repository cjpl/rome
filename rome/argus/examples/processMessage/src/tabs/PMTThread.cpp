// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// PMTThread                                                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ArgusBuilder. Manual changes above the       *
 * following line will be lost next time ArgusBuilder is executed.            */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/PMTThread.h"

ClassImp(PMTThread)

void PMTThread::Init()
{
   fHorz = new TGHorizontalFrame(this, (UInt_t) (700 * gMonitor->GetWindowScale()), (UInt_t) (700 * gMonitor->GetWindowScale()));

   fButton = new TGTextButton(fHorz, "w/ Thread", 0);
   fButton->Associate(this);
   fHorz->AddFrame(fButton, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));

   AddFrame(fHorz, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void PMTThread::MenuClicked(Long_t param)
{
}

Bool_t PMTThread::ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1) {
         case 0:
            withMakingThread();
            break;
         }
         break;
      }
      break;
   }
   return true;
}

void PMTThread::withMakingThread()
{
   cout << "This function is executed with making new thread" << endl;
   cout << " When you don't have ProcessMessage," << endl;
   cout << "ProcessMessageThread will be executed instead of ProcessMessage." << endl;
   int i;
   for (i = 10; i > 0; i--) {
      if (GetActive())
         cout << i << endl;
      gSystem->Sleep(1000);
   }
   cout << "PMTThread::withMakingThread is finished" << endl;
   cout << endl;
}
