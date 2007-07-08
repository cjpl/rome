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
#include <ROMETGraph.h>
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
#include <TArrayI.h>
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
#include "ROMEUtilities.h"
#include "ROMEStrArray.h"
#include "ROMEAnalyzer.h"
#include "ArgusHistoDisplay.h"
#include "XMLToForm.h"

ClassImp(ArgusHistoDisplay)

//______________________________________________________________________________
ArgusHistoDisplay::ArgusHistoDisplay(ArgusWindow* window, ROMEStrArray *drawOpt, TArrayI *logX, TArrayI *logY, TArrayI *logZ,
                                     const char* inheritName, Int_t nDisplayType)
:ArgusTab(window, drawOpt, logX, logY, logZ)
,fStyle(new TStyle())
,fMenuBar(0)
,fMenuDisplay(0)
,fMenuView(0)
,fMenuViewDivide(0)
,fMenuViewSelect(0)
,fNumberOfCurrentDisplayTypes(0)
,fCurrentDisplayType(0)
,fNumberOfDisplayTypes(nDisplayType)
,fDisplayTypeOld(0)
,fDisplayObjIndex(0)
,fDisplayObjLoaded(0)
,fInherited(kFALSE)
,fInheritanceName(inheritName)
,fTabActive(kFALSE)
,fUserObjects(new TObjArray())
,fObjects(new TObjArray())
,fNumberOfObjects(0)
,fUserLines(new TObjArray())
,fLines(new TObjArray())
,fNumberOfUserTGraph(0)
,fNumberOfUserLines(0)
,fNumberOfLines(0)
,fNumberOfPads(0)
,fNumberOfPadsX(1)
,fNumberOfPadsY(1)
,fStatisticBoxFlag(kFALSE)
,fCanvas(0)
,fDialog(0)
,fChannelNumber(0)
,fPadConfigActive(0)
{
   Int_t i,j;
   for (i = 0; i < kMaxNumberOfPadsX; i++) {
      for (j = 0; j < kMaxNumberOfPadsY; j++) {
         M_ARGUS_DISPLAY_VIEW[i][j] = M_ROOT - 400 + i * kMaxNumberOfPadsY + j;
      }
   }
   for (i = 0; i < 10; i++) {
      fMenuView100[i] = 0;
      for (j = 0; j < 10; j++)
         fMenuView10[i][j] = 0;
   }

   for (i = 0; i < kMaxNumberOfPadsX; i++)
      fMenuViewDivideColumn[i] = 0;

   for (i = 0; i < kMaxNumberOfPads; i++) {
      fPad[i] = 0;
   }
}

//______________________________________________________________________________
void ArgusHistoDisplay::InitHistoDisplay() 
{
   int i;
   fDisplayObjLoaded = new TArrayI(fNumberOfDisplayTypes);
   fNumberOfCurrentDisplayTypes = 1;
   fCurrentDisplayType = new TArrayI(fNumberOfDisplayTypes);
   fDrawOption = new ROMEStrArray();
   fLogScaleX = new TArrayI(fNumberOfDisplayTypes);
   fLogScaleY = new TArrayI(fNumberOfDisplayTypes);
   fLogScaleZ = new TArrayI(fNumberOfDisplayTypes);
   for (i=0;i<fNumberOfDisplayTypes;i++) {
      fDisplayObjLoaded->AddAt(0,i);
      fDrawOption->AddLast("");
      fLogScaleX->AddAt(0,i);
      fLogScaleY->AddAt(0,i);
      fLogScaleZ->AddAt(0,i);
   }
}

