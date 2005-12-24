////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Editable class file for Tab1 tab.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef TTTTab1_H
#define TTTTab1_H

#include <include/generated/TTTTab1_Base.h>
#include <TGButton.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TGLayout.h>
#include <TLatex.h>

class TTTTab1 : public TTTTab1_Base
{
protected:
   TGVerticalFrame     *fVert;
   TGHorizontalFrame   *fHorz;
   TRootEmbeddedCanvas *fCanvas;
   TLatex              *fText;
   TGTextButton        *fBStartVertical;
   TGTextButton        *fBStopVertical;
   TGTextButton        *fBStartHorizontal;
   TGTextButton        *fBStopHorizontal;
   enum CommandIdentifiers {
      B_STARTV,
      B_STOPV,
      B_STARTH,
      B_STOPH
   };

public:
   TTTTab1():TTTTab1_Base()
   {
      fVert   = 0;
      fHorz   = 0;
      fCanvas = 0;
      fText   = 0;
      fBStartVertical = 0;
      fBStopVertical = 0;
      fBStartHorizontal = 0;
      fBStopHorizontal = 0;
   }

   ~TTTTab1()
   {
      delete fBStartVertical; 
      delete fBStopVertical;
      delete fBStartHorizontal;
      delete fBStopHorizontal;
      delete fText;
      delete fCanvas;
      delete fHorz;
      delete fVert;
   }

   void Init();
   bool ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
   void func_1_1();
   void func_1_2();

   ClassDef(TTTTab1,1)
};

#endif   // TTTTab1_H
