// Author: Ryu Sawada

#ifndef HWTGerman_H
#define HWTGerman_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HWTGerman                                                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/HWTGerman_Base.h"
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
   HWTGerman():HWTGerman_Base()
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   virtual ~HWTGerman()
   {
      delete fVert;
      delete fCanvas;
      delete fText;
   }

   void Init();
   void EndInit();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler();


   ClassDef(HWTGerman,1)
};

#endif   // HWTGerman_H
