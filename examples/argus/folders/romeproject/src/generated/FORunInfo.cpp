// Author: Ryu Sawada

/******************************************************************************
 *         ***  This file will be overwritten by the ROMEBuilder  ***         *
 *          ***      Don't make manual changes to this file      ***          *
 ******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// FORunInfo                                                                  //
//                                                                            //
// Run information.                                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "generated/FORunInfo.h"
ClassImp(FORunInfo)
FORunInfo::FORunInfo( Short_t RunMode_value,TString Comment_value,Int_t StartTime_value,Int_t StopTime_value )
{
   FORunInfo::Class()->IgnoreTObjectStreamer();
   RunMode = RunMode_value;
   Comment = Comment_value;
   StartTime = StartTime_value;
   StopTime = StopTime_value;
   fModified = false;
}

FORunInfo::~FORunInfo()
{
}


Bool_t FORunInfo::isModified()
{
   if ( fModified ) return true;
   return false;
}



void FORunInfo::ResetModified()
{
   if (isModified()) {
      SetModified(false);
   }
}

void FORunInfo::SetAll( Short_t RunMode_value,TString Comment_value,Int_t StartTime_value,Int_t StopTime_value )
{
   RunMode = RunMode_value;
   Comment = Comment_value;
   StartTime = StartTime_value;
   StopTime = StopTime_value;
   SetModified(true);
}

void FORunInfo::Reset()
{
   if ( !isModified() ) return;
      RunMode = (Short_t)0;
      Comment = (TString)"none";
      StartTime = (Int_t)0;
      StopTime = (Int_t)0;
   fModified = false;
}


