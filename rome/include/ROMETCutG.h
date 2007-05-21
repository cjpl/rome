/********************************************************************
  ROMETCutG.h, M. Schneebeli PSI

  $Id: ROMETCutG.h 1793 2007-04-13 10:06:35Z sawada $

********************************************************************/
#ifndef ROMETCutG_H
#define ROMETCutG_H
#include <ROMETGraph.h>
#include <TH1F.h>

class ROMETCutG : public ROMETGraph
{
public:
   ROMETCutG():ROMETGraph(){}
   ROMETCutG(Int_t n):ROMETGraph(n){}
   ROMETCutG(Int_t n, const Int_t *x, const Int_t *y):ROMETGraph(n,x,y){}
   ROMETCutG(Int_t n, const Float_t *x, const Float_t *y):ROMETGraph(n,x,y){}
   ROMETCutG(Int_t n, const Double_t *x, const Double_t *y):ROMETGraph(n,x,y){}
   ROMETCutG(const ROMETGraph &gr):ROMETGraph(gr){}
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,11,4))
   ROMETCutG(const TVectorF &vx, const TVectorF &vy):ROMETGraph(vx,vy){}
#endif
   ROMETCutG(const TVectorD &vx, const TVectorD &vy):ROMETGraph(vx,vy){}
   ROMETCutG(const TH1 *h):ROMETGraph(h){}
   ROMETCutG(const TF1 *f, Option_t *option=""):ROMETGraph(f,option){}
   ROMETCutG(const char *filename, const char *format="%lg %lg", Option_t *option=""):ROMETGraph(filename,format,option){}
   ROMETCutG& operator=(const ROMETCutG&);

   ClassDef(ROMETCutG, 0) // TGraph class for rome
};

#endif   // ROMETCutG_H
