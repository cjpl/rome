// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  TGraphMT
//
//  TGraph class for multi-threaded programs.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "TGraphMT.h"
#include <TList.h>

ClassImp(TGraphMT)

void TGraphMT::SetPoint(Int_t i, Double_t x, Double_t y)
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
TGraphMT& TGraphMT::operator=(const TGraphMT &gr)
{
// Equal operator for this graph
   if(this!=&gr) {
      TNamed::operator=(gr);
      TAttLine::operator=(gr);
      TAttFill::operator=(gr);
      TAttMarker::operator=(gr);

      if (gr.fMaxSize == 0) {
         if (fX)
            delete [] fX;
         if (fY)
            delete [] fY;
         fX = fY = 0;
      } else if (fMaxSize != gr.fMaxSize) {
         if (fX)
            delete [] fX;
         if (fY)
            delete [] fY;
         fX = new Double_t[gr.fMaxSize];
         fY = new Double_t[gr.fMaxSize];
      }

      fNpoints = gr.fNpoints;
      fMaxSize = gr.fMaxSize;
      if (gr.fFunctions) fFunctions = (TList*)gr.fFunctions->Clone();
      else fFunctions = new TList;

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
