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
ArgusHistoDisplay::ArgusHistoDisplay(ArgusWindow* window, const char* title, ROMEStrArray *drawOpt, 
                                     TArrayI *logX, TArrayI *logY, TArrayI *logZ, Int_t nUserMenus, 
                                     const char* inheritName, Int_t nDisplayType)
:ArgusTab(window, title, drawOpt, logX, logY, logZ, nUserMenus)
,fStyle(new TStyle())
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
   Int_t i, j, k;
   for (i = 0; i < kMaxNumberOfPadsX; i++) {
      for (j = 0; j < kMaxNumberOfPadsY; j++) {
         M_ARGUS_DISPLAY_VIEW[i][j] = M_ROOT - 400 + i * kMaxNumberOfPadsY + j;
      }
   }
   for (i = 0; i < kNumberOfDisplayViewSelections0; i++) {
      fMenuView100[i] = 0;
      for (j = 0; j < kNumberOfDisplayViewSelections1; j++)
         fMenuView10[i][j] = 0;
   }

   for (i = 0; i < kMaxNumberOfPadsX; i++)
      fMenuViewDivideColumn[i] = 0;

   for (i = 0; i < kMaxNumberOfPads; i++) {
      fPad[i] = 0;
   }

   // Create menus
   ROMEString str;
   fMenuDisplay = new TGPopupMenu(fClient->GetRoot());
   fMenuDisplay->Associate(fWindow);
   fMenuView = new TGPopupMenu(fClient->GetRoot());
   fMenuView->Associate(fWindow);
   fMenuViewDivide = new TGPopupMenu(fClient->GetRoot());
   fMenuViewDivide->Associate(fWindow);
   fMenuViewSelect = new TGPopupMenu(fClient->GetRoot());
   fMenuViewSelect->Associate(fWindow);
   for (i=0;i<kNumberOfDisplayViewSelections0;i++) {
      fMenuView100[i] = new TGPopupMenu(fClient->GetRoot());
      fMenuView100[i]->Associate(fWindow);
      for (j=0;j<kNumberOfDisplayViewSelections1;j++) {
         fMenuView10[i][j] = new TGPopupMenu(fClient->GetRoot());
         fMenuView10[i][j]->Associate(fWindow);
      }
   }
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      fMenuViewDivideColumn[i] = new TGPopupMenu(fClient->GetRoot());
      fMenuViewDivideColumn[i]->Associate(fWindow);
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         str.SetFormatted("%dx%d", i+1, j+1);
         fMenuViewDivideColumn[i]->AddEntry(str.Data(), M_ARGUS_DISPLAY_VIEW[i][j]);
      }
      str.SetFormatted("%dx.", i+1);
      fMenuViewDivide->AddPopup(str.Data(), fMenuViewDivideColumn[i]);
   }
   fMenuView->AddPopup("Divide", fMenuViewDivide);
   for (i=0;i<kNumberOfDisplayViewSelections0;i++) {
      for (j=0;j<kNumberOfDisplayViewSelections1;j++) {
         for (k=0;k<kNumberOfDisplayViewSelections2;k++) {
            str.SetFormatted("%d", i*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2+j*kNumberOfDisplayViewSelections2+k);
            fMenuView10[i][j]->AddEntry(str.Data(),
                                        M_ARGUS_DISPLAY_VIEW_SELECT+i*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2+j*kNumberOfDisplayViewSelections2+k);
         }
         str.SetFormatted("%d-%d",
                          i*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2+j*kNumberOfDisplayViewSelections2,
                          i*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2+(j+1)*kNumberOfDisplayViewSelections2-1);
         fMenuView100[i]->AddPopup(str.Data(), fMenuView10[i][j]);
      }
      str.SetFormatted("%d-%d", i*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2, (i+1)*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2-1);
      fMenuViewSelect->AddPopup(str.Data(), fMenuView100[i]);
   }
   fMenuView->AddPopup("Select", fMenuViewSelect);

   fMenuView->AddEntry("Next", M_ARGUS_DISPLAY_VIEW_NEXT);
   fMenuView->AddEntry("Previous", M_ARGUS_DISPLAY_VIEW_PREVIOUS);
   fMenuView->AddEntry("Reset", M_ARGUS_DISPLAY_VIEW_RESET);
   fMenuView->AddEntry("Pad Configuration", M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);

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
      fDisplayObjLoaded->AddAt(0, i);
      fDrawOption->AddLast("");
      fLogScaleX->AddAt(0, i);
      fLogScaleY->AddAt(0, i);
      fLogScaleZ->AddAt(0, i);
   }
}

