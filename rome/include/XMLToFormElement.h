// $Id: XMLToFormElement.h 1041 2006-05-04 18:20:33Z sawada $
#ifndef XMLToFormElement_H
#define XMLToFormElement_H

#include <TObjArray.h>
#include <TArrayI.h>
#include "ROMEStr2DArray.h"
#include "XMLToFormElementSignal.h"

class XMLToFormElement : public TObject
{
private:
   // Widget Data Fields
   ROMEString fType;  //!
   ROMEString fTitle;  //!
   ROMEString fValue;  //!
   ROMEString fPath;  //!
   int        fWidth;  //!
   XMLToFormElementSignal *fSignal;  //!
   // ComboBox
   ROMEStrArray fEntry;  //!
   int fSelectedEntry;  //!
   // Button & CheckButton
   int fButtonID;  //!
   bool fButtonChecked;  //!
public:
   TGHotString *fWidgetTitle;  //!
   // EditBox
   TGVerticalFrame *fEditVFrames;  //!
   TGLabel *fEditLabel;  //!
   TGLayoutHints *fLEditLabel;  //!
   TGTextEntry *fEditBox;  //!
   TGTextBuffer *fEditBoxBuffer;  //!
   TGLayoutHints *fLEditBox;  //!
   // Button
   TGTextButton *fButton;  //!
   TGLayoutHints *fLButton;  //!
   // ComboBox
   TGVerticalFrame *fComboVFrames;  //!
   TGLabel *fComboLabel;  //!
   TGLayoutHints *fLComboLabel;  //!
   TGComboBox *fComboBox;  //!
   TGLayoutHints *fLComboBox;  //!
   // CheckButton
   TGVerticalFrame *fCheckButtonVFrames;  //!
   TGLabel *fCheckButtonLabel;  //!
   TGLayoutHints *fLCheckButtonLabel;  //!
   TGCheckButton *fCheckButton;  //!
   TGLayoutHints *fLCheckButton;  //!
public:
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,int width,XMLToFormElementSignal* signal = NULL) {
      Init(type,title,value,path,width,NULL,0,signal);
   };
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,int width,int buttonID,XMLToFormElementSignal* signal = NULL) {
      Init(type,title,value,path,width,NULL,buttonID,signal);
   };
   XMLToFormElement(const char* type,const char* title,const char* value,const char* path,int width,ROMEStrArray *entries,XMLToFormElementSignal* signal = NULL) {
      Init(type,title,value,path,width,entries,0,signal);
   };
   void Init(const char* type,const char* title,const char* value,const char* path,int width,ROMEStrArray *entries,int buttonID,XMLToFormElementSignal* signal) { 
      fType = type; fTitle = title; fValue = value; fPath = path; fSignal = signal; fButtonID = buttonID; fWidth = width;
      if (entries!=NULL) {
         for (int i=0;i<entries->GetEntriesFast();i++) {
            if (entries->At(i)==value)
               fSelectedEntry = i;
            fEntry.AddLast(entries->At(i).Data()); 
         }
      }
      if (!strcmp(value,"true"))
         fButtonChecked = true;
      else
         fButtonChecked = false;
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
   };
   // Widget Data Getters
   TString GetType() { return fType; };
   TString GetTitle() { return fTitle; };
   TString GetValue() { return fValue; };
   void SetValue(const char* value) { fValue = value; };
   TString GetPath() { return fPath; };
   int GetWidth() { return fWidth; };
   void SetWidth(int width) { fWidth = width; };
   XMLToFormElementSignal *GetSignal() { return fSignal; };
   TString GetEntryAt(int i) { return fEntry.At(i).Data(); };
   int GetNumberOfEntries() { return fEntry.GetEntriesFast(); };
   int GetSelectedEntry() { return fSelectedEntry; };
   void SetSelectedEntry(int index) { fSelectedEntry = index; };
   int GetButtonID() { return fButtonID; };
   bool GetButtonChecked() { return fButtonChecked; };
   void SetButtonChecked(bool state) { fButtonChecked = state; };

   ClassDef(XMLToFormElement, 0)
};

#endif   // XMLToFormElement_H

