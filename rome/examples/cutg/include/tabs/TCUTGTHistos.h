#ifndef TCUTGTHistos_H
#define TCUTGTHistos_H

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TCUTGTHistos                                                               //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/TCUTGTHistos_Base.h"

class TCUTGWindow;


class TCUTGTHistos : public TCUTGTHistos_Base
{
protected:

private:
   TCUTGTHistos(const TCUTGTHistos &c); // not implemented
   TCUTGTHistos &operator=(const TCUTGTHistos &c); // not implemented

public:
   TCUTGTHistos(TCUTGWindow* window = 0):TCUTGTHistos_Base(window)
   {
   }

   virtual ~TCUTGTHistos()
   {
   }

   void Init();
   void EndInit();
   void MenuClicked(TGPopupMenu *menu,Long_t param);
   void TabSelected();
   void TabUnSelected();
   void EventHandler();
   void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);
   void Display(bool processEvents=true);


   ClassDef(TCUTGTHistos,0)
};

#endif   // TCUTGTHistos_H
