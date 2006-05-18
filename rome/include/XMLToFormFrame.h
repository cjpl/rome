// $Id: XMLToForm.h 1041 2006-05-04 18:20:33Z sawada $
#ifndef XMLToFormFrame_H
#define XMLToFormFrame_H

#include <TObjArray.h>
#include <TArrayI.h>
#include "XMLToFormElement.h"
#include "ROMEStr2DArray.h"

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
   ROMEString fFrameTitle;  //!
   ROMEString fFramePath;  //!
   bool fFrameVertical;  //!
   bool fFrameTab;  //!
   bool fFrameVisible;  //!
   int fFrameTabIndex;  //!
   // Widget Data Fields
   TObjArray fElements;  //!
   int fNumberOfElements;  //!
   // SubFrames
   TObjArray fSubFrames;  //!
   int fNumberOfSubFrames;  //!
public:
   // GUI Frame Fields
   TGLayoutHints *fLFrame;  //!
   TGLayoutHints *fLInnerFrame;  //!
   TGLayoutHints *fLInnerCheckButtonFrame;  //!
   XMLToFormFrame *fParentFrame;  //!
   TGCompositeFrame *fFrame;  //!
   TGTab *fTab;  //!
   TGHotString *fTitleString;  //!
   TGLabel *fTitleLabel;  //!
   TGLayoutHints *fLTitleLabel;  //!
   int fIndex;  //!
   // VFrames
   TGHorizontalFrame *fHFrame;  //!
   TGHorizontalFrame **fHHFrames;  //!
   TGVerticalFrame **fVFrames;  //!
   int fNumberOfHHFrames;  //!
   int fNumberOfVFrames;  //!
public:
   XMLToFormFrame(const char* title,const char* path,bool vertical,bool tab,bool visible,int tabIndex) { 
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
      fFramePath = NULL;
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
   bool IsFrameVertical() { return fFrameVertical; };
   bool IsFrameTab() { return fFrameTab; };
   bool IsFrameVisible() { return fFrameVisible; };
   int  GetFrameTabIndex() { return fFrameTabIndex; };
   // Element Data Getter
   XMLToFormElement* GetElementAt(int i) { return ((XMLToFormElement*)fElements.At(i)); };
   int GetNumberOfElements() { return fNumberOfElements; };
   // Sub Frame Data Getter
   XMLToFormFrame* GetSubFrameAt(int i) { return ((XMLToFormFrame*)fSubFrames.At(i)); };
   int GetNumberOfSubFrames() { return fNumberOfSubFrames; };

   ClassDef(XMLToFormFrame, 0)
};
#endif   // XMLToFormFrame_H

