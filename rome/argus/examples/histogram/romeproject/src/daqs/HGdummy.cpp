#include <include/generated/HGAnalyzer.h>
#include <include/generated/HGdummy.h>

HGdummy::HGdummy()
{
}

bool HGdummy::Initialize()
{
   return true;
}

bool HGdummy::Connect()
{
   gAnalyzer->SetCurrentEventNumber(0);
   return true;
}

bool HGdummy::ReadEvent(int event)
{
   gAnalyzer->SetCurrentEventNumber(gAnalyzer->GetCurrentEventNumber() + 1);
   if (gAnalyzer->GetCurrentEventNumber() > 10000)
      SetEndOfRun();
   return true;
}

bool HGdummy::Disconnect()
{
   return true;
}

bool HGdummy::Termination()
{
   return true;
}
