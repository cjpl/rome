//// Author: Ryu Sawada
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// SBTTab2                                                                    //
//                                                                            //
// 
// 
// This header has been generated by the ArgusBuilder.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/SBTTab2.h"
#include "include/framework/SBWindow.h"

ClassImp(SBTTab2)

void SBTTab2::Init()
{
}

void SBTTab2::TabSelected()
{
   Int_t parts[] = { 15, 15 };
   gWindow->GetStatusBar()->SetParts(parts, sizeof(parts) / sizeof(Int_t));
   gWindow->GetStatusBar()->SetText("Tab2-1", 0);
   gWindow->GetStatusBar()->SetText("Tab2-2", 1);
}

void SBTTab2::TabUnSelected()
{
   gWindow->ClearStatusBar();
}
