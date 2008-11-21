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

#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( push )
#   pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TFile.h>
#include <TObjString.h>
#include <TROOT.h>
#if defined( R__VISUAL_CPLUSPLUS )
#   pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

ClassImp(ROMEODBOfflineDataBase)

//______________________________________________________________________________
ROMEODBOfflineDataBase::ROMEODBOfflineDataBase()
:ROMEDataBase()
,fXML(new ROMEXML())
,fStr(new TObjString())
{
}

//______________________________________________________________________________
ROMEODBOfflineDataBase::~ROMEODBOfflineDataBase()
{
   SafeDelete(fXML);
   SafeDelete(fStr);
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::Init(const char *name, const char * /* path */, const char * /* connection */ )
{
   fName = name;
   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::SetBuffer(const char *buffer) const
{
   *fStr = buffer;
   if (buffer) {
      return fXML->OpenBufferForPath(buffer);
   } else {
      return kFALSE;
   }
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::Read(ROMEStr2DArray * values, const char *dataBasePath, Long64_t /* runNumber */,
                                    Long64_t /* eventNumber */ )
{
   ROMEString value;
   ROMEString path = dataBasePath;
   ROMEString tmp;
   ROMEString odbPath;
   ROMEString odbArrayPath;
   if (!fXML->isPathOpen()) {
      return true;
   }
   path += "/";
   odbPath = "odb/";

   // remove () for array
   Int_t ind = path.First('(');
   Int_t ind2;
   while (ind != -1) {
      ind2 = path.Index(")", 1, ind, TString::kIgnoreCase);
      if (ind2 == -1) {
         // wrong path
         break;
      }
      path.Remove(ind, ind2 - ind + 1);
      ind = path.First('(');
   }

   ind = path.First('/');
   while (ind != -1) {
      tmp = path(0, ind);
      if (ind + 1 < path.Length()) {
         path = path(ind + 1, path.Length());
      } else {
         path = "";
      }
      ind = path.First('/');
      if (ind == -1) {
         if (tmp.Length() > 0) {
            odbArrayPath = odbPath;
            odbPath.AppendFormatted("key[@name=\"%s\"]/", tmp.Data());
            odbArrayPath.AppendFormatted("keyarray[@name=\"%s\"]/value/", tmp.Data());
         }
      } else {
         if (tmp.Length() > 0) {
            odbPath.AppendFormatted("dir[@name=\"%s\"]/", tmp.Data());
         }
      }
   }
   odbPath = odbPath(0, odbPath.Length() - 1);
   odbArrayPath = odbArrayPath(0, odbArrayPath.Length() - 1);

   Int_t nValue;
   if (fXML->NumberOfOccurrenceOfPath(odbPath.Data())) {
      // single key
      fXML->GetPathValue(odbPath.Data(), value);
      values->SetAt(value.Data(), 0, 0);
   } else if ((nValue = fXML->NumberOfOccurrenceOfPath(odbArrayPath.Data()))) {
      // key array
      Int_t iValue;
      ROMEString elemPath;
      for (iValue = 0; iValue < nValue; iValue++) {
         elemPath = odbArrayPath;
         elemPath.AppendFormatted("[%d]", iValue + 1);
         fXML->GetPathValue(elemPath.Data(), value);
         values->SetAt(value.Data(), 0, iValue);
      }
   } else {
      ROMEPrint::Warning("Warning: %s was not found in ODB. Default value will be used.\n",
                         dataBasePath);
      return !fTreatWarningAsError;
   }

   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::Write(ROMEStr2DArray * /* values */, const char * /* dataBasePath */,
                                     Long64_t /* runNumber */,
                                     Long64_t /* eventNumber */ )
{
   return true;
}

//______________________________________________________________________________
Bool_t ROMEODBOfflineDataBase::DumpToXML(const char *filename) const
{
   return fXML->WritePathFile(filename);
}

//______________________________________________________________________________
Int_t ROMEODBOfflineDataBase::WriteToTFile(TFile *file) const
{
   if (!file || !fStr) {
      return 0;
   }
   TDirectory *orgdir = gDirectory;
   file->cd();
   Int_t ret = fStr->Write("odb", TObject::kOverwrite);
   if (orgdir) {
      orgdir->cd();
   } else {
      gROOT->cd();
   }
   return ret;
}
