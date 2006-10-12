// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SBTTab1                                                                    //
//                                                                            //
// Description:                                                               //
//                                                                            //
//                                                                            //
// Usage:                                                                     //
//                                                                            //
//                                                                            //
// Status:                                                                    //
//                                                                            //
//                                                                            //
// To Do:                                                                     //
//                                                                            //
//                                                                            //
// Known Problems:                                                            //
//                                                                            //
//                                                                            //
#include "generated/SBWindow.h"
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////
#include "generated/SBAnalyzer.h"
#include "include/generated/SBWindow.h"
#include "include/tabs/SBTTab1.h"
#include <TTimeStamp.h>

// uncomment if you want to include headers of all folders
//#include "SBAllFolders.h"


ClassImp(SBTTab1)

void SBTTab1::Init()
{
   fLine = new TGHorizontal3DLine(this, 500);
   AddFrame(fLine, new TGLayoutHints(kLHintsTop));
}

void SBTTab1::EndInit()
{
}

void SBTTab1::EventHandler()
{
}

void SBTTab1::MenuClicked(TGPopupMenu * /*menu*/,Long_t /*param*/)
{
}

void SBTTab1::TabSelected()
{
   Int_t parts[] = { 15, 15, 23, 47 };
   gAnalyzer->GetWindow()->GetStatusBar()->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   //Fixed text
   gAnalyzer->GetWindow()->GetStatusBar()->SetText("Fixed Text", 0);
   //Program name
   gAnalyzer->GetWindow()->GetStatusBar()->SetText(gAnalyzer->GetProgramName(), 1);
   //Steering parameter
   gAnalyzer->GetWindow()->GetStatusBar()->SetText(GetSP()->Gettext(), 2);
   // Time
   TTimeStamp ts;
   ts.Set();
   gAnalyzer->GetWindow()->GetStatusBar()->SetText(ts.AsString("s"), 3);
}

void SBTTab1::TabUnSelected()
{
   gAnalyzer->GetWindow()->ClearStatusBar();
}

