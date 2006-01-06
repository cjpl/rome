// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HWTFrench                                                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/HWTFrench.h"
#include "include/generated/HWWindow.h"

ClassImp(HWTFrench)

void HWTFrench::Init()
{
   // Create a vertical frame containing buttons and canvas 
   fVert = new TGVerticalFrame(fParent, (UInt_t) (700 * gWindow->GetWindowScale()), (UInt_t) (700 * gWindow->GetWindowScale()));

   // Create an embedded canvas and add to the main frame, centered in x and y
   fCanvas = new TRootEmbeddedCanvas("Hello World", fVert, (UInt_t) (600 * gWindow->GetWindowScale()), (UInt_t) (600 * gWindow->GetWindowScale()));

   // Create a text
   fText = new TLatex(0.5, 0.5, "Bonjour monde.");
   fText->SetTextAlign(22);
   fText->SetTextColor(fSteering->GetColor());

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));

   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}
