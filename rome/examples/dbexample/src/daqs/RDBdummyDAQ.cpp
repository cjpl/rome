#include <include/daqs/RDBdummyDAQ.h>
#include <include/generated/RDBAnalyzer.h>

RDBdummyDAQ::RDBdummyDAQ()
{
}

bool RDBdummyDAQ::Init()
{
   return true;
}

bool RDBdummyDAQ::BeginOfRun()
{
   gAnalyzer->SetCurrentEventNumber(0);
   this->SetAnalyze();
   this->SetRunning();
   return true;
}

bool RDBdummyDAQ::Event(Long64_t event)
{
   gAnalyzer->SetCurrentEventNumber(event);
   if (event >= 2)
      SetEndOfRun();
   return true;
}

bool RDBdummyDAQ::EndOfRun()
{
   return true;
}

bool RDBdummyDAQ::Terminate()
{
   return true;
}

