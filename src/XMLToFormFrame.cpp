// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  XMLToFormFrame
//
//  Frame class for XML GUI configuration
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGFrame.h>
#include <TGTab.h>
#include <TGCanvas.h>
#include <TGListTree.h>
#include <ROMELabel.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "XMLToFormFrame.h"
#include "ROME.h"

ClassImp(XMLToFormFrame)

//______________________________________________________________________________
XMLToFormFrame::XMLToFormFrame(XMLToFormFrame *parentFrame, const char* title, const char* path,
                               Bool_t vertical,Int_t type,Bool_t visible,Int_t tabIndex,const char* toolTip)
:TObject()
,fFrameTitle(title)
,fFramePath(path)
,fFrameVertical(vertical)
,fFrameType(type)
,fFrameVisible(visible)
,fFrameTabIndex(tabIndex)
,fFrameCreated(kFALSE)
,fFrameMaxWidth(1000)
,fFrameToolTip(toolTip)
,fElements()
,fNumberOfElements(0)
,fSubFrames()
,fNumberOfSubFrames(0)
,fLFrame(0)
,fLFrameExpand(0)
,fLInnerFrame(0)
,fLInnerCheckButtonFrame(0)
,fLInnerRadioButtonFrame(0)
,fParentFrame(parentFrame)
,fFrame(0)
,fTab(0)
,fTitleString(0)
,fTitleLabel(0)
,fLTitleLabel(0)
,fIndex(0)
,fListTreeFrame(0)
,fListTreeHFrame(0)
,fListTreeCanvas(0)
,fListTree(0)
,fListTreeItem(0)
,fHFrame(0)
,fHHFrames(0)
,fHHFrameIndex(0)
,fVFrames(0)
,fNumberOfHHFrames(0)
,fNumberOfVFrames(0)
{
   fElements.RemoveAll();
   fSubFrames.RemoveAll();
   if (strlen(title)>0 && visible) {
      fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad,
                                       elementPad, elementPad);
   } else {
      fLTitleLabel = NULL;
   }
}

//______________________________________________________________________________
XMLToFormFrame::~XMLToFormFrame()
{
#if 0 // deleting GUI objects may cause error
   SafeDelete(fLFrame);
   SafeDelete(fLFrameExpand);
   SafeDelete(fLInnerFrame);
   SafeDelete(fLInnerCheckButtonFrame);
   SafeDelete(fFrame);
   SafeDelete(fTab);
   SafeDelete(fListTreeFrame);
   SafeDelete(fListTreeHFrame);
   SafeDelete(fListTreeCanvas);
   SafeDelete(fListTree);
   SafeDelete(fListTreeItem);
   SafeDelete(fTitleLabel);
   SafeDelete(fLTitleLabel);
   SafeDelete(fHFrame);
   SafeDeleteArray(fHHFrames);
   SafeDelete(fHHFrameIndex);
   SafeDeleteArray(fVFrames);
#endif
}
