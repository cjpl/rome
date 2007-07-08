// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMECompositeFrame
//
//  TGCompositeFrame costomized for ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMECompositeFrame.h"

ClassImp(ROMECompositeFrame)

//______________________________________________________________________________
void ROMECompositeFrame::HideFrame(TGFrame *f)
{
   // Hide sub frame.

   if (!fList) return;

   TGFrameElement *el;
   TIter next(fList);

   while ((el = static_cast<TGFrameElement*>(next())))
      if (el->fFrame == f) {
         el->fState = 0;
         el->fFrame->UnmapWindow();
         break;
      }
}
