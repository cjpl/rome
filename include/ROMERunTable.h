#ifndef ROMERunTable_H
#define ROMERunTable_H

#include<TString.h>
#include<TObject.h>

class ROMERunTable : public TObject 
{
protected:
   Int_t    fTimeStamp;          // Time Stamp
   Int_t    fRunNumber;          // Run Number
   TString  fRunDescription;     // Run Description
   Int_t    fNumberOfFiles;      // Number Of Date Base XML Files containing Container Data
   TString* fFile;               // Date Base XML Files containing Container Data
   TString* fFolder;             // Folder containing Container
public:
   ROMERunTable(Int_t timeStamp=0,Int_t runNumber=0,TString runDescription=0,Int_t numberOfFiles=0,TString *file=NULL,TString *folder=NULL)
   { fTimeStamp = timeStamp; fRunNumber = runNumber; fRunDescription = runDescription; fNumberOfFiles = numberOfFiles; SetFile(file); SetFolder(folder); };
   Int_t     GetTimeStamp()      { return fTimeStamp;      };
   Int_t     GetRunNumber()      { return fRunNumber;      };
   TString   GetRunDescription() { return fRunDescription; };
   Int_t     GetNumberOfFiles()  { return fNumberOfFiles;  };
   TString*  GetFile();
   TString*  GetFolder()         { return fFolder;         };
   void SetTimeStamp     (Int_t    timeStamp)      { fTimeStamp      = timeStamp;      };
   void SetRunNumber     (Int_t    runNumber)      { fRunNumber      = runNumber;      };
   void SetRunDescription(TString  runDescription) { fRunDescription = runDescription; };
   void SetNumberOfFiles (Int_t    number)         { fNumberOfFiles  = number;         };
   void SetFile          (TString* file);
   void SetFolder        (TString* folder);
};

#endif   // ROMERunTable_H
