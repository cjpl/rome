////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab1 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef SBTTab1_H
#define SBTTab1_H

#include <include/tabs/SBTTab1_Base.h>
#include <TG3DLine.h>

class SBTTab1 : public SBTTab1_Base
{
protected:
   TGHorizontal3DLine *fLine;
public:
   SBTTab1():SBTTab1_Base()
   {
   }

   ~SBTTab1()
   {
   }

   void Init();
   void TabSelected();
   void TabUnSelected();


   ClassDef(SBTTab1,1)
};

#endif   // SBTTab1_H
