// Author: Ryu Sawada <http://www.icepp.s.u-tokyo.ac.jp/%7Esawada>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HWTItalian                                                                 //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/HWTItalianGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/HWAnalyzer.h"
#include "include/generated/HWWindow.h"
#include "include/tabs/HWTItalian.h"

// uncomment if you want to include headers of all folders
//#include "HWAllFolders.h"


ClassImp(HWTItalian)

void HWTItalian::Init()
{
   // Create a vertical frame containing buttons and canvas
   fVert = new TGVerticalFrame(fParent, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create an embedded canvas and add to the main frame, centered in x and y
   fCanvas = new TRootEmbeddedCanvas("Hello World", fVert, (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create a text
   fText = new TLatex(0.5, 0.5, "Ciao mondo.");
   fText->SetTextAlign(22);
   fText->SetTextColor(fSteering->GetColor());

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));

   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

void HWTItalian::EndInit()
{
}

void HWTItalian::EventHandler()
{
}

void HWTItalian::MenuClicked(TGPopupMenu * /*menu*/,Long_t /*param*/)
{
}

void HWTItalian::TabSelected()
{
}
void HWTItalian::TabUnSelected()
{
}

