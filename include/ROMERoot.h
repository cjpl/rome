/********************************************************************
  ROMERoot.h, M. Schneebeli PSI

  $Log$
  Revision 1.2  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.1  2005/01/24 15:25:06  schneebeli_m
  Seperated DAQ classes


********************************************************************/
#ifndef ROMERoot_H
#define ROMERoot_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMERoot : public ROMEDAQSystem {
protected:
   TFile**       fRootFiles;                       //! Input Root files

   // Tree Info
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int*          fTreePosition;                    //! Array with tree read positions
   int*          fTreeNextSeqNumber;               //! Array with the trees next sequential number

   int           fTimeStamp;                       //! Current time stamp

public:
   ROMERoot();

   int   GetTimeStamp() { return fTimeStamp; };
   const char* GetName() { return "root"; };

   bool Initialize();
   bool Connect();
   bool ReadEvent(int event);
   bool Disconnect();
   bool Termination();

   virtual void ConnectTrees() = 0;

};

#endif   // ROMERoot_H
