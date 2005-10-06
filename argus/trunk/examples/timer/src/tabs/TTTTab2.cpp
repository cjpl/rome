//// Author: Ryu Sawada
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TTTTab2                                                                    //
//                                                                            //
// 
// 
// This header has been generated by the ArgusBuilder.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/TTTTab2.h"

ClassImp(TTTTab2)

void TTTTab2::Init()
{
   fHorz = new TGHorizontalFrame(this, (UInt_t)(700*gMonitor->GetWindowScale()), (UInt_t)(700*gMonitor->GetWindowScale()));

   fBStart_2_1 = new TGTextButton(fHorz, "Start 2_1", B_START_2_1);
   fBStop_2_1  = new TGTextButton(fHorz, "Stop 2_1", B_STOP_2_1);
   fBStart_2_2 = new TGTextButton(fHorz, "Start 2_2", B_START_2_2);
   fBStop_2_2  = new TGTextButton(fHorz, "Stop 2_2", B_STOP_2_2);

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

bool TTTTab2::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:    
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1){
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
