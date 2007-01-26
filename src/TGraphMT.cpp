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
      if (!fHistogram) {
         fHistogram = new TH1F("histInTGraphMTOperator", "histInTGraphMTOperator", 2, 0, 1);
      }

      TH1F *histtmp = fHistogram;
      TList *listtmp = fFunctions;

      if (fX) {
         delete [] fX;
         fX = 0;
      }
      if (fY) {
         delete [] fY;
         fY = 0;
      }

      TGraph::operator=(gr);

      SafeDelete(histtmp);
      SafeDelete(listtmp);

      SafeDelete(fHistogram);
      if (gr.fHistogram) {
         fHistogram = new TH1F(*gr.fHistogram);
      }
   }
   return *this;
}
