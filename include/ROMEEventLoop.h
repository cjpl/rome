#ifndef ROMEEventLoop_H
#define ROMEEventLoop_H

#include <ROMETask.h>
#include "ROMEAnalyzer.h"

class ROMEEventLoop : public ROMETask {
protected:
   ROMEAnalyzer *fAnalyzer; // Handle to the ROMEAnalyzer Class
public:
   ROMEEventLoop() { ; }
   ROMEEventLoop(const char *name,const char *title,ROMEAnalyzer *analyzer);
   void ExecuteTask(Option_t *option="");
   void Exec(Option_t *option="") {};
   void BookHisto() {};
   void ResetHisto() {};
   void Init() {};
   void BeginOfRun() {};
   void Event() {};
   void EndOfRun() {};
   void Terminate() {};
};

#endif   // ROMEEventLoop_H
