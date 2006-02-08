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
#include <TTimer.h>

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
   TTimer              *fTimer1_1;
   TTimer              *fTimer1_2;
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
      fTimer1_1 = 0;
      fTimer1_2 = 0;
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
      if(fTimer1_1)
         delete fTimer1_1;
      if(fTimer1_2)
         delete fTimer1_2;
   }

   void Init();
   bool ProcessMessage(Long_t msg, Long_t param1, Long_t param2);
   void func_1_1();
   void func_1_2();

   Bool_t Startfunc_1_1(Int_t interval = 1000){
      if(!fTimer1_1){
         fTimer1_1 = new TTimer(interval);
         fTimer1_1->Connect("Timeout()","TTTTab1",this,"func_1_1()");
      }
      fTimer1_1->TurnOn();
      return kTRUE;
   }
   Bool_t Stopfunc_1_1(){
      if(fTimer1_1){
         fTimer1_1->TurnOff();
      }
      return kTRUE;
   }
   Bool_t Startfunc_1_2(Int_t interval = 1000){
      if(!fTimer1_2){
         fTimer1_2 = new TTimer(interval);
         fTimer1_1->Connect("Timeout()","TTTTab1",this,"func_1_2()");
      }
      fTimer1_2->TurnOn();
      return kTRUE;
   }
   Bool_t Stopfunc_1_2(){
      if(fTimer1_2){
         fTimer1_2->TurnOff();
      }
      return kTRUE;
   }
   void EventHandler(){;};

   ClassDef(TTTTab1,1)
};

#endif   // TTTTab1_H
