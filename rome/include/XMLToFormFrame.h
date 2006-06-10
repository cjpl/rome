// $Id: XMLToForm.h 1041 2006-05-04 18:20:33Z sawada $
#ifndef XMLToFormFrame_H
#define XMLToFormFrame_H

#include <TGLayout.h>

#include <TObjArray.h>
#include "ROMEString.h"
#include "XMLToFormElement.h"

class TGLayoutHints;
class XMLToFormFrame;
class TGCompositeFrame;
class TGTab;
class TGHotString;
class TGLabel;
class TGHorizontalFrame;
class TGVerticalFrame;

class XMLToFormFrame : public TObject
{
public:
   enum PadValues {
      framePad = 3,
      innerFramePad = 0,
      elementPad = 3
   };

private:
   // Frame Data Fields
   ROMEString fFrameTitle;        //!
   ROMEString fFramePath;         //!
   Bool_t     fFrameVertical;     //!
   Bool_t     fFrameTab;          //!
   Bool_t     fFrameVisible;      //!
   Int_t      fFrameTabIndex;     //!
   // Widget Data Fields
   TObjArray  fElements;          //!
   Int_t      fNumberOfElements;  //!
   // SubFrames
   TObjArray  fSubFrames;         //!
   Int_t      fNumberOfSubFrames; //!

public:
   // GUI Frame Fields
   TGLayoutHints      *fLFrame;           //!
   TGLayoutHints      *fLInnerFrame;      //!
   TGLayoutHints      *fLInnerCheckButtonFrame;  //!
   XMLToFormFrame     *fParentFrame;      //!
   TGCompositeFrame   *fFrame;            //!
   TGTab              *fTab;              //!
   TGHotString        *fTitleString;      //!
   TGLabel            *fTitleLabel;       //!
   TGLayoutHints      *fLTitleLabel;      //!
   Int_t               fIndex;            //!
   // VFrames
   TGHorizontalFrame  *fHFrame;           //!
   TGHorizontalFrame **fHHFrames;         //!
   TGVerticalFrame   **fVFrames;          //!
   Int_t               fNumberOfHHFrames; //!
   Int_t               fNumberOfVFrames;  //!

public:
   XMLToFormFrame(const char* title,const char* path,Bool_t vertical,Bool_t tab,Bool_t visible,Int_t tabIndex) { 
      fFrameTitle = title;
      fFramePath = path;
      fFrameVertical = vertical;
      fFrameTab = tab;
      fFrameVisible = visible;
      fFrameTabIndex = tabIndex;
      fElements.RemoveAll();
      fNumberOfElements = 0;
      fSubFrames.RemoveAll();
      fNumberOfSubFrames = 0;
      fLFrame = NULL;
      fLInnerFrame = NULL;
      fLInnerCheckButtonFrame = NULL;
      fParentFrame = NULL;
      fFrame = NULL;
      fTab = NULL;
      if (strlen(title)>0 && visible) {
         fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
      }
      else {
         fTitleLabel = NULL;
         fLTitleLabel = NULL;
      }
      fHFrame = NULL;
      fHHFrames = NULL;
      fVFrames = NULL;
      fNumberOfHHFrames = 0;
      fNumberOfVFrames = 0;
   };
   void AddSubFrame(XMLToFormFrame* subFrame) {
      fSubFrames.AddLast(subFrame);
      fNumberOfSubFrames++;
   };
   void AddElement(XMLToFormElement* element) {
      fElements.AddLast(element);
      fNumberOfElements++;
   };
   // Frame Data Getters
   TString GetFrameTitle() { return fFrameTitle; };
   TString GetFramePath() { return fFramePath; };
   Bool_t  IsFrameVertical() { return fFrameVertical; };
   Bool_t  IsFrameTab() { return fFrameTab; };
   Bool_t  IsFrameVisible() { return fFrameVisible; };
   void    SetFrameVisible(Bool_t visible) { fFrameVisible = visible; };
   Int_t   GetFrameTabIndex() { return fFrameTabIndex; };
   // Element Data Getter
   XMLToFormElement* GetElementAt(Int_t i) { return ((XMLToFormElement*)fElements.At(i)); };
   Int_t   GetNumberOfElements() { return fNumberOfElements; };
   // Sub Frame Data Getter
   XMLToFormFrame* GetSubFrameAt(Int_t i) { return ((XMLToFormFrame*)fSubFrames.At(i)); };
   Int_t   GetNumberOfSubFrames() { return fNumberOfSubFrames; };

   ClassDef(XMLToFormFrame, 0)
};
#endif   // XMLToFormFrame_H

