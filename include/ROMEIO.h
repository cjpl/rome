#ifndef ROMEIO_H
#define ROMEIO_H

#include <TFile.h>
#include <TString.h>
#include <TArrayI.h>
#include <TObjArray.h>
#include <ROMETree.h>
#include <ROMETreeInfo.h>
#include <ROMEStatic.h>
#include <ROME.h>
#if defined HAVE_MIDAS
#include <midas.h>
#endif
#ifndef __CINT__
  #include <ROMESQL.h>
#endif
#include <Riostream.h>

class ROMEIO {

private:
   // Run Status
   enum {
      kAnalyze   = 0,
      kContinue  = 1,
      kEndOfRun  = 2,
      kTerminate = 3
   };
   // Data Format
   enum {
      kRoot  = 0,
      kMidas = 1
   };
   // Analysis Mode
   enum {
      kAnalyzeOffline = 0,
      kAnalyzeOnline  = 1
   };
   // Data Base
   enum {
      kDataBaseNone = 0,
      kDataBaseSQL  = 1,
      kDataBaseXML  = 2
   };

   Int_t      fAnalysisMode;                    //! Analysis mode flag
   Int_t      fDataFormat;                      //! Root mode flag
   Int_t      fDataBase;                        //! Data Base flag

   Int_t      fRunStatus;                       //! Run Status flag

   TString    fInputDir;                        //! General Input Directory
   TString    fOutputDir;                       //! General Output Directory
   TString    fDataBaseDir;                     //! Data Base File Directory

   Int_t      fIndexOfCurrentRunNumber;         //! Index of currently Analyzed Run Number
   TArrayI    fRunNumber;                       //! Run Numbers to Analyze
   TString    fRunNumberString;                 //! Run Numbers in Input String Format
   Int_t      fCurrentEventNumber;              //! Index of currently Analyzed Event Number
   TArrayI    fEventNumber;                     //! Event Numbers to Analyze
   TString    fEventNumberString;               //! Event Numbers in Input String Format

   char       fEventID[10];                     //! Event ID of current Event
   Int_t      fMidasFileHandle;                 //! Handle to Midas Inputfile
   int        fMidasBuffer;                     //! Midas Online Buffer
   TFile**    fRootFiles;                       //! Root files
   char       fMidasEvent[1000000];             //! Midas Inputdata Stack for an Event

protected:
   TFile**       treeFiles;                        //! File Handles for Tree Objects

   TObjArray*    fTreeObjects;                     //! Handle to Tree Objects
   bool          fTreeAccumulation;                //! Accumulation of runs in Trees

   int           fSequentialNumber;                //! Sequential Number
   ROMETreeInfo* fTreeInfo;                        //! Tree Info Object
   int*          fTreePosition;                    //! Array with tree read positions
   int*          fTreeNextSeqNumber;               //! Array with the trees next sequential number

#ifndef __CINT__
   ROMESQL*   fSQL;                             //! Handle to SQL Class
#endif

public:
   ROMEIO();
   ~ROMEIO(); 

   // methods

   // inline methods
   bool Init();
   bool Connect(Int_t runNumber);
   bool ReadEvent(Int_t event);
   bool UserInput();
   bool WriteEvent();
   bool Disconnect();
   bool Terminate();

   EVENT_HEADER* GetEventHeader() { return (EVENT_HEADER*)fMidasEvent; };

   // Run Status
   Bool_t     isAnalyze()   { return fRunStatus==kAnalyze; };
   Bool_t     isContinue()  { return fRunStatus==kContinue; };
   Bool_t     isEndOfRun()  { return fRunStatus==kEndOfRun; };
   Bool_t     isTerminate() { return fRunStatus==kTerminate; };

   void       SetAnalyze()   { fRunStatus = kAnalyze; };
   void       SetContinue()  { fRunStatus = kContinue; };
   void       SetEndOfRun()  { fRunStatus = kEndOfRun; };
   void       SetTerminate() { fRunStatus = kTerminate; };

