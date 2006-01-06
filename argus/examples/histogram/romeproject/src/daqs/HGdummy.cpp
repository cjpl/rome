#include "include/daqs/HGdummy.h"
#include "include/generated/HGAnalyzer.h"
#include "include/generated/HGConfig.h"

HGdummy::HGdummy()
{
}


bool HGdummy::Init()
{
   return true;
}

bool HGdummy::BeginOfRun()
{
   gAnalyzer->SetCurrentEventNumber(0);
   return true;
}

bool HGdummy::Event(int event)
{
   gAnalyzer->SetCurrentEventNumber(gAnalyzer->GetCurrentEventNumber() + 1);
   if (gAnalyzer->GetCurrentEventNumber() > 10000)
      SetEndOfRun();
   return true;
}

bool HGdummy::EndOfRun()
{
   return true;
}

bool HGdummy::Terminate()
{
   return true;
}

