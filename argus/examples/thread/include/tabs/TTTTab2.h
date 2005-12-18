////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab2 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef TTTTab2_H
#define TTTTab2_H

#include <include/generated/TTTTab2_Base.h>
#include <TGButton.h>
#include <TGLayout.h>

class TTTTab2 : public TTTTab2_Base
{
protected:
   TGHorizontalFrame* fHorz;
   TGTextButton*      fBStart_2_1;
   TGTextButton*      fBStop_2_1;
   TGTextButton*      fBStart_2_2;
   TGTextButton*      fBStop_2_2;
   enum CommandIdentifiers {
      B_START_2_1,
      B_STOP_2_1,
      B_START_2_2,
      B_STOP_2_2
   };

public:
   TTTTab2():TTTTab2_Base()
   {
      fHorz       = 0;
      fBStart_2_1 = 0;
      fBStop_2_1  = 0;
      fBStart_2_2 = 0;
      fBStop_2_2  = 0;
   }

   ~TTTTab2()
   {
      delete fHorz;
      delete fBStart_2_1;
      delete fBStop_2_1;
      delete fBStart_2_2;
      delete fBStop_2_2;
   }

   void Init();
   bool ProcessMessage(Long_t msg, Long_t param1, Long_t param2);

   ClassDef(TTTTab2,1)
};

#endif   // TTTTab2_H
