#ifndef ROMEAnalyzer_H
#define ROMEAnalyzer_H

#include <TTree.h>
#include <TFile.h>
#include <TList.h>
#include <TTask.h>
#include <TFolder.h>
#include <TClonesArray.h>
#include <TArrayI.h>
#include <ROMETree.h>
#include <ROME.h>

class ROMEAnalyzer : public TObject
{
protected:
   enum {
      kBeginOfRun = 1,
      kEndOfRun   = 2,
      kTerminate  = 3
   };
   Bool_t     fOnLineMode;                //! Online mode flag
   Bool_t     fRootMode;                  //! Root mode flag
   Bool_t     fBatchMode;                 //! Batch mode flag
   Int_t      fRunStatus;                 //! Flags the Run Status
   TString    fConfigDir;                 //! Configuration File Directory
   TString    fDataBaseDir;               //! Data Base File Directory
   TString    fInputDir;                  //! General Input Directory
   TString    fOutputDir;                 //! General Output Directory
   Int_t      fIndexOfCurrentRunNumber;   //! Index of currently Analyzed Run Number
   TArrayI    fRunNumber;                 //! Run Numbers to Analyze
   TArrayI    fEventNumber;               //! Event Numbers to Analyze
   TObjArray* fTreeObjects;               //! Handle to Tree Objects
   TTask*     fMainTask;                  //! Handle to Main Task
   TFolder*   fMainFolder;                //! Handle to Main Folder
   TFile*     fHistoFiles;                //! Handle to Histogram Files
   TList*     fRunTable;                  //! RunTable Objects
   int        fCurrentRunTablePos;        //! Current RunTable Position

   char fMidasEvent[gMidasEventSize];     //! Midas Inputdata Stack for an Event

public:
   ROMEAnalyzer();
   Bool_t     isOnline() { return fOnLineMode; };
   Bool_t     isOffline() { return !fOnLineMode; };
   Bool_t     isRoot() { return fRootMode; };
   Bool_t     isMidas() { return !fRootMode; };
   Bool_t     isBatchMode() { return fBatchMode; };

   Bool_t     isEndOfRun() { return fRunStatus==kEndOfRun; };
   Bool_t     isBeginOfRun() { return fRunStatus==kBeginOfRun; };
   Bool_t     isTerminate() { return fRunStatus==kTerminate; };
   void       EndOfRun() { fRunStatus = kEndOfRun; };
   void       BeginOfRun() { fRunStatus = kBeginOfRun; };
   void       Terminate() { fRunStatus = kTerminate; };
   
   void       AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree,0,0,0,0,0)); };

   char*      GetConfigDir() { return (char*)fConfigDir.Data(); }
   char*      GetDataBaseDir() { return (char*)fDataBaseDir.Data(); }
   char*      GetInputDir() { return (char*)fInputDir.Data(); }
   char*      GetOutputDir() { return (char*)fOutputDir.Data(); }
   void       GetRunNumberStringAt(char* buffer,int i) { intToNDigitsChar(buffer,5,fRunNumber.At(i)); }
   int        GetRunNumberAt(int i) { return fRunNumber.At(i); }
   void       GetCurrentRunNumberString(char* buffer) { intToNDigitsChar(buffer,5,fRunNumber.At(fIndexOfCurrentRunNumber)); }
   int        GetCurrentRunNumber() { return fRunNumber.At(fIndexOfCurrentRunNumber); }
   int        GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   TFolder*   GetMainFolder() { return fMainFolder; }
   TTask*     GetMainTask() { return fMainTask; }
   TFile*     GetHistoFileHandle() { return fHistoFiles; };
   ROMETree*  GetTreeObjectAt(int index) { return (ROMETree*)fTreeObjects->At(index); };
   int        GetTreeObjectEntries() { return fTreeObjects->GetEntries(); };
   TList*     GetRunTable() { return fRunTable; };
   int        GetCurrentRunTablePos() { return fCurrentRunTablePos; };

   void*      GetMidasEvent() { return (void*)fMidasEvent; };

   void       SetConfigDir(char* dir) { fConfigDir = dir; }
   void       SetDataBaseDir(char* dir) { fDataBaseDir = dir; }
   void       SetInputDir(char* dir) { fInputDir = dir; }
   void       SetOutputDir(char* dir) { fOutputDir = dir; }
   void       SetIndexOfCurrentRunNumber(int index) { fIndexOfCurrentRunNumber = index; }
   void       SetHistoFileHandle(TFile *file) { fHistoFiles = file; };
   void       SetCurrentRunTablePos(int pos) { fCurrentRunTablePos = pos; };

   bool       Start(int argc=0, char **argv=NULL);

private:
   bool ReadParameters(int argc, char *argv[]);
   int  ReadROMEConfigXML(char buffer[][200]);
   void CreateHistoFolders();
   virtual void startSplashScreen() = 0;
   virtual void consoleStartScreen() = 0;

   static void intToNDigitsChar(char *buffer,int numberOfDigits,int number)
   {
      for (int i=0;i<numberOfDigits;i++) 
         buffer[i] = 48+(int)((number % (int)pow(10,numberOfDigits-i)) / (int)pow(10,numberOfDigits-i-1));
      
      buffer[numberOfDigits] = 0;
   }

   ClassDef(ROMEAnalyzer,1)
};

#endif   // ROMEAnalyzer_H
