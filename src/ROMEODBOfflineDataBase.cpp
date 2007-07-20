// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//
//  ROMEODBOfflineDataBase
//
//  Access to the Midas ODB parameters in the header of a midas file.
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include "ROMEiostream.h"
#include "ROMEODBOfflineDataBase.h"
#include "ROMEString.h"
#include "ROMEXML.h"

ClassImp(ROMEODBOfflineDataBase)

//______________________________________________________________________________
ROMEODBOfflineDataBase::ROMEODBOfflineDataBase()
:ROMEDataBase()
,fXML(new ROMEXML())
{ 
}

//______________________________________________________________________________
ROMEODBOfflineDataBase::~ROMEODBOfflineDataBase() {
   SafeDelete(fXML);
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::Init(const char* name, const char* /*path*/, const char* /*connection*/) {
   fName = name;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::SetBuffer(char *buffer) {
   return fXML->OpenBufferForPath(buffer);
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::Read(ROMEStr2DArray *values, const char *dataBasePath, Long64_t /*runNumber*/,
                                    Long64_t /*eventNumber*/)
{
   ROMEString value;
   ROMEString path = dataBasePath;
   ROMEString tmp;
   ROMEString odbPath;
   if (!fXML->isPathOpen()) {
      return true;
   }
   path += "/";
   odbPath = "odb/";
   int ind = path.First('/');
   while (ind!=-1) {
      tmp = path(0, ind);
      if (ind + 1< path.Length()) {
         path = path(ind+1, path.Length());
      } else {
         path = "";
      }
      ind = path.First('/');
      if (ind == -1) {
         if (tmp.Length() > 0) {
            odbPath.AppendFormatted("key[@name=\"%s\"]/", tmp.Data());
         }
      } else {
         if (tmp.Length() > 0) {
            odbPath.AppendFormatted("dir[@name=\"%s\"]/", tmp.Data());
         }
      }
   }
   odbPath = odbPath(0, odbPath.Length() - 1);
   fXML->GetPathValue(odbPath.Data(), value);
   values->SetAt(value.Data(), 0, 0);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::Write(ROMEStr2DArray* /*values*/, const char* /*dataBasePath*/, Long64_t /*runNumber*/,
                                     Long64_t /*eventNumber*/)
{
   return true;
}