   // Analysis Mode
   Bool_t     isOnline() { return fAnalysisMode==kAnalyzeOnline; };
   Bool_t     isOffline() { return fAnalysisMode==kAnalyzeOffline; };

   void       SetOnline()  { fAnalysisMode = kAnalyzeOnline; };
   void       SetOffline() { fAnalysisMode = kAnalyzeOffline; };

   // Data Format
   Bool_t     isRoot() { return fDataFormat==kRoot; };
   Bool_t     isMidas() { return fDataFormat==kMidas; };

   void       SetRoot()  { fDataFormat = kRoot; };
   void       SetMidas() { fDataFormat = kMidas; };

   // Data Base
   Bool_t     isSQLDataBase() { return fDataBase==kDataBaseSQL; };
   Bool_t     isXMLDataBase() { return fDataBase==kDataBaseXML; };

   void       SetNoDataBase()  { fDataBase = kDataBaseNone; };
   void       SetSQLDataBase() { fDataBase = kDataBaseSQL; };
   void       SetXMLDataBase() { fDataBase = kDataBaseXML; };

   // Tree IO
   Bool_t     isTreeAccumulation()  { return fTreeAccumulation;  };

   void       SetTreeAccumulation() { fTreeAccumulation = true;  };

   // Trees
   void       AddTree(TTree *tree) { fTreeObjects->Add(new ROMETree(tree,0,0)); };
   ROMETree*  GetTreeObjectAt(int index) { return (ROMETree*)fTreeObjects->At(index); };
   int        GetTreeObjectEntries() { return fTreeObjects->GetEntries(); };

   // Directories
   char*      GetInputDir() { return (char*)fInputDir.Data(); }
   char*      GetOutputDir() { return (char*)fOutputDir.Data(); }
   char*      GetDataBaseDir() { return (char*)fDataBaseDir.Data(); }

   void       SetInputDir(char* dir) { fInputDir = dir; }
   void       SetOutputDir(char* dir) { fOutputDir = dir; }
   void       SetDataBaseDir(char* dir) { fDataBaseDir = dir; }

   // Run Number
   void       GetRunNumberStringAt(char* buffer,int i) { sprintf(buffer,"%05d",fRunNumber.At(i)); }
   int        GetRunNumberAt(int i) { return fRunNumber.At(i); }
   void       GetCurrentRunNumberString(char* buffer) { sprintf(buffer,"%05d",fRunNumber.At(fIndexOfCurrentRunNumber)); }
   int        GetCurrentRunNumber() { return fRunNumber.At(fIndexOfCurrentRunNumber); }
   int        GetNumberOfRunNumbers() { return fRunNumber.GetSize(); }
   char*      GetRunNumberStringOriginal() { return (char*)fRunNumberString.Data(); }

   void       SetRunNumbers(char* numbers) { 
                  fRunNumberString = numbers;
                  fRunNumber = ROMEStatic::decodeRunNumbers(numbers); }

   int        GetCurrentEventNumber() { return fCurrentEventNumber; }
   char*      GetEventNumberStringOriginal() { return (char*)fEventNumberString.Data(); }

   void       SetEventNumbers(char* numbers) { 
                  fEventNumberString = numbers;
                  fEventNumber = ROMEStatic::decodeRunNumbers(numbers); }

   // Event ID
   char*      GetEventID() { return fEventID; }
   void       SetEventID(char* eventID) { strcpy(fEventID,eventID); }
   void       SetEventID(int eventID) { sprintf(fEventID,"%d",eventID); }

private:
   virtual void ConnectTrees() = 0;
   virtual void FillTrees() = 0;
   virtual void ClearFolders() = 0;

   virtual bool ReadXMLDataBase() = 0;

   virtual bool InitSQLDataBase() = 0;
   virtual bool ReadSQLDataBase() = 0;

   virtual void InitMidasBanks() = 0;
};

#endif   // ROMEIO_H
