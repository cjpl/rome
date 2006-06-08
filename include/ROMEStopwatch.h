/********************************************************************
  ROMEStopwatch.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMEStopwatch_H
#define ROMEStopwatch_H

#include <TStopwatch.h>
#include <ROMEString.h>

class ROMEStopwatch : public TStopwatch
{
public:
   ROMEStopwatch():TStopwatch() {};
   const char* GetRealTimeString(ROMEString& time) {
                  Int_t runTime = (Int_t)this->RealTime();
                  Int_t milli = (Int_t)((this->RealTime()-runTime)*1000);
                  time.SetFormatted("%d%d:%d%d:%d%d:%03d", (Int_t)(runTime / 36000), (Int_t)((runTime % 36000) / 3600),
                                    (Int_t)((runTime % 3600) / 600), (Int_t)((runTime % 600) / 60), (Int_t)((runTime % 60) / 10),
                                    (Int_t)(runTime % 10),milli);
                  return time.Data();
   }
   ClassDef(ROMEStopwatch, 0) // Customized TStopwatch for ROME
};

#endif   // ROMEStopwatch_H
