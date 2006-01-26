////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// XYZTHistos                                                                 //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "include/tabs/XYZTHistos.h"
#include "include/generated/XYZWindow.h"
#include "include/generated/XYZAnalyzer.h"

ClassImp(XYZTHistos)

void XYZTHistos::Init()
{
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 400, 400);
   AddFrame(fCanvas,new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));
   fCanvas->GetCanvas()->cd();
   histo2 = new TH1F("histo2","Histo 2",200,-100,100);
   histo2->Draw();
}

void XYZTHistos::EventHandler()
{
   histo2->Reset();
   for (int i=0;i<200;i++)
      histo2->Fill(i-100,gAnalyzer->GetGaussData()->GetBinAt(i));
   fCanvas->GetCanvas()->cd();
   gPad->Modified();
   gPad->Update();
}

void XYZTHistos::MenuClicked(TGPopupMenu *menu,Long_t param)
{
}

void XYZTHistos::TabSelected()
{
}
void XYZTHistos::TabUnSelected()
{
}

