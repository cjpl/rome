// $Id$
#ifndef ROMEConfigToForm_H
#define ROMEConfigToForm_H

#include <ROMEStr2DArray.h>
#include <TArrayI.h>
#include <XMLToFormWindow.h>
#include <XMLToForm.h>
class ROMEConfigToFormElements 
{
private:
   ROMEStrArray fType;
   ROMEStrArray fTitle;
   ROMEStrArray fValue;
   ROMEStrArray fPath;
   ROMEStr2DArray fEntry;
   TArrayI fButtonID;
   int fNumberOfElements;
public:
   ROMEConfigToFormElements() { fNumberOfElements = 0; };
   void AddElement(const char* type,const char* title,const char* value,const char* path,int buttonID) {
      AddElement(type,title,value,path,NULL,buttonID);
   };
   void AddElement(const char* type,const char* title,const char* value,const char* path,ROMEStrArray *entries = NULL,int buttonID = 0) 
   { fType.Add(type); fTitle.Add(title); fValue.Add(value); fPath.Add(path); fButtonID.Set(fNumberOfElements+1); fButtonID.AddAt(buttonID,fNumberOfElements); 
     if (entries!=NULL) for (int i=0;i<entries->GetEntriesFast();i++) fEntry.SetAt(entries->At(i).Data(),fNumberOfElements,i); 
     fNumberOfElements++;
   };
   int GetNumberOfElements() { return fNumberOfElements; };
   TString GetTypeAt(int i) { return fType[i]; };
   TString GetTitleAt(int i) { return fTitle[i].Data(); };
   TString GetValueAt(int i) { return fValue[i].Data(); };
   TString GetPathAt(int i) { return fPath[i].Data(); };
   TString GetEntryAt(int i,int j) { return fEntry.At(i,j).Data(); };
   int GetNumberOfEntriesAt(int i) { return fEntry.GetEntriesAt(i); };
   int GetButtonIDAt(int i) { return fButtonID[i]; };
   void Reset() { fType.RemoveAll(); fTitle.RemoveAll(); fValue.RemoveAll(); fPath.RemoveAll(); fEntry.RemoveAll(); fButtonID.Reset(); fNumberOfElements = 0; };
};

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
};

#endif   // ROMEConfigToForm_H

