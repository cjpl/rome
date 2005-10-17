/********************************************************************
  ROMERoot.h, M. Schneebeli PSI

  $Id:$

********************************************************************/
#ifndef ROMERoot_H
#define ROMERoot_H

#include <ROMEAnalyzer.h>
#include <ROMEDAQSystem.h>

class ROMERoot : public ROMEDAQSystem {
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
