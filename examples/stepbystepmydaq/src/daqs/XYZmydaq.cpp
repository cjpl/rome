#include "include/daqs/XYZmydaq.h"
#include "include/generated/XYZAnalyzer.h"
#include "include/generated/XYZConfig.h"

XYZmydaq::XYZmydaq()
{
}


Bool_t XYZmydaq::Init()
{
   return true;
}

Bool_t XYZmydaq::BeginOfRun()
{
   //-- Open data file
   char filename[100];
#if defined( R__VISUAL_CPLUSPLUS )
   sprintf(filename, "%s/data%05I64d.txt", gAnalyzer->GetInputDir(), gAnalyzer->GetCurrentRunNumber());
#else
   sprintf(filename, "%s/data%05lld.txt", gAnalyzer->GetInputDir(), gAnalyzer->GetCurrentRunNumber());
#endif
   fFile.open(filename);
   if(!fFile.good()) {
      cerr<<"Failed to open "<<filename<<" !"<<endl;
      return false;
   }

   //-- Get number of events in a file
   fFile>>fNumberOfEvent;
   return true;
}

Bool_t XYZmydaq::Event(Long64_t event)
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
      fFile>>adc;
      gAnalyzer->GetPMTDataAt(i)->SetADC(adc);
   }

   return true;
}

Bool_t XYZmydaq::EndOfRun()
{
   //-- Close file
   fFile.close();
   return true;
}

Bool_t XYZmydaq::Terminate()
{
   //-- Close file
   fFile.close();
   return true;
}

