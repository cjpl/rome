////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for English tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef HWTEnglish_H
#define HWTEnglish_H

#include <include/tabs/HWTEnglish_Base.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class HWTEnglish : public HWTEnglish_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   HWTEnglish(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground())
      :HWTEnglish_Base(p, w, h, options, back)
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   ~HWTEnglish()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();

   ClassDef(HWTEnglish,1)
};

#endif   // HWTEnglish_H
