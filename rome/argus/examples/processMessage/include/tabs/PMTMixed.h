////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Mixed tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef PMTMixed_H
#define PMTMixed_H

#include <include/generated/PMTMixed_Base.h>
#include <TGButton.h>
#include <TGLayout.h>

class PMTMixed : public PMTMixed_Base
{
protected:
   TGHorizontalFrame* fHorz;
   TGTextButton*      fButton1;
   TGTextButton*      fButton2;
public:
   PMTMixed():PMTMixed_Base()
   {
      fHorz       = 0;
      fButton1    = 0;
      fButton2    = 0;
   }

   ~PMTMixed()
   {
      delete fHorz;
      delete fButton1;
      delete fButton2;
   }

   void Init();
   void MenuClicked(Long_t param);
   Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
   Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2);
   void withOutMakingThread();
   void withMakingThread();

   ClassDef(PMTMixed,1)
};

#endif   // PMTMixed_H
