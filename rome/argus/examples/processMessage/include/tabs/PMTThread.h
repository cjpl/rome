////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Thread tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef PMTThread_H
#define PMTThread_H

#include <include/generated/PMTThread_Base.h>
#include <TGButton.h>
#include <TGLayout.h>

class PMTThread : public PMTThread_Base
{
protected:
   TGHorizontalFrame* fHorz;
   TGTextButton*      fButton;
public:
   PMTThread():PMTThread_Base()
   {
      fHorz       = 0;
      fButton     = 0;
   }

   ~PMTThread()
   {
      delete fHorz;
      delete fButton;
   }

   void Init();
   void MenuClicked(Long_t param);
   Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2);
   void withMakingThread();
   void EventHandler(){;};

   ClassDef(PMTThread,1)
};

#endif   // PMTThread_H
