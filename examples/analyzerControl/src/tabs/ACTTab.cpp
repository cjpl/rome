//// Author: Ryu Sawada
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ACTTab                                                                     //
//                                                                            //
// 
// 
// This header has been generated by the ArgusBuilder.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/ACTTab.h"
#include "include/framework/ACWindow.h"

ClassImp(ACTTab)

void ACTTab::Init()
{
   fController = new ArgusAnalyzerController(gClient->GetRoot(), gWindow, 100, 100, fNetFolder);
}

void ACTTab::MenuClicked(TGPopupMenu *menu, Long_t param)
{
}

void ACTTab::TabSelected()
{
}
void ACTTab::TabUnSelected()
{
}
