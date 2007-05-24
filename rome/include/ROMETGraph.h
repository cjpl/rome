/********************************************************************
  ROMETGraph.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETGraph_H
#define ROMETGraph_H
#include <TGraph.h>
#include <TH1F.h>

class ROMETGraph : public TGraph
{
public:
   ROMETGraph():TGraph(){}
   ROMETGraph(Int_t n):TGraph(n){}
   ROMETGraph(Int_t n, const Int_t *x, const Int_t *y):TGraph(n,x,y){}
   ROMETGraph(Int_t n, const Float_t *x, const Float_t *y):TGraph(n,x,y){}
   ROMETGraph(Int_t n, const Double_t *x, const Double_t *y):TGraph(n,x,y){}
   ROMETGraph(const TGraph &gr):TGraph(gr){}
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,11,4))
   ROMETGraph(const TVectorF &vx, const TVectorF &vy):TGraph(vx,vy){}
#endif
   ROMETGraph(const TVectorD &vx, const TVectorD &vy):TGraph(vx,vy){}
   ROMETGraph(const TH1 *h):TGraph(h){}
   ROMETGraph(const TF1 *f, Option_t *option=""):TGraph(f,option){}
   ROMETGraph(const char *filename, const char *format="%lg %lg", Option_t *option=""):TGraph(filename,format,option){}
   ROMETGraph& operator=(const ROMETGraph&);

   void  SetPoint(Int_t i, Double_t x, Double_t y);
   TH1F* GetHistogramHandle() { return fHistogram; }
   void  DeleteHistogram() { SafeDelete(fHistogram); }

   ClassDef(ROMETGraph, 0) // TGraph class for rome
};

#endif   // ROMETGraph_H