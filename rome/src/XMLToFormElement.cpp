// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  XMLToFormElement
//
//  Elements in XML GUI configuration frame.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGString.h>
#include <TGFrame.h>
#include <ROMELabel.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGComboBox.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "XMLToFormElement.h"

ClassImp(XMLToFormElement)

void XMLToFormElement::Init(const char* type,const char* title,const char* value,const char* path,Int_t width,ROMEStrArray *entries,Int_t buttonID,const char* toolTip,XMLToFormElementSignal* signal)
{
   fType = type; fTitle = title; fValue = value; fPath = path; fSignal = signal; fButtonID = buttonID; fWidth = width; fToolTip = toolTip;
   if (entries!=NULL) {
      for (Int_t i=0;i<entries->GetEntriesFast();i++) {
         if (entries->At(i)==value)
            fSelectedEntry = i;
         fEntry.AddLast(entries->At(i).Data());
      }
   }
   if (!strcmp(value,"true"))
      fButtonChecked = true;
   else
      fButtonChecked = false;
   fParentFrameIndex = 0;

   fWidgetTitle = NULL;
   // EditBox
   fEditVFrames = NULL;
   fEditLabel = NULL;
   fLEditLabel = NULL;
   fEditBox = NULL;
   fEditBoxBuffer = NULL;
   fLEditBox = NULL;
   // Button
   fButton = NULL;
   fLButton = NULL;
   // ComboBox
   fComboVFrames = NULL;
   fComboLabel = NULL;
   fLComboLabel = NULL;
   fComboBox = NULL;
   fLComboBox = NULL;
   // CheckButton
   fCheckButtonVFrames = NULL;
   fCheckButtonLabel = NULL;
   fLCheckButtonLabel = NULL;
   fCheckButton = NULL;
   fLCheckButton = NULL;
}

XMLToFormElement::~XMLToFormElement()
{
#if 0 // deleting GUI objects may cause error
   SafeDelete(fWidgetTitle);
   SafeDelete(fEditVFrames);
   SafeDelete(fEditLabel);
   SafeDelete(fLEditLabel);
   SafeDelete(fEditBox);
   SafeDelete(fEditBoxBuffer);
   SafeDelete(fLEditBox);
   // Button
   SafeDelete(fButton);
   SafeDelete(fLButton);
   // ComboBox
   SafeDelete(fComboVFrames);
   SafeDelete(fComboLabel);
   SafeDelete(fLComboLabel);
   SafeDelete(fComboBox);
   SafeDelete(fLComboBox);
   // CheckButton
   SafeDelete(fCheckButtonVFrames);
   SafeDelete(fCheckButtonLabel);
   SafeDelete(fLCheckButtonLabel);
   SafeDelete(fCheckButton);
   SafeDelete(fLCheckButton);
#endif
}
