// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HWTGerman                                                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ArgusBuilder. Manual changes above the       *
 * following line will be lost next time ArgusBuilder is executed.            */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/HWTGerman.h"

ClassImp(HWTGerman)

void HWTGerman::Init()
{
   // Create a vertical frame containing buttons and canvas 
   fVert = new TGVerticalFrame(fParent, (UInt_t) (700 * gMonitor->GetWindowScale()), (UInt_t) (700 * gMonitor->GetWindowScale()));

   // Create an embedded canvas and add to the main frame, centered in x and y 
   fCanvas = new TRootEmbeddedCanvas("Hello World", fVert, (UInt_t) (600 * gMonitor->GetWindowScale()), (UInt_t) (600 * gMonitor->GetWindowScale()));

   // Create a text
   fText = new TLatex(0.5, 0.5, "Hallo Welt.");
   fText->SetTextAlign(22);
   fText->SetTextColor(fSteering->GetColor());

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));

   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}
