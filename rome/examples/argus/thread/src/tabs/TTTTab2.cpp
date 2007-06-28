// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TTTTab2                                                                    //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
//                                                                            //
// This tab has following thread functions.                                   //
//    func_2_1                                                                //
//    func_2_2                                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/TTTTab2GeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/TTAnalyzer.h"
#include "include/generated/TTWindow.h"
#include "include/tabs/TTTTab2.h"

// uncomment if you want to include headers of all folders
//#include "TTAllFolders.h"


ClassImp(TTTTab2)

void TTTTab2::Init()
{
   fHorz = new TGHorizontalFrame(this, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   fBStart_2_1 = new TGTextButton(fHorz, "Start 2_1", B_START_2_1);
   fBStop_2_1 = new TGTextButton(fHorz, "Stop 2_1", B_STOP_2_1);
   fBStart_2_2 = new TGTextButton(fHorz, "Start 2_2", B_START_2_2);
   fBStop_2_2 = new TGTextButton(fHorz, "Stop 2_2", B_STOP_2_2);

   fBStart_2_1->Associate(this);
   fBStop_2_1->Associate(this);
   fBStart_2_2->Associate(this);
   fBStop_2_2->Associate(this);

   fHorz->AddFrame(fBStart_2_1, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   fHorz->AddFrame(fBStop_2_1, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   fHorz->AddFrame(fBStart_2_2, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   fHorz->AddFrame(fBStop_2_2, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));

   AddFrame(fHorz, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void TTTTab2::EndInit()
{
}

void TTTTab2::EventHandler()
{
}

void TTTTab2::MenuClicked(TGPopupMenu * /*menu*/,Long_t /*param*/)
{
}

void TTTTab2::TabSelected()
{
}
void TTTTab2::TabUnSelected()
{
}

bool TTTTab2::ProcessMessage(Long_t msg, Long_t param1, Long_t /*param2*/)
{
   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
            case kCM_BUTTON:
               switch (param1) {
                  case B_START_2_1:
                     Startfunc_2_1();
                     break;
                  case B_STOP_2_1:
                     Stopfunc_2_1();
                     break;
                  case B_START_2_2:
                     Startfunc_2_2();
                     break;
                  case B_STOP_2_2:
                     Stopfunc_2_2();
                     break;
               }
               break;
         }
         break;
   }
   return true;
}
