#include <include/framework/RDBdummy.h>
#include <include/framework/RDBAnalyzer.h>

RDBdummy::RDBdummy()
{
}


bool RDBdummy::Init()
{
   return true;
}

bool RDBdummy::BeginOfRun()
{
   return true;
}

bool RDBdummy::Event(int event)
{
   SetEndOfRun();
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

