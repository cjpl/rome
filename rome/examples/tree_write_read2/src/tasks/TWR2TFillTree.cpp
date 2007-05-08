////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TWR2TFillTree                                                              //
//                                                                            //
// Begin_Html                                                                 //
//                                                                            //
// Description:                                                               //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Usage:                                                                     //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Status:                                                                    //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// To Do:                                                                     //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// Known Problems:                                                            //
// <p>                                                                        //
//                                                                            //
//                                                                            //
// <p>                                                                        //
// End_Html                                                                   //
//                                                                            //
// The event methods have been written by .                                   //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//                                                                            //
//                                                                            //
// Followings are include files of folders. ROMEBuilder will update it with   //
// reading this source code when it is executed next time.                    //
#include "generated/TWR2Track.h"
#include "generated/TWR2Raw.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include "RawData.h"
#include "generated/TWR2Analyzer.h"
#include "tasks/TWR2TFillTree.h"
#include "ROMEiostream.h"

// uncomment if you want to include headers of all folders
//#include "TWR2AllFolders.h"


ClassImp(TWR2TFillTree)

void TWR2TFillTree::Init()
{
}

void TWR2TFillTree::BeginOfRun()
{
}

void TWR2TFillTree::Event()
{
   int i;
   gAnalyzer->GetTrack()->Setntrack(10);
   gAnalyzer->GetTrack()->SetparticleidSize(10);
   for (i=0;i<10;i++) {
      gAnalyzer->GetTrack()->SetparticleidAt(i,99);
      RawData *rawData = new RawData();
      rawData->SetDataSize(4);
      rawData->SetData("Test");
      gAnalyzer->GetRawAt(i)->SetData(*rawData);
   }

}

void TWR2TFillTree::EndOfRun()
{
}

void TWR2TFillTree::Terminate()
{
}

