////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Italian tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef HWTItalian_H
#define HWTItalian_H

#include <include/generated/HWTItalian_Base.h>
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
   HWTItalian():HWTItalian_Base()
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   ~HWTItalian()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();

   ClassDef(HWTItalian,1)
};

#endif   // HWTItalian_H
