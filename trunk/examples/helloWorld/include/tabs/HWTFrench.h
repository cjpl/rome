////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for French tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef HWTFrench_H
#define HWTFrench_H

#include <include/tabs/HWTFrench_Base.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class HWTFrench : public HWTFrench_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   HWTFrench(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground())
      :HWTFrench_Base(p, w, h, options, back)
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   ~HWTFrench()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();

   ClassDef(HWTFrench,1)
};

#endif   // HWTFrench_H
