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
#include <ROMEIO.h>
#include <ROME.h>
#ifdef HAVE_MIDAS
#include <midas.h>
#endif

class ROMEAnalyzer : public TObject
{
protected:
   Bool_t     fSplashScreen;              //! Splash screen flag
   Bool_t     fBatchMode;                 //! Batch mode flag
   TString    fConfigDir;                 //! Configuration File Directory
   TTask*     fMainTask;                  //! Handle to Main Task
   TFolder*   fMainFolder;                //! Handle to Main Folder
   TFile*     fHistoFiles;                //! Handle to Histogram Files
   TList*     fRunTable;                  //! RunTable Objects
   int        fCurrentRunTablePos;        //! Current RunTable Position

   Bool_t     fFillEvent;                 //! Fill Event Flag

   ROMEIO*    fIO;                         //! Handle to IO Class

public:
   ROMEAnalyzer();
   // IO
   ROMEIO*    GetIO() { return fIO; };

   // modes
   Bool_t     isSplashScreen() { return fSplashScreen; };
   Bool_t     isBatchMode() { return fBatchMode; };

   // directories
   char*      GetConfigDir() { return (char*)fConfigDir.Data(); }
   char*      GetDataBaseDir() { return this->fIO->GetDataBaseDir(); }
   char*      GetOutputDir() { return this->fIO->GetOutputDir(); }
   char*      GetInputDir() { return this->fIO->GetInputDir(); }

   void       SetConfigDir(char* dir) { fConfigDir = dir; }

   // main objects
   TFolder*   GetMainFolder() { return fMainFolder; }
   TTask*     GetMainTask() { return fMainTask; }
   TList*     GetRunTable() { return fRunTable; };
   int        GetCurrentRunTablePos() { return fCurrentRunTablePos; };
   TFile*     GetHistoFileHandle() { return fHistoFiles; };

   void       SetCurrentRunTablePos(int pos) { fCurrentRunTablePos = pos; };
   void       SetHistoFileHandle(TFile *file) { fHistoFiles = file; };

   // Fill Event Flag
   Bool_t     isFillEvent() { return fFillEvent; };

   void       SetFillEvent(Bool_t fillEvent = true) { fFillEvent = fillEvent; }


   // methods

   bool       Start(int argc=0, char **argv=NULL);

   virtual void ClearFolders() = 0;

private:
   void CreateHistoFolders();

   bool ReadParameters(int argc, char *argv[]);
   virtual bool ReadROMEConfigXML(char *configFile) = 0;
   virtual bool WriteROMEConfigXML(char *configFile) = 0;

   virtual void startSplashScreen() = 0;
   virtual void consoleStartScreen() = 0;

   static void intToNDigitsChar(char *buffer,int numberOfDigits,int number);
};

#endif   // ROMEAnalyzer_H