//______________________________________________________________________________
ArgusHistoDisplay::~ArgusHistoDisplay()
{
   Int_t i,j,k;
#if 0
   SafeDelete(fStyle);
   SafeDelete(fMenuBar);
   SafeDelete(fMenuDisplay);
   SafeDelete(fMenuView);
   SafeDelete(fMenuViewDivide);
   for (i=0;i<10;i++) {
      SafeDelete(fMenuView100[i]);
      for (j=0;j<10;j++)
         SafeDelete(fMenuView10[i][j]);
   }
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
      for (j = 0; j < static_cast<TObjArray*>(fUserObjects->At(i))->GetEntriesFast(); j++) {
         delete static_cast<TObjArray*>(fUserObjects->At(i))->At(j);
      }
      delete(fUserObjects->At(i));
   }
   SafeDelete(fUserObjects);
   // Objects
   for (i = 0; i < fObjects->GetEntriesFast(); i++) {
      for (j = 0; j < static_cast<TObjArray*>(fObjects->At(i))->GetEntriesFast(); j++) {
         delete static_cast<TObjArray*>(fObjects->At(i))->At(j);
      }
      delete(fObjects->At(i));
   }
   SafeDelete(fObjects);
   // User Lines
   for (i = 0; i < fUserLines->GetEntriesFast(); i++) {
      for (j = 0; j < static_cast<TObjArray*>(fUserLines->At(i))->GetEntriesFast(); j++) {
         for (k = 0; k < static_cast<TObjArray*>(static_cast<TObjArray*>(fUserLines->At(i))->At(j))->GetEntriesFast(); k++) {
            delete static_cast<TObjArray*>(static_cast<TObjArray*>(fUserLines->At(i))->At(j))->At(k);
         }
         delete static_cast<TObjArray*>(fUserLines->At(i))->At(j);
      }
      delete(fUserLines->At(i));
   }
   SafeDelete(fUserLines);
   // Lines
   for (i = 0; i < fLines->GetEntriesFast(); i++) {
      for (j = 0; j < static_cast<TObjArray*>(fLines->At(i))->GetEntriesFast(); j++) {
         for (k = 0; k < static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->At(i))->At(j))->GetEntriesFast(); k++) {
            delete static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->At(i))->At(j))->At(k);
         }
         delete static_cast<TObjArray*>(fLines->At(i))->At(j);
      }
      delete(fLines->At(i));
   }
   SafeDelete(fLines);
}

//______________________________________________________________________________
ROMETGraph* ArgusHistoDisplay::GetUserTGraphAt(Int_t indx)
{
   int i;
   for (i = 0; i < fUserObjects->GetEntriesFast(); i++) {
      if (static_cast<TObjArray*>(fUserObjects->At(i))->GetEntriesFast() > indx) {
         if (!strcmp(static_cast<TObjArray*>(fUserObjects->At(i))->At(indx)->ClassName(),"ROMETGraph"))
            return static_cast<ROMETGraph*>(static_cast<TObjArray*>(fUserObjects->At(i))->At(indx));
      }
   }
   return NULL;
}

//______________________________________________________________________________
TH1* ArgusHistoDisplay::GetUserHistoAt(Int_t indx,const char* type)
{
   int i;
   for (i = 0; i < fUserObjects->GetEntriesFast(); i++) {
      if (static_cast<TObjArray*>(fUserObjects->At(i))->GetEntriesFast() > indx) {
         if (!strcmp(static_cast<TObjArray*>(fUserObjects->At(i))->At(indx)->ClassName(), type))
            return static_cast<TH1*>(static_cast<TObjArray*>(fUserObjects->At(i))->At(indx));
      }
   }
   return NULL;
}

//______________________________________________________________________________
TLine* ArgusHistoDisplay::GetUserLineAt(Int_t histoIndex,Int_t lineIndex)
{
   int i;
   for (i = 0; i < fUserLines->GetEntriesFast(); i++) {
      if (static_cast<TObjArray*>(fUserLines->At(i))->GetEntriesFast() > histoIndex) {
         if (static_cast<TObjArray*>(static_cast<TObjArray*>(fUserLines->At(i))->At(histoIndex))->GetEntriesFast() > lineIndex) {
            return static_cast<TLine*>(static_cast<TObjArray*>(static_cast<TObjArray*>(fUserLines->At(i))->At(histoIndex))->At(lineIndex));
         }
      }
   }
   return NULL;
}

