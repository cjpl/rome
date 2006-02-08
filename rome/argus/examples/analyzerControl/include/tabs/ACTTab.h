////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ACTTab_H
#define ACTTab_H

#include <include/generated/ACTTab_Base.h>
#include <TNetFolder.h>
#include <ArgusAnalyzerController.h>

class ACTTab : public ACTTab_Base
{
protected:
   ROMENetFolder*           fNetFolder;
   ArgusAnalyzerController* fController;

public:
   ACTTab():ACTTab_Base()
   {
   }

   ~ACTTab()
   {
   }

   void Init();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler(){;};


   ClassDef(ACTTab,1)
};

#endif   // ACTTab_H
