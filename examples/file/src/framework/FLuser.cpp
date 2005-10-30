#include <include/framework/FLuser.h>
#include <include/framework/FLMonitor.h>
#include <include/framework/FLConfig.h>

FLuser::FLuser()
{
}

bool FLuser::Init()
{
   cout << "user DAQ: init" << endl;
   return true;
}

bool FLuser::BeginOfRun()
{
   cout << "user DAQ: begin of run" << endl;
   return true;
}

bool FLuser::Event(int event)
{
   cout << "user DAQ: event " << event << endl;
   return true;
}

bool FLuser::EndOfRun()
{
   cout << "user DAQ: end of run" << endl;
   return true;
}

bool FLuser::Terminate()
{
   cout << "user DAQ: terminate" << endl;
   return true;
}
