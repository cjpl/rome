/********************************************************************
  ROMETreeInfo.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMETreeInfo_H
#define ROMETreeInfo_H

#include<TObject.h>
#include<TClass.h>

class ROMETreeInfo : public TObject
{
protected:
   Int_t   run;       // Run Number
   Int_t   number;    // Sequential Number
   Int_t   event;     // Event Number
   Int_t   time;      // Time Stamp

public:
   ROMETreeInfo() { ROMETreeInfo::Class()->IgnoreTObjectStreamer(); };

   Int_t GetSequentialNumber() { return number; };
   Int_t GetRunNumber() { return run; };
   Int_t GetEventNumber() { return event; };
   Int_t GetTimeStamp() { return time; };

   void  SetSequentialNumber(Int_t   SequentialNumber) { number    = SequentialNumber; };
   void  SetRunNumber       (Int_t   RunNumber       ) { run       = RunNumber;        };
   void  SetEventNumber     (Int_t   EventNumber     ) { event     = EventNumber;      };
   void  SetTimeStamp       (Int_t   TimeStamp       ) { time      = TimeStamp;        };

   ClassDef(ROMETreeInfo,1)
};

#endif   // ROMETreeInfo_H
