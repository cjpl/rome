/********************************************************************
  ROMEHorizontalFrame.h, M. Schneebeli PSI

  $Id: ROMEHorizontalFrame.h 1206 2006-07-05 17:23:11Z sawada $

********************************************************************/
#ifndef ROMEHorizontalFrame_H
#define ROMEHorizontalFrame_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif

class ROMEHorizontalFrame : public TGHorizontalFrame {
public:
   ROMEHorizontalFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1, UInt_t options = kChildFrame,
                     Pixel_t back = GetDefaultFrameBackground()) : TGHorizontalFrame(p, w, h, options, back) { };

   void HideFrame(TGFrame *f);

   ClassDef(ROMEHorizontalFrame, 0) // Customized TGHorizontalFrame for ROME
};

#endif   // ROMEHorizontalFrame_H
