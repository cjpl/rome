// Author: Matthias Schneebeli

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ArgusHistoDisplay                                                                    //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// $Id$

#include <RConfig.h>
#include <sys/stat.h>

#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 4244)
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TLine.h>
#include <TGTextEdit.h>
#include <TGTextEntry.h>
#include <TGListBox.h>
#include <TRootEmbeddedCanvas.h>
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TSpline.h>
#include <TGProgressBar.h>
#include <TGButton.h>
#include <TGStatusBar.h>
#include <TGMenu.h>
#include <TFrame.h>
#include <TFile.h>
#include <TF1.h>
#include <TLine.h>
#include <TH1.h>
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
#include "XMLToForm.h"

ClassImp(ArgusHistoDisplay)

ArgusHistoDisplay::ArgusHistoDisplay() : ArgusTab()
{
   Int_t i,j;
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         M_ARGUS_DISPLAY_VIEW[i][j] = M_ROOT-400+i*kMaxNumberOfPadsY+j;
      }
   }
   fCurrentDisplayType = 0;
   fDisplayTypeOld = 0;
   fDisplayObjIndex = 0;
   fInherited = false;
   fInheritanceName = "";
   fPadConfigActive = false;
   fTabActive = false;
   fNumberOfPadsX = 1;
   fNumberOfPadsY = 1;
   fPadConfigActive = false;

   fNumberOfUserTGraph = 0;

   fMenuBar = 0;
   fMenuDisplay = 0;
   fMenuView = 0;
   fMenuViewDivide = 0;
   fMenuView0_19 = 0;
   fMenuView20_39 = 0;
   fMenuView40_59 = 0;
   fMenuView60_79 = 0;
   fProgress = 0;
   fStatus = 0;
   fStatusText = 0;
   fText = 0;
   fCanvas = 0;
   fDialog = 0;

   for (i = 0; i < kMaxNumberOfPadsX; i++)
      fMenuViewDivideColumn[i] = 0;

   for (i = 0; i < kMaxNumberOfPads; i++) {
      fPad[i] = 0;
   }
   fUserObjects = new TObjArray();
   fObjects = new TObjArray();
   fUserLines = new TObjArray();
   fLines = new TObjArray();
}

ArgusHistoDisplay::~ArgusHistoDisplay()
{
   Int_t i,j,k;
#if 0
   SafeDelete(fMenuBar);
   SafeDelete(fMenuDisplay);
   SafeDelete(fMenuView);
   SafeDelete(fMenuViewDivide);
   SafeDelete(fMenuView0_19);
   SafeDelete(fMenuView20_39);
   SafeDelete(fMenuView40_59);
   SafeDelete(fMenuView60_79);
   SafeDelete(fProgress);
   SafeDelete(fStatus);
   SafeDelete(fStatusText);
   SafeDelete(fText);
   SafeDelete(fCanvas);
   SafeDelete(fDialog);

   for (i = 0; i < kMaxNumberOfPadsX; i++)
      SafeDelete(fMenuViewDivideColumn[i]);
#endif

   for (i = 0; i < kMaxNumberOfPads; i++) {
      SafeDelete(fPad[i]);
   }
   // User Objects
   for (i = 0; i < fUserObjects->GetEntriesFast(); i++) {
      for (j = 0; j < ((TObjArray*)fUserObjects->At(i))->GetEntriesFast(); j++) {
         delete(((TObjArray*)fUserObjects->At(i))->At(j));
      }
      delete(fUserObjects->At(i));
   }
   SafeDelete(fUserObjects);
   // Objects
   for (i = 0; i < fObjects->GetEntriesFast(); i++) {
      for (j = 0; j < ((TObjArray*)fObjects->At(i))->GetEntriesFast(); j++) {
         delete(((TObjArray*)fObjects->At(i))->At(j));
      }
      delete(fObjects->At(i));
   }
   SafeDelete(fObjects);
   // User Lines
   for (i = 0; i < fUserLines->GetEntriesFast(); i++) {
      for (j = 0; j < ((TObjArray*)fUserLines->At(i))->GetEntriesFast(); j++) {
         for (k = 0; k < ((TObjArray*)((TObjArray*)fUserLines->At(i))->At(j))->GetEntriesFast(); k++) {
            delete(((TObjArray*)((TObjArray*)fUserLines->At(i))->At(j))->At(k));
         }
         delete(((TObjArray*)fUserLines->At(i))->At(j));
      }
      delete(fUserLines->At(i));
   }
   SafeDelete(fUserLines);
   // Lines
   for (i = 0; i < fLines->GetEntriesFast(); i++) {
      for (j = 0; j < ((TObjArray*)fLines->At(i))->GetEntriesFast(); j++) {
         for (k = 0; k < ((TObjArray*)((TObjArray*)fLines->At(i))->At(j))->GetEntriesFast(); k++) {
            delete(((TObjArray*)((TObjArray*)fLines->At(i))->At(j))->At(k));
         }
         delete(((TObjArray*)fLines->At(i))->At(j));
      }
      delete(fLines->At(i));
   }
   SafeDelete(fLines);
}