//______________________________________________________________________________
TObject* ArgusHistoDisplay::GetCurrentObjectAt(Int_t indx,Int_t typeIndex)
{
   if (static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(typeIndex)))->GetEntriesFast() > indx) {
      return static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(typeIndex)))->At(indx);
   }
   return NULL;
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseInit()
{
   ROMEString str;

   /* Create an embedded canvas and add to the main frame, centered in x and y */
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 800, 400);

   AddFrame(fCanvas,
            new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));

   SetWindowName("DRS");

   SetSize(GetDefaultSize());
   fChannelNumber = 0;
   fDisplayTypeOld = -1;
   MapSubwindows();
   MapWindow();
   SetupPads(fNumberOfPadsX, fNumberOfPadsY,true);
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseMenuClicked(TGPopupMenu* /*menu*/,Long_t param)
{
   fBusy = true;
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
                     str.SetFormatted("Pad %d/Object Index",i);
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
   for (i=0;i<kNumberOfDisplayViewSelections;i++) {
      if (param == M_ARGUS_DISPLAY_VIEW_SELECT+i) {
         fChannelNumber = i;
         Display();
         break;
      }
   }
   fBusy = false;
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseTabSelected()
{
   Int_t i,j,k;
   ROMEString str;
   // Create menu

   fMenuDisplay = new TGPopupMenu(fClient->GetRoot());
   fMenuDisplay->Associate(fWindow);
   fMenuView = new TGPopupMenu(fClient->GetRoot());
   fMenuView->Associate(fWindow);
   fMenuViewDivide = new TGPopupMenu(fClient->GetRoot());
   fMenuViewDivide->Associate(fWindow);
   fMenuViewSelect = new TGPopupMenu(fClient->GetRoot());
   fMenuViewSelect->Associate(fWindow);
   for (i=0;i<10;i++) {
      fMenuView100[i] = new TGPopupMenu(fClient->GetRoot());
      fMenuView100[i]->Associate(fWindow);
      for (j=0;j<10;j++) {
         fMenuView10[i][j] = new TGPopupMenu(fClient->GetRoot());
         fMenuView10[i][j]->Associate(fWindow);
      }
   }
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      fMenuViewDivideColumn[i] = new TGPopupMenu(fClient->GetRoot());
      fMenuViewDivideColumn[i]->Associate(fWindow);
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         str.SetFormatted("%dx%d",i+1,j+1);
         fMenuViewDivideColumn[i]->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW[i][j]);
      }
      str.SetFormatted("%dx.",i+1);
      fMenuViewDivide->AddPopup(str.Data(), fMenuViewDivideColumn[i]);
   }
   fMenuView->AddPopup("Divide", fMenuViewDivide);
   for (i=0;i<10;i++) {
      for (j=0;j<10;j++) {
         for (k=0;k<10;k++) {
            str.SetFormatted("%d",i*100+j*10+k);
            fMenuView10[i][j]->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW_SELECT+i*100+j*10+k);
         }
         str.SetFormatted("%d-%d",i*100+j*10,i*100+(j+1)*10-1);
         fMenuView100[i]->AddPopup(str.Data(), fMenuView10[i][j]);
      }
      str.SetFormatted("%d-%d",i*100,(i+1)*100-1);
      fMenuViewSelect->AddPopup(str.Data(), fMenuView100[i]);
   }
   fMenuView->AddPopup("Select", fMenuViewSelect);

   fMenuView->AddEntry("Next", M_ARGUS_DISPLAY_VIEW_NEXT);
   fMenuView->AddEntry("Previous", M_ARGUS_DISPLAY_VIEW_PREVIOUS);
   fMenuView->AddEntry("Reset", M_ARGUS_DISPLAY_VIEW_RESET);
   fMenuView->AddEntry("Pad Configuration", M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);

   fWindow->GetMenuBar()->AddPopup("Display", fMenuDisplay, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   fWindow->GetMenuBar()->AddPopup("View", fMenuView, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

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

//______________________________________________________________________________
void ArgusHistoDisplay::BaseTabUnSelected()
{
   while (fBusy)
      gSystem->Sleep(10);

   Int_t i;
   ROMEString str;

   delete fWindow->GetMenuBar()->RemovePopup("Display");
   delete fWindow->GetMenuBar()->RemovePopup("View");
   delete fWindow->GetMenuBar()->RemovePopup("Divide");
   delete fWindow->GetMenuBar()->RemovePopup("0-19");
   delete fWindow->GetMenuBar()->RemovePopup("20-39");
   delete fWindow->GetMenuBar()->RemovePopup("40-59");
   delete fWindow->GetMenuBar()->RemovePopup("60-79");
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      str.SetFormatted("%dx.",i+1);
      delete fWindow->GetMenuBar()->RemovePopup(str.Data());
   }
}

//______________________________________________________________________________
void ArgusHistoDisplay::SetStatisticBox(Bool_t flag)
{
   if (flag && !fStatisticBoxFlag) {
      fStyle->SetStatW(0.1f);
      fStyle->SetOptStat(1110);
      fStyle->SetOptFit(73);
   }
   else if (!flag && fStatisticBoxFlag){
      fStyle->SetOptFit(0);
      fStyle->SetOptStat(0);
   }
   fStatisticBoxFlag = flag;
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseSetupPads(Int_t nx, Int_t ny, Bool_t redraw)
{
   Int_t i,j,k;
   ROMEString str;
   if (fDisplayObjIndex>=fNumberOfDisplayTypes)
      return;

   Bool_t clear = true;
   if (fNumberOfPadsX==nx && fNumberOfPadsY==ny && fDisplayTypeOld==fCurrentDisplayType->At(0))
      clear = false;

   fStyle->cd();
   fDisplayTypeOld = fCurrentDisplayType->At(0);

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
         fPad[i] = static_cast<TPad*>(fCanvas->GetCanvas()->GetPad(i+1));
         fPad[i]->SetGridx();
         fPad[i]->SetGridy();
         fPad[i]->SetLeftMargin(0.08f);
         fPad[i]->SetRightMargin(0.01f);
         fPad[i]->SetTopMargin(1.0f);
         fPad[i]->SetBottomMargin(1.0f);
         fPad[i]->SetLogx(fLogScaleX->At(fDisplayObjIndex));
         fPad[i]->SetLogy(fLogScaleY->At(fDisplayObjIndex));
         fPad[i]->SetLogz(fLogScaleZ->At(fDisplayObjIndex));
         fPad[i]->cd();
         for (j=0;j<fNumberOfCurrentDisplayTypes;j++) {
            if (!strcmp(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->ClassName(),"ROMETGraph") ||
               !strcmp(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->ClassName(),"ROMETCutG")) {
               if (j==0)
                  static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->Draw("A L");
               else
                  static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->Draw("L SAME");
            }
            else {
               if (j==0)
                  static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->Draw(fDrawOption->At(fDisplayObjIndex).Data());
               else {
                  str = fDrawOption->At(fDisplayObjIndex)+" SAME";
                  static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->Draw(str.Data());
               }
            }
         }
         for (k = 0; k < TMath::Min(static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->At(fCurrentDisplayType->At(0)))->At(i))->GetEntriesFast(),
                                    fNumberOfUserLines);k++) {
            static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->At(fCurrentDisplayType->At(0)))->At(i))->At(k)->Draw();
         }
         SetStatisticBox(true);
      }
   }

   fCanvas->GetCanvas()->cd(1);

   if (redraw) {
      Display();
   }
}

