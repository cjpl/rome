//// Author: Ryu Sawada
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGTTab                                                                     //
//                                                                            //
// 
// 
// This header has been generated by the ArgusBuilder.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/HGTTab.h"

ClassImp(HGTTab)

void HGTTab::Init()
{
   // Create a vertical frame containing buttons and canvas 
   fVert = new TGVerticalFrame(this, (UInt_t)(700*gMonitor->GetWindowScale()), (UInt_t)(700*gMonitor->GetWindowScale()));
   
   // Create an embedded canvas and add to the main frame, centered in x and y 
   fCanvas = new TRootEmbeddedCanvas("Sample Canvas", fVert, (UInt_t)(600*gMonitor->GetWindowScale()), (UInt_t)(600*gMonitor->GetWindowScale()));
   
   // Create "Update" button 
   fBUpdate = new TGTextButton(fVert, "Update", B_UPDATE);
   fBUpdate->Associate(this);
   
   fVert->AddFrame(fCanvas,
		   new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   fVert->AddFrame(fBUpdate, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));
}

void HGTTab::Update(){
   if(!gMonitor->GetNetFolder()){
      cout<<gMonitor->GetProgramName()<<" is not connected to server."<<endl;
      return;
   }
   fCanvas->GetCanvas()->cd();
   
   if(fHisto){
      delete fHisto;
   }
   fHisto = (TH1F*)gMonitor->GetNetFolder()->FindObjectAny("hAdc");
   if (!fHisto){
      cout<<"Histo hAdc0 not available."<<endl;
   }
   else{
      fHisto->Draw();
   }  
   
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();  
   return;
}

bool HGTTab::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)
{
   switch (GET_MSG(msg)) {
   case kC_COMMAND:    
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
         switch (param1){
         case B_UPDATE:
            Update();
            break;
         }
         break;
      }
      break;    
   }  
   return true;
}

