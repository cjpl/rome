/********************************************************************
  ROMETreeInfo.h, M. Schneebeli PSI

  $Log$
  Revision 1.3  2005/06/27 07:41:06  schneebeli_m
  added IgnoreTObjectStreamer

  Revision 1.2  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMETreeInfo_H
#define ROMETreeInfo_H

#include<TObject.h>
#include<TClass.h>

class ROMETreeInfo : public TObject 
{
protected:
   Int_t   fSequentialNumber; // Sequential Number
   Int_t   fRunNumber;        // Run Number
   Int_t   fEventNumber;      // Event Number
   Int_t   fTimeStamp;        // Time Stamp

public:
   ROMETreeInfo() { ROMETreeInfo::Class()->IgnoreTObjectStreamer(); };

   Int_t GetSequentialNumber() { return fSequentialNumber; };

   void  SetSequentialNumber(Int_t   SequentialNumber) { fSequentialNumber = SequentialNumber; };
   void  SetRunNumber       (Int_t   RunNumber       ) { fRunNumber        = RunNumber;        };
   void  SetEventNumber     (Int_t   EventNumber     ) { fEventNumber      = EventNumber;      };
   void  SetTimeStamp       (Int_t   TimeStamp       ) { fTimeStamp        = TimeStamp;        };

   ClassDef(ROMETreeInfo,1)
};

#endif   // ROMETreeInfo_H
