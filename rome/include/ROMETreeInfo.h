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

public:
   ROMETreeInfo():TObject(),run(0),event(0),time(0) { ROMETreeInfo::Class()->IgnoreTObjectStreamer(); }
   virtual ~ROMETreeInfo() {};

   Long64_t GetRunNumber()      { return run; }
   Long64_t GetEventNumber()    { return event; }
   Int_t    GetTimeStamp()      { return time; }

   void     SetRunNumber       (Long64_t RunNumber       ) { run            = RunNumber; }
   void     SetEventNumber     (Long64_t EventNumber     ) { event          = EventNumber; }
   void     SetTimeStamp       (Int_t    TimeStamp       ) { time           = TimeStamp; }

   ClassDef(ROMETreeInfo ,4) // Event header of trees
};

#endif   // ROMETreeInfo_H
