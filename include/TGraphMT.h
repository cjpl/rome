/********************************************************************
  TGraphMT.h, M. Schneebeli PSI

  $Id: TGraphMT.h 1354 2006-08-03 21:51:20Z sawada $

********************************************************************/
#ifndef TGraphMT_H
#define TGraphMT_H
#include <TGraph.h>

class TGraphMT : public TGraph
{
public:
   TGraphMT():TGraph(){}
   TGraphMT(Int_t n):TGraph(n){}
   TGraphMT(Int_t n, const Int_t *x, const Int_t *y):TGraph(n,x,y){}
   TGraphMT(Int_t n, const Float_t *x, const Float_t *y):TGraph(n,x,y){}
   TGraphMT(Int_t n, const Double_t *x, const Double_t *y):TGraph(n,x,y){}
   TGraphMT(const TGraph &gr):TGraph(gr){}
   TGraphMT(const TVectorF &vx, const TVectorF &vy):TGraph(vx,vy){}
   TGraphMT(const TVectorD &vx, const TVectorD &vy):TGraph(vx,vy){}
   TGraphMT(const TH1 *h):TGraph(h){}
   TGraphMT(const TF1 *f, Option_t *option=""):TGraph(f,option){}
   TGraphMT(const char *filename, const char *format="%lg %lg", Option_t *option=""):TGraph(filename,format,option){}

   void SetPoint(Int_t i, Double_t x, Double_t y);

   ClassDef(TGraphMT, 0) // TGraph class for multi-threaded programs
};

#endif   // TGraphMT_H
