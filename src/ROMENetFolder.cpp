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

//______________________________________________________________________________
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

//______________________________________________________________________________
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

//______________________________________________________________________________
Bool_t ROMENetFolder::RegisterObject(const char* name)
{
   ROMEString str = "RegisterObject ";
   str.Append(name);
   if (!Send(str.Data()))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMENetFolder::UnRegisterObject(const char* name)
{
   ROMEString str = "UnRegisterObject ";
   str.Append(name);
   if (!Send(str.Data()))
      return false;
   return true;
}

//______________________________________________________________________________
Bool_t ROMENetFolder::RequestNewEvent(Long64_t oldRunNumber,Long64_t oldEventNumber)
{
   ROMEString str = "RequestNewEvent ";
   str.SetFormatted("RequestNewEvent "R_LLD" "R_LLD"",oldRunNumber,oldEventNumber);
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

//______________________________________________________________________________
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

//______________________________________________________________________________
Bool_t ROMENetFolder::IsProgramTerminated()
{
   ROMEString str = "IsProgramTerminated";
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

Bool_t ROMENetFolder::IsTaskActive(Int_t i)
{
   ROMEString str = "IsTaskActive ";
   str.SetFormatted("IsTaskActive %d",i);
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

Bool_t ROMENetFolder::IsHistoActive(Int_t iTask,Int_t iHisto)
{
   ROMEString str = "IsHistoActive ";
   str.SetFormatted("IsHistoActive %d %d",iTask,iHisto);
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

Bool_t ROMENetFolder::IsGraphActive(Int_t iTask,Int_t iGraph)
{
   ROMEString str = "IsGraphActive ";
   str.SetFormatted("IsGraphActive %d %d",iTask,iGraph);
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
