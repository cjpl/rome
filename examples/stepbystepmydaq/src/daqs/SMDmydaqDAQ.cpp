#include "include/daqs/SMDmydaqDAQ.h"
#include "include/generated/SMDAnalyzer.h"
#include "include/generated/SMDConfig.h"
#include "include/generated/SMDPMTData.h"

SMDmydaqDAQ::SMDmydaqDAQ()
:fFile(0)
,fNumberOfEvent(0)
{
}

Bool_t SMDmydaqDAQ::Init()
{
   return true;
}

Bool_t SMDmydaqDAQ::BeginOfRun()
{
   //-- Open data file
   char filename[100];
#if defined( R__VISUAL_CPLUSPLUS )
   sprintf(filename, "%s/data%05I64d.txt", gAnalyzer->GetInputDir(), gAnalyzer->GetCurrentRunNumber());
#else
   sprintf(filename, "%s/data%05lld.txt", gAnalyzer->GetInputDir(), gAnalyzer->GetCurrentRunNumber());
#endif
   fFile = new fstream(filename);
   if(!fFile->good()) {
      cerr<<"Failed to open "<<filename<<" !"<<endl;
      return false;
   }

   //-- Get number of events in a file
   *fFile>>fNumberOfEvent;
   return true;
}

Bool_t SMDmydaqDAQ::Event(Long64_t /*event*/)
{
   //-- Check event number
   if (gAnalyzer->GetCurrentEventNumber() >= fNumberOfEvent) {
      SetEndOfRun();
      return true;
   }

   //-- Set event number
   gAnalyzer->SetCurrentEventNumber(gAnalyzer->GetCurrentEventNumber() + 1);

   //-- Read an event
   Int_t adc;
   Int_t i;
   for(i=0; i<10; i++) {
      *fFile>>adc;
      gAnalyzer->GetPMTDataAt(i)->SetADC(adc);
   }

   return true;
}

Bool_t SMDmydaqDAQ::EndOfRun()
{
   //-- Close file
   if (fFile) {
      fFile->close();
      delete fFile;
      fFile = 0;
   }
   return true;
}

Bool_t SMDmydaqDAQ::Terminate()
{
   //-- Close file
   if (fFile) {
      fFile->close();
      delete fFile;
      fFile = 0;
   }
   return true;
}

