#include <include/daqs/RDBdummy.h>
#include <include/generated/RDBAnalyzer.h>

RDBdummy::RDBdummy()
{
}


bool RDBdummy::Init()
{
   return true;
}

bool RDBdummy::BeginOfRun()
{
   gAnalyzer->SetCurrentEventNumber(0);
   this->SetAnalyze();
   this->SetRunning();
   return true;
}

bool RDBdummy::Event(int event)
{
   gAnalyzer->SetCurrentEventNumber(event);
   return true;
}

bool RDBdummy::EndOfRun()
{
   return true;
}

bool RDBdummy::Terminate()
{
   return true;
}

