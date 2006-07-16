/********************************************************************

  main.cpp, Ryu Sawada

  $Id$

********************************************************************/
#include <Riostream.h>
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TTimeStamp.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMEString.h"
#include "ROMEXML.h"

//------
// change following values when release
const Int_t romeMajor = 2;
const Int_t romeMinor = 5;
const Int_t isStableVersion = 0; // 0: development version, 1: stable version
//------

/*
const char* const monthName[] = {
   "Jan"
   , "Feb"
   , "Mar"
   , "Apr"
   , "May"
   , "Jun"
   , "Jul"
   , "Aug"
   , "Sep"
   , "Oct"
   , "Nov"
   , "Dec"
};
*/

Int_t GetROMEVersion(Int_t a, Int_t b) { return (a << 8) + b; }

void ParseSVNKeyword(ROMEString& str)
{
   // extract Subversion revision number from string.
   if (!str.BeginsWith("$") || !str.EndsWith(" $")) // This isn't Subversion keyword
      return;

   if (str.BeginsWith("$Rev: "))                      str.Remove(0, 6);
   else if (str.BeginsWith("$Revision: "))            str.Remove(0, 11);
   else if (str.BeginsWith("$LastChangedRevision: ")) str.Remove(0, 22);
   else{
      cerr << "Warning: ROMEBuilder does not support keyword '"<<str<<"'"<<endl;
      return;
   }
   str.Remove(str.Length()-2, 2);
   return;
}

int main()
{
   TTimeStamp timestamp;
   timestamp.Set();

   //
   // Reading XML
   //

   ROMEString xmlFileName = gSystem->ExpandPathName("$(ROMESYS)/.revision.xml");
   ROMEXML *xml = new ROMEXML();

   xml->OpenFileForPath(xmlFileName.Data());
   ROMEString path;

   path = "Entry";
   Int_t nEntry = xml->NumberOfOccurrenceOfPath(path.Data());

   Int_t iEntry;
   ROMEString *user = new ROMEString[nEntry + 1];
   ROMEString *host = new ROMEString[nEntry + 1];
   ROMEString *directory = new ROMEString[nEntry + 1];
   ROMEString *lastcompile = new ROMEString[nEntry + 1];
   ROMEString revisionString;
   Bool_t foundIdenticalEntry = kFALSE;

   user[nEntry] = gSystem->GetUserInfo()->fUser;
   host[nEntry] = gSystem->HostName();
   directory[nEntry] = gSystem->ExpandPathName("$(ROMESYS)");
   lastcompile[nEntry] = timestamp.AsString();

   path.SetFormatted("Revision");
   xml->GetPathValue(path,revisionString);
   for(iEntry = 0; iEntry < nEntry; iEntry++) {
      path.SetFormatted("Entry[%d]/User", iEntry + 1);
      xml->GetPathValue(path,user[iEntry]);
      path.SetFormatted("Entry[%d]/Host", iEntry + 1);
      xml->GetPathValue(path,host[iEntry]);
      path.SetFormatted("Entry[%d]/Directory", iEntry + 1);
      xml->GetPathValue(path,directory[iEntry]);
      path.SetFormatted("Entry[%d]/LastCompile", iEntry + 1);
      xml->GetPathValue(path,lastcompile[iEntry]);
      if (user[iEntry] == user[nEntry] &&
          host[iEntry] == host[nEntry] &&
          directory[iEntry] == directory[nEntry]) {
         lastcompile[iEntry] = lastcompile[nEntry];
         foundIdenticalEntry = kTRUE;
      }
   }
   delete xml;

   //
   // Writing XML
   //

   if (
      // Ryu
      user[nEntry] == "sawada" ||
      user[nEntry] == "ryu" ||
      // Matthias
      user[nEntry] == "schneebeli_m" ||
      user[nEntry] == "egger_j"
      ) {

      ROMEXML::SuppressWritingDate();
      xml = new ROMEXML();
      xml->OpenFileForWrite(xmlFileName);
      xml->SetTranslate(0);
      xml->WriteElement("Revision", revisionString.Data());
      for(iEntry = 0; iEntry < nEntry; iEntry++) {
         xml->StartElement("Entry");
         xml->WriteElement("User", user[iEntry].Data());
         xml->WriteElement("Host", host[iEntry].Data());
         xml->WriteElement("Directory", directory[iEntry].Data());
         xml->WriteElement("LastCompile", lastcompile[iEntry].Data());
         xml->EndElement();
      }
      if (!foundIdenticalEntry) {
         xml->StartElement("Entry");
         xml->WriteElement("User", user[nEntry].Data());
         xml->WriteElement("Host", host[nEntry].Data());
         xml->WriteElement("Directory", directory[nEntry].Data());
         xml->WriteElement("LastCompile", lastcompile[nEntry].Data());
         xml->EndElement();
      }
      xml->EndDocument();
      delete xml;
   }

   ROMEString hfile = gSystem->ExpandPathName("$(ROMESYS)/include/");
   hfile.AppendFormatted("ROMEVersion.h");
   //
   // Reading ROMEVersion.h
   //
   ROMEString fileBuffer;
   ifstream originalFile(hfile.Data());
   if (originalFile.good()) {
      fileBuffer.ReadFile(originalFile);
   }
   originalFile.close();

   //
   // Writing ROMEVersion.h
   //

   ROMEString buffer;
   ParseSVNKeyword(revisionString);

   // current time
   UInt_t year;
   UInt_t month;
   UInt_t day;
   UInt_t hour;
   UInt_t min;
   UInt_t sec;
   timestamp.Set();
   timestamp.GetDate(kTRUE, 0, &year, &month, &day);
   timestamp.GetTime(kTRUE, 0, &hour, &min, &sec);

   // prepare new file.
   buffer.Resize(0);
   buffer.AppendFormatted("#ifndef ROMEVersion\n");
   buffer.AppendFormatted("#define ROMEVersion\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("/* Version information automatically generated by installer. */\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("/*\n");
   buffer.AppendFormatted(" * These macros can be used in the following way:\n");
   buffer.AppendFormatted(" *\n");
   buffer.AppendFormatted(" *    #if ROME_VERSION_CODE >= ROME_VERSION(2,5)\n");
   buffer.AppendFormatted(" *       #include <newheader.h>\n");
   buffer.AppendFormatted(" *    #else\n");
   buffer.AppendFormatted(" *       #include <oldheader.h>\n");
   buffer.AppendFormatted(" *    #endif\n");
   buffer.AppendFormatted(" *\n");
   buffer.AppendFormatted("*/\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#define ROME_RELEASE \"%d.%d\"\n", romeMajor, romeMinor);
   buffer.AppendFormatted("#define ROME_REVISION_CODE %s\n", revisionString.Data());
   buffer.AppendFormatted("#define ROME_STABLE %d\n", isStableVersion);
/*
   buffer.AppendFormatted("#define ROME_RELEASE_DATE \"%s %2d %d\"\n", monthName[month], day, year);
   buffer.AppendFormatted("#define ROME_RELEASE_TIME \"%02d:%02d:%02d\"\n", hour, min, sec);
*/
   buffer.AppendFormatted("#define ROME_VERSION_CODE %d\n", GetROMEVersion(romeMajor, romeMinor));
   buffer.AppendFormatted("#define ROME_VERSION(a,b) (((a) << 8) + (b))\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");

   // write file
   if (fileBuffer != buffer) {
      ofstream versionH(hfile.Data());
      if (!versionH.good()) {
         cerr<<"failed to open "<<hfile<<" for write."<<endl;
         return 1;
      }
      versionH<<buffer.Data();
      versionH.close();
   }

   return 0;
}

