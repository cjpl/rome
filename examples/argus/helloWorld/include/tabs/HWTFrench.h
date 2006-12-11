// Author: Ryu Sawada

#ifndef HWTFrench_H
#define HWTFrench_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HWTFrench                                                                  //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/HWTFrench_Base.h"
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class HWFrench;

class HWTFrench : public HWTFrench_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;

public:
   HWTFrench(HWWindow *window):HWTFrench_Base(window)
   {
      fVert   = 0;
      fCanvas = 0;
      fText   = 0;
   }

   virtual ~HWTFrench()
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


   ClassDef(HWTFrench,1)
};

#endif   // HWTFrench_H
