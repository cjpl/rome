#ifndef ROMEEventLoop_H
#define ROMEEventLoop_H

#include <TTask.h>
#include "ROMEAnalyzer.h"

class ROMEEventLoop : public TTask {
protected:
   ROMEAnalyzer *fAnalyzer; // Handle to the ROMEAnalyzer Class
public:
   ROMEEventLoop() { ; }
   ROMEEventLoop(const char *name,const char *title,ROMEAnalyzer *analyzer);
   void ExecuteTask(Option_t *option="");
   void Exec(Option_t *option="") {};
private:
//   void ProcessMessage(HNDLE hBuf, HNDLE id, EVENT_HEADER * pheader, void *message);
//   void ProcessEvent(HNDLE hBuf, HNDLE request_id, EVENT_HEADER * pheader, void *pevent);
};

#endif   // ROMEEventLoop_H
