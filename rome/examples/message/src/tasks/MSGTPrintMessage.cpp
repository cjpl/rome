////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MSGTPrintMessage                                                           //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/MSGTPrintMessageGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "generated/MSGAnalyzer.h"
#include "tasks/MSGTPrintMessage.h"
#include "ROMEiostream.h"
#include <TH1.h>
#include <TRandom.h>

// uncomment if you want to include headers of all folders
//#include "MSGAllFolders.h"


ClassImp(MSGTPrintMessage)

void MSGTPrintMessage::Init()
{
}

void MSGTPrintMessage::BeginOfRun()
{
}

void MSGTPrintMessage::Event()
{
   Report(R_INFO, "This is an info");
   Report(R_PLAIN, "This is a plain message");
   Report(R_VERBOSE, "This is shown when verbose level is more than 'verbose'");
   Report(R_WARNING, "This is a warning at event-%d", static_cast<Int_t>(gAnalyzer->GetCurrentEventNumber()));
   Report(R_ERROR, "This is an error at event-%d", static_cast<Int_t>(gAnalyzer->GetCurrentEventNumber()));
   Report(R_ERROR, "This is a long error.       |_________|_________|_________|_________|_________|");
   Report(R_ERROR, "This is an error\nwith multiple lines.");

   // you may use verbose level to control amount of messages from other libraries
   TString opt;
   if (gVerboseLevel >= kVerbose) {
      opt = "";
   } else {
      opt = "q";
   }

   Int_t i;
   TH1F *hist;
   if (gAnalyzer->GetCurrentEventNumber() == 0) {
      hist = new TH1F("hist", "hist", 10, 0, 10);
      for (i = 0; i < 100; i++) {
         hist->Fill(gRandom->Integer(10));
      }
      hist->Fit("gaus", opt.Data());
      delete hist;
   }
}

void MSGTPrintMessage::EndOfRun()
{
}

void MSGTPrintMessage::Terminate()
{
}

