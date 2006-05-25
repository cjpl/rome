#include <include/daqs/FLuser.h>
#include <include/generated/FLAnalyzer.h>
#include <include/generated/FLConfig.h>
#include "TSystem.h"

FLuser::FLuser()
{
}

bool FLuser::Init()
{
   cout << "user DAQ: init" << endl;
   FLRomeDAQ::Init();
   return true;
}

bool FLuser::BeginOfRun()
{
   FLRomeDAQ::BeginOfRun();
   cout << "user DAQ: begin of run" << endl;
   return true;
}

bool FLuser::Event(Long64_t event)
{
   FLRomeDAQ::Event(event);
   gSystem->Sleep(100);
   cout << "user DAQ: event " << event << endl;
   return true;
}

bool FLuser::EndOfRun()
{
   FLRomeDAQ::EndOfRun();
   cout << "user DAQ: end of run" << endl;
   return true;
}

bool FLuser::Terminate()
{
   FLRomeDAQ::Terminate();
   cout << "user DAQ: terminate" << endl;
   return true;
}
