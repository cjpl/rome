////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for T2 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef TMTT2_H
#define TMTT2_H

#include <include/tabs/TMTT2_Base.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class TMTT2 : public TMTT2_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   TMTT2(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground())
      :TMTT2_Base(p, w, h, options, back)
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   ~TMTT2()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();

   ClassDef(TMTT2,1)
};

#endif   // TMTT2_H
