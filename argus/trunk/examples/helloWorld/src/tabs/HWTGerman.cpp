//// Author: Ryu Sawada
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HWTGerman                                                                  //
//                                                                            //
// 
// 
// This header has been generated by the ArgusBuilder.
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/HWTGerman.h"

ClassImp(HWTGerman)

void HWTGerman::Init()
{
   // Create a vertical frame containing buttons and canvas 
   fVert = new TGVerticalFrame(fParent, (UInt_t)700, (UInt_t)700);
   
   // Create an embedded canvas and add to the main frame, centered in x and y 
   fCanvas = new TRootEmbeddedCanvas("Hello World", fVert, (UInt_t)600, (UInt_t)600);
   
   // Create a text
   fText = new TLatex(0.5,0.5,"Hallo Welt.");
   fText->SetTextAlign(22);
   fText->SetTextColor(fSteering->GetColor());
   
   fVert->AddFrame(fCanvas,
                   new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsCenterX, 10, 10, 4, 4));
   
   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update(); 
}
