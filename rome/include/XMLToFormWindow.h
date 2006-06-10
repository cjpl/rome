// $Id$
#ifndef XMLToFormWindow_H
#define XMLToFormWindow_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

class XMLToFormFrame;

class XMLToFormWindow : public TGTransientFrame {
public:
   enum WidgetTypes {
      EDIT_BOX,
      COMBO_BOX,
      CHECK_BUTTON,
      BUTTON
   };
protected:
   Int_t          *fExitID;
   Bool_t          fFirstEdit;
   Int_t           fMaximalWindowWidth;
   XMLToFormFrame *fMainFrame;

public:
   XMLToFormWindow() {};
   XMLToFormWindow(const TGWindow * p,const TGWindow * main, XMLToFormFrame *frame, Int_t *exitButtonID, Int_t windowWidth=200);
   virtual ~ XMLToFormWindow();
   void   SignalHandler();
   Bool_t SearchWidget(const char* path,XMLToFormFrame** frame,Int_t *index,XMLToFormFrame* mainFrame);

protected:
   XMLToFormFrame* SearchFrame(XMLToFormFrame *frame,const char* title,const char* editBoxLabel);

   void   CheckSignals(XMLToFormFrame *frame);
   void   PlaceWindow(const TGWindow * main);
   Bool_t CreateForm(XMLToFormFrame *frame);
   void   BuildForm(XMLToFormFrame *frame);
   void   CreateFrame(XMLToFormFrame *frame);
   void   BuildFrame(XMLToFormFrame *frame);
   void   SaveCurrentValues(XMLToFormFrame *frame);

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(XMLToFormWindow, 0) // Window for XMLToForm
};

#endif   // XMLToFormWindow_H

