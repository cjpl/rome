/********************************************************************
  ArgusTextDialog.h, R.Sawada

  $Log$
  Revision 1.1  2005/01/29 22:45:08  sawada
  Initial revision


********************************************************************/

#ifndef  ArgusTextDialog_H
#define  ArgusTextDialog_H
#include "TGButton.h"
#include "TGLabel.h"
#include "TGTextEntry.h"

class ArgusTextDialog:public TGTransientFrame {
protected:
   TGCompositeFrame*      fF1;           // sub frames
   TGCompositeFrame*      fF2;           // sub frames
   TGButton*              fOkButton;     // ok button
   TGButton*              fCancelButton; // cancel button
   TGLayoutHints*         fL1;           // layout hints
   TGLayoutHints*         fL5;           // layout hints
   TGLayoutHints*         fL6;           // layout hints
   TGLayoutHints*         fL21;          // layout hints
   TGTextEntry*           fText;         // text entry widget
   TGTextBuffer*          fBLabel;       // text buffer
   TGLabel*               fLabel;        // label
   char*                  fRetStr;       //! return string
   
public:
   ArgusTextDialog(const TGWindow * p, const TGWindow * main, UInt_t w, UInt_t h,
                   char *label, char *ret_str, UInt_t options = kVerticalFrame);
   virtual ~ ArgusTextDialog();   
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   
   ClassDef(ArgusTextDialog, 1)
      };
#endif
