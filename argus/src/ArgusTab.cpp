// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ArgusTab                                                             //
//                                                                      //
//  The Base Class for all Tabs in ARGUS.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "ROMEiostream.h"
#include "ArgusTab.h"
#include "ROMEAnalyzer.h"

ClassImp(ArgusTab)

ArgusTab::ArgusTab():TGCompositeFrame(NULL,1,1) {
   fActive  = kFALSE;
   fBusy = false;
}

void ArgusTab::ArgusInit() {
   // Init EventHandler
   BaseInit();
}

ArgusTab::~ArgusTab() {
}

void ArgusTab::ArgusEventHandler() {
   if (fBusy)
      return;
   fBusy = true;
   BaseEventHandler();
   fBusy = false;
}