//______________________________________________________________________________
ArgusHistoDisplay::~ArgusHistoDisplay()
{
   Int_t i, iMax, j, jMax, k, kMax;;

#if 0
   SafeDelete(fStyle);
//   SafeDelete(fMenuBar);
   SafeDelete(fMenuDisplay);
   SafeDelete(fMenuView);
   SafeDelete(fMenuViewDivide);
   for (i=0;i<kNumberOfDisplayViewSelections0;i++) {
      SafeDelete(fMenuView100[i]);
      for (j=0;j<kNumberOfDisplayViewSelections1;j++)
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

   TObjArray *ptr1;
   TObjArray *ptr2;

   // User Objects
   iMax = fUserObjects->GetEntriesFast();
   for (i = 0; i < iMax; i++) {
      ptr1 = static_cast<TObjArray*>(fUserObjects->At(i));
      jMax = ptr1->GetEntriesFast();
      for (j = 0; j < jMax; j++) {
         delete ptr1->At(j);
      }
      delete ptr1;
   }
   SafeDelete(fUserObjects);

   // Objects
   iMax = fObjects->GetEntriesFast();
   for (i = 0; i < iMax; i++) {
      ptr1 = static_cast<TObjArray*>(fObjects->At(i));
      jMax = ptr1->GetEntriesFast();
      for (j = 0; j < jMax; j++) {
         delete ptr1->At(j);
      }
      delete ptr1;
   }
   SafeDelete(fObjects);

   // User Lines
   iMax = fUserLines->GetEntriesFast();
   for (i = 0; i < iMax; i++) {
      ptr1 = static_cast<TObjArray*>(fUserLines->At(i));
      jMax = ptr1->GetEntriesFast();
      for (j = 0; j < jMax; j++) {
         ptr2 = static_cast<TObjArray*>(ptr1->At(j));
         kMax = ptr2->GetEntriesFast();
         for (k = 0; k < kMax; k++) {
            delete ptr2->At(k);
         }
         delete ptr2;
      }
      delete ptr1;
   }
   SafeDelete(fUserLines);

   // Lines
   iMax = fLines->GetEntriesFast();
   for (i = 0; i < iMax; i++) {
      ptr1 = static_cast<TObjArray*>(fLines->At(i));
      jMax = ptr1->GetEntriesFast();
      for (j = 0; j < jMax; j++) {
         ptr2 = static_cast<TObjArray*>(ptr1->At(j));
         kMax = ptr2->GetEntriesFast();
         for (k = 0; k < kMax; k++) {
            delete ptr2->At(k);
         }
         delete ptr2;
      }
      delete ptr1;
   }
   SafeDelete(fLines);
}

//______________________________________________________________________________
ROMETGraph* ArgusHistoDisplay::GetUserTGraphAt(Int_t indx)
{
   Int_t i;
   const Int_t nObjects = fUserObjects->GetEntriesFast();
   TObjArray  *ptr1;
   ROMETGraph *ptr2;
   for (i = 0; i < nObjects; i++) {
      ptr1 = static_cast<TObjArray*>(fUserObjects->At(i));
      if (ptr1->GetEntriesFast() > indx) {
         ptr2 = static_cast<ROMETGraph*>(ptr1->At(indx));
         if (!strcmp(ptr2->ClassName(), "ROMETGraph")) {
            return ptr2;
         }
      }
   }
   return NULL;
}

//______________________________________________________________________________
TH1* ArgusHistoDisplay::GetUserHistoAt(Int_t indx, const char* type)
{
   int i;
   const Int_t nObjects = fUserObjects->GetEntriesFast();
   TObjArray  *ptr1;
   TH1        *ptr2;
   for (i = 0; i < nObjects; i++) {
      ptr1 = static_cast<TObjArray*>(fUserObjects->At(i));
      if (ptr1->GetEntriesFast() > indx) {
         ptr2 = static_cast<TH1*>(ptr1->At(indx));
         if (!strcmp(ptr2->ClassName(), type)) {
            return ptr2;
         }
      }
   }
   return NULL;
}

//______________________________________________________________________________
TLine* ArgusHistoDisplay::GetUserLineAt(Int_t histoIndex, Int_t lineIndex)
{
   int i;
   const Int_t nLines = fUserLines->GetEntriesFast();
   TObjArray *ptr1;
   TObjArray *ptr2;
   for (i = 0; i < nLines; i++) {
      ptr1 = static_cast<TObjArray*>(fUserLines->At(i));
      if (ptr1->GetEntriesFast() > histoIndex) {
         ptr2 = static_cast<TObjArray*>(ptr1->At(histoIndex));
         if (ptr2->GetEntriesFast() > lineIndex) {
            return static_cast<TLine*>(ptr2->At(lineIndex));
         }
      }
   }
   return NULL;
}

//______________________________________________________________________________
TObject* ArgusHistoDisplay::GetCurrentObjectAt(Int_t indx, Int_t typeIndex)
{
   TObjArray *ptr1 = static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(typeIndex)));
   if (ptr1->GetEntriesFast() > indx) {
      return ptr1->At(indx);
   }
   return NULL;
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseInit()
{
   ROMEString str;

   /* Create an embedded canvas and add to the main frame, centered in x and y */
   fCanvas = new TRootEmbeddedCanvas("Canvas", this, 800, 400);

   AddFrame(fCanvas, new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));

   fChannelNumber = 0;
   fDisplayTypeOld = -1;
   SetupPads(fNumberOfPadsX, fNumberOfPadsY, true);
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseMenuClicked(TGPopupMenu* /*menu*/, Long_t param)
{
   fBusy = true;
   Int_t exitID=1, i, j;
   ROMEString str;
   switch (param) {
   case M_ARGUS_DISPLAY_VIEW_NEXT:
      fChannelNumber += fNumberOfPads;
      Display();
      break;
   case M_ARGUS_DISPLAY_VIEW_PREVIOUS:
      fChannelNumber -= fNumberOfPads;
      if (fChannelNumber < 0) {
         fChannelNumber = 0;
      }
      Display();
      break;
   case M_ARGUS_DISPLAY_VIEW_RESET:
      fChannelNumber = 0;
      fDisplayTypeOld = -1;
      SetupPads(1, 1, true);
      fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW[0][0], M_ARGUS_DISPLAY_VIEW[0][0], M_ARGUS_DISPLAY_VIEW[kMaxNumberOfPadsX-1][kMaxNumberOfPadsY-1]);
      break;
   case M_ARGUS_DISPLAY_VIEW_PAD_CONFIG:
      if (fPadConfigActive) {
         fMenuView->UnCheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
         fPadConfigActive = false;
      } else {
         fMenuView->CheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
         ROMEStrArray* substitutes = new ROMEStrArray(fNumberOfPads);
         for (i=0;i<fNumberOfPads;i++) {
            str.SetFormatted("Pad %d", i);
            substitutes->Add(str.Data());
         }
         fDialog = new XMLToForm(gClient->GetRoot(), this, "res/xmltoform/PadConfigDia.xml", &exitID, substitutes);
         if (exitID==1) {
            for (i=0;i<kMaxNumberOfPads;i++) {
               str.SetFormatted("Pad %d/Object Index", i);
               fPadConfigChannel[i] = fDialog->GetIntValue(str.Data());
            }
            fPadConfigActive = true;
            fDialog->SaveForm();
         } else {
            fMenuView->UnCheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
         }
         delete substitutes;
      }
      break;
   }
   for (i=0;i<kMaxNumberOfPadsX;i++) {
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         if (param == M_ARGUS_DISPLAY_VIEW[i][j]) {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW[i][j], M_ARGUS_DISPLAY_VIEW[0][0], M_ARGUS_DISPLAY_VIEW[kMaxNumberOfPadsX-1][kMaxNumberOfPadsY-1]);
            SetupPads(i+1, j+1, true);
            break;
         }
      }
   }
   for (i=0;i<kNumberOfDisplayViewSelections0*kNumberOfDisplayViewSelections1*kNumberOfDisplayViewSelections2;i++) {
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
   Int_t i, j;

   // Display menus
   fWindow->GetMenuBar()->AddPopup("Display", fMenuDisplay, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
   fWindow->GetMenuBar()->AddPopup("View", fMenuView, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));

   for (i=0;i<kMaxNumberOfPadsX;i++) {
      for (j=0;j<kMaxNumberOfPadsY;j++) {
         if (fNumberOfPadsX==i+1 && fNumberOfPadsY==j+1) {
            fMenuView->RCheckEntry(M_ARGUS_DISPLAY_VIEW[i][j], M_ARGUS_DISPLAY_VIEW[0][0], M_ARGUS_DISPLAY_VIEW[fNumberOfPadsX-1][fNumberOfPadsY-1]);
         }
      }
   }

   if (fPadConfigActive) {
      fMenuView->CheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
   } else {
      fMenuView->UnCheckEntry(M_ARGUS_DISPLAY_VIEW_PAD_CONFIG);
   }
   SetupPads(fNumberOfPadsX, fNumberOfPadsY, true);
   fChannelNumber = 0;
}

