#include "include/daqs/HGdummyDAQ.h"
#include "include/generated/HGAnalyzer.h"
#include "include/generated/HGConfig.h"

HGdummyDAQ::HGdummyDAQ()
{
}


bool HGdummyDAQ::Init()
{
   return true;
}

bool HGdummyDAQ::BeginOfRun()
{
   gAnalyzer->SetCurrentEventNumber(0);
   return true;
}

bool HGdummyDAQ::Event(Long64_t /*event*/)
{
   gAnalyzer->SetCurrentEventNumber(gAnalyzer->GetCurrentEventNumber() + 1);
   if (gAnalyzer->GetCurrentEventNumber() > 10000)
      SetEndOfRun();
   return true;
}

bool HGdummyDAQ::EndOfRun()
{
   return true;
}

bool HGdummyDAQ::Terminate()
{
   return true;
}

