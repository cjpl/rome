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
   ROMEStopwatch():TStopwatch() {}
   virtual ~ROMEStopwatch(){}

   const char* GetRealTimeString(ROMEString& time) {
      Int_t runTime = static_cast<Int_t>(this->RealTime());
      Int_t milli = static_cast<Int_t>((this->RealTime()-runTime)*1000);
      time.SetFormatted("%d%d:%d%d:%d%d.%03d",
                        static_cast<Int_t>(runTime / 36000),
                        static_cast<Int_t>((runTime % 36000) / 3600),
                        static_cast<Int_t>((runTime % 3600) / 600),
                        static_cast<Int_t>((runTime % 600) / 60),
                        static_cast<Int_t>((runTime % 60) / 10),
                        static_cast<Int_t>(runTime % 10),
                        milli);
      return time.Data();
   }
   const char* GetCpuTimeString(ROMEString& time) {
      Int_t runTime = static_cast<Int_t>(this->CpuTime());
      Int_t milli = static_cast<Int_t>((this->CpuTime()-runTime)*1000);
      time.SetFormatted("%d%d:%d%d:%d%d.%03d",
                        static_cast<Int_t>(runTime / 36000),
                        static_cast<Int_t>((runTime % 36000) / 3600),
                        static_cast<Int_t>((runTime % 3600) / 600),
                        static_cast<Int_t>((runTime % 600) / 60),
                        static_cast<Int_t>((runTime % 60) / 10),
                        static_cast<Int_t>(runTime % 10),
                        milli);
      return time.Data();
   }
   ClassDef(ROMEStopwatch, 1) // Customized TStopwatch for ROME
};

#endif   // ROMEStopwatch_H