//______________________________________________________________________________
void ArgusHistoDisplay::BaseTabUnSelected()
{
   while (fBusy) {
      gSystem->Sleep(10);
   }

   fWindow->GetMenuBar()->RemovePopup("Display");
   fWindow->GetMenuBar()->RemovePopup("View");
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
   if (fDisplayObjIndex>=fNumberOfDisplayTypes) {
      return;
   }

   Int_t i, j, k, kMax;
   TObject   *ptr1;
   TObjArray *ptr2;
   ROMEString str;

   fStyle->cd();
   fDisplayTypeOld = fCurrentDisplayType->At(0);

   fChannelNumber = fChannelNumber+gPad->GetNumber()-1;
   if (fChannelNumber < 0) {
      fChannelNumber = 0;
   }

   if (fNumberOfPads<=0 || fNumberOfPadsX!=nx || fNumberOfPadsY!=ny || fDisplayTypeOld!=fCurrentDisplayType->At(0)) {
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
            ptr1 = static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i);
            if (!strcmp(ptr1->ClassName(), "ROMETGraph") || 
                !strcmp(ptr1->ClassName(), "ROMETCutG")) {
               if (j==0) {
                  ptr1->Draw("A L");
               } else {
                  ptr1->Draw("L SAME");
               }
            } else {
               if (j==0) {
                  ptr1->Draw(fDrawOption->At(fDisplayObjIndex).Data());
               } else {
                  str = fDrawOption->At(fDisplayObjIndex)+" SAME";
                  ptr1->Draw(str.Data());
               }
            }
         }

         ptr2 = static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->At(fCurrentDisplayType->At(0)))->At(i));
         kMax = ptr2->GetEntriesFast();
         for (k = 0; k < TMath::Min(kMax, fNumberOfUserLines);k++) {
            ptr2->At(k)->Draw();
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
   if (!fCanvas) {
      return;
   }

   Int_t i, j;
   double x1, x2, y1, y2;
   TObject *ptr;

   fStyle->cd();

   for (i = 0; i < fNumberOfPads; i++) {
      fPad[i]->GetRangeAxis(x1, y1, x2, y2);
      for (j = 0; j < fNumberOfCurrentDisplayTypes; j++) {
         ptr = static_cast<TObjArray*>(fObjects->At(fCurrentDisplayType->At(j)))->At(i);
         if (x1 != 0 && TMath::Abs((1.1 - x2) / 1.1) > 1e-6 && y1 !=0 && TMath::Abs((1.1 - y2)/1.1) > 1e-6) {
            if (!strcmp(ptr->ClassName(), "ROMETGraph") ||
                !strcmp(ptr->ClassName(), "ROMETCutG")) {
               static_cast<ROMETGraph*>(ptr)->GetXaxis()->SetRangeUser(x1, x2);
               static_cast<ROMETGraph*>(ptr)->GetYaxis()->SetRangeUser(y1, y2);
            } else {
               static_cast<TH1*>(ptr)->GetXaxis()->SetRangeUser(x1, x2);
               static_cast<TH1*>(ptr)->GetYaxis()->SetRangeUser(y1, y2);
            }
         }
         if (!strcmp(ptr->ClassName(), "ROMETGraph") ||
             !strcmp(ptr->ClassName(), "ROMETCutG")) {
            SetStatisticBox(true);
            // this allows changing X range
            fPad[i]->cd();
            SetLimits(static_cast<ROMETGraph*>(ptr));
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
   const Int_t n = g->GetN();
   if (n<=1) {
      return;
   }
   int i;
   double xmin, xmax, ymin, ymax;
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
      if(axis) {
         axis->SetLimits(xmin, xmax);
      }
   }
}


