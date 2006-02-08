// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMENetFolder
//
//  $Id: $
//
//////////////////////////////////////////////////////////////////////////

#include "ROMENetFolder.h"

ClassImp(ROMENetFolder)

Long64_t ROMENetFolder::GetCurrentRunNumber()
{
   if (!Send("GetCurrentRunNumber"))
      return -1;

   TMessage *mr = 0;
   if (!Recv(mr))
      return -1;

   if (mr == NULL) {
      delete mr;
      return -1;
   }

   Long64_t retValue;
   *mr >> retValue;

   delete mr;
   return retValue;
}

Long64_t ROMENetFolder::GetCurrentEventNumber()
{
   if (!Send("GetCurrentEventNumber"))
      return -1;

   TMessage *mr = 0;
   if (!Recv(mr))
      return -1;

   if (mr == NULL) {
      delete mr;
      return -1;
   }

   Long64_t retValue;
   *mr >> retValue;

   delete mr;
   return retValue;
}
