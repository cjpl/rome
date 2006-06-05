// $Id$
// Author: Ryu Sawada
#ifndef  ArgusTextDialog_H
#define  ArgusTextDialog_H


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusTextDialog                                                      //
//                                                                      //
// Dialogo bject which asks string                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

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
   Char_t*                fRetStr;       //! return string
   
public:
   ArgusTextDialog(const TGWindow * p, const TGWindow * main, UInt_t w, UInt_t h,
                   Char_t *label, Char_t *ret_str, UInt_t options = kVerticalFrame);
   virtual ~ ArgusTextDialog();   
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   
   ClassDef(ArgusTextDialog, 1) // Panel to input text
};
#endif
