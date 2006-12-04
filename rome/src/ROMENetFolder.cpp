// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMENetFolder
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEString.h"
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

Bool_t ROMENetFolder::RegisterObject(const char* name)
{
   ROMEString str = "RegisterObject ";
   str.Append(name);
   if (!Send(str.Data()))
      return false;
   return true;
}

Bool_t ROMENetFolder::UnRegisterObject(const char* name)
{
   ROMEString str = "UnRegisterObject ";
   str.Append(name);
   if (!Send(str.Data()))
      return false;
   return true;
}

Bool_t ROMENetFolder::RequestNewEvent(Long64_t oldRunNumber,Long64_t oldEventNumber)
{
   ROMEString str = "RequestNewEvent ";
#if defined( R__VISUAL_CPLUSPLUS )
   str.SetFormatted("RequestNewEvent %I64d %I64d",oldRunNumber,oldEventNumber);
#else
   str.SetFormatted("RequestNewEvent %lld %lld",oldRunNumber,oldEventNumber);
#endif
   if (!Send(str.Data()))
      return false;

   TMessage *mr = 0;
   if (!Recv(mr))
      return false;

   if (mr == NULL) {
      delete mr;
      return false;
   }

   Bool_t retValue;
   *mr >> retValue;

   delete mr;
   return retValue;
}

Bool_t ROMENetFolder::RequestEvent()
{
   ROMEString str = "RequestEvent";
   if (!Send(str.Data()))
      return false;

   TMessage *mr = 0;
   if (!Recv(mr))
      return false;

   if (mr == NULL) {
      delete mr;
      return false;
   }

   Bool_t retValue;
   *mr >> retValue;

   delete mr;
   return retValue;
}

