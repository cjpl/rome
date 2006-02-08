////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOTTab_H
#define FOTTab_H

#include <include/generated/FOTTab_Base.h>
#include <TClonesArray.h>
#include "romeproject/include/generated/FORunInfo.h"
#include "romeproject/include/generated/FOPMTData.h"

class FOTTab : public FOTTab_Base
{
protected:
   FORunInfo*   fRunInfo;
   TClonesArray* fPMTDatas;
public:
   FOTTab():FOTTab_Base()
   {
   }

   ~FOTTab()
   {
   }

   void Init();
   void EventHandler(){;};


   ClassDef(FOTTab,1)
};

#endif   // FOTTab_H
