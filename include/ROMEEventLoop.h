#ifndef ROMEEventLoop_H
#define ROMEEventLoop_H

#include <TTask.h>
#include "ROMEAnalyzer.h"

class ROMEEventLoop : public TTask {
protected:
   ROMEAnalyzer *fAnalyzer; // Handle to the ROMEAnalyzer Class
   TFile **files;          //! Root files
   Int_t midFileHandle;    // Handle to Midas Inputfile
public:
   ROMEEventLoop() { ; }
   ROMEEventLoop(const char *name,const char *title,ROMEAnalyzer *analyzer);
   void ExecuteTask(Option_t *option="");
   void Exec(Option_t *option="") {};
   virtual void ReadRunTable() = 0;
   virtual void SaveRunTable() = 0;
   virtual void InitDataBase() = 0;
   virtual void UpdateDataBase() = 0;
   virtual void ConnectTrees() = 0;
private:
   void ReadRootFiles();
//   void ProcessMessage(HNDLE hBuf, HNDLE id, EVENT_HEADER * pheader, void *message);
//   void ProcessEvent(HNDLE hBuf, HNDLE request_id, EVENT_HEADER * pheader, void *pevent);

   ClassDef(ROMEEventLoop,1)
};

#endif   // ROMEEventLoop_H
