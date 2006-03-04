////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab2 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef SBTTab2_H
#define SBTTab2_H

#include <include/generated/SBTTab2_Base.h>

class SBTTab2 : public SBTTab2_Base
{
protected:

public:
   SBTTab2():SBTTab2_Base()
   {
   }

   ~SBTTab2()
   {
   }

   void Init();
   void TabSelected();
   void TabUnSelected();
   void EventHandler(){;};


   ClassDef(SBTTab2,1)
};

#endif   // SBTTab2_H
