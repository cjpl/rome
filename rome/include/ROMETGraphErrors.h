/********************************************************************
  ROMETGraphErrors.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETGraphErrors_H
#define ROMETGraphErrors_H
#include <TGraphErrors.h>
#include <TH1F.h>

class ROMETGraphErrors : public TGraphErrors
{
public:
   ROMETGraphErrors():TGraphErrors(){}
   ROMETGraphErrors(Int_t n):TGraphErrors(n){}
   ROMETGraphErrors(Int_t n, const Float_t *x, const Float_t *y, const Float_t* ex = 0, const Float_t* ey = 0):TGraphErrors(n,x,y,ex,ey){}
   ROMETGraphErrors(Int_t n, const Double_t *x, const Double_t *y, const Double_t* ex = 0, const Double_t* ey = 0):TGraphErrors(n,x,y,ex,ey){}
   ROMETGraphErrors(const TGraphErrors &gr):TGraphErrors(gr){}
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,11,4))
   ROMETGraphErrors(const TVectorF &vx, const TVectorF &vy, const TVectorF& vex, const TVectorF& vey):TGraphErrors(vx,vy,vex,vey){}
#endif
   ROMETGraphErrors(const TVectorD &vx, const TVectorD &vy, const TVectorD& vex, const TVectorD& vey):TGraphErrors(vx,vy,vex,vey){}
   ROMETGraphErrors(const TH1 *h):TGraphErrors(h){}
   ROMETGraphErrors(const char *filename, const char *format="%lg %lg", Option_t *option=""):TGraphErrors(filename,format,option){}
   ROMETGraphErrors& operator=(const ROMETGraphErrors&);
   virtual ~ROMETGraphErrors() {}

   void  SetPoint(Int_t i, Double_t x, Double_t y);
   TH1F* GetHistogramHandle() const { return fHistogram; }
   void  DeleteHistogram() { SafeDelete(fHistogram); }

   ClassDef(ROMETGraphErrors, 0) // TGraphErrors class for rome
};

#endif   // ROMETGraphErrors_H
