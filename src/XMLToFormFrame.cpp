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
#include <TGLabel.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "XMLToFormFrame.h"
#include "ROME.h"

ClassImp(XMLToFormFrame)

XMLToFormFrame::XMLToFormFrame(XMLToFormFrame *parentFrame,const char* title,const char* path,Bool_t vertical,Int_t type,Bool_t visible,Int_t tabIndex)
{
   fFrameCreated = false;
   fFrameTitle = title;
   fFramePath = path;
   fFrameVertical = vertical;
   fFrameType = type;
   fFrameVisible = visible;
   fFrameTabIndex = tabIndex;
   fElements.RemoveAll();
   fNumberOfElements = 0;
   fSubFrames.RemoveAll();
   fNumberOfSubFrames = 0;
   fLFrame = NULL;
   fLInnerFrame = NULL;
   fLInnerCheckButtonFrame = NULL;
   fParentFrame = parentFrame;
   fFrame = NULL;
   fTab = NULL;
   fListTreeFrame = NULL;
   fListTreeHFrame = NULL;
   fListTreeCanvas = NULL;
   fListTree = NULL;
   fListTreeItem = NULL;
   if (strlen(title)>0 && visible) {
      fLTitleLabel = new TGLayoutHints(kLHintsTop | kLHintsLeft, elementPad, elementPad, elementPad, elementPad);
   }
   else {
      fLTitleLabel = NULL;
   }
   fTitleLabel = NULL;
   fHFrame = NULL;
   fHHFrames = NULL;
   fHHFrameIndex = NULL;
   fVFrames = NULL;
   fNumberOfHHFrames = 0;
   fNumberOfVFrames = 0;
}

XMLToFormFrame::~XMLToFormFrame()
{
#if 0 // deleting GUI objects may cause error
   SafeDelete(fLFrame);
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
