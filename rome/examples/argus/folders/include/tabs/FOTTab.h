// Author: Ryu Sawada

#ifndef FOTTab_H
#define FOTTab_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FOTTab                                                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "include/generated/FOTTab_Base.h"
#include "romeproject/include/generated/FORunInfo.h"
#include "romeproject/include/generated/FOPMTData.h"

class FOWindow;

class FOTTab : public FOTTab_Base
{
protected:
   FORunInfo*   fRunInfo;
   TClonesArray* fPMTDatas;

private:
   FOTTab(const FOTTab &c); // not implemented
   FOTTab &operator=(const FOTTab &c); // not implemented

public:
   FOTTab(FOWindow *window):FOTTab_Base(window)
   ,fRunInfo(0)
   ,fPMTDatas(0)
   {
   }

   virtual ~FOTTab()
   {
   }

   void Init();
   void EndInit();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler();


   ClassDef(FOTTab,1)
};

#endif   // FOTTab_H
