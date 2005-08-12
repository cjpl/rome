/********************************************************************
  ROMERoot.h, M. Schneebeli PSI

  $Log$
  Revision 1.5  2005/08/12 15:37:02  schneebeli_m
  added input file based IO

  Revision 1.4  2005/06/16 02:52:54  sawada
  added virtual destructer.

  Revision 1.3  2005/06/13 15:49:04  schneebeli_m
  changed name of DAQ user functions

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
public:
   TFile**       fRootFiles;                       //! Input Root files
   int           fTreeIndex;                       //! Index of tree in current file
   int           fInputFileNameIndex;              //! Index of current input file name

   // Tree Info
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int*          fTreePosition;                    //! Array with tree read positions
   int*          fTreeNextSeqNumber;               //! Array with the trees next sequential number

   int           fTimeStamp;                       //! Current time stamp

public:
   ROMERoot();
   virtual ~ROMERoot() {};

   int   GetTimeStamp() { return fTimeStamp; };
   const char* GetName() { return "root"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();

   virtual void ConnectTrees() = 0;

};

#endif   // ROMERoot_H
