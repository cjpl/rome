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

//______________________________________________________________________________
XMLToFormElement::XMLToFormElement(const char* type, const char* title, const char* value,
                                   const char* path, Int_t width, const char* toolTip,
                                   XMLToFormElementSignal* signal)
:TNamed("", title)
,fType(type)
,fValue(value)
,fPath(path)
,fWidth(width)
,fSignal(signal)
,fParentFrameIndex(0)
,fToolTip(toolTip)
,fEntry()
,fSelectedEntry(0)
,fButtonID(0)
,fButtonChecked(kFALSE)
,fWidgetTitle(0)
,fEditVFrames(0)
,fEditLabel(0)
,fLEditLabel(0)
,fEditBox(0)
,fEditBoxBuffer(0)
,fLEditBox(0)
,fButton(0)
,fLButton(0)
,fComboVFrames(0)
,fComboLabel(0)
,fLComboLabel(0)
,fComboBox(0)
,fLComboBox(0)
,fCheckButtonVFrames(0)
,fCheckButtonLabel(0)
,fLCheckButtonLabel(0)
,fCheckButton(0)
,fLCheckButton(0)
,fRadioButtonVFrames(0)
,fRadioButtonLabel(0)
,fLRadioButtonLabel(0)
,fRadioButton(0)
,fLRadioButton(0)
{
   Init(value, 0);
}

//______________________________________________________________________________
XMLToFormElement::XMLToFormElement(const char* type, const char* title, const char* value,
                                   const char* path, Int_t width, Int_t buttonID, const char* toolTip,
                                   XMLToFormElementSignal* signal)
:TNamed("", title)
,fType(type)
,fValue(value)
,fPath(path)
,fWidth(width)
,fSignal(signal)
,fParentFrameIndex(0)
,fToolTip(toolTip)
,fEntry()
,fSelectedEntry(0)
,fButtonID(buttonID)
,fButtonChecked(kFALSE)
,fWidgetTitle(0)
,fEditVFrames(0)
,fEditLabel(0)
,fLEditLabel(0)
,fEditBox(0)
,fEditBoxBuffer(0)
,fLEditBox(0)
,fButton(0)
,fLButton(0)
,fComboVFrames(0)
,fComboLabel(0)
,fLComboLabel(0)
,fComboBox(0)
,fLComboBox(0)
,fCheckButtonVFrames(0)
,fCheckButtonLabel(0)
,fLCheckButtonLabel(0)
,fCheckButton(0)
,fLCheckButton(0)
,fRadioButtonVFrames(0)
,fRadioButtonLabel(0)
,fLRadioButtonLabel(0)
,fRadioButton(0)
,fLRadioButton(0)
{
   Init(value, 0);
}

//______________________________________________________________________________
XMLToFormElement::XMLToFormElement(const char* type, const char* title, const char* value,
                                   const char* path, Int_t width, ROMEStrArray *entries, const char* toolTip,
                                   XMLToFormElementSignal* signal)
:TNamed("", title)
,fType(type)
,fValue(value)
,fPath(path)
,fWidth(width)
,fSignal(signal)
,fParentFrameIndex(0)
,fToolTip(toolTip)
,fEntry()
,fSelectedEntry(0)
,fButtonID(0)
,fButtonChecked(kFALSE)
,fWidgetTitle(0)
,fEditVFrames(0)
,fEditLabel(0)
,fLEditLabel(0)
,fEditBox(0)
,fEditBoxBuffer(0)
,fLEditBox(0)
,fButton(0)
,fLButton(0)
,fComboVFrames(0)
,fComboLabel(0)
,fLComboLabel(0)
,fComboBox(0)
,fLComboBox(0)
,fCheckButtonVFrames(0)
,fCheckButtonLabel(0)
,fLCheckButtonLabel(0)
,fCheckButton(0)
,fLCheckButton(0)
,fRadioButtonVFrames(0)
,fRadioButtonLabel(0)
,fLRadioButtonLabel(0)
,fRadioButton(0)
,fLRadioButton(0)
{
   Init(value, entries);
}

//______________________________________________________________________________
void XMLToFormElement::Init(const char* value, ROMEStrArray *entries)
{
   if (entries!=NULL) {
      const Int_t n = entries->GetEntriesFast();
      TString str;
      for (Int_t i=0;i<n;i++) {
         str = entries->At(i);
         if (str==value) {
            fSelectedEntry = i;
         }
         fEntry.AddLast(str.Data());
      }
   }
   if (!strcmp(value,"true"))
      fButtonChecked = true;
   else
      fButtonChecked = false;
}

//______________________________________________________________________________
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
   // RadioButton
   SafeDelete(fRadioButtonVFrames);
   SafeDelete(fRadioButtonLabel);
   SafeDelete(fLRadioButtonLabel);
   SafeDelete(fRadioButton);
   SafeDelete(fLRadioButton);
#endif
}
