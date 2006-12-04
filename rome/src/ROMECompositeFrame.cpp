// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMECompositeFrame
//
//  TGCompositeFrame costomized for ROME
//
//  $Id: ROMECompositeFrame.cpp 1433 2006-10-10 17:11:13Z sawada $
//
//////////////////////////////////////////////////////////////////////////

#include "ROMECompositeFrame.h"

ClassImp(ROMECompositeFrame)

void ROMECompositeFrame::HideFrame(TGFrame *f)
{
   // Hide sub frame.

   if (!fList) return;

   TGFrameElement *el;
   TIter next(fList);

   while ((el = (TGFrameElement *) next()))
      if (el->fFrame == f) {
         el->fState = 0;
         el->fFrame->UnmapWindow();
         break;
      }
}
