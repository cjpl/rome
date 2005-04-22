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
   TMTT1():TMTT1_Base()
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
   void MenuClicked(TGPopupMenu *menu,Long_t param);

   ClassDef(TMTT1,1)
};

#endif   // TMTT1_H
