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
      int runTime = (int)fWatch.RealTime();
      int milli = (int)((fWatch.RealTime()-runTime)*1000);
      time.SetFormatted("%d%d:%d%d:%d%d:%d", (int)(runTime / 36000), (int)((runTime % 36000) / 3600),
            (int)((runTime % 3600) / 600), (int)((runTime % 600) / 60), (int)((runTime % 60) / 10),
            (int)(runTime % 10),milli);
      return time.Data();
   }
};

#endif   // ROMEStopwatch_H
