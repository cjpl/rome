#ifndef ROMEIO_H
#define ROMEIO_H

#include <TFile.h>
#include <TString.h>
#include <TArrayI.h>
#include <TObjArray.h>
#include <ROMETree.h>
#include <ROMEStatic.h>
#include <ROME.h>
#include <ROMEIndexObject.h>
#include <ROMENameObject.h>
#if defined HAVE_MIDAS
#include <midas.h>
#endif
#ifndef __CINT__
  #include <ROMESQL.h>
#endif

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
   // Tree IO
   enum {
      kNone         = 0,
      kSynchronized = 1,
      kIndexFile    = 2,
      kEventNumber  = 3
   };

   Int_t      fAnalysisMode;                    //! Analysis mode flag
   Int_t      fDataFormat;                      //! Root mode flag
   Int_t      fDataBase;                        //! Data Base flag
   Int_t      fTreeMode;                        //! Tree IO Flag

   Int_t      fRunStatus;                       //! Run Status flag

   TString    fInputDir;                        //! General Input Directory
   TString    fOutputDir;                       //! General Output Directory
   TString    fDataBaseDir;                     //! Data Base File Directory

   Int_t      fIndexOfCurrentRunNumber;         //! Index of currently Analyzed Run Number
   TArrayI    fRunNumber;                       //! Run Numbers to Analyze
   TString    fRunNumberString;                 //! Run Numbers in Input String Format
   TArrayI    fEventNumber;                     //! Event Numbers to Analyze
   TString    fEventNumberString;               //! Event Numbers in Input String Format

   Int_t      fMidasFileHandle;                 //! Handle to Midas Inputfile
   int        fMidasBuffer;                     //! Midas Online Buffer
   TFile**    fRootFiles;                       //! Root files
   char       fMidasEvent[1000000];             //! Midas Inputdata Stack for an Event

protected:

   ROMEIndexObject* fIndexObject;               //! Object for the Index Tree index branch
   ROMENameObject*  fNameObject;                //! Object for the Index Tree name branch

   TTree*     fIndexTree;                       //! Handle to Index Tree
   TObjArray* fTreeObjects;                     //! Handle to Tree Objects
   bool       fFillTreeFirst;                   //! Flags the first FillTree call
   bool       fTreeAccumulation;                //! Accumulation of runs in Trees

#ifndef __CINT__
   ROMESQL*   fSQL;                             //! Handle to SQL Class
#endif

public:
   ROMEIO() 
   { fAnalysisMode = kAnalyzeOffline; 
     fDataFormat = kRoot; 
     fDataBase = kDataBaseNone; 
     fIndexOfCurrentRunNumber = 0; 
     fRunStatus = kAnalyze;
     fTreeMode = kNone;
     fTreeAccumulation = false;
     fTreeObjects = new TObjArray(0);
//     fIndexObject = new ROMEIndexObject();
//     fNameObject = new ROMENameObject();

     fIndexTree = new TTree("IndexTree","Index to other Trees"); 
     fIndexTree->Branch("Index","ROMEIndexObject",&fIndexObject,32000,99);
     fIndexTree->Branch("Name","ROMENameObject",&fNameObject,32000,99);
   };

   // methods

   // inline methods
   bool Init();
   bool Connect(Int_t runNumber);
   bool ReadEvent(Int_t eventNumber);
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
   Bool_t     isNoTreeMode()        { return fTreeMode==kNone; };
   Bool_t     isSynchronizedMode()  { return fTreeMode==kSynchronized; };
   Bool_t     isIndexFileMode()     { return fTreeMode==kIndexFile;    };
   Bool_t     isEventNumberMode()   { return fTreeMode==kEventNumber;  };
   Bool_t     isTreeAccumulation()  { return fTreeAccumulation;        };

   void       SetNoTreeMode()       { fTreeMode = kNone;         };
   void       SetSynchronizedMode() { fTreeMode = kSynchronized; };
   void       SetIndexFileMode()    { fTreeMode = kIndexFile;    };
   void       SetEventNumberMode()  { fTreeMode = kEventNumber;  };
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

   char*      GetEventNumberStringOriginal() { return (char*)fEventNumberString.Data(); }

   void       SetEventNumbers(char* numbers) { 
                  fEventNumberString = numbers;
                  fEventNumber = ROMEStatic::decodeRunNumbers(numbers); }

private:
   virtual void ConnectTrees() = 0;
   virtual void FillTrees() = 0;

   virtual void InitXMLDataBase() = 0;
   virtual void ReadXMLRunTable() = 0;
   virtual void SaveXMLRunTable() = 0;
   virtual void UpdateXMLDataBase() = 0;

   virtual bool InitSQLDataBase() = 0;
   virtual bool ReadSQLDataBase() = 0;

   virtual void InitMidasBanks() = 0;
};

#endif   // ROMEIO_H
