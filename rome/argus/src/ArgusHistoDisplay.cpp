// Author: Matthias Schneebeli

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ArgusHistoDisplay                                                                    //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// $Id: ArgusHistoDisplay.cpp 3520 2006-05-18 18:54:40Z Matthias Schneebeli $

#include <RConfig.h>
#include <sys/stat.h>

#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

#if defined( R__VISUAL_CPLUSPLUS )
#   include <Windows4Root.h>
#   include <direct.h>
#endif
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__UNIX )
#   include <unistd.h>
#endif

#include "ArgusWindow.h"
#include "ROMEAnalyzer.h"
#include "ArgusHistoDisplay.h"

ClassImp(ArgusHistoDisplay)

void ArgusHistoDisplay::BaseInit()
{
   int i;
   ROMEString str;

   /* Create an embedded canvas and add to the main frame, centered in x and y */
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 800, 400);

   AddFrame(fCanvas, 
            new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));

   // Status bar
   int parts[2] = { 70,30 };
   gROME->GetWindow()->GetStatusBar()->SetParts(parts,2);
   fProgress = new TGHProgressBar(gROME->GetWindow()->GetStatusBar()->GetBarPart(1),TGProgressBar::kStandard,20);
   fProgress->SetPosition(0);
   gROME->GetWindow()->GetStatusBar()->GetBarPart(1)->AddFrame(fProgress, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 10, 10));

   SetWindowName("DRS");

   for (i=0 ; i<kMaxNumberOfPads ; i++) {
      str.SetFormatted("fTH1F_%d_%s",i,fInheritanceName.Data());
      fTH1F[i] = new TH1F(str.Data(),"",1,0,1);
      str.SetFormatted("fTH2F_%d_%s",i,fInheritanceName.Data());
      fTH2F[i] = new TH2F(str.Data(),"",1,0,1,1,0,1);
      fTGraph[i] = new TGraph(1);
   }

   SetSize(GetDefaultSize());
   fChannelNumber = 0;
   MapSubwindows();
   MapWindow();

   SetupPads(1, 1,true);
}

