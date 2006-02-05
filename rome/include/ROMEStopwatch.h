/********************************************************************
  ROMEStopwatch.h, M. Schneebeli PSI

  $Id: $

********************************************************************/
#ifndef ROMEStopwatch_H
#define ROMEStopwatche_H

#include<TStopwatch.h>
#include<ROMEString.h>

class ROMEStopwatch : public TStopwatch
{
protected:
   TStopwatch fWatch;      //! Stopwatch

public:
   ROMEStopwatch():TStopwatch() {};
   const char* GetRealTime(ROMEString& time) {
                  Int_t runTime = (Int_t)fWatch.RealTime();
                  Int_t milli = (Int_t)((fWatch.RealTime()-runTime)*1000);
                  time.SetFormatted("%d%d:%d%d:%d%d:%d", (Int_t)(runTime / 36000), (Int_t)((runTime % 36000) / 3600),
                                    (Int_t)((runTime % 3600) / 600), (Int_t)((runTime % 600) / 60), (Int_t)((runTime % 60) / 10),
                                    (Int_t)(runTime % 10),milli);
                  return time.Data();
               }
};

#endif   // ROMEStopwatch_H
