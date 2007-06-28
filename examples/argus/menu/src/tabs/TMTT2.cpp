// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TMTT2                                                                      //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/TMTT2GeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/TMAnalyzer.h"
#include "include/generated/TMWindow.h"
#include "include/tabs/TMTT2.h"

// uncomment if you want to include headers of all folders
//#include "TMAllFolders.h"


ClassImp(TMTT2)

void TMTT2::Init()
{
   // Create a vertical frame containing buttons and canvas
   fVert = new TGVerticalFrame(fParent, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create an embedded canvas and add to the main frame, centered in x and y
   fCanvas = new TRootEmbeddedCanvas("Tab2", fVert, (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create a text
   fText = new TLatex(0.5, 0.5, "Menu Tab 2.");
   fText->SetTextAlign(22);

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));

   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

void TMTT2::EndInit()
{
}

void TMTT2::EventHandler()
{
}

void TMTT2::MenuClicked(TGPopupMenu * /*menu*/,Long_t /*param*/)
{
}

void TMTT2::TabSelected()
{
}
void TMTT2::TabUnSelected()
{
}

