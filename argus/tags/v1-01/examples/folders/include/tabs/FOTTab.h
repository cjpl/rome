////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FOTTab_H
#define FOTTab_H

#include <include/tabs/FOTTab_Base.h>

class FOTTab : public FOTTab_Base
{
protected:

public:
   FOTTab():FOTTab_Base()
   {
   }

   ~FOTTab()
   {
   }

   void Init();


   ClassDef(FOTTab,1)
};

#endif   // FOTTab_H