TGraph* ArgusHistoDisplay::GetUserTGraphAt(Int_t index)
{
   int i;
   for (i = 0; i < fUserObjects->GetEntriesFast(); i++) {
      if (((TObjArray*)fUserObjects->At(i))->GetEntriesFast()>index) {
         if (!strcmp(((TObjArray*)fUserObjects->At(i))->At(index)->ClassName(),"TGraph"))
            return ((TGraph*)((TObjArray*)fUserObjects->At(i))->At(index));
      }
   }
   return NULL;
}
TH1* ArgusHistoDisplay::GetUserHistoAt(Int_t index,const char* type)
{
   int i;
   for (i = 0; i < fUserObjects->GetEntriesFast(); i++) {
      if (((TObjArray*)fUserObjects->At(i))->GetEntriesFast()>index) {
         if (!strcmp(((TObjArray*)fUserObjects->At(i))->At(index)->ClassName(),type))
            return ((TH1*)((TObjArray*)fUserObjects->At(i))->At(index));
      }
   }
   return NULL;
}

TLine* ArgusHistoDisplay::GetUserLineAt(Int_t histoIndex,Int_t lineIndex)
{
   int i;
   for (i = 0; i < fUserLines->GetEntriesFast(); i++) {
      if (((TObjArray*)fUserLines->At(i))->GetEntriesFast()>histoIndex) {
         if (((TObjArray*)((TObjArray*)fUserLines->At(i))->At(histoIndex))->GetEntriesFast()>lineIndex) {
            return ((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(i))->At(histoIndex))->At(lineIndex));
         }
      }
   }
   return NULL;
}
TObject* ArgusHistoDisplay::GetCurrentObjectAt(Int_t index)
{
   if (((TObjArray*)fObjects->At(fCurrentDisplayType))->GetEntriesFast()>index) {
      return ((TObjArray*)fObjects->At(fCurrentDisplayType))->At(index);
   }
   return NULL;
}

void ArgusHistoDisplay::BaseInit()
{
   ROMEString str;

   /* Create an embedded canvas and add to the main frame, centered in x and y */
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 800, 400);

   AddFrame(fCanvas,
            new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));

   // Status bar
   Int_t parts[2] = { 70,30 };
   gROME->GetWindow()->GetStatusBar()->SetParts(parts,2);
   fProgress = new TGHProgressBar(gROME->GetWindow()->GetStatusBar()->GetBarPart(1),TGProgressBar::kStandard,20);
   fProgress->SetPosition(0);
   gROME->GetWindow()->GetStatusBar()->GetBarPart(1)->AddFrame(fProgress, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 10, 10));

   SetWindowName("DRS");

   SetSize(GetDefaultSize());
   fChannelNumber = 0;
   fDisplayTypeOld = -1;
   MapSubwindows();
   MapWindow();
   SetupPads(fNumberOfPadsX, fNumberOfPadsY,true);
}

void ArgusHistoDisplay::BaseMenuClicked(TGPopupMenu* /*menu*/,Long_t param)
{
   fTabActive = true;
   Int_t exitID=1,i,j;
   ROMEString str;
   switch (param) {
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
            fDisplayTypeOld = -1;
            SetupPads(1, 1,true);
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW[0][0],M_ARGUS_DISPLAY_VIEW[0][0],M_ARGUS_DISPLAY_VIEW[kMaxNumberOfPadsX-1][kMaxNumberOfPadsY-1]);
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
               ROMEStrArray* substitutes = new ROMEStrArray(fNumberOfPads);
               for (i=0;i<fNumberOfPads;i++) {
                  str.SetFormatted("Pad %d",i);
                  substitutes->Add(str.Data());
               }
               fDialog = new XMLToForm(gClient->GetRoot(),this,"res/xmltoform/PadConfigDia.xml",&exitID,substitutes);
               if (exitID==1) {
                  for (i=0;i<kMaxNumberOfPads;i++) {
                     str.SetFormatted("Pad %d/Channel",i);
                     fPadConfigChannel[i] = fDialog->GetIntValue(str.Data());
                  }
                  fPadConfigActive = true;
                  fDialog->SaveForm();
               }
               else
                  fMenuView->UnCheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
               delete substitutes;
            }
            break;
         }
   }
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         if (param == M_ARGUS_DISPLAY_VIEW[i][j]) {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW[i][j],M_ARGUS_DISPLAY_VIEW[0][0],M_ARGUS_DISPLAY_VIEW[kMaxNumberOfPadsX-1][kMaxNumberOfPadsY-1]);
            SetupPads(i+1, j+1,true);
            break;
         }
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
   Int_t i,j;
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
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      fMenuViewDivideColumn[i] = new TGPopupMenu(fClient->GetRoot());
      fMenuViewDivideColumn[i]->Associate(gROME->GetWindow());
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         str.SetFormatted("%dx%d",i+1,j+1);
         fMenuViewDivideColumn[i]->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW[i][j]);
      }
      str.SetFormatted("%dx.",i+1);
      fMenuViewDivide->AddPopup(str.Data(), fMenuViewDivideColumn[i]);
   }
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

   for (i=0;i<kMaxNumberOfPadsX;i++) {
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         if (fNumberOfPadsX==i+1 && fNumberOfPadsY==j+1)
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW[i][j],M_ARGUS_DISPLAY_VIEW[0][0],M_ARGUS_DISPLAY_VIEW[fNumberOfPadsX-1][fNumberOfPadsY-1]);
      }
   }
   if (fPadConfigActive)
      fMenuView->CheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
   else
      fMenuView->UnCheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);

   SetupPads(fNumberOfPadsX,fNumberOfPadsY,true);
   fChannelNumber = 0;
}
void ArgusHistoDisplay::BaseTabUnSelected()
{
   while (fTabActive)
      gSystem->Sleep(10);

   Int_t i;
   ROMEString str;

   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("Display");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("View");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("Divide");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("0-19");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("20-39");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("40-59");
   delete gROME->GetWindow()->GetMenuBar()->RemovePopup("60-79");
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      str.SetFormatted("%dx.",i+1);
      delete gROME->GetWindow()->GetMenuBar()->RemovePopup(str.Data());
   }
}

