////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// NFTHistos                                                                  //
//                                                                            //
//                                                                            //
// Followings are include files of folders. ROMEBuilder will update it with   //
// reading this source code when you execute it next time.                    //
#include "include/generated/NFGaussData.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "include/tabs/NFTHistos.h"
#include "include/generated/NFWindow.h"
#include "include/generated/NFAnalyzer.h"

ClassImp(NFTHistos)

void NFTHistos::Init()
{
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 400, 400);
   AddFrame(fCanvas,new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   fCanvas->GetCanvas()->cd();
   histo2 = new TH1F("histo2","Histo 2",200,-100,100);
   histo2->Draw();
}

void NFTHistos::EventHandler()
{
   histo2->Reset();
   for (int i=0;i<200;i++)
      histo2->Fill(i-100,gAnalyzer->GetGaussData()->GetBinAt(i));
   fCanvas->GetCanvas()->cd();
   gPad->Modified();
   gPad->Update();
}

void NFTHistos::MenuClicked(TGPopupMenu *menu,Long_t param)
{
}

void NFTHistos::TabSelected()
{
}
void NFTHistos::TabUnSelected()
{
}