void ArgusHistoDisplay::BaseMenuClicked(TGPopupMenu *menu,Long_t param)
{
   fTabActive = true;
   int exitID=1,i;
   char str[200];
   switch (param) {
      case M_ARGUS_DISPLAY_VIEW_1_1:
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(1, 1,true);
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_1_2:
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(1, 2,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_2_2:
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_2_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 2,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_3_2:  
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_3_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 3,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_4_2:  
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_4_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 4,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_5_2: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_5_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 5,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_6_2: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_6_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 6,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_7_2: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_7_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 7,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_8_2: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_8_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 8,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_9_2: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_9_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 9,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_10_2: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_10_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(2, 10,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_1_3: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(3, 1,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_2_3: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_2_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(3, 2,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_3_3: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_3_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(3, 3,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_4_3: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_4_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(3, 4,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_5_3: 
         {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_5_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            SetupPads(3, 5,true); 
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_NEXT:
         {
            fChannelNumber += fNumberOfPads;
            Display();
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_PREVIOUS:
         {
            fChannelNumber -= fNumberOfPads;
            if (fChannelNumber < 0)
               fChannelNumber = 0;
            Display();
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_RESET:
         {
            fChannelNumber = 0;
            SetupPads(1, 1,true); 
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
            break;
         }
      case M_ARGUS_DISPLAY_VIEW_PAD_CONFIG: 
         {
            if (fPadConfigActive) {
               fMenuView->UnCheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
               fPadConfigActive = false;
            }
            else {
               fMenuView->CheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
               fPadConfigActive = true;
               fDialog = new XMLToForm(gClient->GetRoot(),this,"res/xmltoform/PadConfigDia.xml",&exitID);
               if (exitID==1) {
                  for (i=0;i<kMaxNumberOfPads;i++) {
                     sprintf(str,"Pad %d/Channel",i);
                     fPadConfigChannel[i] = fDialog->GetIntValue(str);
                  }
                  fDialog->SaveForm();
               }
            }
            break;
         }
   }
   for (i=0;i<80;i++) {
      if (param == M_ARGUS_DISPLAY_VIEW_SELECT+i) { 
         fChannelNumber = i;
         Display();
         break;
      }
   }
   fTabActive = false;
}

void ArgusHistoDisplay::BaseTabSelected()
{
   int i;
   ROMEString str;
   // Create menu

   fMenuDisplay = new TGPopupMenu(fClient->GetRoot());
   fMenuDisplay->Associate(gROME->GetWindow());
   fMenuView = new TGPopupMenu(fClient->GetRoot());
   fMenuView->Associate(gROME->GetWindow());
   fMenuViewDivide = new TGPopupMenu(fClient->GetRoot());
   fMenuViewDivide->Associate(gROME->GetWindow());
   fMenuView0_19 = new TGPopupMenu(fClient->GetRoot());
   fMenuView0_19->Associate(gROME->GetWindow());
   fMenuView20_39 = new TGPopupMenu(fClient->GetRoot());
   fMenuView20_39->Associate(gROME->GetWindow());
   fMenuView40_59 = new TGPopupMenu(fClient->GetRoot());
   fMenuView40_59->Associate(gROME->GetWindow());
   fMenuView60_79 = new TGPopupMenu(fClient->GetRoot());
   fMenuView60_79->Associate(gROME->GetWindow());
   fMenuViewDivide->AddEntry("1x1", M_ARGUS_DISPLAY_VIEW_1_1);
   fMenuViewDivide->AddEntry("1x2", M_ARGUS_DISPLAY_VIEW_1_2);
   fMenuViewDivide->AddEntry("2x2", M_ARGUS_DISPLAY_VIEW_2_2);
   fMenuViewDivide->AddEntry("3x2", M_ARGUS_DISPLAY_VIEW_3_2);
   fMenuViewDivide->AddEntry("4x2", M_ARGUS_DISPLAY_VIEW_4_2);
   fMenuViewDivide->AddEntry("5x2", M_ARGUS_DISPLAY_VIEW_5_2);
   fMenuViewDivide->AddEntry("6x2", M_ARGUS_DISPLAY_VIEW_6_2);
   fMenuViewDivide->AddEntry("7x2", M_ARGUS_DISPLAY_VIEW_7_2);
   fMenuViewDivide->AddEntry("8x2", M_ARGUS_DISPLAY_VIEW_8_2);
   fMenuViewDivide->AddEntry("9x2", M_ARGUS_DISPLAY_VIEW_9_2);
   fMenuViewDivide->AddEntry("10x2", M_ARGUS_DISPLAY_VIEW_10_2);
   fMenuViewDivide->AddEntry("1x3", M_ARGUS_DISPLAY_VIEW_1_3);
   fMenuViewDivide->AddEntry("2x3", M_ARGUS_DISPLAY_VIEW_2_3);
   fMenuViewDivide->AddEntry("3x3", M_ARGUS_DISPLAY_VIEW_3_3);
   fMenuViewDivide->AddEntry("4x3", M_ARGUS_DISPLAY_VIEW_4_3);
   fMenuViewDivide->AddEntry("5x3", M_ARGUS_DISPLAY_VIEW_5_3);
   fMenuView->AddPopup("Divide", fMenuViewDivide);
   for (i=0;i<20;i++) {
      str.SetFormatted("%d",i);
      fMenuView0_19->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW_SELECT+i);
   }
   fMenuView->AddPopup("0-19", fMenuView0_19);
   for (i=20;i<40;i++) {
      str.SetFormatted("%d",i);
      fMenuView20_39->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW_SELECT+i);
   }
   fMenuView->AddPopup("20-39", fMenuView20_39);
   for (i=40;i<60;i++) {
      str.SetFormatted("%d",i);
      fMenuView40_59->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW_SELECT+i);
   }
   fMenuView->AddPopup("40-59", fMenuView40_59);
   for (i=60;i<80;i++) {
      str.SetFormatted("%d",i);
      fMenuView60_79->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW_SELECT+i);
   }
   fMenuView->AddPopup("60-79", fMenuView60_79);

   fMenuView->AddEntry("Next", M_ARGUS_DISPLAY_VIEW_NEXT);
   fMenuView->AddEntry("Previous", M_ARGUS_DISPLAY_VIEW_PREVIOUS);
   fMenuView->AddEntry("Reset", M_ARGUS_DISPLAY_VIEW_RESET);
   fMenuView->AddEntry("Pad Configuration", M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);

   gROME->GetWindow()->GetMenuBar()->AddPopup("Display", fMenuDisplay, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   gROME->GetWindow()->GetMenuBar()->AddPopup("View", fMenuView, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

   if (fNumberOfPadsX==1 && fNumberOfPadsY==1)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==1 && fNumberOfPadsY==2)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==2)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_2_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==3)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_3_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==4)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_4_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==5)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_5_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==6)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_6_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==7)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_7_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==8)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_8_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==9)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_9_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==2 && fNumberOfPadsY==10)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_10_2,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==3 && fNumberOfPadsY==1)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_1_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==3 && fNumberOfPadsY==2)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_2_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==3 && fNumberOfPadsY==3)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_3_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==3 && fNumberOfPadsY==4)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_4_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   if (fNumberOfPadsX==3 && fNumberOfPadsY==5)
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW_5_3,M_ARGUS_DISPLAY_VIEW_1_1,M_ARGUS_DISPLAY_VIEW_5_3);
   SetupPads(fNumberOfPadsX,fNumberOfPadsY,true);
}
void ArgusHistoDisplay::BaseTabUnSelected()
{
   while (fTabActive)
      gSystem->Sleep(10);
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("Display");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("View");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("Divide");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("0-19");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("20-39");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("40-59");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("60-79");
}

