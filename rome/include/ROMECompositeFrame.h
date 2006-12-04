/********************************************************************
  ROMECompositeFrame.h, M. Schneebeli PSI

  $Id: ROMECompositeFrame.h 1433 2006-10-10 17:11:13Z sawada $

********************************************************************/
#ifndef ROMECompositeFrame_H
#define ROMECompositeFrame_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

class ROMECompositeFrame : public TGCompositeFrame {
public:
   ROMECompositeFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = kChildFrame,
                     Pixel_t back = GetDefaultFrameBackground()) : TGCompositeFrame(p, w, h, options, back) { };

   void HideFrame(TGFrame *f);

   ClassDef(ROMECompositeFrame, 0) // Customized TGCompositeFrame for ROME
};

#endif   // ROMECompositeFrame_H
