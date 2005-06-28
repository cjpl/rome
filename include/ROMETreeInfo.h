/********************************************************************
  ROMETreeInfo.h, M. Schneebeli PSI

  $Log$
  Revision 1.4  2005/06/28 14:53:30  sawada
  renamed members of ROMETreeInfo.

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
   Int_t   run;       // Run Number
   Int_t   number;    // Sequential Number
   Int_t   event;     // Event Number
   Int_t   time;      // Time Stamp

public:
   ROMETreeInfo() { ROMETreeInfo::Class()->IgnoreTObjectStreamer(); };

   Int_t GetSequentialNumber() { return number; };

   void  SetSequentialNumber(Int_t   SequentialNumber) { number    = SequentialNumber; };
   void  SetRunNumber       (Int_t   RunNumber       ) { run       = RunNumber;        };
   void  SetEventNumber     (Int_t   EventNumber     ) { event     = EventNumber;      };
   void  SetTimeStamp       (Int_t   TimeStamp       ) { time      = TimeStamp;        };

   ClassDef(ROMETreeInfo,1)
};

#endif   // ROMETreeInfo_H
