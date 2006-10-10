// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
// ROMELabel
//
//  TGLabel costomized for ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TGToolTip.h>
#include <Riostream.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif
#include "ROMELabel.h"

ClassImp(ROMELabel)

void ROMELabel::SetToolTipText(const char *text, Long_t delayms)
{
   // Set tool tip text associated with this label. The delay is in
   // milliseconds (minimum 250). To remove tool tip call method with
   // text = 0.
   if (fTip) {
      delete fTip;
      fTip = 0;
   }

   if (text && strlen(text))
      fTip = new TGToolTip(fClient->GetDefaultRoot(), this, text, delayms);

   AddInput(kEnterWindowMask | kLeaveWindowMask);
}

Bool_t ROMELabel::HandleCrossing(Event_t *event)
{
   // Handle mouse crossing event.

   if (fTip) {
      if (event->fType == kEnterNotify)
         fTip->Reset();
      else
         fTip->Hide();
   }

   return kTRUE;
}