//______________________________________________________________________________
void ArgusHistoDisplay::Modified(Bool_t processEvents)
{
   Int_t i,j;
   double x1,x2,y1,y2;

   if (!fCanvas)
      return;

   fStyle->cd();

   for (i = 0; i < fNumberOfPads; i++) {
      fPad[i]->GetRangeAxis(x1, y1, x2, y2);
      for (j = 0; j < fNumberOfCurrentDisplayTypes; j++) {
         if (x1 != 0 && TMath::Abs((1.1 - x2) / 1.1) > 1e-6 && y1 !=0 && TMath::Abs((1.1 - y2)/1.1) > 1e-6) {
            if (!strcmp(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->ClassName(), "ROMETGraph") ||
                !strcmp(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->ClassName(), "ROMETCutG")) {
               static_cast<ROMETGraph*>(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i))->GetXaxis()->SetRangeUser(x1, x2);
               static_cast<ROMETGraph*>(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i))->GetYaxis()->SetRangeUser(y1, y2);
            } else {
               static_cast<TH1*>(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i))->GetXaxis()->SetRangeUser(x1, x2);
               static_cast<TH1*>(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i))->GetYaxis()->SetRangeUser(y1, y2);
            }
         }
         if (!strcmp(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->ClassName(), "ROMETGraph") ||
             !strcmp(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i)->ClassName(), "ROMETCutG")) {
            SetStatisticBox(true);
            // this allows changing X range
            fPad[i]->cd();
            ROMETGraph *pgraph = static_cast<ROMETGraph*>(static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i));
            SetLimits(pgraph);
         }
      }
      fPad[i]->Modified();
   }

   fCanvas->GetCanvas()->cd(1);
   fCanvas->GetCanvas()->Modified();
   fCanvas->GetCanvas()->Update();

   if (processEvents) {
      gROME->GetApplication()->DisableFPETrap();
      gSystem->ProcessEvents();
      gROME->GetApplication()->EnableFPETrap();
      gSystem->Sleep(10);
   }
}

//______________________________________________________________________________
void ArgusHistoDisplay::SetLimits(ROMETGraph *g)
{
   Int_t n = g->GetN();
   if (n<=1)
      return;
   int i;
   double xmin,xmax,ymin,ymax;
   double *x;
   double *y;
   x = g->GetX();
   y = g->GetY();
   xmin = xmax = x[0];
   ymin = ymax = y[0];
   for (i=1;i<n;i++) {
      if (xmin>x[i])
         xmin = x[i];
      if (xmax<x[i])
         xmax = x[i];
      if (ymin>y[i])
         ymin = y[i];
      if (ymax<y[i])
         ymax = y[i];
   }
   if (xmin<xmax && ymin<ymax) {
      g->SetMinimum(ymin-(ymax-ymin)/10.);
      g->SetMaximum(ymax+(ymax-ymin)/10.);
      TAxis *axis = g->GetXaxis();
      if(axis)
         axis->SetLimits(xmin,xmax);
   }
}


