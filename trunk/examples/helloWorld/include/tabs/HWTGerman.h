////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for German tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef HWTGerman_H
#define HWTGerman_H

#include <include/tabs/HWTGerman_Base.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class HWTGerman : public HWTGerman_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   HWTGerman(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground())
      :HWTGerman_Base(p, w, h, options, back)
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   ~HWTGerman()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();

   ClassDef(HWTGerman,1)
};

#endif   // HWTGerman_H
