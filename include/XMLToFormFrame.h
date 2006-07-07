// $Id$
#ifndef XMLToFormFrame_H
#define XMLToFormFrame_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGLayout.h>
#include <TObjArray.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "ROMEString.h"
#include "XMLToFormElement.h"

class XMLToFormFrame;
class TGCompositeFrame;
class TGTab;
class TGHotString;
class TGLabel;
class TGHorizontalFrame;
class TGVerticalFrame;
class TGListTreeItem;
class TGListTree;
class TGCanvas;

class XMLToFormFrame : public TObject
{
public:
   enum PadValues {
      framePad = 3,
      innerFramePad = 0,
      elementPad = 3
   };

   enum FrameTypes {
      kFrame,
      kTab,
      kListTree,
      kListTreeItem
   };

private:
   // Frame Data Fields
   ROMEString fFrameTitle;        //!
   ROMEString fFramePath;         //!
   Bool_t     fFrameVertical;     //!
   Int_t      fFrameType;         //!
   Bool_t     fFrameVisible;      //!
   Int_t      fFrameTabIndex;     //!
   Bool_t     fFrameCreated;      //!
   Int_t      fFrameMaxWidth;     //!
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
   TGCompositeFrame   *fListTreeFrame;    //!
   TGCompositeFrame   *fListTreeHFrame;   //!
   TGCanvas           *fListTreeCanvas;   //!
   TGListTree         *fListTree;         //!
   TGListTreeItem     *fListTreeItem;     //!
   // VFrames
   TGHorizontalFrame  *fHFrame;           //!
   TGHorizontalFrame **fHHFrames;         //!
   Int_t              *fHHFrameIndex;     //!
   TGVerticalFrame   **fVFrames;          //!
   Int_t               fNumberOfHHFrames; //!
   Int_t               fNumberOfVFrames;  //!

public:
   XMLToFormFrame(XMLToFormFrame *parentFrame,const char* title,const char* path,Bool_t vertical,Int_t type,Bool_t visible,Int_t tabIndex);
   virtual ~XMLToFormFrame();

   void AddSubFrame(XMLToFormFrame* subFrame) {
      fSubFrames.AddLast(subFrame);
      fNumberOfSubFrames++;
   }
   void AddElement(XMLToFormElement* element) {
      fElements.AddLast(element);
      fNumberOfElements++;
   }
   // Frame Data Getters
   TString GetFrameTitle() { return fFrameTitle; }
   TString GetFramePath() { return fFramePath; }
   Bool_t  IsFrameVertical() { return fFrameVertical; }
   Bool_t  IsFrameTab() { return fFrameType==kTab; }
   Bool_t  IsFrameListTree() { return fFrameType==kListTree; }
   Bool_t  IsFrameListTreeItem() { return fFrameType==kListTreeItem; }
   Bool_t  IsFrameVisible() { return fFrameVisible; }
   void    SetFrameVisible(Bool_t visible) { fFrameVisible = visible; }
   Int_t   GetFrameTabIndex() { return fFrameTabIndex; }
   void    SetFrameMaxWidth(Int_t maxWidth) { fFrameMaxWidth = maxWidth; }
   Int_t   GetFrameMaxWidth() { return fFrameMaxWidth; }
   void    SetFrameCreated(Bool_t created) { fFrameCreated = created; }
   Bool_t  IsFrameCreated() { return fFrameCreated; }
   // Element Data Getter
   XMLToFormElement* GetElementAt(Int_t i) { return ((XMLToFormElement*)fElements.At(i)); }
   Int_t   GetNumberOfElements() { return fNumberOfElements; }
   // Sub Frame Data Getter
   XMLToFormFrame* GetSubFrameAt(Int_t i) { return ((XMLToFormFrame*)fSubFrames.At(i)); }
   Int_t   GetNumberOfSubFrames() { return fNumberOfSubFrames; }

   ClassDef(XMLToFormFrame, 0)
};
#endif   // XMLToFormFrame_H