void ArgusHistoDisplay::SetStatisticBox(bool flag)
{
   if (flag) {
      gStyle->SetStatW(0.1f);
      gStyle->SetOptStat(1110);
      gStyle->SetOptFit(73);
   }
   else {
      gStyle->SetOptFit(0);
      gStyle->SetOptStat(0);
   }
}

void ArgusHistoDisplay::SetStatus(int mode,const char *text,double progress,int sleepingTime)
{
   // Set status bar
   // mode 0 : initialization
   // mode 1 : set progress
   // mode 2 : finish

   fProgress->SetPosition((float)(fProgress->GetMax()*progress));
   gROME->GetWindow()->GetStatusBar()->SetText(text);

   if (mode==0) {
      gROME->GetWindow()->GetStatusBar()->GetBarPart(0)->SetBackgroundColor(TColor::RGB2Pixel(255,150,150));
      gROME->GetWindow()->GetStatusBar()->Layout();
      fProgress->Reset();
   }
   if (mode==2) {
      gROME->GetWindow()->GetStatusBar()->GetBarPart(0)->SetBackgroundColor(GetDefaultFrameBackground());
   }
   gSystem->ProcessEvents();
   gSystem->Sleep(sleepingTime);
   return;
}

void ArgusHistoDisplay::SetupPads(int nx, int ny, bool redraw)
{
   int i;

   fChannelNumber = fChannelNumber+gPad->GetNumber()-1;
   if (fChannelNumber < 0)
      fChannelNumber = 0;

   fCanvas->GetCanvas()->Clear();
   fCanvas->GetCanvas()->Divide(nx, ny);
   fCanvas->GetCanvas()->cd(1);
   fNumberOfPads = nx*ny;
   fNumberOfPadsX = nx;
   fNumberOfPadsY = ny;

   for (i=0 ; i<fNumberOfPads ; i++) {
      fCanvas->GetCanvas()->cd(i+1);
      fPad[i] = (TPad*)gPad;
      gPad->SetGridx();
      gPad->SetGridy();
      gPad->SetLeftMargin(0.08f);
      gPad->SetRightMargin(0.01f);
      gPad->SetTopMargin(1.0f);
      gPad->SetBottomMargin(1.0f);
      if (fDisplayType==kTGraphDisplay) {
         fTGraph[i]->Draw("AL");
         SetStatisticBox(false);
      }
      else if (fDisplayType==kTH1FDisplay) {
         fTH1F[i]->Draw();
         SetStatisticBox(true);
      }
      else if (fDisplayType==kTH2FDisplay) {
         fTH2F[i]->Draw();
         SetStatisticBox(true);
      }
   }

   fCanvas->GetCanvas()->cd(1);

   if (redraw) {
      Display();
   }
}

void ArgusHistoDisplay::Modified(bool processEvents)
{
   int i;
   double x1,x2,y1,y2;
   for (i=0 ; i<fNumberOfPads ; i++) {
      fPad[i]->GetRangeAxis(x1,y1,x2,y2);
      if (x1!=0 && x2!=1.1 && y1!=0 && y2!=1.1) {
         fTGraph[i]->GetXaxis()->SetRangeUser(x1,x2-1);
         fTGraph[i]->GetYaxis()->SetRangeUser(y1,y2);
         fTH1F[i]->GetXaxis()->SetRangeUser(x1,x2-1);
         fTH1F[i]->GetYaxis()->SetRangeUser(y1,y2);
      }
      if (fDisplayType==kTH1FDisplay)
         SetStatisticBox(true);
      if (fDisplayType==kTH2FDisplay)
         SetStatisticBox(true);
      fPad[i]->Modified();
   }

   fCanvas->GetCanvas()->cd(1);
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();

   fCanvas->GetCanvas()->cd(1);
   if (processEvents) {
      gSystem->ProcessEvents();
      gSystem->Sleep(10);
   }
}


