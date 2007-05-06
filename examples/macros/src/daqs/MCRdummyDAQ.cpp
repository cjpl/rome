////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MCRdummyDAQ                                                                //
//                                                                            //
// dummy DAQ class for MCRAnalyzer.                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <TMath.h>
#include "daqs/MCRdummyDAQ.h"
#include "generated/MCRAnalyzer.h"
#include "generated/MCRConfig.h"
#include "generated/MCRPMTData.h"

ClassImp(MCRdummyDAQ)
MCRdummyDAQ::MCRdummyDAQ()
:fRandom(new TRandom())
{
}

Bool_t MCRdummyDAQ::Init()
{
   return true;
}

Bool_t MCRdummyDAQ::BeginOfRun()
{
   return true;
}

Bool_t MCRdummyDAQ::Event(Long64_t event)
{
   if (event >= 100000) {
      SetEndOfRun();
      return kTRUE;
   }

   gAnalyzer->SetCurrentEventNumber(event);

   Int_t iPmt;
   for(iPmt = 0; iPmt < 10; iPmt++)
      gAnalyzer->GetPMTDataAt(iPmt)->SetADC(TMath::Nint(fRandom->Gaus(1000, 31.6)));

   return true;
}

Long64_t MCRdummyDAQ::Seek(Long64_t /* event */)
{
   return -1;
}

Bool_t MCRdummyDAQ::EndOfRun()
{
   return true;
}

Bool_t MCRdummyDAQ::Terminate()
{
   return true;
}

