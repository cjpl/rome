////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TWR2TPrintValue                                                            //
//                                                                            //
// Begin_Html <!--
/*-->

<!--*/
// --> End_Html
//                                                                            //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//     RawPointerSplit                                                        //
//     RawPointerNoSplit                                                      //
//     RawSplit                                                               //
//     RawNoSplit                                                             //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

/* Generated header file containing necessary includes                        */
#include "generated/TWR2TPrintValueGeneratedIncludes.h"

////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "RawData.h"
#include "generated/TWR2Analyzer.h"
#include "tasks/TWR2TPrintValue.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "TWR2AllFolders.h"


ClassImp(TWR2TPrintValue)

void TWR2TPrintValue::Init()
{
}

void TWR2TPrintValue::BeginOfRun()
{
}

void TWR2TPrintValue::Event()
{
   Int_t i;
   cout<<"___ event "<<gAnalyzer->GetCurrentEventNumber()<<" ___"<<endl;
   for (i = 0; i < gAnalyzer->GetRawPointerSplitSize(); i++) {
      cout<<gAnalyzer->GetRawPointerSplitAt(i)->GetData()->GetData()<<endl;
      if (strcmp(gAnalyzer->GetRawPointerSplitAt(i)->GetData()->GetData(),
                 gAnalyzer->GetRawPointerNoSplitAt(i)->GetData()->GetData())) {
         cerr<<"error by splitting a branch"<<endl;
      }
   }
   cout<<endl;
   for (i = 0; i < gAnalyzer->GetRawSplitSize(); i++) {
      cout<<gAnalyzer->GetRawSplitAt(i)->GetData()->GetData()<<endl;
      if (strcmp(gAnalyzer->GetRawSplitAt(i)->GetData()->GetData(),
                 gAnalyzer->GetRawNoSplitAt(i)->GetData()->GetData())) {
         cerr<<"error by splitting a branch"<<endl;
      }
   }
   cout<<endl;
}

void TWR2TPrintValue::EndOfRun()
{
}

void TWR2TPrintValue::Terminate()
{
}

