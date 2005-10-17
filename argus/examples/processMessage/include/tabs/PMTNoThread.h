////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for NoThread tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef PMTNoThread_H
#define PMTNoThread_H

#include <include/tabs/PMTNoThread_Base.h>
#include <TGButton.h>
#include <TGLayout.h>

class PMTNoThread : public PMTNoThread_Base
{
protected:
   TGHorizontalFrame* fHorz;
   TGTextButton*      fButton;
public:
   PMTNoThread():PMTNoThread_Base()
   {
      fHorz       = 0;
      fButton     = 0;
   }

   ~PMTNoThread()
   {
      delete fHorz;
      delete fButton;
   }

   void Init();
   void MenuClicked(Long_t param);
   Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
   void withOutMakingThread();

   ClassDef(PMTNoThread,1)
};

#endif   // PMTNoThread_H
