// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMETGraph
//
//  TGraph class for ROME. Fixes problems when used in multi-threaded programs and in the operator=.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMETGraph.h"
#include <TList.h>

ClassImp(ROMETGraph)

//______________________________________________________________________________
void ROMETGraph::SetPoint(Int_t i, Double_t x, Double_t y)
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
ROMETGraph& ROMETGraph::operator=(const ROMETGraph &gr)
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

#if (ROOT_VERSION_CODE < ROOT_VERSION(5,17,6))
//______________________________________________________________________________
void ROMETGraph::ComputeRange(Double_t& xmin, Double_t& ymin, Double_t& xmax, Double_t& ymax) const
{
   // Compute the x/y range of the points in this graph
   if (fNpoints <= 0) {
      xmin = xmax = ymin = ymax = 0;
      return;
   }
   xmin = xmax = fX[0];
   ymin = ymax = fY[0];
   for (Int_t i=1;i<fNpoints;i++) {
      if (fX[i] < xmin) xmin = fX[i];
      if (fX[i] > xmax) xmax = fX[i];
      if (fY[i] < ymin) ymin = fY[i];
      if (fY[i] > ymax) ymax = fY[i];
   }
}
#endif

//______________________________________________________________________________
void ROMETGraph::SetLimits()
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
void ROMETGraph::Add(const TObject *obj)
{
   if (!obj) {
      return;
   }
   if (!obj->InheritsFrom("TGraph")) {
      return;
   }

   const TGraph *g1 = static_cast<const TGraph*>(obj);
   Int_t n = g1->GetN();
   Int_t oldN = fNpoints;
   Int_t newN = fNpoints + n;

   Set(newN);
   memcpy(fX + oldN, g1->GetX(), sizeof(Double_t) * n);
   memcpy(fY + oldN, g1->GetY(), sizeof(Double_t) * n);

   SetLimits();
}

//______________________________________________________________________________
void ROMETGraph::Copy(TObject &gnew) const
{
   ROMETGraph *gnew1 = static_cast<ROMETGraph*>(&gnew);
   gnew1->Set(0);
   gnew1->Add(this);
}
