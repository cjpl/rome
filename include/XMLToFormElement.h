// $Id$
#ifndef XMLToFormElement_H
#define XMLToFormElement_H

#include <TString.h>
#include "ROMEString.h"
#include "ROMEStrArray.h"
#include "XMLToFormElementSignal.h"

class TGHotString;
class TGVerticalFrame;
class TGLabel;
class TGLayoutHints;
class TGTextEntry;
class TGTextBuffer;
class TGLayoutHints;
class TGTextButton;
class TGComboBox;
class TGCheckButton;

class XMLToFormElement : public TObject
{
private:
   // Widget Data Fields
   ROMEString              fType;          //!
   ROMEString              fTitle;         //!
   ROMEString              fValue;         //!
   ROMEString              fPath;          //!
   Int_t                   fWidth;         //!
   XMLToFormElementSignal *fSignal;        //!
   Int_t                   fParentFrameIndex; //!
   // ComboBox
   ROMEStrArray            fEntry;         //!
   Int_t                   fSelectedEntry; //!
   // Button & CheckButton
   Int_t                   fButtonID;      //!
   Bool_t                  fButtonChecked; //!

public:
   TGHotString            *fWidgetTitle;      //!
   // EditBox
   TGVerticalFrame        *fEditVFrames;   //!
   TGLabel                *fEditLabel;     //!
   TGLayoutHints          *fLEditLabel;    //!
   TGTextEntry            *fEditBox;       //!
   TGTextBuffer           *fEditBoxBuffer; //!
   TGLayoutHints          *fLEditBox;      //!
   // Button
   TGTextButton           *fButton;  //!
   TGLayoutHints          *fLButton; //!
   // ComboBox
   TGVerticalFrame        *fComboVFrames;//!
   TGLabel                *fComboLabel;  //!
   TGLayoutHints          *fLComboLabel; //!
   TGComboBox             *fComboBox;    //!
   TGLayoutHints          *fLComboBox;   //!
   // CheckButton
   TGVerticalFrame        *fCheckButtonVFrames;//!
   TGLabel                *fCheckButtonLabel;  //!
   TGLayoutHints          *fLCheckButtonLabel; //!
   TGCheckButton          *fCheckButton;       //!
   TGLayoutHints          *fLCheckButton;      //!

public:
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,Int_t width,XMLToFormElementSignal* signal = NULL) {
      Init(type,title,value,path,width,NULL,0,signal);
   }
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,Int_t width,Int_t buttonID,XMLToFormElementSignal* signal = NULL) {
      Init(type,title,value,path,width,NULL,buttonID,signal);
   }
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,Int_t width,ROMEStrArray *entries,XMLToFormElementSignal* signal = NULL) {
      Init(type,title,value,path,width,entries,0,signal);
   }
   void XMLToFormElement::Init(const char* type,const char* title,const char* value,const char* path,Int_t width,ROMEStrArray *entries,Int_t buttonID,XMLToFormElementSignal* signal);
   virtual ~XMLToFormElement();

   // Widget Data Getters
   TString GetType() { return fType; }
   TString GetTitle() { return fTitle; }
   TString GetValue() { return fValue; }
   void    SetValue(const char* value) { fValue = value; }
   TString GetPath() { return fPath; }
   Int_t   GetWidth() { return fWidth; }
   void    SetWidth(Int_t width) { fWidth = width; }
   TString GetEntryAt(Int_t i) { return fEntry.At(i).Data(); }
   Int_t   GetNumberOfEntries() { return fEntry.GetEntriesFast(); }
   Int_t   GetSelectedEntry() { return fSelectedEntry; }
   void    SetSelectedEntry(Int_t index) { fSelectedEntry = index; }
   Int_t   GetButtonID() { return fButtonID; }
   Bool_t  GetButtonChecked() { return fButtonChecked; }
   void    SetButtonChecked(Bool_t state) { fButtonChecked = state; }
   Int_t   GetParentFrameIndex() { return fParentFrameIndex; }
   void    SetParentFrameIndex(Int_t index) { fParentFrameIndex = index; }
   XMLToFormElementSignal *GetSignal() { return fSignal; }

   ClassDef(XMLToFormElement, 0)
};

#endif   // XMLToFormElement_H
