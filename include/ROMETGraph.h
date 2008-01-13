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
   virtual ~ROMETGraph() {}

   void  SetPoint(Int_t i, Double_t x, Double_t y);
   TH1F* GetHistogramHandle() const { return fHistogram; }
   void  DeleteHistogram() { SafeDelete(fHistogram); }

#if (ROOT_VERSION_CODE < ROOT_VERSION(5,17,6))
   virtual void ComputeRange(Double_t& xmin, Double_t& ymin, Double_t& xmax, Double_t& ymax) const;
#endif
   virtual void SetLimits();
   virtual void Add(const TObject *obj);
   virtual void Copy(TObject &gnew) const;

   ClassDef(ROMETGraph, 1) // TGraph class for rome
};

#endif   // ROMETGraph_H
