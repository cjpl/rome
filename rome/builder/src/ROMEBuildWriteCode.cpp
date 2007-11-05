/********************************************************************

  ROMEBuildWriteCode.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 4244)
#endif // R__VISUAL_CPLUSPLUS
#include <TSystem.h>
#include <TTimeStamp.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <Riostream.h>
#include "TArrayI.h"
#include "ROMEBuilder.h"
#include "ROMEXML.h"
#include "ROMEConfig.h"
#include "ROMEConfigParameter.h"

namespace {
   const char* Relation(ROMEString &str) {
      if (str.ContainsFast("*")) {
         return "->";
      } else {
         return ".";
      }
      return 0;
   }
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteFolderCpp()
{
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString header;
   ROMEString clsName;
   ROMEString clsDescription;
   int i;
   ROMEString str;
   bool changeableFlagChanged;
   ROMEString tempBuffer;
   ROMEString tmp;
   Int_t iDm;

   if (makeOutput) cout<<"\n   Output CPP-Files:"<<endl;
   for (int iFold = 0; iFold < numOfFolder; iFold++) {
      if (!folderUsed[iFold])
         continue;
      changeableFlagChanged = false;
      if (!FolderToBeGenerated(iFold)) continue;

      // back up old files
      if (folderUserCode[iFold]) {
         cppFile.SetFormatted("%ssrc/generated/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(cppFile.Data());
      } else {
         cppFile.SetFormatted("%ssrc/generated/%s%s_Base.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(cppFile.Data());
         cppFile.SetFormatted("%ssrc/folders/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         BackUpFile(cppFile.Data());
      }
      if (changeableFlagChanged) {
         if (folderUserCode[iFold]) {
            cppFile.SetFormatted("src/generated/%s%s.cpp",shortCut.Data(),folderName[iFold].Data());
            RemoveDepFiles(cppFile.Data());
         } else {
            cppFile.SetFormatted("src/generated/%s%s_Base.cpp",shortCut.Data(),folderName[iFold].Data());
            RemoveDepFiles(cppFile.Data());
            cppFile.SetFormatted("src/folders/%s%s.cpp",shortCut.Data(),folderName[iFold].Data());
            RemoveDepFiles(cppFile.Data());
         }
      }

      // File name
      if (folderUserCode[iFold]) {
         cppFile.SetFormatted("%ssrc/generated/%s%s_Base.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      } else {
         cppFile.SetFormatted("%ssrc/generated/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      }

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, numOfFolderAuthors[iFold], folderAuthor[iFold], folderAuthorEmail[iFold], kTRUE);
      if (folderUserCode[iFold]) {
         clsName.SetFormatted("%s%s_Base", shortCut.Data(), folderName[iFold].Data());
      } else {
         clsName.SetFormatted("%s%s", shortCut.Data(), folderName[iFold].Data());
      }
      clsDescription = folderDescription[iFold].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
      buffer.AppendFormatted("\n\n");

      // Includes
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#include \"Windows4Root.h\"\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"ROME.h\"\n");
      buffer.AppendFormatted("#include \"generated/%s.h\"\n",clsName.Data());
      buffer.AppendFormatted("\nClassImp(%s)\n",clsName.Data());
      buffer.AppendFormatted("\n");

      // Constructor
      ROMEString separator = "";
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%s::%s(",clsName.Data(),clsName.Data());
      if (numOfValue[iFold] < maxNumberOfArguments) { // rootcint does not accept more than 40
         for (i = 0; i < numOfValue[iFold]; i++) {
            if (valueDimension[iFold][i] == 0) {
               if (isFolder(valueType[iFold][i].Data())) {
                  continue;
               } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) &&
                          valueType[iFold][i] != "TRef") {
                  continue;
               } else if (valueType[iFold][i] == "TRef") {
                  buffer.AppendFormatted("%sTObject* %s_value",separator.Data(),valueName[iFold][i].Data());
                  separator = ", ";
               } else {
                  buffer.AppendFormatted("%s%s %s_value",separator.Data(),valueType[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  separator = ", ";
               }
            }
         }
      }
      buffer.AppendFormatted(" )\n");

      if (folderInheritName[iFold].Length() > 0) {
         buffer.AppendFormatted(":%s%s()\n",shortCut.Data(),folderInheritName[iFold].Data());
      } else {
         buffer.AppendFormatted(":TObject()\n");
      }
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            tmp = valueType[iFold][i];
            tmp.ReplaceAll("*","");
            if (valueDimension[iFold][i] == 0) {
               if (isPointerType(valueType[iFold][i].Data())) {
                  buffer.AppendFormatted(",%s(0)\n",valueName[iFold][i].Data());
               } else {
                  buffer.AppendFormatted(",%s()\n",valueName[iFold][i].Data());
               }
            } else {
               buffer.AppendFormatted(",%s(new TClonesArray(\"%s\"))\n",valueName[iFold][i].Data(),tmp.Data());
            }
         } else if (valueIsTObject[iFold][i] && !isTArrayType(valueType[iFold][i])) {
            if (valueDimension[iFold][i] == 0) {
               if (isPointerType(valueType[iFold][i].Data()) || valueType[iFold][i] == "TRef") {
                  buffer.AppendFormatted(",%s(%s_value)\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               } else {
                  buffer.AppendFormatted(",%s()\n",valueName[iFold][i].Data());
               }
            } else {
               if (valueArray[iFold][i][0] == "variable") {
                  buffer.AppendFormatted(",%s(0)\n",valueName[iFold][i].Data());
                  buffer.AppendFormatted(",%sSize(0)\n",valueName[iFold][i].Data());
               }
            }
         } else if (isTArrayType(valueType[iFold][i])) {
            if (valueType[iFold][i].ContainsFast("*")) {
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted(",%s(new %s(%s))\n",valueName[iFold][i].Data(),tmp.Data(),
                                      valueArray[iFold][i][0].Data());
            } else {
               buffer.AppendFormatted(",%s()\n",valueName[iFold][i].Data());
            }
         } else {
            if (valueDimension[iFold][i] == 0) {
               if (numOfValue[iFold] < maxNumberOfArguments) { // rootcint does not accept more than 40
                  buffer.AppendFormatted(",%s(%s_value)\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               } else {
                  buffer.AppendFormatted(",%s(%s)\n",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               }
            } else if (valueArray[iFold][i][0] == "variable") {
               buffer.AppendFormatted(",%s(0)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted(",%sSize(0)\n",valueName[iFold][i].Data());
            } else {
            }
         }
      }
      if (folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted(",fModified(kFALSE)\n");
      }

      buffer.AppendFormatted("{\n");
      if (!folderIgnoreTObjectStreamer[iFold] && folderInheritName[iFold].Length() != 0) {
         cout<<"Warning: <IgnoreTObjectStreamer> in "<<folderName[iFold]<<"'s definition does not make sense."<<endl<<
               "         This key is available only in definitions of a class deriving directly from TObject."<<endl;
      }
      if (folderIgnoreTObjectStreamer[iFold] && folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted("   %s::Class()->IgnoreTObjectStreamer();\n",clsName.Data());
      }
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i] == 0) {
            } else {
               buffer.AppendFormatted("   %s->SetName(\"%s%s\");\n",valueName[iFold][i].Data(),
                                      shortCut.Data(), tmp.Data());
               if (valueArray[iFold][i][0] != "variable") {
                  buffer.AppendFormatted("   Set%sSize(%s);\n",valueName[iFold][i].Data(),
                                         valueArray[iFold][i][0].Data());
               }
            }
         } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) &&
                    !isTArrayType(valueType[iFold][i]) && valueType[iFold][i] != "TRef") {
            continue;
         } else if (isTArrayType(valueType[iFold][i])) {
            if (valueType[iFold][i].ContainsFast("*")) {
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("   %s%sReset(%s);\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]),
                                      valueInit[iFold][i].Data());
            }
         } else {
            if (valueDimension[iFold][i] == 0) {
            } else if (valueArray[iFold][i][0] == "variable") {
               if (valueArraySpecifier[iFold][i].Length()) {
                  buffer.AppendFormatted("   if (%s > 0) {\n",valueArraySpecifier[iFold][i].Data());
                  buffer.AppendFormatted("      %s = new %s[%s];\n",valueName[iFold][i].Data(),
                                         valueType[iFold][i].Data(),
                                         valueArraySpecifier[iFold][i].Data());
                  buffer.AppendFormatted("      %sSize = %s;\n",valueName[iFold][i].Data(),
                                         valueArraySpecifier[iFold][i].Data());
                  buffer.AppendFormatted("   }\n");
               }
            } else {
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
                  buffer.AppendFormatted("%*s   for (int %c%d = 0; %c%d < %s; %c%d++) {\n",
                                         iDm * 3, "", valueCounter[iDm], i, valueCounter[iDm], i,
                                         valueArray[iFold][i][iDm].Data(), valueCounter[iDm], i);
               }
               buffer.AppendFormatted("%*s   %s",valueDimension[iFold][i] * 3, "", valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted(" = %s;\n",valueInit[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
                  buffer.AppendFormatted("%*s   }\n", (valueDimension[iFold][i] - iDm - 1) * 3,"");
               }
            }
         }
      }
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Destructor
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%s::~%s()\n",clsName.Data(),clsName.Data());
      buffer.AppendFormatted("{\n");
      for (i = 0 ; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   SafeDelete(%s);\n",valueName[iFold][i].Data());
            }
         } else if (isTArrayType(valueType[iFold][i]) && valueType[iFold][i].ContainsFast("*")) {
            buffer.AppendFormatted("   SafeDelete(%s);\n",valueName[iFold][i].Data());
         } else if (valueDimension[iFold][i] != 0 && valueArray[iFold][i][0] == "variable") {
            buffer.AppendFormatted("   SafeDeleteArray(%s);\n",valueName[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Getters
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (valueDimension[iFold][i] > 0) {
            if (isFolder(valueType[iFold][i].Data())) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %s::Get%sAt(Int_t indx) const\n",valueType[iFold][i].Data(),clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", indx))\n",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  buffer.AppendFormatted("      return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return static_cast<%s*>(%s->At(indx));\n",valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueType[iFold][i] == "TRef") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("TRef* %s::Get%sAt(Int_t indx)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", indx)) return %s;\n",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data(), valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return &%s[indx];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("TObject* %s::Get%sObjAt(Int_t indx) const\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", indx)) return %s;\n",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data(), valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return %s[indx].GetObject();\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueType[iFold][i] == "TRefArray") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("TObject* %s::Get%sAt(Int_t indx) const\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return %s.At(indx);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (isTArrayType(valueType[iFold][i])) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n, %s* arrayToCopy) const\n",clsName.Data(),
                                      valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s%sGetSize() || !n) return;\n",valueName[iFold][i].Data(),
                                      Relation(valueType[iFold][i]));
               buffer.AppendFormatted("   if (!arrayToCopy) arrayToCopy = new %s[n];\n",
                                      TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("   memcpy(arrayToCopy,%s%sGetArray(),n*sizeof(%s));\n",
                                      valueName[iFold][i].Data(),Relation(valueType[iFold][i]),
                                      TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %s::Get%sAt(",valueType[iFold][i].Data(),clsName.Data(),
                                      valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", ",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return &%s",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(";\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t indx) const\n",valueType[iFold][i].Data(),clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", indx)) return %s;\n",
                                         valueName[iFold][i].Data(),valueName[iFold][i].Data(),
                                         valueInit[iFold][i].Data());
               buffer.AppendFormatted("   return %s[indx];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueArray[iFold][i][0] == "variable") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t indx) const\n",valueType[iFold][i].Data(),clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", indx)) return %s;\n",
                                         valueName[iFold][i].Data(),valueName[iFold][i].Data(),
                                         valueInit[iFold][i].Data());
               buffer.AppendFormatted("   return %s[indx];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* array) const\n",clsName.Data(),
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(array,%s,n*sizeof(%s));\n",valueName[iFold][i].Data(),
                                      valueType[iFold][i].Data());
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %s::Get%sAt(",valueType[iFold][i].Data(),clsName.Data(),
                                      valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(") const\n");
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", ",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return %s",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(";\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* array) const\n",clsName.Data(),
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(array,&%s",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[0]");
               buffer.AppendFormatted(",n*sizeof(%s));\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // isModified
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Bool_t %s::isModified()\n",clsName.Data());
      buffer.AppendFormatted("{\n");
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i] > 0) {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   int nentry;\n");
            break;
         }
      }
      if (folderInheritName[iFold].Length() > 0) {
         buffer.AppendFormatted("   if (%s%s::isModified()) return true;\n",shortCut.Data(),
                                folderInheritName[iFold].Data());
      } else {
         buffer.AppendFormatted("   if (fModified) return true;\n");
      }
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i] == 0) {
               if (!isPointerType(valueType[iFold][i].Data())) {
                  buffer.AppendFormatted("   if (%s.isModified()) {\n",valueName[iFold][i].Data());
               } else {
                  buffer.AppendFormatted("   if (%s && %s->isModified()) {\n",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
               }
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("      return true;\n");
               buffer.AppendFormatted("   }\n");
            } else {
               // check only the first element for speed up.
               buffer.AppendFormatted("   if (%s->GetEntriesFast()) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if (static_cast<%s*>(%s->At(0))->isModified()) {\n",valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("         SetModified(true);\n");
               buffer.AppendFormatted("         return true;\n");
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      // check remaining elements
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i] > 0) {
               buffer.AppendFormatted("   nentry = %s->GetEntriesFast();\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   for (i = 1; i < nentry; i++) {\n");
               buffer.AppendFormatted("      if (static_cast<%s*>(%s->At(i))->isModified()) {\n",valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("         SetModified(true);\n");
               buffer.AppendFormatted("         return true;\n");
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      buffer.AppendFormatted("   return false;\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Setters
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (valueDimension[iFold][i] != 0) {
            if (isFolder(valueType[iFold][i].Data())) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sSize(Int_t number)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s || number < 0)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   Int_t n = %s->GetEntriesFast();\n", valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (n == number) {\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   Int_t i;\n");
               buffer.AppendFormatted("   for (i = number; i < n; i++) {\n");
               buffer.AppendFormatted("      %s->RemoveAt(i);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   %s->ExpandCreate(number);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueType[iFold][i] == "TRef") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sAt(Int_t indx, TObject *obj)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", indx))",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  buffer.AppendFormatted(" return;\n");
               }
               buffer.AppendFormatted("   %s[indx] = obj;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueType[iFold][i] == "TRefArray") {
            } else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n, const %s* array)\n",clsName.Data(),
                                      valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || !n) return;\n");
               if (valueType[iFold][i].ContainsFast("*")) {
                  buffer.AppendFormatted("   if (!%s || %s%sGetSize() < n) Set%sSize(n);\n",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data(),Relation(valueType[iFold][i]),valueName[iFold][i].Data());
               } else {
                  buffer.AppendFormatted("   if (%s%sGetSize() < n) Set%sSize(n);\n",valueName[iFold][i].Data(),
                                         Relation(valueType[iFold][i]),valueName[iFold][i].Data());
               }
               buffer.AppendFormatted("   %s%sSet(n,array);\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]));
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())) {
               continue;
            } else if (valueArray[iFold][i][0] == "variable") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sAt(Int_t indx,%s %s_value)\n{\n",clsName.Data(),
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", indx)) return;\n",
                                         valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s[indx] = %s_value;\n",valueName[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");

               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sSize(Int_t number, Bool_t copyOldData, Bool_t fillZero)\n",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (number < 0) return;\n");
               buffer.AppendFormatted("   if (number == %sSize) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if (fillZero && !copyOldData && number != 0)\n");
               buffer.AppendFormatted("         memset(%s, 0, number * sizeof(%s));\n",
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   };\n");
               buffer.AppendFormatted("   %s *tmp = %s;\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (number != 0) {\n");
               buffer.AppendFormatted("      %s = new %s[number];\n",valueName[iFold][i].Data(),
                                      valueType[iFold][i].Data());
               buffer.AppendFormatted("      if (number < %sSize) {\n", valueName[iFold][i].Data());
               buffer.AppendFormatted("         if (copyOldData)\n");
               buffer.AppendFormatted("            memcpy(%s, tmp, number * sizeof(%s));\n",
                                      valueName[iFold][i].Data(), valueType[iFold][i].Data());
               buffer.AppendFormatted("         else if (fillZero)\n");
               buffer.AppendFormatted("            memset(%s, 0, number * sizeof(%s));\n",
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      } else {\n");
               buffer.AppendFormatted("         if (copyOldData) {\n");
               buffer.AppendFormatted("            memcpy(%s, tmp, %sSize * sizeof(%s));\n",
                                      valueName[iFold][i].Data(),valueName[iFold][i].Data(),
                                      valueType[iFold][i].Data());
               buffer.AppendFormatted("            if (fillZero)\n");
               buffer.AppendFormatted("               memset(%s + %sSize, 0, (number - %sSize) * sizeof(%s));\n",
                                      valueName[iFold][i].Data(),valueName[iFold][i].Data(),
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("         } else if (fillZero) {\n");
               buffer.AppendFormatted("            memset(%s, 0, number * sizeof(%s));\n",
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("         }\n");
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("   } else {\n");
               buffer.AppendFormatted("      %s = 0;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   delete [] tmp;\n");
               buffer.AppendFormatted("   %sSize = number;\n",valueName[iFold][i].Data());
#if 0 // Maybe, this should be taken care of by users rather than framework.
               if (valueArraySpecifier[iFold][i].Length())
                  buffer.AppendFormatted("   %s = number;\n",valueArraySpecifier[iFold][i].Data());
#endif
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n, const %s* array)\n",clsName.Data(),
                                      valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || n <= 0) return;\n");
               buffer.AppendFormatted("   if (%sSize < n) Set%sSize(n, kFALSE);\n",valueName[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),
                                      valueType[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%s %s_value)\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", ",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return;\n");
               }
               buffer.AppendFormatted("   %s",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted("= %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n, const %s* array)\n",clsName.Data(),
                                      valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || !n) return;\n");
               buffer.AppendFormatted("   memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),
                                      valueType[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }
      }
      buffer.AppendFormatted("\n");
      // Add
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data()) || isPointerType(valueType[iFold][i].Data()) ||
             (valueIsTObject[iFold][i] &&! isTArrayType(valueType[iFold][i])) || isBoolType(valueType[iFold][i].Data())) {
            continue;
         }
         if (valueDimension[iFold][i] != 0) {
            if (isTArrayType(valueType[iFold][i])) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Add%sAt(Int_t indx,%s %s_value)\n",clsName.Data(),
                                      valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   %s tmp = %s%sAt(indx);\n",
                                      TArray2StandardType(valueType[iFold][i],tempBuffer),
                                      valueName[iFold][i].Data(),Relation(valueType[iFold][i]));
               buffer.AppendFormatted("   tmp += %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s%sAddAt(tmp,indx);\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]));
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("void %s::Add%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%s %s_value)\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Add%sAt\", ",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
                  for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return;\n");
               }
               buffer.AppendFormatted("   %s",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(" += %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // ResetModified
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %s::ResetModified()\n",clsName.Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("   if (isModified()) {\n");
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i] > 0) {
            buffer.AppendFormatted("      int nentry;\n");
            break;
         }
      }
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (!isFolder(valueType[iFold][i].Data()))
            continue;
         if (valueDimension[iFold][i] == 0) {
            if (!isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("      %s.ResetModified();\n",valueName[iFold][i].Data());
            } else {
               buffer.AppendFormatted("      if(%s) { %s->ResetModified(); }\n",valueName[iFold][i].Data(),
                                      valueName[iFold][i].Data());
            }
         } else {
            buffer.AppendFormatted("      nentry = %s->GetEntriesFast();\n",valueName[iFold][i].Data());
            buffer.AppendFormatted("      for (Int_t i%d = 0; i%d < nentry; i%d++) { static_cast<%s*>(%s->At(i%d))->ResetModified(); }\n",
                                   i,i,i,valueType[iFold][i].Data(),valueName[iFold][i].Data(),i);
         }
      }
      if (folderInheritName[iFold].Length() > 0) {
         buffer.AppendFormatted("      %s%s::ResetModified();\n",shortCut.Data(),
                                folderInheritName[iFold].Data());
      } else {
         buffer.AppendFormatted("      SetModified(false);\n");
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Set All
      if (folderInheritName[iFold].Length() == 0) { // only base class can have SetAll
         if (numOfValue[iFold] < maxNumberOfArguments) { // rootcint does not accept more than 40
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("void %s::SetAll( ",clsName.Data());
            for (i = 0; i < numOfValue[iFold]; i++) {
               if (isFolder(valueType[iFold][i].Data()))
                  continue;
               if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) && valueType[iFold][i] != "TRef") {
                  continue;
               }
               if (valueDimension[iFold][i] == 0) {
                  if (valueType[iFold][i] == "TRef") {
                     buffer.AppendFormatted("TObject* %s_value,",valueName[iFold][i].Data());
                  } else {
                     buffer.AppendFormatted("%s %s_value,",valueType[iFold][i].Data(),valueName[iFold][i].Data());
                  }
               }
            }
            buffer.Resize(buffer.Length() - 1);
            buffer.AppendFormatted(" )\n");
            buffer.AppendFormatted("{\n");
            for (i = 0; i < numOfValue[iFold]; i++) {
               if (isFolder(valueType[iFold][i].Data()))
                  continue;
               if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) && valueType[iFold][i] != "TRef") {
                  continue;
               }
               if (valueDimension[iFold][i] == 0) {
                  buffer.AppendFormatted("   %s = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               }
            }
            buffer.AppendFormatted("   SetModified(true);\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
         }
      }
      // Reset
      buffer.AppendFormatted("void %s::Reset()\n",clsName.Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("   if (!isModified()) return;\n");
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i] > 0) {
            buffer.AppendFormatted("   int nentry;\n");
            break;
         }
      }
      if (folderInheritName[iFold].Length() > 0) {
         buffer.AppendFormatted("   %s%s::Reset();\n",shortCut.Data(),
                                folderInheritName[iFold].Data());
      } else {
         buffer.AppendFormatted("   fModified = false;\n");
      }
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i] == 0) {
               if (!isPointerType(valueType[iFold][i].Data())) {
                  buffer.AppendFormatted("   %s.Reset();\n",valueName[iFold][i].Data());
               } else {
                  buffer.AppendFormatted("   if (%s) { %s->Reset(); }\n",valueName[iFold][i].Data(),
                                         valueName[iFold][i].Data());
               }
            } else {
               buffer.AppendFormatted("   nentry = %s->GetEntriesFast();\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   for (int i%d = 0; i%d < nentry; i%d++) { static_cast<%s*>(%s->At(i%d))->Reset(); }\n",
                                      i,i,i,valueType[iFold][i].Data(),valueName[iFold][i].Data(),i);
            }
         } else if (isTArrayType(valueType[iFold][i])) {
            buffer.AppendFormatted("   %s%sReset(%s);\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]),
                                   valueInit[iFold][i].Data());
         } else if (valueType[iFold][i] == "TRefArray") {
            buffer.AppendFormatted("   %s%sClear();\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]));
         } else {
            if (valueArray[iFold][i][0] == "variable") {
               buffer.AppendFormatted("   for (int i%d = 0; i%d < %sSize; i%d++) {\n",i,i, valueName[iFold][i].Data(),i);
            } else {
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
                  buffer.AppendFormatted("%*s   for (int %c%d = 0; %c%d < %s; %c%d++) {\n",
                                         iDm * 3, "", valueCounter[iDm], i, valueCounter[iDm], i,
                                         valueArray[iFold][i][iDm].Data(), valueCounter[iDm], i);
               }
            }
            if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) && valueType[iFold][i] != "TRef") {
               buffer.AppendFormatted("%*s   %s", valueDimension[iFold][i] * 3, "", valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted(".Clear();\n");
            } else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data()) && valueType[iFold][i] != "TRef") {
               buffer.AppendFormatted("%*s   if (%s", valueDimension[iFold][i] * 3, "", valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("%*s      %s", valueDimension[iFold][i] * 3, "", valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted("->Clear();\n");
            } else {
               buffer.AppendFormatted("%*s   %s", valueDimension[iFold][i] * 3, "", valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted(" = static_cast<%s>(%s);\n",valueType[iFold][i].Data(),valueInit[iFold][i].Data());
            }
            if (valueArray[iFold][i][0] == "variable") {
               buffer.AppendFormatted("   }\n");
            } else {
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
                  buffer.AppendFormatted("%*s   }\n", (valueDimension[iFold][i] - iDm - 1) * 3, "");
               }
            }
         }
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // BoundsOk
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (valueDimension[iFold][i] > 0) {
            if (isFolder(valueType[iFold][i].Data())) {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at) const\n",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (at < 0 || at >= Get%sSize())\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i) const",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index %%d out of bounds (size: %%d, this: 0x%%08x)\", i, Get%sSize(), this);\n",
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else if (valueArray[iFold][i][0] == "variable") {
#if 0 // disabled because no way to know array size when the folder is read from TFile.
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at) const\n",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (at < 0 || at >= Get%sSize())\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i) const\n",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index %%d out of bounds (size: %%d, this: 0x%%08x)\", i, Get%sSize(), this);\n",
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
#else
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at) const\n",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (%s == 0)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t /*i*/) const\n",clsName.Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where,\"%s is not allocated. Please allocate with %s::Set%sSize(Int_t number)\");\n",
                                      valueName[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
#endif
            } else if (isTArrayType(valueType[iFold][i]) || valueType[iFold][i] == "TRefArray") {
               // TArray itself checks bounary.
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, ",clsName.Data(),
                                      valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(") const\n");
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (");
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
                  buffer.AppendFormatted(" %c < 0 ||",valueCounter[iDm]);
                  buffer.AppendFormatted(" %c >= %s ||",valueCounter[iDm],valueArray[iFold][i][iDm].Data());
               }
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where,",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted(" %c,",valueCounter[iDm]);
               buffer.Resize(buffer.Length() - 1);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where,",clsName.Data(),
                                      valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted(" Int_t %c,",valueCounter[iDm]);
               buffer.Resize(buffer.Length() - 1);
               buffer.AppendFormatted(") const\n");
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index ");
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("%%d:");
               buffer.Resize(buffer.Length() - 1);
               buffer.AppendFormatted(" out of bounds (size:");
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("%%d:");
               buffer.Resize(buffer.Length() - 1);
               buffer.AppendFormatted(" , this: 0x%%08x)\",");
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted(" %c,",valueCounter[iDm]);
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted(" %s,",valueArray[iFold][i][iDm].Data());
               buffer.Resize(buffer.Length() - 1);
               buffer.AppendFormatted(", this);\n");
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // Write File
      WriteFile(cppFile.Data(),buffer.Data(),6);

      // User CPP-File
      cppFile.SetFormatted("%ssrc/folders/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && (changeableFlagChanged || gSystem->AccessPathName(cppFile.Data(),kFileExists))) {
         buffer.Resize(0);
         // Description
         WriteHeader(buffer, numOfFolderAuthors[iFold], folderAuthor[iFold], folderAuthorEmail[iFold], kFALSE);
         clsName.SetFormatted("%s%s", shortCut.Data(), folderName[iFold].Data());
         clsDescription = folderDescription[iFold].Data();
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kTRUE);

         buffer.AppendFormatted("\n\n#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("\nClassImp(%s%s)\n",shortCut.Data(),folderName[iFold].Data());

         // Write File
         WriteFile(cppFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteFolderH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString header;
   ROMEString clsName;
   ROMEString clsDescription;
   int i,j;
   ROMEString str1, str2;
   bool changeableFlagChanged;
   ROMEString tempBuffer;
   ROMEString tmp;

   if (makeOutput) cout<<"\n   Output H-Files:"<<endl;
   for (int iFold = 0; iFold < numOfFolder; iFold++) {
      if (!folderUsed[iFold])
         continue;
      changeableFlagChanged = false;
      if (!FolderToBeGenerated(iFold)) continue;

      // back up old files
      if (folderUserCode[iFold]) {
         hFile.SetFormatted("%sinclude/generated/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(hFile.Data());
      } else {
         hFile.SetFormatted("%sinclude/generated/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(hFile.Data());
         hFile.SetFormatted("%sinclude/folders/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         BackUpFile(hFile.Data());
      }
      if (changeableFlagChanged) {
         if (folderUserCode[iFold]) {
            hFile.SetFormatted("include/generated/%s%s.h",shortCut.Data(),folderName[iFold].Data());
            RemoveDepFiles(hFile.Data());
         } else {
            hFile.SetFormatted("include/generated/%s%s_Base.h",shortCut.Data(),folderName[iFold].Data());
            RemoveDepFiles(hFile.Data());
            hFile.SetFormatted("include/folders/%s%s.h",shortCut.Data(),folderName[iFold].Data());
            RemoveDepFiles(hFile.Data());
         }
      }

      // File name
      if (folderUserCode[iFold]) {
         hFile.SetFormatted("%sinclude/generated/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      } else {
         hFile.SetFormatted("%sinclude/generated/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      }

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, numOfFolderAuthors[iFold], folderAuthor[iFold], folderAuthorEmail[iFold], kTRUE);
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("#ifndef %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_Base_H\n\n",shortCut.Data(),folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      clsName.SetFormatted("%s%s", shortCut.Data(), folderName[iFold].Data());
      clsDescription = folderDescription[iFold].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
      buffer.AppendFormatted("\n\n");

      // Includes
      buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
      for (i = 0; i < numOfFolderInclude[iFold]; i++) {
         if (folderInclude[iFold][i]=="TRef.h") {
            buffer.AppendFormatted("#include \"Windows4Root.h\"\n");
            break;
         }
      }
#endif // R__VISUAL_CPLUSPLUS

#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4244 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <TObject.h>\n");
      buffer.AppendFormatted("#include <TClass.h>\n");
      buffer.AppendFormatted("#include <TClonesArray.h>\n");
      for (i = 0; i < numOfFolderInclude[iFold]; i++) {
         if (folderLocalFlag[iFold][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",folderInclude[iFold][i].Data());
         } else {
            buffer.AppendFormatted("#include <%s>\n",folderInclude[iFold][i].Data());
         }
      }
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <ROMEString.h>\n");
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isTArrayType(valueType[iFold][i])) {
            str1 = valueType[iFold][i](0, 7);
            buffer.AppendFormatted("#include <%s.h>\n",str1.Data());
            break;
         }
      }

      for (i = 0; i < numOfValue[iFold]; i++) {
         for (j = 0; j < numOfFolder; j++) {
            if (!folderUsed[j]) {
               continue;
            }
            str1.SetFormatted("%s*",folderName[j].Data());
            if (valueType[iFold][i] == folderName[j] || valueType[iFold][i] == str1) {
               valueType[iFold][i].Insert(0, shortCut);
            }

            str1.SetFormatted("%s%s",shortCut.Data(),folderName[j].Data());
            str2.SetFormatted("%s%s*",shortCut.Data(),folderName[j].Data());
            if (valueType[iFold][i] == str1 || valueType[iFold][i] == str2) {
               if (valueDimension[iFold][i] > 0 && isPointerType(valueType[iFold][i].Data())) {
                  // Array of folder pointer is not supported yet.
                  valueType[iFold][i].ReplaceAll("*", "");
               }
               if (folderUserCode[j]) {
                  buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               } else {
                  buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               }
               break;
            }
         }
      }
      if (folderInheritName[iFold].Length() > 0) {
         // this line is limiting inheriting folder to be non-changeable class or, _Base class of changeable class.
         // if some-one want to use changeable class as base class. we must modify here.
         buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderInheritName[iFold].Data());
      }

      // Class
      if (folderInheritName[iFold].Length()>0) {
         buffer.AppendFormatted("\nclass %s%s : public %s%s\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),
                                folderInheritName[iFold].Data());
      } else {
         if (folderUserCode[iFold]) {
            buffer.AppendFormatted("\nclass %s%s_Base : public TObject\n",shortCut.Data(),folderName[iFold].Data());
         } else {
            buffer.AppendFormatted("\nclass %s%s : public TObject\n",shortCut.Data(),folderName[iFold].Data());
         }
      }
      buffer.AppendFormatted("{\n");

      // Fields
      buffer.AppendFormatted("protected:\n");
      int typeLen = 13;
      int nameLen = 8;
      int nameLenT = 0;
      int arrayLen = 0;
      int iDm;
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (typeLen < valueType[iFold][i].Length()) {
            typeLen = valueType[iFold][i].Length();
         }
         nameLenT = static_cast<int>(valueName[iFold][i].Length());
         for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
            nameLenT += static_cast<int>(2 + valueArray[iFold][i][iDm].Length());
         }
         if (nameLen < nameLenT) nameLen = nameLenT;
      }
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i] > 0) {
            if (valueDimension[iFold][i] > 1) {
               cout<<"You can not have multiple dimensional array of support folders."<<endl;
               cout<<"Terminating program."<<endl;
               return false;
            }
            buffer.AppendFormatted("   TClonesArray*%*s %s;%*s %s\n", typeLen - 13, "",
                                   valueName[iFold][i].Data() , nameLen - valueName[iFold][i].Length(), "",
                                   ProcessCommentCPP(valueComment[iFold][i],tmp).Data());
         } else if (valueArray[iFold][i][0] == "variable") {
            buffer.AppendFormatted("   %-*s* %s;%*s %s\n", typeLen - 1, valueType[iFold][i].Data(),
                                   valueName[iFold][i].Data(), nameLen - valueName[iFold][i].Length(), "",
                                   ProcessCommentCPP(valueComment[iFold][i],tmp).Data());
            buffer.AppendFormatted("   %-*s %sSize;%*s // ! number of elements of %s\n",
                                   typeLen, "Int_t", valueName[iFold][i].Data(),
                                   nameLen - valueName[iFold][i].Length() - 4, "",
                                   valueName[iFold][i].Data());
         } else {
            buffer.AppendFormatted("   %-*s %s", typeLen, valueType[iFold][i].Data(), valueName[iFold][i].Data());
            arrayLen = 0;
            if (!isTArrayType(valueType[iFold][i]) && valueType[iFold][i] != "TRefArray") {
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++) {
                  buffer.AppendFormatted("[%s]",valueArray[iFold][i][iDm].Data());
                  arrayLen += 2+valueArray[iFold][i][iDm].Length();
               }
            }
            buffer.AppendFormatted(";%*s %s\n", nameLen - valueName[iFold][i].Length() - arrayLen, "",
                                   ProcessCommentCPP(valueComment[iFold][i],tmp).Data());
         }
      }
      if (folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted("   %-*s f%s;%*s %s\n", typeLen,
                                "Bool_t", "Modified", nameLen - 9, "", "//! Modified Folder Flag");
      }
      buffer.AppendFormatted("\n");

      // Methods

      // Constructor
      buffer.AppendFormatted("private:\n");
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("   %s%s_Base(const %s%s_Base &c); // not implemented\n",
                                shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("   %s%s_Base &operator=(const %s%s_Base &c); // not implemented\n",
                                shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("   %s%s(const %s%s &c); // not implemented\n",
                                shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("   %s%s &operator=(const %s%s &c); // not implemented\n",
                                shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
      }
      buffer.AppendFormatted("\n");
      ROMEString separator = "";
      buffer.AppendFormatted("public:\n");
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("   %s%s_Base(",shortCut.Data(),folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("   %s%s(",shortCut.Data(),folderName[iFold].Data());
      }
      if (numOfValue[iFold] < maxNumberOfArguments) { // rootcint does not accept more than 40
         for (i = 0; i < numOfValue[iFold]; i++) {
            if (valueDimension[iFold][i] == 0) {
               if (isFolder(valueType[iFold][i].Data()))
                  continue;
               if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) && valueType[iFold][i] != "TRef")
                  continue;
               if (valueType[iFold][i] == "TRef") {
                  buffer.AppendFormatted("%sTObject* %s_value=%s",separator.Data(),valueName[iFold][i].Data(),
                                         valueInit[iFold][i].Data());
                  separator = ", ";
               } else {
                  buffer.AppendFormatted("%s%s %s_value=%s",separator.Data(),valueType[iFold][i].Data(),
                                         valueName[iFold][i].Data(),valueInit[iFold][i].Data());
                  separator = ", ";
               }
            }
         }
      }
      buffer.AppendFormatted(" );\n");
      buffer.AppendFormatted("\n");

      // Destructor
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("   virtual ~%s%s_Base();\n",shortCut.Data(),folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("   virtual ~%s%s();\n",shortCut.Data(),folderName[iFold].Data());
      }
      buffer.AppendFormatted("\n");

      // Getters
      for (i = 0; i < numOfValue[iFold]; i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i] > 0) {
            if (valueType[iFold][i] == "TRef") {
               buffer.AppendFormatted("   %-*s  Get%sAt(Int_t indx);\n", typeLen, "TRef*", valueName[iFold][i].Data());
               buffer.AppendFormatted("   %-*s  Get%sObjAt(Int_t indx) const;\n", typeLen, "TObject*", valueName[iFold][i].Data());
               if (valueArray[iFold][i][0] == "variable") {
                  buffer.AppendFormatted("   %-*s  Get%sSize() const%*s { return %sSize;%*s }\n", typeLen,
                                         "Int_t", valueName[iFold][i].Data(), lb, "",
                                         valueName[iFold][i].Data(), lb, "");
               }
            } else if (valueType[iFold][i] == "TRefArray") {
               buffer.AppendFormatted("   %-*s* Get%sAt(Int_t indx) const;\n", typeLen, "TObject", valueName[iFold][i].Data());
               buffer.AppendFormatted("   %-*s  Get%s() %*s { return &%s;%*s }\n",
                                      typeLen, "TRefArray*", valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(),lb, "");
               buffer.AppendFormatted("   %-*s  Get%sSize() const%*s { return %s.GetEntriesFast();%*s }\n", typeLen,
                                      "Int_t", valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(), lb, "");
            } else if (isFolder(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   %-*s* Get%sAt(Int_t indx) const;\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   %-*s  Get%s() const%*s { return %s;%*s }\n",typeLen, "TClonesArray*",
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(), lb, "");
               buffer.AppendFormatted("   %-*s  Get%sSize() const%*s { return %s->GetEntriesFast();%*s }\n",
                                      typeLen, "Int_t", valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(),
                                      lb, "");
            } else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               buffer.AppendFormatted("   %-*s  Get%sAt(Int_t indx) const%*s { return %s%sAt(indx);%*s }\n",
                                      typeLen, TArray2StandardType(valueType[iFold][i],tempBuffer),
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), Relation(valueType[iFold][i]), lb, "");
               if (valueType[iFold][i].ContainsFast("*")) {
                  buffer.AppendFormatted("   %-*s  Get%s() const%*s { return %s;%*s }\n",
                                         typeLen, valueType[iFold][i].Data(), valueName[iFold][i].Data(), lb, "",
                                         valueName[iFold][i].Data(), lb, "");
               }
               buffer.AppendFormatted("   %-*s  Get%sSize() const%*s { return %s%sGetSize();%*s }\n",
                                      typeLen, "Int_t", valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), Relation(valueType[iFold][i]), lb, "");
               buffer.AppendFormatted("   %-*s  Get%sCopy(Int_t n,%s* arrayToCopy) const;\n",
                                      typeLen, "void", valueName[iFold][i].Data(),
                                      TArray2StandardType(valueType[iFold][i],tempBuffer));
            } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   %-*s* Get%sAt(", typeLen, valueType[iFold][i].Data(), valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(");\n");
            } else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   %-*s  Get%sAt(Int_t indx) const;\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
            } else if (valueArray[iFold][i][0] == "variable") {
               buffer.AppendFormatted("   %-*s  Get%sAt(Int_t indx) const;\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   %-*s* Get%s() const%*s { return %s;%*s }\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
               buffer.AppendFormatted("   %-*s  Get%sSize() const%*s { return %sSize;%*s }\n", typeLen,
                                      "Int_t", valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
               buffer.AppendFormatted("   %-*s  Get%sCopy(Int_t n,%s* array) const;\n", typeLen,
                                      "void", valueName[iFold][i].Data(), valueType[iFold][i].Data());
            } else {
               buffer.AppendFormatted("   %-*s  Get%sAt(", typeLen, valueType[iFold][i].Data(), valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(") const;\n");
               buffer.AppendFormatted("   %-*s* Get%s()%*s { return &%s", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("[0]");
               buffer.AppendFormatted(";%*s }\n", lb, "");
               buffer.AppendFormatted("   %-*s  Get%sCopy(Int_t n,%s* array) const;\n", typeLen,
                                      "void", valueName[iFold][i].Data(), valueType[iFold][i].Data());
            }
         } else {
            if (isFolder(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   %-*s* Get%s()%*s { return &%s;%*s }\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            } else if (valueType[iFold][i] == "TRef") {
               buffer.AppendFormatted("   %-*s* Get%s() %*s { return &%s;%*s }\n", typeLen, "TRef",
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
               buffer.AppendFormatted("   %-*s* Get%sObj() const%*s { return %s.GetObject();%*s }\n", typeLen, "TObject",
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   %-*s* Get%s()%*s { return &%s;%*s }\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            } else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   %-*s  Get%s() const%*s { return %s;%*s }\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            } else {
               buffer.AppendFormatted("   %-*s  Get%s() const%*s { return %s;%*s }\n",
                                      typeLen, valueType[iFold][i].Data(), valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // isModified
      if (folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted("   virtual %-*s  is%s();\n", typeLen, "Bool_t", "Modified");
      } else {
         buffer.AppendFormatted("   %-*s  is%s();\n", typeLen, "Bool_t", "Modified");
      }
      buffer.AppendFormatted("\n");

      // Setters
      for (i = 0; i < numOfValue[iFold]; i++) {
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i] == 0) {
            if (valueType[iFold][i] == "TRef") {
               buffer.AppendFormatted("   void Set%s%*s(%-*s %s_value%*s) { %s%*s = %s_value;%*s SetModified(true); }\n",
                                      valueName[iFold][i].Data(), lb, "", typeLen, "TObject*",
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            } else if (isFolder(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())) {
               // operator= is not implemented
            } else {
               if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) &&
                   !valueType[iFold][i].ContainsFast("TRef")) {
                  buffer.AppendFormatted("   // you might have errors to use Set%s, if operator= is not implemented\n",
                                         valueName[iFold][i].Data());
                  buffer.AppendFormatted("   // in %s class but %s has pointer data member.\n",
                                         valueType[iFold][i].Data(), valueType[iFold][i].Data());
               }
               buffer.AppendFormatted("   void Set%s%*s(%-*s %s_value%*s) { %s%*s = %s_value;%*s SetModified(true); }\n",
                                      valueName[iFold][i].Data(), lb, "", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(), lb, "",
                                      valueName[iFold][i].Data(), lb, "");
            }
         } else {
            if (valueType[iFold][i] == "TRef") {
               buffer.AppendFormatted("   void Set%sAt%*s(", valueName[iFold][i].Data(), lb, "");
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%-*s %s_value);\n", typeLen, "TObject*", valueName[iFold][i].Data());
            } else if (valueType[iFold][i] == "TRefArray") {
               buffer.AppendFormatted("   void Set%sAt%*s(Int_t indx, TObject* %s_value%*s) { %s%sAddAt(%s_value,indx)%*s;%*s SetModified(true); }\n",
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(),
                                      lb, "", valueName[iFold][i].Data(), Relation(valueType[iFold][i]),
                                      valueName[iFold][i].Data(), lb, "", lb, "");
               buffer.AppendFormatted("   void Set%sSize(Int_t number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s%sExpand(number);\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]));
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
            } else if (isFolder(valueType[iFold][i].Data())) {
               if (valueArray[iFold][i][0] != "variable")
                  buffer.AppendFormatted("private:\n");
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",valueName[iFold][i].Data());
               if (valueArray[iFold][i][0] != "variable")
                  buffer.AppendFormatted("public:\n");
            } else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               buffer.AppendFormatted("   void Set%sAt%*s(Int_t indx,%-*s %s_value%*s) { %s%sAddAt(%s_value,indx)%*s;%*s SetModified(true); }\n",
                                      valueName[iFold][i].Data(), lb, "", typeLen,TArray2StandardType(valueType[iFold][i], tempBuffer),
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(), Relation(valueType[iFold][i]),
                                      valueName[iFold][i].Data(), lb, "", lb, "");
               buffer.AppendFormatted("   void Set%sSize(Int_t number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s%sSet(number);\n",valueName[iFold][i].Data(),Relation(valueType[iFold][i]));
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   %-*s  Set%sCopy(Int_t n, const %s* array);\n",
                                      typeLen, "void", valueName[iFold][i].Data(),
                                      TArray2StandardType(valueType[iFold][i], tempBuffer));
            } else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())) {
               continue;
            } else if (valueArray[iFold][i][0] == "variable") {
               buffer.AppendFormatted("   void Set%sAt%*s(Int_t indx,%-*s %s_value);\n",valueName[iFold][i].Data(), lb, "",
                                      typeLen, valueType[iFold][i].Data(), valueName[iFold][i].Data());
               buffer.AppendFormatted("   void Set%s%*s(%-*s* %s_value%*s) { %s = %s_value%*s;%*s SetModified(true); }\n",
                                      valueName[iFold][i].Data(), lb, "", typeLen - 1, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(),
                                      valueName[iFold][i].Data(), lb, "", lb, "");
               buffer.AppendFormatted("   void Set%sSize(Int_t number, Bool_t copyOldData = kFALSE, Bool_t fillZero = kFALSE);\n",
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   %-*s  Set%sCopy(Int_t n, const %s* array);\n", typeLen, "void",
                                      valueName[iFold][i].Data(), valueType[iFold][i].Data());
            } else {
               buffer.AppendFormatted("   void Set%sAt%*s(", valueName[iFold][i].Data(), lb, "");
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%-*s %s_value);\n", typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   %-*s  Set%sCopy(Int_t n, const %s* array);\n",
                                      typeLen, "void", valueName[iFold][i].Data(),
                                      TArray2StandardType(valueType[iFold][i],tempBuffer));
            }
         }
      }
      buffer.AppendFormatted("\n");
      // Add
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (isFolder(valueType[iFold][i].Data()) || isPointerType(valueType[iFold][i].Data()) ||
             (valueIsTObject[iFold][i] && !isTArrayType(valueType[iFold][i])) || isBoolType(valueType[iFold][i].Data())) {
            continue;
         }
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i] == 0) {
            buffer.AppendFormatted("   void Add%s%*s(%-*s %s_value%*s) { %s%*s += %s_value;%*s SetModified(true); }\n",
                                   valueName[iFold][i].Data(), lb, "", typeLen, valueType[iFold][i].Data(),
                                   valueName[iFold][i].Data(), lb, "", valueName[iFold][i].Data(), lb, "",
                                   valueName[iFold][i].Data(), lb, "");
         } else {
            if (isTArrayType(valueType[iFold][i])) {
               buffer.AppendFormatted("   void Add%sAt%*s(Int_t indx,%-*s %s_value);\n",
                                      valueName[iFold][i].Data(), lb, "", typeLen,
                                      TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data());
            } else {
               buffer.AppendFormatted("   void Add%sAt(",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%-*s %s_value);\n",typeLen, valueType[iFold][i].Data(),
                                      valueName[iFold][i].Data());
            }
         }
      }
      buffer.AppendFormatted("\n");

      // SetModified
      int lb = nameLen-8;
      if (folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted("   virtual void Set%s%*s(%-*s %s%*s) { f%s%*s = %s;%*s}\n",
                                "Modified", lb, "", typeLen, "Bool_t", "modified", lb, "", "Modified",
                                lb, "", "modified", lb, "");
         buffer.AppendFormatted("\n");
      }

      // ResetModified
      if (folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted("   virtual void ResetModified();\n");
      } else {
         buffer.AppendFormatted("   void ResetModified();\n");
      }
      buffer.AppendFormatted("\n");

      // Set All
      if (folderInheritName[iFold].Length() == 0) { // only base class can have SetAll
         if (numOfValue[iFold] < maxNumberOfArguments) { // rootcint does not accept more than 40
            buffer.AppendFormatted("   void SetAll( ");
            for (i = 0; i < numOfValue[iFold]; i++) {
               if (isFolder(valueType[iFold][i].Data()))
                  continue;
               if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data()) &&
                   valueType[iFold][i] != "TRef")
                  continue;
               if (valueDimension[iFold][i] == 0) {
                  if (valueType[iFold][i] == "TRef") {
                     buffer.AppendFormatted("TObject* %s_value=%s,",valueName[iFold][i].Data(),
                                            valueInit[iFold][i].Data());
                  } else {
                     buffer.AppendFormatted("%s %s_value=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),
                                            valueInit[iFold][i].Data());
                  }
               }
            }
            buffer.Resize(buffer.Length() - 1);
            buffer.AppendFormatted(" );\n");
         }
      }
      buffer.AppendFormatted("\n");
      // Reset
      if (folderInheritName[iFold].Length() == 0) {
         buffer.AppendFormatted("   virtual void Reset();\n");
      } else {
         buffer.AppendFormatted("   void Reset();\n");
      }
      buffer.AppendFormatted("\n");

      // Private
      buffer.AppendFormatted("private:\n");

      // BoundsOk
      for (i = 0; i < numOfValue[iFold]; i++) {
         if (valueDimension[iFold][i] > 0) {
            if (isFolder(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   Bool_t %sBoundsOk(const char* where, Int_t at) const;\n",
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   Bool_t %sOutOfBoundsError(const char* where, Int_t i) const;\n",
                                      valueName[iFold][i].Data());
            } else if (valueArray[iFold][i][0] == "variable") {
               buffer.AppendFormatted("   Bool_t %sBoundsOk(const char* where, Int_t at) const;\n",
                                      valueName[iFold][i].Data());
               buffer.AppendFormatted("   Bool_t %sOutOfBoundsError(const char* where, Int_t i) const;\n",
                                      valueName[iFold][i].Data());
            } else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
            } else if (valueType[iFold][i] == "TRefArray") {
               // TRefArray itself checks bounds.
            } else {
               buffer.AppendFormatted("   Bool_t %sBoundsOk(const char* where, ",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(") const;\n");
               buffer.AppendFormatted("   Bool_t %sOutOfBoundsError(const char* where,",valueName[iFold][i].Data());
               for (iDm = 0; iDm < valueDimension[iFold][i]; iDm++)
                  buffer.AppendFormatted(" Int_t %c,",valueCounter[iDm]);
               buffer.Resize(buffer.Length() - 1);
               buffer.AppendFormatted(") const;\n");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // Footer
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("\n   ClassDef(%s%s_Base,%s) // Base class of %s%s\n",shortCut.Data(),
                                folderName[iFold].Data(),folderVersion[iFold].Data(),shortCut.Data(),
                                folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)",shortCut.Data(),folderName[iFold].Data(),
                                folderVersion[iFold].Data());
         if (folderShortDescription[iFold].Length())
            buffer.AppendFormatted(" // %s", folderShortDescription[iFold].Data());
         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("};\n\n");
#if 0
      buffer.AppendFormatted("#if !defined(__CINT__)\n");
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("ClassImp(%s%s_Base)\n",shortCut.Data(),folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("ClassImp(%s%s)\n",shortCut.Data(),folderName[iFold].Data());
      }
      buffer.AppendFormatted("#endif\n");
#endif
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("#endif   // %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
      }

      // Write File
      WriteFile(hFile.Data(),buffer.Data(),6);

      // User H-File
      hFile.SetFormatted("%sinclude/folders/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && (changeableFlagChanged || gSystem->AccessPathName(hFile.Data(),kFileExists))) {
         buffer.Resize(0);

         // Description
         WriteHeader(buffer, numOfFolderAuthors[iFold], folderAuthor[iFold], folderAuthorEmail[iFold], kFALSE);
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
         clsName.SetFormatted("%s%s", shortCut.Data(), folderName[iFold].Data());
         WriteDescription(buffer, clsName.Data(), 0, kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"generated/%s%s_Base.h\"\n",shortCut.Data(),folderName[iFold].Data());

         // Class
         buffer.AppendFormatted("\nclass %s%s : public %s%s_Base\n",shortCut.Data(),folderName[iFold].Data(),
                                shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i = 0; i < numOfValue[iFold]; i++) {
            if (valueDimension[iFold][i] == 0)
               buffer.AppendFormatted("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),
                                      valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length() - 1);
         buffer.AppendFormatted(" ) : %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
#if 0
         for (i = 0; i < numOfValue[iFold]; i++) {
            if (valueDimension[iFold][i] == 0)
               buffer.AppendFormatted("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length() - 1);
#endif
         buffer.AppendFormatted(" ) {}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)",shortCut.Data(),folderName[iFold].Data(),
                                folderVersion[iFold].Data());
         if (folderShortDescription[iFold].Length())
            buffer.AppendFormatted(" // %s", folderShortDescription[iFold].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteAllFoldersH() {
   ROMEString hFile;
   ROMEString buffer;
   Int_t i;

   buffer.Resize(0);
   hFile.SetFormatted("%sinclude/generated/%sAllFolders.h", outDir.Data(), shortCut.Data());
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);

   buffer.AppendFormatted("#ifndef %sAllFolders_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sAllFolders_H\n\n",shortCut.Data());
   // Folder includes
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]){
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");
   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteTaskCpp()
{
   ROMEString cppFile;
   ROMEString genFile;
   ROMEString header;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString fileBuffer;
   ROMEString tmp, tmp2;

   int i,j;
   ROMEString discript;
   ROMEString str;

   if (makeOutput) cout<<"\n   Output Cpp-Files:"<<endl;
   for (int iTask = 0; iTask < numOfTask; iTask++) {
      if (!taskUsed[iTask])
         continue;
      header.Resize(0);
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      fileBuffer.ReadFile(cppFile.Data(), kTRUE);

      // Generated Includes
      buffer.Resize(0);
      WriteHeader(buffer, numOfTaskAuthors[iTask], taskAuthor[iTask], taskAuthorEmail[iTask], kTRUE);
      genFile.SetFormatted("%sinclude/generated/%sT%sGeneratedIncludes.h", outDir.Data(), shortCut.Data(),
                           taskName[iTask].Data());

      numOfTaskAccessedFolder[iTask] = 0;
      for (j = 0; j < numOfFolder; j++) {
         if (accessFolder(cppFile.Data(),j)) {
            if (!folderUsed[j])
               continue;
            if (FolderToBeGenerated(j) && !folderSupport[j]) {
               taskAccessedFolder[iTask][numOfTaskAccessedFolder[iTask]] = j;
               if (folderUserCode[j]) {
                  buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               } else {
                  buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),
                                         folderName[j].Data());
               }
               numOfTaskAccessedFolder[iTask]++;
            }
         }
      }
      if (fileBuffer.ContainsFast("GetWindow") || fileBuffer.ContainsFast("fWindow"))
         buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
      if (fileBuffer.ContainsFast("GetGSP"))
         buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
      for (j = 0; j < daqNameArray->GetEntriesFast(); j++) {
         tmp.SetFormatted("Get%sDAQ()",daqNameArray->At(j).Data());
         tmp2.SetFormatted("Get%s()",daqNameArray->At(j).Data());
         if (fileBuffer.ContainsFast(tmp) || fileBuffer.ContainsFast(tmp2))
            buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(j).Data(),shortCut.Data(),
                                   daqNameArray->At(j).Data());
      }

      WriteFile(genFile.Data(), buffer.Data(),6);

      // Description
      WriteHeader(header, numOfTaskAuthors[iTask], taskAuthor[iTask], taskAuthorEmail[iTask], kFALSE);
      clsName.SetFormatted("%sT%s", shortCut.Data(), taskName[iTask].Data());
      clsDescription = "Begin_Html\n\n";
      if (taskDescription[iTask].Length()) {
         clsDescription.AppendFormatted("Description:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(taskDescription[iTask].Data());
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (taskUsage[iTask].Length()) {
         clsDescription.AppendFormatted("Usage:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(taskUsage[iTask]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (taskStatus[iTask].Length()) {
         clsDescription.AppendFormatted("Status:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(taskStatus[iTask]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (taskToDo[iTask].Length()) {
         clsDescription.AppendFormatted("To Do:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(taskToDo[iTask]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (taskKnownProblems[iTask].Length()) {
         clsDescription.AppendFormatted("Known Problems:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(taskKnownProblems[iTask]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      clsDescription.AppendFormatted("End_Html\n\n");
      if (numOfTaskAuthors[iTask] > 0) {
         clsDescription.AppendFormatted("The event methods have been written by");
         for (j = 0; j < numOfTaskAuthors[iTask]; j++) {
            if (j == 0) {
               clsDescription.AppendFormatted(" %s", taskAuthor[iTask][j].Data());
            } else if (j == numOfTaskAuthors[iTask] - 1) {
               clsDescription.AppendFormatted(" and %s", taskAuthor[iTask][j].Data());
            } else {
               clsDescription.AppendFormatted(", %s", taskAuthor[iTask][j].Data());
            } 
         }
         clsDescription.AppendFormatted(".\n");
      }
      bool first = true;
      for (j = 0; j < numOfFolder; j++) {
         if (accessFolder(cppFile.Data(), j, kTRUE)) {
            if (first) {
               clsDescription.AppendFormatted("\n");
               clsDescription.AppendFormatted("Please note: The following information is only correct after executing the ROMEBuilder.\n");
               clsDescription.AppendFormatted("\n");
               clsDescription.AppendFormatted("This task accesses the following folders :\n");
               first = false;
            }
            if (!folderUsed[j]) {
#if 0
               cout<<"Error : Task "<<taskName[iTask].Data()<<" tries to access folder "<<folderName[j].Data()<<endl;
               cout<<"        but this folder is currently not linked into the project."<<endl<<endl;
               cout<<"        Please check the affiliations of the task and the folder,"<<endl;
               cout<<"        or modify the source code of the task."<<endl;
               cout<<"Terminating program."<<endl;
               return false;
#endif
            }
            clsDescription.AppendFormatted("    %s\n",folderName[j].Data());
         }
      }
      clsDescription.AppendFormatted("\n");
      if (numOfHistos[iTask] > 0 || numOfGraphs[iTask] > 0) {
         if (first) {
            clsDescription.AppendFormatted("Please note: The following information is only correct after executing the ROMEBuilder.\n");
            clsDescription.AppendFormatted("              \n");
            clsDescription.AppendFormatted("\n");
            first = false;
         }
         if (numOfHistos[iTask] > 0) {
            clsDescription.AppendFormatted("This task contains the following histgrams :\n");
            for (i = 0; i < numOfHistos[iTask]; i++) {
               clsDescription.AppendFormatted("   %s\n",histoName[iTask][i].Data());
            }
         }
         if (numOfGraphs[iTask] > 0) {
            clsDescription.AppendFormatted("This task contains the following graphs :\n");
            for (i = 0; i < numOfGraphs[iTask]; i++) {
               clsDescription.AppendFormatted("   %s\n",graphName[iTask][i].Data());
            }
         }
         clsDescription.AppendFormatted("\n");
         clsDescription.AppendFormatted("The histograms/graph are created and saved automaticaly by the task.\n");
         clsDescription.AppendFormatted("\n");
         clsDescription.AppendFormatted("The following method can be used to get a handle of the histogram/graph :\n");
         clsDescription.AppendFormatted("\n");
         clsDescription.AppendFormatted("Get<Histogram/Graph Name>()\n");
         clsDescription.AppendFormatted("\n");
         clsDescription.AppendFormatted("For histogram/graph arrays use :\n");
         clsDescription.AppendFormatted("\n");
         clsDescription.AppendFormatted("Get<Histogram/Graph Name>At(Int_t index)\n");
      }
      // Generated Includes
      clsDescription.AppendFormatted("\n");
      ROMEString genInclude;
      genInclude.SetFormatted("#include \"generated/%sT%sGeneratedIncludes.h\"\n", shortCut.Data(),
                              taskName[iTask].Data());
      WriteDescription(header, clsName.Data(), clsDescription.Data(), kTRUE, genInclude.Data());

      buffer.Resize(0);
      buffer.AppendFormatted("\n\n");
      // Header
      buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
      buffer.AppendFormatted("#include \"tasks/%sT%s.h\"\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("// uncomment if you want to include headers of all folders\n");
      buffer.AppendFormatted("//#include \"%sAllFolders.h\"\n",shortCut.Data());
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("\nClassImp(%sT%s)\n\n",shortCut.Data(),taskName[iTask].Data());

      // User Functions
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::Init()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::Event()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::Terminate()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());

      //Write File
      // replace header
      ROMEString str1, str2;
      str1.SetFormatted("include/generated/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
      str2.SetFormatted("include/tasks/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
      ReplaceHeader(cppFile.Data(),header.Data(),buffer.Data(),6,str1.Data(),str2.Data());
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteBaseTaskCpp()
{
   int i,j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString tmp;

   ROMEString steerPointer;
   ROMEString discript;
   ROMEString str;
   bool found;

   if (makeOutput) cout<<"\n   Output Cpp-Files:"<<endl;
   for (int iTask = 0; iTask < numOfTask; iTask++) {
      if (!taskUsed[iTask])
         continue;

      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/generated/%sT%s_Base.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      WriteHeader(buffer, numOfTaskAuthors[iTask], taskAuthor[iTask], taskAuthorEmail[iTask], kTRUE);
      clsName.SetFormatted("%sT%s_Base", shortCut.Data(), taskName[iTask].Data());
      clsDescription = taskDescription[iTask].Data();
      clsDescription.AppendFormatted("\n\n");

      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kTRUE);

      // Header
      buffer.AppendFormatted("\n\n");

      if ((numOfHistos[iTask] > 0 || numOfGraphs[iTask] > 0) && readGlobalSteeringParameters) {
         buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
      }
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
      buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("\nClassImp(%sT%s_Base)\n\n",shortCut.Data(),taskName[iTask].Data());

      // Constructor
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(const char *name,const char *title,int level,const char *taskSuffix,TFolder *histoFolder)\n",
                             shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());

      buffer.AppendFormatted(":ROMETask(name,title,level");
      buffer.AppendFormatted(",%s",taskVersion[iTask].Data());
      buffer.AppendFormatted(",%s",taskEventID[iTask].Data());
      buffer.AppendFormatted(",histoFolder,taskSuffix,%d,%d)\n",numOfHistos[iTask], numOfGraphs[iTask]);

      if (numOfSteering[iTask] > 0) {
         buffer.AppendFormatted(",fSteering(new Steering())\n");
      }
      buffer.AppendFormatted("{\n");
      if (numOfHistos[iTask] > 0 || numOfGraphs[iTask] > 0) {
         buffer.AppendFormatted("   int j;\n");
      }
      buffer.AppendFormatted("   fTaskIndex = %d;\n", iTask);

      // Create Root Folders
      for (i = 0; i < numOfHistos[iTask]; i++) {
         if (histoFolderName[iTask][i] == "") {
            buffer.AppendFormatted("   fRootFolder->AddAt(GetHistoFolder(), %d);\n", i);
            continue;
         }
         found = false;
         for (j = 0; j < i; j++) {
            if (histoFolderName[iTask][i] == histoFolderName[iTask][j]) {
               found = true;
               break;
            }
         }
         if (found) {
            buffer.AppendFormatted("   fRootFolder->AddAt(static_cast<TFolder*>(GetHistoFolder()->FindObject(\"%s\")), %d);\n",
                                   histoFolderName[iTask][j].Data(), i);
         } else {
            buffer.AppendFormatted("   fRootFolder->AddAt(GetHistoFolder()->AddFolder(\"%s\",\"folder to store %s histos/graphs\"), %d);\n",
                                   histoFolderName[iTask][i].Data(), histoFolderName[iTask][i].Data(), i);
         }
      }
      for (i = 0; i < numOfGraphs[iTask]; i++) {
         if (graphFolderName[iTask][i] == "") {
            buffer.AppendFormatted("   fRootFolder->AddAt(GetHistoFolder(), %d);\n", numOfHistos[iTask] + i);
            continue;
         }
         found = false;
         for (j = 0; j < numOfHistos[iTask]; j++) {
            if (graphFolderName[iTask][i] == histoFolderName[iTask][j]) {
               found = true;
               break;
            }
         }
         if (found) {
            buffer.AppendFormatted("   fRootFolder->AddAt(static_cast<TFolder*>(GetHistoFolder()->FindObject(\"%s\")), %d);\n",
                                   histoFolderName[iTask][j].Data(), numOfHistos[iTask] + i);
            continue;
         }
         for (j = 0; j < i; j++) {
            if (graphFolderName[iTask][i] == graphFolderName[iTask][j]) {
               found = true;
               break;
            }
         }
         if (found) {
            buffer.AppendFormatted("   fRootFolder->AddAt(static_cast<TFolder*>(GetHistoFolder()->FindObject(\"%s\")), %d);\n",
                                   graphFolderName[iTask][j].Data(), numOfHistos[iTask]+i);
         } else {
            buffer.AppendFormatted("   fRootFolder->AddAt(GetHistoFolder()->AddFolder(\"%s\",\"folder to store %s histos/graphs\"), %d);\n",
                                   graphFolderName[iTask][i].Data(), graphFolderName[iTask][i].Data(), numOfHistos[iTask]+i);
         }
      }

      // Histos
      if (numOfHistos[iTask] > 0) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   for (j=0;j<fNumberOfHistos;j++) {\n");
         buffer.AppendFormatted("      fHistoParameter->AddAt(new ROMEHisto(),j);\n");
         buffer.AppendFormatted("   }\n");
//         buffer.AppendFormatted("   SetOriginalHistoParameters();\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   fHistoName      = new ROMEString [fNumberOfHistos];\n");
         buffer.AppendFormatted("   fHistoType      = new ROMEString [fNumberOfHistos];\n");
         buffer.AppendFormatted("   fHistoDimension = new Int_t      [fNumberOfHistos];\n");
         buffer.AppendFormatted("   fHistoArray     = new Bool_t     [fNumberOfHistos];\n");
         buffer.AppendFormatted("   fHistoUpToDate  = new Bool_t     [fNumberOfHistos];\n");
         buffer.AppendFormatted("   for (j = 0; j < fNumberOfHistos; j++) { fHistoUpToDate[j] = false; }\n");
         buffer.AppendFormatted("\n");
         for (i = 0; i < numOfHistos[iTask]; i++) {
            buffer.AppendFormatted("   fHistoName[%d] = \"%s\";\n", i, histoName[iTask][i].Data());
            buffer.AppendFormatted("   fHistoType[%d] = \"%s\";\n", i, histoType[iTask][i].Data());
            if (histoType[iTask][i][2] == '1') {
               buffer.AppendFormatted("   fHistoDimension[%d] = 1;\n", i);
            } else if (histoType[iTask][i][2] == '2') {
               buffer.AppendFormatted("   fHistoDimension[%d] = 2;\n", i);
            } else if (histoType[iTask][i][2] == '3') {
               buffer.AppendFormatted("   fHistoDimension[%d] = 3;\n", i);
            }
            if (histoArraySize[iTask][i] == "1") {
               buffer.AppendFormatted("   fHistoArray[%d] = false;\n", i);
            } else {
               buffer.AppendFormatted("   fHistoArray[%d] = true;\n", i);
            }
         }
      }

      // Graphs
      if (numOfGraphs[iTask] > 0) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   for (j=0;j<fNumberOfGraphs;j++) {\n");
         buffer.AppendFormatted("      fGraphParameter->AddAtAndExpand(new ROMEGraph(),j);\n");
         buffer.AppendFormatted("   }\n");
//         buffer.AppendFormatted("   SetOriginalGraphParameters();\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   fGraphName      = new ROMEString [fNumberOfGraphs];\n");
         buffer.AppendFormatted("   fGraphType      = new ROMEString [fNumberOfGraphs];\n");
         buffer.AppendFormatted("   fGraphDimension = new Int_t      [fNumberOfGraphs];\n");
         buffer.AppendFormatted("   fGraphArray     = new Bool_t     [fNumberOfGraphs];\n");
         buffer.AppendFormatted("\n");
         for (i = 0; i < numOfGraphs[iTask]; i++) {
            buffer.AppendFormatted("   fGraphName[%d] = \"%s\";\n", i, graphName[iTask][i].Data());
            buffer.AppendFormatted("   fGraphType[%d] = \"%s\";\n", i, graphType[iTask][i].Data());
            if (graphType[iTask][i][2] == '1') {
               buffer.AppendFormatted("   fGraphDimension[%d] = 1;\n", i);
            } else if (graphType[iTask][i][2] == '2') {
               buffer.AppendFormatted("   fGraphDimension[%d] = 2;\n", i);
            } else if (graphType[iTask][i][2] == '3') {
               buffer.AppendFormatted("   fGraphDimension[%d] = 3;\n", i);
            }
            if (graphArraySize[iTask][i] == "1") {
               buffer.AppendFormatted("   fGraphArray[%d] = false;\n", i);
            } else {
               buffer.AppendFormatted("   fGraphArray[%d] = true;\n", i);
            }
         }
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Set Original Parameters
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::SetOriginalHistoParameters()\n{\n", shortCut.Data(), taskName[iTask].Data());
      if (numOfHistos[iTask] > 0) {
         for (i = 0; i < numOfHistos[iTask]; i++) {
            buffer.AppendFormatted("   ((ROMEHisto*)fHistoParameter->At(%d))->SetOriginal(\"%s\", \"%s\", %s, %s, \"%s\", \"%s\", \"%s\", %s, %s, %s, %s, %s, %s, %s, %s, %s);\n",
                                   i, histoTitle[iTask][i].Data(),
                                   histoFolderTitle[iTask][i].Data(), histoArraySize[iTask][i].Data(),
                                   histoArrayStartIndex[iTask][i].Data(),
                                   histoXLabel[iTask][i].Data(), histoYLabel[iTask][i].Data(), histoZLabel[iTask][i].Data(),
                                   histoXNbins[iTask][i].Data(), histoXmin[iTask][i].Data(), histoXmax[iTask][i].Data(),
                                   histoYNbins[iTask][i].Data(), histoYmin[iTask][i].Data(), histoYmax[iTask][i].Data(),
                                   histoZNbins[iTask][i].Data(), histoZmin[iTask][i].Data(), histoZmax[iTask][i].Data());
         }
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::SetOriginalGraphParameters()\n{\n", shortCut.Data(), taskName[iTask].Data());
      if (numOfGraphs[iTask] > 0) {
         for (i = 0; i < numOfGraphs[iTask]; i++) {
            buffer.AppendFormatted("   ((ROMEGraph*)fGraphParameter->At(%d))->SetOriginal(\"%s\", \"%s\", %s, %s, \"%s\", \"%s\", \"%s\", %s, %s, %s, %s, %s, %s);\n",
                                   i, graphTitle[iTask][i].Data(),
                                   graphFolderTitle[iTask][i].Data(), graphArraySize[iTask][i].Data(),
                                   graphArrayStartIndex[iTask][i].Data(), graphXLabel[iTask][i].Data(),
                                   graphYLabel[iTask][i].Data(), graphZLabel[iTask][i].Data(),
                                   graphXmin[iTask][i].Data(), graphXmax[iTask][i].Data(),
                                   graphYmin[iTask][i].Data(), graphYmax[iTask][i].Data(),
                                   graphZmin[iTask][i].Data(), graphZmax[iTask][i].Data());
         }
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Graph Getters
      if (numOfGraphs[iTask] > 0) {
         buffer.AppendFormatted("// Graph Getters\n");
         for (i = 0; i < numOfGraphs[iTask]; i++) {
            if (graphArraySize[iTask][i] == "1") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %sT%s_Base::Get%s()\n", graphType[iTask][i].Data(), shortCut.Data(),
                                      taskName[iTask].Data(), graphName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!CheckGraphActive(%d)) return 0;\n",i);
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
               buffer.AppendFormatted("      return static_cast<%s*>(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(\"Task_%d:Graph_%d\"));\n",
                                      graphType[iTask][i].Data(), iTask, i);
               buffer.AppendFormatted("   return static_cast<%s*>(fGraph->At(%d));\n", graphType[iTask][i].Data(), i);
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %sT%s_Base::Get%sAt(Int_t indx)\n", graphType[iTask][i].Data(),
                                      shortCut.Data(), taskName[iTask].Data(), graphName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!CheckGraphActive(%d)) return 0;\n", i);
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor()) {\n");
               buffer.AppendFormatted("      ROMEString str;\n");
               buffer.AppendFormatted("      str.SetFormatted(\"Task_%d:Graph_%d_%%d\",indx);\n", iTask, i);
               buffer.AppendFormatted("      return static_cast<%s*>(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(str.Data()));\n",
                                      graphType[iTask][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   return static_cast<%s*>(static_cast<TObjArray*>(fGraph->At(%d))->At(indx));\n", graphType[iTask][i].Data(), i);
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("TObjArray* %sT%s_Base::Get%s()\n", shortCut.Data(), taskName[iTask].Data(),
                                      graphName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!CheckGraphActive(%d)) { return 0; }\n",i);
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
               buffer.AppendFormatted("      return static_cast<TObjArray*>(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(\"Task_%d:Graphs_%d\"));\n",
                                      iTask, i);
               buffer.AppendFormatted("   return static_cast<TObjArray*>(fGraph->At(%d));\n", i);
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("Bool_t %sT%s_Base::Register%s()\n{\n", shortCut.Data(), taskName[iTask].Data(),
                                   graphName[iTask][i].Data());
            buffer.AppendFormatted("   if (((ROMEGraph*)fGraphParameter->At(%d))->IsActive()) {\n", i);
            buffer.AppendFormatted("      if (gAnalyzer->IsROMEMonitor())\n");
            buffer.AppendFormatted("         return gAnalyzer->GetSocketClientNetFolder()->RegisterObject(\"Task_%d:Graph_%d\");\n", iTask, i);
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   return false;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("Bool_t %sT%s_Base::UnRegister%s()\n{\n", shortCut.Data(), taskName[iTask].Data(),
                                   graphName[iTask][i].Data());
            buffer.AppendFormatted("   if (((ROMEGraph*)fGraphParameter->At(%d))->IsActive()) {\n", i);
            buffer.AppendFormatted("      if (gAnalyzer->IsROMEMonitor())\n");
            buffer.AppendFormatted("         return gAnalyzer->GetSocketClientNetFolder()->UnRegisterObject(\"Task_%d:Graph_%d\");\n", iTask, i);
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   return false;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
         }
      }

      // Get Object Interpreter Code
      int codeNumber = fNumberOfInterpreterCodes;
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Int_t %sT%s_Base::GetObjectInterpreterCode(const char* objectPath) const\n{\n", shortCut.Data(),
                             taskName[iTask].Data());
      buffer.AppendFormatted("   ROMEString path = objectPath;\n");
      buffer.AppendFormatted("   if (path.Index(\"/\") == -1 && path.Index(\"gAnalyzer->\") == -1)\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   if (path.Index(\"/SP\") == 0 || path.Index(\"gAnalyzer->GetSP()\") == 0) {\n");
      ROMEString path1 = "/SP";
      ROMEString path2 = "gAnalyzer->GetSP()";
      WriteSteeringInterpreterCode(buffer,codeNumber, 0, iTask, path1, path2, 1);
      buffer.AppendFormatted("      ROMEPrint::Error(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
      buffer.AppendFormatted("      ROMEPrint::Error(\"   %%s\\n\", path.Data());\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCode(objectPath);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Int Value
      codeNumber = fNumberOfInterpreterCodes;
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Int_t %sT%s_Base::GetObjectInterpreterIntValue(Int_t code, Int_t defaultValue) const\n{\n",
                             shortCut.Data(), taskName[iTask].Data());
      if (numOfSteerFields[iTask][0] > 0 || numOfSteerChildren[iTask][0] > 0) {
         buffer.AppendFormatted("   char *cstop;\n");
         buffer.AppendFormatted("   cstop = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   switch (code) {\n");
         buffer.AppendFormatted("      case -1:\n");
         buffer.AppendFormatted("         return defaultValue;\n");
         steerPointer = "GetSP()";
         WriteSteeringInterpreterValue(buffer, "Int_t", codeNumber, 0, iTask, steerPointer, 1);
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterIntValue(code, defaultValue);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Double Value
      codeNumber = fNumberOfInterpreterCodes;
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Double_t %sT%s_Base::GetObjectInterpreterDoubleValue(Int_t code, Double_t defaultValue) const\n{\n",
                             shortCut.Data(), taskName[iTask].Data());
      if (numOfSteerFields[iTask][0] > 0 || numOfSteerChildren[iTask][0] > 0) {
         buffer.AppendFormatted("   char *cstop;\n");
         buffer.AppendFormatted("   cstop = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   switch (code) {\n");
         buffer.AppendFormatted("      case -1:\n");
         buffer.AppendFormatted("         return defaultValue;\n");
         steerPointer = "GetSP()";
         WriteSteeringInterpreterValue(buffer, "Double_t", codeNumber, 0, iTask, steerPointer, 1);
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterDoubleValue(code, defaultValue);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Char Value
      codeNumber = fNumberOfInterpreterCodes;
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("ROMEString& %sT%s_Base::GetObjectInterpreterCharValue(Int_t code, ROMEString& defaultValue, ROMEString& buffer) const\n{\n",
                             shortCut.Data(), taskName[iTask].Data());
      if (numOfSteerFields[iTask][0] > 0 || numOfSteerChildren[iTask][0] > 0) {
         buffer.AppendFormatted("   ROMEString str;\n");
         buffer.AppendFormatted("   switch (code) {\n");
         buffer.AppendFormatted("      case -1:\n");
         buffer.AppendFormatted("         return defaultValue;\n");
         steerPointer = "GetSP()";
         WriteSteeringInterpreterValue(buffer, "const char*", codeNumber, 0, iTask, steerPointer, 1);
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCharValue(code,defaultValue,buffer);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      //Write File
      WriteFile(cppFile.Data(), buffer.Data(), 6);
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteTaskH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout<<"\n   Output H-Files:"<<endl;
   for (int iTask = 0; iTask < numOfTask; iTask++) {
      if (!taskUsed[iTask])
         continue;

      // User H-File
      hFile.SetFormatted("%sinclude/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      if ((gSystem->AccessPathName(hFile.Data(),kFileExists))) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, numOfTaskAuthors[iTask], taskAuthor[iTask], taskAuthorEmail[iTask], kFALSE);
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());
         clsName.SetFormatted("%sT%s", shortCut.Data(), taskName[iTask].Data());
         WriteDescription(buffer, clsName.Data(), 0, kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[iTask].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n",shortCut.Data(),taskName[iTask].Data(),
                                shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("\n");
         // Constructor
         buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   %sT%s(const %sT%s &c); // not implemented\n",
                                shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s &operator=(const %sT%s &c); // not implemented\n",
                                shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %sT%s(const char *name = 0, const char *title = 0, int level = 0, const char *taskSuffix = 0, TFolder *histoFolder = 0)\n",
                                shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   :%sT%s_Base(name,title,level,taskSuffix,histoFolder) {}\n",
                                shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   virtual ~%sT%s() {}\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("   // Event Methods\n");
         buffer.AppendFormatted("   void Init();\n");
         buffer.AppendFormatted("   void BeginOfRun();\n");
         buffer.AppendFormatted("   void Event();\n");
         buffer.AppendFormatted("   void EndOfRun();\n");
         buffer.AppendFormatted("   void Terminate();\n\n");
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)",shortCut.Data(),taskName[iTask].Data(),
                                taskVersion[iTask].Data());
         if (taskShortDescription[iTask].Length())
            buffer.AppendFormatted(" // %s", taskShortDescription[iTask].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteBaseTaskH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   int i;

   if (makeOutput) cout<<"\n   Output H-Files:"<<endl;
   for (Int_t iTask = 0; iTask < numOfTask; iTask++) {
      if (!taskUsed[iTask]) {
         continue;
      }

      // File name
      hFile.SetFormatted("%sinclude/generated/%sT%s_Base.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, numOfTaskAuthors[iTask], taskAuthor[iTask], taskAuthorEmail[iTask], kTRUE);
      buffer.AppendFormatted("#ifndef %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("#define %sT%s_Base_H\n\n",shortCut.Data(),taskName[iTask].Data());
      clsName.SetFormatted("%sT%s_Base", shortCut.Data(), taskName[iTask].Data());
      clsDescription = taskDescription[iTask].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
      buffer.AppendFormatted("\n\n");

      buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
      if (numOfHistos[iTask] > 0) {
         buffer.AppendFormatted("#include <TH1.h>\n");
         buffer.AppendFormatted("#include <TH2.h>\n");
         buffer.AppendFormatted("#include <TH3.h>\n");
         buffer.AppendFormatted("#include <TProfile.h>\n");
         buffer.AppendFormatted("#include <TProfile2D.h>\n");
      }
      if (numOfGraphs[iTask] > 0) {
         buffer.AppendFormatted("#include <ROMETGraph.h>\n");
         buffer.AppendFormatted("#include <TGraph2D.h>\n");
         buffer.AppendFormatted("#include <ROMETCutG.h>\n");
      }
      if (numOfSteering[iTask] > 0) {
         buffer.AppendFormatted("#include <TString.h>\n"); // TString is often used for steering parameter.
      }
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"ROMETask.h\"\n");
      buffer.AppendFormatted("#include \"ROMEAnalyzer.h\"\n");

      for (i = 0; i < numOfTaskInclude[iTask]; i++) {
         if (taskLocalFlag[iTask][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",taskInclude[iTask][i].Data());
         } else {
            buffer.AppendFormatted("#include <%s>\n",taskInclude[iTask][i].Data());
         }
      }

      // Class
      buffer.AppendFormatted("\nclass %sT%s_Base : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("{\n");

      buffer.AppendFormatted("protected:\n");

      // Fields
      if (numOfSteering[iTask] > 0) {
         WriteSteeringClass(buffer, 0, iTask, 1);
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Steering* fSteering; // Handle to Steering class\n\n");
      }

      // Methods
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("private:\n");
      buffer.AppendFormatted("   %sT%s_Base(const %sT%s_Base &c); // not implemented\n",
                             shortCut.Data(),taskName[iTask].Data(), shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   %sT%s_Base &operator=(const %sT%s_Base &c); // not implemented\n",
                             shortCut.Data(),taskName[iTask].Data(), shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("public:\n");
      buffer.AppendFormatted("   %sT%s_Base(const char *name,const char *title,int level,const char *taskSuffix,TFolder *histoFolder);\n",
                             shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   virtual ~%sT%s_Base() {",shortCut.Data(),taskName[iTask].Data());
      if (numOfSteering[iTask] > 0) {
         buffer.AppendFormatted(" SafeDelete(fSteering);");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTask] > 0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() const { return fSteering; }\n");
         buffer.AppendFormatted("   Steering* GetSP() const { return fSteering; }\n");
      }
      // Access Methods
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("   Int_t GetHistoDimensionAt(Int_t i) { return fHistoDimension[i]; };\n");
      }
      if (numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("   ROMEString* GetGraphNameAt(Int_t i) { return &fGraphName[i]; };\n");
         buffer.AppendFormatted("   Int_t GetGraphDimensionAt(Int_t i) { return fGraphDimension[i]; };\n");
      }
      for (i = 0; i < numOfHistos[iTask]; i++) {
         if (histoArraySize[iTask][i] == "1") {
            buffer.AppendFormatted("   %s* Get%s() { return ((%s*)GetHistoAt(%d)); };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoType[iTask][i].Data(),i);
         } else {
            buffer.AppendFormatted("   %s* Get%sAt(Int_t indx) { return static_cast<%s*>(static_cast<TObjArray*>(GetHistoAt(%d))->At(indx)); };\n",histoType[iTask][i].Data(),
                                   histoName[iTask][i].Data(),histoType[iTask][i].Data(),i);
            buffer.AppendFormatted("   TObjArray* Get%s() { return static_cast<TObjArray*>(GetHistoAt(%d)); };\n",histoName[iTask][i].Data(),i);
         }
         buffer.AppendFormatted("   ROMEHisto* Get%sHisto() const { return static_cast<ROMEHisto*>(fHistoParameter->At(%d)); }\n",
                                histoName[iTask][i].Data(),i);
         buffer.AppendFormatted("   Bool_t Is%sActive() const {return static_cast<ROMEHisto*>(fHistoParameter->At(%d))->IsActive(); }\n",histoName[iTask][i].Data(),i);
      }

      for (i = 0; i < numOfGraphs[iTask]; i++) {
         if (graphArraySize[iTask][i] == "1") {
            buffer.AppendFormatted("   %s* Get%s();\n",graphType[iTask][i].Data(),graphName[iTask][i].Data());
         } else {
            buffer.AppendFormatted("   %s* Get%sAt(Int_t indx);\n",graphType[iTask][i].Data(),
                                   graphName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* Get%s();\n",graphName[iTask][i].Data());
         }
         buffer.AppendFormatted("   ROMEGraph* Get%sGraph() const { return static_cast<ROMEGraph*>(fGraphParameter->At(%d)); }\n",
                                graphName[iTask][i].Data(),i);
         buffer.AppendFormatted("   Bool_t Is%sActive() const {return static_cast<ROMEGraph*>(fGraphParameter->At(%d))->IsActive(); }\n",
                                graphName[iTask][i].Data(),i);
         buffer.AppendFormatted("   Bool_t Register%s();\n",graphName[iTask][i].Data());
         buffer.AppendFormatted("   Bool_t UnRegister%s();\n",graphName[iTask][i].Data());
      }

      // Object Interpreter
      buffer.AppendFormatted("   Int_t GetObjectInterpreterCode(const char* objectPath) const;\n");
      buffer.AppendFormatted("   Int_t GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue) const;\n");
      buffer.AppendFormatted("   Double_t GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue) const;\n");
      buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer) const;\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("protected:\n");
      // Event Methods
      buffer.AppendFormatted("   // Event Methods\n");
      buffer.AppendFormatted("   virtual void Init() = 0;\n");
      buffer.AppendFormatted("   virtual void BeginOfRun() = 0;\n");
      buffer.AppendFormatted("   virtual void Event() = 0;\n");
      buffer.AppendFormatted("   virtual void EndOfRun() = 0;\n");
      buffer.AppendFormatted("   virtual void Terminate() = 0;\n");
      buffer.AppendFormatted("\n");

      // Histo Methods
      buffer.AppendFormatted("   // Histo Methods\n");
      buffer.AppendFormatted("   void SetOriginalHistoParameters();\n");
      // Graph Methods
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   // Graph Methods\n");
      buffer.AppendFormatted("   void SetOriginalGraphParameters();\n");
      buffer.AppendFormatted("\n");

      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s) // Base class of %sT%s\n",shortCut.Data(),
                             taskName[iTask].Data(),taskVersion[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("};\n\n");

      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());

      // Write File
      WriteFile(hFile.Data(),buffer.Data(),6);

   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteTabCpp()
{
   ROMEString cppFile;
   ROMEString genFile;
   ROMEString header;
   ROMEString buffer;
   Int_t i, j;
   ROMEString clsDescription;
   ROMEString clsName;
   ROMEString fileBuffer;
   ROMEString str;
   ROMEString tmp, tmp2;
   ROMEStrArray str1;
   ROMEStrArray str2;
   ROMEStrArray cond;
   TArrayI equal;

   if (makeOutput)
      cout<<"\n   Output Cpp-Files:"<<endl;
   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      header.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      fileBuffer.ReadFile(cppFile.Data(), kTRUE);

      // Generated Includes
      buffer.Resize(0);
      WriteHeader(buffer, numOfTabAuthors[iTab], tabAuthor[iTab], tabAuthorEmail[iTab], kTRUE);
      genFile.SetFormatted("%sinclude/generated/%sT%sGeneratedIncludes.h", outDir.Data(), shortCut.Data(),
                           tabName[iTab].Data());
#if defined( R__VISUAL_CPLUSPLUS ) // This fixes errors in root includes on windows
      buffer.AppendFormatted("#include <RConfig.h>\n");
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#include <TGTab.h>\n");
      buffer.AppendFormatted("#include <TGeoManager.h>\n");
      buffer.AppendFormatted("#include <Windows4Root.h>\n");
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      for (j = 0; j < numOfFolder; j++) {
         if (accessFolder(cppFile.Data(),j)) {
            if (!folderUsed[j])
               continue;
            if (FolderToBeGenerated(j) && !folderSupport[j]) {
               if (folderUserCode[j]) {
                  buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               } else {
                  buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               }
            }
         }
      }
      if (fileBuffer.ContainsFast("GetWindow"))
         buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
      if (fileBuffer.ContainsFast("GetGSP"))
         buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());

      for (j = 0; j < daqNameArray->GetEntriesFast(); j++) {
         tmp.SetFormatted("Get%sDAQ()",daqNameArray->At(j).Data());
         tmp2.SetFormatted("Get%s()",daqNameArray->At(j).Data());
         if (fileBuffer.ContainsFast(tmp) || fileBuffer.ContainsFast(tmp2))
            buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(j).Data(),shortCut.Data(),
                                   daqNameArray->At(j).Data());
      }

      WriteFile(genFile.Data(), buffer.Data(),6);

      // Description
      buffer.Resize(0);
      WriteHeader(header, numOfTabAuthors[iTab], tabAuthor[iTab], tabAuthorEmail[iTab], kFALSE);
      clsName.SetFormatted("%sT%s", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = "Begin_Html\n\n";
      if (tabDescription[iTab].Length()) {
         clsDescription.AppendFormatted("Description:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(tabDescription[iTab]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (tabUsage[iTab].Length()) {
         clsDescription.AppendFormatted("Usage:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(tabUsage[iTab]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (tabStatus[iTab].Length()) {
         clsDescription.AppendFormatted("Status:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(tabStatus[iTab]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (tabToDo[iTab].Length()) {
         clsDescription.AppendFormatted("To Do:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(tabToDo[iTab]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      if (tabKnownProblems[iTab].Length()) {
         clsDescription.AppendFormatted("Known Problems:\n");
         clsDescription.AppendFormatted("<p>\n");
         clsDescription.Append(tabKnownProblems[iTab]);
         clsDescription.AppendFormatted("\n</p>\n\n");
      }
      clsDescription.AppendFormatted("End_Html\n\n");
      // Thread
      if (numOfThreadFunctions[iTab] > 0) {
         clsDescription.AppendFormatted("\n\n");
         clsDescription.AppendFormatted("This tab has following thread functions.\n");
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            clsDescription.AppendFormatted("   %s\n", threadFunctionName[iTab][i].Data());
         }
      }
      // Generated Includes
      ROMEString genInclude;
      genInclude.SetFormatted("#include \"generated/%sT%sGeneratedIncludes.h\"\n", shortCut.Data(),
                              tabName[iTab].Data());
      WriteDescription(header, clsName.Data(), clsDescription.Data(), kTRUE, genInclude.Data());

      buffer.Resize(0);

      buffer.AppendFormatted("\n\n");
      // Header
      buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n", shortCut.Data());
      buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n", shortCut.Data());
      buffer.AppendFormatted("#include \"tabs/%sT%s.h\"\n", shortCut.Data(), tabName[iTab].Data());

      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("// uncomment if you want to include headers of all folders\n");
      buffer.AppendFormatted("//#include \"%sAllFolders.h\"\n",shortCut.Data());
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("\nClassImp(%sT%s)\n\n", shortCut.Data(), tabName[iTab].Data());

      // Functions
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::Init()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::EndInit()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::EventHandler()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::MenuClicked(TGPopupMenu * /* menu */, Long_t /* param */)\n",
                             shortCut.Data(),
                             tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::TabSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s::TabUnSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      if (tabObjectDisplay[iTab]) {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sT%s::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)\n", shortCut.Data(),
                                tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   BaseSetupPads(nx, ny, redraw);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sT%s::Display(bool processEvents)\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   BaseDisplay(processEvents);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sT%s::%s()\n", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   gSystem->Sleep(10000);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Write file
#if 0 // this is not necessary already, maybe.
      if (tabObjectDisplay[iTab]) {
         str1.RemoveAll();
         str2.RemoveAll();
         cond.RemoveAll();
         equal.Set(2);
         str1.AddFormatted("ClassImp(%sT%s)",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("ClassImp(%sT%s)\nvoid %sT%s::Display(bool processEvents)\n{\n   BaseDisplay(processEvents);\n}",
                           shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
         cond.AddFormatted("%sT%s::Display(bool processEvents)",shortCut.Data(),tabName[iTab].Data());
         equal.AddAt(0, 0);
         str1.AddFormatted("ClassImp(%sT%s)",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("ClassImp(%sT%s)\nvoid %sT%s::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)\n{\n   BaseSetupPads(nx,ny,redraw);\n}",
                           shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
         cond.AddFormatted("%sT%s::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)",shortCut.Data(),tabName[iTab].Data());
         equal.AddAt(0, 1);
         ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6,str1,str2,cond,equal);
      } else {
         ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6);
      }
#else
      ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6);
#endif
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteBaseTabCpp()
{
   ROMEString cppFile;
   ROMEString buffer;
   Int_t i, j, k, indx;
   Int_t iTab;
   ROMEString clsDescription;
   ROMEString clsName;
   ROMEString str;
   ROMEString str1;
   ROMEString str2;
   ROMEString tabFile;
   ROMEString tabFileBuffer;
   Bool_t second;

   if (makeOutput)
      cout<<"\n   Output Cpp-Files:"<<endl;
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/generated/%sT%s_Base.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());

      // Description
      WriteHeader(buffer, numOfTabAuthors[iTab], tabAuthor[iTab], tabAuthorEmail[iTab], kFALSE);
      clsName.SetFormatted("%sT%s_Base", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = tabDescription[iTab].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kTRUE);

      buffer.AppendFormatted("\n\n");
      // Header
      buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <TCanvas.h>\n");
      buffer.AppendFormatted("#include <TFrame.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
      buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n", shortCut.Data());
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("#include \"ROMETGraph.h\"\n");
         buffer.AppendFormatted("#include <TGraph2D.h>\n");
         buffer.AppendFormatted("#include <ROMETCutG.h>\n");
         buffer.AppendFormatted("#include <TH1.h>\n");
         buffer.AppendFormatted("#include <TH2.h>\n");
         buffer.AppendFormatted("#include <TH3.h>\n");
         buffer.AppendFormatted("#include <TProfile.h>\n");
         buffer.AppendFormatted("#include <TProfile2D.h>\n");
         buffer.AppendFormatted("#include <TStyle.h>\n");
         buffer.AppendFormatted("#include <TLine.h>\n");
         buffer.AppendFormatted("#include <TArrayI.h>\n");
         if (readGlobalSteeringParameters)
            buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
      }

      if (tabHeredity[iTab].Length() > 0)
         buffer.AppendFormatted("#include \"tabs/%sT%s.h\"\n",shortCut.Data(),tabHeredity[iTab].Data());
      // Task class includes
      Bool_t *taskListed = new Bool_t[numOfTask];
      for (i = 0; i < numOfTask; i++) {
         taskListed[i] = false;
      }
      tabFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      tabFileBuffer.SetFormatted("%sBuffer", tabFile.Data());
      for (j = 0; j < numOfTaskHierarchy; j++) {
         if (!taskUsed[taskHierarchyClassIndex[j]])
            continue;
         for (k = 0; k < numOfGraphs[taskHierarchyClassIndex[j]]; k++) {
            if (accessGraph(tabFile.Data(),taskHierarchyClassIndex[j],k) &&
                !taskListed[taskHierarchyClassIndex[j]]) {
               taskListed[taskHierarchyClassIndex[j]] = true;
               buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),
                                      taskHierarchyName[j].Data());
            }
         }
      }
      for (i = 0; i < tabSingleObjectIndexMax[iTab]; i++) {
         for (j = 0; j < numOfTabSingleObjects[iTab]; j++) {
            if (tabSingleObjectIndex[iTab][j] == i) {
               indx = tabSingleObjectTaskHierarchyIndex[iTab][j];
               if (indx != -1 && !taskListed[taskHierarchyClassIndex[indx]]) {
                  taskListed[taskHierarchyClassIndex[indx]] = true;
                  for (k = 0; k < numOfTaskInclude[taskHierarchyClassIndex[indx]]; k++) {
                     if (taskLocalFlag[taskHierarchyClassIndex[indx]][k]) {
                        buffer.AppendFormatted("#include \"%s\"\n",taskInclude[taskHierarchyClassIndex[indx]][k].Data());
                     } else {
                        buffer.AppendFormatted("#include <%s>\n",taskInclude[taskHierarchyClassIndex[indx]][k].Data());
                     }
                  }
               }
            }
         }
      }
      for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
         if (tabObjectDisplayTaskIndex[iTab][i] > -1 && !taskUsed[tabObjectDisplayTaskIndex[iTab][i]])
            continue;
         indx = tabObjectDisplayTaskHierarchyIndex[iTab][i];
         if (indx != -1 && !taskListed[taskHierarchyClassIndex[indx]]) {
            taskListed[taskHierarchyClassIndex[indx]] = true;
            for (j = 0; j < numOfTaskInclude[taskHierarchyClassIndex[indx]]; j++) {
               if (taskLocalFlag[taskHierarchyClassIndex[indx]][j]) {
                  buffer.AppendFormatted("#include \"%s\"\n",taskInclude[taskHierarchyClassIndex[indx]][j].Data());
               } else {
                  buffer.AppendFormatted("#include <%s>\n",taskInclude[taskHierarchyClassIndex[indx]][j].Data());
               }
            }
         }
      }

      delete [] taskListed;
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[iTab].Data());

      buffer.AppendFormatted("\nClassImp(%sT%s_Base)\n\n", shortCut.Data(), tabName[iTab].Data());

      // Constructor
      buffer.AppendFormatted("// Constructor\n");
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(%sWindow* window)\n", shortCut.Data(), tabName[iTab].Data(),
                             shortCut.Data(), tabName[iTab].Data(), shortCut.Data());

      if (tabHeredity[iTab].Length() > 0) {
         buffer.AppendFormatted(":%sT%s(window)",shortCut.Data(),tabHeredity[iTab].Data());
      } else {
         if (tabObjectDisplay[iTab]) {
            buffer.AppendFormatted(":ArgusHistoDisplay(window");
         } else {
            buffer.AppendFormatted(":ArgusTab(window");
         }
         buffer.AppendFormatted(", \"%s\"", tabTitle[iTab].Data());
         if (numOfTabSingleObjects[iTab] > 0) {
            buffer.AppendFormatted(", new ROMEStrArray()");
            buffer.AppendFormatted(", new TArrayI(%d)",numOfTabSingleObjects[iTab]);
            buffer.AppendFormatted(", new TArrayI(%d)",numOfTabSingleObjects[iTab]);
            buffer.AppendFormatted(", new TArrayI(%d)",numOfTabSingleObjects[iTab]);
         } else {
            buffer.AppendFormatted(", 0, 0, 0, 0");
         }

         int numMenu = numOfMenu[iTab];
         if (tabHeredity[iTab].Length() > 0) {
            numMenu += numOfMenu[tabHeredityIndex[iTab]];
         }
         if (numMenu <=0) {
            numMenu = 0;
         }
         buffer.AppendFormatted(", %d", numMenu);
         if (tabObjectDisplay[iTab]) {
            buffer.AppendFormatted(", \"%s\"", tabName[iTab].Data());
            buffer.AppendFormatted(", %d", numOfTabObjectDisplays[iTab]);
         }
         buffer.AppendFormatted(")\n");
      }

      if (numOfSteering[iTab+numOfTask + 1] > 0) {
         buffer.AppendFormatted(",fSteering(new Steering())\n");
      }
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted(",m%s(0)\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted(",f%sActive(kFALSE)\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted(",f%sNumberOfLoops(0)\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted(",f%sInterval(1000)\n", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            // initializing with 0 can be a problem, depending on type of argument
            buffer.AppendFormatted(",f%sArgument_%d(0)\n", threadFunctionName[iTab][i].Data(), j);
         }
      }
      // Single Objects
      if (numOfTabSingleObjects[iTab] > 0) {
         buffer.AppendFormatted(",fGeneratedCanvas(0)\n");
         for (i = 0; i < numOfTabSingleObjects[iTab]; i++) {
            if (tabSingleObjectType[iTab][i] == "Histogram") {
               if (histoArraySize[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]] == "1") {
                  buffer.AppendFormatted(",f%sSingleObject%d(0)\n",tabSingleObjectName[iTab][i].Data(),i);
                  buffer.AppendFormatted(",f%sPad%d(0)\n",tabSingleObjectName[iTab][i].Data(),i);
               } else {
                  for (j = tabSingleObjectArrayIndexStart[iTab][i]; j <= tabSingleObjectArrayIndexEnd[iTab][i]; j++) {
                     buffer.AppendFormatted(",f%sSingleObject%d_%d(0)\n",tabSingleObjectName[iTab][i].Data(),i,j);
                     buffer.AppendFormatted(",f%sPad%d_%d(0)\n",tabSingleObjectName[iTab][i].Data(),i,j);
                  }
               }
            } else if (tabSingleObjectType[iTab][i] == "Graph") {
               if (graphArraySize[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]] == "1") {
                  buffer.AppendFormatted(",f%sSingleObject%d(0)\n",tabSingleObjectName[iTab][i].Data(),i);
                  buffer.AppendFormatted(",f%sPad%d(0)\n",tabSingleObjectName[iTab][i].Data(),i);
               } else {
                  for (j = tabSingleObjectArrayIndexStart[iTab][i]; j <= tabSingleObjectArrayIndexEnd[iTab][i]; j++) {
                     buffer.AppendFormatted(",f%sSingleObject%d_%d(0)\n",tabSingleObjectName[iTab][i].Data(),i,j);
                     buffer.AppendFormatted(",f%sPad%d_%d(0)\n",tabSingleObjectName[iTab][i].Data(),i,j);
                  }
               }
            }
         }
      }
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("   fName = \"%s\";\n", tabName[iTab].Data());

      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   InitHistoDisplay();\n");
      }

      if (numOfTabSingleObjects[iTab] > 0) {
         buffer.AppendFormatted("   int i;\n");
         buffer.AppendFormatted("   for (i = 0; i < %d; i++) {\n",numOfTabSingleObjects[iTab]);
         buffer.AppendFormatted("      fDrawOption->AddLast(\"\");\n");
         buffer.AppendFormatted("      fLogScaleX->AddAt(0, i);\n");
         buffer.AppendFormatted("      fLogScaleY->AddAt(0, i);\n");
         buffer.AppendFormatted("      fLogScaleZ->AddAt(0, i);\n");
         buffer.AppendFormatted("   }\n");
      }

      if (numOfTabObjectDisplays[iTab] > 0) {
         buffer.AppendFormatted("\n");
         for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
            if (tabObjectDisplayTaskHierarchyIndex[iTab][i] < 0) {
               buffer.AppendFormatted("   fMenuDisplay->AddEntry(\"%s\", M_DISPLAY_%s);\n",
                                      tabObjectDisplayTitle[iTab][i].Data(),
                                      tabObjectDisplayName[iTab][i].ToUpper(str));
            } else {
               if (tabObjectDisplayTaskHierarchyNumber[iTab][i] > 0) {
                  buffer.AppendFormatted("   fMenuDisplay->AddEntry(\"%d. %s\", M_DISPLAY_%s%s);\n",
                                         tabObjectDisplayTaskHierarchyNumber[iTab][i] + 1,
                                         tabObjectDisplayTitle[iTab][i].Data(),
                                         tabObjectDisplayName[iTab][i].ToUpper(str),
                                         taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               } else {
                  buffer.AppendFormatted("   fMenuDisplay->AddEntry(\"%s\", M_DISPLAY_%s%s);\n",
                                         tabObjectDisplayTitle[iTab][i].Data(),
                                         tabObjectDisplayName[iTab][i].ToUpper(str),
                                         taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               }
            }
         }
         if (tabObjectDisplayTaskHierarchyIndex[iTab][0] < 0) {
            buffer.AppendFormatted("   fMenuDisplay->RCheckEntry(M_DISPLAY_%s+fDisplayObjIndex,M_DISPLAY_%s",
                                   tabObjectDisplayName[iTab][0].ToUpper(str),
                                   tabObjectDisplayName[iTab][0].ToUpper(str1));
         } else {
            buffer.AppendFormatted("   fMenuDisplay->RCheckEntry(M_DISPLAY_%s%s+fDisplayObjIndex,M_DISPLAY_%s%s",
                                   tabObjectDisplayName[iTab][0].ToUpper(str),
                                   taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][0]].Data(),
                                   tabObjectDisplayName[iTab][0].ToUpper(str1),
                                   taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][0]].Data());
         }
         if (tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab] - 1] < 0) {
            buffer.AppendFormatted(",M_DISPLAY_%s);\n",
                                   tabObjectDisplayName[iTab][numOfTabObjectDisplays[iTab] - 1].ToUpper(str2));
         } else {
            buffer.AppendFormatted(",M_DISPLAY_%s%s);\n",
                                   tabObjectDisplayName[iTab][numOfTabObjectDisplays[iTab] - 1].ToUpper(str2),
                                   taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab] - 1]].Data());
         }
      }

      if (tabHeredity[iTab].Length() > 0 || numOfMenu[iTab] > 0) {
         buffer.AppendFormatted("\n");

         if (tabHeredity[iTab].Length() > 0) {
            for (j = 0; j < numOfMenu[tabHeredityIndex[iTab]]; j++) {
               if (menuDepth[tabHeredityIndex[iTab]][j] == 1) {
                  if (!AddMenuItems(buffer, tabHeredityIndex[iTab], j, i, j+numOfMenu[iTab], numOfMenu[iTab])) {
                     return kFALSE;
                  }
               }
            }
         }
         for (j = 0; j < numOfMenu[tabHeredityIndex[iTab]]; j++) {
            if (menuDepth[iTab][j] == 1) {
               if (!AddMenuItems(buffer, iTab, j, iTab, j, 0)) {
                  return kFALSE;
               }
            }
         }
      }

      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%sT%s_Base::~%sT%s_Base()\n{\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(),
                             tabName[iTab].Data());
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   Stop%s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // InitTab
      int nx = 1,ny = 1;
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::BaseInit()\n{\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   int i,j;\n");
      buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
      buffer.AppendFormatted("   j = 0;\n"); // to suppress unused warning
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   RegisterObjects();\n");
      buffer.AppendFormatted("   if (!RequestNewEvent(kMinInt, kMinInt)) {\n");
      buffer.AppendFormatted("      ROMEPrint::Warning(\"The analyzer doesn't provide any data in it's current state.\\n\");\n");
      buffer.AppendFormatted("      ROMEPrint::Warning(\"The monitor waits until the analyzer provides new data.\\n\\n\");\n");
      buffer.AppendFormatted("      gSystem->Sleep(100);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   while (!RequestNewEvent(kMinInt, kMinInt)) {\n");
      buffer.AppendFormatted("      gSystem->Sleep(100);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   Init();\n");
      if (numOfTabSingleObjects[iTab] > 0) {
         buffer.AppendFormatted("   // Init Histos\n");
         buffer.AppendFormatted("   fGeneratedCanvas = new TRootEmbeddedCanvas(\"GeneratedCanvas\", this, 600, 600);\n");
         buffer.AppendFormatted("   AddFrame(fGeneratedCanvas,new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n");
         if (tabSingleObjectIndexMax[iTab] > 1)
            ny = 2;
         if (tabSingleObjectIndexMax[iTab] > 2)
            nx = 2;
         if (tabSingleObjectIndexMax[iTab] > 4)
            ny = 3;
         if (tabSingleObjectIndexMax[iTab] > 6)
            nx = 3;
         if (tabSingleObjectIndexMax[iTab] > 9)
            ny = 4;
         if (tabSingleObjectIndexMax[iTab] > 12)
            nx = 4;
         if (tabSingleObjectIndexMax[iTab] > 16)
            ny = 5;
         if (tabSingleObjectIndexMax[iTab] > 20)
            nx = 5;
         buffer.AppendFormatted("   fGeneratedCanvas->GetCanvas()->Divide(%d, %d);\n",nx,ny);
         for (i = 0; i < tabSingleObjectIndexMax[iTab]; i++) {
            second = false;
            for (j = 0; j < numOfTabSingleObjects[iTab]; j++) {
               if (tabSingleObjectIndex[iTab][j] == i) {
                  buffer.AppendFormatted("   if (true");
                  indx = tabSingleObjectTaskHierarchyIndex[iTab][j];
                  while (indx != -1) {
                     buffer.AppendFormatted(" && gAnalyzer->GetTaskObjectAt(%d)->IsActive()",indx);
                     indx = taskHierarchyParentIndex[indx];
                  }
                  indx = tabSingleObjectTaskHierarchyIndex[iTab][j];
                  if (tabSingleObjectType[iTab][j] == "Graph") {
                     buffer.AppendFormatted(" && gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->IsActive()",
                                             tabSingleObjectTaskHierarchyIndex[iTab][j],
                                             tabSingleObjectObjectIndex[iTab][j]);
                  }
                  else {
                     buffer.AppendFormatted(" && gAnalyzer->GetTaskObjectAt(%d)->GetHistoParameterAt(%d)->IsActive()",
                                             tabSingleObjectTaskHierarchyIndex[iTab][j],
                                             tabSingleObjectObjectIndex[iTab][j]);
                  }
                  buffer.AppendFormatted(") {\n");
                  if (tabSingleObjectType[iTab][j] == "Histogram") {
                     if (histoArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]] == "1") {
                        buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",i + 1);
                        buffer.AppendFormatted("      f%sPad%d = static_cast<TPad*>(gPad);\n",
                                               tabSingleObjectName[iTab][j].Data(),j);
                        buffer.AppendFormatted("      f%sPad%d->SetLogx(fLogScaleX->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,j);
                        buffer.AppendFormatted("      f%sPad%d->SetLogy(fLogScaleY->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,j);
                        buffer.AppendFormatted("      f%sPad%d->SetLogz(fLogScaleZ->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,j);
                        buffer.AppendFormatted("      f%sSingleObject%d = new %s();\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data());
                        buffer.AppendFormatted("      *(f%sSingleObject%d) = *static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetHistoAt(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               histoType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j]);
                        if (second)
                           buffer.AppendFormatted("      f%sSingleObject%d->Draw(\"SAME\");\n",tabSingleObjectName[iTab][j].Data(),
                                                  j);
                        else
                           buffer.AppendFormatted("      f%sSingleObject%d->Draw();\n",tabSingleObjectName[iTab][j].Data(),
                                                  j);
                     } else {
                        for (k = tabSingleObjectArrayIndexStart[iTab][j]; k <= tabSingleObjectArrayIndexEnd[iTab][j];
                             k++) {
                           buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",
                                                  i + 1 + k - tabSingleObjectArrayIndexStart[iTab][j]);
                           buffer.AppendFormatted("      f%sPad%d_%d = static_cast<TPad*>(gPad);\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k);
                           buffer.AppendFormatted("      f%sPad%d_%d->SetLogx(fLogScaleX->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,j);
                           buffer.AppendFormatted("      f%sPad%d_%d->SetLogy(fLogScaleY->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,j);
                           buffer.AppendFormatted("      f%sPad%d_%d->SetLogz(fLogScaleZ->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,j);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d = new %s();\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data());
                           buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetHistoAt(%d))->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  histoType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j],k);
                           if (second)
                              buffer.AppendFormatted("      f%sSingleObject%d_%d->Draw(\"SAME\");\n",
                                                     tabSingleObjectName[iTab][j].Data(),j,k);
                           else
                              buffer.AppendFormatted("      f%sSingleObject%d_%d->Draw();\n",
                                                     tabSingleObjectName[iTab][j].Data(),j,k);
                        }
                     }
                  } else if (tabSingleObjectType[iTab][j] == "Graph") {
                     if (graphArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]] == "1") {
                        buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",i + 1);
                        buffer.AppendFormatted("      f%sPad%d = static_cast<TPad*>(gPad);\n",
                                               tabSingleObjectName[iTab][j].Data(),j);
                        buffer.AppendFormatted("      f%sPad%d->SetLogx(fLogScaleX->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,j);
                        buffer.AppendFormatted("      f%sPad%d->SetLogy(fLogScaleY->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,j);
                        buffer.AppendFormatted("      f%sPad%d->SetLogz(fLogScaleZ->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,j);
                        buffer.AppendFormatted("      f%sSingleObject%d = new %s();\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data());
                        buffer.AppendFormatted("      if ((static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d)))->GetN()<=0)\n",
                                               graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j]);
                        buffer.AppendFormatted("         f%sSingleObject%d->Set(1);\n",
                                               tabSingleObjectName[iTab][j].Data(),j);
                        buffer.AppendFormatted("      else\n");
                        buffer.AppendFormatted("         *(f%sSingleObject%d) = *static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j]);
                        buffer.AppendFormatted("      f%sSingleObject%d->GetXaxis()->SetLimits(gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetXmin(),gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetXmax());\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j],
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j]);
                        buffer.AppendFormatted("      f%sSingleObject%d->SetMinimum(gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetYmin());\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j]);
                        buffer.AppendFormatted("      f%sSingleObject%d->SetMaximum(gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetYmax());\n",
                                               tabSingleObjectName[iTab][j].Data(),j,
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j]);
                        if (second)
                           buffer.AppendFormatted("      f%sSingleObject%d->Draw(\"L SAME\");\n",
                                                  tabSingleObjectName[iTab][j].Data(),j);
                        else
                           buffer.AppendFormatted("      f%sSingleObject%d->Draw(\"AL\");\n",
                                                  tabSingleObjectName[iTab][j].Data(),j);
                     } else {
                        for (k = tabSingleObjectArrayIndexStart[iTab][j]; k <= tabSingleObjectArrayIndexEnd[iTab][j];
                             k++) {
                           buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",
                                                  i + 1 + k - tabSingleObjectArrayIndexStart[iTab][j]);
                           buffer.AppendFormatted("      f%sPad%d_%d = static_cast<TPad*>(gPad);\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k);
                           buffer.AppendFormatted("      f%sPad%d_%d->SetLogx(fLogScaleX->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,j);
                           buffer.AppendFormatted("      f%sPad%d_%d->SetLogy(fLogScaleY->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,j);
                           buffer.AppendFormatted("      f%sPad%d_%d->SetLogz(fLogScaleZ->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,j);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d = new %s();\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data());
                           buffer.AppendFormatted("      if ((static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d))->At(%d)))->GetN() <= 0)\n",
                                                  graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j],k);
                           buffer.AppendFormatted("         f%sSingleObject%d_%d->Set(1);\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k);
                           buffer.AppendFormatted("      else\n");
                           buffer.AppendFormatted("         *(f%sSingleObject%d_%d) = *static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d))->At(%d));\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j],k);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->GetXaxis()->SetLimits(gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetXmin(),gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetXmax());\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j],
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j]);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->SetMinimum(gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetYmin());\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j]);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->SetMaximum(gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->GetYmax());\n",
                                                  tabSingleObjectName[iTab][j].Data(),j,k,
                                                  tabSingleObjectTaskHierarchyIndex[iTab][j],
                                                  tabSingleObjectObjectIndex[iTab][j]);
                           if (second)
                              buffer.AppendFormatted("      f%sSingleObject%d_%d->Draw(\"L SAME\");\n",
                                                     tabSingleObjectName[iTab][j].Data(),j,k);
                           else
                              buffer.AppendFormatted("      f%sSingleObject%d_%d->Draw(\"AL\");\n",
                                                     tabSingleObjectName[iTab][j].Data(),j,k);
                        }
                     }
                  }
                  buffer.AppendFormatted("   }\n");
                  second = true;
               }
            }
         }
      }
      if (tabObjectDisplay[iTab]) {
         for (i = 0; i < numOfTabObjectDisplayObjectTypes[iTab]; i++) {
            buffer.AppendFormatted("   fObjects->AddLast(new TObjArray());\n");
            buffer.AppendFormatted("   fLines->AddLast(new TObjArray());\n");
            buffer.AppendFormatted("   for (i = 0 ; i<kMaxNumberOfPads ; i++) {\n");
            buffer.AppendFormatted("      if (fWindow->GetWindowId() == -1) {\n");
            buffer.AppendFormatted("         str.SetFormatted(\"f%s_%%d_%%s\",i,fInheritanceName.Data());\n",
                                   tabObjectDisplayObjectType[iTab][i].Data());
            buffer.AppendFormatted("      } else {\n");
            buffer.AppendFormatted("         str.SetFormatted(\"fSubWindow%%d_%s_%%d_%%s\",fWindow->GetWindowId(),i,fInheritanceName.Data());\n",
                                   tabObjectDisplayObjectType[iTab][i].Data());
            buffer.AppendFormatted("      }\n");
            if (tabObjectDisplayObjectType[iTab][i] == "ROMETGraph" ||
                tabObjectDisplayObjectType[iTab][i] == "ROMETCutG") {
               buffer.AppendFormatted("      static_cast<TObjArray*>(fObjects->Last())->AddLast(new %s(1));\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      static_cast<%s*>(static_cast<TObjArray*>(fObjects->Last())->Last())->SetTitle(str.Data());\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      static_cast<%s*>(static_cast<TObjArray*>(fObjects->Last())->Last())->SetName(str.Data());\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      static_cast<%s*>(static_cast<TObjArray*>(fObjects->Last())->Last())->GetHistogram();\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      static_cast<%s*>(static_cast<TObjArray*>(fObjects->Last())->Last())->SetPoint(0, 0, 0);\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
            } else if (tabObjectDisplayObjectType[iTab][i].ContainsFast("1")) {
               buffer.AppendFormatted("      static_cast<TObjArray*>(fObjects->Last())->AddLast(new %s(str.Data(),\"\", 1, 0, 1));\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
            } else if (tabObjectDisplayObjectType[iTab][i].ContainsFast("2")) {
               buffer.AppendFormatted("      static_cast<TObjArray*>(fObjects->Last())->AddLast(new %s(str.Data(),\"\", 1, 0, 1, 1, 0, 1));\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
            } else if (tabObjectDisplayObjectType[iTab][i].ContainsFast("3")) {
               buffer.AppendFormatted("      static_cast<TObjArray*>(fObjects->Last())->AddLast(new %s(str.Data(),\"\", 1, 0, 1, 1, 0, 1, 1, 0, 1));\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
            }
            buffer.AppendFormatted("      static_cast<TObjArray*>(fLines->Last())->AddLast(new TObjArray());\n");
            buffer.AppendFormatted("      for (j = 0; j<kMaxNumberOfLines; j++) {\n");
            buffer.AppendFormatted("         static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->Last())->Last())->AddLast(new TLine(-1, -1, -1, -1));\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }

         for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
            if (tabObjectDisplayTaskIndex[iTab][i] > -1) {
               for (j = 0; j < numOfTabObjectDisplayObjects[iTab][i]; j++) {
                  if (tabObjectDisplayType[iTab][i][j].Index("ROMETGraph") != -1 ||
                      tabObjectDisplayType[iTab][i][j].Index("ROMETCutG") != -1) {
                     buffer.AppendFormatted("   if (fNumberOfObjects < %s) {\n",
                                            graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     buffer.AppendFormatted("      fNumberOfObjects = %s;\n",
                                            graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     buffer.AppendFormatted("   }\n");

                  } else {
                     buffer.AppendFormatted("   if (fNumberOfObjects < %s) {\n",
                                            histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     buffer.AppendFormatted("      fNumberOfObjects = %s;\n",
                                            histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     buffer.AppendFormatted("   }\n");

                  }
               }
            }
         }
         buffer.AppendFormatted("   if (fNumberOfObjects < fNumberOfUserTGraph) {\n");
         buffer.AppendFormatted("      fNumberOfObjects = fNumberOfUserTGraph;\n");
         buffer.AppendFormatted("   }\n");
         if (numOfTabObjectDisplays[iTab] > 0) {
            for (i = 0; i < numOfTabObjectDisplayObjectTypes[iTab]; i++) {
               buffer.AppendFormatted("   fUserObjects->AddLast(new TObjArray());\n");
               buffer.AppendFormatted("   fUserLines->AddLast(new TObjArray());\n");
               buffer.AppendFormatted("   for (i = 0; i < fNumberOfObjects; i++) {\n");
               buffer.AppendFormatted("      if (fWindow->GetWindowId() == -1) {\n");
               buffer.AppendFormatted("         str.SetFormatted(\"fUser%s_%%d_%%s\",i,fInheritanceName.Data());\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      } else {\n");
               buffer.AppendFormatted("         str.SetFormatted(\"fSubWindow%%d_User%s_%%d_%%s\",fWindow->GetWindowId(),i,fInheritanceName.Data());\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      }\n");
               if (tabObjectDisplayObjectType[iTab][i] == "ROMETGraph") {
                  buffer.AppendFormatted("      static_cast<TObjArray*>(fUserObjects->Last())->AddLast(new %s(1));\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
                  buffer.AppendFormatted("      str.SetFormatted(\"fUser%s_%%d_%%s_Histo\",i,fInheritanceName.Data());\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
                  buffer.AppendFormatted("      static_cast<ROMETGraph*>(static_cast<TObjArray*>(fUserObjects->Last())->Last())->GetHistogram()->SetName(str.Data());\n");
                  buffer.AppendFormatted("      static_cast<ROMETGraph*>(static_cast<TObjArray*>(fUserObjects->Last())->Last())->SetTitle(str.Data());\n");
                  buffer.AppendFormatted("      static_cast<ROMETGraph*>(static_cast<TObjArray*>(fUserObjects->Last())->Last())->SetPoint(0, 0, 0);\n");
               }else if (tabObjectDisplayObjectType[iTab][i] == "ROMETCutG") {
                  buffer.AppendFormatted("      static_cast<TObjArray*>(fUserObjects->Last())->AddLast(new %s(1));\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
                  buffer.AppendFormatted("      str.SetFormatted(\"fUser%s_%%d_%%s_Histo\",i,fInheritanceName.Data());\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
                  buffer.AppendFormatted("      static_cast<ROMETCutG*>(static_cast<TObjArray*>(fUserObjects->Last())->Last())->GetHistogram()->SetName(str.Data());\n");
                  buffer.AppendFormatted("      static_cast<ROMETCutG*>(static_cast<TObjArray*>(fUserObjects->Last())->Last())->SetTitle(str.Data());\n");
                  buffer.AppendFormatted("      static_cast<ROMETCutG*>(static_cast<TObjArray*>(fUserObjects->Last())->Last())->SetPoint(0, 0, 0);\n");
               } else if (tabObjectDisplayObjectType[iTab][i].ContainsFast("1")) {
                  buffer.AppendFormatted("      static_cast<TObjArray*>(fUserObjects->Last())->AddLast(new %s(str.Data(),\"\", 1, 0, 1));\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
               } else if (tabObjectDisplayObjectType[iTab][i].ContainsFast("2")) {
                  buffer.AppendFormatted("      static_cast<TObjArray*>(fUserObjects->Last())->AddLast(new %s(str.Data(),\"\", 1, 0, 1, 1, 0, 1));\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
               } else if (tabObjectDisplayObjectType[iTab][i].ContainsFast("3")) {
                  buffer.AppendFormatted("      static_cast<TObjArray*>(fUserObjects->Last())->AddLast(new %s(str.Data(),\"\", 1, 0, 1, 1, 0, 1, 1, 0, 1));\n",
                                         tabObjectDisplayObjectType[iTab][i].Data());
               }
               buffer.AppendFormatted("      static_cast<TObjArray*>(fUserLines->Last())->AddLast(new TObjArray());\n");
               buffer.AppendFormatted("      for (j = 0; j < fNumberOfUserLines; j++) {\n");
               buffer.AppendFormatted("         static_cast<TObjArray*>(static_cast<TObjArray*>(fUserLines->Last())->Last())->AddLast(new TLine(-1, -1, -1, -1));\n");
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("   }\n");
            }
            buffer.AppendFormatted("   fNumberOfCurrentDisplayTypes = %d;\n",
                                   numOfTabObjectDisplayObjects[iTab][0]);
            buffer.AppendFormatted("   fCurrentDisplayType->Set(fNumberOfCurrentDisplayTypes);\n");
            for (j = 0; j < numOfTabObjectDisplayObjects[iTab][0]; j++) {
               buffer.AppendFormatted("   fCurrentDisplayType->AddAt(%d,%d);\n",
                                      tabObjectDisplayObjectTypeIndex[iTab][0][j],j);
            }
            buffer.AppendFormatted("   fDisplayObjIndex = 0;\n");
         }
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseInit();\n");
      }
      buffer.AppendFormatted("   EndInit();\n");
      buffer.AppendFormatted("   UnRegisterObjects();\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // BaseTabEventHandler
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::BaseEventHandler()\n{\n", shortCut.Data(), tabName[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   int ii;\n");
         buffer.AppendFormatted("   if (RequestNewEvent(fCurrentRun, fCurrentEvent)) {\n");
         buffer.AppendFormatted("      for (ii = 0; ii < fNumberOfDisplayTypes; ii++) {\n");
         buffer.AppendFormatted("         fDisplayObjLoaded->AddAt(0, ii);\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (fDisplayObjIndex >= fNumberOfDisplayTypes) {\n");
         buffer.AppendFormatted("      return;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (fDisplayObjLoaded->At(fDisplayObjIndex)) {\n");
         buffer.AppendFormatted("      return;\n");
         buffer.AppendFormatted("   }\n");
      } else {
         buffer.AppendFormatted("   if (!RequestNewEvent(fCurrentRun, fCurrentEvent)) {\n");
         buffer.AppendFormatted("      return;\n");
         buffer.AppendFormatted("   }\n");
      }
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   fStyle->cd();\n");
         for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
            if (tabObjectDisplayTaskIndex[iTab][i] != -1) {
               buffer.AppendFormatted("   int i;\n");
               break;
            }
         }
         buffer.AppendFormatted("   TObjArray  *ptr = 0;\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   ptr = 0; //warning supression\n");
         for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
            if (tabObjectDisplayTaskIndex[iTab][i] != -1) {
               for (j = 0; j < numOfTabObjectDisplayObjects[iTab][i]; j++) {
                  buffer.AppendFormatted("   if (fDisplayObjIndex == %d",i);
                  indx = tabObjectDisplayTaskHierarchyIndex[iTab][i];
                  while (indx != -1) {
                     buffer.AppendFormatted("\n       && gAnalyzer->GetTaskObjectAt(%d)->IsActive()",indx);
                     indx = taskHierarchyParentIndex[indx];
                  }
                  if (tabObjectDisplayType[iTab][i][j].BeginsWith("ROMETG") || tabObjectDisplayType[iTab][i][j].BeginsWith("ROMETC")) {
                     buffer.AppendFormatted("\n       && gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->IsActive()",
                                            tabObjectDisplayTaskHierarchyIndex[iTab][i],
                                            tabObjectDisplayObjectIndex[iTab][i][j]);
                  } else {
                     buffer.AppendFormatted("\n       && gAnalyzer->GetTaskObjectAt(%d)->GetHistoParameterAt(%d)->IsActive()",
                                            tabObjectDisplayTaskHierarchyIndex[iTab][i],
                                            tabObjectDisplayObjectIndex[iTab][i][j]);
                  }
                  buffer.AppendFormatted(") {\n");
                  buffer.AppendFormatted("      ptr = static_cast<TObjArray*>(fUserObjects->At(%d));\n",
                                         tabObjectDisplayObjectTypeIndex[iTab][i][j]);
                  if (tabObjectDisplayType[iTab][i][j].BeginsWith("ROMETG") || tabObjectDisplayType[iTab][i][j].BeginsWith("ROMETC")) {
                     buffer.AppendFormatted("      for (i = 0; i < %s; i++) {\n",
                                            graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     if (graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]] == "1") {
                        buffer.AppendFormatted("         *static_cast<%s*>(ptr->At(i)) = *static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d));\n",
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayTaskHierarchyIndex[iTab][i],
                                               tabObjectDisplayObjectIndex[iTab][i][j]);
                     } else {
                        buffer.AppendFormatted("         *static_cast<%s*>(ptr->At(i)) = *static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d))->At(i));\n",
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayTaskHierarchyIndex[iTab][i],
                                               tabObjectDisplayObjectIndex[iTab][i][j]);
                     }
                     buffer.AppendFormatted("         if (static_cast<%s*>(ptr->At(i))->GetN() == 0)\n",
                                            tabObjectDisplayType[iTab][i][j].Data());
                     buffer.AppendFormatted("            static_cast<%s*>(ptr->At(i))->Set(1);\n",
                                            tabObjectDisplayType[iTab][i][j].Data());
                     buffer.AppendFormatted("      }\n");
                     buffer.AppendFormatted("      for (i = %s; i < fNumberOfObjects; i++) {\n",
                                            graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     buffer.AppendFormatted("         static_cast<%s*>(ptr->At(i))->Set(1);\n",
                                            tabObjectDisplayType[iTab][i][j].Data());
                     buffer.AppendFormatted("      }\n");
                  } else {
                     buffer.AppendFormatted("      for (i = 0; i < %s; i++) {\n",
                                            histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     if (histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]] == "1") {
                        buffer.AppendFormatted("         *static_cast<%s*>(ptr->At(i)) = *static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetHistoAt(%d));\n",
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayTaskHierarchyIndex[iTab][i],
                                               tabObjectDisplayObjectIndex[iTab][i][j]);
                     } else {
                        buffer.AppendFormatted("         *static_cast<%s*>(ptr->At(i)) = *static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetHistoAt(%d))->At(i));\n",
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayType[iTab][i][j].Data(),
                                               tabObjectDisplayTaskHierarchyIndex[iTab][i],
                                               tabObjectDisplayObjectIndex[iTab][i][j]);
                     }
                     buffer.AppendFormatted("      }\n");
                     buffer.AppendFormatted("      for (i = %s; i < fNumberOfObjects; i++) {\n",
                                            histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i][j]].Data());
                     buffer.AppendFormatted("         static_cast<%s*>(ptr->At(i))->Reset();\n",
                                            tabObjectDisplayType[iTab][i][j].Data());
                     buffer.AppendFormatted("      }\n");
                  }
                  buffer.AppendFormatted("   }\n");
               }
            }
            buffer.AppendFormatted("\n");
         }
      }
      for (i = 0; i < tabSingleObjectIndexMax[iTab]; i++) {
         for (j = 0; j < numOfTabSingleObjects[iTab]; j++) {
            if (tabSingleObjectIndex[iTab][j] == i) {
               buffer.AppendFormatted("   if (true");
               indx = tabSingleObjectTaskHierarchyIndex[iTab][j];
               while (indx != -1) {
                  buffer.AppendFormatted(" && gAnalyzer->GetTaskObjectAt(%d)->IsActive()",indx);
                  indx = taskHierarchyParentIndex[indx];
               }
               if (tabSingleObjectType[iTab][j] == "Graph") {
                  buffer.AppendFormatted(" && gAnalyzer->GetTaskObjectAt(%d)->GetGraphParameterAt(%d)->IsActive()",
                                         tabSingleObjectTaskHierarchyIndex[iTab][j],
                                         tabSingleObjectObjectIndex[iTab][j]);
               } else {
                  buffer.AppendFormatted(" && gAnalyzer->GetTaskObjectAt(%d)->GetHistoParameterAt(%d)->IsActive()",
                                         tabSingleObjectTaskHierarchyIndex[iTab][j],
                                         tabSingleObjectObjectIndex[iTab][j]);
               }
               buffer.AppendFormatted(") {\n");
               if (tabSingleObjectType[iTab][j] == "Histogram") {
                  if (histoArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]] == "1") {
                     buffer.AppendFormatted("      *(f%sSingleObject%d) = *static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetHistoAt(%d));\n",
                                            tabSingleObjectName[iTab][j].Data(),j,
                                            histoType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                            tabSingleObjectTaskHierarchyIndex[iTab][j],
                                            tabSingleObjectObjectIndex[iTab][j]);
                     buffer.AppendFormatted("      f%sPad%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),j);
                  } else {
                     for (k = tabSingleObjectArrayIndexStart[iTab][j]; k <= tabSingleObjectArrayIndexEnd[iTab][j]; k++) {
                        buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetHistoAt(%d))->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,k,
                                               histoType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j],k);
                        buffer.AppendFormatted("      f%sPad%d_%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),
                                               j, k);
                     }
                  }
               } else if (tabSingleObjectType[iTab][j] == "Graph") {
                  if (graphArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]] == "1") {
                     buffer.AppendFormatted("      *(f%sSingleObject%d) = *static_cast<%s*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d));\n",
                                            tabSingleObjectName[iTab][j].Data(),j,
                                            graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                            tabSingleObjectTaskHierarchyIndex[iTab][j],
                                            tabSingleObjectObjectIndex[iTab][j]);
                     buffer.AppendFormatted("      f%sPad%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),j);
                  } else {
                     for (k = tabSingleObjectArrayIndexStart[iTab][j]; k <= tabSingleObjectArrayIndexEnd[iTab][j];
                          k++) {
                        buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *static_cast<%s*>(static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphAt(%d))->At(%d));\n",
                                               tabSingleObjectName[iTab][j].Data(),j,k,
                                               graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data(),
                                               tabSingleObjectTaskHierarchyIndex[iTab][j],
                                               tabSingleObjectObjectIndex[iTab][j],k);
                        buffer.AppendFormatted("      f%sPad%d_%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),
                                               j, k);
                     }
                  }
               }
               buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(1);\n");
               buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->Modified();\n");
               buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->Update();\n");
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      buffer.AppendFormatted("   fWatchUserEvent.Start(false);\n");
      buffer.AppendFormatted("   EventHandler();\n");
      buffer.AppendFormatted("   fWatchUserEvent.Stop();\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   Display(false);\n");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Display
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::BaseDisplay(bool processEvents)\n{\n",
                             shortCut.Data(), tabName[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   int i,j,chn;\n");
         buffer.AppendFormatted("   int iLine, nLines;\n");
         buffer.AppendFormatted("   Int_t      iDisplay = 0;\n");
         buffer.AppendFormatted("   TObjArray *userObjects = 0;\n");
         buffer.AppendFormatted("   TObjArray *objects = 0;\n");
         buffer.AppendFormatted("   TObjArray *linearray = 0;\n");
         buffer.AppendFormatted("   TLine     *userLine = 0;\n");
         buffer.AppendFormatted("   TLine     *line = 0;\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   for (i = 0 ; i<fNumberOfPads ; i++) {\n");
         buffer.AppendFormatted("      if (fPadConfigActive) {\n");
         buffer.AppendFormatted("         chn = fPadConfigChannel[i];\n");
         buffer.AppendFormatted("      } else {\n");
         buffer.AppendFormatted("         chn = fChannelNumber + i;\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      for (j=0;j<fNumberOfCurrentDisplayTypes;j++) {\n");
         buffer.AppendFormatted("         iDisplay    = fCurrentDisplayType->At(j);\n");
         buffer.AppendFormatted("         userObjects = static_cast<TObjArray*>(fUserObjects->At(iDisplay));\n");        
         buffer.AppendFormatted("         objects     = static_cast<TObjArray*>(fObjects->At(iDisplay));\n");        
         buffer.AppendFormatted("         if (chn < userObjects->GetEntriesFast()) {\n");
         for (i = 0; i < numOfTabObjectDisplayObjectTypes[iTab]; i++) {
            buffer.AppendFormatted("            if (!strcmp(userObjects->At(i)->ClassName(),\"%s\")) {\n",
                                   tabObjectDisplayObjectType[iTab][i].Data());
//            if (tabObjectDisplayObjectType[iTab][i] == "ROMETGraph" ||
//                tabObjectDisplayObjectType[iTab][i] == "ROMETCutG") { // TGraph::operator= does not free existing fX and fY
//            }
            buffer.AppendFormatted("               *static_cast<%s*>(objects->At(i)) = *static_cast<%s*>(userObjects->At(chn));\n",
                                   tabObjectDisplayObjectType[iTab][i].Data(),
                                   tabObjectDisplayObjectType[iTab][i].Data());
            if (tabObjectDisplayObjectType[iTab][i] == "ROMETGraph" ||
                tabObjectDisplayObjectType[iTab][i] == "ROMETCutG") {
               buffer.AppendFormatted("               if(static_cast<%s*>(objects->At(i))->GetN() == 0) {\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("                  static_cast<%s*>(objects->At(i))->Set(1);\n",
                                      tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("               }\n");
            }
            buffer.AppendFormatted("            }\n");
         }
         buffer.AppendFormatted("            static_cast<TNamed*>(objects->At(i))->SetTitle(static_cast<TNamed*>(static_cast<TObjArray*>(userObjects)->At(chn))->GetTitle());\n");
         buffer.AppendFormatted("            linearray = static_cast<TObjArray*>(static_cast<TObjArray*>(fLines->At(iDisplay))->At(i));\n");
         buffer.AppendFormatted("            nLines = linearray->GetEntriesFast();\n");
         buffer.AppendFormatted("            for (iLine = 0; iLine < TMath::Min(nLines,fNumberOfUserLines); iLine++) {\n");
         buffer.AppendFormatted("               line     = static_cast<TLine*>(linearray->At(iLine));\n");
         buffer.AppendFormatted("               userLine = static_cast<TLine*>(static_cast<TObjArray*>(static_cast<TObjArray*>(fUserLines->At(iDisplay))->At(chn))->At(iLine));\n");
         buffer.AppendFormatted("               line->SetX1       (userLine->GetX1());\n");
         buffer.AppendFormatted("               line->SetY1       (userLine->GetY1());\n");
         buffer.AppendFormatted("               line->SetX2       (userLine->GetX2());\n");
         buffer.AppendFormatted("               line->SetY2       (userLine->GetY2());\n");
         buffer.AppendFormatted("               line->SetLineColor(userLine->GetLineColor());\n");
         buffer.AppendFormatted("               line->SetLineStyle(userLine->GetLineStyle());\n");
         buffer.AppendFormatted("               line->SetLineWidth(userLine->GetLineWidth());\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("            if (!strcmp(objects->At(i)->ClassName(),\"ROMETGraph\")) {\n");
         buffer.AppendFormatted("               SetLimits(static_cast<ROMETGraph*>(objects->At(i)));\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("            if (!strcmp(objects->At(i)->ClassName(),\"ROMETCutG\")) {\n");
         buffer.AppendFormatted("               SetLimits(static_cast<ROMETCutG*>(objects->At(i)));\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("         } else {\n");
         buffer.AppendFormatted("            if (!strcmp(objects->At(i)->ClassName(),\"ROMETGraph\"))\n");
         buffer.AppendFormatted("               static_cast<ROMETGraph*>(objects->At(i))->Set(1);\n");
         buffer.AppendFormatted("            else if (!strcmp(objects->At(i)->ClassName(),\"ROMETCutG\"))\n");
         buffer.AppendFormatted("               static_cast<ROMETCutG*>(objects->At(i))->Set(1);\n");
         buffer.AppendFormatted("            else \n");
         buffer.AppendFormatted("               static_cast<TH1*>(objects->At(i))->Reset();\n");
         buffer.AppendFormatted("            static_cast<TNamed*>(objects->At(i))->SetTitle(\"\");\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   Modified(processEvents);\n");
      } else {
         buffer.AppendFormatted("   return;\n");
         buffer.AppendFormatted("   WarningSuppression(processEvents);\n");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Thread
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Bool_t %sT%s_Base::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)\n{\n",
                             shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   return RunProcessMessageThread(msg, param1, param2);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::ThreadProcessMessageThread(void* arg)\n{\n",
                             shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   static_cast<%sT%s_Base*>(static_cast<%sArgs*>(arg)->inst)->ProcessMessageThread(static_cast<%sArgs*>(arg)->msg, static_cast<%sArgs*>(arg)->param1, static_cast<%sArgs*>(arg)->param2);\n",
                             shortCut.Data(), tabName[iTab].Data(), tabName[iTab].Data(),
                             tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Bool_t %sT%s_Base::RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2)\n{\n",
                             shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   %sArgs* arg = new %sArgs();\n",
                             tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   arg->inst   = this;\n");
      buffer.AppendFormatted("   arg->msg    = msg;\n");
      buffer.AppendFormatted("   arg->param1 = param1;\n");
      buffer.AppendFormatted("   arg->param2 = param2;\n");
      buffer.AppendFormatted("   TThread* mProcessMessageThread = new TThread(\"processMessageThread\",static_cast<void(*) (void *)>(&ThreadProcessMessageThread), static_cast<void*>(arg));\n");
      buffer.AppendFormatted("   mProcessMessageThread->Run();\n");
      buffer.AppendFormatted("   return kTRUE;\n");
      buffer.AppendFormatted("}\n");
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("//%s\n", threadFunctionName[iTab][i].Data());
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sT%s_Base::%s(", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("%s", threadFunctionArgument[iTab][i][j].Data());
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted(")\n");
         buffer.AppendFormatted("{\n");
         // following lines should be modified to use ROMEPrint instead of iostream.
         buffer.AppendFormatted("   cout<<endl\n");
         buffer.AppendFormatted("       <<\" Thread function %s is not implemented.\"<<endl\n",
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
         buffer.AppendFormatted("       <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("       <<\"   void %s();\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("       <<\"   void %sT%s::%s()\"<<endl\n", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\"   {\"<<endl\n");
         buffer.AppendFormatted("       <<\"      cout<<\\\"Thread function %s is running.\\\"<<endl;\"<<endl\n",
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\"   }\"<<endl<<endl;\n");
         buffer.AppendFormatted("   Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sT%s_Base::Thread%s(void* arg)\n{\n", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   TThread::SetCancelOn();\n");
         buffer.AppendFormatted("   TThread::SetCancelDeferred();\n");
         buffer.AppendFormatted("   %sT%s_Base* inst = static_cast<%sT%s_Base*>(arg);\n", shortCut.Data(), tabName[iTab].Data(),
                                shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("   Int_t iLoop = 0;\n");
         //         buffer.AppendFormatted("   Int_t meid=TThread::SelfId(); // get pthread id\n");
         buffer.AppendFormatted("   while (inst->f%sActive) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::CancelPoint();\n");
         buffer.AppendFormatted("      inst->%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("inst->f%sArgument_%d", threadFunctionName[iTab][i].Data(), j);
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted("); // call the user defined threaded function\n");
         buffer.AppendFormatted("      if (inst->f%sNumberOfLoops != 0 && ++iLoop >= inst->f%sNumberOfLoops) {\n",
                                threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         inst->f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         if (inst->m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            TThread::Delete(inst->m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            inst->m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      gSystem->Sleep(inst->f%sInterval);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %sT%s_Base::Start%s(", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("%s arg%d,", threadFunctionArgument[iTab][i][j].Data(), j);
         buffer.AppendFormatted("Int_t interval, Int_t nloop)\n{\n");

         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("   f%sArgument_%d = arg%d;\n", threadFunctionName[iTab][i].Data(), j, j);
         buffer.AppendFormatted("   f%sActive        = kTRUE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sNumberOfLoops = nloop;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sInterval      = interval;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   if (!m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s = new TThread(\"Thread%s\",static_cast<void(*) (void *)>(&Thread%s),static_cast<void*>(this));\n",
                                threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(),
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s->Run();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %sT%s_Base::Stop%s()\n{\n", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   m%s->Join(); // wait a while for threads to halt\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   if (m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::Delete(m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("} \n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %sT%s_Base::Kill%s()\n{\n", shortCut.Data(), tabName[iTab].Data(),
                                threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   if (m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s->Kill();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("} \n");

         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("\n");

      // Menu Clicked
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::BaseMenuClicked(TGPopupMenu *menu,Long_t param)\n{\n", shortCut.Data(),
                             tabName[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         if (numOfTabObjectDisplays[iTab] > 0) {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
            buffer.AppendFormatted("   switch (param) {\n");
            for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
               if (tabObjectDisplayTaskHierarchyIndex[iTab][i] < 0) {
                  buffer.AppendFormatted("      case M_DISPLAY_%s:\n",   tabObjectDisplayName[iTab][i].ToUpper(str));
               } else {
                  buffer.AppendFormatted("      case M_DISPLAY_%s%s:\n", tabObjectDisplayName[iTab][i].ToUpper(str),
                                         taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               }
               buffer.AppendFormatted("         fDisplayObjIndex = %d;\n", i);
               buffer.AppendFormatted("         fNumberOfCurrentDisplayTypes = %d;\n", numOfTabObjectDisplayObjects[iTab][i]);
               buffer.AppendFormatted("         fCurrentDisplayType->Set(fNumberOfCurrentDisplayTypes);\n");
               for (j = 0; j < numOfTabObjectDisplayObjects[iTab][i]; j++) {
                  buffer.AppendFormatted("         fCurrentDisplayType->AddAt(%d, %d);\n",
                                         tabObjectDisplayObjectTypeIndex[iTab][i][j], j);
               }
               buffer.AppendFormatted("         SetupPads(fNumberOfPadsX,fNumberOfPadsY, true);\n");
               buffer.AppendFormatted("         fMenuDisplay->RCheckEntry(M_DISPLAY_%s",
                                      tabObjectDisplayName[iTab][i].ToUpper(str));
               if (tabObjectDisplayTaskHierarchyIndex[iTab][i] > -1) {
                  buffer.AppendFormatted(taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               }
               buffer.AppendFormatted(",M_DISPLAY_%s",tabObjectDisplayName[iTab][0].ToUpper(str1));
               if (tabObjectDisplayTaskHierarchyIndex[iTab][0] > -1) {
                  buffer.AppendFormatted(taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][0]].Data());
               }
               buffer.AppendFormatted(",M_DISPLAY_%s",
                                      tabObjectDisplayName[iTab][numOfTabObjectDisplays[iTab] - 1].ToUpper(str2));
               if (tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab] - 1] > -1) {
                  buffer.AppendFormatted(taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab] - 1]].Data());
               }
               buffer.AppendFormatted(");\n");
               buffer.AppendFormatted("         gAnalyzer->GetWindow()->RequestEventHandling();\n");
               buffer.AppendFormatted("         break;\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("   MenuClicked(menu,param);\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseMenuClicked(menu,param);\n");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Tab Selected
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::BaseTabSelected()\n{\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   SetForeground(kTRUE);\n");
      // Create build-in menus
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   fStyle->cd();\n");
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseTabSelected();\n");
      }

      // Display user menus
      if (tabHeredity[iTab].Length() > 0 || numOfMenu[iTab] > 0) {
         buffer.AppendFormatted("\n");
         if (tabHeredity[iTab].Length() > 0) {
            for (j = 0; j < numOfMenu[tabHeredityIndex[iTab]]; j++) {
               if (menuDepth[tabHeredityIndex[iTab]][j] == 1) {
                  buffer.AppendFormatted("   fWindow->GetMenuBar()->AddPopup(\"%s\", menu,\n", menuTitle[tabHeredityIndex[iTab]][j].Data());
                  buffer.AppendFormatted("                                   new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
               }
            }
         }
         for (j = 0; j < numOfMenu[tabHeredityIndex[iTab]]; j++) {
            if (menuDepth[iTab][j] == 1) {
               buffer.AppendFormatted("   fWindow->GetMenuBar()->AddPopup(\"%s\", GetUserPopupMenuAt(%d),\n", menuTitle[iTab][j].Data(), j);
               buffer.AppendFormatted("                                   new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
            }
         }
      }

      //
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   TabSelected();\n");
      buffer.AppendFormatted("   RegisterObjects(); // Check Task File for Folder/Histo/Graph access\n");
      buffer.AppendFormatted("   while (!RequestEvent()) {\n");
      buffer.AppendFormatted("      gSystem->Sleep(100);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   gAnalyzer->GetWindow()->RequestEventHandling();\n");
      buffer.AppendFormatted("   if (gAnalyzer->IsProgramTerminated()) {\n");
      buffer.AppendFormatted("      ArgusEventHandler();\n");
      buffer.AppendFormatted("   }\n");

      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Tab Unselected
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::BaseTabUnSelected()\n{\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   SetForeground(kFALSE);\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseTabUnSelected();\n");
      }
      buffer.AppendFormatted("   TabUnSelected();\n");
      // Check Task File for Folder/Histo/Graph access
      buffer.AppendFormatted("   UnRegisterObjects();\n");
      // Remove Popup
      ROMEString menu_title;
      if (tabHeredity[iTab].Length() > 0) {
         for (j = 0; j < numOfMenu[tabHeredityIndex[iTab]]; j++) {
            menu_title = menuTitle[tabHeredityIndex[iTab]][j];
            menu_title.ReplaceAll("&", "");
            buffer.AppendFormatted("   fWindow->GetMenuBar()->RemovePopup(\"%s\");\n", menu_title.Data());
         }
      }
      for (j = 0; j < numOfMenu[iTab]; j++) {
         menu_title = menuTitle[iTab][j];
         menu_title.ReplaceAll("&", "");
         buffer.AppendFormatted("   fWindow->GetMenuBar()->RemovePopup(\"%s\");\n", menu_title.Data());
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Register Objects
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::RegisterObjects()\n{\n", shortCut.Data(), tabName[iTab].Data());
      if (numOfFolder>0 || numOfTaskHierarchy>0) {
         buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor() && fRegisteringActive) {\n");
         for (j = 0; j < numOfFolder; j++) {
            if (accessFolder(tabFile.Data(),j) || accessFolderBuffer(buffer,j,tabFileBuffer.Data())) {
               buffer.AppendFormatted("      gAnalyzer->RegisterFolder(\"%s%s\");\n",shortCut.Data(),folderName[j].Data());
            }
         }
         for (j = 0; j < numOfTaskHierarchy; j++) {
            if (!taskUsed[taskHierarchyClassIndex[j]])
               continue;
            for (k = 0; k < numOfGraphs[taskHierarchyClassIndex[j]]; k++) {
               if (accessGraph(tabFile.Data(),taskHierarchyClassIndex[j],k) ||
                   accessGraphBuffer(buffer,taskHierarchyClassIndex[j], k,tabFileBuffer.Data())) {
                  buffer.AppendFormatted("      gAnalyzer->Get%s%sTaskBase()->Register%s();\n",
                                         taskHierarchyName[j].Data(),
                                         taskHierarchySuffix[j].Data(),graphName[taskHierarchyClassIndex[j]][k].Data());
               }
            }
         }
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // UnRegister Objects
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sT%s_Base::UnRegisterObjects()\n{\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor() && fRegisteringActive) {\n");
      tabFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      for (j = 0; j < numOfFolder; j++) {
         if (accessFolder(tabFile.Data(),j) || accessFolderBuffer(buffer,j,tabFileBuffer.Data())) {
            buffer.AppendFormatted("      gAnalyzer->UnRegisterFolder(\"%s%s\");\n",shortCut.Data(),folderName[j].Data());
         }
      }
      for (j = 0; j < numOfTaskHierarchy; j++) {
         if (!taskUsed[taskHierarchyClassIndex[j]])
            continue;
         for (k = 0; k < numOfGraphs[taskHierarchyClassIndex[j]]; k++) {
            if (accessGraph(tabFile.Data(),taskHierarchyClassIndex[j],k) ||
                accessGraphBuffer(buffer,taskHierarchyClassIndex[j], k,tabFileBuffer.Data())) {
               buffer.AppendFormatted("      gAnalyzer->Get%s%sTaskBase()->UnRegister%s();\n",
                                      taskHierarchyName[j].Data(),taskHierarchySuffix[j].Data(),
                                      graphName[taskHierarchyClassIndex[j]][k].Data());
            }
         }
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Write file
      WriteFile(cppFile.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteTabH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString fileBuffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString str;
   ROMEStrArray str1;
   ROMEStrArray str2;
   ROMEStrArray cond;
   TArrayI equal;

   Int_t i;
   if (makeOutput)
      cout<<"\n   Output H-Files:"<<endl;

   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;

      // User H-File
      hFile.SetFormatted("%sinclude/tabs/%sT%s.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      WriteHeader(buffer, numOfTabAuthors[iTab], tabAuthor[iTab], tabAuthorEmail[iTab], kFALSE);
      buffer.AppendFormatted("#ifndef %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#define %sT%s_H\n\n", shortCut.Data(), tabName[iTab].Data());
      clsName.SetFormatted("%sT%s", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = tabDescription[iTab].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kTRUE);
      buffer.AppendFormatted("\n\n");

      // Header
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("class %sWindow;\n", shortCut.Data());
      buffer.AppendFormatted("\n");

      // Class
      buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n", shortCut.Data(), tabName[iTab].Data(),
                             shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("\n");

      // Constructor
      buffer.AppendFormatted("private:\n");
      buffer.AppendFormatted("   %sT%s(const %sT%s &c); // not implemented\n",
                             shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   %sT%s &operator=(const %sT%s &c); // not implemented\n",
                             shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("public:\n");
      buffer.AppendFormatted("   %sT%s(%sWindow* window = 0):%sT%s_Base(window)\n", shortCut.Data(),
                             tabName[iTab].Data(), shortCut.Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual ~%sT%s()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   void Init();\n");
      buffer.AppendFormatted("   void EndInit();\n");
      buffer.AppendFormatted("   void MenuClicked(TGPopupMenu *menu,Long_t param);\n");
      buffer.AppendFormatted("   void TabSelected();\n");
      buffer.AppendFormatted("   void TabUnSelected();\n");
      buffer.AppendFormatted("   void EventHandler();\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);\n");
         buffer.AppendFormatted("   void Display(bool processEvents=true);\n");
      }

      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   void %s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("\n");

      // Fields
      buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)", shortCut.Data(), tabName[iTab].Data(),
                             tabVersion[iTab].Data());
      if (tabShortDescription[iTab].Length())
         buffer.AppendFormatted(" // %s", tabShortDescription[iTab].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File

      // Check for deprecated constructor
      fileBuffer.ReadFile(hFile.Data(), kTRUE);
      str.SetFormatted(" %sT%s()",shortCut.Data(),tabName[iTab].Data());
      if (fileBuffer.ContainsFast(str)) {
         str.SetFormatted("Please replace %sT%s():%sT%s_Base() with %sT%s(%sWindow* window):%sT%s_Base(window).",
                          shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),
                          tabName[iTab].Data(),shortCut.Data(),shortCut.Data(),tabName[iTab].Data());
         cout<<"The parameter list of the constructor of tabs has changed."<<endl;
         cout<<str.Data()<<endl<<endl;
      }
#if 0 // this is not necessary already, maybe.
      if (tabObjectDisplay[iTab]) {
         str1.RemoveAll();
         str2.RemoveAll();
         cond.RemoveAll();
         equal.Set(2);
         str1.AddFormatted("ClassDef(%sT%s",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("void Display(bool processEvents=true);\n   ClassDef(%sT%s",shortCut.Data(),
                           tabName[iTab].Data());
         cond.AddFormatted("void Display(bool processEvents=true);");
         equal.AddAt(0, 0);
         str1.AddFormatted("ClassDef(%sT%s",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);\n   ClassDef(%sT%s",shortCut.Data(),
                           tabName[iTab].Data());
         cond.AddFormatted("void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);");
         equal.AddAt(0, 1);
         ReplaceHeader(hFile.Data(), 0, buffer.Data(), 6,str1,str2,cond,equal);
      } else
         ReplaceHeader(hFile.Data(), 0, buffer.Data(), 6);
#else
      ReplaceHeader(hFile.Data(), 0, buffer.Data(), 6);
#endif
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteBaseTabH()
{
   Int_t i, j;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString hFile;
   ROMEString str;
   ROMEString str1;
   ROMEString str2;

   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;

      // File name
      hFile.SetFormatted("%sinclude/generated/%sT%s_Base.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      WriteHeader(buffer, numOfTabAuthors[iTab], tabAuthor[iTab], tabAuthorEmail[iTab], kTRUE);
      buffer.AppendFormatted("#ifndef %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("#define %sT%s_Base_H\n\n", shortCut.Data(), tabName[iTab].Data());
      clsName.SetFormatted("%sT%s_Base", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = tabDescription[iTab].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
      buffer.AppendFormatted("\n\n");

      // Header
      buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <TGMenu.h>\n");
      buffer.AppendFormatted("#include <TThread.h>\n");
      buffer.AppendFormatted("#include <TRootEmbeddedCanvas.h>\n");
      buffer.AppendFormatted("#include <TPad.h>\n");
      if (numOfTabSingleObjects[iTab] > 0) {
         buffer.AppendFormatted("#include <TH1.h>\n");
         buffer.AppendFormatted("#include <TH2.h>\n");
         buffer.AppendFormatted("#include <TH3.h>\n");
         buffer.AppendFormatted("#include <TProfile.h>\n");
         buffer.AppendFormatted("#include <TProfile2D.h>\n");
         buffer.AppendFormatted("#include <ROMETGraph.h>\n");
         buffer.AppendFormatted("#include <TGraph2D.h>\n");
         buffer.AppendFormatted("#include <ROMETCutG.h>\n");
      }
      if (numOfSteering[iTab+numOfTask + 1] > 0) {
         buffer.AppendFormatted("#include <TString.h>\n"); // TString is often used for steering parameter.
      }
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("#include \"ArgusHistoDisplay.h\"\n");
      } else {
         buffer.AppendFormatted("#include \"ArgusTab.h\"\n");
      }
      buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("class %sWindow;\n", shortCut.Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("struct %sArgs{\n", tabName[iTab].Data());
      buffer.AppendFormatted("   void*  inst;\n");
      buffer.AppendFormatted("   Long_t msg;\n");
      buffer.AppendFormatted("   Long_t param1;\n");
      buffer.AppendFormatted("   Long_t param2;\n");
      buffer.AppendFormatted("};\n");

      // Class
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("\nclass %sT%s_Base : public ArgusHistoDisplay\n", shortCut.Data(),
                                tabName[iTab].Data());
      } else if (tabHeredity[iTab].Length() > 0) {
         buffer.AppendFormatted("\nclass %sT%s_Base : public %sT%s\n", shortCut.Data(), tabName[iTab].Data(),
                                shortCut.Data(),tabHeredity[iTab].Data());
      } else {
         buffer.AppendFormatted("\nclass %sT%s_Base : public ArgusTab\n", shortCut.Data(), tabName[iTab].Data());
      }
      buffer.AppendFormatted("{\n");

      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   enum MenuEnumeration {\n");
         buffer.AppendFormatted("      M_DISPLAY_ROOT = 800,\n");
         for (i = 0; i < numOfTabObjectDisplays[iTab]; i++) {
            if (tabObjectDisplayTaskHierarchyIndex[iTab][i]<0) {
               buffer.AppendFormatted("      M_DISPLAY_%s,\n",tabObjectDisplayName[iTab][i].ToUpper(str));
            } else {
               buffer.AppendFormatted("      M_DISPLAY_%s%s,\n",tabObjectDisplayName[iTab][i].ToUpper(str),
                                      taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
            }
         }
         buffer = buffer(0, buffer.Length()-2);
         buffer.AppendFormatted("   \n};\n");
      }
      buffer.AppendFormatted("protected:\n");

      // Fields
      if (numOfSteering[iTab+numOfTask + 1] > 0) {
         WriteSteeringClass(buffer, 0, iTab+numOfTask + 1, 1);
         buffer.AppendFormatted("\n");
      }
      if (numOfSteering[iTab+numOfTask + 1] > 0) {
         buffer.AppendFormatted("   Steering* fSteering; //! Handle to Steering class\n\n");
      }
      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   TThread* m%s;         //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Bool_t   f%sActive;   //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sNumberOfLoops; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sInterval; //!\n", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("   %s f%sArgument_%d; //!\n", threadFunctionArgument[iTab][i][j].Data(),
                                   threadFunctionName[iTab][i].Data(), j);
      }
      // Single Objects
      if (numOfTabSingleObjects[iTab] > 0) {
         buffer.AppendFormatted("   TRootEmbeddedCanvas *fGeneratedCanvas; //!\n");
         for (i = 0; i < numOfTabSingleObjects[iTab]; i++) {
            if (tabSingleObjectType[iTab][i] == "Histogram") {
               if (histoArraySize[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]] == "1") {
                  buffer.AppendFormatted("   %s* f%sSingleObject%d; //!\n",
                                         histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),
                                         tabSingleObjectName[iTab][i].Data(),i);
                  buffer.AppendFormatted("   TPad* f%sPad%d; //!\n",tabSingleObjectName[iTab][i].Data(),i);
               } else {
                  for (j = tabSingleObjectArrayIndexStart[iTab][i]; j <= tabSingleObjectArrayIndexEnd[iTab][i]; j++) {
                     buffer.AppendFormatted("   %s* f%sSingleObject%d_%d; //!\n",
                                            histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),
                                            tabSingleObjectName[iTab][i].Data(),i,j);
                     buffer.AppendFormatted("   TPad* f%sPad%d_%d; //!\n",tabSingleObjectName[iTab][i].Data(),i,j);
                  }
               }
            } else if (tabSingleObjectType[iTab][i] == "Graph") {
               if (graphArraySize[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]] == "1") {
                  buffer.AppendFormatted("   %s* f%sSingleObject%d; //!\n",
                                         graphType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),
                                         tabSingleObjectName[iTab][i].Data(),i);
                  buffer.AppendFormatted("   TPad* f%sPad%d; //!\n",tabSingleObjectName[iTab][i].Data(),i);
               } else {
                  for (j = tabSingleObjectArrayIndexStart[iTab][i]; j <= tabSingleObjectArrayIndexEnd[iTab][i]; j++) {
                     buffer.AppendFormatted("   %s* f%sSingleObject%d_%d; //!\n",
                                            graphType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),
                                            tabSingleObjectName[iTab][i].Data(),i,j);
                     buffer.AppendFormatted("   TPad* f%sPad%d_%d; //!\n",tabSingleObjectName[iTab][i].Data(),i,j);
                  }
               }
            }
         }
      }

      // Methods
      // Constructor
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("private:\n");
      buffer.AppendFormatted("   %sT%s_Base(const %sT%s_Base &c); // not implemented\n",
                             shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   %sT%s_Base &operator=(const %sT%s_Base &c); // not implemented\n",
                             shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("public:\n");
      buffer.AppendFormatted("   %sT%s_Base(%sWindow* window);\n", shortCut.Data(), tabName[iTab].Data(),
                             shortCut.Data());
      buffer.AppendFormatted("   virtual ~%sT%s_Base();\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("\n");

      // InitTab
      buffer.AppendFormatted("   void BaseInit();\n");
      buffer.AppendFormatted("   virtual void Init() = 0;\n");
      buffer.AppendFormatted("   void EndInit() {}\n");
      buffer.AppendFormatted("\n");

      // Display
      buffer.AppendFormatted("   void BaseDisplay(bool processEvents=true);\n");
      if (tabObjectDisplay[iTab])
         buffer.AppendFormatted("   virtual void Display(bool processEvents=true) = 0;\n");
      buffer.AppendFormatted("\n");

      // BaseTabEventHandler
      buffer.AppendFormatted("   void BaseEventHandler();\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual void EventHandler() = 0;\n");
      buffer.AppendFormatted("\n");

      // Thread
      buffer.AppendFormatted("   virtual Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2);\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual Bool_t ProcessMessageThread(Long_t /*msg*/, Long_t /*param1*/, Long_t /*param2*/) {return kTRUE;}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   static void ThreadProcessMessageThread(void* arg);\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   Bool_t RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2);\n");
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   //%s\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   virtual void %s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("%s", threadFunctionArgument[iTab][i][j].Data());
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted(");\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   static void Thread%s(void* arg);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Start%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("%s arg%d,", threadFunctionArgument[iTab][i][j].Data(), j);
         buffer.AppendFormatted("Int_t interval = 1000, Int_t nloop = 0);\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Bool_t Kill%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("\n");

      // Register
      buffer.AppendFormatted("   void RegisterObjects();\n");
      buffer.AppendFormatted("   void UnRegisterObjects();\n");

      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTab+numOfTask + 1] > 0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() const { return fSteering; }\n");
         buffer.AppendFormatted("   Steering* GetSP() const { return fSteering; }\n");
      }

      // Tab Selected
      buffer.AppendFormatted("   void         BaseTabSelected();\n");
      buffer.AppendFormatted("   virtual void TabSelected() = 0;\n");
      // Tab Unselected
      buffer.AppendFormatted("   void         BaseTabUnSelected();\n");
      buffer.AppendFormatted("   virtual void TabUnSelected() = 0;\n");

      buffer.AppendFormatted("\n");
      // Menu Clicked
      buffer.AppendFormatted("   void         BaseMenuClicked(TGPopupMenu *menu,Long_t param);\n");
      buffer.AppendFormatted("   virtual void MenuClicked(TGPopupMenu *menu,Long_t param) = 0;\n");
      buffer.AppendFormatted("\n");


      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s) // Base class of %sT%s\n", shortCut.Data(),
                             tabName[iTab].Data(), tabVersion[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File
      WriteFile(hFile.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteSteering(Int_t iTask)
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString tmp;
   Int_t i;

   hFile.SetFormatted("%sinclude/generated/%sGlobalSteering.h",outDir.Data(),shortCut.Data());

   if (numOfSteering[numOfTask] == -1) {
      remove(hFile.Data());
      return true;
   }

   if (makeOutput) cout<<"\n   Output Files:"<<endl;

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sGlobalSteering_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sGlobalSteering_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sGlobalSteering", shortCut.Data());
   clsDescription.SetFormatted("Global steering parameter class for %s", experimentName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <Rtypes.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TString.h>\n"); // TString is often used for steering parameter.
   // includes
   for (i = 0; i < numOfGSPInclude; i++) {
      if (gspLocalFlag[i]) {
         tmp.SetFormatted("#include \"%s\"",gspInclude[i].Data());
      } else {
         tmp.SetFormatted("#include <%s>",gspInclude[i].Data());
      }
      if (!buffer.ContainsFast(tmp))
         buffer.AppendFormatted("%s\n",tmp.Data());
   }
   // Folder includes
#if 0 // this is not necessary, probably
   Int_t j = 0;
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i]) {
         for (j = 0; j < numOfFolderInclude[i]; j++) {
            if (folderLocalFlag[i][j]) {
               tmp.SetFormatted("#include \"%s\"",folderInclude[i][j].Data());
            } else {
               tmp.SetFormatted("#include <%s>",folderInclude[i][j].Data());
            }
            if (!buffer.ContainsFast(tmp))
               buffer.AppendFormatted("%s\n",tmp.Data());
         }
      }
   }
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

   WriteSteeringClass(buffer, 0, iTask, 0);

   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());

   //Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteAnalyzerCpp()
{
   int i,j,k,idx;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;
   ROMEString buf;
   ROMEString str;
   ROMEString pointer;
   bool treeFolder;

   if (makeOutput) cout<<"\n   Output Cpp-File:"<<endl;

   ROMEString tmp, tmp2;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i]) {
         continue;
      }
      if (!folderSupport[i]) {
         if (typeLen < static_cast<int>(folderName[i].Length() + scl)) {
            typeLen = folderName[i].Length() + scl;
         }
         if (nameLen < static_cast<int>(folderName[i].Length())) {
            nameLen = folderName[i].Length();
         }
      }
   }

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",
                               shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <direct.h>\n");
#endif
   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TSystem.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sNetFolderServer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMERint.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"ROMEStr2DArray.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAllFolders.h\"\n",shortCut.Data());
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\n");

   // If there is any dependencies, add all "#include" for base tab classes, even if the tab has no dependencies
   for (i = 0; i < numOfTab; i++) {
      if (tabUsed[i] && tabDependence[i].Length()) {
         for (j = 0; j < numOfTab; j++) {
            buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[j].Data());
         }
         buffer.AppendFormatted("\n");
         break;
      }
   }

   buffer.AppendFormatted("ClassImp(%sAnalyzer)\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sAnalyzer *gAnalyzer;  // global Analyzer Handle\n",shortCut.Data());

   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("void CreateSplash(unsigned long time,char*,char*,ROMEString*,int);\n");
   buffer.AppendFormatted("DWORD WINAPI SplashThread ( LPVOID lpvoid) {\n");
   bool same = false;
   int numAuthors = 1;
   ROMEString authors[50];
   authors[0] = mainAuthor->Data();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      same = false;
      for (j = 0; j < numAuthors; j++) {
         if (!authors[j].CompareTo(folderAuthor[i]->Data(),TString::kIgnoreCase)) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = folderAuthor[i]->Data();
         numAuthors++;
      }
   }
   for (i = 0; i < numOfTask; i++) {
      same = false;
      for (j = 0; j < numAuthors; j++) {
         if (!authors[j].CompareTo(taskAuthor[i]->Data(),TString::kIgnoreCase)) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = taskAuthor[i]->Data();
         numAuthors++;
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      same = false;
      for (j = 0; j < numAuthors; j++) {
         if (!authors[j].CompareTo(tabAuthor[i]->Data(),TString::kIgnoreCase)) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = tabAuthor[i]->Data();
         numAuthors++;
      }
   }
   buffer.AppendFormatted("   ROMEString *authors = new ROMEString[%d];\n",numAuthors);
   for (i = 0; i < numAuthors; i++) {
      buffer.AppendFormatted("   authors[%d] = \"%s\";\n",i,authors[i].Data());
   }
   buffer.AppendFormatted("   CreateSplash(3,\"%s%s\",\"%s\",authors,%d);\n",shortCut.Data(),mainProgName.Data(),
                          romeVersion.Data(),numAuthors);
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
#endif
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sAnalyzer::%sAnalyzer(ROMERint *app,Bool_t batch,Bool_t daemon,Bool_t nographics,Int_t mode)\n",
                          shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(":ROMEAnalyzer(app,batch,daemon,nographics,mode");
   buffer.AppendFormatted(",\"%s%s\"",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted(",\"Analyzer\"");
   buffer.AppendFormatted(",new %sConfig()",shortCut.Data());
   buffer.AppendFormatted(",%d", numOfNetFolder);
   buffer.AppendFormatted(")\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            if (folderArray[i] == "1") {
               buffer.AppendFormatted(",f%sFolder(0)\n",folderName[i].Data());
               buffer.AppendFormatted(",f%sFolderStorage(0)\n",folderName[i].Data());
            } else {
               buffer.AppendFormatted(",f%sFolders(0)\n",folderName[i].Data());
               buffer.AppendFormatted(",f%sFoldersStorage(0)\n",folderName[i].Data());
            }
         }
      }
   }
   if (numOfSteering[numOfTask] > 0) {
      buffer.AppendFormatted(",fGlobalSteeringParameters(new %sGlobalSteering())\n",shortCut.Data());
   }
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted(",f%sDAQ(0)\n",daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted(",fDBAccess(new %sDBAccess())\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fMaxEventID = %d;\n", maxEventID);
   buffer.AppendFormatted("   fStatistics = new Statistics[fMaxEventID];\n");
   buffer.AppendFormatted("   memset(fStatistics, 0, sizeof(Statistics) * fMaxEventID);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("// Folder and Task initialisation\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gAnalyzer = this;\n");
   buffer.AppendFormatted("   gROME = static_cast<ROMEAnalyzer*>(this);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (!isNoGraphics()) {\n");
   buffer.AppendFormatted("      fWindow = new %sWindow(gClient->GetRoot(),kTRUE);\n",shortCut.Data());
   buffer.AppendFormatted("   } else {\n");
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,4))
   // Due to change at revision 1.29 of TGWindow.cxx, there is a problem to create TGCompositFrame in batch mode.
   // Following line is a work around to avoid the problem.
   buffer.AppendFormatted("      if (!gClient) new TGClient();\n");
#endif
   buffer.AppendFormatted("      fWindow = new %sWindow();\n",shortCut.Data());
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   ROMEString name;\n");
   buffer.AppendFormatted("   name.SetFormatted(\"ARGUS - %%s\", GetProgramName());\n");
   buffer.AppendFormatted("   fWindow->SetWindowName(name.Data());\n");
   buffer.AppendFormatted("\n");

   // Folder
   buffer.AppendFormatted("   // Folder initialisation\n");
   buffer.AppendFormatted("   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n",
                          shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (folderParentName[i] == "GetMainFolder()") {
            parentt = folderParentName[i];
         } else {
            parentt.SetFormatted("%sFolder",folderParentName[i].Data());
         }
         buffer.AppendFormatted("   TFolder* %sFolder%*s = %s->AddFolder(\"%s\",\"%s\");\n",
                                folderName[i].Data(), nameLen - folderName[i].Length(), "",
                                parentt.Data(), folderName[i].Data(), folderTitle[i].Data());
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            buffer.AppendFormatted("\n");
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("   f%sFolder = new %s%s();\n",folderName[i].Data(),shortCut.Data(),
                                      folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolder);\n",folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFolderStorage = new %s%s();\n",folderName[i].Data(),shortCut.Data(),
                                      folderName[i].Data());
            } else {
               buffer.AppendFormatted("   f%sFolders = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),
                                      shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFolders->SetName(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),
                                      folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolders);\n",folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFoldersStorage = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),
                                      shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFoldersStorage->SetName(\"%s%s\");\n",folderName[i].Data(),
                                      shortCut.Data(), folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   InitTasks();\n");
   buffer.AppendFormatted("\n");

   // NetFolder
   buffer.AppendFormatted("   InitNetFolders(fNumberOfNetFolders);\n");
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("   SetNetFolderName(%d,\"%s\");\n", i, netFolderName[i].Data());
      buffer.AppendFormatted("   SetNetFolderRoot(%d,\"%s\");\n", i, shortCut.Data());
   }
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   // %s\n", netFolderName[i].Data());
      buffer.AppendFormatted("   fNetFolderName[%d]   = \"%s\";\n", i, netFolderName[i].Data());
      buffer.AppendFormatted("   fNetFolderActive[%d] = kFALSE;\n", i);
      buffer.AppendFormatted("   fNetFolderReconnect[%d] = kTRUE;\n", i);
      buffer.AppendFormatted("   fNetFolder[%d]       = 0;\n", i);
      buffer.AppendFormatted("   fNetFolderSocket[%d] = 0;\n", i);
      if (netFolderHost[i].Length()) {
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"%s\";\n", i, netFolderHost[i].Data());
      } else {
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"localhost\";\n", i);
      }
      if (netFolderPort[i].Length()) {
         buffer.AppendFormatted("   fNetFolderPort[%d]   = %s;\n", i, netFolderPort[i].Data());
      } else {
         buffer.AppendFormatted("   fNetFolderPort[%d]   = 9090;\n", i);
      }
      if (netFolderRoot[i].Length()) {
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n", i, netFolderRoot[i].Data());
      } else {
//         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"histos\";\n",i);
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n", i, shortCut.Data());
      }
      buffer.AppendFormatted("\n");
   }
   // End of Constructor
   buffer.AppendFormatted("}\n\n");

   // Destructor
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sAnalyzer::~%sAnalyzer()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   // Folder Fields
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      treeFolder = false;
      for (j = 0; j < numOfTree && !treeFolder; j++) {
         for (k = 0; k < numOfBranch[j] && !treeFolder; k++) {
            if (branchFolder[j][k] == folderName[i])
               treeFolder = true;
         }
      }
      for (j = 0; j < numOfTree && !treeFolder; j++) {
         for (k = 0; k < numOfRunHeader[j] && !treeFolder; k++) {
            if (runHeaderFolder[j][k] == folderName[i])
               treeFolder = true;
         }
      }
      if (!folderSupport[i] && !treeFolder) {
         if (FolderToBeGenerated(i)) {
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("   SafeDelete(f%sFolder);\n",folderName[i].Data());
               buffer.AppendFormatted("   SafeDelete(f%sFolderStorage);\n",folderName[i].Data());
            } else {
               buffer.AppendFormatted("   SafeDelete(f%sFolders);\n",folderName[i].Data());
               buffer.AppendFormatted("   SafeDelete(f%sFoldersStorage);\n",folderName[i].Data());
            }
         }
      }
   }
   if (numOfSteering[numOfTask] > 0) {
      buffer.AppendFormatted("   SafeDelete(fGlobalSteeringParameters);\n");
   }
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("   SafeDelete(f%sDAQ);\n",daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("   SafeDelete(fWindow);\n");
   buffer.AppendFormatted("   SafeDelete(fConfiguration);\n");
   buffer.AppendFormatted("   SafeDelete(fDBAccess);\n");
   buffer.AppendFormatted("   gAnalyzer = 0;\n");
   // End of Destructor
   buffer.AppendFormatted("}\n\n");

   // Folder Getters and Setters
   buffer.AppendFormatted("   // Folders\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      WriteFolderGetterSource(buffer,i);
      WriteFolderSetterSource(buffer,i);
   }
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::FolderArrayOutOfBouds(Int_t index,const char* folder,const char* arraySize) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEPrint::Error(\"\\nYou have tried to access the %%d th element of the array folder %%s\\nwhich was defined with array size %%s.\\n\\nShutting down the program.\\n\",index,folder,arraySize);\n");
   buffer.AppendFormatted("   gSystem->StackTrace();\n");
   buffer.AppendFormatted("   fApplication->Terminate(1);\n");
   buffer.AppendFormatted("   return;\n");
   buffer.AppendFormatted("}\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::RegisterFolder(const char* object)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (IsROMEMonitor())\n");
   buffer.AppendFormatted("      return GetSocketClientNetFolder()->RegisterObject(object);\n");
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::UnRegisterFolder(const char* object)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (IsROMEMonitor())\n");
   buffer.AppendFormatted("      GetSocketClientNetFolder()->UnRegisterObject(object);\n");
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   // Set size
   ROMEString separator;
   for (i = 0; i < numOfFolder; i++) {
      separator = "";
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i] && FolderToBeGenerated(i) && folderArray[i] != "1") {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("void %sAnalyzer::Set%sSize(Int_t number)\n{\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("   if (!f%sFolders || number < 0)\n",folderName[i].Data());
         buffer.AppendFormatted("      return;\n");
         buffer.AppendFormatted("   Int_t n = f%sFolders->GetEntriesFast();\n", folderName[i].Data());
         buffer.AppendFormatted("   if (n == number) {\n");
         buffer.AppendFormatted("      return;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   Int_t i;\n");
         buffer.AppendFormatted("   for (i = number; i < n; i++) {\n");
         buffer.AppendFormatted("      f%sFolders->RemoveAt(i);\n", folderName[i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   f%sFolders->ExpandCreate(number);\n", folderName[i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }
   buffer.AppendFormatted("\n");

   // GetDBPathWriteFlag
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::GetDBPathWriteFlag(const char* path) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   Int_t i,num;\n");
   buffer.AppendFormatted("   ROMEString subStr;\n");
   buffer.AppendFormatted("   ROMEString str = path;\n");
   buffer.AppendFormatted("   Int_t ind = str.Last('\"');\n");
   buffer.AppendFormatted("   if (ind > -1 && ind < str.Length() - 1) {\n");
   buffer.AppendFormatted("      str = str(ind + 2,str.Length() - ind + 2);\n");
   buffer.AppendFormatted("      num = str.CountChar(',') + 1;\n");
   buffer.AppendFormatted("      for (i = 0; i < num; i++) {\n");
   buffer.AppendFormatted("         if (i < num - 1)\n");
   buffer.AppendFormatted("            ind = str.First(',');\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            ind = str.Length();\n");
   buffer.AppendFormatted("         subStr = str(0, ind);\n");
   buffer.AppendFormatted("         if (GetObjectInterpreterCode(subStr.Data()) == -2)\n");
   buffer.AppendFormatted("            return kFALSE;\n");
   buffer.AppendFormatted("         str = str(ind + 1, str.Length() - ind + 1);\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ReadUserParameter
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadUserParameter(const char* opt, const char* value, Int_t& i)\n{\n",
                          shortCut.Data());

   // Global Steering Parameter
   buffer.AppendFormatted("   ROMEString option = opt;\n");
   buffer.AppendFormatted("   ROMEString tmp;\n");
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop=0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTask] > 0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("fGlobalSteering");
      steerPointerT.SetFormatted("GetGSP()");
      WriteSteeringReadParameters(buffer, 0, numOfTask,pointerT,steerPointerT);
   }

   // Task steering parameter
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      idx = i;
      pointer.Resize(0);
      while (idx != -1) {
         pointer.InsertFormatted(0, "->f%sTask",taskHierarchyName[idx].Data());
         idx = taskHierarchyParentIndex[idx];
      }
      if (numOfSteering[taskHierarchyClassIndex[i]] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("static_cast<%sT%s_Base*>(GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),
                                    taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         WriteSteeringReadParameters(buffer, 0, taskHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }

   // Tab steering parameter
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      idx = i;
      pointer.Resize(0);
      while (idx != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[idx].Data());
         idx = tabParentIndex[idx];
      }
      if (numOfSteering[i+numOfTask + 1] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("static_cast<%sT%s_Base*>(gAnalyzer->GetWindow()->GetTabObjectAt(%d))->GetSP()", shortCut.Data(), tabName[i].Data(), tabUsedIndex[i]);
         WriteSteeringReadParameters(buffer, 0, i+numOfTask + 1, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("   WarningSuppression(value);\n");
   buffer.AppendFormatted("   WarningSuppression(i);\n");
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n\n");

   // UserParameterUsage
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::UserParameterUsage() const\n{\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTask] > 0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("GetGSP()");
      WriteSteeringParameterUsage(buffer, 0, numOfTask,pointerT,steerPointerT);
   }
   // Task steering parameter
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      idx = i;
      pointer.Resize(0);
      while (idx != -1) {
         pointer.InsertFormatted(0, "->f%sTask",taskHierarchyName[idx].Data());
         idx = taskHierarchyParentIndex[idx];
      }
      if (numOfSteering[taskHierarchyClassIndex[i]] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("static_cast<%sT%s_Base*>(GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),
                                    taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         WriteSteeringParameterUsage(buffer, 0, taskHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }
   // Tab steering parameter
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      idx = i;
      pointer.Resize(0);
      while (idx != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[idx].Data());
         idx = tabParentIndex[idx];
      }
      if (numOfSteering[i+numOfTask + 1] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("static_cast<%sT%s_Base*>(gAnalyzer->GetWindow()->GetTabObjectAt(%d))->GetSP()", shortCut.Data(), tabName[i].Data(), tabUsedIndex[i]);
         WriteSteeringParameterUsage(buffer, 0, i+numOfTask + 1, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Tasks and tabs dependences
   buffer.AppendFormatted("// Tasks and tabs dependenes\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::CheckDependences() const\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Bool_t ret = kTRUE;\n");

   buffer.AppendFormatted("   Bool_t analyzer;\n");
   buffer.AppendFormatted("   Bool_t monitor;\n");

   buffer.AppendFormatted("   switch(fProgramMode) {\n");
   buffer.AppendFormatted("   case kStandAloneROME:\n");
   buffer.AppendFormatted("      analyzer = kTRUE;\n");
   buffer.AppendFormatted("      monitor = kFALSE;\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   case kStandAloneARGUS:\n");
   buffer.AppendFormatted("      analyzer = kFALSE;\n");
   buffer.AppendFormatted("      monitor = kTRUE;\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   case kROMEAndARGUS:\n");
   buffer.AppendFormatted("      analyzer = kTRUE;\n");
   buffer.AppendFormatted("      monitor = kTRUE;\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   case kROMEMonitor:\n");
   buffer.AppendFormatted("      analyzer = kTRUE;\n");
   buffer.AppendFormatted("      monitor = kTRUE;\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   default:\n");
   buffer.AppendFormatted("      analyzer = kFALSE;\n");
   buffer.AppendFormatted("      monitor = kFALSE;\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]] || !taskDependence[taskHierarchyClassIndex[i]].Length())
         continue;
      tmp2.SetFormatted("Task(%s)",taskHierarchyName[i].Data());
      ParseDependences(tmp2, tmp);
      buffer.AppendFormatted("   if ((%s) &&\n",tmp.Data());
      ParseDependences(taskDependence[taskHierarchyClassIndex[i]], tmp);
      buffer.AppendFormatted("       !(%s)) {\n", tmp.Data());
      buffer.AppendFormatted("      cerr<<\"Error: Dependence for the '%s' task is not satisfied.\"<<endl;\n",
                             taskHierarchyName[i].Data());
      buffer.AppendFormatted("      cerr<<\"       '%s'\"<<endl<<endl;\n",
                             taskDependence[taskHierarchyClassIndex[i]].Data());
      buffer.AppendFormatted("      ret = kFALSE;\n");
      buffer.AppendFormatted("   }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i] || !tabDependence[i].Length())
         continue;
      tmp2.SetFormatted("Tab(%s)", tabName[i].Data());
      ParseDependences(tmp2, tmp);
      buffer.AppendFormatted("   if ((%s) &&\n",tmp.Data());
      ParseDependences(tabDependence[i], tmp);
      buffer.AppendFormatted("       !(%s)) {\n", tmp.Data());
      buffer.AppendFormatted("      cerr<<\"Error: Dependence for the '%s' tab is not satisfied.\"<<endl;\n",
                             tabName[i].Data());
      buffer.AppendFormatted("      cerr<<\"       '%s'\"<<endl<<endl;\n", tabDependence[i].Data());
      buffer.AppendFormatted("      ret = kFALSE;\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   if (!ret)\n");
   buffer.AppendFormatted("      cerr<<\"Please modify your configuration XML file.\"<<endl;\n");
   buffer.AppendFormatted("   return ret;\n");
   buffer.AppendFormatted("}\n\n");

   // Update ConfigParameters
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::UpdateConfigParameters()\n{\n", shortCut.Data());
   if (hasDependenceCheck || mainDefinitionVersion != "1") {
      // Task active flag
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   f%sConfigParametersFolder->Set%s%sTaskActive(static_cast<TTask*>(fTaskObjects->At(%d))->IsActive());\n",
                                mainProgName.Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                taskHierarchyObjectIndex[i]);
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Check tree filename
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::CheckTreeFileNames() const\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("// Check tree filenames\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   if (!static_cast<ROMETree*>(fTreeObjects->At(%d))->CheckConfiguration(fInputDir.Data(), fOutputDir.Data()))\n",
                             i);
      buffer.AppendFormatted("      return kFALSE;\n");
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n\n");

   // DAQ Access Methods
   buffer.AppendFormatted("// Deprecated DAQ Access Methods\n");
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("// %s DAQ Access Methods\n",daqNameArray->At(i).Data());
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("%s%sDAQ* %sAnalyzer::Get%sDAQ() const\n{\n",daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data(),shortCut.Data(),daqNameArray->At(i).Data());
      buffer.AppendFormatted("   if (f%sDAQ == 0) {\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("      ROMEPrint::Error(\"\\nYou have tried to access the %s DAQ system over a gAnalyzer->Get%sDAQ()\\nhandle but the current DAQ system is not '%s'.\\n\\nShutting down the program.\\n\");\n",
                             daqNameArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
      buffer.AppendFormatted("      gSystem->StackTrace();\n");
      buffer.AppendFormatted("      fApplication->Terminate(1);\n");
      buffer.AppendFormatted("      return 0;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return f%sDAQ;\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("}\n\n");
   }

   // Histos
   buffer.AppendFormatted("// Histos\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("TH1* %sAnalyzer::GetHisto(const char* pathToHisto) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   return static_cast<TH1*>(gROOT->FindObjectAny(pathToHisto));\n");
   buffer.AppendFormatted("}\n\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("bool %sAnalyzer::ResetAllHistos()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   Int_t iTask;\n");
   buffer.AppendFormatted("   const Int_t nTasks = fTaskObjects->GetEntriesFast();\n");
   buffer.AppendFormatted("   for (iTask = 0; iTask < nTasks; iTask++) {\n");
   buffer.AppendFormatted("      static_cast<ROMETask*>(fTaskObjects->At(iTask))->ResetHisto();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Graphs
   buffer.AppendFormatted("// Graphs\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("TH1* %sAnalyzer::GetGraph(const char* pathToGraph) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   return static_cast<TH1*>(gROOT->FindObjectAny(pathToGraph));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("bool %sAnalyzer::ResetAllGraphs()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   Int_t iTask;\n");
   buffer.AppendFormatted("   const Int_t nTasks = fTaskObjects->GetEntriesFast();\n");
   buffer.AppendFormatted("   for (iTask = 0; iTask < nTasks; iTask++) {\n");
   buffer.AppendFormatted("      static_cast<ROMETask*>(fTaskObjects->At(iTask))->ResetGraph();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Connect Socket Client NetFolder
   buffer.AppendFormatted("// Connect Socket Client NetFolder\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::ConnectSocketClientNetFolder(TSocket *sock)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   SafeDelete(fSocketClientNetFolder);\n");
   buffer.AppendFormatted("   fSocketClientNetFolder = new ROMENetFolder(\"%s\",\"RootNetFolder\", sock, true);\n",
                          shortCut.Data());
   buffer.AppendFormatted("   if (!fSocketClientNetFolder->GetPointer()) {\n");
   buffer.AppendFormatted("      Warning(\"ConnectSocketClientNetFolder\", \"Failed to connect to the ROME analyzer.\");\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Splash Screen
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("LPDWORD lpThreadId;\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   CloseHandle(CreateThread(0, 1024,&SplashThread, 0, 0, lpThreadId));\n");
   buffer.AppendFormatted("}\n");
#endif
   buffer.AppendFormatted("\n");
#if defined( R__UNIX )
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   \n");
   buffer.AppendFormatted("}\n");
#endif
   buffer.AppendFormatted("\n");

   // Folder dump and load
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::DumpFolders(const char* filename, Bool_t only_database) const\n{\n",
                          shortCut.Data());
   buffer.AppendFormatted("   if(!filename) return kFALSE;\n");
   buffer.AppendFormatted("   TFile out(filename, \"RECREATE\");\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            if (!folderDataBase[i]) {
               buffer.AppendFormatted("   if (!only_database) {\n");
            } else {
               buffer.AppendFormatted("   {\n");
            }
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("      out.WriteTObject(f%sFolder, \"%s\");\n",folderName[i].Data(),
                                      folderName[i].Data());
            } else {
               buffer.AppendFormatted("      out.WriteTObject(f%sFolders, \"%ss\", \"SingleKey\");\n",
                                      folderName[i].Data(),folderName[i].Data());
            }
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("   out.Write();\n");
   buffer.AppendFormatted("   out.Close();\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("   WarningSuppression(only_database);\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::LoadFolders(const char* filename, Bool_t only_database)\n{\n",
                          shortCut.Data());
   buffer.AppendFormatted("   if(!filename) return kFALSE;\n");
   buffer.AppendFormatted("   TFile in(filename);\n");
   buffer.AppendFormatted("   if(in.IsZombie()) {\n");
   buffer.AppendFormatted("      Error(\"LoadFolders\", \"%%s not found\", filename);\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            if (!folderDataBase[i]) {
               buffer.AppendFormatted("   if (!only_database) {\n");
            } else {
               buffer.AppendFormatted("   {\n");
            }
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("      %s%s* p%s = static_cast<%s%s*>(in.FindObjectAny(\"%s\"));\n",shortCut.Data(),
                                      folderName[i].Data(),folderName[i].Data()
                                      ,shortCut.Data(),folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      if (p%s) {\n",folderName[i].Data());
               buffer.AppendFormatted("         SafeDelete(f%sFolder);\n",folderName[i].Data());
               buffer.AppendFormatted("         f%sFolder = static_cast<%s%s*>(p%s->Clone(p%s->GetName()));\n",
                                      folderName[i].Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),
                                      folderName[i].Data());
               buffer.AppendFormatted("      }\n");
            } else {
               buffer.AppendFormatted("      TClonesArray *p%s = static_cast<TClonesArray*>(in.FindObjectAny(\"%ss\"));\n",
                                      folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      if (p%s) {\n",folderName[i].Data());
               buffer.AppendFormatted("          SafeDelete(f%sFolders);\n",folderName[i].Data());
               buffer.AppendFormatted("          f%sFolders = static_cast<TClonesArray*>(p%s->Clone(p%s->GetName()));\n",
                                      folderName[i].Data(),folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      }\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("   WarningSuppression(only_database);\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Start NetFolder server
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::StartNetFolderServer()\n{\n", shortCut.Data());
   buffer.AppendFormatted("   if(fNetFolderServer) {\n");
   buffer.AppendFormatted("      Error(\"StartNetFolderServer\", \"Another server is already running.\");\n");
   buffer.AppendFormatted("      return;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   fNetFolderServer = new %sNetFolderServer();\n", shortCut.Data());
   buffer.AppendFormatted("   static_cast<%sNetFolderServer*>(fNetFolderServer)->StartServer(GetApplication(),GetSocketServerPortNumber(),\"ROME\");\n",
                          shortCut.Data());
   buffer.AppendFormatted("   ROMEPrint::Print(\"Root server listening on port %%d\\n\", GetSocketServerPortNumber());\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Fill Config Parameters Folder
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::FillConfigParametersFolder()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   // Config file name and content
   buffer.AppendFormatted("   f%sConfigParametersFolder->SetConfigFileName(fConfiguration->GetConfigFileName());\n",
                          mainProgName.Data());
   buffer.AppendFormatted("   f%sConfigParametersFolder->SetConfigString(fConfiguration->GetConfigContent());\n",
                          mainProgName.Data());
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Save Config Parameters Folder
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::SaveConfigParametersFolder() const\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   f%sConfigParametersFolder->Write(\"%sConfigParameters\", TObject::kOverwrite);\n",
                          mainProgName.Data(),mainProgName.Data());
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Fill Object Storage
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::FillObjectStorage()\n{\n", shortCut.Data());
   buffer.AppendFormatted("   //create a buffer where the object will be streamed\n");
   buffer.AppendFormatted("   TFile *filsav = gFile;\n");
   buffer.AppendFormatted("   gFile = 0;\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   i = 0;\n");
   buffer.AppendFormatted("   const Int_t bufsize = 10000;\n");
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0))
   buffer.AppendFormatted("   TBufferFile *buffer = new TBufferFile(TBuffer::kWrite,bufsize);\n");
#else
   buffer.AppendFormatted("   TBuffer *buffer = new TBuffer(TBuffer::kWrite,bufsize);\n");
#endif
   buffer.AppendFormatted("   Bool_t bypassOld;\n");
   buffer.AppendFormatted("   bypassOld = kFALSE;\n"); // to suppress unused warning
   buffer.AppendFormatted("   Bool_t bypassStorageOld;\n");
   buffer.AppendFormatted("   bypassStorageOld = kFALSE;\n"); // to suppress unused warning
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderSupport[i])
         continue;
      if (FolderToBeGenerated(i)) {
         if (folderArray[i] == "1") {
            WriteFillObjectStorageObject(buffer,"f" + folderName[i] + "Folder",
                                         "f" + folderName[i] + "FolderStorage",
                                         "static_cast<" + shortCut + "NetFolderServer*>(fNetFolderServer)->Get" + folderName[i] +
                                         "FolderActive(i)",false);
         } else {
            WriteFillObjectStorageObject(buffer,"f" + folderName[i] + "Folders", "f" + folderName[i] + "FoldersStorage",
                                         "static_cast<" + shortCut + "NetFolderServer*>(fNetFolderServer)->Get" + folderName[i] +
                                         "FolderActive(i)", true);
         }
      }
   }
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (numOfGraphs[taskHierarchyClassIndex[i]] > 0) {
         buffer.AppendFormatted("   if (GetTaskObjectAt(%d)->IsActive()",i);
         idx = taskHierarchyParentIndex[i];
         while (idx != -1) {
            buffer.AppendFormatted(" && GetTaskObjectAt(%d)->IsActive()",i);
            idx = taskHierarchyParentIndex[idx];
         }
         buffer.AppendFormatted(") {\n");
      }
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         ROMEString temp1,temp2,temp3;
         temp1.SetFormatted("GetTaskObjectAt(%d)->GetGraphAt(%d)",i,j); 
         temp2.SetFormatted("GetTaskObjectAt(%d)->GetGraphStorageAt(%d)",i,j); 
         temp3.SetFormatted("static_cast<%sNetFolderServer*>(fNetFolderServer)->Get%s%s_%sGraphActive(i)",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data()); 
         WriteFillObjectStorageObject(buffer,temp1.Data(),temp2.Data(),temp3.Data(), false);
      }
      if (numOfGraphs[taskHierarchyClassIndex[i]] > 0)
         buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   gFile = filsav;\n");
   buffer.AppendFormatted("   delete buffer;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteAnalyzer2Cpp()
{
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;
   ROMEString buf;
   ROMEString str;
   ROMEString pointer;

   if (makeOutput) cout<<"\n   Output Cpp-File:"<<endl;

   ROMEString tmp;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer2.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",
                               shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TROOT.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i]) {
         continue;
      }
      buffer.AppendFormatted("#include \"tasks/%sT%s.h\"\n", shortCut.Data(), taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sVersion.h\"\n", shortCut.Data());
   buffer.AppendFormatted("\n");

   // Console Screen
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::consoleStartScreen()\n{\n", shortCut.Data());
   buffer.AppendFormatted("   ROMEPrint::Print(%sLogo);\n", shortCut.Data());
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   if (numOfTaskHierarchy>0) {
      buffer.AppendFormatted("void %sAnalyzer::ConstructHistoFolders(TFolder *f, Bool_t addToArray)\n",shortCut.Data());
   } else {
      buffer.AppendFormatted("void %sAnalyzer::ConstructHistoFolders(TFolder */*f*/, Bool_t /*addToArray*/)\n",shortCut.Data());
   }
   buffer.AppendFormatted("{\n");
   if (numOfTaskHierarchy > 0) {
      buffer.AppendFormatted("   TFolder **folder = new TFolder*[%d];\n", numOfTaskHierarchy);
   }
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]]) {
         continue;
      }
      // create histo folder
      if (taskHierarchyParentIndex[i] == -1) {
         buffer.AppendFormatted("   folder[%d] = f->AddFolder(\"%sHistos%s\", \"Histograms of Task '%s%s'\");\n", i,
                                taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data(), taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data());
      } else {
         buffer.AppendFormatted("   folder[%d] = folder[%d]->AddFolder(\"%sHistos%s\", \"Histograms of Task '%s%s'\");\n",
                                i, taskHierarchyParentIndex[i], taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data(),
                                taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("   if (addToArray) {\n");
      buffer.AppendFormatted("      fHistoFolders->AddAtAndExpand(folder[%d], %d);\n", i, i);
      buffer.AppendFormatted("   }\n");
   }
   if (numOfTaskHierarchy > 0) {
      buffer.AppendFormatted("   delete [] folder;\n");
   }
   buffer.AppendFormatted("}\n\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::ConstructHistoDirectories(TDirectory *d, TObjArray *cratedDir)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   if (numOfTaskHierarchy <= 0) {
      buffer.AppendFormatted("   WarningSuppression(d);\n");
      buffer.AppendFormatted("   WarningSuppression(cratedDir);\n");
   }
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]]) {
         continue;
      }
      // create histo directory
      if (taskHierarchyParentIndex[i] == -1) {
         buffer.AppendFormatted("   d->cd();\n");
      } else {
         buffer.AppendFormatted("   ((TDirectory*)cratedDir->At(%d))->cd();\n", taskHierarchyParentIndex[i]);
      }
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,15,2))
      buffer.AppendFormatted("   cratedDir->AddAtAndExpand(new TDirectory(\"%sHistos%s\", \"Histograms of Task '%s%s'\"),%d);\n",
#else
      buffer.AppendFormatted("   cratedDir->AddAtAndExpand(new TDirectoryFile(\"%sHistos%s\", \"Histograms of Task '%s%s'\"),%d);\n",
#endif
                             taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data(),
                             taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data(),i);
   }
   buffer.AppendFormatted("}\n\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sAnalyzer::InitTasks()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   fMainTask = new %sEventLoop(\"analyzer\",\"Main Task of %s%s\");\n", shortCut.Data(),
                          shortCut.Data(), mainProgName.Data());
   buffer.AppendFormatted("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormatted("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   buffer.AppendFormatted("   fMainHistoFolder = fMainFolder->AddFolder(\"histos\", \"Histogram Folder\");\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fTaskObjects = new TObjArray(%d);\n", numOfTaskHierarchy);
   buffer.AppendFormatted("   ConstructHistoFolders(fMainHistoFolder, kTRUE);\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]]) {
         continue;
      }
      // create task
      buffer.AppendFormatted("   AddTask(new %sT%s(\"%s%s\", \"%s\", %d, \"%s\", GetHistoFolderAt(%d)));\n",
                             shortCut.Data(), taskHierarchyName[i].Data(), taskHierarchyName[i].Data(),
                             taskHierarchySuffix[i].Data(), "", taskHierarchyLevel[i], taskHierarchySuffix[i].Data(), i);
      buffer.AppendFormatted("   GetTaskObjectAt(%d)->SetActive(false);\n", taskHierarchyObjectIndex[i]);
      if (taskHierarchyParentIndex[i] == -1) {
         parentt = "GetMainTask()";
      } else {
         parentt.SetFormatted("GetTaskObjectAt(%d)", taskHierarchyParentIndex[i]);
      }
      buffer.AppendFormatted("   %s->Add(GetTaskObjectAt(%d));\n", parentt.Data(), taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteAnalyzer3Cpp()
{
   int i,j;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;
   ROMEString buf;
   ROMEString str;
   ROMEString pointer;

   if (makeOutput) cout<<"\n   Output Cpp-File:"<<endl;

   ROMEString tmp;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer3.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",
                               shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <direct.h>\n");
#endif
   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#include <TGClient.h>\n");
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfigToForm.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Count number of databases
   int ndb = 0;
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i])
         ndb++;
   }

   // ReadSingleDataBaseFolders
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadSingleDataBaseFolders()\n{\n",shortCut.Data());
   if (ndb > 0) {
      for (i = 0 ; i < numOfFolder; i++) {
         if (!folderUsed[i])
            continue;
         if (folderDataBase[i] && folderArray[i] == "1" && !folderSupport[i]) {
            buffer.AppendFormatted("   if (!fDBAccess->Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ReadArrayDataBaseFolders
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadArrayDataBaseFolders()\n{\n",shortCut.Data());
   if (ndb > 0) {
      for (i = 0; i < numOfFolder; i++) {
         if (!folderUsed[i])
            continue;
         if (folderDataBase[i] && folderArray[i] != "1" && !folderSupport[i]) {
            buffer.AppendFormatted("   if (!fDBAccess->Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ShowConfigurationFile
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::ShowConfigurationFile()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   bool writeFlag;\n");
   buffer.AppendFormatted("   writeFlag = true;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString path = \"\";\n");
   buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
   buffer.AppendFormatted("   int ind;\n");
   buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   int exitID;\n");
   buffer.AppendFormatted("   %sConfigToForm *dialog = new %sConfigToForm();\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   if((exitID=dialog->Show(gClient->GetRoot(),0)) == 1) {\n");
   buffer.AppendFormatted("      if (isNoGraphics())\n");
   buffer.AppendFormatted("         SetStandAloneROME();\n");
   buffer.AppendFormatted("      else {\n");
   buffer.AppendFormatted("         if (dialog->IsChecked(\"Analyzer\"))\n");
   buffer.AppendFormatted("            SetStandAloneROME();\n");
   buffer.AppendFormatted("         else if (dialog->IsChecked(\"Monitor\"))\n");
   buffer.AppendFormatted("            SetStandAloneARGUS();\n");
   buffer.AppendFormatted("         else if (dialog->IsChecked(\"Analyzer with Monitor\"))\n");
   buffer.AppendFormatted("            SetROMEAndARGUS();\n");
   buffer.AppendFormatted("         else if (dialog->IsChecked(\"Monitor connecting to Analyzer\"))\n");
   buffer.AppendFormatted("            SetROMEMonitor();\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            SetStandAloneROME();\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      SetRunNumbers(dialog->GetValue(\"Offline/RunNumbers\"));\n");
   buffer.AppendFormatted("      SetInputFileNames(dialog->GetValue(\"Offline/InputFileNames\"));\n");
   for (i = 0; i < mainParGroup->GetNumberOfSubGroups(); i++) {
      buffer.AppendFormatted("      if (!Save%s(dialog)) return false;\n",
                             mainParGroup->GetSubGroupAt(i)->GetGroupName().Data());
   }
   buffer.AppendFormatted("      GetConfiguration()->CheckConfigurationModified(0);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   SafeDelete(dialog);\n");
   buffer.AppendFormatted("   return exitID != -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Get Object Interpreter Code
   int codeNumber = 0;
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Int_t %sAnalyzer::GetObjectInterpreterCode(const char* objectPath) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString path = objectPath;\n");
   buffer.AppendFormatted("   if (path.Index(\"/\") == -1 && path.Index(\"gAnalyzer->\") == -1)\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   if (path.Index(\"/GSP\") == 0 || path.Index(\"gAnalyzer->GetGSP()\") == 0) {\n");
   ROMEString path1 = "/GSP";
   ROMEString path2 = "GetGSP()";
   codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber, 0, numOfTask,path1,path2, 1);
   buffer.AppendFormatted("      ROMEPrint::Error(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
   buffer.AppendFormatted("      ROMEPrint::Error(\"   %%s\\n\", path.Data());\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   }\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderArray[i] == "1" && !folderSupport[i]) {
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] == 0 && !isFolder(valueType[i][j].Data()) && !valueIsTObject[i][j]) {
               buffer.AppendFormatted("   if (path == \"/%s/%s\")\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("      return %d;\n",codeNumber);
               buffer.AppendFormatted("   if (path.Index(\"/%s/%s\") != -1)\n",folderName[i].Data(),
                                      valueName[i][j].Data());
               buffer.AppendFormatted("      return -2;\n");
               buffer.AppendFormatted("   if (path == \"gAnalyzer->Get%s()->Get%s()\")\n",folderName[i].Data(),
                                      valueName[i][j].Data());
               buffer.AppendFormatted("      return %d;\n",codeNumber);
               buffer.AppendFormatted("   if (path.Index(\"gAnalyzer->Get%s()->Get%s()\") != -1)\n",
                                      folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("      return -2;\n");
               codeNumber++;
            }
         }
      }
   }
//   buffer.AppendFormatted("   ROMEPrint::Error(\"\\nWrong path in configuration file.\\n\");\n");
//   buffer.AppendFormatted("   ROMEPrint::Error(\"   %%s\\n\", path.Data());\n");
   buffer.AppendFormatted("   return -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   fNumberOfInterpreterCodes = codeNumber;

   // Get Object Interpreter Values
   WriteObjectInterpreterValue(buffer,"int","Int");
   WriteObjectInterpreterValue(buffer,"double","Double");
   WriteObjectInterpreterValue(buffer,"ROMEString&","Char");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteAnalyzer4Cpp()
{
   int i;
   Bool_t isY,isZ;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;
   ROMEString buf;
   ROMEString str;
   ROMEString pointer;

   if (makeOutput) cout<<"\n   Output Cpp-File:"<<endl;

   ROMEString tmp;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer4.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",
                               shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <direct.h>\n");
#endif
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfigToForm.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("#include \"ROMEDataBaseDAQ.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   WriteConfigToFormSave(buffer,mainParGroup,"","","", 0, 1,"");
   buffer.AppendFormatted("\n");

   // SaveHisto
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sAnalyzer::SaveHisto(%sConfigToForm *dialog,const char* path,ROMEHisto* histo,ROMEConfigHisto* configHisto,Int_t histoDimension)\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   for (i = 0; i < histoParameters->GetEntriesFast(); i++) {
      isY = false;
      isZ = false;
      if (histoParameters->At(i).Index("Ym")!=-1 ||
          histoParameters->At(i).Index("YN")!=-1 ||
          histoParameters->At(i).Index("ZL")!=-1)
         isY = true;
      if (histoParameters->At(i).Index("Zm")!=-1 ||
          histoParameters->At(i).Index("ZN")!=-1)
         isZ = true;
      if (isY)
         buffer.AppendFormatted("   if (histoDimension>1) {\n");
      else if (isZ)
         buffer.AppendFormatted("   if (histoDimension>2) {\n");
      else
         buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("      // %s;\n",histoParameters->At(i).Data());
      buffer.AppendFormatted("      str = path;\n");
      buffer.AppendFormatted("      str += \"Hist%s\";\n",histoParameters->At(i).Data());
      buffer.AppendFormatted("      str = dialog->GetValue(str.Data());\n");
      if (histoParameterTypes->At(i) == "bool") {
         buffer.AppendFormatted("      writeString = kFalseTrueString[histo->Is%s()?1:0];\n",
                                histoParameters->At(i).Data());
      } else {
         buffer.AppendFormatted("      writeString.SetFormatted(\"%s\",histo->Get%s());\n",
                                histoParameterTypes->At(i).Data(),histoParameters->At(i).Data());
      }
      buffer.AppendFormatted("      if (str != writeString) {\n");
      buffer.AppendFormatted("         configHisto->fHist%sModified = true;\n",histoParameters->At(i).Data());
      buffer.AppendFormatted("         configHisto->fHist%s = str;\n",histoParameters->At(i).Data());
      if (histoParameters->At(i)=="Active" || histoParameters->At(i)=="Write")
         buffer.AppendFormatted("         histo->Set%s(str == \"true\");\n",histoParameters->At(i).Data());
      else
         buffer.AppendFormatted("         histo->Set%s(str);\n",histoParameters->At(i).Data());
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   // HistAccumulate;\n");
   buffer.AppendFormatted("   str = dialog->GetValue(path);\n");
   buffer.AppendFormatted("   writeString = kFalseTrueString[histo->IsAccumulate()?1:0];\n");
   buffer.AppendFormatted("   if (str != writeString) {\n");
   buffer.AppendFormatted("      configHisto->fHistAccumulateModified = true;\n");
   buffer.AppendFormatted("      configHisto->fHistAccumulate = str;\n");
   buffer.AppendFormatted("      histo->SetAccumulate(str != \"false\");\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteAnalyzerH()
{
   int i,j,k;

   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString bankname;
   ROMEString tmp;
   if (makeOutput) cout<<"\n   Output H-File:"<<endl;
   // max folder name length
   int nameLen = -1;
   int fieldLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i]) {
         continue;
      }
      if (!folderSupport[i]) {
         if (typeLen < static_cast<int>(folderName[i].Length() + scl)) {
            typeLen = folderName[i].Length()+scl;
         }
         if (nameLen < static_cast<int>(folderName[i].Length())) {
            nameLen = folderName[i].Length();
         }
         if (folderDataBase[i]) {
            for (j = 0; j < numOfValue[i]; j++) {
               if (fieldLen < static_cast<int>(valueName[i][j].Length()))
                  fieldLen = valueName[i][j].Length();
            }
         }
      }
   }

   // max task switch name length
   int switchLen = -1;
   ROMEString switchString;
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      int indx = taskHierarchyParentIndex[i];
      switchString = taskHierarchyName[i].Data();
      while (indx != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, taskHierarchyName[indx].Data());
         indx = taskHierarchyParentIndex[indx];
      }
      if (switchLen < static_cast<int>(switchString.Length())) {
         switchLen = switchString.Length();
      }
   }

   // File name
   hFile.SetFormatted("%sinclude/generated/%sAnalyzer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sAnalyzer_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sAnalyzer_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",
                               shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // includes
   buffer.AppendFormatted("#include \"ROMEAnalyzer.h\"\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TTree.h>\n");
   buffer.AppendFormatted("#include <TFile.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("class %sGlobalSteering;\n", shortCut.Data());
   buffer.AppendFormatted("class %sWindow;\n", shortCut.Data());

   // DAQ class declaration
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("class %s%sDAQ;\n",daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
   }

   // Folder class declaration
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i]) {
         buffer.AppendFormatted("class %s%s;\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Task class declaration
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("class %sT%s;\n",shortCut.Data(),taskHierarchyName[i].Data());
      buffer.AppendFormatted("class %sT%s_Base;\n",shortCut.Data(),taskHierarchyName[i].Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("class %sEventLoop;\n",shortCut.Data());
   buffer.AppendFormatted("class %sConfig;\n",shortCut.Data());
   buffer.AppendFormatted("class %sDBAccess;\n",shortCut.Data());
   buffer.AppendFormatted("class %sConfigToForm;\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sAnalyzer : public ROMEAnalyzer\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("friend class %sWindow;\n",shortCut.Data());
   buffer.AppendFormatted("friend class %sEventLoop;\n",shortCut.Data());
   buffer.AppendFormatted("friend class %sConfig;\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("friend class %sGlobalSteering;\n",shortCut.Data());
   // Fields
   buffer.AppendFormatted("protected:\n");

   // Folder Fields
   buffer.AppendFormatted("   // Folder Fields\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("   %s%s* f%sFolder; // Handle to %s%s Folder\n",shortCut.Data(),
                                      folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %s%s* f%sFolderStorage; // Handle to %s%s Folder Storage\n",shortCut.Data(),
                                      folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            } else {
               buffer.AppendFormatted("   TClonesArray* f%sFolders; // Handle to %s%s Folders\n",folderName[i].Data(),
                                      shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   TClonesArray* f%sFoldersStorage; // Handle to %s%s Folders Storage\n",
                                      folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Steering Parameter Fields
   if (numOfSteering[numOfTask] > 0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n",
                             shortCut.Data());
   }
   buffer.AppendFormatted("\n");

   // DAQ Handle
   buffer.AppendFormatted("   // DAQ Handle\n");
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("   %s%sDAQ* f%sDAQ; // Handle to the %s DAQ Class\n",daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   %sDBAccess* fDBAccess; // Handle to the DBAccess Class\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Methods
   // Constructor
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sAnalyzer(const %sAnalyzer &c); // not implemented\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   %sAnalyzer &operator=(const %sAnalyzer &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sAnalyzer(ROMERint *app = dynamic_cast<ROMERint*>(gApplication), Bool_t batch = kFALSE,\n",
                          shortCut.Data());
   buffer.AppendFormatted("               Bool_t daemon = kFALSE, Bool_t nographics = kFALSE, Int_t mode = 0);\n");
   buffer.AppendFormatted("   virtual ~%sAnalyzer();\n",shortCut.Data());

   // Folder Getters and Setters
   buffer.AppendFormatted("   // Folders\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      WriteFolderGetterInclude(buffer,i);
      WriteFolderSetterInclude(buffer,i);
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void FolderArrayOutOfBouds(Int_t index,const char* folder,const char* arraySize) const;\n");
   buffer.AppendFormatted("   Bool_t RegisterFolder(const char* object);\n");
   buffer.AppendFormatted("   Bool_t UnRegisterFolder(const char* object);\n");
   buffer.AppendFormatted("\n");

   // Config Parameters folder
   buffer.AppendFormatted("   // Config Parameter Folder\n");
   buffer.AppendFormatted("   void FillConfigParametersFolder();\n");
   buffer.AppendFormatted("   void SaveConfigParametersFolder() const;\n");

   // Storage
   buffer.AppendFormatted("   // Storage\n");
   buffer.AppendFormatted("   void   FillObjectStorage();\n");

   // check dependence
   buffer.AppendFormatted("   // Check dependence\n");
   buffer.AppendFormatted("   Bool_t CheckDependences() const;\n");
   buffer.AppendFormatted("   void   UpdateConfigParameters();\n");

   // Check tree names
   buffer.AppendFormatted("   // Check tree names\n");
   buffer.AppendFormatted("   Bool_t CheckTreeFileNames() const;\n");

   // Set size
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i] && FolderToBeGenerated(i) && folderArray[i] != "1") {
         if (folderArray[i] != "variable")
            buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   void   Set%sSize(Int_t number);\n",folderName[i].Data());
         if (folderArray[i] != "variable")
            buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   Int_t  Get%sSize() const { return f%sFolders->GetEntriesFast(); }\n",folderName[i].Data(),
                                folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Histos
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TH1* GetHisto(const char* pathToHisto) const;\n");
   buffer.AppendFormatted("   bool ResetAllHistos();\n");
   buffer.AppendFormatted("\n");

   // Graphs
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TH1* GetGraph(const char* pathToGraph) const;\n");
   buffer.AppendFormatted("   bool ResetAllGraphs();\n");
   buffer.AppendFormatted("\n");

   // Tasks
   buffer.AppendFormatted("   // Tasks\n");
   buffer.AppendFormatted("   void ConstructHistoFolders(TFolder *f, Bool_t addToArray);\n");
   buffer.AppendFormatted("   void ConstructHistoDirectories(TDirectory *d, TObjArray *cratedDir);\n");
   buffer.AppendFormatted("   void InitTasks();\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("   %sT%s* Get%s%sTask() const { return reinterpret_cast<%sT%s*>(GetTaskObjectAt(%d)); }\n",shortCut.Data(),
                             taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                             shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("   %sT%s_Base* Get%s%sTaskBase() const { return reinterpret_cast<%sT%s_Base*>(GetTaskObjectAt(%d)); }\n",
                             shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),
                             taskHierarchySuffix[i].Data(),shortCut.Data(),taskHierarchyName[i].Data(),
                             taskHierarchyObjectIndex[i]);
   }
   buffer.AppendFormatted("\n");

   // Tree Getters
   buffer.AppendFormatted("   // Trees\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   TTree* Get%sTree() const { return static_cast<ROMETree*>(fTreeObjects->At(%d))->GetTree(); }\n",
                             treeName[i].Data(),i);
      buffer.AppendFormatted("   TFile* Get%sFile() const { return static_cast<ROMETree*>(fTreeObjects->At(%d))->GetFile(); }\n",
                             treeName[i].Data(),i);
   }
   buffer.AppendFormatted("\n");

   // Database Getters
   buffer.AppendFormatted("   // Database\n");
   buffer.AppendFormatted("   ROMEDataBase* GetXMLDataBase() const { return GetDataBase(\"XML\"); }\n");
   buffer.AppendFormatted("   ROMEDataBase* GetTextDataBase() const { return GetDataBase(\"TEXT\"); }\n");
   buffer.AppendFormatted("   ROMEDataBase* GetODBDataBase() const { return GetDataBase(\"ODB\"); }\n");
   if (sql)
      buffer.AppendFormatted("   ROMEDataBase* GetSQLDataBase() const { return GetDataBase(\"SQL\"); }\n");
   for (i = 0; i < numOfDB; i++) {
      buffer.AppendFormatted("   ROMEDataBase* Get%sDataBase() const { return GetDataBase(\"%s\"); }\n",dbName[i].Data(),
                             dbName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   buffer.AppendFormatted("   Bool_t ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   Bool_t ReadArrayDataBaseFolders();\n");
   // GetDBPathWriteFlag
   buffer.AppendFormatted("   Bool_t GetDBPathWriteFlag(const char* path) const;\n");
   buffer.AppendFormatted("\n");
   // DBAccess
   buffer.AppendFormatted("   %sDBAccess* GetDBAccess() const { return fDBAccess; };\n",shortCut.Data());
   buffer.AppendFormatted("\n");
#if 0
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   void Write%sDataBase();\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");
#endif
   // Object Interpreter
   buffer.AppendFormatted("   // Object Interpreter\n");
   buffer.AppendFormatted("   Int_t       GetObjectInterpreterCode(const char* objectPath) const;\n");
   buffer.AppendFormatted("   Int_t       GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue) const;\n");
   buffer.AppendFormatted("   Double_t    GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue) const;\n");
   buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer) const;\n");
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTask] > 0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() const { return fGlobalSteeringParameters; }\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // DAQ Access Methods
   buffer.AppendFormatted("   // Deprecated DAQ Access Methods\n");
   if (numOfEvent > 0 || midas)
      buffer.AppendFormatted("   %sMidasDAQ* GetMidas() const { return GetMidasDAQ(); }\n",shortCut.Data());
   if (numOfTree > 0)
      buffer.AppendFormatted("   %sRomeDAQ* GetRome() const { return GetRomeDAQ(); }\n",shortCut.Data());
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("   // %s DAQ Access Methods\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("   Bool_t Is%sDAQ() const { return f%sDAQ != 0; }\n",daqNameArray->At(i).Data(),
                             daqNameArray->At(i).Data());
      buffer.AppendFormatted("   %s%sDAQ* Get%sDAQ() const;\n",daqTypeArray->At(i).Data(),daqNameArray->At(i).Data(),
                             daqNameArray->At(i).Data());
      buffer.AppendFormatted("   void     Set%sDAQ(%s%sDAQ* handle) { f%sDAQ = handle; }\n",daqNameArray->At(i).Data(),
                             daqTypeArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
   }

   // Folder dump and load
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t   DumpFolders(const char* filename, Bool_t only_database = kFALSE) const;\n");
   buffer.AppendFormatted("   Bool_t   LoadFolders(const char* filename, Bool_t only_database = kFALSE);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t   ShowConfigurationFile();\n");
   // Config to form save
   buffer.AppendFormatted("   Bool_t   SaveHisto(%sConfigToForm *dialog,const char* path,ROMEHisto* histo,ROMEConfigHisto* configHisto,Int_t histoDimension);\n",shortCut.Data());
   for (i = 0; i < mainParGroup->GetNumberOfSubGroups(); i++) {
      buffer.AppendFormatted("   Bool_t   Save%s(%sConfigToForm *dialog);\n",
                             mainParGroup->GetSubGroupAt(i)->GetGroupName().Data(),shortCut.Data());
      if (mainParGroup->GetSubGroupAt(i)->GetGroupName() == "Common") {
         for (j = 0; j < mainParGroup->GetSubGroupAt(i)->GetNumberOfSubGroups(); j++) {
            buffer.AppendFormatted("   Bool_t   Save%s(%sConfigToForm *dialog);\n",
                                   mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data(),
                                   shortCut.Data());
            if (mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName() == "Folders") {
               for (k = 0; k < mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetNumberOfSubGroups(); k++)
                  buffer.AppendFormatted("   Bool_t   Save%s(%sConfigToForm *dialog);\n",
                                         mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetSubGroupAt(k)->GetGroupName().Data(),
                                         shortCut.Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");


   buffer.AppendFormatted("   %sWindow *GetWindow() const { return reinterpret_cast<%sWindow*>(fWindow); }\n",shortCut.Data(),shortCut.Data());
   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   Bool_t ReadUserParameter(const char* opt, const char *value, Int_t& i);\n");
   buffer.AppendFormatted("   void   UserParameterUsage() const;\n");
   buffer.AppendFormatted("   void   startSplashScreen();\n");
   buffer.AppendFormatted("   void   consoleStartScreen();\n");
   buffer.AppendFormatted("   Bool_t ConnectSocketClientNetFolder(TSocket *sock);\n");
   buffer.AppendFormatted("   void   StartNetFolderServer();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sAnalyzer, 0);\n",shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("extern %sAnalyzer *gAnalyzer;  // global Analyzer Handle\n\n",shortCut.Data());

   buffer.AppendFormatted("#endif   // %sAnalyzer_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteWindowCpp()
{
   Int_t iTab, iFolder, j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString bufferTemp;
   ROMEString menu_title;
   ROMEString buf;

   ROMEString clsDescription;
   ROMEString clsName;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sWindow.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sWindow", shortCut.Data());
   clsDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.",
                               shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TGLabel.h>\n");
   buffer.AppendFormatted("#include <TGTab.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("#include \"ArgusTextDialog.h\"\n");
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[iTab].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"ROMECompositeFrame.h\"\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sWindow)\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sWindow::%sWindow()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted(":ArgusWindow(kTRUE");
   if (numOfTab>0) {
      buffer.AppendFormatted(", %d",numOfTab);
   } else {
      buffer.AppendFormatted(", 0");
   }
   buffer.AppendFormatted(")\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ConstructTabs();\n"); // this is necessary to handle config parameters
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Int_t iTab;\n");
   buffer.AppendFormatted("   const Int_t nTabs = fTabObjects->GetEntriesFast();\n");
   buffer.AppendFormatted("   for (iTab = 0; iTab < nTabs; iTab++) {\n");
   buffer.AppendFormatted("      GetTabObjectAt(iTab)->SetSwitch(kTRUE);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sWindow::%sWindow(const TGWindow* p,Bool_t tabWindow)\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted(":ArgusWindow(p, kTRUE");
   if (numOfTab > 0) {
      buffer.AppendFormatted(", %d", numOfTab);
   } else {
      buffer.AppendFormatted(", 0");
   }
   buffer.AppendFormatted(", tabWindow");
   buffer.AppendFormatted(")\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ConstructTabs();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Int_t iTab;\n");
   buffer.AppendFormatted("   const Int_t nTabs = fTabObjects->GetEntriesFast();\n");
   buffer.AppendFormatted("   for (iTab = 0; iTab < nTabs; iTab++) {\n");
   buffer.AppendFormatted("      GetTabObjectAt(iTab)->SetSwitch(tabWindow);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Clone Window
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("ArgusWindow* %sWindow::CreateSubWindow()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ArgusWindow* window = new %sWindow(gClient->GetRoot(),kFALSE);\n", shortCut.Data());
   buffer.AppendFormatted("   return window;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ProcessMessage
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sWindow::ProcessMessageNetFolder(Long_t param1)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   switch (param1) {\n");
   for (iFolder = 0; iFolder < numOfNetFolder; iFolder++) {
      buffer.AppendFormatted("   case M_FILE_CONNECT_%s:\n", netFolderName[iFolder].Data());
      buffer.AppendFormatted("      gAnalyzer->ConnectNetFolder(\"%s\");\n", netFolderName[iFolder].Data());
      buffer.AppendFormatted("      break;\n");
   }
//   buffer.AppendFormatted("      default:\n");
//   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // GetMenuHandle
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("TGPopupMenu* %sWindow::GetMenuHandle(const char* menuName) const\n", shortCut.Data());
   buffer.AppendFormatted("{\n");

   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab] || (tabHeredity[iTab].Length() <= 0 && numOfMenu[iTab] <= 0)) {
         continue;
      }
      buffer.AppendFormatted("   if (GetCurrentTabObjectIndex() == %d) {\n", tabUsedIndex[iTab]);
      if (tabHeredity[iTab].Length() > 0) {
         for (j = 0; j < numOfMenu[tabHeredityIndex[iTab]]; j++) {
            buffer.AppendFormatted("      if (!strcmp(menuName,\"%s\")) {\n", menuTitle[tabHeredityIndex[iTab]][j].Data());
            buffer.AppendFormatted("         return GetTabObjectAt(%d)->GetUserPopupMenuAt(%d);\n", tabUsedIndex[iTab], j+numOfMenu[iTab]);
            buffer.AppendFormatted("      }\n");
         }
      }
      for (j = 0; j < numOfMenu[iTab]; j++) {
         buffer.AppendFormatted("      if (!strcmp(menuName,\"%s\")) {\n", menuTitle[iTab][j].Data());
         buffer.AppendFormatted("         return GetTabObjectAt(%d)->GetUserPopupMenuAt(%d);\n", tabUsedIndex[iTab], j);
         buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("   }\n");
   }

   buffer.AppendFormatted("   ROMEPrint::Error(\"Error: A menu (%%s) was not found.\\n\", menuName);\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddMenuNetFolder
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sWindow::AddMenuNetFolder(TGPopupMenu* menu)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   if (gAnalyzer->GetNumberOfNetFolders() <= 0 ) {\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   Bool_t active = kFALSE;\n");
   for (iFolder = 0; iFolder < numOfNetFolder; iFolder++) {
      buffer.AppendFormatted("   if (gAnalyzer->GetNetFolderActive(%d)) {\n", iFolder);
      buffer.AppendFormatted("      menu->AddEntry(\"%s\", M_FILE_CONNECT_%s);\n", netFolderName[iFolder].Data(),
                             netFolderName[iFolder].Data());
      buffer.AppendFormatted("      active = kTRUE;\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   return active;\n");
   if (!numOfNetFolder) {
      buffer.AppendFormatted("   WarningSuppression(menu);\n");
      buffer.AppendFormatted("   return active;\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteWindow2Cpp()
{
   Int_t iTab;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString bufferTemp;
   ROMEString buf;

   ROMEString clsDescription;
   ROMEString clsName;

   ROMEString parenttab;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sWindow2.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sWindow", shortCut.Data());
   clsDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.",
                               shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TGTab.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      buffer.AppendFormatted("#include \"tabs/%sT%s.h\"\n", shortCut.Data(), tabName[iTab].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sWindow::ConstructTabs()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");

   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (tabUsed[iTab]) {
         buffer.AppendFormatted("   Int_t iTab = 0;\n");
         break;
      }
   }

   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab]) {
         continue;
      }
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   AddTab(new %sT%s(this));\n", shortCut.Data(), tabName[iTab].Data() );
      buffer.AppendFormatted("   fParentIndex[iTab] = %d;\n", tabParentIndex[iTab]>=0?tabUsedIndex[tabParentIndex[iTab]]:-1);
      buffer.AppendFormatted("   fNumberOfChildren[iTab] = %d;\n", tabNumOfChildren[iTab]);
      buffer.AppendFormatted("   iTab++;\n");
   }
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteWindowH()
{
   Int_t iTab, iFolder, j, k;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString buf;

   ROMEString clsDescription;
   ROMEString clsName;

   Int_t nameLen = -1;
   Int_t typeLen = -1;
   Int_t scl = shortCut.Length();
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[iTab].Length() + strlen("f000Tab")));
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[iTab].Length() + scl + strlen("tT")));
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[iTab].Length() + strlen("Get000Tab()")));
      typeLen = TMath::Max(typeLen, static_cast<Int_t>(tabName[iTab].Length() + scl + strlen("T*_Base")));
   }
   typeLen = TMath::Max(typeLen, static_cast<Int_t>(strlen("TGCompositeFrame*")));

   // max tab switch name length
   Int_t switchLen = -1;
   ROMEString switchString;
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      Int_t indx = tabParentIndex[iTab];
      switchString = tabName[iTab].Data();
      while (indx != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[indx].Data());
         indx = tabParentIndex[indx];
      }
      if (switchLen < static_cast<Int_t>(switchString.Length()))
         switchLen = switchString.Length();
   }

   // File name
   hFile.SetFormatted("%sinclude/generated/%sWindow.h", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sWINDOW_H\n", shortCut.Data());
   buffer.AppendFormatted("#define %sWINDOW_H\n\n", shortCut.Data());
   clsName.SetFormatted("%sWindow", shortCut.Data());
   clsDescription.SetFormatted("Main window class for the %s%s.", shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include \"ArgusWindow.h\"\n");
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      buffer.AppendFormatted("class %sT%s_Base;\n", shortCut.Data(), tabName[iTab].Data());
   }
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("class %sWindow : public ArgusWindow {  \n", shortCut.Data());
   // Enumeration
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   enum CommandIdentifiers{\n");
   buffer.AppendFormatted("      M_FILE_CONNECT_ROOT = 10,\n");
   for (iFolder = 0; iFolder < numOfNetFolder; iFolder++) {
      buffer.AppendFormatted("      M_FILE_CONNECT_%s,\n", netFolderName[iFolder].Data());
   }
   buffer.Remove(buffer.Length() - 2, 1);       // remove the last ','
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   enum MenuEnumeration {\n");
   buffer.AppendFormatted("      M_ROOT = 10000,\n");
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      for (j = 0; j < numOfMenu[iTab]; j++) {
         for (k = 0; k < numOfMenuItem[iTab][j]; k++) {
            if (menuItemEnumName[iTab][j][k].Length() > 0)
               buffer.AppendFormatted("      %s,\n", menuItemEnumName[iTab][j][k].Data());
         }
      }
   }
   buffer.Remove(buffer.Length() - 2, 1);       // remove the last ','
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sWindow(const %sWindow &c); // not implemented\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   %sWindow &operator=(const %sWindow &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sWindow();\n", shortCut.Data());
   buffer.AppendFormatted("   %sWindow(const TGWindow *p,Bool_t tabWindow=kTRUE);\n", shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sWindow() {}\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ArgusWindow *CreateSubWindow();\n");
   buffer.AppendFormatted("   void         ConstructTabs();\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   Bool_t       AddMenuNetFolder(TGPopupMenu* menu);\n");
   buffer.AppendFormatted("   Bool_t       ProcessMessageNetFolder(Long_t param1);\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   TGPopupMenu* GetMenuHandle(const char* menuName) const;\n");
   buffer.AppendFormatted("\n");

   // Tab Getters
   buffer.AppendFormatted("   // Tabs\n");
   for (iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      // cast to avoid inconsistency between format and arguments.
      buffer.AppendFormatted("   %sT%s_Base* Get%s%sTab() const { return reinterpret_cast<%sT%s_Base*>(GetTabObjectAt(%d)); }\n",
                             shortCut.Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabSuffix[iTab].Data(),
                             shortCut.Data(), tabName[iTab].Data(), tabUsedIndex[iTab]);
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sWindow, 0)\n", shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDBAccessCpp()
{
   int i,j,k;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString str;

   ROMEString clsDescription;
   ROMEString clsName;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sDBAccess.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sDBAccess", shortCut.Data());
   clsDescription.SetFormatted("Handles the database access for the %s%s.", shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   for (i = 0; i < numOfFolder; i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         if (folderUserCode[i]) {
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());         
         }
      }
   }
   if (readGlobalSteeringParameters) {
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   }
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"TMath.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");


   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]]) {
         continue;
      }
      str.SetFormatted("Get%s%sTask", taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data());
      for (j = 0; j < numOfFolder; j++) {
         for (k = 0; k < numOfValue[j]; k++) {
            if (valueDimension[j][k] > 1 || valueArray[j][k][0] == "variable" ||
                isFolder(valueType[j][k].Data()) || valueIsTObject[j][k]) {
               continue;
            }
#if 1 // forbid DB access for fields without DB path in definixion XML file
            if (!valueDBPath[j][k].Length()) {
               continue;
            }
#endif
            if (valueDBIf[j][k].Contains(str)) {
               buffer.AppendFormatted("#include \"tasks/%sT%s.h\"\n", shortCut.Data(),
                                      taskHierarchyName[i].Data());
               j = numOfFolder;
               break;
            }
         }
      }
   }

   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sDBAccess)\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sDBAccess::%sDBAccess() : TObject()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i,j,k;\n");
   for (i = 0; i < numOfFolder; i++) {
      buffer.AppendFormatted("   fNumberOfValues[%d] = %d;\n",i,numOfValue[i]);
   }
   buffer.AppendFormatted("   for (i = 0; i < %d; i++) {\n",numOfFolder);
   buffer.AppendFormatted("      fDBName[i] = new ROMEString[fNumberOfValues[i]];\n");
   buffer.AppendFormatted("      fDBPath[i] = new ROMEString[fNumberOfValues[i]];\n");
   buffer.AppendFormatted("      fDBCode[i] = new TArrayI*[fNumberOfValues[i]];\n");
   buffer.AppendFormatted("      for (j = 0; j < fNumberOfValues[i]; j++) {\n");
   buffer.AppendFormatted("         fDBName[i][j] = \"\";\n");
   buffer.AppendFormatted("         fDBPath[i][j] = \"\";\n");
   buffer.AppendFormatted("         fDBCode[i][j] = new TArrayI(%d);\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("         for (k = 0; k < %d; k++) {\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("            fDBCode[i][j]->AddAt(-1,k);\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sDBAccess::~%sDBAccess()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i,j;\n");
   buffer.AppendFormatted("   for (i = 0; i < %d; i++) {\n",numOfFolder);
   buffer.AppendFormatted("      for (j = 0; j < fNumberOfValues[i]; j++) {\n");
   buffer.AppendFormatted("         SafeDelete(fDBCode[i][j]);\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      SafeDeleteArray(fDBName[i]);\n");
   buffer.AppendFormatted("      SafeDeleteArray(fDBPath[i]);\n");
   buffer.AppendFormatted("      SafeDeleteArray(fDBCode[i]);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("const char* %sDBAccess::GetDBNameAt(Int_t folderIndex,Int_t valueIndex) const\n{\n",
                          shortCut.Data());
   buffer.AppendFormatted("   if (fDBName[folderIndex][valueIndex] == \"\") {\n");
   buffer.AppendFormatted("      switch (folderIndex) {\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("      case %d:\n",i);
         buffer.AppendFormatted("      {\n");
         buffer.AppendFormatted("         switch (valueIndex) {\n");
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1) {
               continue;
            }
            buffer.AppendFormatted("         case %d:\n",j);
            buffer.AppendFormatted("            return \"%s\";\n",valueDBName[i][j].Data());
         }
         buffer.AppendFormatted("         default:\n");
         buffer.AppendFormatted("            return \"\";\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("      default:\n");
   buffer.AppendFormatted("         return \"\";\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   } else {\n");
   buffer.AppendFormatted("      return fDBName[folderIndex][valueIndex].Data();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("const char* %sDBAccess::GetDBPathAt(Int_t folderIndex,Int_t valueIndex,ROMEString& path) const\n{\n",
                          shortCut.Data());
   buffer.AppendFormatted("   if (fDBPath[folderIndex][valueIndex] == \"\") {\n");
   buffer.AppendFormatted("      switch (folderIndex) {\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("      case %d:\n",i);
         buffer.AppendFormatted("      {\n");
         buffer.AppendFormatted("         switch (valueIndex) {\n");
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1) {
               continue;
            }
            buffer.AppendFormatted("         case %d:\n",j);
            if (valueDBPath[i][j].Length()) {
               buffer.AppendFormatted("            path.SetFormatted(%s);\n",valueDBPath[i][j].Data());
            } else {
               buffer.AppendFormatted("            path = \"\";\n");
            }
            buffer.AppendFormatted("            return path.Data();\n");
         }
         buffer.AppendFormatted("         default:\n");
         buffer.AppendFormatted("            return \"\";\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("         default:\n");
   buffer.AppendFormatted("            return \"\";\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      return fDBPath[folderIndex][valueIndex].Data();\n");
   buffer.AppendFormatted("   WarningSuppression(path.Length());\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("const char* %sDBAccess::GetDBPathOriginalAt(Int_t folderIndex,Int_t valueIndex) const\n{\n",
                          shortCut.Data());
   buffer.AppendFormatted("   if (fDBPath[folderIndex][valueIndex] == \"\") {\n");
   buffer.AppendFormatted("      switch (folderIndex) {\n");
   for (i = 0; i < numOfFolder; i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("         case %d:\n",i);
         buffer.AppendFormatted("         {\n");
         buffer.AppendFormatted("            switch (valueIndex) {\n");
         for (j = 0; j < numOfValue[i]; j++) {
            if (valueDimension[i][j] > 1)
               continue;
            buffer.AppendFormatted("               case %d:\n",j);
            str = valueDBPath[i][j];
            str.ReplaceAll("\"","\\\"");
            buffer.AppendFormatted("                  return \"%s\";\n",str.Data());
         }
         buffer.AppendFormatted("               default:\n");
         buffer.AppendFormatted("                  return \"\";\n");
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("         }\n");
      }
   }
   buffer.AppendFormatted("         default:\n");
   buffer.AppendFormatted("            return \"\";\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      return fDBPath[folderIndex][valueIndex].Data();\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Int_t %sDBAccess::GetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex) const\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   return fDBCode[folderIndex][valueIndex]->GetSize();\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Int_t %sDBAccess::GetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i) const\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   return fDBCode[folderIndex][valueIndex]->At(i);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sDBAccess::SetDBNameAt(Int_t folderIndex,Int_t valueIndex,const char* name)\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBName[folderIndex][valueIndex] = name;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sDBAccess::SetDBPathAt(Int_t folderIndex,Int_t valueIndex,const char* path)\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBPath[folderIndex][valueIndex] = path;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sDBAccess::SetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t num)\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBCode[folderIndex][valueIndex]->Set(num);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sDBAccess::SetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i,Int_t code)\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBCode[folderIndex][valueIndex]->AddAt(code,i);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ReadDataBaseFolders
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %sDBAccess::Read%s()\n{\n",shortCut.Data(),folderName[i].Data());
         WriteReadDataBaseFolder(buffer,i,folderArray[i] == "1" ? 1 : 2);
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }


   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDBAccessH()
{
   int i;
   ROMEString hFile;
   ROMEString buffer;

   ROMEString clsDescription;
   ROMEString clsName;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sDBAccess.h", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sDBACCESS_H\n", shortCut.Data());
   buffer.AppendFormatted("#define %sDBACCESS_H\n\n", shortCut.Data());
   clsName.SetFormatted("%sDBAccess", shortCut.Data());
   clsDescription.SetFormatted("Handles the database access for the %s%s.", shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <TArrayI.h>\n");
   buffer.AppendFormatted("#include <TObject.h>\n");
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("class %sDBAccess : public TObject {  \n", shortCut.Data());

   // Fields
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   ROMEString *fDBName[%d];         //! Name of the database from which to read the field\n",
                          TMath::Max(numOfFolder, 1));
   buffer.AppendFormatted("   ROMEString *fDBPath[%d];         //! Database path to the value of the field\n",
                          TMath::Max(numOfFolder, 1));
   buffer.AppendFormatted("   TArrayI   **fDBCode[%d];         //! Object Interpreter codes for database path to the value of the field\n",
                          TMath::Max(numOfFolder, 1));
   buffer.AppendFormatted("   Int_t       fNumberOfValues[%d]; //! Number of values in folder\n",
                          TMath::Max(numOfFolder, 1));

   buffer.AppendFormatted("\n");

   // Method
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sDBAccess(const %sDBAccess &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("   %sDBAccess &operator=(const %sDBAccess &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sDBAccess();\n", shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sDBAccess();\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   const char* GetDBNameAt(Int_t folderIndex,Int_t valueIndex) const;\n");
   buffer.AppendFormatted("   const char* GetDBPathAt(Int_t folderIndex,Int_t valueIndex,ROMEString& path) const;\n");
   buffer.AppendFormatted("   const char* GetDBPathOriginalAt(Int_t folderIndex,Int_t valueIndex) const;\n");
   buffer.AppendFormatted("   Int_t       GetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex) const;\n");
   buffer.AppendFormatted("   Int_t       GetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i) const;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void SetDBNameAt(Int_t folderIndex,Int_t valueIndex,const char* name);\n");
   buffer.AppendFormatted("   void SetDBPathAt(Int_t folderIndex,Int_t valueIndex,const char* path);\n");
   buffer.AppendFormatted("   void SetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t num);\n");
   buffer.AppendFormatted("   void SetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i,Int_t code);\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i])
         buffer.AppendFormatted("   Bool_t Read%s();\n",folderName[i].Data());
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sDBAccess, 0)\n", shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigToFormCpp() {
   int i;
   Bool_t isY,isZ;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString str;
   ROMEString path;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfigToForm.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sConfigToForm", shortCut.Data());
   clsDescription.SetFormatted("This class makes form for configuration for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <typeinfo>\n");
   buffer.AppendFormatted("#include \"ROMEUtilities.h\"\n");
   buffer.AppendFormatted("#include \"XMLToFormFrame.h\"\n");
   buffer.AppendFormatted("#include \"ROMEDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]) {
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }

   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sConfigToForm.h\"\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      if (daqNameArray->At(i) == "Midas")
         buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                                daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\nClassImp(%sConfigToForm)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sConfigToForm::%sConfigToForm():ROMEConfigToForm()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddConfig
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sConfigToForm::AddConfig(XMLToFormFrame *frame)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fCommentLevel = gAnalyzer->GetConfiguration()->GetCommentLevel();\n");
   for (i = 0; i < mainParGroup->GetNumberOfSubGroups(); i++)
      buffer.AppendFormatted("   Add%sFrame(frame);\n",mainParGroup->GetSubGroupAt(i)->GetGroupName().Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddHisto
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sConfigToForm::AddHisto(XMLToFormFrame *frame,ROMEHisto* histo,Int_t histoDimension)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString comment;\n");
   buffer.AppendFormatted("   ROMEStrArray entries;\n");
   for (i = 0; i < histoParameters->GetEntriesFast(); i++) {
      isY = false;
      isZ = false;
      if (histoParameters->At(i).Index("Ym")!=-1 ||
          histoParameters->At(i).Index("YN")!=-1 ||
          histoParameters->At(i).Index("ZL")!=-1)
         isY = true;
      if (histoParameters->At(i).Index("Zm")!=-1 ||
          histoParameters->At(i).Index("ZN")!=-1)
         isZ = true;
      if (isY)
         buffer.AppendFormatted("   if (histoDimension>1) {\n");
      else if (isZ)
         buffer.AppendFormatted("   if (histoDimension>2) {\n");
      else
         buffer.AppendFormatted("   {\n");
      buffer.AppendFormatted("      // %s;\n",histoParameters->At(i).Data());
      if (histoParameterTypes->At(i) == "bool") {
         buffer.AppendFormatted("      writeString = kFalseTrueString[histo->Is%s()?1:0];\n",
                                histoParameters->At(i).Data());
      } else {
         buffer.AppendFormatted("      writeString.SetFormatted(\"%s\",histo->Get%s());\n",
                                histoParameterTypes->At(i).Data(),histoParameters->At(i).Data());
      }
      buffer.AppendFormatted("      comment = \"\";\n");
      buffer.AppendFormatted("      if (fCommentLevel >= %d)\n",ROMEConfig::kCommentLevelParam);
      path.SetFormatted("/xs:schema/xs:complexType[@name='HistogramDesc']/xs:sequence/xs:element[@name=Hist%s]/xs:annotation/xs:documentation",
                        histoParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("         comment = \"%s\";\n",str.Data());
      buffer.AppendFormatted("      frame->AddElement(new XMLToFormElement(\"%s\",\"Hist%s\",writeString.Data(),\"\", 0, &entries,comment.Data()));\n",
                             histoParameterWidgetTypes->At(i).Data(),histoParameters->At(i).Data());
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   // Accumulate;\n");
   buffer.AppendFormatted("   writeString = kFalseTrueString[histo->IsAccumulate()?1:0];\n");
   buffer.AppendFormatted("   comment = \"\";\n");
   buffer.AppendFormatted("   if (fCommentLevel >= %d)\n",ROMEConfig::kCommentLevelParam);
   path.SetFormatted("/xs:schema/xs:complexType[@name='HistogramDesc']/xs:sequence/xs:element[@name=HistAccumulate]/xs:annotation/xs:documentation");
   configXSD->GetPathValue(path, str, "");
   buffer.AppendFormatted("      comment = \"%s\";\n",str.Data());
   buffer.AppendFormatted("   frame->AddElement(new XMLToFormElement(\"CheckButton\",\"HistAccumulate\",writeString.Data(),\"\", 0, &entries,comment.Data()));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddGraph
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sConfigToForm::AddGraph(XMLToFormFrame *frame,ROMEGraph* graph)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString comment;\n");
   buffer.AppendFormatted("   ROMEStrArray entries;\n");
   for (i = 0; i < graphParameters->GetEntriesFast(); i++) {
      buffer.AppendFormatted("   // %s;\n",graphParameters->At(i).Data());
      if (graphParameterTypes->At(i) == "bool") {
         buffer.AppendFormatted("   writeString = kFalseTrueString[graph->Is%s()?1:0];\n",
                                graphParameters->At(i).Data());
      } else {
         buffer.AppendFormatted("   writeString.SetFormatted(\"%s\",graph->Get%s());\n",
                                graphParameterTypes->At(i).Data(),graphParameters->At(i).Data());
      }
      buffer.AppendFormatted("   comment = \"\";\n");
      buffer.AppendFormatted("   if (fCommentLevel >= %d)\n",ROMEConfig::kCommentLevelParam);
      path.SetFormatted("/xs:schema/xs:complexType[@name='GraphDesc']/xs:sequence/xs:element[@name=Graph%s]/xs:annotation/xs:documentation",
                        graphParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("      comment = \"%s\";\n",str.Data());
      buffer.AppendFormatted("   frame->AddElement(new XMLToFormElement(\"%s\",\"Graph%s\",writeString.Data(),\"\", 0, &entries,comment.Data()));\n",
                             histoParameterWidgetTypes->At(i).Data(),graphParameters->At(i).Data());
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   WriteConfigToFormSubMethods(buffer,mainParGroup,"","", 0, 1);

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigToFormH() {
   int i,j,k;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sConfigToForm.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sConfigToForm_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sConfigToForm_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sConfigToForm", shortCut.Data());
   clsDescription.SetFormatted("This class makes form of configuration for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMEHisto.h\"\n");
   buffer.AppendFormatted("#include \"ROMEGraph.h\"\n");
   buffer.AppendFormatted("#include \"ROMEConfigToForm.h\"\n");

   // Class
   buffer.AppendFormatted("\nclass %sConfigToForm : public ROMEConfigToForm\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   // Constructor
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sConfigToForm(const %sConfigToForm &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("   %sConfigToForm &operator=(const %sConfigToForm &c); // not implemented\n",
                          shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sConfigToForm();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sConfigToForm() {}\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void AddConfig(XMLToFormFrame *frame);\n");
   buffer.AppendFormatted("   void AddHisto(XMLToFormFrame *frame,ROMEHisto* histo,Int_t histoDimension);\n");
   buffer.AppendFormatted("   void AddGraph(XMLToFormFrame *frame,ROMEGraph* graph);\n");
   for (i = 0; i < mainParGroup->GetNumberOfSubGroups(); i++) {
      buffer.AppendFormatted("   void Add%sFrame(XMLToFormFrame *frame);\n",
                             mainParGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (mainParGroup->GetSubGroupAt(i)->GetGroupName() == "Common") {
         for (j = 0; j < mainParGroup->GetSubGroupAt(i)->GetNumberOfSubGroups(); j++) {
            buffer.AppendFormatted("   void Add%sFrame(XMLToFormFrame *frame);\n",
                                   mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
            if (mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName() == "Folders") {
               for (k = 0; k < mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetNumberOfSubGroups(); k++)
                  buffer.AppendFormatted("   void Add%sFrame(XMLToFormFrame *frame);\n",
                                         mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetSubGroupAt(k)->GetGroupName().Data());
            }
         }
      }
   }

   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%sConfigToForm, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sConfigToForm_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigCpp() {
   int i;
   ROMEString str;
   ROMEString pointer;
   ROMEString path;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString cppFile;
   ROMEString buffer;

   int iSub = 0;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfig.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <typeinfo>\n");
   buffer.AppendFormatted("#include \"ROME.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]) {
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data());
   }
   for (i = 0; i < numOfDB; i++)
      buffer.AppendFormatted("#include \"databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[i].Data());

   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEUtilities.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXML.h\"\n");
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   if (sql)
      buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");

   buffer.AppendFormatted("\nClassImp(%sConfig)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sConfig::%sConfig()\n",shortCut.Data(),shortCut.Data());

   buffer.AppendFormatted(":fConfigData(new ConfigData*[1])\n");
   buffer.AppendFormatted(",fXSDFile(\"\")\n");
   buffer.AppendFormatted(",fNumberOfRunConfigs(0)\n");
   buffer.AppendFormatted(",fActiveConfiguration(0)\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("}\n\n");

   // Destructor
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sConfig::~%sConfig()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   for (i = 0; i < fNumberOfRunConfigs + 1; i++)\n");
   buffer.AppendFormatted("      SafeDelete(fConfigData[i]);\n");
   buffer.AppendFormatted("   SafeDeleteArray(fConfigData);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Read Configuration File
   buffer.AppendFormatted("\n// Read Configuration File\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfigurationFile(const char *file)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   // Fill config file content\n");
   buffer.AppendFormatted("   ifstream ifile(file);\n");
   buffer.AppendFormatted("   if (ifile.good()) {\n");
   buffer.AppendFormatted("      fConfigFileName = file;\n");
   buffer.AppendFormatted("      fConfigContent.ReadFile(ifile);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ROMEString XMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(XMLFile);\n");
   buffer.AppendFormatted("   xml->GetPathAttribute(\"/Configuration\",\"xsi:noNamespaceSchemaLocation\",fXSDFile,\"romeConfig.xsd\");\n");
   buffer.AppendFormatted("   if (!ReadProgramConfiguration(xml))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   for (i = 0; i < fNumberOfRunConfigs + 1; i++)\n");
   buffer.AppendFormatted("      SafeDelete(fConfigData[i]);\n");
   buffer.AppendFormatted("   SafeDeleteArray(fConfigData);\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = xml->NumberOfOccurrenceOfPath(\"/Configuration/RunConfiguration\");\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData*[fNumberOfRunConfigs + 1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   ROMEString path = \"/Configuration/MainConfiguration\";\n");
   buffer.AppendFormatted("   ReadConfiguration(xml,path, 0);\n");
   buffer.AppendFormatted("   CheckConfigurationModified(0);\n");
   buffer.AppendFormatted("   if (!SetConfiguration(0, 0))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   for (i = 0; i < fNumberOfRunConfigs; i++) {\n");
   buffer.AppendFormatted("      fConfigData[i + 1] = new ConfigData();\n");
   buffer.AppendFormatted("      path.SetFormatted(\"/Configuration/RunConfiguration[%%d]\",i + 1);\n");
   buffer.AppendFormatted("      ReadConfiguration(xml,path,i + 1);\n");
   buffer.AppendFormatted("      CheckConfigurationModified(i + 1);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fOffline->fRunNumbersModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetRunNumbers(fConfigData[0]->fOffline->fRunNumbers);\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fOffline->fInputFileNamesModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetInputFileNames(fConfigData[0]->fOffline->fInputFileNames);\n");

   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Program Configuration
   buffer.AppendFormatted("\n// Read Program Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::ReadProgramConfiguration(ROMEXML *xml)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString buffer;\n");
   buffer.AppendFormatted("   xml->GetPathValue(\"/Configuration/ProgramConfiguration/ProgramMode\",buffer,\"\");\n");
   buffer.AppendFormatted("   buffer.ToLower();\n");
   buffer.AppendFormatted("   if (buffer == \"0\")\n");
   buffer.AppendFormatted("      gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("   else if (buffer == \"1\")\n");
   buffer.AppendFormatted("      gAnalyzer->SetStandAloneARGUS();\n");
   buffer.AppendFormatted("   else if (buffer == \"2\")\n");
   buffer.AppendFormatted("      gAnalyzer->SetROMEAndARGUS();\n");
   buffer.AppendFormatted("   else if (buffer == \"3\")\n");
   buffer.AppendFormatted("      gAnalyzer->SetROMEMonitor();\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      Error(\"ReadProgramConfiguration\",\"Program mode must be one of the following :\\n");
   buffer.AppendFormatted("0 : analyzer\\n");
   buffer.AppendFormatted("1 : monitor\\n");
   buffer.AppendFormatted("2 : analyzer and monitor\\n");
   buffer.AppendFormatted("3 : monitor connected to an analyzer.\");\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t indx)\n{\n",
                          shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[indx];\n",shortCut.Data());
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEString tempPath;\n");
   buffer.AppendFormatted("   tempPath = \"\";\n"); // to suppress unused warning
   iSub = 0;
   WriteConfigRead(buffer,mainParGroup, 1,"","","configData->","",&iSub);
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfiguration(Long64_t runNumber)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   int i,j,ret;\n");
   buffer.AppendFormatted("   if (fActiveConfiguration != 0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(fActiveConfiguration, 0))\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   for (i = 1; i <= fNumberOfRunConfigs; i++) {\n");
   buffer.AppendFormatted("      if (fConfigData[i]->fOffline->fRunNumbersModified) {\n");
   buffer.AppendFormatted("         if (fConfigData[i]->fLastRunNumberIndex != -1) {\n");
   buffer.AppendFormatted("            for (j = fConfigData[i]->fLastRunNumberIndex; j < fConfigData[i]->fRunNumberArray.GetSize(); j++) {\n");
   buffer.AppendFormatted("               ret = gAnalyzer->CheckNumber(runNumber,fConfigData[i]->fRunNumberArray);\n");
   buffer.AppendFormatted("               if (ret == -1) {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = -1;\n");
   buffer.AppendFormatted("                  break;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else if (ret == 1) {\n");
   buffer.AppendFormatted("                  if (!SetConfiguration(i,i))\n");
   buffer.AppendFormatted("                     return false;\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = j;\n");
   buffer.AppendFormatted("                  return true;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfiguration(const char *file)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString fileName = file;\n");
   buffer.AppendFormatted("   int i,j;\n");
   buffer.AppendFormatted("   if (fActiveConfiguration != 0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(fActiveConfiguration, 0))\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   for (i = 1; i <= fNumberOfRunConfigs; i++) {\n");
   buffer.AppendFormatted("      if (fConfigData[i]->fOffline->fInputFileNamesModified) {\n");
   buffer.AppendFormatted("         if (fConfigData[i]->fLastInputFileNameIndex != -1) {\n");
   buffer.AppendFormatted("            for (j = fConfigData[i]->fLastInputFileNameIndex; j < fConfigData[i]->fInputFileNameArray.GetEntriesFast(); j++) {\n");
   buffer.AppendFormatted("               if (fConfigData[i]->fInputFileNameArray.At(j) == fileName) {\n");
   buffer.AppendFormatted("                  if (!SetConfiguration(i,i))\n");
   buffer.AppendFormatted("                     return false;\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else if (fConfigData[i]->fInputFileNameArray.At(j) < fileName) {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = -1;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Write Configuration File
   buffer.AppendFormatted("\n// Write Configuration File\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfigurationFile(const char *file) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString XMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForWrite(XMLFile))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   xml->SetTranslate(0);\n");
   buffer.AppendFormatted("   xml->WriteComment(\"$%s: $\");\n", "Id"); // to avoid Id: is replaced by SVN.
   buffer.AppendFormatted("   xml->WriteEmptyLine();\n");
   buffer.AppendFormatted("   xml->WriteComment(\"Please check ROME homepage for details of tags\\n\"\n");
   buffer.AppendFormatted("      \"http://midas.psi.ch/rome/romeConfigSchemaDocumentation.htm\");\n");
   buffer.AppendFormatted("   xml->WriteEmptyLine();\n");
   buffer.AppendFormatted("   xml->StartElement(\"Configuration\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xmlns:xsi\",\"http://www.w3.org/2001/XMLSchema-instance\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xsi:noNamespaceSchemaLocation\",fXSDFile.Data());\n");
   buffer.AppendFormatted("   WriteProgramConfiguration(xml);\n");
   buffer.AppendFormatted("   xml->WriteEmptyLine();\n");
   buffer.AppendFormatted("   xml->StartElement(\"MainConfiguration\");\n");
   buffer.AppendFormatted("   WriteConfiguration(xml, 0);\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   for (int i = 0; i < fNumberOfRunConfigs; i++) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"RunConfiguration\");\n");
   buffer.AppendFormatted("      WriteConfiguration(xml,i + 1);\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   xml->EndDocument();\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write Program Configuration
   buffer.AppendFormatted("\n// Write Program Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::WriteProgramConfiguration(ROMEXML *xml) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   xml->StartElement(\"ProgramConfiguration\");\n");
   buffer.AppendFormatted("   if (fCommentLevel >= 2)\n");
   configXSD->GetPathValue("/xs:schema/xs:complexType[@name='ProgramConfigurationDesc']/xs:sequence/xs:element[@name=ProgramMode]/xs:annotation/xs:documentation",
                           str, "");
   buffer.AppendFormatted("      xml->WriteComment(\"%s\");\n",str.Data());
   buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneROME())\n");
   buffer.AppendFormatted("      xml->WriteElement(\"ProgramMode\",\"0\");\n");
   buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneARGUS())\n");
   buffer.AppendFormatted("      xml->WriteElement(\"ProgramMode\",\"1\");\n");
   buffer.AppendFormatted("   if (gAnalyzer->IsROMEAndARGUS())\n");
   buffer.AppendFormatted("      xml->WriteElement(\"ProgramMode\",\"2\");\n");
   buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
   buffer.AppendFormatted("      xml->WriteElement(\"ProgramMode\",\"3\");\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfig2Cpp() {
   int i;
   ROMEString str;
   ROMEString pointer;
   ROMEString path;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString cppFile;
   ROMEString buffer;

   int iSub = 0;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfig2.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <typeinfo>\n");
   buffer.AppendFormatted("#include \"ROME.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]) {
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data());
   }
   for (i = 0; i < numOfDB; i++)
      buffer.AppendFormatted("#include \"databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[i].Data());

   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEUtilities.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXML.h\"\n");
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   if (sql)
      buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");

   buffer.AppendFormatted("\nClassImp(%sConfig)\n",shortCut.Data());

   // Check Configuration Modified
   buffer.AppendFormatted("\n// Check Configuration Modified\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfigurationModified(Int_t indx) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   ROMEString tempPath;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[indx];\n",shortCut.Data());
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   iSub = 0;
   WriteConfigCheckModified(buffer,mainParGroup, 1,"","","configData->","",&iSub);
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfig3Cpp() {
   int i;
   ROMEString str;
   ROMEString pointer;
   ROMEString path;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString cppFile;
   ROMEString buffer;

   int iSub = 0;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfig3.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <typeinfo>\n");
   buffer.AppendFormatted("#include \"ROME.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]) {
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   buffer.AppendFormatted("#include \"generated/%s%sConfigParameters.h\"\n", shortCut.Data(), mainProgName.Data());
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data());
   }
   for (i = 0; i < numOfDB; i++)
      buffer.AppendFormatted("#include \"databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[i].Data());

   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEUtilities.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXML.h\"\n");
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   if (sql)
      buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");

   buffer.AppendFormatted("\nClassImp(%sConfig)\n",shortCut.Data());

   // Set Configuration
   buffer.AppendFormatted("\n// Set Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::SetConfiguration(Int_t modIndex,Int_t indx)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[indx];\n",shortCut.Data());
   buffer.AppendFormatted("   %sConfig::ConfigData *modConfigData = fConfigData[modIndex];\n",shortCut.Data());
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   char* cstop;\n");
   buffer.AppendFormatted("   cstop=0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   Int_t inttmp;\n");
   buffer.AppendFormatted("   inttmp = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEString path = \"\";\n");
   buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
   buffer.AppendFormatted("   int ind;\n");
   buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   str = \"\";\n"); // to suppress unused warning
   buffer.AppendFormatted("   fActiveConfiguration = indx;\n");
   iSub = 0;
   WriteConfigSet(buffer,mainParGroup, 1,"","",&iSub);
   // end
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfig4Cpp() {
   int i;
   ROMEString str;
   ROMEString pointer;
   ROMEString path;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString cppFile;
   ROMEString buffer;

   int iSub = 0;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfig4.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("\n");

#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <typeinfo>\n");
   buffer.AppendFormatted("#include \"ROME.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderUserCode[i]) {
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         } else {
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         }
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   for (i = 0; i < daqNameArray->GetEntriesFast(); i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),
                             daqNameArray->At(i).Data());
   }
   for (i = 0; i < numOfDB; i++)
      buffer.AppendFormatted("#include \"databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[i].Data());

   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEUtilities.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXML.h\"\n");
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   if (sql)
      buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");

   buffer.AppendFormatted("\nClassImp(%sConfig)\n",shortCut.Data());

   // Write Configuration
   buffer.AppendFormatted("\n// Write Configuration\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfiguration(ROMEXML *xml,Int_t indx) const\n{\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[indx];\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEStrArray histoComments;\n");
   for (i = 0; i < histoParameters->GetEntriesFast(); i++) {
      path.SetFormatted("/xs:schema/xs:complexType[@name='HistogramDesc']/xs:sequence/xs:element[@name=Hist%s]/xs:annotation/xs:documentation",
                        histoParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("   histoComments.AddLast(\"%s\");\n",str.Data());
   }
   buffer.AppendFormatted("   ROMEStrArray graphComments;\n");
   for (i = 0; i < graphParameters->GetEntriesFast(); i++) {
      path.SetFormatted("/xs:schema/xs:complexType[@name='GraphDesc']/xs:sequence/xs:element[@name=Graph%s]/xs:annotation/xs:documentation",
                        graphParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("   graphComments.AddLast(\"%s\");\n",str.Data());
   }
   iSub = 0;
   WriteConfigWrite(buffer,mainParGroup, 1,"","",&iSub);
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteConfigH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sConfig.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sConfig_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sConfig_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include <TArrayL64.h>\n");
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMEStrArray.h\"\n");
   buffer.AppendFormatted("#include \"ROMEConfigHisto.h\"\n");
   buffer.AppendFormatted("#include \"ROMEConfigGraph.h\"\n");
   buffer.AppendFormatted("#include \"ROMEConfig.h\"\n");
   buffer.AppendFormatted("class ROMEXML;\n");

   // Class
   buffer.AppendFormatted("\nclass %sConfig : public ROMEConfig\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#if !defined(__CINT__)\n");


   // sub classes
   buffer.AppendFormatted("public:\n");
   WriteConfigClass(buffer,mainParGroup, 1);
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("#endif\n"); // defined(__CINT__)

   // Fields
   buffer.AppendFormatted("   ROMEString fXSDFile;\n");
   buffer.AppendFormatted("   Int_t fNumberOfRunConfigs;\n");
   buffer.AppendFormatted("   Int_t fActiveConfiguration;\n");
   buffer.AppendFormatted("\n");

   // Methods
   // Constructor
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sConfig(const %sConfig &c); // not implemented\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   %sConfig &operator=(const %sConfig &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sConfig();\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("   Bool_t WriteConfigurationFile(const char *file) const;\n");
   buffer.AppendFormatted("   Bool_t ReadConfigurationFile(const char *file);\n");
   buffer.AppendFormatted("   Bool_t CheckConfiguration(Long64_t runNumber);\n");
   buffer.AppendFormatted("   Bool_t CheckConfiguration(const char *file);\n");
   buffer.AppendFormatted("   Bool_t CheckConfigurationModified(Int_t indx) const;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   Bool_t ReadProgramConfiguration(ROMEXML *xml);\n");
   buffer.AppendFormatted("   Bool_t ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t indx);\n");
   buffer.AppendFormatted("   Bool_t WriteProgramConfiguration(ROMEXML *xml) const;\n");
   buffer.AppendFormatted("   Bool_t WriteConfiguration(ROMEXML *xml,Int_t indx) const;\n");
   buffer.AppendFormatted("   Bool_t SetConfiguration(Int_t modIndex,Int_t indx);\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("   ClassDef(%sConfig, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sConfig_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteMidasDAQCpp() {
   int i,j,k;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   hasStructuredBank = false;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sMidasDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sMidasDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the midas odb access for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <math.h>\n");
   buffer.AppendFormatted("#include \"ROMEEventLoop.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sMidasDAQ.h\"\n",shortCut.Data());
   if (bankHasHeader) {
      buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),bankHeaderFolder.Data());
   }
   buffer.AppendFormatted("#include \"ROMEUtilities.h\"\n");

   buffer.AppendFormatted("\nClassImp(%sMidasDAQ)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sMidasDAQ::%sMidasDAQ()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(":ROMEMidasDAQ()\n");
   for (i = 0; i < numOfEvent; i++) {
      for (j = 0; j < numOfBank[i]; j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
         } else {   // Single Bank
            buffer.AppendFormatted(",f%sBankPointer(0)\n",bankAlias[i][j].Data());
            buffer.AppendFormatted(",fLast%sBankPointer(0)\n",bankAlias[i][j].Data());
            buffer.AppendFormatted(",f%sBankLength(0)\n",bankAlias[i][j].Data());
            buffer.AppendFormatted(",fLast%sBankLength(0)\n",bankAlias[i][j].Data());
            buffer.AppendFormatted(",f%sBankExists(kFALSE)\n",bankAlias[i][j].Data());
            buffer.AppendFormatted(",fLast%sBankExists(kFALSE)\n",bankAlias[i][j].Data());
         }
         buffer.AppendFormatted(",f%sBankActive(kTRUE)\n",bankAlias[i][j].Data());
      }
      buffer.AppendFormatted(",f%sEventActive(kTRUE)\n",eventName[i].Data());
   }
   if (midas) {
      buffer.AppendFormatted("   // Hot Links\n");
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted(",f%s%sHotLinks()\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted(",fGSPHotLinks()\n");
      for (i = 0; i < numOfTab; i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted(",f%s%sHotLinks()\n",tabName[i].Data(),tabSuffix[i].Data());
      }
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("{\n");
   // Event Requests
   buffer.AppendFormatted("   // Event Requests\n");
   buffer.AppendFormatted("   SetNumberOfEventRequests(%d);\n",numOfEvent);
   for (i = 0; i < numOfEvent; i++) {
      buffer.AppendFormatted("   SetEventRequestID(%d,%s);\n",i,eventID[i].Data());
      buffer.AppendFormatted("   SetEventRequestMask(%d,%s);\n",i,eventTriggerMask[i].Data());
      buffer.AppendFormatted("   SetEventRequestRate(%d,%s);\n",i,eventSamplingRate[i].Data());
   }

   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Hot Links call back
   if (midas) {
      buffer.AppendFormatted("// Hot Links call back\n");
      buffer.AppendFormatted("void HotLinksChanged(HNDLE /*hDB*/,HNDLE /*hKey*/,void * /*info*/) {\n");
      buffer.AppendFormatted("   ROMEEventLoop::fHotLinksChanged = true;\n");
      buffer.AppendFormatted("}\n\n");
   }

   // Fill Bank Header to Folder
   buffer.AppendFormatted("// Fill Bank Header to Folder\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sMidasDAQ::InitHeader()\n{\n",shortCut.Data());
   if (bankHasHeader) {
      if (bankHeaderEventID != "")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(reinterpret_cast<EVENT_HEADER*>(GetRawDataEvent())->event_id);\n",
                                bankHeaderFolder.Data(),bankHeaderEventID.Data());
      if (bankHeaderTriggerMask != "")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(reinterpret_cast<EVENT_HEADER*>(GetRawDataEvent())->trigger_mask);\n",
                                bankHeaderFolder.Data(),bankHeaderTriggerMask.Data());
      if (bankHeaderSerialNumber != "")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(reinterpret_cast<EVENT_HEADER*>(GetRawDataEvent())->serial_number);\n",
                                bankHeaderFolder.Data(),bankHeaderSerialNumber.Data());
      if (bankHeaderTimeStamp != "")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(reinterpret_cast<EVENT_HEADER*>(GetRawDataEvent())->time_stamp);\n",
                                bankHeaderFolder.Data(),bankHeaderTimeStamp.Data());
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Initialize ODB
   if (midas) {
      ROMEString midType;
      buffer.AppendFormatted("// InitODB\n");
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("Bool_t %sMidasDAQ::InitODB()\n{\n",shortCut.Data());
      ROMEString steerPath;
      buffer.AppendFormatted("   HNDLE hKey;\n");
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   ROMEString hotLinkString;\n");
      buffer.AppendFormatted("   // Hot Links\n");
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   str = \"/%s%s/Tasks/%s%s\";\n",shortCut.Data(),mainProgName.Data(),
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("   hotLinkString =  \"Active = BOOL : 0\\n");
         for (j = 0; j < numOfSteering[i]; j++) {
            for (k = 0; k < numOfSteerFields[i][j]; k++) {
               if (steerFieldHotLink[i][j][k]) {
                  GetSteerPath(steerPath,i,j,k,"_");
                  if (!toMidasODBType(steerFieldType[i][j][k],midType)) {
                     cout<<"Steering parameter field "<<steerFieldName[i][j][k].Data()<<" of task "
                         <<taskHierarchyName[i].Data()<<" can not be a hot link"<<endl;
                     return false;
                  }
                  buffer.AppendFormatted("%s = %s : 0\\n",steerPath.Data(),midType.Data());
               }
            }
         }
         buffer.AppendFormatted("\";\n");
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), const_cast<char*>(hotLinkString.Data()), TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), &hKey);\n");
         buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,&f%s%sHotLinks,sizeof(%s%sHotLinks), 0) != DB_SUCCESS) {\n",
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot write to hot links.\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, &f%s%sHotLinks, sizeof(%s%sHotLinks), MODE_READ, HotLinksChanged, 0) != DB_SUCCESS) {\n",
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot open hot links, probably other analyzer is using it\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
      }
      bool hasHotLink = false;
      buffer.AppendFormatted("   str = \"/%s%s/Global Steering Parameters\";\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("   hotLinkString =  \"\\n");
      for (j = 0; j < numOfSteering[numOfTaskHierarchy]; j++) {
         for (k = 0; k < numOfSteerFields[numOfTaskHierarchy][j]; k++) {
            if (steerFieldHotLink[numOfTaskHierarchy][j][k]) {
               if (!toMidasODBType(steerFieldType[numOfTaskHierarchy][j][k],midType)) {
                  cout<<"General steering parameter field "<<steerFieldName[numOfTaskHierarchy][j][k].Data()
                      <<" can not be a hot link"<<endl;
                  return false;
               }
               hasHotLink = true;
               GetSteerPath(steerPath,numOfTaskHierarchy,j,k,"_");
               buffer.AppendFormatted("%s = %s : 0\\n",steerPath.Data(),midType.Data());
            }
         }
      }
      buffer.AppendFormatted("\";\n");
      if (hasHotLink) {
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), const_cast<char*>(hotLinkString.Data()), TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), &hKey);\n");
         buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,gAnalyzer->GetMidasDAQ()->GetGSPHotLinks(),sizeof(GSPHotLinks), 0) != DB_SUCCESS) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot write to hot links.\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetMidasDAQ()->GetGSPHotLinks(), sizeof(GSPHotLinks), MODE_READ, HotLinksChanged, 0) != DB_SUCCESS) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot open hot links, probably other analyzer is using it\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
      }
      for (i = 0; i < numOfTab; i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("   str = \"/%s%s/Tabs/%s%s\";\n",shortCut.Data(),mainProgName.Data(),tabName[i].Data(),
                                tabSuffix[i].Data());
         buffer.AppendFormatted("   hotLinkString =  \"Active = BOOL : 0\\n");
         for (j = 0; j < numOfSteering[numOfTaskHierarchy + 1 + i]; j++) {
            for (k = 0; k < numOfSteerFields[numOfTaskHierarchy + 1 + i][j]; k++) {
               if (steerFieldHotLink[numOfTaskHierarchy + 1 + i][j][k]) {
                  if (!toMidasODBType(steerFieldType[numOfTaskHierarchy + 1 + i][j][k],midType)) {
                     cout<<"Steering parameter field "<<steerFieldName[numOfTaskHierarchy + 1 + i][j][k].Data()<<" of tab "
                         <<tabName[i].Data()<<" can not be a hot link"<<endl;
                     return false;
                  }
                  GetSteerPath(steerPath,numOfTaskHierarchy + 1 + i,j,k,"_");
                  buffer.AppendFormatted("%s = %s : 0\\n",steerPath.Data(),midType.Data());
               }
            }
         }
         buffer.AppendFormatted("\";\n");
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), const_cast<char*>(hotLinkString.Data()), TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), &hKey);\n");
         buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,&f%s%sHotLinks,sizeof(%s%sHotLinks), 0) != DB_SUCCESS) {\n",
                                tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot write to hot links.\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, &f%s%sHotLinks, sizeof(%s%sHotLinks), MODE_READ, HotLinksChanged, 0) != DB_SUCCESS) {\n",
                                tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot open hot links, probably other analyzer is using it\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
      }

      // Statistics
      char *cstop = 0;
      if (numOfEvent) {
         buffer.AppendFormatted("   char *triggerStatisticsString = const_cast<char*>(\"Events received = DOUBLE : 0\\n"
                                "Events per sec. = DOUBLE : 0\\n"
                                "Events written = DOUBLE : 0\\n\");\n");
      }
      for (i = 0; i < numOfEvent; i++) {
         buffer.AppendFormatted("   str = \"//%s/Statistics\";\n", eventName[i].Data());
         buffer.AppendFormatted("   str.Insert(1, gAnalyzer->GetOnlineAnalyzerName());\n");
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()),\n");
         buffer.AppendFormatted("                   triggerStatisticsString, TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, const_cast<char*>(str.Data()), &hKey);\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetStatisticsAt(%d),\n",
                                static_cast<Int_t>(strtol(eventID[i], &cstop, 10) - 1));
         buffer.AppendFormatted("                      sizeof(Statistics), MODE_WRITE, 0, 0) != DB_SUCCESS) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"\\nCan not open %s statistics record, probably other analyzer is using it\\n\");\n",
                                eventName[i].Data());
         buffer.AppendFormatted("   }\n");
      }

      buffer.AppendFormatted("   return true;\n");
      buffer.AppendFormatted("}\n\n");
   }

   // Midas Bank Initialisation
   buffer.AppendFormatted("// Midas Bank Initialisation\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sMidasDAQ::InitMidasBanks()\n{\n",shortCut.Data());
   bool done = false;
   for (i = 0; i < numOfEvent && !done; i++) {
      for (j = 0; j < numOfBank[i] && !done; j++) {
         if (bankArrayDigit[i][j]>0) {
            buffer.AppendFormatted("   int i;\n");
            done = true;
         }
      }
   }
   for (i = 0; i < numOfEvent; i++) {
      for (j = 0; j < numOfBank[i]; j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            buffer.AppendFormatted("   for (i = %d; i < %d; i++)\n",bankArrayStart[i][j],
                                   bankArraySize[i][j]+bankArrayStart[i][j]);
            buffer.AppendFormatted("      Init%sBank(i);\n",bankAlias[i][j].Data());
         } else {  // Single Bank
            buffer.AppendFormatted("   Init%sBank();\n",bankAlias[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Midas Bank Methodes
   buffer.AppendFormatted("\n// Midas Bank Getters\n");
   for (i = 0; i < numOfEvent; i++) {
      for (j = 0; j < numOfBank[i]; j++) {
         buffer.AppendFormatted("// %s bank\n",bankAlias[i][j].Data());
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            // Functions
            if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(Int_t bankIndex,Int_t indx) const\n{\n",
                                      bankAlias[i][j].Data(),shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex] + indx;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankPointer(Int_t bankIndex) const\n{\n",
                                      bankAlias[i][j].Data(),shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex];\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(Int_t bankIndex,Int_t indx) const\n{\n",
                                      bankAlias[i][j].Data(),shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex] + indx;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankPointer(Int_t bankIndex) const\n{\n",
                                      bankAlias[i][j].Data(),shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex];\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(Int_t bankIndex,Int_t indx) const\n{\n",
                                      bankType[i][j].Data(),shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer[bankIndex] + indx);\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return static_cast<%s>(exp(999.));\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %sMidasDAQ::Get%sBankPointer(Int_t bankIndex) const\n{\n",
                                      bankType[i][j].Data(), shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex];\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(Int_t bankIndex,Int_t indx) const\n{\n",
                                      bankType[i][j].Data(),shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer[bankIndex] + indx);\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return static_cast<%s>(exp(999.));\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %sMidasDAQ::GetLast%sBankPointer(Int_t bankIndex) const\n{\n",
                                      bankType[i][j].Data(), shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex];\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank(Int_t bankIndex)\n{\n",shortCut.Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID() == %s) {\n",
                                   eventName[i].Data(),bankAlias[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists[bankIndex] = f%sBankExists[bankIndex];\n",
                                   bankAlias[i][j].Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer[bankIndex] = f%sBankPointer[bankIndex];\n",
                                   bankAlias[i][j].Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength[bankIndex] = f%sBankLength[bankIndex];\n",
                                   bankAlias[i][j].Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("      DWORD bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(GetRawDataEvent());\n");
            buffer.AppendFormatted("      pevent++;\n");
            buffer.AppendFormatted("      ROMEString bankname;\n");
            buffer.AppendFormatted("      bankname.SetFormatted(\"%s%%0%dd\",bankIndex);\n",bankName[i][j].Data(),
                                   bankArrayDigit[i][j]);
            buffer.AppendFormatted("      if (bk_find(reinterpret_cast<BANK_HEADER*>(pevent), bankname.Data(), &f%sBankLength[bankIndex], &bktype, reinterpret_cast<void**>(&f%sBankPointer[bankIndex]))) {\n",
                                   bankAlias[i][j].Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("         f%sBankExists[bankIndex] = true;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("         return;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   f%sBankExists[bankIndex] = false;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("   f%sBankPointer[bankIndex] = 0;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("   f%sBankLength[bankIndex] = 0;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("   return;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");

            buffer.Append(kMethodLine);
            buffer.AppendFormatted("Int_t %sMidasDAQ::Get%sBankEntries(Int_t bankIndex) const\n{\n",shortCut.Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength[bankIndex];\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("Int_t %sMidasDAQ::GetLast%sBankEntries(Int_t bankIndex) const\n{\n",shortCut.Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength[bankIndex];\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         } else {   // Single Bank
            if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(Int_t indx) const\n{\n",bankAlias[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer + indx;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankPointer() const\n{\n",bankAlias[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(Int_t indx) const\n{\n",bankAlias[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer + indx;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankPointer() const\n{\n",bankAlias[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(Int_t indx) const\n{\n",bankType[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer + indx);\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return static_cast<%s>(exp(999.));\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %sMidasDAQ::Get%sBankPointer() const\n{\n",bankType[i][j].Data(),shortCut.Data(),
                                      bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(Int_t indx) const\n{\n",bankType[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer + indx);\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return static_cast<%s>(exp(999.));\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s* %sMidasDAQ::GetLast%sBankPointer() const\n{\n",bankType[i][j].Data(),
                                      shortCut.Data(),bankAlias[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   return 0;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank()\n{\n",shortCut.Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID() == %s) {\n",
                                   eventName[i].Data(),bankAlias[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists = f%sBankExists;\n",bankAlias[i][j].Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer = f%sBankPointer;\n",bankAlias[i][j].Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength = f%sBankLength;\n",bankAlias[i][j].Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("      DWORD bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = reinterpret_cast<EVENT_HEADER*>(GetRawDataEvent());\n");
            buffer.AppendFormatted("      pevent++;\n");
            buffer.AppendFormatted("      if (bk_find(reinterpret_cast<BANK_HEADER*>(pevent), \"%s\", &f%sBankLength, &bktype, reinterpret_cast<void**>(&f%sBankPointer))) {\n",
                                   bankName[i][j].Data(),bankAlias[i][j].Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("         f%sBankExists = true;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("         return;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   f%sBankExists = false;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("   f%sBankPointer = 0;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("   f%sBankLength = 0;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("   return;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");

            buffer.Append(kMethodLine);
            buffer.AppendFormatted("Int_t %sMidasDAQ::Get%sBankEntries() const\n{\n",shortCut.Data(),bankAlias[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.Append(kMethodLine);
            buffer.AppendFormatted("Int_t %sMidasDAQ::GetLast%sBankEntries() const\n{\n",shortCut.Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength;\n",bankAlias[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         }
      }
   }

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sMidasDAQ::IsActiveEventID(Int_t id) const\n{\n",shortCut.Data());
   if (numOfEvent <= 0) {
      buffer.AppendFormatted("   WarningSuppression(id);\n");
   }
   for (i = 0; i < numOfEvent; i++) {
      buffer.AppendFormatted("   if (f%sEventActive && id == %s)\n",eventName[i].Data(),eventID[i].Data());
      buffer.AppendFormatted("      return true;\n");
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // -- Append method for byte swapping bank structures that are defined ----------
   buffer.AppendFormatted( "\n//Used for byte swapping banks which are structs\n" );
   buffer.Append(kMethodLine);
   buffer.AppendFormatted( "void* %sMidasDAQ::ByteSwapStruct(char* aName, void* pData) const\n", shortCut.Data() );
   buffer.AppendFormatted( "{\n" );

   for ( long iEvent = 0; iEvent < numOfEvent; iEvent++ )
   {
      for ( long iBank = 0; iBank < numOfBank[ iEvent ]; iBank++ )
      {

         // We have a struct bank so create byte swapping code.
         if ( bankType[ iEvent ][ iBank ] == "structure" || bankType[ iEvent ][ iBank ] == "struct" )
         {
            if ( !hasStructuredBank )
            {
               buffer.AppendFormatted( "    if (strstr(aName, \"%s\"))\n", bankAlias[ iEvent ][ iBank ].Data() );
               hasStructuredBank = true;
            } else {
               buffer.AppendFormatted( "    else if (strstr(aName, \"%s\" ))\n", bankAlias[ iEvent ][ iBank ].Data() );
            }

            buffer.AppendFormatted( "    {\n" );

            // Loop through all fields for this structure and write out code to perform byte swapping.
            for ( long iField = 0; iField < numOfStructFields[ iEvent ][ iBank ]; iField++ )
            {
               if ( structFieldType[ iEvent ][ iBank ][ iField ] == "char" )
               {
                  buffer.AppendFormatted( "        pData = (void *)(((char*)pData) + %s); // %s\n",
                                          bankFieldArraySize[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
               } else if ( bankFieldArraySize[ iEvent ][ iBank ][ iField ] == "1" ) {
                  buffer.AppendFormatted( "        ROMEUtilities::ByteSwap((%s*)pData); // %s\n",
                                          structFieldType[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        pData = (void*)(((%s*)pData) + 1);\n", structFieldType[ iEvent ][ iBank ][ iField ].Data() );
               } else {
                  buffer.AppendFormatted( "        for (long i = 0; i < %s; i++)\n", bankFieldArraySize[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        {\n" );
                  buffer.AppendFormatted( "            ROMEUtilities::ByteSwap((%s*)pData); // %s\n",
                                          structFieldType[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "            pData = (void*)(((%s*)pData) + 1);\n", structFieldType[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        }\n" );
               }
            }    // End loop through fields.
            buffer.AppendFormatted( "    }\n" );

         }        // End if is structured bank.
      }            // End loop through banks.
   }                // End loop through events.

   buffer.AppendFormatted( "    return(pData);\n" );
   buffer.AppendFormatted( "    WarningSuppression(aName);\n" );
   buffer.AppendFormatted( "    return(pData);\n" );
   buffer.AppendFormatted( "}\n" );
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteMidasDAQH() {
   int i,j,k;

   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sMidasDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sMidasDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sMidasDAQ_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sMidasDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the midas odb access for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMEMidasDAQ.h\"\n");
   buffer.AppendFormatted("\n");

   // bank structures
   if (numOfBank>0)
      buffer.AppendFormatted("// Bank Structures\n");
   for (i = 0; i < numOfEvent; i++) {
      for (j = 0; j < numOfBank[i]; j++) {
         if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
            buffer.AppendFormatted("typedef struct {\n");
            for (k = 0; k < numOfStructFields[i][j]; k++) {
               if (bankFieldArraySize[i][j][k] == "1") {
                  if (structFieldSize[i][j][k].Length()>0) {
                     buffer.AppendFormatted("   %s %s : %s;\n",structFieldType[i][j][k].Data(),
                                            structFieldName[i][j][k].Data(),structFieldSize[i][j][k].Data());
                  } else {
                     buffer.AppendFormatted("   %s %s;\n",structFieldType[i][j][k].Data(),
                                            structFieldName[i][j][k].Data());
                  }
               } else {
                  if (structFieldSize[i][j][k].Length()>0) {
                     buffer.AppendFormatted("   %s %s[%s] : %s;\n",structFieldType[i][j][k].Data(),
                                            structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data(),
                                            structFieldSize[i][j][k].Data());
                  } else {
                     buffer.AppendFormatted("   %s %s[%s];\n",structFieldType[i][j][k].Data(),
                                            structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data());
                  }
               }
            }
            buffer.AppendFormatted("} %sStruct;\n",bankAlias[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Hot Links Structures
   if (midas) {
      ROMEString steerPath;
      buffer.AppendFormatted("// Hot Links Structures\n");
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("typedef struct{\n");
         buffer.AppendFormatted("   Int_t Active; //! Task Active\n");
         for (j = 0; j < numOfSteering[i]; j++) {
            for (k = 0; k < numOfSteerFields[i][j]; k++) {
               if (steerFieldHotLink[i][j][k]) {
                  GetSteerPath(steerPath,i,j,k,"_");
                  buffer.AppendFormatted("   %s %s; //! %s Steering Parameter\n",steerFieldType[i][j][k].Data(),
                                         steerPath.Data(),steerPath.Data());
               }
            }
         }
         buffer.AppendFormatted("} %s%sHotLinks; // Task %s%s\n",taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("typedef struct{\n");
      for (j = 0; j < numOfSteering[numOfTaskHierarchy]; j++) {
         for (k = 0; k < numOfSteerFields[numOfTaskHierarchy][j]; k++) {
            if (steerFieldHotLink[numOfTaskHierarchy][j][k]) {
               GetSteerPath(steerPath,numOfTaskHierarchy,j,k,"_");
               buffer.AppendFormatted("   %s %s; //! %s Steering Parameter\n",
                                      steerFieldType[numOfTaskHierarchy][j][k].Data(),steerPath.Data(),
                                      steerPath.Data());
            }
         }
      }
      buffer.AppendFormatted("} GSPHotLinks; // Global Steering Parameters\n");
      for (i = 0; i < numOfTab; i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("typedef struct{\n");
         buffer.AppendFormatted("   Int_t Active; //! Tab Active\n");
         for (j = 0; j < numOfSteering[numOfTaskHierarchy + 1 + i]; j++) {
            for (k = 0; k < numOfSteerFields[numOfTaskHierarchy + 1 + i][j]; k++) {
               if (steerFieldHotLink[numOfTaskHierarchy + 1 + i][j][k]) {
                  GetSteerPath(steerPath,numOfTaskHierarchy + 1 + i,j,k,"_");
                  buffer.AppendFormatted("   %s %s; //! %s Steering Parameter\n"
                                         ,steerFieldType[numOfTaskHierarchy + 1 + i][j][k].Data(),
                                         steerPath.Data(),steerPath.Data());
               }
            }
         }
         buffer.AppendFormatted("} %s%sHotLinks; // Tab %s%s\n",tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),
                                tabSuffix[i].Data());
      }
   }

   // Class
   buffer.AppendFormatted("\nclass %sMidasDAQ : public ROMEMidasDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Bank Fields
   int bankNameLen = -1;
   int bankTypeLen = -1;
   for (i = 0; i < numOfEvent; i++) {
      if (numOfBank[i]>0) {
         for (j = 0; j < numOfBank[i]; j++) {
            if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
               if (bankTypeLen < static_cast<int>(bankAlias[i][j].Length() + 6)) {
                  bankTypeLen = bankAlias[i][j].Length() + 6;
               }
            } else {
               if (bankTypeLen < static_cast<int>(bankType[i][j].Length())) {
                  bankTypeLen = bankType[i][j].Length();
               }
            }
            if (bankNameLen < static_cast<int>(bankAlias[i][j].Length() + bankArrayDigit[i][j])) {
               bankNameLen = bankAlias[i][j].Length()+bankArrayDigit[i][j];
            }
         }
      }
   }
   buffer.AppendFormatted("   // Bank Fields\n");
   for (i = 0; i < numOfEvent; i++) {
      for (j = 0; j < numOfBank[i]; j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
               buffer.AppendFormatted("   %sStruct*%*s f%sBankPointer[%d]; %*s //! Pointer to the %s Bank\n",
                                      bankAlias[i][j].Data(), bankTypeLen - bankAlias[i][j].Length() - 6, "",
                                      bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                      bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());

               buffer.AppendFormatted("   %sStruct*%*s fLast%sBankPointer[%d]; %*s //! Pointer to the %s Bank of the last event\n",
                                      bankAlias[i][j].Data(), bankTypeLen-bankAlias[i][j].Length() - 6, "",
                                      bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                      bankNameLen-bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
            } else {
               buffer.AppendFormatted("   %s*%*s f%sBankPointer[%d]; %*s //! Pointer to the %s Bank\n",
                                      bankType[i][j].Data(), bankTypeLen-bankType[i][j].Length(), "",
                                      bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                      bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
               buffer.AppendFormatted("   %s*%*s fLast%sBankPointer[%d]; %*s //! Pointer to the %s Bank of the last event\n",
                                      bankType[i][j].Data(), bankTypeLen-bankType[i][j].Length(), "",
                                      bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                      bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
            }
            buffer.AppendFormatted("   DWORD%*s f%sBankLength[%d];  %*s //! Length  of the %s Bank\n",
                                   bankTypeLen - 5, "", bankAlias[i][j].Data(), bankArraySize[i][j]+bankArrayStart[i][j],
                                   bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
            buffer.AppendFormatted("   DWORD%*s fLast%sBankLength[%d];  %*s //! Length  of the %s Bank of the last event\n",
                                   bankTypeLen - 5, "", bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                   bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
            buffer.AppendFormatted("   Bool_t%*s f%sBankExists[%d];  %*s //! Exist Flags of the %s Bank\n",
                                   bankTypeLen - 6, "", bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                   bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
            buffer.AppendFormatted("   Bool_t%*s fLast%sBankExists[%d];  %*s //! Exist Flags of the %s Bank of the last event\n",
                                   bankTypeLen - 6, "", bankAlias[i][j].Data(), bankArraySize[i][j] + bankArrayStart[i][j],
                                   bankNameLen - bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
         } else {  // Single Bank
            if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
               buffer.AppendFormatted("   %sStruct*%*s f%sBankPointer; %*s //! Pointer to the %s Bank\n",
                                      bankAlias[i][j].Data(), bankTypeLen - bankAlias[i][j].Length() - 6, "",
                                      bankAlias[i][j].Data(), bankNameLen-bankAlias[i][j].Length(), "", bankAlias[i][j].Data());
               buffer.AppendFormatted("   %sStruct*%*s fLast%sBankPointer; %*s //! Pointer to the %s Bank of the last event\n",
                                      bankAlias[i][j].Data(), bankTypeLen - bankAlias[i][j].Length() - 6, "",
                                      bankAlias[i][j].Data(), bankNameLen - bankAlias[i][j].Length(), "",
                                      bankAlias[i][j].Data());
            } else {
               buffer.AppendFormatted("   %s*%*s f%sBankPointer; %*s //! Pointer to the %s Bank\n",
                                      bankType[i][j].Data(), bankTypeLen - bankType[i][j].Length(), "",
                                      bankAlias[i][j].Data(), bankNameLen - bankAlias[i][j].Length(), "",
                                      bankAlias[i][j].Data());
               buffer.AppendFormatted("   %s*%*s fLast%sBankPointer; %*s //! Pointer to the %s Bank of the last event\n",
                                      bankType[i][j].Data(), bankTypeLen - bankType[i][j].Length(), "",
                                      bankAlias[i][j].Data(), bankNameLen - bankAlias[i][j].Length(), "",
                                      bankAlias[i][j].Data());
            }
            buffer.AppendFormatted("   DWORD%*s f%sBankLength;  %*s //! Length  of the %s Bank\n",
                                   bankTypeLen - 5, "", bankAlias[i][j].Data(), bankNameLen-bankAlias[i][j].Length(), "",
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   DWORD%*s fLast%sBankLength;  %*s //! Length  of the %s Bank of the last event\n",
                                   bankTypeLen - 5, "", bankAlias[i][j].Data(), bankNameLen - bankAlias[i][j].Length(), "",
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   Bool_t%*s f%sBankExists;  %*s //! Exist Flags of the %s Bank\n",
                                   bankTypeLen - 6, "", bankAlias[i][j].Data(), bankNameLen - bankAlias[i][j].Length(), "",
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   Bool_t%*s fLast%sBankExists;  %*s //! Exist Flags of the %s Bank of the last event\n",
                                   bankTypeLen - 6, "", bankAlias[i][j].Data(), bankNameLen - bankAlias[i][j].Length(), "",
                                   bankAlias[i][j].Data());
         }
         buffer.AppendFormatted("   Bool_t%*s f%sBankActive;  %*s //! Active Flags of the %s Bank\n",bankTypeLen-6,"",
                                bankAlias[i][j].Data(),bankNameLen-bankAlias[i][j].Length(),"",bankAlias[i][j].Data());
      }
      buffer.AppendFormatted("   Bool_t%*s f%sEventActive;  %*s //! Active Flags of the %s Bank\n", bankTypeLen - 6, "",
                             eventName[i].Data(), bankNameLen-eventName[i].Length(), "", eventName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Hot Links
   if (midas) {
      buffer.AppendFormatted("   // Hot Links\n");
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks f%s%sHotLinks;               //! %s%s Hot Links\n",
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("   GSPHotLinks fGSPHotLinks;               //! GSP Hot Links\n");
      for (i = 0; i < numOfTab; i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks f%s%sHotLinks;               //! %s%s Hot Links\n",tabName[i].Data(),
                                tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),
                                tabSuffix[i].Data());
      }
      buffer.AppendFormatted("\n");
   }


   // Methods
   // Constructor
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sMidasDAQ(const %sMidasDAQ &c); // not implemented\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   %sMidasDAQ &operator=(const %sMidasDAQ &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sMidasDAQ();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sMidasDAQ() {}\n",shortCut.Data());

   // Banks
   buffer.AppendFormatted("   // Bank Methodes\n");
   buffer.AppendFormatted("   void InitMidasBanks();\n");
   for (i = 0; i < numOfEvent; i++) {
      if (numOfBank[i]>0) {
         for (j = 0; j < numOfBank[i]; j++) {
            // Bank Array
            if (bankArrayDigit[i][j]>0) {
               if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
                  buffer.AppendFormatted("   %sStruct* Get%sBankAt(Int_t bankIndex,Int_t indx) const;\n",
                                         bankAlias[i][j].Data(),bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* Get%sBankPointer(Int_t bankIndex) const;\n",bankAlias[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(Int_t bankIndex,Int_t indx) const;\n",
                                         bankAlias[i][j].Data(),bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankPointer(Int_t bankIndex) const;\n",bankAlias[i][j].Data(),
                                         bankAlias[i][j].Data());
               } else {
                  buffer.AppendFormatted("   %s  Get%sBankAt(Int_t bankIndex,Int_t indx) const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %s* Get%sBankPointer(Int_t bankIndex) const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %s  GetLast%sBankAt(Int_t bankIndex,Int_t indx) const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %s* GetLast%sBankPointer(Int_t bankIndex) const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
               }
               buffer.AppendFormatted("   Int_t Get%sBankEntries(Int_t bankIndex) const;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   Int_t GetLast%sBankEntries(Int_t bankIndex) const;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   void Init%sBank(Int_t bankIndex);\n",bankAlias[i][j].Data());
            } else {         // Single Bank
               if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
                  buffer.AppendFormatted("   %sStruct* Get%sBankAt(Int_t indx) const;\n",bankAlias[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* Get%sBankPointer() const;\n",bankAlias[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(Int_t indx) const;\n",bankAlias[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankPointer() const;\n",bankAlias[i][j].Data(),
                                         bankAlias[i][j].Data());
               } else {
                  buffer.AppendFormatted("   %s  Get%sBankAt(Int_t indx) const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %s* Get%sBankPointer() const;\n",bankType[i][j].Data(),bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %s  GetLast%sBankAt(Int_t indx) const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
                  buffer.AppendFormatted("   %s* GetLast%sBankPointer() const;\n",bankType[i][j].Data(),
                                         bankAlias[i][j].Data());
               }
               buffer.AppendFormatted("   Int_t Get%sBankEntries() const;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   Int_t GetLast%sBankEntries() const;\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("   void Init%sBank();\n",bankAlias[i][j].Data());
            }
            buffer.AppendFormatted("   bool is%sBankActive() const { return f%sBankActive; }\n",bankAlias[i][j].Data(),
                                   bankAlias[i][j].Data());
            buffer.AppendFormatted("   void Set%sBankActive(bool flag) { f%sBankActive = flag; }\n",
                                   bankAlias[i][j].Data(),bankAlias[i][j].Data());
         }
      }
      buffer.AppendFormatted("   bool is%sEventActive() const { return f%sEventActive; }\n",eventName[i].Data(),
                             eventName[i].Data());
      buffer.AppendFormatted("   void Set%sEventActive(bool flag) { f%sEventActive = flag; }\n",eventName[i].Data(),
                             eventName[i].Data());
      buffer.AppendFormatted("\n");
   }

   // Hot Links
   if (midas) {
      buffer.AppendFormatted("   // Hot Links\n");
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks* Get%s%sHotLinks() { return &f%s%sHotLinks; };\n",
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("   GSPHotLinks* GetGSPHotLinks() { return &fGSPHotLinks; };\n");
      for (i = 0; i < numOfTab; i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks* Get%s%sHotLinks() { return &f%s%sHotLinks; };\n",tabName[i].Data(),
                                tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),
                                tabSuffix[i].Data());
      }
      buffer.AppendFormatted("\n");
   }


   // protected
   buffer.AppendFormatted("protected:\n");
   if (midas) {
      buffer.AppendFormatted("   Bool_t  InitODB();\n");
   }
   buffer.AppendFormatted("   Bool_t  InitHeader();\n");
   buffer.AppendFormatted("   Bool_t  IsActiveEventID(Int_t id) const ;\n");
   buffer.AppendFormatted("   void *ByteSwapStruct( char* aName, void* pData ) const;\n");

   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%sMidasDAQ, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sMidasDAQ_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteRomeDAQCpp() {
   int i ,j;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString tmp;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sRomeDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sRomeDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT TTree access for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sRomeDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%s%sConfigParameters.h\"\n", shortCut.Data(), mainProgName.Data());
   for (i = 0; i < numOfTree; i++) {
      for (j = 0; j < numOfRunHeader[i]; j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            tmp.SetFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(), runHeaderFolder[i][j].Data());
            if (!buffer.ContainsFast(tmp)) {
               buffer.Append(tmp);
            }
         }
      }
   }

   buffer.AppendFormatted("\nClassImp(%sRomeDAQ)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sRomeDAQ::%sRomeDAQ() {}\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");

   // Connect Trees
   int iFold = 0, k;
   bool found = false;
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sRomeDAQ::ConnectTrees()\n{\n",shortCut.Data());
   for (i = 0; i < numOfTree && !found; i++) {
      for (j = 0; j < numOfBranch[i]; j++) {
         for (k = 0; k < numOfFolder; k++) {
            if (branchFolder[i][j] == folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("   TBranchElement *bb;\n");
         found = true;
         break;
      }
   }
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   if (fROMETrees[%d]->isRead()) {\n", i);
      for (j = 0; j < numOfBranch[i]; j++) {
         for (k = 0; k < numOfFolder; k++) {
            if (branchFolder[i][j] == folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("     bb = static_cast<TBranchElement*>(fROMETrees[%d]->GetTree()->FindBranch(\"%s\"));\n",i,
                                branchName[i][j].Data());
         buffer.AppendFormatted("     if (bb) {\n");
         buffer.AppendFormatted("        if (fROMETrees[%d]->GetBranchActiveAt(%d)) {\n",i,j);
         if (folderArray[iFold] == "1") {
            buffer.AppendFormatted("           bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         } else {
            buffer.AppendFormatted("           bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         buffer.AppendFormatted("        }\n");
         buffer.AppendFormatted("        else {\n");
         buffer.AppendFormatted("           fROMETrees[%d]->GetTree()->SetBranchStatus(\"%s*\", 0);\n",i,
                                branchName[i][j].Data());
         buffer.AppendFormatted("        }\n");
         buffer.AppendFormatted("     }\n");
         buffer.AppendFormatted("     bb = static_cast<TBranchElement*>(fROMETrees[%d]->GetTree()->FindBranch(\"Info\"));\n",i);
         buffer.AppendFormatted("     bb->SetAddress(&fTreeInfo);\n");
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("}\n\n");

   // Read run header
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sRomeDAQ::ReadRunHeaders()\n{\n",shortCut.Data());
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   if (fROMETrees[%d]->isRead()) {\n", i);
      buffer.AppendFormatted("      if (fROMETrees[%d]->GetFile()) {\n", i);
      buffer.AppendFormatted("         fROMETrees[%d]->GetFile()->cd();\n", i);
      for (j = 0; j < numOfRunHeader[i]; j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            if (folderArray[runHeaderFolderIndex[i][j]] == "1") {
               buffer.AppendFormatted("         gAnalyzer->Get%s()->Read(\"%s\");\n", runHeaderFolder[i][j].Data(),
                                      runHeaderName[i][j].Data());
            } else {
               buffer.AppendFormatted("         gAnalyzer->Get%ss()->Read(\"%s\");\n", runHeaderFolder[i][j].Data(),
                                      runHeaderName[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("         gROOT->cd();\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Get run header
   for (i = 0; i < numOfTree; i++) {
      for (j = 0; j < numOfRunHeader[i]; j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            if (folderArray[runHeaderFolderIndex[i][j]] == "1") {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("%s%s* %sRomeDAQ::Get%sFrom%s() const\n{\n", shortCut.Data(),
                                      runHeaderFolder[i][j].Data(), shortCut.Data(),
                                      runHeaderFolder[i][j].Data(), treeName[i].Data());
            } else {
               buffer.Append(kMethodLine);
               buffer.AppendFormatted("TClonesArray* %sRomeDAQ::Get%ssFrom%s() const\n{\n", shortCut.Data(),
                                      runHeaderFolder[i][j].Data(), treeName[i].Data());
            }
            buffer.AppendFormatted("   if (fROMETrees[%d]->isRead()) {\n", i);
            buffer.AppendFormatted("      if (fROMETrees[%d]->GetFile()) {\n", i);
            if (folderArray[runHeaderFolderIndex[i][j]] == "1") {
               buffer.AppendFormatted("         return static_cast<%s%s*>(fROMETrees[%d]->GetFile()->FindObjectAny(\"%s\"));\n",
                                      shortCut.Data(), runHeaderFolder[i][j].Data(), i, runHeaderName[i][j].Data());
            } else {
               buffer.AppendFormatted("         return static_cast<TClonesArray*>(fROMETrees[%d]->GetFile()->FindObjectAny(\"%s\"));\n",
                                      i, runHeaderName[i][j].Data());
            }
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   return 0;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
         }
      }
   }

   // Update ConfigParameters
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sRomeDAQ::UpdateConfigParameters()\n{\n", shortCut.Data());
   if (hasDependenceCheck || mainDefinitionVersion != "1") {
      buffer.AppendFormatted("   Int_t i;\n");
      buffer.AppendFormatted("   %s%sConfigParameters *p = 0;\n",shortCut.Data(), mainProgName.Data());
      buffer.AppendFormatted("   for (i = 0; i < %d; i++) {\n", numOfTree);
      buffer.AppendFormatted("      if (fROMETrees[i]->isRead() && fROMETrees[i]->GetFile()) {\n");
      buffer.AppendFormatted("         p = static_cast<%s%sConfigParameters*>(fROMETrees[i]->GetFile()->FindObjectAny(\"%sConfigParameters\"));\n",
                             shortCut.Data(), mainProgName.Data(), mainProgName.Data());
      buffer.AppendFormatted("         if (p) {\n");
      // Task active flag
      for (i = 0; i < numOfTaskHierarchy; i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         tmp.SetFormatted("%s%s",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("            if (p->Get%sTaskActive()) {\n", tmp.Data());
         buffer.AppendFormatted("               gAnalyzer->Get%sConfigParameters()->Set%sTaskActive(kTRUE);\n",
                                mainProgName.Data(),tmp.Data());
         buffer.AppendFormatted("            }\n");
      }
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteRomeDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString tmp;
   Int_t i;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sRomeDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sRomeDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sRomeDAQ_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sRomeDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT TTree access for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMERomeDAQ.h\"\n");
   Int_t j;
   buffer.AppendFormatted("class TClonesArray;\n");
   for (i = 0; i < numOfTree; i++) {
      for (j = 0; j < numOfRunHeader[i]; j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            tmp.SetFormatted("class %s%s;\n", shortCut.Data(), runHeaderFolder[i][j].Data());
            if (!buffer.ContainsFast(tmp)) {
               buffer.Append(tmp.Data());
            }
         }
      }
   }

   // Class
   buffer.AppendFormatted("\nclass %sRomeDAQ : public ROMERomeDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   // Constructor
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sRomeDAQ(const %sRomeDAQ &c); // not implemented\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ &operator=(const %sRomeDAQ &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sRomeDAQ();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sRomeDAQ() {}\n",shortCut.Data());

   // Run header
   buffer.AppendFormatted("   void ReadRunHeaders();\n");
   // Get run header
   for (i = 0; i < numOfTree; i++) {
      for (j = 0; j < numOfRunHeader[i]; j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            if (folderArray[runHeaderFolderIndex[i][j]] == "1") {
               buffer.AppendFormatted("   %s%s* Get%sFrom%s() const;\n", shortCut.Data(), runHeaderFolder[i][j].Data(),
                                      runHeaderFolder[i][j].Data(),treeName[i].Data());
            } else {
               buffer.AppendFormatted("   TClonesArray* Get%ssFrom%s() const;\n", runHeaderFolder[i][j].Data(),
                                      treeName[i].Data());
            }
         }
      }
   }

   // Update ConfigParameters
   buffer.AppendFormatted("   void UpdateConfigParameters();\n");

   // File getter
   for (i = 0; i < numOfTree; i++)
      buffer.AppendFormatted("   TFile* Get%sFile() const { return fRootFiles ? fRootFiles[%d] : 0; }\n",treeName[i].Data(),
                             i);

   // methods
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   void ConnectTrees();\n");

   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%sRomeDAQ, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sRomeDAQ_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteRootDAQCpp() {
   int i,j;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sRootDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sRootDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT DAQ for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sRootDAQ.h\"\n",shortCut.Data());

   buffer.AppendFormatted("\nClassImp(%sRootDAQ)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sRootDAQ::%sRootDAQ()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted(":ROMERootDAQ()\n");
   for (i = 0; i < numOfRootTree; i++) {
      buffer.AppendFormatted(",f%s(new %s())\n",rootTreeName[i].Data(),rootTreeName[i].Data());
   }
   buffer.AppendFormatted("{\n");
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("   f%s->f%s = new %s();\n",rootTreeName[i].Data(),rootBranchName[i][j].Data(),
                                   rootBranchClassName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Create Trees
   buffer.AppendFormatted("// Create Trees\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sRootDAQ::CreateTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   TTree *tree;\n");
   buffer.AppendFormatted("   fTrees->RemoveAll();\n");
   for (i = 0; i < numOfRootTree; i++) {
      buffer.AppendFormatted("   tree = 0;\n");
      buffer.AppendFormatted("   for (i = 0; i < gAnalyzer->GetNumberOfInputFileNames() && tree == 0; i++) {\n");
      buffer.AppendFormatted("      tree = (TTree*)fRootFiles[i]->FindObjectAny(\"%s\");\n",rootTreeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   if (tree == 0) {\n");
      buffer.AppendFormatted("      ROMEPrint::Warning(\"Tree '%s' not found.\\n\");\n",rootTreeName[i].Data());
      buffer.AppendFormatted("      return false;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   fTrees->AddLast(tree);\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Connect Trees
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sRootDAQ::ConnectTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TBranchElement *bb;\n");
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         buffer.AppendFormatted("   bb = static_cast<TBranchElement*>(static_cast<TTree*>(fTrees->At(%d))->FindBranch(\"%s\"));\n",i,
                                rootBranchName[i][j].Data());
         buffer.AppendFormatted("   if (!bb) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Branch '%s' not found in tree '%s'.\\n\");\n",
                                rootBranchName[i][j].Data(),rootTreeName[i].Data());
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         if (rootBranchArraySize[i][j].Length() == 0) {
            buffer.AppendFormatted("   bb->SetAddress(&(Get%s()->f%s));\n",rootTreeName[i].Data(),
                                   rootBranchName[i][j].Data());
         } else {
            buffer.AppendFormatted("   bb->SetAddress(Get%s()->f%s);\n",rootTreeName[i].Data(),
                                   rootBranchName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteRootDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   Int_t i,j;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sRootDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sRootDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sRootDAQ_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sRootDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT DAQ for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"TTree.h\"\n");
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("#include \"%s.h\"\n",rootBranchClassName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("#include \"ROMERootDAQ.h\"\n");

   // Class
   buffer.AppendFormatted("\nclass %sRootDAQ : public ROMERootDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("protected:\n");
   ROMEString separator;
   // Fields
   for (i = 0; i < numOfRootTree; i++) {
      buffer.AppendFormatted("   class %s {\n",rootTreeName[i].Data());
      buffer.AppendFormatted("   public:\n");
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("      %s* f%s; // %s Branch\n",rootBranchClassName[i][j].Data(),
                                   rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
         } else {
            if (rootBranchArraySize[i][j].Length() == 0) {
               buffer.AppendFormatted("      %s f%s; // %s Branch\n",rootBranchType[i][j].Data(),
                                      rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            } else {
               buffer.AppendFormatted("      %s *f%s; // %s Branch\n",rootBranchType[i][j].Data(),
                                      rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("      %s()\n",rootTreeName[i].Data());
      separator = ":";

      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("      %sf%s(0)\n",separator.Data(),rootBranchName[i][j].Data());
            separator = ",";
         } else {
            if (rootBranchArraySize[i][j].Length() == 0) {
               // initializing by zero can be a problem for some types of the branch.
               buffer.AppendFormatted("      %sf%s(0)\n",separator.Data(),rootBranchName[i][j].Data());
               separator = ",";
            } else {
               buffer.AppendFormatted("      %sf%s(new %s[%s])\n",separator.Data(),rootBranchName[i][j].Data(),
                                      rootBranchType[i][j].Data(),rootBranchArraySize[i][j].Data());
               separator = ",";
            }
         }
      }
      buffer.AppendFormatted("      {\n");
      buffer.AppendFormatted("      }\n");
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("      %s* Get%s() { return f%s; }\n",rootBranchClassName[i][j].Data(),
                                   rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
         } else {
            if (rootBranchArraySize[i][j].Length() == 0) {
               buffer.AppendFormatted("      %s Get%s() const { return f%s; }\n",rootBranchType[i][j].Data(),
                                      rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            } else {
               buffer.AppendFormatted("      %s Get%sAt(int i) const { return f%s[i]; }\n",rootBranchType[i][j].Data(),
                                      rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
               buffer.AppendFormatted("      %s* Get%s() { return f%s; }\n",rootBranchType[i][j].Data(),
                                      rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("   %s* f%s; // %s Branch\n",rootTreeName[i].Data(),rootTreeName[i].Data(),
                             rootTreeName[i].Data());
   }
   // Methods
   // Constructor
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sRootDAQ(const %sRootDAQ &c); // not implemented\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   %sRootDAQ &operator=(const %sRootDAQ &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");

   buffer.AppendFormatted("   %sRootDAQ();\n",shortCut.Data());
   for (i = 0; i < numOfRootTree; i++) {
      buffer.AppendFormatted("   %s* Get%s() { return f%s; }\n",rootTreeName[i].Data(),rootTreeName[i].Data(),
                             rootTreeName[i].Data());
   }
   buffer.AppendFormatted("   virtual ~%sRootDAQ() {}\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   Bool_t CreateTrees();\n");
   buffer.AppendFormatted("   Bool_t ConnectTrees();\n");

   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%sRootDAQ, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sRootDAQ_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteRootDAQClassesH() {
   int i,j;
   for (i = 0; i < numOfRootTree; i++) {
      for (j = 0; j < numOfRootBranch[i]; j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            WriteRootDAQClassH(i,j);
         }
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteRootDAQClassH(Int_t iTree,Int_t iBranch) {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   Int_t i;

   // File name
   hFile.SetFormatted("%sinclude/generated/%s.h",outDir.Data(),rootBranchClassName[iTree][iBranch].Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %s_H\n",rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("#define %s_H\n\n",rootBranchClassName[iTree][iBranch].Data());
   clsName.SetFormatted("%s",rootBranchClassName[iTree][iBranch].Data());
   clsDescription.SetFormatted("This class is used for the branch %s of the tree %s of the ROOT DAQ.",
                               rootBranchName[iTree][iBranch].Data(),rootTreeName[iTree].Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"TObject.h\"\n");

   // Class
   buffer.AppendFormatted("\nclass %s : public TObject\n",rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("protected:\n");
   for (i = 0; i < numOfRootBranchField[iTree][iBranch]; i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length() == 0) {
         buffer.AppendFormatted("   %s %s; // %s Field\n",rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
      } else {
         buffer.AppendFormatted("   %s %s[%s]; // %s Field\n",rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldArraySize[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Methods
   // Constructor
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %s(const %s &c); // not implemented\n",rootBranchClassName[iTree][iBranch].Data(),
                          rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("   %s &operator=(const %s &c); // not implemented\n",
                          rootBranchClassName[iTree][iBranch].Data(),
                          rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   ROMEString separator = ":";
   buffer.AppendFormatted("   %s()\n",rootBranchClassName[iTree][iBranch].Data());

   for (i = 0; i < numOfRootBranchField[iTree][iBranch]; i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length() == 0) {
         buffer.AppendFormatted("%s%s(0)\n",separator.Data(),rootBranchFieldName[iTree][iBranch][i].Data());
         separator = ",";
      }
   }
   buffer.AppendFormatted("   {\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");

   // methods
   for (i = 0; i < numOfRootBranchField[iTree][iBranch]; i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length() == 0) {
         buffer.AppendFormatted("   %s Get%s() const { return %s; }\n",rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
      } else {
         buffer.AppendFormatted("   %s  Get%sAt(int i) const { return %s[i]; }\n",
                                rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
         buffer.AppendFormatted("   %s* Get%s() { return %s; }\n",rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfRootBranchField[iTree][iBranch]; i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length() == 0) {
         buffer.AppendFormatted("   void Set%s(%s %sValue) { %s = %sValue; }\n",
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
      } else {
         buffer.AppendFormatted("   void Set%sAt(int i,%s %sValue) { %s[i] = %sValue; }\n",
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldType[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data(),
                                rootBranchFieldName[iTree][iBranch][i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%s, %d)\n",rootBranchClassName[iTree][iBranch].Data(),
                          rootBranchClassVersion[iTree][iBranch].ToInteger() + 1);
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %s_H\n",rootBranchClassName[iTree][iBranch].Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDAQCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout<<"\n   Output Cpp-Files:"<<endl;
   for (int iDAQ = 0; iDAQ < numOfDAQ; iDAQ++) {
      if (!daqUsed[iDAQ])
         continue;
      // File name
      cppFile.SetFormatted("%ssrc/daqs/%s%sDAQ.cpp",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

      if (gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kFALSE);
         clsName.SetFormatted("%s%sDAQ", shortCut.Data(), daqName[iDAQ].Data());
         clsDescription.SetFormatted("%s DAQ class for %s%s.",daqName[iDAQ].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"daqs/%s%sDAQ.h\"\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
         buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("ClassImp(%s%sDAQ)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%sDAQ::%s%sDAQ()\n",shortCut.Data(),daqName[iDAQ].Data(),shortCut.Data(),
                                daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDAQ::Init()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDAQ::BeginOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDAQ::Event(Long64_t event)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Long64_t %s%sDAQ::Seek(Long64_t event)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return -1;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDAQ::EndOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDAQ::Terminate()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         // Write File
         WriteFile(cppFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout<<"\n   Output H-Files:"<<endl;
   for (int iDAQ = 0; iDAQ < numOfDAQ; iDAQ++) {
      if (!daqUsed[iDAQ])
         continue;
      // File name
      hFile.SetFormatted("%sinclude/daqs/%s%sDAQ.h",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

      if (gSystem->AccessPathName(hFile.Data(),kFileExists)) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kFALSE);
         buffer.AppendFormatted("#ifndef %s%sDAQ_H\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#define %s%sDAQ_H\n\n",shortCut.Data(),daqName[iDAQ].Data());
         clsName.SetFormatted("%s%sDAQ", shortCut.Data(), daqName[iDAQ].Data());
         clsDescription.SetFormatted("%s DAQ class for %s%s.",daqName[iDAQ].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"ROMEDAQSystem.h\"\n");

         // Class
         buffer.AppendFormatted("\nclass %s%sDAQ : public ROMEDAQSystem\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");

         buffer.AppendFormatted("\n");
         // Methods
         // Constructor
         buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   %s%sDAQ(const %s%sDAQ &c); // not implemented\n", shortCut.Data(),
                                daqName[iDAQ].Data(), shortCut.Data(), daqName[iDAQ].Data());
         buffer.AppendFormatted("   %s%sDAQ &operator=(const %s%sDAQ &c); // not implemented\n",
                                shortCut.Data(), daqName[iDAQ].Data(), shortCut.Data(), daqName[iDAQ].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%sDAQ();\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("   virtual ~%s%sDAQ() {}\n",shortCut.Data(),daqName[iDAQ].Data());

         // Methods
         buffer.AppendFormatted("   Int_t    GetTimeStamp() { return 0; }\n");
         buffer.AppendFormatted("   const char* GetName() const { return \"%s\"; }\n",daqName[iDAQ].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t   Init();\n");
         buffer.AppendFormatted("   Bool_t   BeginOfRun();\n");
         buffer.AppendFormatted("   Bool_t   Event(Long64_t event);\n");
         buffer.AppendFormatted("   Long64_t Seek(Long64_t event);\n");
         buffer.AppendFormatted("   Bool_t   EndOfRun();\n");
         buffer.AppendFormatted("   Bool_t   Terminate();\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   // Change 'kFALSE' to 'kTRUE' if trees must be filled in every events\n");
         buffer.AppendFormatted("   // When this is 'kFALSE', the program does not fill a tree unless any folders in the tree is modified.\n");
         buffer.AppendFormatted("   Bool_t   GetAlwaysFillTrees() const { return kFALSE; };\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   ClassDef(%s%sDAQ, 0)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("};\n\n");


         buffer.AppendFormatted("#endif   // %s%sDAQ_H\n",shortCut.Data(),daqName[iDAQ].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDBCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout<<"\n   Output Cpp-Files:"<<endl;
   for (int iDB = 0; iDB < numOfDB; iDB++) {
      // File name
      cppFile.SetFormatted("%ssrc/databases/%s%sDataBase.cpp",outDir.Data(),shortCut.Data(),dbName[iDB].Data());

#if 0
// this special treatment is neccesary only for several month from Feb.2006
      if (!gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
         ROMEString fileBuffer;
         fileBuffer.ReadFile(cppFile.Data(), kTRUE);
         if (fileBuffer.ContainsFast("int runNumber")
             || fileBuffer.ContainsFast("int eventNumber")
             || fileBuffer.ContainsFast("long runNumber")
             || fileBuffer.ContainsFast("long eventNumber")) {
            cerr<<"type of event number was changed to Long64."<<endl
                <<"Please modify."<<shortCut<<dbName[iDB]<<".h and "
                <<shortCut<<dbName[iDB]<<".cpp"<<endl;
            return false;
         }
      } else {
#else
      if (gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
#endif
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kFALSE);
         clsName.SetFormatted("%s%s", shortCut.Data(), dbName[iDB].Data());
         clsDescription.SetFormatted("%s database class for %s%s.",dbName[iDB].Data(),shortCut.Data(),
                                     mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("ClassImp(%s%sDataBase)\n",shortCut.Data(),dbName[iDB].Data());

         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%sDataBase::%s%sDataBase()\n",shortCut.Data(),dbName[iDB].Data(),shortCut.Data(),
                                dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("%s%sDataBase::~%s%sDataBase()\n",shortCut.Data(),dbName[iDB].Data(),shortCut.Data(),
                                dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDataBase::Init(const char* name,const char* path,const char* connection)\n",
                                shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)\n",
                                shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.Append(kMethodLine);
         buffer.AppendFormatted("Bool_t %s%sDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)\n",
                                shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         // Write File
         WriteFile(cppFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDBH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout<<"\n   Output H-Files:"<<endl;
   for (int iDB = 0; iDB < numOfDB; iDB++) {
      // File name
      hFile.SetFormatted("%sinclude/databases/%s%sDataBase.h",outDir.Data(),shortCut.Data(),dbName[iDB].Data());

#if 0
// this special treatment is neccesary only for several month from Feb.2006
      if (!gSystem->AccessPathName(hFile.Data(),kFileExists)) {
         ROMEString fileBuffer;
         fileBuffer.ReadFile(hFile.Data(), kTRUE);
         if (fileBuffer.ContainsFast("int runNumber")
             || fileBuffer.ContainsFast("int eventNumber")
             || fileBuffer.ContainsFast("long runNumber")
             || fileBuffer.ContainsFast("long eventNumber")) {
            cerr<<"type of event number was changed to Long64."<<endl
                <<"Please modify."<<shortCut<<dbName[iDB]<<".h and "
                <<shortCut<<dbName[iDB]<<".cpp"<<endl;
            return false;
         }
      } else {
#else
      if (gSystem->AccessPathName(hFile.Data(),kFileExists)) {
#endif
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kFALSE);
         buffer.AppendFormatted("#ifndef %s%sDataBase_H\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("#define %s%sDataBase_H\n\n",shortCut.Data(),dbName[iDB].Data());
         clsName.SetFormatted("%s%s", shortCut.Data(), dbName[iDB].Data());
         clsDescription.SetFormatted("%s database class for %s%s.",dbName[iDB].Data(),shortCut.Data(),
                                     mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"ROMEDataBase.h\"\n");

         // Class
         buffer.AppendFormatted("\nclass %s%sDataBase : public ROMEDataBase\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");

         buffer.AppendFormatted("\n");
         // Methods
         // Constructor
         buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   %s%sDB(const %s%sDB &c); // not implemented\n", shortCut.Data(),
                                dbName[iDB].Data(), shortCut.Data(), dbName[iDB].Data());
         buffer.AppendFormatted("   %s%sDB &operator=(const %s%sDB &c); // not implemented\n", shortCut.Data(),
                                dbName[iDB].Data(), shortCut.Data(), dbName[iDB].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%sDataBase();\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("   virtual ~%s%sDataBase();\n",shortCut.Data(),dbName[iDB].Data());

         // Methods
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t  Init(const char* name,const char* path,const char* connection);\n");
         buffer.AppendFormatted("   Bool_t  Read(ROMEStr2DArray *values,const char *path,Long64_t runNumber,Long64_t eventNumber);\n");
         buffer.AppendFormatted("   Bool_t  Write(ROMEStr2DArray* values,const char *path,Long64_t runNumber,Long64_t eventNumber);\n");
         buffer.AppendFormatted("   char   *GetType() const { return \"%s\"; }\n",dbName[iDB].Data());
         buffer.AppendFormatted("   char   *GetDescription() const { return \"%s\"; }\n",dbDescription[iDB].Data());

         buffer.AppendFormatted("   ClassDef(%s%sDataBase, 0)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("};\n\n");

         buffer.AppendFormatted("#endif   // %s%sDataBase_H\n",shortCut.Data(),dbName[iDB].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
   }
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteNetFolderServerCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString temp;

   int i,j;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sNetFolderServer.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sNetFolderServer", shortCut.Data());
   clsDescription.SetFormatted("This class implements the NetFolderServer for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TVirtualMutex.h>\n");
   buffer.AppendFormatted("#include <TSocket.h>\n");
   buffer.AppendFormatted("#include <TServerSocket.h>\n");
   buffer.AppendFormatted("#include <TMessage.h>\n");
   buffer.AppendFormatted("#include <TThread.h>\n");
   buffer.AppendFormatted("#include <TList.h>\n");
   buffer.AppendFormatted("#include <TCollection.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sNetFolderServer.h\"\n",shortCut.Data());

   buffer.AppendFormatted("\nClassImp(%sNetFolderServer)\n",shortCut.Data());

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("static TVirtualMutex *fgSocketServerMutex = 0;\n");
   buffer.AppendFormatted("\n");

   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sNetFolderServer::ConstructObjects(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t id = FindId(socket);\n");
   buffer.AppendFormatted("   if(id == -1) return;\n");
   buffer.AppendFormatted("   fFolder[id] = new TObjArray(%d);\n",numOfFolder);
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            buffer.AppendFormatted("   fFolderActive[id][%d] = kFALSE;\n",i);
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("   fFolder[id]->AddAt(new %s%s(),%d);\n",shortCut.Data(),folderName[i].Data(),i);
            } else {
               buffer.AppendFormatted("   fFolder[id]->AddAt(new TClonesArray(\"%s%s\"),%d);\n",shortCut.Data(),
                                      folderName[i].Data(),i);
               buffer.AppendFormatted("   static_cast<TClonesArray*>(fFolder[id]->At(%d))->SetName(\"%s%s\");\n",i,
                                      shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         buffer.AppendFormatted("   f%s%s_%sGraphActive[id] = kFALSE;\n",taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j] == "1") {
            buffer.AppendFormatted("   f%s%s_%sGraph[id] = new %s();\n",taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),
                                   graphType[taskHierarchyClassIndex[i]][j].Data());
         } else {
            buffer.AppendFormatted("   f%s%s_%sGraphs[id] = new TObjArray();\n",taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sNetFolderServer::DestructObjects(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t id = FindId(socket);\n");
   buffer.AppendFormatted("   if(id == -1) return;\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            buffer.AppendFormatted("   delete fFolder[id]->At(%d);\n",i);
         }
      }
   }
   buffer.AppendFormatted("   SafeDelete(fFolder[id]);\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j] == "1") {
            buffer.AppendFormatted("   SafeDelete(f%s%s_%sGraph[id]);\n",taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         } else {
            buffer.AppendFormatted("   SafeDelete(f%s%s_%sGraphs[id]);\n",taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("int %sNetFolderServer::ResponseFunction(TSocket *socket)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (!socket->IsValid())\n");
   buffer.AppendFormatted("      return 0;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Read Command\n");
   buffer.AppendFormatted("   char str[200];\n");
   buffer.AppendFormatted("   Int_t status = socket->Select(TSocket::kRead, kNetFolderServerTimeOut);\n");
   buffer.AppendFormatted("   if (status == 0) { // time out\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if (status == -1 || socket->Recv(str, sizeof(str)) <= 0) { // error\n");
   buffer.AppendFormatted("      return 0;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return CheckCommand(socket,str);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("int %sNetFolderServer::CheckCommand(TSocket *socket,char *str)\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (!socket->IsValid() || !gAnalyzer)\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   %sNetFolderServer* localThis = static_cast<%sNetFolderServer*>(gAnalyzer->GetNetFolderServer());\n",
                          shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   TMessage message(kMESS_OBJECT);\n");
   buffer.AppendFormatted("   // Check Command\n");
   buffer.AppendFormatted("   Int_t i,j,k;\n");
   buffer.AppendFormatted("   ROMEString command;\n");
   buffer.AppendFormatted("   ROMETask *task;\n");
   buffer.AppendFormatted("   ROMEHisto *histoPar;\n");
   buffer.AppendFormatted("   Int_t id = localThis->FindId(socket);\n");
   buffer.AppendFormatted("   if (id >= 0) {\n");
   buffer.AppendFormatted("      while (gAnalyzer && gNetFolderServerRunning && !localThis->IsSocketClientRead(id))\n");
   buffer.AppendFormatted("         localThis->UpdateObjects();\n");
   buffer.AppendFormatted("      if (!gAnalyzer || !gNetFolderServerRunning) {\n");
   buffer.AppendFormatted("         return 1;\n");
   buffer.AppendFormatted("      }\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("      if (strncmp(str, \"FindObjectAny %s%s\", %d) == 0) {\n",shortCut.Data(),
                                      folderName[i].Data(),
                                      static_cast<int>(strlen("FindObjectAny ") + shortCut.Length() +
                                                       folderName[i].Length()));
            } else {
               buffer.AppendFormatted("      if (strncmp(str, \"FindObjectAny %s%ss\", %d) == 0) {\n",shortCut.Data(),
                                      folderName[i].Data(),
                                      static_cast<int>(strlen("FindObjectAny s") + shortCut.Length() +
                                                       folderName[i].Length()));
            }
            buffer.AppendFormatted("         ReadFolderPointer(socket);\n");
            buffer.AppendFormatted("         {\n");
            buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
            buffer.AppendFormatted("            message<<localThis->fFolder[id]->At(%d);\n",i);
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("         socket->Send(message);\n");
            buffer.AppendFormatted("         return 1;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      if (strncmp(str, \"RegisterObject %s%s\", %d) == 0) {\n",shortCut.Data(),
                                   folderName[i].Data(),
                                   static_cast<int>(strlen("RegisterObject ") + shortCut.Length() +
                                                    folderName[i].Length()));
            buffer.AppendFormatted("         {\n");
            buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
            buffer.AppendFormatted("            localThis->fFolderActive[id][%d] = kTRUE;\n",i);
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      if (strncmp(str, \"UnRegisterObject %s%s\", %d) == 0) {\n",shortCut.Data(),
                                   folderName[i].Data(),
                                   static_cast<int>(strlen("UnRegisterObject ") + shortCut.Length() +
                                                    folderName[i].Length()));
            buffer.AppendFormatted("         {\n");
            buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
            buffer.AppendFormatted("            localThis->fFolderActive[id][%d] = kFALSE;\n",i);
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
         }
      }
   }

   buffer.AppendFormatted("      for (i=0;i<%d;i++) {\n",numOfTaskHierarchy);
   buffer.AppendFormatted("         task = gAnalyzer->GetTaskObjectAt(i);\n");
   buffer.AppendFormatted("         if (task->IsActive()) {\n");
   buffer.AppendFormatted("            for (j=0;j<task->GetNumberOfHistos();j++) {\n");
   buffer.AppendFormatted("               histoPar = task->GetHistoParameterAt(j);\n");
   buffer.AppendFormatted("               if (histoPar->IsActive()) {\n");
   buffer.AppendFormatted("                  if (histoPar->GetArraySize()>1) {\n");
   buffer.AppendFormatted("                     for (k=0;k<histoPar->GetArraySize();k++) {\n");
   buffer.AppendFormatted("                        command.SetFormatted(\"FindObjectAny Task_%%d:Histo_%%d_%%d\",i,j,k);\n");
   buffer.AppendFormatted("                        if (strncmp(str, command, command.Length()) == 0) {\n");
   buffer.AppendFormatted("                           ReadFolderPointer(socket);\n");
   buffer.AppendFormatted("                           message<< static_cast<TObjArray*>(task->GetHistoAt(j))->At(k);\n");
   buffer.AppendFormatted("                           socket->Send(message);\n");
   buffer.AppendFormatted("                           return 1;\n");
   buffer.AppendFormatted("                        }\n");
   buffer.AppendFormatted("                     }\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("                  command.SetFormatted(\"FindObjectAny Task_%%d:Histo_%%d\",i,j);\n");
   buffer.AppendFormatted("                  if (strncmp(str, command, command.Length()) == 0) {\n");
   buffer.AppendFormatted("                     ReadFolderPointer(socket);\n");
   buffer.AppendFormatted("                     message<< task->GetHistoAt(j);\n");
   buffer.AppendFormatted("                     socket->Send(message);\n");
   buffer.AppendFormatted("                     return 1;\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");
   // TODO graphs -> replace f%s%s_%sGraph by TObjArray
/*   buffer.AppendFormatted("            for (j=0;j<task->GetNumberOfGraphs();j++) {\n");
   buffer.AppendFormatted("               graphPar = task->GetGraphParameterAt(j);\n");
   buffer.AppendFormatted("               if (graphPar->IsActive()) {\n");
   buffer.AppendFormatted("                  command.SetFormatted(\"FindObjectAny Task_%%d:Graph_%%d\",i,j);\n");
   buffer.AppendFormatted("                  if (strncmp(str, command, command.Length()) == 0) {\n");
   buffer.AppendFormatted("                     ReadFolderPointer(socket);\n");
   buffer.AppendFormatted("                     {\n");
   buffer.AppendFormatted("                        ROME_LOCKGUARD(fgSocketServerMutex);\n");
   buffer.AppendFormatted("                        message<< localThis->f%s%s_%sGraph[id];\n");
   buffer.AppendFormatted("                     }\n");
   buffer.AppendFormatted("                     socket->Send(message);\n");
   buffer.AppendFormatted("                     return 1;\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("                  if (graphPar->GetArraySize()>1) {\n");
   buffer.AppendFormatted("                     for (k=0;k<graphPar->GetArraySize();k++) {\n");
   buffer.AppendFormatted("                        command.SetFormatted(\"FindObjectAny Task_%%d:Graph_%%d_%%d\",i,j,k);\n");
   buffer.AppendFormatted("                        if (strncmp(str, command, command.Length()) == 0) {\n");
   buffer.AppendFormatted("                           ReadFolderPointer(socket);\n");
   buffer.AppendFormatted("                           {\n");
   buffer.AppendFormatted("                              ROME_LOCKGUARD(fgSocketServerMutex);\n");
   buffer.AppendFormatted("                              message<< static_cast<TObjArray*>(task->GetGraphAt(j))->At(k);\n");
   buffer.AppendFormatted("                           }\n");
   buffer.AppendFormatted("                           socket->Send(message);\n");
   buffer.AppendFormatted("                           return 1;\n");
   buffer.AppendFormatted("                        }\n");
   buffer.AppendFormatted("                     }\n");
   buffer.AppendFormatted("                  }\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("            }\n");*/
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j] == "1") {
            buffer.AppendFormatted("      command.SetFormatted(\"FindObjectAny Task_%d:Graph_%d\");\n",i,j);
            buffer.AppendFormatted("      if (strncmp(str, command, command.Length()) == 0) {\n");
            buffer.AppendFormatted("         ReadFolderPointer(socket);\n");
            buffer.AppendFormatted("         {\n");
            buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
            buffer.AppendFormatted("            message<<localThis->f%s%s_%sGraph[id];\n",taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("         socket->Send(message);\n");
            buffer.AppendFormatted("         return 1;\n");
            buffer.AppendFormatted("      }\n");
         } else {
            buffer.AppendFormatted("      command.SetFormatted(\"FindObjectAny Task_%d:Graph_%d_\");\n",i,j);
            buffer.AppendFormatted("      if (strncmp(str, command, command.Length()) == 0) {\n");
            buffer.AppendFormatted("         char* cstop;\n");
            buffer.AppendFormatted("         int indx = strtol(&str[command.Length()],&cstop, 10);\n");
            buffer.AppendFormatted("         ReadFolderPointer(socket);\n");
            buffer.AppendFormatted("         {\n");
            buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
            buffer.AppendFormatted("            message<<localThis->f%s%s_%sGraphs[id]->At(indx);\n",
                                   taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                   graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("         socket->Send(message);\n");
            buffer.AppendFormatted("         return 1;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      command.SetFormatted(\"FindObjectAny Task_%d:Graphs_%d\");\n",i,j);
            buffer.AppendFormatted("      if (strncmp(str, command, command.Length()) == 0) {\n");
            buffer.AppendFormatted("         ReadFolderPointer(socket);\n");
            buffer.AppendFormatted("         {\n");
            buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
            buffer.AppendFormatted("            message<<localThis->f%s%s_%sGraphs[id];\n",taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("         socket->Send(message);\n");
            buffer.AppendFormatted("         return 1;\n");
            buffer.AppendFormatted("      }\n");
         }
         buffer.AppendFormatted("      command.SetFormatted(\"RegisterObject Task_%d:Graph_%d\");\n",i,j);
         buffer.AppendFormatted("      if (strncmp(str, command, command.Length()) == 0) {\n");
         buffer.AppendFormatted("         {\n");
         buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
         buffer.AppendFormatted("            localThis->f%s%s_%sGraphActive[id] = kTRUE;\n",taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      command.SetFormatted(\"UnRegisterObject Task_%d:Graph_%d\");\n",i,j);
         buffer.AppendFormatted("      if (strncmp(str, command, command.Length()) == 0) {\n");
         buffer.AppendFormatted("         {\n");
         buffer.AppendFormatted("            ROME_LOCKGUARD(fgSocketServerMutex);\n");
         buffer.AppendFormatted("            localThis->f%s%s_%sGraphActive[id] = kFALSE;\n",taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return ROMENetFolderServer::CheckCommand(socket,str);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("THREADTYPE %sNetFolderServer::Server(void *arg)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   TSocket *socket = (TSocket *) arg;\n");
   buffer.AppendFormatted("   if (!socket->IsValid() || !gAnalyzer) {\n");
   buffer.AppendFormatted("      delete socket;\n");
   buffer.AppendFormatted("      return THREADRETURN;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   %sNetFolderServer* localThis = static_cast<%sNetFolderServer*>(gAnalyzer->GetNetFolderServer());\n",
                          shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (localThis->Register(socket) == -1) {\n");
   buffer.AppendFormatted("      delete socket;\n");
   buffer.AppendFormatted("      return THREADRETURN;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   localThis->ConstructObjects(socket);\n");
   buffer.AppendFormatted("   while (gAnalyzer && gNetFolderServerRunning && %sNetFolderServer::ResponseFunction(socket))\n",shortCut.Data());
   buffer.AppendFormatted("   {}\n\n");
//   buffer.AppendFormatted("   if (gAnalyzer && gNetFolderServerRunning) {\n");
   buffer.AppendFormatted("   localThis->DestructObjects(socket);\n");
   buffer.AppendFormatted("   localThis->UnRegister(socket);\n");
//   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   delete socket;\n");
   buffer.AppendFormatted("   return THREADRETURN;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("THREADTYPE %sNetFolderServer::ServerLoop(void *arg)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("// Server loop listening for incoming network connections on port\n");
   buffer.AppendFormatted("// specified by command line option -s. Starts a searver_thread for\n");
   buffer.AppendFormatted("// each connection.\n");
   buffer.AppendFormatted("   Int_t port;\n");
   buffer.AppendFormatted("   memcpy(&port, arg, sizeof(Int_t));\n");
   buffer.AppendFormatted("   TServerSocket *lsock = new TServerSocket(port, kTRUE);\n");
   buffer.AppendFormatted("   if (!lsock->IsValid()) {\n");
   buffer.AppendFormatted("      switch(lsock->GetErrorCode()) {\n");
   buffer.AppendFormatted("         case -1:\n");
   buffer.AppendFormatted("            ROMEPrint::Error(\"Error: Low level socket() call failed. Failed to connect port %%d\\n\", port);\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         case -2:\n");
   buffer.AppendFormatted("            ROMEPrint::Error(\"Error: Low level bind() call failed. Failed to connect port %%d\\n\", port);\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         case -3:\n");
   buffer.AppendFormatted("            ROMEPrint::Error(\"Error: Low level listen() call failed. Failed to connect port %%d\\n\", port);\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         default:\n");
   buffer.AppendFormatted("            ROMEPrint::Error(\"Error: Failed to connect port %%d\\n\", port);\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("      };\n");
   buffer.AppendFormatted("      delete lsock;\n");
   buffer.AppendFormatted("      return THREADRETURN;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TSocket *sock;\n");
   buffer.AppendFormatted("   Int_t status;\n");
   buffer.AppendFormatted("   TThread *thread;\n");
   buffer.AppendFormatted("   while (gAnalyzer && gNetFolderServerRunning) {\n");
   buffer.AppendFormatted("      status = lsock->Select(TSocket::kRead, kNetFolderServerTimeOut);\n");
   buffer.AppendFormatted("      if (status == -1) { // error\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      } else if (status == 0) { // time out\n");
   buffer.AppendFormatted("         continue;\n");
   buffer.AppendFormatted("      } else {\n");
   buffer.AppendFormatted("         if ((sock = lsock->Accept()) > 0) {\n");
   buffer.AppendFormatted("            TThread *thread = new TThread(\"Server\", %sNetFolderServer::Server, sock);\n",
                          shortCut.Data());
   buffer.AppendFormatted("            fServerThreadList->Add(thread);\n");
   buffer.AppendFormatted("            thread->Run();\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      TIter join(fServerThreadList);\n");
   buffer.AppendFormatted("      while ((thread = static_cast<TThread*>(join()))) {\n");
   buffer.AppendFormatted("         if (thread->GetState() == TThread::kTerminatedState) {\n");
   buffer.AppendFormatted("            thread->Join();\n");
   buffer.AppendFormatted("            fServerThreadList->Remove(thread);\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   TIter next(fServerThreadList);\n");
   buffer.AppendFormatted("   while ((thread = static_cast<TThread*>(next()))) {\n");
   buffer.AppendFormatted("      thread->Join();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   delete lsock;\n");
   buffer.AppendFormatted("   return THREADRETURN;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sNetFolderServer::StartServer(TApplication *app,Int_t port,const char* serverName)\n",
                          shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("// start Socket server loop\n");
   buffer.AppendFormatted("   if (gNetFolderServerRunning || fServerLoopThread) {\n");
   buffer.AppendFormatted("      Warning(\"StartServer\", \"server is already running.\");\n");
   buffer.AppendFormatted("      return;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   fApplication = app;\n");
   buffer.AppendFormatted("   fPort = port;\n");
   buffer.AppendFormatted("   fServerName = serverName;\n");
   buffer.AppendFormatted("   gNetFolderServerRunning = kTRUE;\n");
   buffer.AppendFormatted("   fServerLoopThread = new TThread(\"server_loop\", %sNetFolderServer::ServerLoop, &fPort);\n",
                          shortCut.Data());
   buffer.AppendFormatted("   fServerLoopThread->Run();\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("Bool_t %sNetFolderServer::UpdateObjects()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   if (!gAnalyzer)\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ROME_LOCKGUARD(fgSocketServerMutex);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   //create a buffer where the object will be streamed\n");
   buffer.AppendFormatted("   TFile *filsav = gFile;\n");
   buffer.AppendFormatted("   gFile = 0;\n");
   buffer.AppendFormatted("   const Int_t bufsize = 10000;\n");
#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0))
   buffer.AppendFormatted("   TBufferFile *buffer = new TBufferFile(TBuffer::kWrite,bufsize);\n");
#else
   buffer.AppendFormatted("   TBuffer *buffer = new TBuffer(TBuffer::kWrite,bufsize);\n");
#endif
   buffer.AppendFormatted("   Bool_t bypassOrgOld;\n");
   buffer.AppendFormatted("   bypassOrgOld = kFALSE;\n"); // to suppress unused warning
   buffer.AppendFormatted("   Bool_t bypassOld;\n");
   buffer.AppendFormatted("   bypassOld = kFALSE;\n"); // to suppress unused warning
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (fCopyAll)\n");
   buffer.AppendFormatted("      fAllDataAvailable = kTRUE;\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fAllDataAvailable = kFALSE;\n");
   buffer.AppendFormatted("   Int_t iClient;\n");
   buffer.AppendFormatted("   for (iClient = 0; iClient < kMaxSocketClients; iClient++) {\n");
   buffer.AppendFormatted("      if(!fAcceptedSockets[iClient]) continue;\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderSupport[i])
         continue;
      if (FolderToBeGenerated(i)) {
         buffer.AppendFormatted("      buffer->Reset();\n");
         buffer.AppendFormatted("      buffer->SetWriteMode();\n");
         buffer.AppendFormatted("      if (fFolderActive[iClient][%d] || fCopyAll) {\n",i);
         if (folderArray[i] == "1") {
            buffer.AppendFormatted("         gAnalyzer->CopyTObjectWithStreamer(buffer,gAnalyzer->Get%sFolderStorage(),fFolder[iClient]->At(%d));\n",
                                   folderName[i].Data(),i);
         } else {
            buffer.AppendFormatted("         bypassOrgOld = gAnalyzer->Get%sFoldersStorage()->CanBypassStreamer();\n",
                                   folderName[i].Data());
            buffer.AppendFormatted("         bypassOld = static_cast<TClonesArray*>(fFolder[iClient]->At(%d))->CanBypassStreamer();\n",
                                   i);
            buffer.AppendFormatted("         gAnalyzer->Get%sFoldersStorage()->BypassStreamer(kTRUE);\n",
                                   folderName[i].Data());
            buffer.AppendFormatted("         static_cast<TClonesArray*>(fFolder[iClient]->At(%d))->BypassStreamer(kTRUE);\n",i);
            buffer.AppendFormatted("         gAnalyzer->CopyTObjectWithStreamer(buffer,gAnalyzer->Get%sFoldersStorage(),fFolder[iClient]->At(%d));\n",
                                   folderName[i].Data(),i);
            buffer.AppendFormatted("         gAnalyzer->Get%sFoldersStorage()->BypassStreamer(bypassOrgOld);\n",
                                   folderName[i].Data());
            buffer.AppendFormatted("         static_cast<TClonesArray*>(fFolder[iClient]->At(%d))->BypassStreamer(bypassOld);\n",
                                   i);
         }
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("\n");
      }
   }
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (numOfGraphs[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("      if (gAnalyzer->GetTaskObjectAt(%d)->IsActive()) {\n",i);
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         buffer.AppendFormatted("      buffer->Reset();\n");
         buffer.AppendFormatted("      buffer->SetWriteMode();\n");
         buffer.AppendFormatted("      if (f%s%s_%sGraphActive[iClient] || fCopyAll) {\n",taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j] == "1") {
            buffer.AppendFormatted("         %s *%sOrg = ((%s*)gAnalyzer->GetTaskObjectAt(%d)->GetGraphStorageAt(%d));\n",
                                   graphType[taskHierarchyClassIndex[i]][j].Data(),
                                   graphName[taskHierarchyClassIndex[i]][j].Data(),
                                   graphType[taskHierarchyClassIndex[i]][j].Data(),
                                   i,j);
            WriteUpdateObjectsObject(buffer,"f" + taskHierarchyName[i] + taskHierarchySuffix[i] + "_" +
                                     graphName[taskHierarchyClassIndex[i]][j] + "Graph[iClient]",
                                     graphName[taskHierarchyClassIndex[i]][j]+"Org",false);
         } else {
            buffer.AppendFormatted("         TObjArray *%ssOrg = static_cast<TObjArray*>(gAnalyzer->GetTaskObjectAt(%d)->GetGraphStorageAt(%d));\n",
                                   graphName[taskHierarchyClassIndex[i]][j].Data(),i,j);
            WriteUpdateObjectsObject(buffer,"f" + taskHierarchyName[i] + taskHierarchySuffix[i] + "_" +
                                     graphName[taskHierarchyClassIndex[i]][j] + "Graphs[iClient]",
                                     graphName[taskHierarchyClassIndex[i]][j]+"sOrg",false);
         }
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("\n");
      }
      if (numOfGraphs[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("      fSocketClientRead[iClient] = kTRUE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   gFile = filsav;\n");
   buffer.AppendFormatted("   delete buffer;\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteNetFolderServerH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sNetFolderServer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sNetFolderServer_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sNetFolderServer_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sNetFolderServer", shortCut.Data());
   clsDescription.SetFormatted("This class implements the NetFolderServer for %s%s.",shortCut.Data(),
                               mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMENetFolderServer.h\"\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TH2.h>\n");
   buffer.AppendFormatted("#include <TH3.h>\n");
   buffer.AppendFormatted("#include <TProfile.h>\n");
   buffer.AppendFormatted("#include <TProfile2D.h>\n");
   buffer.AppendFormatted("#include <ROMETGraph.h>\n");
   buffer.AppendFormatted("#include <TGraph2D.h>\n");
   buffer.AppendFormatted("#include <ROMETCutG.h>\n");

   // Folder class declaration
   Int_t i,j;
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i]) {
         buffer.AppendFormatted("class %s%s;\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sNetFolderServer : public ROMENetFolderServer\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("protected:\n");
   // Folder Fields
   buffer.AppendFormatted("   // Folder Fields\n");
   buffer.AppendFormatted("   Bool_t fFolderActive[kMaxSocketClients][%d]; //! Flag if folder is active\n",
                          TMath::Max(numOfFolder, 1));
   buffer.AppendFormatted("   TObjArray* fFolder[kMaxSocketClients]; //! Handle to Folders\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         buffer.AppendFormatted("   Bool_t f%s%s_%sGraphActive[kMaxSocketClients]; //! Flag if %s graph is active\n",
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                graphName[taskHierarchyClassIndex[i]][j].Data(),
                                graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j] == "1") {
            buffer.AppendFormatted("   %s* f%s%s_%sGraph[kMaxSocketClients]; //! Handle to %s graph\n",
                                   graphType[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),
                                   taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),
                                   graphName[taskHierarchyClassIndex[i]][j].Data());
         } else {
            buffer.AppendFormatted("   TObjArray* f%s%s_%sGraphs[kMaxSocketClients]; //! Handle to %s graphs\n",
                                   taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                   graphName[taskHierarchyClassIndex[i]][j].Data(),
                                   graphName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   void              StartServer(TApplication *app,Int_t port,const char* serverName);\n");
   buffer.AppendFormatted("   static Int_t      ResponseFunction(TSocket *socket);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sNetFolderServer(const %sNetFolderServer &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("   %sNetFolderServer &operator=(const %sNetFolderServer &c); // not implemented\n",
                          shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sNetFolderServer():ROMENetFolderServer(){}\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sNetFolderServer(){}\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   for (i = 0 ; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (FolderToBeGenerated(i)) {
            buffer.AppendFormatted("   Bool_t Get%sFolderActive(Int_t i) const { return fFolderActive[i][%d]; }\n",
                                   folderName[i].Data(),i);
         }
      }
   }
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j = 0; j < numOfGraphs[taskHierarchyClassIndex[i]]; j++) {
         buffer.AppendFormatted("   Bool_t Get%s%s_%sGraphActive(Int_t i) const { return f%s%s_%sGraphActive[i]; }\n",
                                taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                graphName[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),
                                taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
      }
   }

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t UpdateObjects();\n");
   buffer.AppendFormatted("   void   ConstructObjects(TSocket* socket);\n");
   buffer.AppendFormatted("   void   DestructObjects(TSocket* socket);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   static Int_t      CheckCommand(TSocket *socket,char *str);\n");
   buffer.AppendFormatted("   static THREADTYPE Server(void *arg);\n");
   buffer.AppendFormatted("   static THREADTYPE ServerLoop(void *arg);\n");
   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%sNetFolderServer, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sNetFolderServer_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteEventLoopCpp()
{
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;

   bool breaking;
   int j,k,iFold = 0;

   ROMEString tmp;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (typeLen < static_cast<int>(folderName[i].Length() + scl)) {
            typeLen = folderName[i].Length()+scl;
         }
         if (nameLen < static_cast<int>(folderName[i].Length())) {
            nameLen = folderName[i].Length();
         }
      }
   }


   // File name
   cppFile.SetFormatted("%ssrc/generated/%sEventLoop.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   clsName.SetFormatted("%sEventLoop", shortCut.Data());
   clsDescription.SetFormatted("Framework specific event loop class for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <direct.h>\n");
#endif

   buffer.AppendFormatted("#include <sys/stat.h>\n");
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
   buffer.AppendFormatted("#include <TObjString.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TBranchElement.h>\n");
   buffer.AppendFormatted("#include <TTask.h>\n");
   buffer.AppendFormatted("#include <TFile.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

   // Task class includes
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMETree.h\"\n");
   buffer.AppendFormatted("#include \"ROMETreeInfo.h\"\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAllFolders.h\"\n",shortCut.Data());
   if (numOfTree>0)
      buffer.AppendFormatted("#include \"generated/%sRomeDAQ.h\"\n",shortCut.Data());
   if (numOfEvent>0 || midas)
      buffer.AppendFormatted("#include \"generated/%sMidasDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("\nClassImp(%sEventLoop)\n",shortCut.Data());

   // Constructor
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("%sEventLoop::%sEventLoop(const char *name,const char *title):ROMEEventLoop(name,title)\n{\n",
                          shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   fStatisticsTimeOfLastEvent = new ULong_t[gAnalyzer->GetMaxEventID()];\n");
   buffer.AppendFormatted("   fStatisticsLastEvent = new Double_t[gAnalyzer->GetMaxEventID()];\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Tree Initialization
   buffer.AppendFormatted("// Tree initialization\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::InitTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TFolder *treeFolder;\n");
   buffer.AppendFormatted("   treeFolder = gAnalyzer->GetMainFolder()->AddFolder(\"Trees\",\"Trees of the %s framework\");\n",
                          shortCut.Data()); // to suppress unused warning
   if (numOfTree>0) {
      buffer.AppendFormatted("   TTree *tree;\n");
      buffer.AppendFormatted("   ROMEString fileName;\n\n");
   }
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   tree = new TTree(\"%s\",\"%s\");\n",treeName[i].Data(),treeTitle[i].Data());
      buffer.AppendFormatted("   gAnalyzer->AddTree(tree);\n");
      buffer.AppendFormatted("   treeFolder->Add(tree);\n\n");
      buffer.AppendFormatted("   gAnalyzer->GetTreeObjectAt(%d)->AllocateBranchActive(%d);\n",i,numOfBranch[i]);
      buffer.AppendFormatted("   gAnalyzer->GetTreeObjectAt(%d)->SetName(\"%s\");\n",i,treeName[i].Data());
   }
   buffer.AppendFormatted("}\n\n");

   // Add Tree Branches
   buffer.AppendFormatted("// Add branches\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::AddTreeBranches()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (!gAnalyzer->IsROMEMonitor()) {\n");
   if (numOfTree>0) {
      buffer.AppendFormatted("      TTree *tree;\n");
   }
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("      tree = gAnalyzer->GetTreeObjectAt(%d)->GetTree();\n",i);
      buffer.AppendFormatted("      tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000, 99)->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",
                             i);
      for (j = 0; j < numOfBranch[i]; j++) {
         for (k = 0; k < numOfFolder; k++) {
            if (branchFolder[i][j] == folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("      if(gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d)) {\n",i,j);
         if (folderArray[iFold] == "1") {
            buffer.AppendFormatted("         tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sAddress(),%s,%s)->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",
                                   branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),
                                   branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),
                                   branchSplitLevel[i][j].Data(),i);
         } else {
            buffer.AppendFormatted("         tree->Branch(\"%s\", \"TClonesArray\", gAnalyzer->Get%sAddress(),%s,%s)->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",
                                   branchName[i][j].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),
                                   branchSplitLevel[i][j].Data(),i);
         }
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n\n");
   // Write run headers
   buffer.AppendFormatted("// Write run headers\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::WriteRunHeaders()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (!gAnalyzer->IsROMEMonitor()) {\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("      if (gAnalyzer->GetTreeObjectAt(%d)->isWrite()) {\n", i);
      buffer.AppendFormatted("         if (gAnalyzer->GetTreeObjectAt(%d)->GetFile()) {\n", i);
      buffer.AppendFormatted("            gAnalyzer->GetTreeObjectAt(%d)->GetFile()->cd();\n", i);
      for (j = 0; j < numOfRunHeader[i]; j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            if (folderArray[runHeaderFolderIndex[i][j]] == "1") {
               buffer.AppendFormatted("            gAnalyzer->Get%s()->Write(\"%s\", TObject::kOverwrite);\n",
                                      runHeaderFolder[i][j].Data(), runHeaderName[i][j].Data());
            } else {
               buffer.AppendFormatted("            gAnalyzer->Get%ss()->Write(\"%s\", TObject::kOverwrite | TObject::kSingleKey);\n",
                                      runHeaderFolder[i][j].Data(), runHeaderName[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("            gROOT->cd();\n");
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // Read run headers
   buffer.AppendFormatted("// Read run headers\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::ReadRunHeaders()\n{\n",shortCut.Data());
   if (numOfTree>0) {
      buffer.AppendFormatted("   if(gAnalyzer->GetActiveDAQ()->InheritsFrom(\"%sRomeDAQ\"))\n", shortCut.Data());
      buffer.AppendFormatted("      static_cast<%sRomeDAQ*>(gAnalyzer->GetActiveDAQ())->ReadRunHeaders();\n",
                             shortCut.Data());
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // clean up folders
   buffer.AppendFormatted("// Delete Unused Folders\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::CleanUpFolders()\n{\n",shortCut.Data());
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderNoReset[i] && !folderSupport[i]) {
         if (folderArray[i] == "variable") {
            buffer.AppendFormatted("   int i;\n");
            break;
         }
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderNoReset[i] && !folderSupport[i]) {
         if (folderArray[i] == "variable") {
            buffer.AppendFormatted("   for (i = gAnalyzer->Get%ss()->GetEntriesFast() - 1; i >= 0; i--) {\n",
                                   folderName[i].Data());
            buffer.AppendFormatted("      if (static_cast<%s%s*>(gAnalyzer->Get%sAt(i))->isModified())\n",shortCut.Data(),
                                   folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("         break;\n");
            buffer.AppendFormatted("      gAnalyzer->Get%ss()->RemoveAt(i);\n",folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // reset folders
   buffer.AppendFormatted("// Reset Folders\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::ResetFolders()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
   buffer.AppendFormatted("      return;\n");
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i] && folderArray[i] != "1") {
         buffer.AppendFormatted("   int i;\n");
         buffer.AppendFormatted("   int nentry;\n");
         break;
      }
   }
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!FolderToBeGenerated(i) || folderSupport[i])
         continue;
      if (folderNoReset[i]) {
         if (!folderNoResetModified[i]) {
            if (folderArray[i] == "1") {
               buffer.AppendFormatted("   gAnalyzer->Get%s()->ResetModified();\n",folderName[i].Data());
            } else {
               buffer.AppendFormatted("   nentry = gAnalyzer->Get%ss()->GetEntriesFast();\n",folderName[i].Data());
               buffer.AppendFormatted("   for (i = 0; i < nentry; i++) {\n");
               buffer.AppendFormatted("      static_cast<%s%s*>(gAnalyzer->Get%sAt(i))->ResetModified();\n",shortCut.Data(),
                                      folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("   }\n");
            }
         }
      } else {
         if (folderArray[i] == "1") {
            buffer.AppendFormatted("   gAnalyzer->Get%s()->Reset();\n",folderName[i].Data());
         } else {
            buffer.AppendFormatted("   nentry = gAnalyzer->Get%ss()->GetEntriesFast();\n", folderName[i].Data());
            buffer.AppendFormatted("   %s%s *p%s;\n", shortCut.Data(), folderName[i].Data(), folderName[i].Data());
            buffer.AppendFormatted("   for (i = 0; i < nentry; i++) {\n");
            buffer.AppendFormatted("      p%s = static_cast<%s%s*>(gAnalyzer->Get%sAt(i));\n", folderName[i].Data(),
                                   shortCut.Data(), folderName[i].Data(), folderName[i].Data());
            buffer.AppendFormatted("      if (p%s) {p%s->Reset();}\n", folderName[i].Data(), folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Array Folders
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::InitArrayFolders()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (FolderToBeGenerated(i) && !folderSupport[i]) {
         if (folderArray[i] != "1" && folderArray[i] != "variable")
            buffer.AppendFormatted("   gAnalyzer->Set%sSize(%s);\n",folderName[i].Data(),folderArray[i].Data());
      }
   }
   buffer.AppendFormatted("}\n\n");

   // fill trees
   buffer.AppendFormatted("// Tree Filling\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::FillTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   Int_t eventID = gAnalyzer->GetEventID() == -1 ? 1 : gAnalyzer->GetEventID();\n");
   buffer.AppendFormatted("   Statistics *stat = (eventID > gAnalyzer->GetMaxEventID() || eventID <= 0) ?\n");
   buffer.AppendFormatted("                      0 : gAnalyzer->GetStatisticsAt(eventID - 1);\n");
   if (numOfTree>0) {
      buffer.AppendFormatted("   ROMETree *romeTree;\n");
   }
   breaking = false;
   for (i = 0; i < numOfTree && !breaking; i++) {
      for (j = 0; j < numOfBranch[i] && !breaking; j++) {
         for (k = 0; k < numOfFolder; k++) {
            if (branchFolder[i][j] == folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         if (folderArray[iFold] != "1") {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   int nentry;\n");
            breaking = true;
         }
      }
   }
   buffer.AppendFormatted("   // Fill Trees;\n");
   buffer.AppendFormatted("   bool write;\n");
   buffer.AppendFormatted("   write = false;\n"); // to suppress unused warning
   buffer.AppendFormatted("   bool written;\n");
   buffer.AppendFormatted("   written = false;\n"); // to suppress unused warning
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("   write = false;\n");
      buffer.AppendFormatted("   romeTree = static_cast<ROMETree*>(gAnalyzer->GetTreeObjectAt(%d));\n",i);
      buffer.AppendFormatted("   if (romeTree->isFill()) {\n");
      buffer.AppendFormatted("      write = fAlwaysFillTrees;\n");
      for (j = 0; j < numOfBranch[i]; j++) {
         for (k = 0; k < numOfFolder; k++) {
            if (folderName[k] == branchFolder[i][j] && !folderSupport[k]) {
               iFold = k;
               break;
            }
         }
         if (!folderUsed[iFold])
            continue;
         if (folderArray[iFold] == "1") {
            buffer.AppendFormatted("      if (!write && gAnalyzer->Get%s()->isModified()) {\n",
                                   branchFolder[i][j].Data());
            buffer.AppendFormatted("         write = true;\n");
            buffer.AppendFormatted("      }\n");
         } else {
            buffer.AppendFormatted("      nentry = gAnalyzer->Get%ss()->GetEntriesFast();\n",branchFolder[i][j].Data());
            buffer.AppendFormatted("      for (i = 0; !write && i < nentry; i++) {\n");
            buffer.AppendFormatted("         if (static_cast<%s%s*>(gAnalyzer->Get%sAt(i))->isModified()) {\n",shortCut.Data(),
                                   branchFolder[i][j].Data(),branchFolder[i][j].Data());
            buffer.AppendFormatted("            write = true;\n");
            buffer.AppendFormatted("            break;\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
         }
      }
      buffer.AppendFormatted("      if (write) {\n");
      buffer.AppendFormatted("         written = true;\n");
      buffer.AppendFormatted("         romeTree->GetTree()->Fill();\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   if (written && stat) {\n");
   buffer.AppendFormatted("      stat->writtenEvents++;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // get tree file names
   buffer.AppendFormatted("// Get Tree File Names\n");
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::GetTreeFileName(ROMEString& buffer, Int_t treeIndex, Bool_t inputFile) const\n{\n",
                          shortCut.Data());
   if (numOfTree>0) {
      buffer.AppendFormatted("   ROMETree *romeTree = static_cast<ROMETree*>(gAnalyzer->GetTreeObjectAt(treeIndex));\n");
      buffer.AppendFormatted("   ROMEString base;\n");
      buffer.AppendFormatted("   if (inputFile) {\n");
      buffer.AppendFormatted("      base = romeTree->GetConfigInputFileName();\n");
      buffer.AppendFormatted("   } else {\n");
      buffer.AppendFormatted("      base = romeTree->GetConfigOutputFileName();\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   if (base.Length() == 0) {\n");
      buffer.AppendFormatted("      switch (treeIndex) {\n");
      for (i = 0; i < numOfTree; i++) {
         buffer.AppendFormatted("      case %d:\n",i);
         if (treeFileName[i].Length() == 0) {
            buffer.AppendFormatted("         base.SetFormatted(\"%s#.root\");\n",treeName[i].Data());
         } else {
            buffer.AppendFormatted("         base.SetFormatted(%s);\n",treeFileName[i].Data());
         }
         buffer.AppendFormatted("         break;\n");
      }
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   if (inputFile) {\n");
      buffer.AppendFormatted("      gAnalyzer->ConstructFilePath(gAnalyzer->GetInputDirString(), base, buffer);\n");
      buffer.AppendFormatted("   } else {\n");
      buffer.AppendFormatted("      gAnalyzer->ConstructFilePath(gAnalyzer->GetOutputDirString(), base, buffer);\n");
      buffer.AppendFormatted("   }\n");
   } else {
      buffer.AppendFormatted("   return;\n");
      buffer.AppendFormatted("   WarningSuppression(buffer);\n");
      buffer.AppendFormatted("   WarningSuppression(treeIndex);\n");
      buffer.AppendFormatted("   WarningSuppression(inputFile);\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Hot Links
   if (midas) {
      // Initialize Hot Links
      ROMEString steerPointer;
      ROMEString steerPath;
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sEventLoop::InitHotLinks()\n{\n",shortCut.Data());
      if (numOfEvent>0) {
         buffer.AppendFormatted("   if (gAnalyzer->IsActiveDAQ(\"Midas\")) {\n");
         for (i = 0; i < numOfTaskHierarchy; i++) {
            if (!taskUsed[taskHierarchyClassIndex[i]])
               continue;
            buffer.AppendFormatted("      gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->Active = gAnalyzer->GetTaskObjectAt(%d)->IsActive();\n",
                                   taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                   taskHierarchyObjectIndex[i]);
            for (j = 0; j < numOfSteering[i]; j++) {
               for (k = 0; k < numOfSteerFields[i][j]; k++) {
                  if (steerFieldHotLink[i][j][k]) {
                     GetSteerPath(steerPath,i,j,k,"_");
                     GetSteerPath(steerPointer,i,j,k,"()->Get");
                     buffer.AppendFormatted("      gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->%s = static_cast<%sT%s_Base*>(gAnalyzer->GetTaskObjectAt(%d))->GetSP()->Get%s();\n",
                                            taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),steerPath.Data(),
                                            shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),
                                            taskHierarchyObjectIndex[i],steerPointer.Data());
                  }
               }
            }
         }
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("}\n\n");

      // Update Hot Links
      buffer.Append(kMethodLine);
      buffer.AppendFormatted("void %sEventLoop::UpdateHotLinks()\n{\n",shortCut.Data());
      if (numOfEvent>0) {
         buffer.AppendFormatted("   if (gAnalyzer->IsActiveDAQ(\"Midas\")) {\n");
         for (i = 0; i < numOfTaskHierarchy; i++) {
            if (!taskUsed[taskHierarchyClassIndex[i]])
               continue;
            buffer.AppendFormatted("      if (gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->Active)\n",
                                   taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
            buffer.AppendFormatted("         gAnalyzer->GetTaskObjectAt(%d)->SetActive(true);\n",
                                   taskHierarchyObjectIndex[i]);
            buffer.AppendFormatted("      else\n");
            buffer.AppendFormatted("         gAnalyzer->GetTaskObjectAt(%d)->SetActive(false);\n",
                                   taskHierarchyObjectIndex[i]);
            for (j = 0; j < numOfSteering[i]; j++) {
               for (k = 0; k < numOfSteerFields[i][j]; k++) {
                  if (steerFieldHotLink[i][j][k]) {
                     GetSteerPath(steerPath,i,j,k,"_");
                     GetSteerPath(steerPointer,i,j,k,"()->Get");
                     buffer.AppendFormatted("      static_cast<%sT%s_Base*>(gAnalyzer->GetTaskObjectAt(%d))->GetSP()->Get%s(",
                                            shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),
                                            taskHierarchyObjectIndex[i],steerPointer.Data());
                     buffer[buffer.Last('G')] = 'S';
                     buffer.AppendFormatted("gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->%s",
                                            taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),
                                            steerPath.Data());
                     buffer.AppendFormatted(");\n");
                  }
               }
            }
         }
         buffer.AppendFormatted("      PropagateDeactivation();\n");
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("}\n\n");
   }

   // Reset statistics
   buffer.Append(kMethodLine);
   buffer.AppendFormatted("void %sEventLoop::ResetStatistics()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   memset(fStatisticsTimeOfLastEvent, 0, sizeof(ULong_t) * gAnalyzer->GetMaxEventID());\n");
   buffer.AppendFormatted("   memset(fStatisticsLastEvent, 0, sizeof(Double_t) * gAnalyzer->GetMaxEventID());\n");
   buffer.AppendFormatted("   Statistics *stat;\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   for (i = 0; i < gAnalyzer->GetMaxEventID(); i++) {\n");
   buffer.AppendFormatted("      stat = gAnalyzer->GetStatisticsAt(i);\n");
   buffer.AppendFormatted("      stat->processedEvents = 0;\n");
   buffer.AppendFormatted("      stat->eventsPerSecond = 0;\n");
   buffer.AppendFormatted("      stat->writtenEvents = 0;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteEventLoopH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sEventLoop.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);
   buffer.AppendFormatted("#ifndef %sEventLoop_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sEventLoop_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sEventLoop", shortCut.Data());
   clsDescription.SetFormatted("Framework specific event loop class for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMEEventLoop.h\"\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("class ROMEString;\n");

   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("\nclass %sEventLoop : public ROMEEventLoop\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Fields
   buffer.AppendFormatted("\n");

   // Methods
   // Constructor
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   %sEventLoop(const %sEventLoop &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("   %sEventLoop &operator=(const %sEventLoop &c); // not implemented\n", shortCut.Data(),
                          shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sEventLoop(const char *name,const char *title);\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sEventLoop() {}\n",shortCut.Data());
   buffer.AppendFormatted("   void AddTreeBranches();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");

   // Folders
   buffer.AppendFormatted("   // Folder Methodes\n");
   buffer.AppendFormatted("   void InitArrayFolders();\n");
   buffer.AppendFormatted("   void ResetFolders();\n");
   buffer.AppendFormatted("   void CleanUpFolders();\n");
   buffer.AppendFormatted("\n");

   // Trees
   buffer.AppendFormatted("   // Tree Methodes\n");
   buffer.AppendFormatted("   void InitTrees();\n");
   buffer.AppendFormatted("   void FillTrees();\n");
   buffer.AppendFormatted("   void GetTreeFileName(ROMEString& buffer,Int_t treeIndex, Bool_t inputFile = kFALSE) const;\n");
   buffer.AppendFormatted("   void WriteRunHeaders();\n");
   buffer.AppendFormatted("   void ReadRunHeaders();\n");
   buffer.AppendFormatted("\n");

   // Hot Links
   if (midas) {
      buffer.AppendFormatted("   // Hot Links\n");
      buffer.AppendFormatted("   void InitHotLinks();\n");
      buffer.AppendFormatted("   void UpdateHotLinks();\n");
      buffer.AppendFormatted("\n");
   }

   buffer.AppendFormatted("   void ResetStatistics();\n");
   buffer.AppendFormatted("\n");

   // Footer
   buffer.AppendFormatted("   ClassDef(%sEventLoop, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sEventLoop_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteMain()
{
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString tmp, tmp2;

   cppFile.SetFormatted("%ssrc/generated/main.cpp",outDir.Data());

   buffer.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, kTRUE);

   buffer.AppendFormatted("#include <RConfig.h>\n");
   buffer.AppendFormatted("#include <string.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TGClient.h>\n");
   buffer.AppendFormatted("#include <TSystem.h>\n");
   buffer.AppendFormatted("#include <TException.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMERint.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sVersion.h\"\n",shortCut.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("\n");

   // Additional include path for ACLiC mode.
   buffer.AppendFormatted("const char* const kAdditionalInclude =\n");
   TString currentDirectory = gSystem->WorkingDirectory();
   gSystem->ChangeDirectory(outDir.Data());
   TString outDirAbsolute = gSystem->WorkingDirectory();
   gSystem->ChangeDirectory(currentDirectory.Data());
   ROMEString cmd;
   ROMEString cmdRes;
   ROMEString tmpName;
   if (this->pgsql) {
      cmd.SetFormatted("pg_config --includedir");
      cmdRes.ReadCommandOutput(cmd.Data());
      if (cmdRes.Length() > 0) {
         cmdRes.ReplaceAll("\\\\","/");
         cmdRes.ReplaceAll("\\","/");
         if (cmdRes.EndsWith("\n"))
            cmdRes.Resize(cmdRes.Length() - 1);
         buffer.AppendFormatted("      \" -I%s\"\n", cmdRes.Data());
      }
   }
#if defined( R__UNIX )
   if (this->mysql) {
      cmd.SetFormatted("mysql_config --include");
      cmdRes.ReadCommandOutput(cmd.Data());
      if (cmdRes.Length() > 0) {
         if (cmdRes.EndsWith("\n"))
            cmdRes.Resize(cmdRes.Length() - 1);
         buffer.AppendFormatted("      \" %s\"\n", cmdRes.Data());
      }
   }
#endif // R__UNIX
#if defined( R__MACOSX )
   TString finkDir;
   cmd.SetFormatted("which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\"");
   cmdRes.ReadCommandOutput(cmd.Data());
   if (cmdRes.Length() > 0) {
      if (cmdRes.EndsWith("\n"))
         cmdRes.Resize(cmdRes.Length() - 1);
      finkDir = cmdRes;
   }
   if (finkDir.Length())
      buffer.AppendFormatted("      \" -I%s/include\"\n", finkDir.Data());
#endif
   if (this->midas) {
      tmpName = gSystem->ExpandPathName("$(MIDASSYS)/include");
      tmpName.ReplaceAll("\\\\","/");
      tmpName.ReplaceAll("\\","/");
      buffer.AppendFormatted("      \" -I%s\"\n", tmpName.Data());
   }
   tmpName = gSystem->ExpandPathName("$(ROMESYS)/include");
   tmpName.ReplaceAll("\\\\","/");
   tmpName.ReplaceAll("\\","/");
   buffer.AppendFormatted("      \" -I%s\"\n", tmpName.Data());
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,14,0))
   buffer.AppendFormatted("      \" -I%s/array64\"\n", tmpName.Data());
#endif
   tmpName = gSystem->ExpandPathName("$(ROMESYS)/argus/include");
   tmpName.ReplaceAll("\\\\","/");
   tmpName.ReplaceAll("\\","/");
   buffer.AppendFormatted("      \" -I%s\"\n", tmpName.Data());
   outDirAbsolute.ReplaceAll("\\\\","/");
   outDirAbsolute.ReplaceAll("\\","/");
   buffer.AppendFormatted("      \" -I%s/include\"\n", outDirAbsolute.Data());
   buffer.AppendFormatted("      \" -I%s\"\n", outDirAbsolute.Data());
   buffer.AppendFormatted("      \" -I%s/include/generated\"\n", outDirAbsolute.Data());
   if (numOfDAQ > 0) buffer.AppendFormatted("      \" -I%s/include/daqs\"\n", outDirAbsolute.Data());
   if (numOfDB > 0) buffer.AppendFormatted("      \" -I%s/include/databases\"\n", outDirAbsolute.Data());
   if (numOfTab > 0) buffer.AppendFormatted("      \" -I%s/include/tabs\"\n", outDirAbsolute.Data());
   if (numOfTask > 0) buffer.AppendFormatted("      \" -I%s/include/tasks\"\n", outDirAbsolute.Data());
   Int_t i;
   for (i = 0; i < numOfMFIncDirs; i++) {
      tmpName = gSystem->ExpandPathName(mfIncDir[i].Data());
      tmpName.ReplaceAll("\\\\","/");
      tmpName.ReplaceAll("\\","/");
      buffer.AppendFormatted("      \" -I%s\"\n", tmpName.Data());
   }
   if (buffer.EndsWith("\n"))
      buffer.Resize(buffer.Length() - 1);
   buffer.AppendFormatted(";\n");
   buffer.AppendFormatted("\n");

   // Source directory for THTML document generation
   buffer.AppendFormatted("const char* kHTMLSourceDir =\n");
   if (this->pgsql) {
      cmd.SetFormatted("pg_config --includedir");
      cmdRes.ReadCommandOutput(cmd.Data());
      if (cmdRes.Length() > 0) {
         cmdRes.ReplaceAll("\\\\","/");
         cmdRes.ReplaceAll("\\","/");
         if (cmdRes.EndsWith("\n"))
            cmdRes.Resize(cmdRes.Length() - 1);
         buffer.AppendFormatted("      \":%s\"\n", cmdRes.Data());
      }
   }
#if defined( R__UNIX )
   if (this->mysql) {
      cmd.SetFormatted("mysql_config --include");
      cmdRes.ReadCommandOutput(cmd.Data());
      if (cmdRes.Length() > 0) {
         if (cmdRes.EndsWith("\n"))
            cmdRes.Resize(cmdRes.Length() - 1);
         cmdRes.ReplaceAll("-I", "");
         buffer.AppendFormatted("      \":%s\"\n", cmdRes.Data());
      }
   }
#endif // R__UNIX
#if defined( R__MACOSX )
   cmd.SetFormatted("which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\"");
   cmdRes.ReadCommandOutput(cmd.Data());
   if (cmdRes.Length() > 0) {
      if (cmdRes.EndsWith("\n"))
         cmdRes.Resize(cmdRes.Length() - 1);
      finkDir = cmdRes;
   }
   if (finkDir.Length())
      buffer.AppendFormatted("      \":%s/include\"\n", finkDir.Data());
#endif
   if (this->midas) {
      tmpName = gSystem->ExpandPathName("$(MIDASSYS)");
      tmpName.ReplaceAll("\\\\","/");
      tmpName.ReplaceAll("\\","/");
//      buffer.AppendFormatted("      \":%s/src\"\n", tmpName.Data());
      buffer.AppendFormatted("      \":%s/include\"\n", tmpName.Data());
   }
   tmpName = gSystem->ExpandPathName("$(ROMESYS)");
   tmpName.ReplaceAll("\\\\","/");
   tmpName.ReplaceAll("\\","/");
   buffer.AppendFormatted("      \":%s/src\"\n", tmpName.Data());
   buffer.AppendFormatted("      \":%s/include\"\n", tmpName.Data());
#if (ROOT_VERSION_CODE < ROOT_VERSION(5,14,0))
   buffer.AppendFormatted("      \":%s/include/array64\"\n", tmpName.Data());
#endif
   buffer.AppendFormatted("      \":%s/argus/src\"\n", tmpName.Data());
   buffer.AppendFormatted("      \":%s/argus/include\"\n", tmpName.Data());
   buffer.AppendFormatted("      \":%s/src/generated\"\n", outDirAbsolute.Data());
   buffer.AppendFormatted("      \":%s/include/generated\"\n", outDirAbsolute.Data());
   if (numOfDAQ > 0) {
      buffer.AppendFormatted("      \":%s/src/daqs\"\n", outDirAbsolute.Data());
      buffer.AppendFormatted("      \":%s/include/daqs\"\n", outDirAbsolute.Data());
   }
   if (numOfDB > 0) {
      buffer.AppendFormatted("      \":%s/src/databases\"\n", outDirAbsolute.Data());
      buffer.AppendFormatted("      \":%s/include/databases\"\n", outDirAbsolute.Data());
   }
   if (numOfTab > 0) {
      buffer.AppendFormatted("      \":%s/src/tabs\"\n", outDirAbsolute.Data());
      buffer.AppendFormatted("      \":%s/include/tabs\"\n", outDirAbsolute.Data());
   }
   if (numOfTask > 0) {
      buffer.AppendFormatted("      \":%s/src/tasks\"\n", outDirAbsolute.Data());
      buffer.AppendFormatted("      \":%s/include/tasks\"\n", outDirAbsolute.Data());
   }
   for (i = 0; i < numOfMFIncDirs; i++) {
      tmpName = gSystem->ExpandPathName(mfIncDir[i].Data());
      tmpName.ReplaceAll("\\\\","/");
      tmpName.ReplaceAll("\\","/");
      buffer.AppendFormatted("      \":%s\"\n", tmpName.Data());
   }
   for (i = 0;i < numOfMFDictHeaders; i++) {
      if (!mfDictHeaderUsed[i])
         continue;
      tmpName = gSystem->ExpandPathName(gSystem->DirName(mfDictHeaderName[i].Data()));
      tmpName.ReplaceAll("\\\\","/");
      tmpName.ReplaceAll("\\","/");
      if (!buffer.ContainsFast(tmpName.Data()))
         buffer.AppendFormatted("      \":%s\"\n", tmpName.Data());
   }
   for (i = 0; i < numOfMFSources; i++) {
      if (!mfSourceFileUsed[i])
         continue;
      tmpName = gSystem->ExpandPathName(gSystem->DirName(mfSourceFileName[i].Data()));
      tmpName.ReplaceAll("\\\\","/");
      tmpName.ReplaceAll("\\","/");
      if (!buffer.ContainsFast(tmpName.Data()))
         buffer.AppendFormatted("      \":%s\"\n", tmpName.Data());
   }

   if (buffer.EndsWith("\n"))
      buffer.Resize(buffer.Length() - 1);
   buffer.AppendFormatted(";\n");

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("int main(int argc, char *argv[])\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   if (strcmp(gROOT->GetVersion(),ROOT_RELEASE)) {\n");
   buffer.AppendFormatted("      cerr<<\"Version of ROOT which is used when compiling and running are different.\"<<endl;\n");
   buffer.AppendFormatted("      cerr<<\"(\"<<ROOT_RELEASE<<\" and \"<<gROOT->GetVersion()<<\")\"<<endl;\n");
   buffer.AppendFormatted("      cerr<<\"Please rebuild\"<<endl;\n");
   buffer.AppendFormatted("      cerr<<\"   make -k clean;\"<<endl;\n");
   buffer.AppendFormatted("      cerr<<\"   make -k build;\"<<endl;\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   bool nographics = false;\n");
   buffer.AppendFormatted("   bool batch = false;\n");
   buffer.AppendFormatted("   bool daemon = false;\n");
   buffer.AppendFormatted("   bool interactive = false;\n");
   buffer.AppendFormatted("   int mode = ROMEAnalyzer::kAnalyzeNotSpecified;\n");
   buffer.AppendFormatted("   int argn = 1;\n");
   buffer.AppendFormatted("   char progname[512];\n");
   buffer.AppendFormatted("   char batchopt[] = \"-b\";\n");
   buffer.AppendFormatted("   char *argp[2];\n");
   buffer.AppendFormatted("   argp[0] = progname;\n");
   buffer.AppendFormatted("   argp[1] = batchopt;\n");
   buffer.AppendFormatted("   strcpy(argp[0],argv[0]);\n");
   buffer.AppendFormatted("   bool remote = false;\n");
   buffer.AppendFormatted("   char remoteConnection[256];\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   for (i = 1; i < argc; i++) {\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-ng\"))\n");
   buffer.AppendFormatted("         nographics = true;\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-D\"))\n");
   buffer.AppendFormatted("         daemon = true;\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-b\"))\n");
   buffer.AppendFormatted("         batch = true;\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-I\")) {\n");
   buffer.AppendFormatted("         interactive = true;\n");
   buffer.AppendFormatted("         argv[i][0] = '\\0';\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-R\")) {\n");
   buffer.AppendFormatted("         remote = true;\n");
   buffer.AppendFormatted("         argv[i][0] = '\\0';\n");
   buffer.AppendFormatted("         if (i + 1 < argc) {\n");
   buffer.AppendFormatted("            strcpy(remoteConnection, argv[i + 1]);\n");
   buffer.AppendFormatted("            argv[i + 1][0] = '\\0';\n");
   buffer.AppendFormatted("         } else {\n");
   buffer.AppendFormatted("            strcpy(remoteConnection, \"localhost:9090\");\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-m\")) {\n");
   buffer.AppendFormatted("         if (i + 1 >= argc) {\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         if (!strcmp(argv[i + 1], \"online\")) {\n");
   buffer.AppendFormatted("            mode = ROMEAnalyzer::kAnalyzeOnline;\n");
   buffer.AppendFormatted("         } else if (!strcmp(argv[i + 1], \"offline\")) {\n");
   buffer.AppendFormatted("            mode = ROMEAnalyzer::kAnalyzeOffline;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         i++;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gSystem->AddIncludePath(kAdditionalInclude);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (remote) {\n");
   buffer.AppendFormatted("      ROMERint *intapp = new ROMERint(\"App\", &argc, argv, 0, 0, kTRUE);\n");
   buffer.AppendFormatted("      intapp->SetPrompt(\"%s%s [%%d] \");\n", shortCut.ToLower(tmp),
                          mainProgName.ToLower(tmp2));
   buffer.AppendFormatted("      cout<<%sLogo<<endl;\n", shortCut.Data());
   buffer.AppendFormatted("      intapp->SetSocketClientConnection(remoteConnection);\n");
   buffer.AppendFormatted("      intapp->SetRemoteProcess(true);\n");
   buffer.AppendFormatted("      cout<<\"Remote session to \"<<remoteConnection<<endl;\n");
   buffer.AppendFormatted("      intapp->Run(false);\n");
   buffer.AppendFormatted("   } else if (interactive) {\n");
   buffer.AppendFormatted("      TRint *intapp = new TRint(\"App\", &argc, argv, 0, 0, kTRUE);\n");
   buffer.AppendFormatted("      intapp->SetPrompt(\"%s%s [%%d] \");\n", shortCut.ToLower(tmp),
                          mainProgName.ToLower(tmp2));
   buffer.AppendFormatted("      cout<<%sLogo<<endl;\n", shortCut.Data());
   buffer.AppendFormatted("      intapp->Run();\n");
   buffer.AppendFormatted("   } else {\n");
   buffer.AppendFormatted("      if (nographics || batch || daemon) {\n");
   buffer.AppendFormatted("         argn++;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      ROMERint *app = new ROMERint(\"App\", &argn, argp, 0, 0, true);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      if (!gClient || gClient->IsZombie()) {\n");
   buffer.AppendFormatted("         nographics = true;\n");
   buffer.AppendFormatted("      }\n");
#if defined( R__UNIX )
   buffer.AppendFormatted("      if (!nographics && !batch && !daemon && !gSystem->Getenv(\"DISPLAY\")) {\n");
   buffer.AppendFormatted("         cerr<<\"Graphics is disabled because DISPLAY is not set.\"<<endl;\n");
   buffer.AppendFormatted("         nographics = true;\n");
   buffer.AppendFormatted("      }\n");
#endif
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      new %sAnalyzer(app,batch,daemon,nographics,mode);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("      char str[200];\n");
   buffer.AppendFormatted("      sprintf(str,\"ROME - %%s\", gAnalyzer->GetProgramName());\n");
   buffer.AppendFormatted("      SetConsoleTitle(str);\n");
   buffer.AppendFormatted("\n");
#endif
   buffer.AppendFormatted("      TFolder *fMainFolder = gROOT->GetRootFolder()->AddFolder(\"ROME\",\"ROME Folder\");\n");
   buffer.AppendFormatted("      fMainFolder->Add(gAnalyzer);\n");
   buffer.AppendFormatted("      gAnalyzer->SetCintInitialisation(\"%sAnalyzer* gAnalyzer = ((%sAnalyzer*)((TFolder*)gROOT->FindObjectAny(\\\"ROME\\\"))->GetListOfFolders()->MakeIterator()->Next());\");\n",
                          shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("      app->ProcessLine(gAnalyzer->GetCintInitialisation());\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      TRY {\n");
   buffer.AppendFormatted("         if (!gAnalyzer->Start(argc, argv)) {\n");
   buffer.AppendFormatted("            delete gAnalyzer;\n");
   buffer.AppendFormatted("            return 1;\n");
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("      } CATCH(excode) {\n");
   buffer.AppendFormatted("         ROMEAnalyzer::Cleaning();\n");
   buffer.AppendFormatted("         return excode;\n");
   buffer.AppendFormatted("      } ENDTRY;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      delete gAnalyzer;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

//______________________________________________________________________________
void ROMEBuilder::WriteHTMLDoku()
{
   int i = 0, j = 0, k = 0;
   ROMEString buffer;
   ROMEString parentt;
   int depthold = 0;
   int depth = 0;
   bool trodd = true;
   ROMEString separator;

   // Generate task connection map
   ROMEString mapFileName, mapIconName;
   ROMEString dotOption = "";
#if defined( R__MACOSX )
   dotOption = " -Nfontname=/System/Library/Fonts/Times.dfont -Gfontname=/System/Library/Fonts/Times.dfont";
#endif
   mapFileName.SetFormatted("res/documents/%s%s.gif", shortCut.Data(),mainProgName.Data());
   mapIconName.SetFormatted("res/documents/%s%s_icon.gif", shortCut.Data(),mainProgName.Data());
#if defined( R__UNIX )
   char *dotexe = gSystem->Which(gSystem->Getenv("PATH"), "dot", kExecutePermission);
   ROMEString dotCommand;
   ROMEString cmdRes;
   if (dotexe) {
      dotCommand.SetFormatted("dot -V");
      cmdRes.ReadCommandOutput(dotCommand.Data());
      cmdRes.Replace(0, sizeof("dot version ") - 1, "");
      if (cmdRes.Index(" ", 1, TString::kExact) != -1) {
         cmdRes.Resize(cmdRes.Index(" ", 1, TString::kExact));
      }
      Double_t dotVersion = cmdRes.ToDouble();
      if (dotVersion > 1.155) { // I know that v1.6 support GIF output. I don't know about older versions
         dotCommand.SetFormatted("%s -Tgif %s src/generated/%s%s.dot -o %s",dotexe,dotOption.Data(),
                                 shortCut.Data(),mainProgName.Data(),
                                 mapFileName.Data());
         gSystem->Exec(dotCommand.Data());
         dotCommand.SetFormatted("%s -Tgif -Gsize=\"5,5\" %s src/generated/%s%s.dot -o %s",dotexe,dotOption.Data(),
                                 shortCut.Data(),mainProgName.Data(),
                                 mapIconName.Data());
         gSystem->Exec(dotCommand.Data());
      }
   }
   delete [] dotexe;
#endif
   bool mapImageFound = !gSystem->AccessPathName(mapFileName.Data(), kFileExists) &&
         !gSystem->AccessPathName(mapIconName.Data(), kFileExists);

   // Header
   // ------
   buffer.Resize(0);
   buffer.AppendFormatted("<!--\n");
   buffer.AppendFormatted("   *** This file will be overwritten by the ROMEBuilder ***\n");
   buffer.AppendFormatted("   ***      Don't make manual changes to this file      ***\n");
   buffer.AppendFormatted("-->\n\n");

   buffer.AppendFormatted("<HTML>\n");
   buffer.AppendFormatted("<HEAD>\n");
   buffer.AppendFormatted("<a name=top><TITLE>%s%s Manual</TITLE></a>\n",shortCut.Data(),mainProgName.Data());
   if (styleSheet.Length()) {
      buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s\">\n",styleSheet.Data());
   } else {
      buffer.AppendFormatted("<STYLE>\n");
      WriteHTMLStyle(buffer);
      buffer.AppendFormatted("</STYLE>\n");
   }
#if 0
   if (baseURL.Length()) {
      buffer.AppendFormatted("<BASE HREF=\"%s\">\n", baseURL.Data());
   }
#endif
   buffer.AppendFormatted("</HEAD>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<H1>%s%s Manual</H1>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   // Table of Contents
   // -----------------
   buffer.AppendFormatted("<H2>Table of Contents</H2>\n");
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#introduction\">Introduction</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#tasks\">Tasks</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#tabs\">Tabs</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#folders\">Folders</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#supportFolders\">Support Folders</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#database\">Database Connections</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#trees\">Trees</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#steers\">Global Steering Parameters</a></li>\n");
   if (numOfEvent > 0)
      buffer.AppendFormatted("<li><a href=\"#midasbanks\">Midas Banks</a></li>\n");
   if (affiliationList.GetEntriesFast() > 0)
      buffer.AppendFormatted("<li><a href=\"#affiliations\">Affiliations</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",
                          shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/ClassIndex.html\">Class Overview</A></li>\n", baseURL.Data());
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   // Introduction
   // ------------
   buffer.AppendFormatted("<H2><a name=introduction>Introduction</a> </H2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%s\n",mainDescription.Data());
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Tasks
   // -----
   buffer.AppendFormatted("<h2><a name=tasks>Tasks</a></h2>\n");
   buffer.AppendFormatted("\n");
   // Index
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#taskhierarchy\">Task Hierarchy</a></li>\n");
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("<li><a href=\"#%sTask\">%s</a></li>\n",taskName[i].Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>\n");
   buffer.AppendFormatted("<p>\n\n");
   buffer.AppendFormatted("<hr size=\"1\">\n");
   buffer.AppendFormatted("<p>\n\n");
   // Task Hierarchy
   buffer.AppendFormatted("<h3><a name=taskhierarchy>Task Hierarchy</a></h3>\n");
   if (mapImageFound) {
      buffer.AppendFormatted("<a href=\"%s\"><img src=\"%s\"></a>\n", mapFileName.Data(), mapIconName.Data());
      buffer.AppendFormatted("This image was generated from &lt;TaskConnectedFrom&gt; tag in task definition.\n");
   }

   buffer.AppendFormatted("<p>\n");
   int ddelta;
   depthold = 1;
   buffer.AppendFormatted("<ul>\n");
   for (i = 0; i < numOfTaskHierarchy; i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      depth=taskHierarchyLevel[i];
      ddelta = depth-depthold;
      if (ddelta>0) for (k = 0; k < ddelta; k++)  buffer.AppendFormatted("<ul>\n");
      if (ddelta < 0) for (k = 0; k < -ddelta; k++) buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<li type=\"circle\"><h4><a href=\"#%sTask\">%s</a></h4></li>\n",
                             taskHierarchyName[i].Data(),taskHierarchyName[i].Data());
      depthold = depth;
   }
   for (i = 0; i < depth; i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#tasks\">Tasks</a>\n");
   buffer.AppendFormatted("<p>\n\n");
   buffer.AppendFormatted("<hr size=\"1\">\n");
   buffer.AppendFormatted("<p>\n\n");
   // Tasks
   ROMEString cppFile;
   ROMEString str;
   for (i = 0; i < numOfTask; i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("<h3><a name=%sTask class=\"object\">%s</a></h3>\n",taskName[i].Data(),taskName[i].Data());
      buffer.AppendFormatted("<p>\n");
      separator = "";
      if (numOfTaskAuthors[i] > 0) {
         buffer.AppendFormatted("contact person :");
      }
      for (j = 0; j < numOfTaskAuthors[i]; j++) {
         if (taskAuthorEmail[i][j].Length()) {
            if (!taskAuthorEmail[i][j].ContainsFast("://")) {
               buffer.AppendFormatted("%s <a href=\"mailto:%s\">%s</a>\n", separator.Data(),
                                      taskAuthorEmail[i][j].Data(), taskAuthor[i][j].Data());
            } else {
               buffer.AppendFormatted("%s <a href=\"%s\">%s</a>\n", separator.Data(),
                                      taskAuthorEmail[i][j].Data(), taskAuthor[i][j].Data());
            }
         } else {
            buffer.AppendFormatted("%s %s\n", separator.Data(), taskAuthor[i][j].Data());
         }
         separator = ",";
      }
      buffer.AppendFormatted("\n");
      if (taskDescription[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Description</h4>\n");
         buffer.Append(taskDescription[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (taskUsage[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Usage</h4>\n");
         buffer.Append(taskUsage[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (taskStatus[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Status</h4>\n");
         buffer.Append(taskStatus[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (taskToDo[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>To Do</h4>\n");
         buffer.Append(taskToDo[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (taskKnownProblems[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Known Problems</h4>\n");
         buffer.Append(taskKnownProblems[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Steering Parameters</h4>\n");
      buffer.AppendFormatted("<p>\n");
      if (numOfSteerChildren[i][0]>0 || numOfSteerFields[i][0]>0) {
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
         WriteHTMLSteering(buffer, 0, i,"");
         buffer.AppendFormatted("</table>\n");
      } else {
         buffer.AppendFormatted("This task containes no steering parameters.\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Histograms</h4>\n");
      buffer.AppendFormatted("<p>\n");
      if (numOfHistos[i]>0) {
         buffer.AppendFormatted("This task containes the following histograms :\n");
         buffer.AppendFormatted("<ul>\n");
         for (j = 0; j < numOfHistos[i]; j++) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",histoName[i][j].Data());
         }
         buffer.AppendFormatted("</ul>\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      } else {
         buffer.AppendFormatted("This task containes no histograms.\n");
      }
      if (numOfGraphs[i]>0) {
         buffer.AppendFormatted("This task containes the following graphs :\n");
         buffer.AppendFormatted("<ul>\n");
         for (j = 0; j < numOfGraphs[i]; j++) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",graphName[i][j].Data());
         }
         buffer.AppendFormatted("</ul>\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      } else {
         buffer.AppendFormatted("This task containes no graphs.\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Folders</h4>\n");
      buffer.AppendFormatted("<p>\n");
      cppFile.SetFormatted("src/tasks/%sT%s.cpp",shortCut.Data(),taskName[i].Data()); // we are already in outputDir
      buffer.AppendFormatted("This task accesses data from the following folders :\n");
      buffer.AppendFormatted("<ul>\n");
      for (j = 0; j < numOfFolder; j++) {
         if (!folderUsed[j]) {
            continue;
         }
         if (accessFolder(cppFile.Data(), j, kTRUE)) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[j].Data());
         }
      }
      buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>More Info</h4>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"%s/%sT%s.html\">class file</a>\n",
                             baseURL.Data(), shortCut.Data(), taskName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#tasks\">Tasks</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0, buffer.Length() - 19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Tabs
   // -----
   buffer.AppendFormatted("<h2><a name=tabs>Tabs</a></h2>\n");
   buffer.AppendFormatted("\n");
   // Index
   buffer.AppendFormatted("<ul>\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("<li><a href=\"#%sTab\">%s</a></li>\n",tabName[i].Data(),tabName[i].Data());
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>\n");
   buffer.AppendFormatted("<p>\n\n");
   buffer.AppendFormatted("<hr size=\"1\">\n");
   buffer.AppendFormatted("<p>\n\n");
   // Tabs
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("<h3><a name=%sTab class=\"object\">%s</a></h3>\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("<p>\n");
      separator = "";
      if (numOfTabAuthors[i] > 0) {
         buffer.AppendFormatted("contact person :");
      }
      for (j = 0; j < numOfTabAuthors[i]; j++) {
         if (tabAuthorEmail[i][j].Length()) {
            if (!tabAuthorEmail[i][j].ContainsFast("://")) {
               buffer.AppendFormatted("%s <a href=\"mailto:%s\">%s</a>\n", separator.Data(),
                                      tabAuthorEmail[i][j].Data(), tabAuthor[i][j].Data());
            } else {
               buffer.AppendFormatted("%s <a href=\"%s\">%s</a>\n", separator.Data(),
                                      tabAuthorEmail[i][j].Data(), tabAuthor[i][j].Data());
            }
         } else {
            buffer.AppendFormatted("%s %s\n", separator.Data(), tabAuthor[i][j].Data());
         }
         separator = ",";
      }
      buffer.AppendFormatted("\n");
      if (tabDescription[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Description</h4>\n");
         buffer.Append(tabDescription[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (tabUsage[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Usage</h4>\n");
         buffer.Append(tabUsage[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (tabStatus[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Status</h4>\n");
         buffer.Append(tabStatus[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (tabToDo[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>To Do</h4>\n");
         buffer.Append(tabToDo[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      if (tabKnownProblems[i].Length()) {
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<h4>Known Problems</h4>\n");
         buffer.Append(tabKnownProblems[i]);
         buffer.AppendFormatted("\n</p>\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Steering Parameters</h4>\n");
      buffer.AppendFormatted("<p>\n");
      if (numOfSteerChildren[numOfTask + 1 + i][0]>0 || numOfSteerFields[numOfTask + 1 + i][0]>0) {
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
         WriteHTMLSteering(buffer, 0, numOfTask + 1 + i,"");
         buffer.AppendFormatted("</table>\n");
      } else {
         buffer.AppendFormatted("This tab containes no steering parameters.\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>More Info</h4>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"%s/%sT%s.html\">class file</a>\n",
                             baseURL.Data(), shortCut.Data(), tabName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#tabs\">Tabs</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0, buffer.Length() - 19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Folders
   // -------
   // normal folders listing
   buffer.AppendFormatted("<h2><a name=folders>Folders</a></h2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following folder structure :\n",shortCut.Data(),
                          mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   depthold = 0;
   depth = 0;
   // Structure
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (folderSupport[i])
         continue;
      depth = 0;
      if (folderParentName[i] != "GetMainFolder()") {
         depth++;
         parentt = folderParentName[i];
         for (j = 0; j < maxNumberOfFolders; j++) {
            for (k = 0; k < numOfFolder; k++) {
               if (!folderUsed[k])
                  continue;
               if (parentt == folderName[k]) {
                  parentt = folderParentName[k];
                  break;
               }
            }
            if (k >= numOfFolder) {
               cout<<"Invalid folder structure."<<endl;
               return;
            }
            if (folderParentName[k] == "GetMainFolder()")
               break;
            depth++;
         }
      }
      ddelta = depth-depthold;
      if (ddelta > 0) for (k = 0; k < ddelta; k++)  buffer.AppendFormatted("<ul>\n");
      if (ddelta < 0) for (k = 0; k < -ddelta; k++) buffer.AppendFormatted("</ul>\n");
      if (FolderToBeGenerated(i)) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%sFolder\">%s</a></li>\n",folderName[i].Data(),
                                folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      } else {
         buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[i].Data());
      }
      depthold = depth;
   }
   for (i = 0; i < depth; i++) {
      buffer.AppendFormatted("</ul>\n");
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("The bold folders are data folders. The others are only used to structure the frame work.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#folders\">Folders</a>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"1\">\n");
   buffer.AppendFormatted("<p>\n\n");
   // support folder listing
   buffer.AppendFormatted("<h2><a name=supportFolders>Support Folders</a></h2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following support folders :\n",shortCut.Data(),
                          mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   depthold = 0;
   depth = 0;
   // Structure
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i])
         continue;
      if (FolderToBeGenerated(i)) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%sFolder\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      } else {
         buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#supportFolders\">Support Folders</a>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"1\">\n");
   buffer.AppendFormatted("<p>\n\n");
   ROMEString tmp;
   // Folders
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!FolderToBeGenerated(i)) continue;
      buffer.AppendFormatted("<h3><a name=%sFolder class=\"object\">%s</a></h3>\n",folderName[i].Data(),
                             folderName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("%s<br>\n",folderDescription[i].Data());
      if (folderInheritName[i].Length() > 0) {
         buffer.AppendFormatted("This folder is inherited from <a href=\"#%sFolder\">%s</a>.<br>\n",
                                folderInheritName[i].Data(), folderInheritName[i].Data());
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table>\n");
      if (folderDataBase[i]) {
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Database</td><td>Description</td></tr>\n");
      } else {
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      }
      for (j = 0; j < numOfValue[i]; j++) {
         buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td>",trodd ? "odd" : "even",
                                valueName[i][j].Data(),valueType[i][j].Data());
         if (folderDataBase[i]) {
            if (valueDBPath[i][j].Length() || valueDBName[i][j].Length()) {
               buffer.AppendFormatted("<td><center><a href=\"#db_%s_%s\">Yes</a></center></td>",folderName[i].Data(),
                                      valueName[i][j].Data());
            } else {
               buffer.AppendFormatted("<td><center>No</center></td>");
            }
         }
         buffer.AppendFormatted("<td>&nbsp;%s&nbsp;</td></tr>\n",ProcessCommentHTML(valueComment[i][j],tmp).Data());
         trodd = !trodd;
      }
      buffer.AppendFormatted("</table>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#folders\">Folders</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0, buffer.Length() - 19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Database
   // --------
   ROMEStrArray dbList;
   dbList.RemoveAll();
   for (i = 0; i < numOfFolder; i++) {
      if (!folderUsed[i])
         continue;
      if (!FolderToBeGenerated(i)) continue;
      if (!folderDataBase[i]) continue;
      for (j = 0; j < numOfValue[i]; j++) {
         if (!valueDBPath[i][j].Length() && !valueDBName[i][j].Length()) continue;
         if (dbList.IndexOf(valueDBName[i][j]) < 0)
            dbList.Add(valueDBName[i][j]);
      }
   }
   if (dbList.GetEntriesFast() > 0) {
      buffer.AppendFormatted("<h2><a name=database>Database Connections</a></h2>\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("List of defualt database connections. These values can be overwritten by configuration file.\n");
      buffer.AppendFormatted("<p>\n");
   }
   Int_t iDB;
   for (iDB = 0; iDB < dbList.GetEntriesFast(); iDB++) {
      buffer.AppendFormatted("<h3>%s</h3>\n", dbList.At(iDB).Data());
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Folder</td><td>Field</td><td>Database name</td><td>Database path</td></tr>\n");
      for (i = 0; i < numOfFolder; i++) {
         if (!folderUsed[i])
            continue;
         if (!FolderToBeGenerated(i)) continue;
         if (!folderDataBase[i]) continue;
         for (j = 0; j < numOfValue[i]; j++) {
            if (!valueDBPath[i][j].Length() && !valueDBName[i][j].Length()) continue;
            if (dbList.At(iDB) == valueDBName[i][j])
               buffer.AppendFormatted("<tr class=\"%s\"><td>%s</td><td>&nbsp;<a name = db_%s_%s>%s</a>&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",
                                      trodd ? "odd" : "even",
                                      folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),
                                      valueName[i][j].Data(),valueDBName[i][j].Data(),valueDBPath[i][j].Data());
            trodd = !trodd;
         }
      }
      buffer.AppendFormatted("</table>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#database\">Database Connections</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0, buffer.Length() - 19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Trees
   // -----
   buffer.AppendFormatted("<h2><a name=trees>Trees</a></h2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following trees and branches.\n",shortCut.Data(),
                          mainProgName.Data());
   buffer.AppendFormatted("In brackets are the folders, which are filled to the branch.\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   for (i = 0; i < numOfTree; i++) {
      buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",treeName[i].Data());
      if (treeDescription[i].Length())
         buffer.AppendFormatted("%s\n",treeDescription[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j = 0; j < numOfBranch[i]; j++) {
         buffer.AppendFormatted("<li type=\"circle\">%s (%s)</li>\n",branchName[i][j].Data(),branchFolder[i][j].Data());
      }
      buffer.AppendFormatted("</ul>\n");
   }
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#database\">Database Connections</a>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Global Steering Parameters
   // --------------------------
   buffer.AppendFormatted("<H2><a name=steers>Global Steering Parameters</a> </H2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<a class=\"object\">Global Steering Parameters</a><br>\n");
   buffer.AppendFormatted("<table>\n");
   buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
   WriteHTMLSteering(buffer, 0, numOfTask,"");
   buffer.AppendFormatted("</table>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#steers\">Global Steering Parameters</a>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Midas Banks
   // -----------
   if (numOfEvent > 0 ) {
      buffer.AppendFormatted("<h2><a name=midasbanks>Midas Banks</a></h2>\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("<ul>\n");
      for (i = 0; i < numOfEvent; i++) {
         for (j = 0; j < numOfBank[i]; j++) {
            if (bankType[i][j] == "structure"||bankType[i][j] == "struct") {
               buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankAlias[i][j].Data());
               buffer.AppendFormatted("<ul>\n");
               for (k = 0; k < numOfStructFields[i][j]; k++) {
                  if (bankFieldArraySize[i][j][k] == "1") {
                     buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",structFieldName[i][j][k].Data());
                  } else {
                     buffer.AppendFormatted("<li type=\"disc\">%s[%s]</li>\n",structFieldName[i][j][k].Data(),
                                            bankFieldArraySize[i][j][k].Data());
                  }
               }
               buffer.AppendFormatted("</ul>\n");
            } else {
               buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankAlias[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#midasbanks\">Midas Banks</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
      buffer.AppendFormatted("<p>\n");
   }

   // Affiliations
   // ------------
   if (affiliationList.GetEntriesFast() > 0 ) {
      buffer.AppendFormatted("<h2><a name=affiliations>Affiliations</a></h2>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<ul>\n");
      if (affiliations.GetEntriesFast() == 0) {
         buffer.AppendFormatted("<li>Enabled affiliations</li><ul>\n");
         for (i = 0; i < affiliationList.GetEntriesFast(); i++)
            buffer.AppendFormatted("<li><a href=\"#%saff\">%s</a></li>\n", affiliationList.At(i, 0).Data(),
                                   affiliationList.At(i, 0).Data());
         buffer.AppendFormatted("</ul><li>Disabled affiliations</li><ul>\n");
         buffer.AppendFormatted("</ul>\n");
      } else {
         buffer.AppendFormatted("<li>Enabled affiliations</li><ul>\n");
         for (i = 0; i < affiliationList.GetEntriesFast(); i++) {
            for (j = 0; j < affiliations.GetEntriesFast(); j++) {
               if (affiliationList.At(i, 0) == affiliations.At(j)) {
                  buffer.AppendFormatted("<li><a href=\"#%saff\">%s</a></li>\n", affiliationList.At(i, 0).Data(),
                                         affiliationList.At(i, 0).Data());
                  break;
               }
            }
         }
         buffer.AppendFormatted("</ul><li>Disabled affiliations</li><ul>\n");
         if (affiliations.GetEntriesFast()) {
            for (i = 0; i < affiliationList.GetEntriesFast(); i++) {
               for (j = 0; j < affiliations.GetEntriesFast(); j++) {
                  if (affiliationList.At(i, 0) == affiliations.At(j))
                     break;
               }
               if (j == affiliations.GetEntriesFast())
                  buffer.AppendFormatted("<li><a href=\"#%saff\">%s</a></li>\n", affiliationList.At(i, 0).Data(),
                                         affiliationList.At(i, 0).Data());
            }
         }
         buffer.AppendFormatted("</ul>\n");
      }
      buffer.AppendFormatted("</ul>\n");
      for (i = 0; i < affiliationList.GetEntriesFast(); i++) {
         buffer.AppendFormatted("<h3><a name=\"%saff\" class=\"object\">%s</a></h3>\n", affiliationList.At(i, 0).Data(),
                                affiliationList.At(i, 0).Data());
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Type</td><td>Name</td></tr>\n");
         for (j = 1; j < affiliationList.GetEntriesAt(i); j += 2) {
            buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",
                                   trodd ? "odd" : "even", affiliationList.At(i, j).Data(),
                                   affiliationList.At(i, j + 1).Data());
            trodd = !trodd;
         }
         buffer.AppendFormatted("</table>\n");
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#affiliations\">Affiliations</a>\n");
         buffer.AppendFormatted("<p>\n");
      }
      buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
      buffer.AppendFormatted("<p>\n");
   }

   // Access Methods
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",
                          shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("For a description of all access methods to all types of objects in the %s%s please visit the\n",
                          shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<A TARGET=_top HREF=\"http://midas.psi.ch/rome/usersGuide.html\">users guide on the ROME homepage</A>.\n");
   buffer.AppendFormatted("The <A TARGET=_top HREF=\"http://midas.psi.ch/rome/usersGuide.html#fwaccess\">Access Methods</A> subsection of the\n");
   buffer.AppendFormatted("<A TARGET=_top HREF=\"http://midas.psi.ch/rome/usersGuide.html#framework\">The generated Framework</A> section\n");
   buffer.AppendFormatted("describes all access methods of frameworks generated with ROME.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Footer
   buffer.AppendFormatted("<ADDRESS>\n");
   buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
   for (i = 0; i < numOfMainAuthors; i++) {
      buffer.AppendFormatted("%s</br>\n",mainAuthor[i].Data());
      buffer.AppendFormatted("%s</br>\n",mainInstitute[i].Data());
      buffer.AppendFormatted("%s</br>\n",mainCollaboration[i].Data());
      if (!mainEmail[i].ContainsFast("://")) {
         buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail[i].Data(),mainEmail[i].Data());
      } else {
         buffer.AppendFormatted("link : <a href=\"%s\">%s</a><p>\n",mainEmail[i].Data(),mainEmail[i].Data());
      }
      buffer.AppendFormatted("</br>\n");
   }
   buffer.AppendFormatted("<u> Contact person from ROME</u></br>\n");
   buffer.AppendFormatted("Matthias Schneebeli (PSI)</br>\n");
   buffer.AppendFormatted("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
   buffer.AppendFormatted("</ADDRESS>\n");
   buffer.AppendFormatted("</BODY>\n");
   buffer.AppendFormatted("</HTML>\n");

   // Write documentation
   ROMEString htmlFile;
   htmlFile.SetFormatted("%s%s.html",shortCut.Data(),mainProgName.Data()); // we are already in outputDir
   WriteFile(htmlFile.Data(),buffer.Data(), 0);

   // Write style sheet
   ROMEString css;
   if (styleSheet.Length()) {
      css.SetFormatted("%s/%s",outDir.Data(),styleSheet.Data());
      if (gSystem->AccessPathName(css.Data(),kFileExists)) {
         buffer.Resize(0);
         WriteHTMLStyle(buffer);
         WriteFile(css.Data(),buffer.Data(), 0);
      }
   }

   // Write UserHTML
#if 0
   ROMEString userHtmlFile;
   userHtmlFile.SetFormatted("%sUserHTML.html",shortCut.Data()); // we are already in outputDir
   if (gSystem->AccessPathName(userHtmlFile.Data(),kFileExists)) {
      buffer.Resize(0);
      buffer.AppendFormatted("<html>\n");
      buffer.AppendFormatted("<head>\n");
      buffer.AppendFormatted("  <title>%s%s Additional Info</title>\n",shortCut.Data(),mainProgName.Data());
      if (styleSheet.Length()) {
         buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s\">\n",outDir.Data(),styleSheet.Data());
      } else {
         buffer.AppendFormatted("<STYLE>\n");
         WriteHTMLStyle(buffer);
         buffer.AppendFormatted("</STYLE>\n");
      }
      buffer.AppendFormatted("</head>\n");
      buffer.AppendFormatted("<body>\n");
      buffer.AppendFormatted("  <h1>Additional Info</h1>\n");
      buffer.AppendFormatted("  <a href=\"%s%s.html\">Main page</a>\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("\n\n\n\n");
      buffer.AppendFormatted("  <hr width=\"100%%\">\n");
      buffer.AppendFormatted("<address>\n");
      buffer.AppendFormatted("<u> Contact person from the %s Experiment</u></br>\n",shortCut.Data());
      for (i = 0; i < numOfMainAuthors; i++) {
         buffer.AppendFormatted("%s</br>\n",mainAuthor[i].Data());
         buffer.AppendFormatted("%s</br>\n",mainInstitute[i].Data());
         buffer.AppendFormatted("%s</br>\n",mainCollaboration[i].Data());
         buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail[i].Data(),mainEmail[i].Data());
         buffer.AppendFormatted("</br>\n");
      }
      buffer.AppendFormatted("<u> Contact person from ROME</u></br>\n");
      buffer.AppendFormatted("Matthias Schneebeli (PSI)</br>\n");
      buffer.AppendFormatted("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
      buffer.AppendFormatted("</address>\n");
      buffer.AppendFormatted("</body>\n");
      buffer.AppendFormatted("</html>");
      WriteFile(userHtmlFile.Data(),buffer.Data(), 0);
   }
#endif
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteReadTreesC()
{
   // Write sample macro to read output trees
   int i;
   int iFold;
   int iValue;
   int iTree;
   int iBranch;
   int iDm;
   ROMEString cFile;
   ROMEString buffer;
   ROMEString tmp,tmp2,tmp3,tmp4;
   ROMEString macroDescription;
   buffer.Resize(0);
   macroDescription.Resize(0);

   // make temporary branch name without '.'.
   ROMEString ***branchNameTmp = new ROMEString**[numOfTree];
   for (iTree = 0; iTree < numOfTree; iTree++) {
      branchNameTmp[iTree] = new ROMEString*[numOfBranch[iTree]];
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         branchNameTmp[iTree][iBranch] = new ROMEString(branchName[iTree][iBranch].Data());
         branchNameTmp[iTree][iBranch]->ReplaceAll(".", "");
      }
   }

   // File name
   cFile.SetFormatted("%ssrc/generated/%sReadTrees.C", outDir.Data(), shortCut.Data());
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);
   macroDescription.AppendFormatted("This macro shows how to read output file from %s%s.exe.\n\n",
                                    shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   macroDescription.AppendFormatted(" Usage\n");
   macroDescription.AppendFormatted("   %% %s%s.exe -I;\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   macroDescription.AppendFormatted("   %s%s [0] .L src/generated/%sReadTrees.C\n", shortCut.ToLower(tmp),
                                    mainProgName.ToLower(tmp2), shortCut.Data());
   macroDescription.AppendFormatted("   %s%s [1] %sReadTrees(1, 0, 10)\n", shortCut.ToLower(tmp),
                                    mainProgName.ToLower(tmp2), shortCut.Data());
   macroDescription.AppendFormatted("\n");
   macroDescription.AppendFormatted(" Arguments\n");
   macroDescription.AppendFormatted("   run_num : run number\n");
   macroDescription.AppendFormatted("   ev_1    : the first event number to print\n");
   macroDescription.AppendFormatted("   ev_2    : the last event number to print\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      macroDescription.AppendFormatted("   read%s : switch to read %s tree\n", treeName[iTree].Data(),
                                       treeName[iTree].Data());
   macroDescription.AppendFormatted("\n");
   macroDescription.AppendFormatted("This macro reads following trees.\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      macroDescription.AppendFormatted("   %s\n", treeName[iTree].Data());
   macroDescription.AppendFormatted("\n");
   macroDescription.AppendFormatted(" ! This is just an example. This macro does not show all data !\n");
   macroDescription.AppendFormatted("\n");
   WriteDescription(buffer, gSystem->BaseName(cFile.Data()), macroDescription.Data(), false);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
//   buffer.AppendFormatted("#include <TSystem.h>\n");
   buffer.AppendFormatted("#include <TTree.h>\n");
   buffer.AppendFormatted("#include <TFile.h>\n");
   buffer.AppendFormatted("#include <TClonesArray.h>\n");
   buffer.AppendFormatted("#include <Riostream.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

   // check if branch
   Bool_t *isBranch = new Bool_t[maxNumberOfFolders];
   int **branchFolderNum = new int*[maxNumberOfTrees];
   for (i = 0; i < maxNumberOfTrees; i++) {
      branchFolderNum[i] = new int[maxNumberOfBranches];
   }
   for (iTree = 0; iTree < numOfTree; iTree++) {
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         branchFolderNum[iTree][iBranch] = -1;
      }
   }
   for (iFold = 0; iFold < numOfFolder; iFold++) {
      isBranch[iFold] = false;
      if (!folderUsed[iFold])
         continue;
      if (!FolderToBeGenerated(iFold) || folderSupport[iFold])
         continue;
      for (iTree = 0; iTree < numOfTree; iTree++) {
         for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
            if (folderName[iFold] == branchFolder[iTree][iBranch]) {
               isBranch[iFold] = true;
               branchFolderNum[iTree][iBranch] = iFold;
            }
         }
      }
   }

   // Folder includes
   buffer.AppendFormatted("#if !defined(__CINT__)\n");
   buffer.AppendFormatted("#   include \"include/generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sReadTrees(Int_t run_num = 1\n", shortCut.Data());
   buffer.AppendFormatted("                 , Int_t ev_1 = 0\n");
   buffer.AppendFormatted("                 , Int_t ev_2 = 9\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      buffer.AppendFormatted("                 , Bool_t read%s = kTRUE\n", treeName[iTree].Data());
   buffer.AppendFormatted("                 ) {\n");

   // Open files
   buffer.AppendFormatted("   char filename[100];\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      buffer.AppendFormatted("   TFile *%sFile = 0;\n", treeName[iTree].Data());
   for (iTree = 0; iTree < numOfTree; iTree++)
      buffer.AppendFormatted("   TTree *%s = 0;\n", treeName[iTree].Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Open files\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      buffer.AppendFormatted("   if (read%s) {\n", treeName[iTree].Data());
      buffer.AppendFormatted("      sprintf(filename, \"%s%%05d.root\", run_num);\n", treeName[iTree].Data());
      buffer.AppendFormatted("      %sFile = new TFile(filename);\n", treeName[iTree].Data());
      buffer.AppendFormatted("      %s = ((TTree*) %sFile->Get(\"%s\"));\n", treeName[iTree].Data(),
                             treeName[iTree].Data(), treeName[iTree].Data());
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("\n");

   // Folder Fields
   const int scl = shortCut.Length();
   const int ltc = strlen("TClonesArray");
   int typeLen = ltc;
   for (iFold = 0; iFold < numOfFolder; iFold++) {
      if (!folderUsed[iFold])
         continue;
      if (!isBranch[iFold])
         continue;
      if (typeLen < folderName[iFold].Length() + scl)
         typeLen = folderName[iFold].Length() + scl;
   }
   buffer.AppendFormatted("   // Create objects to fill data\n");
   for (iFold = 0; iFold < numOfFolder; iFold++) {
      if (!folderUsed[iFold]) {
         continue;
      }
      if (!isBranch[iFold]) {
         continue;
      }
      if (folderArray[iFold] == "1") {
         buffer.AppendFormatted("   %s%s*%*s %s = new %s%s();\n", shortCut.Data(),
                                folderName[iFold].Data(), typeLen - folderName[iFold].Length() - scl, "",
                                folderName[iFold].Data(), shortCut.Data(), folderName[iFold].Data());
      } else {
         buffer.AppendFormatted("   TClonesArray*%*s %s = new TClonesArray(\"%s%s\");\n", typeLen - ltc, "",
                                folderName[iFold].Data(), shortCut.Data(), folderName[iFold].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Set address
   buffer.AppendFormatted("   // Map branchs and objects\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      buffer.AppendFormatted("   // %s\n", treeName[iTree].Data());
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         buffer.AppendFormatted("   TBranch *branch%s = 0;\n", branchNameTmp[iTree][iBranch]->Data());
      }
      buffer.AppendFormatted("   if (read%s) {\n", treeName[iTree].Data());
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         buffer.AppendFormatted("      branch%s = %s->GetBranch(\"%s\");\n", branchNameTmp[iTree][iBranch]->Data(),
                                treeName[iTree].Data(), branchName[iTree][iBranch].Data());
         buffer.AppendFormatted("      if(branch%s)\n", branchNameTmp[iTree][iBranch]->Data());
         buffer.AppendFormatted("         branch%s->SetAddress(&%s);\n", branchNameTmp[iTree][iBranch]->Data(),
                                branchFolder[iTree][iBranch].Data());
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("\n");

   // Print
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   // Event loop\n");
   buffer.AppendFormatted("   for (i = ev_1; i <= ev_2; i++) {\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      buffer.AppendFormatted("      if (read%s && i >= %s->GetEntries()) break;\n", treeName[iTree].Data(),
                             treeName[iTree].Data());
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("      // Read event\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         buffer.AppendFormatted("      if (read%s && branch%s)\n", treeName[iTree].Data(),
                                branchNameTmp[iTree][iBranch]->Data());
         buffer.AppendFormatted("         branch%s->GetEntry(i);\n", branchNameTmp[iTree][iBranch]->Data());
      }
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("      // Print data\n");
   buffer.AppendFormatted("      cout<<\"*******  Event-\"<<i<<\"  *******\"<<endl;\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      buffer.AppendFormatted("      if (read%s) {\n", treeName[iTree].Data());
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         iFold = branchFolderNum[iTree][iBranch];
         if (iFold == -1)
            continue;
         if (folderArray[iFold] == "1") {
            for (iValue = 0; iValue < numOfValue[iFold]; iValue++) {
               if (isFolder(valueType[iFold][iValue].Data())) {
                  // not yet implemented
               } else {
                  if (valueDimension[iFold][iValue] > 0 ) {
                     buffer.AppendFormatted("//       cout<<left<<setw(50)<<\"   /%s/%s/%s\"<<%s->Get%sAt(",treeName[iTree].Data(),
                                            branchNameTmp[iTree][iBranch]->Data(), valueName[iFold][iValue].Data(),
                                            folderName[iFold].Data(), valueName[iFold][iValue].Data());
                     for (iDm = 0; iDm < valueDimension[iFold][iValue]; iDm++)
                        buffer.AppendFormatted("0, ");
                     buffer.Resize(buffer.Length()-2);
                     buffer.AppendFormatted(")<<endl;\n");
                  } else {
                     buffer.AppendFormatted("         cout<<left<<setw(50)<<\"   /%s/%s/%s\"<<%s->Get%s()<<endl;\n",
                                            treeName[iTree].Data(), branchNameTmp[iTree][iBranch]->Data(),
                                            valueName[iFold][iValue].Data(), folderName[iFold].Data(),
                                            valueName[iFold][iValue].Data());
                  }
               }
            }
         } else {
            buffer.AppendFormatted("         if (%s->GetEntriesFast()) {\n", folderName[iFold].Data());
            for (iValue = 0; iValue < numOfValue[iFold]; iValue++) {
               if (isFolder(valueType[iFold][iValue].Data())) {
                  // not yet implemented
               } else {
                  if (valueDimension[iFold][iValue] > 0 ) {
                     buffer.AppendFormatted("//          cout<<left<<setw(50)<<\"   /%s/%s/%s\"<<static_cast<%s%s*>(%s->At(0))->Get%sAt(",
                                            treeName[iTree].Data(), branchNameTmp[iTree][iBranch]->Data(),
                                            valueName[iFold][iValue].Data(), shortCut.Data(), folderName[iFold].Data(),
                                            folderName[iFold].Data(), valueName[iFold][iValue].Data());
                     for (iDm = 0; iDm < valueDimension[iFold][iValue]; iDm++)
                        buffer.AppendFormatted("0, ");
                     buffer.Resize(buffer.Length()-2);
                     buffer.AppendFormatted(")<<endl;\n");
                  } else {
                     buffer.AppendFormatted("            cout<<left<<setw(50)<<\"   /%s/%s/%s\"<<static_cast<%s%s*>(%s->At(0))->Get%s()<<endl;\n",
                                            treeName[iTree].Data(), branchNameTmp[iTree][iBranch]->Data(),
                                            valueName[iFold][iValue].Data(), shortCut.Data(), folderName[iFold].Data(),
                                            folderName[iFold].Data(), valueName[iFold][iValue].Data());
                  }
               }
            }
            buffer.AppendFormatted("         }\n");
         }
      }
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return;\n");
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cFile.Data(),buffer.Data(),6);

   delete [] isBranch;
   for (i = 0; i < maxNumberOfTrees; i++) {
      delete [] branchFolderNum[i];
   }
   delete [] branchFolderNum;

   for (iTree = 0; iTree < numOfTree; iTree++) {
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         delete branchNameTmp[iTree][iBranch];
      }
      delete [] branchNameTmp[iTree];
   }
   delete [] branchNameTmp;

   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteDOT()
{
   // Write dot file for task map
   ROMEString dotFile;
   ROMEString buffer;
   ROMEString dotDescription;
   buffer.Resize(0);
   dotDescription.Resize(0);

   int i, j, k;
   Int_t maxDepth = 8;
   for (i = 0; i < numOfTaskHierarchy; i++) {
      maxDepth = TMath::Max(taskHierarchyLevel[i], maxDepth);
   }

   // File name
   dotFile.SetFormatted("%ssrc/generated/%s%s.dot", outDir.Data(), shortCut.Data(),mainProgName.Data());
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);
   dotDescription.AppendFormatted("This is a DOT file describing the connection of tasks in %s%s.\n",
                                  shortCut.Data(), mainProgName.Data());
   dotDescription.AppendFormatted("To generate images, graphviz((http://www.graphviz.org) is necessary.\n\n");
   dotDescription.AppendFormatted("You can make an image with a command like,\n");
   dotDescription.AppendFormatted("   dot -Tgif %s%s.dot -o %s%s.gif\n", shortCut.Data(), mainProgName.Data(),
                                  shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, gSystem->BaseName(dotFile.Data()), dotDescription.Data(), false);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("digraph %s%s {\n", shortCut.Data(), mainProgName.Data());
   int ddelta;
   int depth = 0;
   int depthold = 1;
   ROMEString name;
   ROMEString nameTmp;
   Bool_t found;
   unsigned char grayScale;
   buffer.AppendFormatted("graph [compound = true, splines=true, label = \"Task Map of %s%s\", labelloc = t, labeljust = l];\n",
                          shortCut.Data(), mainProgName.Data());
   for (i = 0; i < numOfTaskHierarchy; i++) {
      name.SetFormatted("%s%s", taskHierarchyName[i].Data(), taskHierarchySuffix[i].Data());
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      depth = taskHierarchyLevel[i];
      ddelta = depth - depthold;
      if (ddelta > 0) {
         nameTmp.SetFormatted("%s%s", taskHierarchyName[i - 1].Data(), taskHierarchySuffix[i - 1].Data());
         buffer.AppendFormatted("%*ssubgraph cluster%s {\n", 3 * (depth - 1), "", nameTmp.Data());
         buffer.AppendFormatted("%*slabel = \"%s\";\n", 3 * depth, "", nameTmp.Data());
         buffer.AppendFormatted("%*slabeljust = l;\n", 3 * depth, "");
         grayScale = TMath::Max(static_cast<unsigned char>(0),
                                static_cast<unsigned char>(static_cast<Double_t>(0xFF) / maxDepth * (maxDepth - depth + 1)));
         buffer.AppendFormatted("%*sfillcolor = \"#%02X%02X%02X\";\n", 3 * depth, "", grayScale, grayScale, grayScale);
         buffer.AppendFormatted("%*sstyle = filled;\n", 3 * depth, "");
         buffer.AppendFormatted("%*s%s[style = filled, fillcolor = \"#FFFFFF\"];\n", 3 * depth, "", nameTmp.Data());
      }
      if (ddelta < 0) {
         for (k = 0; k < -ddelta; k++) {
            buffer.AppendFormatted("%*s}\n", 3 * (depth - ddelta - k - 1), "");
         }
      }
      if (i == numOfTaskHierarchy - 1 || taskHierarchyParentIndex[i + 1] != i) {
         buffer.AppendFormatted("%*s%s[style = filled, fillcolor = \"#FFFFFF\"];\n", 3 * depth, "", name.Data());
      }
      for (j = 0; j < numOfTaskHierarchyConnectedFrom[i]; j++) {
         found = kFALSE;
         for (k = i - 1; k >= 0; k--) {
            nameTmp.SetFormatted("%s%s", taskHierarchyName[k].Data(), taskHierarchySuffix[k].Data());
            if (!taskUsed[taskHierarchyClassIndex[k]])
               continue;
            if (nameTmp == taskHierarchyConnectedFrom[i][j]) {
               found = kTRUE;
               break;
            }
         }
         if (!found) {
            cerr<<"Warning : task '"<<taskHierarchyConnectedFrom[i][j]<<"' connected from '"<<name<<"' was not found."<<endl;
         } else {
            buffer.AppendFormatted("%*s%s -> %s;\n", 3 * depth, "", taskHierarchyConnectedFrom[i][j].Data(), name.Data());
         }
      }
      depthold = depth;
   }
   for (i = 1; i < depth; i++) {
      buffer.AppendFormatted("%*s}\n", 3 * (depth - i), "");
   }

   buffer.AppendFormatted("}\n");

   // Write File
   WriteFile(dotFile.Data(),buffer.Data(),6);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WriteVersionH()
{
   // Write XXXVersion.h
   ROMEString hFile;
   ROMEString buffer;
   ROMEString headerDescription;
   ROMEString tmp, tmp2, tmp3;
   hFile.SetFormatted("%sinclude/generated/%sVersion.h", outDir.Data(), shortCut.Data());

   ROMEString revNumber = "0";

   // read current revision
   ROMEString path;
   path.SetFormatted("%s.svn/entries", outDir.Data());

   if (!gSystem->AccessPathName(path,kFileExists)) {
      // check format
      ifstream entryStream(path.Data());
      if (entryStream.good()) {
         ROMEString entryLine;
         entryLine.ReadLine(entryStream);
         if (entryLine.Length() && entryLine.IsDigit()) {
            // This is maybe text format (Subversion 1.4 or lator)
            entryLine.ReadLine(entryStream);
            entryLine.ReadLine(entryStream);
            entryLine.ReadLine(entryStream);
            revNumber = entryLine;
            entryStream.close();
         } else {
            // This is maybe XML format (Subversion 1.3 or older)
            entryStream.close();
            ROMEXML *svnxml = new ROMEXML();
            svnxml->OpenFileForPath(path.Data());
            svnxml->GetPathAttribute("/wc-entries/entry[1]", "revision", revNumber, "0");
            delete svnxml;
         }
      }
   }

   // current time
//   const char* const monthName[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
   TTimeStamp timestamp;
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
   headerDescription.Resize(0);
   WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);
   headerDescription.AppendFormatted("This header contains Subversion revision code of %s%s.\n\n", shortCut.Data(),
                                     mainProgName.Data());
   headerDescription.AppendFormatted("It makes sense when you use Subversion.\n");
   headerDescription.AppendFormatted("\n");
   WriteDescription(buffer, gSystem->BaseName(hFile.Data()), headerDescription.Data(), false);
   buffer.AppendFormatted("\n\n");
   buffer.AppendFormatted("#ifndef %sVersion_H\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("#define %sVersion_H\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#define %s_SVN_REVISION_CODE %s\n",shortCut.ToUpper(tmp),revNumber.Data());
#if 0
   buffer.AppendFormatted("#define %s_RELEASE_DATE \"%s %2d %d\"\n",shortCut.ToUpper(tmp),
                          monthName[month], day, year);
   buffer.AppendFormatted("#define %s_RELEASE_TIME \"%02d:%02d:%02d\"\n",shortCut.ToUpper(tmp),
                          hour, min, sec);
#endif
   ROMEString prog;
   if (revNumber == "0") {
      prog.SetFormatted("%s%s",shortCut.Data(),mainProgName.Data());
   } else {
      prog.SetFormatted("%s%s (rev.%s)",shortCut.Data(),mainProgName.Data(),revNumber.Data());
   }
   int len1 = static_cast<int>((39 - static_cast<double>(prog.Length())) / 2 + 0.5);
   int len2 = static_cast<int>((39 - static_cast<double>(prog.Length())) / 2);
   buffer.AppendFormatted("const char* const %sLogo =\n", shortCut.Data());
   buffer.AppendFormatted("      \"*****************************************\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*");
   buffer.AppendFormatted("%*s%s%*s",len1,"",prog.Data(),len2,"");
   buffer.AppendFormatted("*\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*   generated by the ROME Environment   *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*             %s               *\\n\"\n",romeVersion.Data());
   if (!romeStable) {
      buffer.AppendFormatted("      \"*          SVN Revision %4d            *\\n\"\n", romeRevisionCode);
   } else {
      buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   }
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*****************************************\\n\\n\";\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n\n");

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);
   return true;
}

//______________________________________________________________________________
Bool_t ROMEBuilder::WritePrecompiledHeaders()
{
   // Write XXXVersion.h
   ROMEString hFile;
   ROMEString buffer;
   ROMEString headerDescription;
   ROMEString tmp, tmp2, tmp3;
   ROMEString path;
   ROMEString fileName;
   ROMEString fileExtension;

   Int_t nstrarray = 1; // currently only one precompiled header
   ROMEStrArray** strarray = new ROMEStrArray*[nstrarray];
   strarray[0] = precompiledIncludeHeaders;
   Int_t i ,j;

   for (i = 0; i < nstrarray; i++) {
      AnalyzeFileName(precompiledHeaders->At(i).Data(),path,fileName,fileExtension);
      hFile.SetFormatted("include/%s", precompiledHeaders->At(i).Data()); // we are in outDir.
      buffer.Resize(0);
      headerDescription.Resize(0);
      WriteHeader(buffer, numOfMainAuthors, mainAuthor, mainEmail, true);
      headerDescription.AppendFormatted("Header including headers to be precompiled.\n\n");
      WriteDescription(buffer, gSystem->BaseName(hFile.Data()), headerDescription.Data(), false);
      buffer.AppendFormatted("\n\n");
      buffer.AppendFormatted("#ifndef %s_H\n",fileName.Data());
      buffer.AppendFormatted("#define %s_H\n",fileName.Data());
      buffer.AppendFormatted("\n");
      for (j = 0; j < strarray[i]->GetEntriesFast(); j++)
         buffer.AppendFormatted("#include \"%s\"\n", strarray[i]->At(j).Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("#endif\n\n");
      WriteFile(hFile.Data(),buffer.Data(),6);
   }

   delete [] strarray;
   return true;
}
