//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MEGCMPMTInfo                                                               //
//                                                                            //
// Container Class for Calorimeter Photo Multiplier Information.                
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////

#include "MEG.h"
#include "MEGCMPMTInfo.h"

ClassImp(MEGCMPMTInfo)

Int_t MEGCMPMTInfo::GetFace()
{
   if (fAddress[0]=='T'&&fAddress[1]!='C') {
      return TOP;
   }
   if (fAddress[0]=='B'&&fAddress[1]=='T') {
      return BOTTOM;
   }
   if (fAddress[0]=='F') {
      return FRONT;
   }
   if (fAddress[0]=='B'&&fAddress[1]=='K') {
      return BACK;
   }
   if (fAddress[0]=='R') {
      return RIGHT;
   }
   if (fAddress[0]=='L'&&fAddress[1]!='S') {
      return LEFT;
   }
   return NOFACE;
}

