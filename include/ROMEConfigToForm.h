// $Id$
#ifndef ROMEConfigToForm_H
#define ROMEConfigToForm_H

#include "XMLToForm.h"

class ROMEConfigToForm : public XMLToForm{
protected:
public:
   ROMEConfigToForm();
   ~ROMEConfigToForm();
   Int_t  Show(const TGWindow * p,const TGWindow * main);

protected:
   Bool_t Init();
   Bool_t XMLToRootClass();
   void   XMLToClass(XMLToFormFrame *frame);
   void   FillButtonFrame(XMLToFormFrame *frame);
   void   FillRunModeFrame(XMLToFormFrame *frame);

   virtual void AddTabs(XMLToFormFrame *frame) = 0;

   ClassDef(ROMEConfigToForm, 0) // Translate ROME configuration to form
};

#endif   // ROMEConfigToForm_H

