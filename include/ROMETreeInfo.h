#ifndef ROMETreeInfo_H
#define ROMETreeInfo_H

#include<TObject.h>

class ROMETreeInfo : public TObject 
{
protected:
   Int_t   fSequentialNumber; // Sequential Number
   Int_t   fRunNumber;        // Run Number
   Int_t   fEventNumber;      // Event Number
   Int_t   fTimeStamp;        // Time Stamp

public:
   ROMETreeInfo() { };

   Int_t GetSequentialNumber() { return fSequentialNumber; };

   void  SetSequentialNumber(Int_t   SequentialNumber) { fSequentialNumber = SequentialNumber; };
   void  SetRunNumber       (Int_t   RunNumber       ) { fRunNumber        = RunNumber;        };
   void  SetEventNumber     (Int_t   EventNumber     ) { fEventNumber      = EventNumber;      };
   void  SetTimeStamp       (Int_t   TimeStamp       ) { fTimeStamp        = TimeStamp;        };

   ClassDef(ROMETreeInfo,1)
};

#endif   // ROMETreeInfo_H
