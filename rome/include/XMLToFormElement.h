// $Id$
#ifndef XMLToFormElement_H
#define XMLToFormElement_H

#include <TNamed.h>
#include <TString.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "XMLToFormElementSignal.h"

class TGHotString;
class TGVerticalFrame;
class ROMELabel;
class TGLayoutHints;
class TGTextEntry;
class TGTextBuffer;
class TGLayoutHints;
class TGTextButton;
class TGComboBox;
class TGCheckButton;
class TGRadioButton;

class XMLToFormElement : public TNamed
{
private:
   // Widget Data Fields
   ROMEString              fType;          //!
   ROMEString              fValue;         //!
   ROMEString              fPath;          //!
   Int_t                   fWidth;         //!
   XMLToFormElementSignal *fSignal;        //!
   Int_t                   fParentFrameIndex; //!
   ROMEString              fToolTip;       //!
   // ComboBox
   ROMEStrArray            fEntry;         //!
   Int_t                   fSelectedEntry; //!
   // Button & CheckButton & RadioButton
   Int_t                   fButtonID;      //!
   Bool_t                  fButtonChecked; //!

public:
   TGHotString            *fWidgetTitle;   //!
   // EditBox
   TGVerticalFrame        *fEditVFrames;   //!
   ROMELabel              *fEditLabel;     //!
   TGLayoutHints          *fLEditLabel;    //!
   TGTextEntry            *fEditBox;       //!
   TGTextBuffer           *fEditBoxBuffer; //!
   TGLayoutHints          *fLEditBox;      //!
   // Button
   TGTextButton           *fButton;  //!
   TGLayoutHints          *fLButton; //!
   // ComboBox
   TGVerticalFrame        *fComboVFrames;//!
   ROMELabel              *fComboLabel;  //!
   TGLayoutHints          *fLComboLabel; //!
   TGComboBox             *fComboBox;    //!
   TGLayoutHints          *fLComboBox;   //!
   // CheckButton
   TGVerticalFrame        *fCheckButtonVFrames;//!
   ROMELabel              *fCheckButtonLabel;  //!
   TGLayoutHints          *fLCheckButtonLabel; //!
   TGCheckButton          *fCheckButton;       //!
   TGLayoutHints          *fLCheckButton;      //!
   // RadioButton
   TGVerticalFrame        *fRadioButtonVFrames;//!
   ROMELabel              *fRadioButtonLabel;  //!
   TGLayoutHints          *fLRadioButtonLabel; //!
   TGRadioButton          *fRadioButton;       //!
   TGLayoutHints          *fLRadioButton;      //!

private:
   XMLToFormElement(const XMLToFormElement &elem); // not implemented
   XMLToFormElement &operator=(const XMLToFormElement &rhs); // not implemented

public:
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,Int_t width,
                    const char* toolTip = "",XMLToFormElementSignal* signal = NULL);
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,Int_t width,
                    Int_t buttonID,const char* toolTip = "",XMLToFormElementSignal* signal = NULL);
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,Int_t width,
                    ROMEStrArray *entries,const char* toolTip = "",XMLToFormElementSignal* signal = NULL);
   virtual ~XMLToFormElement();

   void Init(const char* value, ROMEStrArray *entries);

   // Widget Data Getters
   TString GetTitleString() const { return fTitle; }
   TString GetType() const { return fType; }
   TString GetValue() const { return fValue; }
   void    SetValue(const char* value) { fValue = value; }
   TString GetPath() const { return fPath; }
   Int_t   GetWidth() const { return fWidth; }
   void    SetWidth(Int_t width) { fWidth = width; }
   void    AddEntryAt(const char* entry,Int_t i) { return fEntry.AddAt(entry,i); }
   TString GetEntryAt(Int_t i) const { return fEntry.At(i).Data(); }
   Int_t   GetNumberOfEntries() const { return fEntry.GetEntriesFast(); }
   Int_t   GetSelectedEntry() const { return fSelectedEntry; }
   void    SetSelectedEntry(Int_t index) { fSelectedEntry = index; }
   Int_t   GetButtonID() const { return fButtonID; }
   Bool_t  GetButtonChecked() const { return fButtonChecked; }
   void    SetButtonChecked(Bool_t state) { fButtonChecked = state; }
   Int_t   GetParentFrameIndex() const { return fParentFrameIndex; }
   void    SetParentFrameIndex(Int_t index) { fParentFrameIndex = index; }
   XMLToFormElementSignal *GetSignal() const { return fSignal; }
   TString GetToolTip() const { return fToolTip; }

   ClassDef(XMLToFormElement, 0)
};

#endif   // XMLToFormElement_H
