// Author: Ryu Sawada

#ifndef PMTMixed_H
#define PMTMixed_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// PMTMixed                                                                   //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/PMTMixed_Base.h"
#include <TGButton.h>
#include <TGLayout.h>

class PMWindow;

class PMTMixed : public PMTMixed_Base
{
protected:
   TGHorizontalFrame* fHorz;
   TGTextButton*      fButton1;
   TGTextButton*      fButton2;

private:
   PMTMixed(const PMTMixed &c); // not implemented
   PMTMixed &operator=(const PMTMixed &c); // not implemented

public:
   PMTMixed(PMWindow *window):PMTMixed_Base(window)
   ,fHorz(0)
   ,fButton1(0)
   ,fButton2(0)
   {
   }

   virtual ~PMTMixed()
   {
      delete fHorz;
      delete fButton1;
      delete fButton2;
   }

   void Init();
   void EndInit();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler();
   Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
   Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2);
   void withOutMakingThread();
   void withMakingThread();

   ClassDef(PMTMixed,1)
};

#endif   // PMTMixed_H
