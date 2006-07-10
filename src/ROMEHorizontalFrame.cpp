// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMEHorizontalFrame
//
//  TGHorizontalFrame costomized for ROME
//
//  $Id: ROMEHorizontalFrame.cpp 1235 2006-07-07 23:26:48Z sawada $
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