void ArgusHistoDisplay::SetStatisticBox(Bool_t flag)
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

void ArgusHistoDisplay::SetStatus(Int_t mode,const char *text,double progress,Int_t sleepingTime)
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

void ArgusHistoDisplay::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)
{
   Int_t i,k;
   Bool_t clear = true;
   if (fNumberOfPadsX==nx && fNumberOfPadsY==ny && fDisplayTypeOld==fCurrentDisplayType)
      clear = false;

   fDisplayTypeOld = fCurrentDisplayType;

   fChannelNumber = fChannelNumber+gPad->GetNumber()-1;
   if (fChannelNumber < 0)
      fChannelNumber = 0;

   if (clear) {
      fCanvas->GetCanvas()->Clear();
      fCanvas->GetCanvas()->Divide(nx, ny);
      fNumberOfPads = nx*ny;
      fNumberOfPadsX = nx;
      fNumberOfPadsY = ny;

      for (i=0 ; i<fNumberOfPads ; i++) {
         fPad[i] = (TPad*)fCanvas->GetCanvas()->GetPad(i+1);
         fPad[i]->SetGridx();
         fPad[i]->SetGridy();
         fPad[i]->SetLeftMargin(0.08f);
         fPad[i]->SetRightMargin(0.01f);
         fPad[i]->SetTopMargin(1.0f);
         fPad[i]->SetBottomMargin(1.0f);
         fPad[i]->cd();
         if (!strcmp(((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->ClassName(),"TGraph"))
            ((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->Draw("AL");
         else
            ((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->Draw();
         for (k=0;k<TMath::Min(kMaxNumberOfLines,fNumberOfUserLines);k++)
            ((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k)->Draw();
         SetStatisticBox(true);
      }
   }

   fCanvas->GetCanvas()->cd(1);

   if (redraw) {
      Display();
   }
}

void ArgusHistoDisplay::Modified(Bool_t processEvents)
{
   Int_t i;
   double x1,x2,y1,y2;

   if (!fCanvas)
      return;
   for (i=0 ; i<fNumberOfPads ; i++) {
      fPad[i]->GetRangeAxis(x1,y1,x2,y2);
      if (x1!=0 && x2!=1.1 && y1!=0 && y2!=1.1) {
         if (strcmp(((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->ClassName(),"TGraph")) {
            ((TGraph*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->GetXaxis()->SetRangeUser(x1,x2-1);
            ((TGraph*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->GetYaxis()->SetRangeUser(y1,y2);
         }
         else {
            ((TH1*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->GetXaxis()->SetRangeUser(x1,x2-1);
            ((TH1*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->GetYaxis()->SetRangeUser(y1,y2);
         }
      }
      if (strcmp(((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->ClassName(),"TGraph")) {
         SetStatisticBox(true);
      }
      fPad[i]->Modified();
   }

   fCanvas->GetCanvas()->cd(1);
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();

   if (processEvents) {
      gSystem->ProcessEvents();
      gSystem->Sleep(10);
   }
}
void ArgusHistoDisplay::SetLimits(TGraph *g)
{
   if (g->GetN()<=1)
      return;
   int i;
   double xmin,xmax,ymin,ymax,x,y;
   g->GetPoint(0,xmin,ymin);
   g->GetPoint(0,xmax,ymax);
   for (i=1;i<g->GetN();i++) {
      g->GetPoint(i,x,y);
      if (xmin>x)
         xmin = x;
      if (xmax<x)
         xmax = x;
      if (ymin>y)
         ymin = y;
      if (ymax<y)
         ymax = y;
   }
   if (xmin<xmax && ymin<ymax) {
      g->SetMinimum(ymin-(ymax-ymin)/10.);
      g->SetMaximum(ymax+(ymax-ymin)/10.);
      g->GetXaxis()->SetLimits(xmin,xmax);
   }
}


