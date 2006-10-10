// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMEHorizontalFrame
//
//  TGHorizontalFrame costomized for ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEHorizontalFrame.h"

ClassImp(ROMEHorizontalFrame)

void ROMEHorizontalFrame::HideFrame(TGFrame *f)
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
