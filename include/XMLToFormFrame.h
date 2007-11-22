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
class ROMELabel;
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
   ROMEString fFrameToolTip;      //!
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
   TGLayoutHints      *fLInnerRadioButtonFrame;  //!
   XMLToFormFrame     *fParentFrame;      //!
   TGCompositeFrame   *fFrame;            //!
   TGTab              *fTab;              //!
   TGHotString        *fTitleString;      //!
   ROMELabel          *fTitleLabel;       //!
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

private:
   XMLToFormFrame(const XMLToFormFrame &frame); // not implemented
   XMLToFormFrame &operator=(const XMLToFormFrame &rhs); // not implemented

public:
   XMLToFormFrame(XMLToFormFrame *parentFrame,const char* title,const char* path,Bool_t vertical,Int_t type,
                  Bool_t visible,Int_t tabIndex,const char* toolTip = "");
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
   TString GetFrameTitle() const { return fFrameTitle; }
   TString GetFramePath() const { return fFramePath; }
   Bool_t  IsFrameVertical() const { return fFrameVertical; }
   Bool_t  IsFrameTab() const { return fFrameType==kTab; }
   Bool_t  IsFrameListTree() const { return fFrameType==kListTree; }
   Bool_t  IsFrameListTreeItem() const { return fFrameType==kListTreeItem; }
   Bool_t  IsFrameVisible() const { return fFrameVisible; }
   void    SetFrameVisible(Bool_t visible) { fFrameVisible = visible; }
   Int_t   GetFrameTabIndex() const { return fFrameTabIndex; }
   void    SetFrameMaxWidth(Int_t maxWidth) { fFrameMaxWidth = maxWidth; }
   Int_t   GetFrameMaxWidth() const { return fFrameMaxWidth; }
   void    SetFrameCreated(Bool_t created) { fFrameCreated = created; }
   Bool_t  IsFrameCreated() const { return fFrameCreated; }
   TString GetFrameToolTip() const { return fFrameToolTip; }
   // Element Data Getter
   XMLToFormElement* GetElementAt(Int_t i) { return static_cast<XMLToFormElement*>(fElements.At(i)); }
   Int_t   GetNumberOfElements() const { return fNumberOfElements; }
   // Sub Frame Data Getter
   XMLToFormFrame* GetSubFrameAt(Int_t i) { return static_cast<XMLToFormFrame*>(fSubFrames.At(i)); }
   Int_t   GetNumberOfSubFrames() const { return fNumberOfSubFrames; }

   ClassDef(XMLToFormFrame, 0)
};
#endif   // XMLToFormFrame_H

