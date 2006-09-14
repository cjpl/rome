/********************************************************************
  ROMETreeInfo.h, M. Schneebeli PSI

  $Id$

********************************************************************/
#ifndef ROMETreeInfo_H
#define ROMETreeInfo_H

#include <TObject.h>
#include <TClass.h>

class ROMETreeInfo : public TObject
{
protected:
   Long64_t run;            // Run Number
   Long64_t event;          // Event Number
   Int_t    time;           // Time Stamp
   Int_t    rawEventNumber; // Event Number in Raw Data
   Int_t    rawEventType;   // Event Type in Raw Data

public:
   ROMETreeInfo() { ROMETreeInfo::Class()->IgnoreTObjectStreamer(); }
   virtual ~ROMETreeInfo() {};

   Long64_t GetRunNumber()      { return run; }
   Long64_t GetEventNumber()    { return event; }
   Int_t    GetTimeStamp()      { return time; }
   Int_t    GetRawEventNumber() { return rawEventNumber; }
   Int_t    GetRawEventType()   { return rawEventType; }

   void     SetRunNumber       (Long64_t RunNumber       ) { run            = RunNumber; }
   void     SetEventNumber     (Long64_t EventNumber     ) { event          = EventNumber; }
   void     SetTimeStamp       (Int_t    TimeStamp       ) { time           = TimeStamp; }
   void     SetRawEventNumber  (Int_t    RawEventNumber  ) { rawEventNumber = RawEventNumber; }
   void     SetRawEventType    (Int_t    RawEventType    ) { rawEventType   = RawEventType; }

   ClassDef(ROMETreeInfo,3) // Event header of trees
};

#endif   // ROMETreeInfo_H