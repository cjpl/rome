/********************************************************************
  ROMEMidas.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2005/01/24 15:25:06  schneebeli_m
  Seperated DAQ classes


********************************************************************/
#ifndef ROMEMidas_H
#define ROMEMidas_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMEMidas : public ROMEDAQSystem {
protected:
   int        fMidasOnlineDataBase;             //! Handle to the Midas Online Data Base
   int        fMidasOnlineBuffer;               //! Midas Online Buffer
   int        fMidasFileHandle;                 //! Handle to Midas Inputfile

   int        fTimeStamp;                       //! Current time stamp

public:
   ROMEMidas::ROMEMidas();

   int   GetMidasOnlineDataBase() { return fMidasOnlineDataBase; };
   int*  GetMidasOnlineDataBasePointer() { return &fMidasOnlineDataBase; };

   int   GetTimeStamp() { return fTimeStamp; };

   bool Initialize();
   bool Connect();
   bool ReadEvent(int event);
   bool Disconnect();
   bool Termination();

   virtual bool InitODB() = 0;
};

#endif   // ROMEMidas_H
