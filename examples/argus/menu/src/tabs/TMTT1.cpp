// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TMTT1                                                                      //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/TMAnalyzer.h"
#include "include/generated/TMWindow.h"
#include "include/tabs/TMTT1.h"

// uncomment if you want to include headers of all folders
//#include "TMAllFolders.h"


ClassImp(TMTT1)

void TMTT1::Init()
{
   // Create a vertical frame containing buttons and canvas 
   fVert = new TGVerticalFrame(fParent, (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (700 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create an embedded canvas and add to the main frame, centered in x and y 
   fCanvas = new TRootEmbeddedCanvas("Tab1", fVert, (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()), (UInt_t) (600 * gAnalyzer->GetWindow()->GetWindowScale()));

   // Create a text
   fText = new TLatex(0.5, 0.5, "Menu Tab 1.");
   fText->SetTextAlign(22);

   fVert->AddFrame(fCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 4, 4, 4, 4));
   AddFrame(fVert, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 4, 4));

   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

void TMTT1::EndInit()
{
}

void TMTT1::EventHandler()
{
}

void TMTT1::MenuClicked(TGPopupMenu *menu,Long_t param)
{
   switch (param) {
   case TMWindow::M_T1_Color_Black:
      fText->SetTextColor(1);
      menu->CheckEntry(param);
      break;
   case TMWindow::M_T1_Color_Red:
      fText->SetTextColor(2);
      break;
   case TMWindow::M_T1_Color_Green:
      fText->SetTextColor(3);
      break;
   case TMWindow::M_T1_Color_Blue:
      fText->SetTextColor(4);
      break;
   case TMWindow::M_T1_Color_Gray_Gray1:
      fText->SetTextColor(12);
      break;
   case TMWindow::M_T1_Color_Gray_Gray2:
      fText->SetTextColor(13);
      break;
   case TMWindow::M_T1_Color_Gray_Gray3:
      fText->SetTextColor(14);
      break;
   case TMWindow::M_T1_Color_Gray_Gray4:
      fText->SetTextColor(15);
      break;
   case TMWindow::M_T1_Color_Gray_DarkGray_DarkGray1:
      fText->SetTextColor(16);
      break;
   case TMWindow::M_T1_Color_Gray_DarkGray_DarkGray2:
      fText->SetTextColor(17);
      break;
   case TMWindow::M_T1_Color_Gray_DarkGray_DarkGray3:
      fText->SetTextColor(18);
      break;
   case TMWindow::M_T1_Color_Gray_DarkGray_DarkGray4:
      fText->SetTextColor(19);
      break;
   case TMWindow::M_T1_Size_Small:
      fText->SetTextSize(0.03);
      break;
   case TMWindow::M_T1_Size_Midium:
      fText->SetTextSize(0.08);
      break;
   case TMWindow::M_T1_Size_Large:
      fText->SetTextSize(0.15);
      break;
   }
   fCanvas->GetCanvas()->cd();
   fText->Draw();
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();
}

void TMTT1::TabSelected()
{
}
void TMTT1::TabUnSelected()
{
}

