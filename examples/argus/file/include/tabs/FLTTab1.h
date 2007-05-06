////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab1 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef FLTTab1_H
#define FLTTab1_H

#include <include/generated/FLTTab1_Base.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGSplitter.h>

class FLTTab1 : public FLTTab1_Base
{
protected:
   Int_t                fRunNumber;
   TGTextEntry         *fTextRun;
   TGTextEntry         *fTextEvent;
   TGTextEntry         *fTextValue;
   TGLabel             *fLabelRun;
   TGLabel             *fLabelEvent;
   TGLabel             *fLabelValue;
   TGTextButton        *fBUpdate;

   enum CommandIdentifiers{
      B_UPDATE
   };

private:
   FLTTab1(const FLTTab1 &c); // not implemented
   FLTTab1 &operator=(const FLTTab1 &c); // not implemented

public:
   FLTTab1(FLWindow *window):FLTTab1_Base(window)
   ,fRunNumber(0)
   ,fTextRun(0)
   ,fTextEvent(0)
   ,fTextValue(0)
   ,fLabelRun(0)
   ,fLabelEvent(0)
   ,fLabelValue(0)
   ,fBUpdate(0)
   {
   }

   ~FLTTab1()
   {
      delete fTextRun;
      delete fTextEvent;
      delete fTextValue;
      delete fBUpdate;
      delete fLabelRun;
      delete fLabelEvent;
      delete fLabelValue;
   }

   void Init();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler(){;};

   bool ProcessMessage(Long_t msg, Long_t param1, Long_t param2);

private:
   void Update();

   ClassDef(FLTTab1,1)
};

#endif   // FLTTab1_H
