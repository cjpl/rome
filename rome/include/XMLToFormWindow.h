// $Id$
#ifndef XMLToFormWindow_H
#define XMLToFormWindow_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGButton.h>
#include <TGTab.h>
#include <TGLayout.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGComboBox.h>
#include <Riostream.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "XMLToForm.h"

class XMLToFormWindow : public TGTransientFrame {
public:
   enum WidgetTypes {
      EDIT_BOX,
      COMBO_BOX,
      CHECK_BUTTON,
      BUTTON
   };
protected:
   int *fExitID;
   bool fFirstEdit;
   int fMaximalWindowWidth;
   XMLToFormFrame *fMainFrame;

public:
   XMLToFormWindow() {};
   XMLToFormWindow(const TGWindow * p,const TGWindow * main, XMLToFormFrame *frame, int *exitButtonID, int windowWidth=200);
   virtual ~ XMLToFormWindow();
   void SignalHandler();
   void CheckSignals(XMLToFormFrame *frame);
   bool SearchWidget(const char* path,XMLToFormFrame** frame,int *index);

protected:
   XMLToFormFrame* SearchFrame(XMLToFormFrame *frame,const char* title,const char* editBoxLabel);

   void PlaceWindow(const TGWindow * main);
   bool CreateForm(XMLToFormFrame *frame);
   void BuildForm(XMLToFormFrame *frame);
   void CreateFrame(XMLToFormFrame *frame);
   void BuildFrame(XMLToFormFrame *frame);
   void SaveCurrentValues(XMLToFormFrame *frame);

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(XMLToFormWindow, 0) // Window for XMLToForm
};

#endif   // XMLToFormWindow_H

