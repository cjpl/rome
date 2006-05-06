// $Id$
#ifndef ROMEConfigToForm_H
#define ROMEConfigToForm_H

#include <ROMEStr2DArray.h>
#include <XMLToFormWindow.h>
#include <XMLToForm.h>
#include "ROMEConfigToFormElements.h"

class ROMEConfigToForm : public XMLToForm{
protected:
public:
   ROMEConfigToForm();
   ~ROMEConfigToForm();

   int Show(const TGWindow * p,const TGWindow * main);

protected:
   bool Init();
   bool XMLToRootClass();
   void XMLToClass(XMLToFormFrame *frame);
   void AddElements(XMLToFormFrame *frame,ROMEConfigToFormElements *elements);
   virtual void AddTabs(XMLToFormFrame *frame) = 0;
   void FillButtonFrame(XMLToFormFrame *frame);

   ClassDef(ROMEConfigToForm, 0) // Translate ROME configuration to form
};

#endif   // ROMEConfigToForm_H

