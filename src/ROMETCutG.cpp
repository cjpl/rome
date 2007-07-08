// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMETCutG
//
//  ROMETCutG class for ROME. Fixes problems in the operator=.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMETCutG.h"
#include <TList.h>

ClassImp(ROMETCutG)

//______________________________________________________________________________
ROMETCutG& ROMETCutG::operator=(const ROMETCutG &gr)
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
