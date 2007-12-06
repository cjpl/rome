// Author: Ryu Sawada <http://www.icepp.s.u-tokyo.ac.jp/%7Esawada>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// PMTNoThread                                                                //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/PMTNoThreadGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/PMAnalyzer.h"
#include "include/generated/PMWindow.h"
#include "include/tabs/PMTNoThread.h"

// uncomment if you want to include headers of all folders
//#include "PMAllFolders.h"


ClassImp(PMTNoThread)

void PMTNoThread::Init()
{
   fHorz = new TGHorizontalFrame(this, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   fButton = new TGTextButton(fHorz, "w/o Thread", 0);
   fButton->Associate(this);
   fHorz->AddFrame(fButton, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));

   AddFrame(fHorz, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void PMTNoThread::EndInit()
{
}

void PMTNoThread::EventHandler()
{
}

void PMTNoThread::MenuClicked(TGPopupMenu * /*menu*/,Long_t /*param*/)
{
}

void PMTNoThread::TabSelected()
{
}
void PMTNoThread::TabUnSelected()
{
}

Bool_t PMTNoThread::ProcessMessage(Long_t msg, Long_t param1, Long_t /*param2*/)
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
   return true;
}

void PMTNoThread::withOutMakingThread()
{
   cout << "This function is executed without making new thread" << endl;
   int i;
   for (i = 10; i > 0; i--) {
      cout << i << endl;
      gSystem->Sleep(1000);
   }
   cout << "PMTNoThread::withOutMakingThread is finished" << endl;
   cout << endl;
}
