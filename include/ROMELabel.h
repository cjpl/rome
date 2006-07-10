/********************************************************************
  ROMELabel.h, M. Schneebeli PSI

  $Id: ROMELabel.h 1206 2006-07-05 17:23:11Z sawada $

********************************************************************/
#ifndef ROMELabel_H
#define ROMELabel_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TGLabel.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

class TGToolTip;

class ROMELabel : public TGLabel {
protected:
   TGToolTip     *fTip;         // tool tip associated with label
public:
   ROMELabel(const TGWindow *p = 0, const char *text = 0,
           GContext_t norm = GetDefaultGC()(),
           FontStruct_t font = GetDefaultFontStruct(),
           UInt_t options = kChildFrame,
           Pixel_t back = GetDefaultFrameBackground()) : TGLabel(p, text, norm, font, options, back) { };

   void SetToolTipText(const char *text, Long_t delayms);
   Bool_t HandleCrossing(Event_t *event);

   ClassDef(ROMELabel, 0) // Customized TGLabel for ROME
};

#endif   // ROMELabel_H
