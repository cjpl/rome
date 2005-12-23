// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FOTTab                                                                     //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ArgusBuilder. Manual changes above the       *
 * following line will be lost next time ArgusBuilder is executed.            */
/////////////////////////////////////----///////////////////////////////////////

#include "include/tabs/FOTTab.h"

ClassImp(FOTTab)

void FOTTab::Init()
{
   //ODB
   gMonitor->ReadODB();
   cout << gMonitor->GetODB()->GetRunNumber() << endl;

   //ROMEDataBase
   gMonitor->ReadROMEDataBase();
   cout << gMonitor->GetROMEDataBase()->Getcomment() << endl;

   //ROMEFolder
   if (gMonitor->GetNetFolder("mynetfolder")) {
      fRunInfo = (FORunInfo *) ((TFolder *) gMonitor->GetNetFolder("mynetfolder")->FindObjectAny("RunInfo"))->FindObjectAny("LPRunInfo");
      cout << fRunInfo->GetRunMode() << endl;
   }

   if (gMonitor->GetNetFolder("mynetfolder")) {
      fPMTDatas = (TClonesArray *) ((TFolder *) gMonitor->GetNetFolder("mynetfolder")->FindObjectAny("PMTData"))->FindObjectAny("LPPMTData");
      for (int i = 0; i < fPMTDatas->GetEntries(); i++) {
         cout << i << " :" << ((FOPMTData *) (fPMTDatas->At(i)))->GetADC() << endl;
      }
   }
}
