// Author: Ryu Sawada

#ifndef HGTTab_H
#define HGTTab_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// HGTTab                                                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/HGTTab_Base.h"
#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLayout.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGFrame.h"

class HGTTab : public HGTTab_Base
{
protected:
   TGVerticalFrame     *fVert;
   TRootEmbeddedCanvas *fCanvas;
   TGTextButton        *fBUpdate;
   TH1                 *fHisto;
   enum CommandIdentifiers {
      B_UPDATE,
   };

public:
   HGTTab():HGTTab_Base()
   {
      fVert    = 0;
      fCanvas  = 0;
      fBUpdate = 0;
      fHisto   = 0;
   }

   virtual ~HGTTab()
   {
      delete fVert;
      delete fCanvas;
      delete fBUpdate;
      delete fHisto;
   }

   void Init();
   void EndInit();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler();
   bool ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
   void Update();

   ClassDef(HGTTab,1)
};

#endif   // HGTTab_H