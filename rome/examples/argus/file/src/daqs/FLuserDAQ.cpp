#include <include/daqs/FLuserDAQ.h>
#include <include/generated/FLAnalyzer.h>
#include <include/generated/FLConfig.h>
#include "TSystem.h"

FLuserDAQ::FLuserDAQ()
{
}

bool FLuserDAQ::Init()
{
   cout << "user DAQ: init" << endl;
   FLRomeDAQ::Init();
   return true;
}

bool FLuserDAQ::BeginOfRun()
{
   FLRomeDAQ::BeginOfRun();
   cout << "user DAQ: begin of run" << endl;
   return true;
}

bool FLuserDAQ::Event(Long64_t event)
{
   FLRomeDAQ::Event(event);
   gSystem->Sleep(100);
   cout << "user DAQ: event " << event << endl;
   return true;
}

bool FLuserDAQ::EndOfRun()
{
   FLRomeDAQ::EndOfRun();
   cout << "user DAQ: end of run" << endl;
   return true;
}

bool FLuserDAQ::Terminate()
{
   FLRomeDAQ::Terminate();
   cout << "user DAQ: terminate" << endl;
   return true;
}
