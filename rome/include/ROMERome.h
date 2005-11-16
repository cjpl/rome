/********************************************************************
  ROMERome.h, M. Schneebeli PSI

  $Id: ROMERome.h 480 2005-10-17 12:12:54Z schneebeli_m $

********************************************************************/
#ifndef ROMERome_H
#define ROMERome_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMERome : public ROMEDAQSystem {
protected:
   TFile**       fRootFiles;                       //! Input Root files
   int           fTreeIndex;                       //! Index of tree in current file
   int           fInputFileNameIndex;              //! Index of current input file name
   ROMEString    fCurrentTreeName;                 //! Name of the current tree

   // Tree Info
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int*          fTreePosition;                    //! Array with tree read positions
   int*          fTreeNextSeqNumber;               //! Array with the trees next sequential number

   int           fTimeStamp;                       //! Current time stamp

public:
   ROMERome();
   virtual ~ROMERome() {};

   int   GetTimeStamp() { return fTimeStamp; };
   const char* GetName() { return "rome"; };

   bool Init();
   bool BeginOfRun();
   bool Event(int event);
   bool EndOfRun();
   bool Terminate();

   virtual void ConnectTrees() = 0;

};

#endif   // ROMERome_H
