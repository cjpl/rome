////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Italian tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef HWTItalian_H
#define HWTItalian_H

#include <include/tabs/HWTItalian_Base.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class HWTItalian : public HWTItalian_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   HWTItalian(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = 0,
         Pixel_t back = GetDefaultFrameBackground());
   virtual ~HWTItalian();
   void Init();

   ClassDef(HWTItalian,1)
};

#endif   // HWTItalian_H
