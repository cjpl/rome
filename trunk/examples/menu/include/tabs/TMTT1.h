////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for T1 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef TMTT1_H
#define TMTT1_H

#include <include/tabs/TMTT1_Base.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class TMTT1 : public TMTT1_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   TMTT1(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground())
      :TMTT1_Base(p, w, h, options, back)
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   ~TMTT1()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();
   void MenuClicked(Long_t param);

   ClassDef(TMTT1,1)
};

#endif   // TMTT1_H
