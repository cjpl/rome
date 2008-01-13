// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMETGraphErrors
//
//  TGraphErrors class for ROME. Fixes problems when used in multi-threaded programs and in the operator=.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMETGraphErrors.h"
#include <TList.h>

ClassImp(ROMETGraphErrors)

//______________________________________________________________________________
void ROMETGraphErrors::SetPoint(Int_t i, Double_t x, Double_t y)
{
   // Set x and y values for point number i.

   if (i < 0) return;
   if (i >= fMaxSize) {
      Double_t **ps = ExpandAndCopy(i+1, fNpoints);
      CopyAndRelease(ps, 0,0,0);
   }
   if (i >= fNpoints) {
      // points above i can be not initialized
      // set zero up to i-th point to avoid redefenition
      // of this method in descendant classes
      FillZero(fNpoints, i + 1);
      fNpoints = i+1;
   }
   fX[i] = x;
   fY[i] = y;
}

//______________________________________________________________________________
ROMETGraphErrors& ROMETGraphErrors::operator=(const ROMETGraphErrors &gr)
{
// Equal operator for this graph
   if(this!=&gr) {
      TNamed::operator=(gr);
      TAttLine::operator=(gr);
      TAttFill::operator=(gr);
      TAttMarker::operator=(gr);

      if (gr.fMaxSize == 0) {
         // deleting this pointers crashes on windows
         // I have no clue why!
         // uncommented as a temorary fix
/*         if (fX)
            delete [] fX;
         if (fY)
            delete [] fY;*/
         fX = fY = 0;
      } else if (fMaxSize != gr.fMaxSize) {
/*         if (fX)
            delete [] fX;
         if (fY)
            delete [] fY;*/
         fX = new Double_t[gr.fMaxSize];
         fY = new Double_t[gr.fMaxSize];
      }

      fNpoints = gr.fNpoints;
      fMaxSize = gr.fMaxSize;
      if (gr.fFunctions) {
         fFunctions = static_cast<TList*>(gr.fFunctions->Clone());
      } else {
         fFunctions = new TList;
      }

      if (gr.fHistogram) {
         if (!fHistogram) {
            fHistogram = new TH1F(*gr.fHistogram);
         } else {
#if 0 // this copies allmost all, but maybe too much for copying TGraph.
            gr.fHistogram->Copy(*fHistogram);
#else
            gr.fHistogram->TNamed::Copy(*fHistogram);
            fHistogram->SetMaximum(gr.fHistogram->GetMaximum());
            fHistogram->SetMinimum(gr.fHistogram->GetMinimum());
            gr.fHistogram->TAttLine::Copy(*fHistogram);
            gr.fHistogram->TAttFill::Copy(*fHistogram);
            gr.fHistogram->TAttMarker::Copy(*fHistogram);   
            gr.fHistogram->GetXaxis()->Copy(*fHistogram->GetXaxis());
            gr.fHistogram->GetYaxis()->Copy(*fHistogram->GetYaxis());
            gr.fHistogram->GetZaxis()->Copy(*fHistogram->GetZaxis());
            fHistogram->GetXaxis()->SetParent(fHistogram);
            fHistogram->GetYaxis()->SetParent(fHistogram);
            fHistogram->GetZaxis()->SetParent(fHistogram);
#endif
         }
      } else {
         SafeDelete(fHistogram);
      }
      fMinimum = gr.fMinimum;
      fMaximum = gr.fMaximum;

      if (!fMaxSize) {
         return *this;
      }

      Int_t n = gr.GetN() * sizeof(Double_t);
      if (n > 0) {
         memcpy(fX, gr.fX, n);
         memcpy(fY, gr.fY, n);
      }
   }
   return *this;
}

//______________________________________________________________________________
void ROMETGraphErrors::SetLimits()
{
   Double_t rwxmin,rwxmax, rwymin, rwymax, dx, dy;
   ComputeRange(rwxmin, rwymin, rwxmax, rwymax);

   if (rwxmin == rwxmax) rwxmax += 1.;
   if (rwymin == rwymax) rwymax += 1.;
   dx = 0.1 * (rwxmax - rwxmin);
   dy = 0.1 * (rwymax - rwymin);
   fMinimum = rwymin - dy;
   fMaximum = rwymax + dy;
   TAxis *axis = GetXaxis();

   if(axis) {
      axis->SetLimits(rwxmin - dx, rwxmax + dx);
   }
}

//______________________________________________________________________________
void ROMETGraphErrors::Add(const TObject *obj)
{
   const ROMETGraphErrors *g1 = static_cast<const ROMETGraphErrors*>(obj);
   Int_t n = g1->GetN();
   Int_t oldN = fNpoints;
   Int_t newN = fNpoints + n;

   Set(newN);
   memcpy(fX + oldN,  g1->GetX(), sizeof(Double_t) * n);
   memcpy(fY + oldN,  g1->GetY(), sizeof(Double_t) * n);
   memcpy(fEX + oldN, g1->GetEX(), sizeof(Double_t) * n);
   memcpy(fEY + oldN, g1->GetEY(), sizeof(Double_t) * n);

   SetLimits();
}

//______________________________________________________________________________
void ROMETGraphErrors::Copy(TObject &gnew) const
{
   ROMETGraphErrors *gnew1 = static_cast<ROMETGraphErrors*>(&gnew);
   gnew1->Set(0);
   gnew1->Add(this);
}
