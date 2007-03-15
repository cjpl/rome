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
   ROMEString relation;
   ROMEString tempBuffer;
   ROMEString tmp;
   ROMEString format;
   Int_t iDm;

   if (makeOutput) cout << "\n   Output CPP-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (!folderUsed[iFold])
         continue;
      changeableFlagChanged = false;
      if (numOfValue[iFold] == 0) continue;

      // back up old files
      if (folderUserCode[iFold]) {
         cppFile.SetFormatted("%ssrc/generated/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(cppFile.Data());
      }
      else {
         cppFile.SetFormatted("%ssrc/generated/%s%s_Base.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(cppFile.Data());
         cppFile.SetFormatted("%ssrc/folders/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         BackUpFile(cppFile.Data());
      }
      if (changeableFlagChanged) {
         if (folderUserCode[iFold])
            cppFile.SetFormatted("src/generated/%s%s.cpp",shortCut.Data(),folderName[iFold].Data());
         else
            cppFile.SetFormatted("src/folders/%s%s.cpp",shortCut.Data(),folderName[iFold].Data());
         RemoveDepFiles(cppFile.Data());
      }

      // File name
      if (folderUserCode[iFold])
         cppFile.SetFormatted("%ssrc/generated/%s%s_Base.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      else
         cppFile.SetFormatted("%ssrc/generated/%s%s.cpp",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, folderAuthor[iFold].Data(), kTRUE);
      if (folderUserCode[iFold])
         clsName.SetFormatted("%s%s_Base", shortCut.Data(), folderName[iFold].Data());
      else
         clsName.SetFormatted("%s%s", shortCut.Data(), folderName[iFold].Data());
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
      buffer.AppendFormatted("%s::%s(",clsName.Data(),clsName.Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]==0) {
            if (isFolder(valueType[iFold][i].Data()))
               continue;
            if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString") && !isTArrayType(valueType[iFold][i]))
               continue;
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("%sTObject* %s_value",separator.Data(),valueName[iFold][i].Data());
               separator = ", ";
            }
            else {
               buffer.AppendFormatted("%s%s %s_value",separator.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               separator = ", ";
            }
         }
      }
      buffer.AppendFormatted(" )\n");
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("   %s::Class()->IgnoreTObjectStreamer();\n",clsName.Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
             && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         else if (isFolder(valueType[iFold][i].Data())) {
            tmp = valueType[iFold][i];
            tmp.ReplaceAll("*","");
            if (valueDimension[iFold][i]==0) {
               buffer.AppendFormatted("   %s = new %s();\n",valueName[iFold][i].Data(),tmp.Data());
            }
            else {
               buffer.AppendFormatted("   %s = new TClonesArray(\"%s\");\n",valueName[iFold][i].Data(),tmp.Data());
               buffer.AppendFormatted("   %s->SetName(\"%s%s\");\n",valueName[iFold][i].Data(),shortCut.Data(),tmp.Data());
               if (valueArray[iFold][i][0]!="variable")
                  buffer.AppendFormatted("   Set%sSize(%s);\n",valueName[iFold][i].Data(),valueArray[iFold][i][0].Data());
            }
         }
         else if (isTArrayType(valueType[iFold][i])) {
            if (valueType[iFold][i].Contains("*")) {
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("   %s = new %s(%s);\n",valueName[iFold][i].Data(),tmp.Data(),valueArray[iFold][i][0].Data());
               buffer.AppendFormatted("   %s%sReset(%s);\n",valueName[iFold][i].Data(),relation.Data(),valueInit[iFold][i].Data());
            }
         }
         else {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("   %s = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
            else if (valueArray[iFold][i][0]=="variable") {
               buffer.AppendFormatted("   if (%s > 0) {\n",valueArraySpecifier[iFold][i].Data());
               buffer.AppendFormatted("      %s = new %s[%s];\n",valueName[iFold][i].Data(),valueType[iFold][i].Data(),valueArraySpecifier[iFold][i].Data());
               buffer.AppendFormatted("      %sActualSize = %s;\n",valueName[iFold][i].Data(),valueArraySpecifier[iFold][i].Data());
               buffer.AppendFormatted("      %sSize = %s;\n",valueName[iFold][i].Data(),valueArraySpecifier[iFold][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   else {\n");
               buffer.AppendFormatted("      %s = NULL;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %sActualSize = 0;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %sSize = 0;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   }\n");
            }
            else{
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
                  format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++) {\n",iDm*3);
                  buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
               }
               format.SetFormatted("%%%ds      %%s",valueDimension[iFold][i]*3);
               buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
               buffer.AppendFormatted(" = %s;\n",valueInit[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
                  format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
                  buffer.AppendFormatted(format.Data(),"");
               }
            }
         }
      }
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   fModified = false;\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Destructor
      buffer.AppendFormatted("%s::~%s()\n",clsName.Data(),clsName.Data());
      buffer.AppendFormatted("{\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            buffer.AppendFormatted("   SafeDelete(%s);\n",valueName[iFold][i].Data());
         else if (isTArrayType(valueType[iFold][i]) && valueType[iFold][i].Contains("*"))
            buffer.AppendFormatted("   SafeDelete(%s);\n",valueName[iFold][i].Data());
         else if (valueDimension[iFold][i]!=0 && valueArray[iFold][i][0]=="variable")
            buffer.AppendFormatted("   SafeDeleteArray(%s);\n",valueName[iFold][i].Data());
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Getters
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (valueDimension[iFold][i]>0) {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("TRef* %s::Get%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.SetFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return &%s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(";\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t index)\n",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", index))\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  buffer.AppendFormatted("      return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return (%s)(%s->At(index));\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* arrayToCopy)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s%sGetSize() || !n) return;\n",valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("   if (!arrayToCopy) arrayToCopy = new %s[n];\n",TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("   memcpy(arrayToCopy,%s%sGetArray(),n*sizeof(%s));\n",valueName[iFold][i].Data(),relation.Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               buffer.AppendFormatted("%s* %s::Get%sAt(",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return &%s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(";\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t index)\n",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", index)) return %s;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               buffer.AppendFormatted("   return %s[index];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (valueArray[iFold][i][0]=="variable") {
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t index)\n",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", index)) return %s;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               buffer.AppendFormatted("   return %s[index];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(array,%s,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else {
               buffer.AppendFormatted("%s %s::Get%sAt(",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return %s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(";\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(array,&%s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[0]");
               buffer.AppendFormatted(",n*sizeof(%s));\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }
         else {
            if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
                && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
            }
            else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
            }
            else if (valueType[iFold][i]=="TRef") {
            }
            else {
            }
         }
      }
      buffer.AppendFormatted("\n");

      // isModified
      buffer.AppendFormatted("Bool_t %s::isModified()\n",clsName.Data());
      buffer.AppendFormatted("{\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i]>0) {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   int nentry;\n");
            break;
         }
      }
      buffer.AppendFormatted("   if ( fModified ) return true;\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i]==0) {
               buffer.AppendFormatted("   if ( %s->isModified() ) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("      return true;\n");
               buffer.AppendFormatted("   }\n");
            }
            else {
               // check only the first element for speed up.
               buffer.AppendFormatted("   if ( %s->GetEntries() ) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      if ( ((%s)%s->At(0))->isModified() ) {\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("         SetModified(true);\n");
               buffer.AppendFormatted("         return true;\n");
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      // check remaining elements
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i]>0) {
               buffer.AppendFormatted("   nentry = %s->GetEntries();\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   for (i=1;i<nentry;i++) {\n");
               buffer.AppendFormatted("      if ( ((%s)%s->At(i))->isModified() ) {\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
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
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
            }
            else {
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("void %s::Set%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("TObjects* %s_value)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(") return;\n");
               }
               buffer.AppendFormatted("%s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted("%s = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("void %s::Set%sSize(Int_t number)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   int i;\n");
               buffer.AppendFormatted("   if (%s) %s->Delete();\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   for (i=0;i<number;i++) {\n");
               buffer.AppendFormatted("      new((*%s)[i]) %s();\n",valueName[iFold][i].Data(),tmp.Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || !n) return;\n");
               if (valueType[iFold][i].Contains("*"))
                  buffer.AppendFormatted("   if (!%s || %s%sGetSize()<n) Set%sSize(n);\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),relation.Data(),valueName[iFold][i].Data());
               else
                  buffer.AppendFormatted("   if (%s%sGetSize()<n) Set%sSize(n);\n",valueName[iFold][i].Data(),relation.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s%sSet(n,array);\n",valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               continue;
            }
            else if (valueArray[iFold][i][0]=="variable") {
               buffer.AppendFormatted("void %s::Set%sAt(Int_t index,%s %s_value) {\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", index)) return;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s[index] = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");

               buffer.AppendFormatted("void %s::Set%sSize(Int_t number)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (number==%sSize) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (number<0) return;\n");
               buffer.AppendFormatted("   if (number>%sActualSize) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s *tmp = %s;\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s = new %s[number];\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      memcpy(%s,tmp,%sSize*sizeof(%s));\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      delete [] tmp;\n");
               buffer.AppendFormatted("      %sActualSize = number;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   %sSize = number;\n",valueName[iFold][i].Data());
#if 0 // Maybe, this should be taken care of users rather than framework.
               if (valueArraySpecifier[iFold][i].Length())
                  buffer.AppendFormatted("   %s = number;\n",valueArraySpecifier[iFold][i].Data());
#endif
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || n<=0) return;\n");
               buffer.AppendFormatted("   if (%sActualSize<n) Set%sSize(n);\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else {
               buffer.AppendFormatted("void %s::Set%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%s %s_value)\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return;\n");
               }
               buffer.AppendFormatted("   %s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted("= %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");

               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || !n) return;\n");
               buffer.AppendFormatted("   memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }
      }
      buffer.AppendFormatted("\n");
      // Add
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (isPointerType(valueType[iFold][i].Data()))
            continue;
         if (valueIsTObject[iFold][i]
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
            }
            else {
               if (!isBoolType(valueType[iFold][i].Data())) {
               }
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("void %s::Add%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("TObject* %s_value)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Add%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return;\n");
               }
               buffer.AppendFormatted("   %s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted("%s += %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               buffer.AppendFormatted("void %s::Add%sAt(Int_t index,%s %s_value)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Add%sAt\", index)) return;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s tmp = %s%sAt(index);\n",TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("   tmp += %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s%sAddAt(tmp,index);\n",valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (!isBoolType(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("void %s::Add%sAt(",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.AppendFormatted("%s %s_value)\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Add%sAt\", ",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                     buffer.AppendFormatted("%c, ",valueCounter[iDm]);
                  buffer.Resize(buffer.Length()-2);
                  buffer.AppendFormatted(")) return;\n");
               }
               buffer.AppendFormatted("   %s",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[%c]",valueCounter[iDm]);
               buffer.AppendFormatted(" += %s_value;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // ResetModified
      buffer.AppendFormatted("void %s::ResetModified()\n",clsName.Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("   if (isModified()) {\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i]>0) {
            buffer.AppendFormatted("      int nentry;\n");
            break;
         }
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (!isFolder(valueType[iFold][i].Data()))
            continue;
         if (valueDimension[iFold][i]==0)
            buffer.AppendFormatted("      %s->ResetModified();\n",valueName[iFold][i].Data());
         else {
            buffer.AppendFormatted("      nentry = %s->GetEntries();\n",valueName[iFold][i].Data());
            buffer.AppendFormatted("      for (Int_t i%d=0;i%d<nentry;i%d++) { ((%s)%s->At(i%d))->ResetModified(); }\n",i,i,i,valueType[iFold][i].Data(),valueName[iFold][i].Data(),i);
         }
      }
      buffer.AppendFormatted("      SetModified(false);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Set All
      buffer.AppendFormatted("void %s::SetAll( ",clsName.Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("TObject* %s_value,",valueName[iFold][i].Data());
            }
            else {
               buffer.AppendFormatted("%s %s_value,",valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
         }
      }
      buffer.Resize(buffer.Length()-1);
      buffer.AppendFormatted(" )\n");
      buffer.AppendFormatted("{\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
            buffer.AppendFormatted("   %s = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("   SetModified(true);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      // Reset
      buffer.AppendFormatted("void %s::Reset()\n",clsName.Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("   if ( !isModified() ) return;\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i]>0) {
            buffer.AppendFormatted("      int nentry;\n");
            break;
         }
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
             && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString") && !isTArrayType(valueType[iFold][i])) {
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
               format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++) {\n",iDm*3);
               buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
            }
            format.SetFormatted("%%%ds      %%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted(".Clear();\n");
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
               format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
               buffer.AppendFormatted(format.Data(),"");
            }
         }
         else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())
                 && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
               format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++) {\n",iDm*3);
               buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
            }
            format.SetFormatted("%%%ds      if (%%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted(")\n");
            format.SetFormatted("%%%ds         %%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted("->Clear();\n");
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
               format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
               buffer.AppendFormatted(format.Data(),"");
            }
         }
         else if (isFolder(valueType[iFold][i].Data())) {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("   %s->Reset();\n",valueName[iFold][i].Data());
            else {
               buffer.AppendFormatted("   nentry = %s->GetEntries();\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   for (int i%d=0;i%d<nentry;i%d++) { ((%s)%s->At(i%d))->Reset(); }\n",i,i,i,valueType[iFold][i].Data(),valueName[iFold][i].Data(),i);
            }
         }
         else if (isTArrayType(valueType[iFold][i])) {
            buffer.AppendFormatted("   %s%sReset(%s);\n",valueName[iFold][i].Data(),relation.Data(),valueInit[iFold][i].Data());
         }
         else if (valueArray[iFold][i][0]=="variable") {
            buffer.AppendFormatted("   for (int i%d=0;i%d<%sSize;i%d++) {\n",i,i,valueName[iFold][i].Data(),i);
            buffer.AppendFormatted("      %s[i%d] = %s;\n",valueName[iFold][i].Data(),i,valueInit[iFold][i].Data());
            buffer.AppendFormatted("   }\n");
         }
         else {
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
               format.SetFormatted("%%%ds      for (int %%c%%d=0;%%c%%d<%%s;%%c%%d++) {\n",iDm*3);
               buffer.AppendFormatted(format.Data(),"",valueCounter[iDm],i,valueCounter[iDm],i,valueArray[iFold][i][iDm].Data(),valueCounter[iDm],i);
            }
            format.SetFormatted("%%%ds      %%s",valueDimension[iFold][i]*3);
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data());
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
               buffer.AppendFormatted("[%c%d]",valueCounter[iDm],i);
            buffer.AppendFormatted(" = (%s)%s;\n",valueType[iFold][i].Data(),valueInit[iFold][i].Data());
            for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
               format.SetFormatted("%%%ds      }\n",(valueDimension[iFold][i]-iDm-1)*3);
               buffer.AppendFormatted(format.Data(),"");
            }
         }
      }
      buffer.AppendFormatted("   fModified = false;\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // BoundsOk
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]>0) {
            if (isFolder(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (at < 0 || at >= Get%sSize())\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i)",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index %%d out of bounds (size: %%d, this: 0x%%08x)\", i, Get%sSize(), this);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else if (valueArray[iFold][i][0]=="variable") {
#if 0 // disabled because no way to know array size when the folder is read from TFile.
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (at < 0 || at >= Get%sSize())\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i)",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index %%d out of bounds (size: %%d, this: 0x%%08x)\", i, Get%sSize(), this);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
#else
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (%s == 0)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t /*i*/)",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where,\"%s is not allocated. Please allocate with %s::Set%sSize(Int_t number)\");\n",valueName[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
#endif
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
            }
            else {
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, ",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (");
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
                  buffer.AppendFormatted(" %c < 0 ||",valueCounter[iDm]);
                  buffer.AppendFormatted(" %c >= %s ||",valueCounter[iDm],valueArray[iFold][i][iDm].Data());
               }
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where,",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted(" %c,",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-1);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where,",clsName.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted(" Int_t %c,",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-1);
               buffer.AppendFormatted(")\n");
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index ");
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("%%d:");
               buffer.Resize(buffer.Length()-1);
               buffer.AppendFormatted(" out of bounds (size:");
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("%%d:");
               buffer.Resize(buffer.Length()-1);
               buffer.AppendFormatted(" , this: 0x%%08x)\",");
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted(" %c,",valueCounter[iDm]);
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted(" %s,",valueArray[iFold][i][iDm].Data());
               buffer.Resize(buffer.Length()-1);
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
         WriteHeader(buffer, folderAuthor[iFold].Data(), kFALSE);
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

Bool_t ROMEBuilder::WriteFolderH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString header;
   ROMEString clsName;
   ROMEString clsDescription;
   int i,j;
   ROMEString str;
   ROMEString format;
   bool changeableFlagChanged;
   ROMEString relation;
   ROMEString tempBuffer;
   ROMEString tmp;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iFold=0;iFold<numOfFolder;iFold++) {
      if (!folderUsed[iFold])
         continue;
      changeableFlagChanged = false;
      if (numOfValue[iFold] == 0) continue;

      // back up old files
      if (folderUserCode[iFold]) {
         hFile.SetFormatted("%sinclude/generated/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(hFile.Data());
      }
      else {
         hFile.SetFormatted("%sinclude/generated/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         changeableFlagChanged = RemoveFile(hFile.Data());
         hFile.SetFormatted("%sinclude/folders/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
         BackUpFile(hFile.Data());
      }
      if (changeableFlagChanged) {
         if (folderUserCode[iFold])
            hFile.SetFormatted("include/generated/%s%s.h",shortCut.Data(),folderName[iFold].Data());
         else
            hFile.SetFormatted("include/folders/%s%s.h",shortCut.Data(),folderName[iFold].Data());
         RemoveDepFiles(hFile.Data());
      }

      // File name
      if (folderUserCode[iFold])
         hFile.SetFormatted("%sinclude/generated/%s%s_Base.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      else
         hFile.SetFormatted("%sinclude/generated/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, folderAuthor[iFold].Data(), kTRUE);
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("#ifndef %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_Base_H\n\n",shortCut.Data(),folderName[iFold].Data());
      }
      else {
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
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4244 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <TObject.h>\n");
      buffer.AppendFormatted("#include <TClass.h>\n");
      buffer.AppendFormatted("#include <TClonesArray.h>\n");
      for (i=0;i<numOfFolderInclude[iFold];i++) {
         if (folderLocalFlag[iFold][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",folderInclude[iFold][i].Data());
         }
         else {
            buffer.AppendFormatted("#include <%s>\n",folderInclude[iFold][i].Data());
         }
      }
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include <ROMEString.h>\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isTArrayType(valueType[iFold][i])) {
            str = valueType[iFold][i](0,7);
            buffer.AppendFormatted("#include <%s.h>\n",str.Data());
            break;
         }
      }

      for (i=0;i<numOfValue[iFold];i++) {
         for (j=0;j<numOfFolder;j++) {
            if (!folderUsed[j])
               continue;
            str.SetFormatted("%s*",folderName[j].Data());
            if (valueType[iFold][i]==folderName[j] || valueType[iFold][i]==str) {
               if (folderUserCode[j])
                  buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               else
                  buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               if (valueType[iFold][i]==folderName[j])
                  valueType[iFold][i] += "*";
               valueType[iFold][i].Insert(0,shortCut);
               break;
            }
         }
      }

      // Class
      if (folderUserCode[iFold])
         buffer.AppendFormatted("\nclass %s%s_Base : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("\nclass %s%s : public TObject\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("{\n");

      // Fields
      buffer.AppendFormatted("protected:\n");
      int typeLen = 13;
      int nameLen = 8;
      int nameLenT = 0;
      int arrayLen = 0;
      int iDm;
      for (i=0;i<numOfValue[iFold];i++) {
         if (typeLen<valueType[iFold][i].Length()) typeLen = valueType[iFold][i].Length();
         nameLenT = (int)valueName[iFold][i].Length();
         for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
            nameLenT += (int)(2+valueArray[iFold][i][iDm].Length());
         }
         if (nameLen<nameLenT) nameLen = nameLenT;
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()) && valueDimension[iFold][i]>0) {
            if (valueDimension[iFold][i]>1) {
               cout<<"You can not have multiple dimensional array of support folders."<<endl;
               cout << "Terminating program." << endl;
               return false;
            }
            format.SetFormatted("   TClonesArray*%%%ds %%s;%%%ds %%s\n",typeLen-13,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data(),"",ProcessCommentCPP(valueComment[iFold][i],tmp).Data());
         }
         else if (valueArray[iFold][i][0]=="variable") {
            format.SetFormatted("   %%-%ds* %%s;%%%ds %%s\n",typeLen-1,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",ProcessCommentCPP(valueComment[iFold][i],tmp).Data());
            format.SetFormatted("   %%-%ds %%sSize;%%%ds // ! number of elements of %%s\n",typeLen,nameLen-valueName[iFold][i].Length()-4);
            buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
            format.SetFormatted("   %%-%ds %%sActualSize;%%%ds // ! actual size of %%s allocated in memory\n",typeLen,nameLen-valueName[iFold][i].Length()-10);
            buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
         }
         else {
            format.SetFormatted("   %%-%ds %%s",typeLen);
            buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            arrayLen = 0;
            if (!isTArrayType(valueType[iFold][i])) {
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++) {
                  buffer.AppendFormatted("[%s]",valueArray[iFold][i][iDm].Data());
                  arrayLen += 2+valueArray[iFold][i][iDm].Length();
               }
            }
            format.SetFormatted(";%%%ds %%s\n",nameLen-valueName[iFold][i].Length()-arrayLen);
            buffer.AppendFormatted(format.Data(),"",ProcessCommentCPP(valueComment[iFold][i],tmp).Data());
         }
      }
      format.SetFormatted("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-9);
      buffer.AppendFormatted(format.Data(),"Bool_t","Modified","","//! Modified Folder Flag");
      buffer.AppendFormatted("\n");

      // Methods

      buffer.AppendFormatted("public:\n");
      ROMEString separator = "";
      // Constructor
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("   %s%s_Base(",shortCut.Data(),folderName[iFold].Data());
      }
      else {
         buffer.AppendFormatted("   %s%s(",shortCut.Data(),folderName[iFold].Data());
      }
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]==0) {
            if (isFolder(valueType[iFold][i].Data()))
               continue;
            if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString") && !isTArrayType(valueType[iFold][i]))
               continue;
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("%sTObject* %s_value=%s",separator.Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               separator = ", ";
            }
            else {
               buffer.AppendFormatted("%s%s %s_value=%s",separator.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               separator = ", ";
            }
         }
      }
      buffer.AppendFormatted(" );\n");
      buffer.AppendFormatted("\n");

      // Destructor
      if (folderUserCode[iFold])
         buffer.AppendFormatted("   virtual ~%s%s_Base();\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("   virtual ~%s%s();\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("\n");

      // Getters
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]>0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds  Get%%sAt();",typeLen);
               buffer.AppendFormatted(format.Data(),"TRef*",valueName[iFold][i].Data());
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"TClonesArray*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%s->GetEntries();%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index)%%%ds { return %%s%%sAt(index);%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),relation.Data(),"");
               if (valueType[iFold][i].Contains("*")) {
                  format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds }\n",typeLen,lb,lb);
                  buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%s%%sGetSize();%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),relation.Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* arrayToCopy);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
            }
            else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds* Get%%sAt(",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(");\n");
            }
            else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
            else if (valueArray[iFold][i][0]=="variable") {
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return %%s;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%sSize;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data());
            }
            else {
               format.SetFormatted("   %%-%ds  Get%%sAt(",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(");\n");
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return &%%s",typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("[0]");
               format.SetFormatted(";%%%ds }\n",lb);
               buffer.AppendFormatted(format.Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data());
            }
         }
         else {
            if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return &%%s;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (valueIsTObject[iFold][i] && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return &%%s;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds }\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // isModified
      format.SetFormatted("   %%-%ds  is%%s();\n",typeLen);
      buffer.AppendFormatted(format.Data(),"Bool_t","Modified");
      buffer.AppendFormatted("\n");

      // Setters
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds SetModified(true); }\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds SetModified(true); }\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Set%%sAt%%%ds(",lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"");
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"TObject*",valueName[iFold][i].Data());
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               if (valueArray[iFold][i][0]!="variable")
                  buffer.AppendFormatted("private:\n");
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",valueName[iFold][i].Data());
               if (valueArray[iFold][i][0]!="variable")
                  buffer.AppendFormatted("public:\n");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               format.SetFormatted("   void Set%%sAt%%%ds(Int_t index,%%-%ds %%s_value%%%ds) { %%s%%sAddAt(%%s_value,index)%%%ds;%%%ds SetModified(true); }\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),relation.Data(),valueName[iFold][i].Data(),"","");               buffer.AppendFormatted("   void Set%sSize(Int_t number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s%sSet(number);\n",valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
            }
            else if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               continue;
            }
            else if (valueArray[iFold][i][0]=="variable") {
               format.SetFormatted("   void Set%%sAt%%%ds(Int_t index,%%-%ds %%s_value);\n",lb,typeLen);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds* %%s_value%%%ds) { %%s = %%s_value%%%ds;%%%ds SetModified(true); }\n",lb,typeLen-1,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),valueName[iFold][i].Data(),"","");
               buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",valueName[iFold][i].Data());
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data());
            }
            else {
               format.SetFormatted("   void Set%%sAt%%%ds(",lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"");
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
            }
         }
      }
      buffer.AppendFormatted("\n");
      // Add
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (isPointerType(valueType[iFold][i].Data()))
            continue;
         if (valueIsTObject[iFold][i]
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Add%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds += %%s_value;%%%ds SetModified(true); }\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               if (!isBoolType(valueType[iFold][i].Data())) {
                  format.SetFormatted("   void Add%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds += %%s_value;%%%ds SetModified(true); }\n",lb,typeLen,lb,lb,lb);
                  buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
            }
         }
         else {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Add%%sAt(");
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value%%%ds);\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),"TObject*",valueName[iFold][i].Data(),"");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               format.SetFormatted("   void Add%%sAt%%%ds(Int_t index,%%-%ds %%s_value);\n",lb,typeLen);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data());
            }
            else if (!isBoolType(valueType[iFold][i].Data())) {
               format.SetFormatted("   void Add%%sAt(");
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               format.SetFormatted("%%-%ds %%s_value);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
         }
      }
      buffer.AppendFormatted("\n");

      // SetModified
      int lb = nameLen-8;
      format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s;%%%ds}\n",lb,typeLen,lb,lb,lb);
      buffer.AppendFormatted(format.Data(),"Modified","","Bool_t","modified","","Modified","","modified","");
      buffer.AppendFormatted("\n");

      // ResetModified
      buffer.AppendFormatted("   void ResetModified();\n");
      buffer.AppendFormatted("\n");

      // Set All
      buffer.AppendFormatted("   void SetAll( ");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (valueIsTObject[iFold][i] && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               buffer.AppendFormatted("TObject* %s_value=%s,",valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
            else {
               buffer.AppendFormatted("%s %s_value=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
            }
         }
      }
      buffer.Resize(buffer.Length()-1);
      buffer.AppendFormatted(" );\n");
      buffer.AppendFormatted("\n");
      // Reset
      buffer.AppendFormatted("   void Reset();\n");
      buffer.AppendFormatted("\n");

      // Private
      buffer.AppendFormatted("private:\n");

      // BoundsOk
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]>0) {
            if (isFolder(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("   Bool_t %sBoundsOk(const char* where, Int_t at);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   Bool_t %sOutOfBoundsError(const char* where, Int_t i);\n",valueName[iFold][i].Data());
            }
            else if (valueArray[iFold][i][0]=="variable") {
               buffer.AppendFormatted("   Bool_t %sBoundsOk(const char* where, Int_t at);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   Bool_t %sOutOfBoundsError(const char* where, Int_t i);\n",valueName[iFold][i].Data());
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
            }
            else {
               buffer.AppendFormatted("   Bool_t %sBoundsOk(const char* where, ",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted("Int_t %c, ",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-2);
               buffer.AppendFormatted(");\n");
               buffer.AppendFormatted("   Bool_t %sOutOfBoundsError(const char* where,",valueName[iFold][i].Data());
               for (iDm=0;iDm<valueDimension[iFold][i];iDm++)
                  buffer.AppendFormatted(" Int_t %c,",valueCounter[iDm]);
               buffer.Resize(buffer.Length()-1);
               buffer.AppendFormatted(");\n");
            }
         }
      }
      buffer.AppendFormatted("\n");

      // Footer
      if (folderUserCode[iFold]) {
         buffer.AppendFormatted("\n   ClassDef(%s%s_Base,%s) // Base class of %s%s\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
      }
      else {
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
         if (folderShortDescription[iFold].Length())
            buffer.AppendFormatted(" // %s", folderShortDescription[iFold].Data());
         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("};\n\n");
/*
      buffer.AppendFormatted("#if !defined(__CINT__)\n");
      if (folderUserCode[iFold])
         buffer.AppendFormatted("ClassImp(%s%s_Base)\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("ClassImp(%s%s)\n",shortCut.Data(),folderName[iFold].Data());
      buffer.AppendFormatted("#endif\n");
*/
      if (folderUserCode[iFold])
         buffer.AppendFormatted("#endif   // %s%s_Base_H\n",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),folderName[iFold].Data());

      // Write File
      WriteFile(hFile.Data(),buffer.Data(),6);

      // User H-File
      hFile.SetFormatted("%sinclude/folders/%s%s.h",outDir.Data(),shortCut.Data(),folderName[iFold].Data());
      if (folderUserCode[iFold] && (changeableFlagChanged || gSystem->AccessPathName(hFile.Data(),kFileExists))) {
         buffer.Resize(0);

         // Description
         WriteHeader(buffer, folderAuthor[iFold].Data(), kFALSE);
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),folderName[iFold].Data());
         clsName.SetFormatted("%s%s", shortCut.Data(), folderName[iFold].Data());
         WriteDescription(buffer, clsName.Data(), 0, kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"generated/%s%s_Base.h\"\n",shortCut.Data(),folderName[iFold].Data());

         // Class
         buffer.AppendFormatted("\nclass %s%s : public %s%s_Base\n",shortCut.Data(),folderName[iFold].Data(),shortCut.Data(),folderName[iFold].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("%s %s=%s,",valueType[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) : %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
         for (i=0;i<numOfValue[iFold];i++) {
            if (valueDimension[iFold][i]==0)
               buffer.AppendFormatted("%s,",valueName[iFold][i].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" ) {}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
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

Bool_t ROMEBuilder::WriteAllFoldersH() {
   ROMEString hFile;
   ROMEString buffer;
   Int_t i;

   buffer.Resize(0);
   hFile.SetFormatted("%sinclude/generated/%sAllFolders.h", outDir.Data(), shortCut.Data());
   WriteHeader(buffer, mainAuthor, true);

   buffer.AppendFormatted("#ifndef %sAllFolders_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sAllFolders_H\n\n",shortCut.Data());
   // Folder includes
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderUserCode[i])
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         else
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");
   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);
   return kTRUE;
}

Bool_t ROMEBuilder::WriteTaskCpp()
{
   ROMEString cppFile;
   ROMEString header;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString fileBuffer;
   ROMEString tmp, tmp2;

   int i,j;
   ROMEString format;
   ROMEString discript;
   ROMEString str;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (!taskUsed[iTask])
         continue;
      header.Resize(0);
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      WriteHeader(header, taskAuthor[iTask].Data(), kFALSE);
      clsName.SetFormatted("%sT%s", shortCut.Data(), taskName[iTask].Data());
      clsDescription = "Begin_Html\n\n";
      clsDescription.AppendFormatted("Description:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",taskDescription[iTask].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("Usage:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",taskUsage[iTask].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("Status:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",taskStatus[iTask].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("To Do:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",taskToDo[iTask].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("Known Problems:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",taskKnownProblems[iTask].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("End_Html\n\n");
      clsDescription.AppendFormatted("The event methods have been written by %s.\n",taskAuthor[iTask].Data());
      fstream *fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      bool first = true;
      for (j=0;j<numOfFolder;j++) {
         if (accessFolder(fileBuffer,j)) {
            if (first) {
               clsDescription.AppendFormatted("\n");
               clsDescription.AppendFormatted("Please note: The following information is only correct after executing the ROMEBuilder.\n");
               clsDescription.AppendFormatted("\n");
               clsDescription.AppendFormatted("This task accesses the following folders :\n");
               first = false;
            }
            if (!folderUsed[j]) {
/*                  cout << "Error : Task " << taskName[iTask].Data() << " tries to access folder " << folderName[j].Data() << endl;
               cout << "        but this folder is currently not linked into the project." << endl << endl;
               cout << "        Please check the affiliations of the task and the folder," << endl;
               cout << "        or modify the source code of the task." << endl;
               cout << "Terminating program." << endl;
               return false;
*/                  clsDescription.AppendFormatted("    %s\n",folderName[j].Data());
            }
         }
      }
      clsDescription.AppendFormatted("\n");
      if (numOfHistos[iTask]>0 || numOfGraphs[iTask]>0) {
         if (first) {
            clsDescription.AppendFormatted("Please note: The following information is only correct after executing the ROMEBuilder.\n");
            clsDescription.AppendFormatted("              \n");
            clsDescription.AppendFormatted("\n");
            first = false;
         }
         if (numOfHistos[iTask]>0) {
            clsDescription.AppendFormatted("This task contains the following histgrams :\n");
            for (i=0;i<numOfHistos[iTask];i++) {
               clsDescription.AppendFormatted("   %s\n",histoName[iTask][i].Data());
            }
         }
         if (numOfGraphs[iTask]>0) {
            clsDescription.AppendFormatted("This task contains the following graphs :\n");
            for (i=0;i<numOfGraphs[iTask];i++) {
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
      bool folderIncludeFirst = true;
      numOfTaskAccessedFolder[iTask] = 0;
      for (j=0;j<numOfFolder;j++) {
         if (accessFolder(fileBuffer,j)) {
            if (!folderUsed[j])
               continue;
            if (numOfValue[j] > 0 && !folderSupport[j]) {
               if (folderIncludeFirst) {
                  folderIncludeFirst = false;
                  clsDescription.AppendFormatted("\n");
                  clsDescription.AppendFormatted("Followings are include files of folders. ROMEBuilder will update it with reading this source code when it is executed next time.\n");
               }
               taskAccessedFolder[iTask][numOfTaskAccessedFolder[iTask]] = j;
               if (folderUserCode[j])
                  clsDescription.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               else
                  clsDescription.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               numOfTaskAccessedFolder[iTask]++;
            }
         }
      }
      if (fileBuffer.Contains("GetWindow"))
         clsDescription.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
      if (fileBuffer.Contains("GetGSP"))
         clsDescription.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
      for (j=0;j<daqNameArray->GetEntriesFast();j++) {
         tmp.SetFormatted("Get%sDAQ()",daqNameArray->At(j).Data());
         tmp2.SetFormatted("Get%s()",daqNameArray->At(j).Data());
         if (fileBuffer.Contains(tmp) || fileBuffer.Contains(tmp2))
            clsDescription.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(j).Data(),shortCut.Data(),daqNameArray->At(j).Data());
      }

      WriteDescription(header, clsName.Data(), clsDescription.Data(), kTRUE);

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
      buffer.AppendFormatted("void %sT%s::Init()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("void %sT%s::Event()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n}\n\n",shortCut.Data(),taskName[iTask].Data());
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

Bool_t ROMEBuilder::WriteBaseTaskCpp()
{
   int i,j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString fileBuffer;
   ROMEString tmp;

   ROMEString format;
   ROMEString discript;
   ROMEString str;
   bool array;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (!taskUsed[iTask])
         continue;
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/generated/%sT%s_Base.cpp",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      WriteHeader(buffer, taskAuthor[iTask].Data(), kTRUE);
      clsName.SetFormatted("%sT%s_Base", shortCut.Data(), taskName[iTask].Data());
      clsDescription = taskDescription[iTask].Data();
      clsDescription.AppendFormatted("\n\n");
      fstream *fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;

      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kTRUE);

      // Header
      buffer.AppendFormatted("\n\n");

      if ((numOfHistos[iTask]>0 || numOfGraphs[iTask]>0) && readGlobalSteeringParameters)
         buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
      buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("\nClassImp(%sT%s_Base)\n\n",shortCut.Data(),taskName[iTask].Data());

      // Constructor
      if (numOfHistos[iTask]>0 || numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder):ROMETask(name,title,level) {\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      }
      else {
         buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(const char *name,const char *title,int level,const char * /* histoSuffix */,TFolder *histoFolder):ROMETask(name,title,level) {\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      }
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("   fHistoSuffix = histoSuffix;\n");
      }
      if (numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("   fGraphSuffix = histoSuffix;\n");
      }
      buffer.AppendFormatted("   fHistoFolder = histoFolder;\n");
      buffer.AppendFormatted("   fEventID = %s;\n",taskEventID[iTask].Data());
      buffer.AppendFormatted("   fVersion = %s;\n",taskVersion[iTask].Data());
      if (numOfHistos[iTask]>0)
         buffer.AppendFormatted("   fHasHistograms = true;\n");
      else
         buffer.AppendFormatted("   fHasHistograms = false;\n");
      if (numOfGraphs[iTask]>0)
         buffer.AppendFormatted("   fHasGraphs = true;\n");
      else
         buffer.AppendFormatted("   fHasGraphs = false;\n");
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   f%s = 0;\n",histoName[iTask][i].Data());
         } else {
            buffer.AppendFormatted("   f%ss = 0;\n",histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("   f%sHisto = new ROMEHisto();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sHisto->SetOriginal(\"%s\",\"%s\",%s,%s,\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),
            histoTitle[iTask][i].Data(),histoFolderTitle[iTask][i].Data(),histoArraySize[iTask][i].Data(),histoArrayStartIndex[iTask][i].Data(),
            histoXLabel[iTask][i].Data(),histoYLabel[iTask][i].Data(),histoZLabel[iTask][i].Data(),
            histoXNbins[iTask][i].Data(),histoXmin[iTask][i].Data(),histoXmax[iTask][i].Data(),
            histoYNbins[iTask][i].Data(),histoYmin[iTask][i].Data(),histoYmax[iTask][i].Data(),
            histoZNbins[iTask][i].Data(),histoZmin[iTask][i].Data(),histoZmax[iTask][i].Data());
      }
      for (i=0;i<numOfGraphs[iTask];i++) {
         if (graphArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   f%s = 0;\n",graphName[iTask][i].Data());
         } else {
            buffer.AppendFormatted("   f%ss = 0;\n",graphName[iTask][i].Data());
         }
         buffer.AppendFormatted("   f%sGraph = new ROMEGraph();\n",graphName[iTask][i].Data());
         buffer.AppendFormatted("   f%sGraph->SetOriginal(\"%s\",\"%s\",%s,%s,\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%s,%s);\n",graphName[iTask][i].Data(),
            graphTitle[iTask][i].Data(),graphFolderTitle[iTask][i].Data(),graphArraySize[iTask][i].Data(),graphArrayStartIndex[iTask][i].Data(),
            graphXLabel[iTask][i].Data(),graphYLabel[iTask][i].Data(),graphZLabel[iTask][i].Data(),
            graphXmin[iTask][i].Data(),graphXmax[iTask][i].Data(),
            graphYmin[iTask][i].Data(),graphYmax[iTask][i].Data(),
            graphZmin[iTask][i].Data(),graphZmax[iTask][i].Data());
         if (graphArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   f%sGraphStorage = new %s();\n",graphName[iTask][i].Data(),graphType[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   f%sGraphsStorage = new TObjArray();\n",graphName[iTask][i].Data());
         }
      }
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   fSteering = new Steering();\n");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Book Histo
      buffer.AppendFormatted("void %sT%s_Base::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      if (numOfHistos[iTask]>0) {
         // create histo folders
         for (i=0;i<numOfHistos[iTask];i++) {
            if (histoFolderName[iTask][i]=="")
               continue;
            bool alreadyDefined = false;
            for (j=0;j<i;j++) {
               if (histoFolderName[iTask][i]==histoFolderName[iTask][j])
                  alreadyDefined = true;
            }
            if (!alreadyDefined)
               buffer.AppendFormatted("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"folder to store %s histos/graphs\");\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data());
         }
         array = false;
         for (i=0;i<numOfHistos[iTask];i++) {
            if (histoArraySize[iTask][i]!="1") array = true;
         }
         if (array) {
            buffer.AppendFormatted("   int j;\n");
            buffer.AppendFormatted("   ROMEString name;\n");
            buffer.AppendFormatted("   ROMEString title;\n");
         }
         if (numOfHistos[iTask]>0) {
            buffer.AppendFormatted("   ROMEHisto* histoHandle;\n");
            buffer.AppendFormatted("   ROMEString histoName;\n");
            buffer.AppendFormatted("   ROMEString histoTitle;\n");
            buffer.AppendFormatted("   ROMEString folderTitle;\n");
            buffer.AppendFormatted("   ROMEString xLabel;\n");
            buffer.AppendFormatted("   ROMEString yLabel;\n");
            buffer.AppendFormatted("   ROMEString zLabel;\n");
            buffer.AppendFormatted("   int arraySize;\n");
            buffer.AppendFormatted("   ROMEString arraySizeStr;\n");
            buffer.AppendFormatted("   int arrayStartIndex;\n");
            buffer.AppendFormatted("   ROMEString arrayStartIndexStr;\n");
            buffer.AppendFormatted("   int xNbins;\n");
            buffer.AppendFormatted("   ROMEString xNbinsStr;\n");
            buffer.AppendFormatted("   double xmin;\n");
            buffer.AppendFormatted("   ROMEString xminStr;\n");
            buffer.AppendFormatted("   double xmax;\n");
            buffer.AppendFormatted("   ROMEString xmaxStr;\n");
            buffer.AppendFormatted("   int yNbins;\n");
            buffer.AppendFormatted("   ROMEString yNbinsStr;\n");
            buffer.AppendFormatted("   double ymin;\n");
            buffer.AppendFormatted("   ROMEString yminStr;\n");
            buffer.AppendFormatted("   double ymax;\n");
            buffer.AppendFormatted("   ROMEString ymaxStr;\n");
            buffer.AppendFormatted("   int zNbins;\n");
            buffer.AppendFormatted("   ROMEString zNbinsStr;\n");
            buffer.AppendFormatted("   double zmin;\n");
            buffer.AppendFormatted("   ROMEString zminStr;\n");
            buffer.AppendFormatted("   double zmax;\n");
            buffer.AppendFormatted("   ROMEString zmaxStr;\n");
         }
         for (i=0;i<numOfHistos[iTask];i++) {
            buffer.AppendFormatted("   if (f%sHisto->IsActive()) {\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      f%sHisto->SetOriginal(\"%s\",\"%s\",%s,%s,\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),
               histoTitle[iTask][i].Data(),histoFolderTitle[iTask][i].Data(),histoArraySize[iTask][i].Data(),histoArrayStartIndex[iTask][i].Data(),
               histoXLabel[iTask][i].Data(),histoYLabel[iTask][i].Data(),histoZLabel[iTask][i].Data(),
               histoXNbins[iTask][i].Data(),histoXmin[iTask][i].Data(),histoXmax[iTask][i].Data(),
               histoYNbins[iTask][i].Data(),histoYmin[iTask][i].Data(),histoYmax[iTask][i].Data(),
               histoZNbins[iTask][i].Data(),histoZmin[iTask][i].Data(),histoZmax[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
         for (i=0;i<numOfHistos[iTask];i++) {
            bool homeFolder = false;
            if (histoFolderName[iTask][i]=="") {
               homeFolder = true;
            }
            buffer.AppendFormatted("   if (f%sHisto->IsActive()) {\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      histoHandle = Get%sHisto();\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      histoTitle = histoHandle->GetTitle();\n");
            buffer.AppendFormatted("      folderTitle = histoHandle->GetFolderTitle();\n");
            buffer.AppendFormatted("      xLabel = histoHandle->GetXLabel();\n");
            buffer.AppendFormatted("      yLabel = histoHandle->GetYLabel();\n");
            buffer.AppendFormatted("      zLabel = histoHandle->GetZLabel();\n");
            buffer.AppendFormatted("      arraySize = histoHandle->GetArraySize();\n");
            buffer.AppendFormatted("      arraySizeStr = histoHandle->GetArraySizeString(arraySizeStr);\n");
            buffer.AppendFormatted("      arrayStartIndex = histoHandle->GetArrayStartIndex();\n");
            buffer.AppendFormatted("      arrayStartIndexStr = histoHandle->GetArrayStartIndexString(arrayStartIndexStr);\n");
            buffer.AppendFormatted("      xNbins = histoHandle->GetXNbins();\n");
            buffer.AppendFormatted("      xNbinsStr = histoHandle->GetXNbinsString(xNbinsStr);\n");
            buffer.AppendFormatted("      xmin = histoHandle->GetXmin();\n");
            buffer.AppendFormatted("      xminStr = histoHandle->GetXminString(xminStr);\n");
            buffer.AppendFormatted("      xmax = histoHandle->GetXmax();\n");
            buffer.AppendFormatted("      xmaxStr = histoHandle->GetXmaxString(xmaxStr);\n");
            buffer.AppendFormatted("      yNbins = histoHandle->GetYNbins();\n");
            buffer.AppendFormatted("      yNbinsStr = histoHandle->GetYNbinsString(yNbinsStr);\n");
            buffer.AppendFormatted("      ymin = histoHandle->GetYmin();\n");
            buffer.AppendFormatted("      yminStr = histoHandle->GetYminString(yminStr);\n");
            buffer.AppendFormatted("      ymax = histoHandle->GetYmax();\n");
            buffer.AppendFormatted("      ymaxStr = histoHandle->GetYmaxString(ymaxStr);\n");
            buffer.AppendFormatted("      zNbins = histoHandle->GetZNbins();\n");
            buffer.AppendFormatted("      zNbinsStr = histoHandle->GetZNbinsString(zNbinsStr);\n");
            buffer.AppendFormatted("      zmin = histoHandle->GetZmin();\n");
            buffer.AppendFormatted("      zminStr = histoHandle->GetZminString(zminStr);\n");
            buffer.AppendFormatted("      zmax  = histoHandle->GetZmax();\n");
            buffer.AppendFormatted("      zmaxStr  = histoHandle->GetZmaxString(zmaxStr);\n");

            buffer.AppendFormatted("      histoTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(histoTitle),histoTitle,histoTitle);\n");
            buffer.AppendFormatted("      folderTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(folderTitle),folderTitle,folderTitle);\n");
            buffer.AppendFormatted("      xLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(xLabel),xLabel,xLabel);\n");
            buffer.AppendFormatted("      yLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(yLabel),yLabel,yLabel);\n");
            buffer.AppendFormatted("      zLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(zLabel),zLabel,zLabel);\n");
            buffer.AppendFormatted("      arraySize = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arraySizeStr),arraySize);\n");
            buffer.AppendFormatted("      arrayStartIndex = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arrayStartIndexStr),arrayStartIndex);\n");
            buffer.AppendFormatted("      xNbins = GetObjectInterpreterIntValue(GetObjectInterpreterCode(xNbinsStr),xNbins);\n");
            buffer.AppendFormatted("      xmin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xminStr),xmin );\n");
            buffer.AppendFormatted("      xmax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xmaxStr),xmax );\n");
            buffer.AppendFormatted("      yNbins = GetObjectInterpreterIntValue(GetObjectInterpreterCode(yNbinsStr),yNbins);\n");
            buffer.AppendFormatted("      ymin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yminStr),ymin );\n");
            buffer.AppendFormatted("      ymax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(ymaxStr),ymax);\n");
            buffer.AppendFormatted("      zNbins = GetObjectInterpreterIntValue(GetObjectInterpreterCode(zNbinsStr),zNbins);\n");
            buffer.AppendFormatted("      zmin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zminStr),zmin);\n");
            buffer.AppendFormatted("      zmax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zmaxStr),zmax);\n");

            // create histos
            buffer.AppendFormatted("      histoName = \"%s\";\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      histoName+=fHistoSuffix;\n");
            if (histoArraySize[iTask][i]=="1") {
               if (histoType[iTask][i][2]==49) {
                  buffer.AppendFormatted("      f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
               }
               if (histoType[iTask][i][2]==50) {
                  buffer.AppendFormatted("      f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
               }
               if (histoType[iTask][i][2]==51) {
                  buffer.AppendFormatted("      f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
               }
               if (histoType[iTask][i]=="TProfile") {
                  buffer.AppendFormatted("      f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,ymin,ymax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
               }
               if (histoType[iTask][i]=="TProfile2D") {
                  buffer.AppendFormatted("      f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zmin,zmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
               }
               if (!homeFolder)
                  buffer.AppendFormatted("      %sFolder->Add(f%s);\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
               else
                  buffer.AppendFormatted("      GetHistoFolder()->Add(f%s);\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      f%s->GetXaxis()->SetTitle(xLabel.Data());\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      f%s->GetYaxis()->SetTitle(yLabel.Data());\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      f%s->GetZaxis()->SetTitle(zLabel.Data());\n",histoName[iTask][i].Data());
            }
            else {
               buffer.AppendFormatted("      %s *hist%d;\n",histoType[iTask][i].Data(),i);
               buffer.AppendFormatted("      f%ss = new TObjArray(arraySize);\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      for (j=0;j<arraySize;j++) {\n");
               buffer.AppendFormatted("         name.SetFormatted(\"_%%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         name.Insert(0,histoName.Data());\n");
               buffer.AppendFormatted("         title.SetFormatted(\" %%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         title.Insert(0,histoTitle.Data());\n");
               if (histoType[iTask][i][2]==49) {
                  buffer.AppendFormatted("         hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax);\n",i,histoType[iTask][i].Data());
               }
               if (histoType[iTask][i][2]==50) {
                  buffer.AppendFormatted("         hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",i,histoType[iTask][i].Data());
               }
               if (histoType[iTask][i][2]==51) {
                  buffer.AppendFormatted("         hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",i,histoType[iTask][i].Data());
               }
               buffer.AppendFormatted("         f%ss->Add(hist%d);\n",histoName[iTask][i].Data(),i);
               if (!homeFolder)
                  buffer.AppendFormatted("         %sFolder->Add(f%ss->At(j));\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
               else
                  buffer.AppendFormatted("         GetHistoFolder()->Add(f%ss->At(j));\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("         hist%d->GetXaxis()->SetTitle(xLabel.Data());\n",i);
               buffer.AppendFormatted("         hist%d->GetYaxis()->SetTitle(yLabel.Data());\n",i);
               buffer.AppendFormatted("         hist%d->GetZaxis()->SetTitle(zLabel.Data());\n",i);
               buffer.AppendFormatted("      }\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // Book Graph
      buffer.AppendFormatted("void %sT%s_Base::BookGraph() {\n",shortCut.Data(),taskName[iTask].Data());
      if (numOfGraphs[iTask]>0) {
         // create graph folders
         for (i=0;i<numOfGraphs[iTask];i++) {
            if (graphFolderName[iTask][i]=="")
               continue;
            bool alreadyDefined = false;
            for (j=0;j<i;j++) {
               if (graphFolderName[iTask][i]==graphFolderName[iTask][j])
                  alreadyDefined = true;
            }
            if (!alreadyDefined) {
               for (j=0;j<numOfHistos[iTask];j++) {
                  if (graphFolderName[iTask][i]==histoFolderName[iTask][j])
                     alreadyDefined = true;
               }
               if (alreadyDefined) {
                  buffer.AppendFormatted("      TFolder *%sFolder;\n",graphFolderName[iTask][i].Data());
                  buffer.AppendFormatted("      %sFolder = static_cast<TFolder*>(GetHistoFolder()->FindObject(\"%s\"));\n",graphFolderName[iTask][i].Data(),graphFolderName[iTask][i].Data());
               }
               else
                  buffer.AppendFormatted("      TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",\"folder to store %s histos/graphs\");\n",graphFolderName[iTask][i].Data(),graphFolderName[iTask][i].Data(),graphFolderName[iTask][i].Data());
            }
         }
         array = false;
         for (i=0;i<numOfGraphs[iTask];i++) {
            if (graphArraySize[iTask][i]!="1") array = true;
         }
         if (array) {
            buffer.AppendFormatted("   int j;\n");
            buffer.AppendFormatted("   ROMEString name;\n");
            buffer.AppendFormatted("   ROMEString title;\n");
         }
         if (numOfGraphs[iTask]>0) {
            buffer.AppendFormatted("   ROMEGraph* graphHandle;\n");
            buffer.AppendFormatted("   ROMEString graphName;\n");
            buffer.AppendFormatted("   ROMEString graphTitle;\n");
            buffer.AppendFormatted("   ROMEString folderTitle;\n");
            buffer.AppendFormatted("   ROMEString xLabel;\n");
            buffer.AppendFormatted("   ROMEString yLabel;\n");
            buffer.AppendFormatted("   ROMEString zLabel;\n");
            buffer.AppendFormatted("   int arraySize;\n");
            buffer.AppendFormatted("   ROMEString arraySizeStr;\n");
            buffer.AppendFormatted("   int arrayStartIndex;\n");
            buffer.AppendFormatted("   ROMEString arrayStartIndexStr;\n");
            buffer.AppendFormatted("   double xMin;\n");
            buffer.AppendFormatted("   ROMEString xMinStr;\n");
            buffer.AppendFormatted("   double xMax;\n");
            buffer.AppendFormatted("   ROMEString xMaxStr;\n");
            buffer.AppendFormatted("   double yMin;\n");
            buffer.AppendFormatted("   ROMEString yMinStr;\n");
            buffer.AppendFormatted("   double yMax;\n");
            buffer.AppendFormatted("   ROMEString yMaxStr;\n");
            buffer.AppendFormatted("   double zMin;\n");
            buffer.AppendFormatted("   ROMEString zMinStr;\n");
            buffer.AppendFormatted("   double zMax;\n");
            buffer.AppendFormatted("   ROMEString zMaxStr;\n");
         }
         for (i=0;i<numOfGraphs[iTask];i++) {
            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      f%sGraph->SetOriginal(\"%s\",\"%s\",%s,%s,\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%s,%s);\n",graphName[iTask][i].Data(),
               graphTitle[iTask][i].Data(),graphFolderTitle[iTask][i].Data(),graphArraySize[iTask][i].Data(),graphArrayStartIndex[iTask][i].Data(),
               graphXLabel[iTask][i].Data(),graphYLabel[iTask][i].Data(),graphZLabel[iTask][i].Data(),
               graphXmin[iTask][i].Data(),graphXmax[iTask][i].Data(),
               graphYmin[iTask][i].Data(),graphYmax[iTask][i].Data(),
               graphZmin[iTask][i].Data(),graphZmax[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
         for (i=0;i<numOfGraphs[iTask];i++) {
            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            bool homeFolder = false;
            if (graphFolderName[iTask][i]=="") {
               homeFolder = true;
            }

            buffer.AppendFormatted("      graphHandle = Get%sGraph();\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      graphTitle = graphHandle->GetTitle();\n");
            buffer.AppendFormatted("      folderTitle = graphHandle->GetFolderTitle();\n");
            buffer.AppendFormatted("      xLabel = graphHandle->GetXLabel();\n");
            buffer.AppendFormatted("      yLabel = graphHandle->GetYLabel();\n");
            buffer.AppendFormatted("      zLabel = graphHandle->GetZLabel();\n");
            buffer.AppendFormatted("      arraySize = graphHandle->GetArraySize();\n");
            buffer.AppendFormatted("      arraySizeStr = graphHandle->GetArraySizeString(arraySizeStr);\n");
            buffer.AppendFormatted("      arrayStartIndex = graphHandle->GetArrayStartIndex();\n");
            buffer.AppendFormatted("      arrayStartIndexStr = graphHandle->GetArrayStartIndexString(arrayStartIndexStr);\n");
            buffer.AppendFormatted("      xMin = graphHandle->GetXmin();\n");
            buffer.AppendFormatted("      xMinStr = graphHandle->GetXminString(xMinStr);\n");
            buffer.AppendFormatted("      xMax = graphHandle->GetXmax();\n");
            buffer.AppendFormatted("      xMaxStr = graphHandle->GetXmaxString(xMaxStr);\n");
            buffer.AppendFormatted("      yMin = graphHandle->GetYmin();\n");
            buffer.AppendFormatted("      yMinStr = graphHandle->GetYminString(yMinStr);\n");
            buffer.AppendFormatted("      yMax = graphHandle->GetYmax();\n");
            buffer.AppendFormatted("      yMaxStr = graphHandle->GetYmaxString(yMaxStr);\n");
            buffer.AppendFormatted("      zMin = graphHandle->GetZmin();\n");
            buffer.AppendFormatted("      zMinStr = graphHandle->GetZminString(zMinStr);\n");
            buffer.AppendFormatted("      zMax = graphHandle->GetZmax();\n");
            buffer.AppendFormatted("      zMaxStr = graphHandle->GetZmaxString(zMaxStr);\n");

            buffer.AppendFormatted("      graphTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(graphTitle),graphTitle,graphTitle);\n");
            buffer.AppendFormatted("      folderTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(folderTitle),folderTitle,folderTitle);\n");
            buffer.AppendFormatted("      xLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(xLabel),xLabel,xLabel);\n");
            buffer.AppendFormatted("      yLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(yLabel),yLabel,yLabel);\n");
            buffer.AppendFormatted("      zLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(zLabel),zLabel,zLabel);\n");
            buffer.AppendFormatted("      arraySize = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arraySizeStr),arraySize);\n");
            buffer.AppendFormatted("      arrayStartIndex = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arrayStartIndexStr),arrayStartIndex);\n");
            buffer.AppendFormatted("      xMin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xMinStr),xMin);\n");
            buffer.AppendFormatted("      xMax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xMaxStr),xMax);\n");
            buffer.AppendFormatted("      yMin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yMinStr),yMin);\n");
            buffer.AppendFormatted("      yMax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yMaxStr),yMax);\n");
            buffer.AppendFormatted("      zMin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zMinStr),zMin);\n");
            buffer.AppendFormatted("      zMax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zMaxStr),zMax);\n");

            // create graphs
            buffer.AppendFormatted("      graphName = \"%s\";\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      graphName+=fGraphSuffix;\n");
            if (graphArraySize[iTask][i]=="1") {
               buffer.AppendFormatted("      f%s = new %s(1);\n",graphName[iTask][i].Data(),graphType[iTask][i].Data());
               buffer.AppendFormatted("      f%s->SetNameTitle(\"%s\",\"%s\");\n",graphName[iTask][i].Data(),graphName[iTask][i].Data(),graphTitle[iTask][i].Data());
               if (!homeFolder)
                  buffer.AppendFormatted("      %sFolder->Add(f%s);\n",graphFolderName[iTask][i].Data(),graphName[iTask][i].Data());
               else
                  buffer.AppendFormatted("      GetHistoFolder()->Add(f%s);\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      f%s->GetXaxis()->SetTitle(xLabel.Data());\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      f%s->GetYaxis()->SetTitle(yLabel.Data());\n",graphName[iTask][i].Data());
            }
            else {
               buffer.AppendFormatted("      %s *graph%d;\n",graphType[iTask][i].Data(),i);
               buffer.AppendFormatted("      f%ss = new TObjArray(arraySize);\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      for (j=0;j<arraySize;j++) {\n");
               buffer.AppendFormatted("         name.SetFormatted(\"_%%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         name.Insert(0,graphName.Data());\n");
               buffer.AppendFormatted("         title.SetFormatted(\" %%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         title.Insert(0,graphTitle.Data());\n");
               buffer.AppendFormatted("         graph%d = new %s(1);\n",i,graphType[iTask][i].Data());
               buffer.AppendFormatted("         graph%d->SetNameTitle(\"%s\",\"%s\");\n",i,graphName[iTask][i].Data(),graphTitle[iTask][i].Data());
               buffer.AppendFormatted("         f%ss->Add(graph%d);\n",graphName[iTask][i].Data(),i);
               if (!homeFolder)
                  buffer.AppendFormatted("         %sFolder->Add(f%ss->At(j));\n",graphFolderName[iTask][i].Data(),graphName[iTask][i].Data());
               else
                  buffer.AppendFormatted("         GetHistoFolder()->Add(f%ss->At(j));\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("         graph%d->GetXaxis()->SetTitle(xLabel.Data());\n",i);
               buffer.AppendFormatted("         graph%d->GetYaxis()->SetTitle(yLabel.Data());\n",i);
               buffer.AppendFormatted("      }\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // ReBook Histo
      buffer.AppendFormatted("void %sT%s_Base::ReBookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      if (numOfHistos[iTask]>0) {
         // get histo folders
         for (i=0;i<numOfHistos[iTask];i++) {
            if (histoFolderName[iTask][i]=="")
               continue;
            bool alreadyDefined = false;
            for (j=0;j<i;j++) {
               if (histoFolderName[iTask][i]==histoFolderName[iTask][j])
                  alreadyDefined = true;
            }
            if (!alreadyDefined) {
               buffer.AppendFormatted("   TFolder *%sFolder;\n",histoFolderName[iTask][i].Data());
               buffer.AppendFormatted("   %sFolder = static_cast<TFolder*>(GetHistoFolder()->FindObject(\"%s\"));\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data());
            }
         }
         array = false;
         for (i=0;i<numOfHistos[iTask];i++) {
            if (histoArraySize[iTask][i]!="1") array = true;
         }
         if (array) {
            buffer.AppendFormatted("   int j;\n");
            buffer.AppendFormatted("   int arraySizeOld;\n");
            buffer.AppendFormatted("   ROMEString name;\n");
            buffer.AppendFormatted("   ROMEString title;\n");
         }
         if (numOfHistos[iTask]>0) {
            buffer.AppendFormatted("   ROMEHisto* histoHandle;\n");
            buffer.AppendFormatted("   ROMEString histoName;\n");
            buffer.AppendFormatted("   ROMEString histoTitle;\n");
            buffer.AppendFormatted("   ROMEString folderTitle;\n");
            buffer.AppendFormatted("   ROMEString xLabel;\n");
            buffer.AppendFormatted("   ROMEString yLabel;\n");
            buffer.AppendFormatted("   ROMEString zLabel;\n");
            buffer.AppendFormatted("   int arraySize;\n");
            buffer.AppendFormatted("   ROMEString arraySizeStr;\n");
            buffer.AppendFormatted("   int arrayStartIndex;\n");
            buffer.AppendFormatted("   ROMEString arrayStartIndexStr;\n");
            buffer.AppendFormatted("   int xNbins;\n");
            buffer.AppendFormatted("   ROMEString xNbinsStr;\n");
            buffer.AppendFormatted("   double xmin;\n");
            buffer.AppendFormatted("   ROMEString xminStr;\n");
            buffer.AppendFormatted("   double xmax;\n");
            buffer.AppendFormatted("   ROMEString xmaxStr;\n");
            buffer.AppendFormatted("   int yNbins;\n");
            buffer.AppendFormatted("   ROMEString yNbinsStr;\n");
            buffer.AppendFormatted("   double ymin;\n");
            buffer.AppendFormatted("   ROMEString yminStr;\n");
            buffer.AppendFormatted("   double ymax;\n");
            buffer.AppendFormatted("   ROMEString ymaxStr;\n");
            buffer.AppendFormatted("   int zNbins;\n");
            buffer.AppendFormatted("   ROMEString zNbinsStr;\n");
            buffer.AppendFormatted("   double zmin;\n");
            buffer.AppendFormatted("   ROMEString zminStr;\n");
            buffer.AppendFormatted("   double zmax;\n");
            buffer.AppendFormatted("   ROMEString zmaxStr;\n");
         }
         for (i=0;i<numOfHistos[iTask];i++) {
            buffer.AppendFormatted("   if (f%sHisto->IsActive() && !f%sHisto->isAccumulation()) {\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("      f%sHisto->SetOriginal(\"%s\",\"%s\",%s,%s,\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%s,%s,%s,%s,%s);\n",histoName[iTask][i].Data(),
               histoTitle[iTask][i].Data(),histoFolderTitle[iTask][i].Data(),histoArraySize[iTask][i].Data(),histoArrayStartIndex[iTask][i].Data(),
               histoXLabel[iTask][i].Data(),histoYLabel[iTask][i].Data(),histoZLabel[iTask][i].Data(),
               histoXNbins[iTask][i].Data(),histoXmin[iTask][i].Data(),histoXmax[iTask][i].Data(),
               histoYNbins[iTask][i].Data(),histoYmin[iTask][i].Data(),histoYmax[iTask][i].Data(),
               histoZNbins[iTask][i].Data(),histoZmin[iTask][i].Data(),histoZmax[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
         for (i=0;i<numOfHistos[iTask];i++) {
            bool homeFolder = false;
            if (histoFolderName[iTask][i]=="") {
               homeFolder = true;
            }
            buffer.AppendFormatted("   if (f%sHisto->IsActive() && !f%sHisto->isAccumulation()) {\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("      histoHandle = Get%sHisto();\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      histoTitle = histoHandle->GetTitle();\n");
            buffer.AppendFormatted("      folderTitle = histoHandle->GetFolderTitle();\n");
            buffer.AppendFormatted("      xLabel = histoHandle->GetXLabel();\n");
            buffer.AppendFormatted("      yLabel = histoHandle->GetYLabel();\n");
            buffer.AppendFormatted("      zLabel = histoHandle->GetZLabel();\n");
            buffer.AppendFormatted("      arraySize = histoHandle->GetArraySize();\n");
            buffer.AppendFormatted("      arraySizeStr = histoHandle->GetArraySizeString(arraySizeStr);\n");
            buffer.AppendFormatted("      arrayStartIndex = histoHandle->GetArrayStartIndex();\n");
            buffer.AppendFormatted("      arrayStartIndexStr = histoHandle->GetArrayStartIndexString(arrayStartIndexStr);\n");
            buffer.AppendFormatted("      xNbins = histoHandle->GetXNbins();\n");
            buffer.AppendFormatted("      xNbinsStr = histoHandle->GetXNbinsString(xNbinsStr);\n");
            buffer.AppendFormatted("      xmin = histoHandle->GetXmin();\n");
            buffer.AppendFormatted("      xminStr = histoHandle->GetXminString(xminStr);\n");
            buffer.AppendFormatted("      xmax = histoHandle->GetXmax();\n");
            buffer.AppendFormatted("      xmaxStr = histoHandle->GetXmaxString(xmaxStr);\n");
            buffer.AppendFormatted("      yNbins = histoHandle->GetYNbins();\n");
            buffer.AppendFormatted("      yNbinsStr = histoHandle->GetYNbinsString(yNbinsStr);\n");
            buffer.AppendFormatted("      ymin = histoHandle->GetYmin();\n");
            buffer.AppendFormatted("      yminStr = histoHandle->GetYminString(yminStr);\n");
            buffer.AppendFormatted("      ymax = histoHandle->GetYmax();\n");
            buffer.AppendFormatted("      ymaxStr = histoHandle->GetYmaxString(ymaxStr);\n");
            buffer.AppendFormatted("      zNbins = histoHandle->GetZNbins();\n");
            buffer.AppendFormatted("      zNbinsStr = histoHandle->GetZNbinsString(zNbinsStr);\n");
            buffer.AppendFormatted("      zmin = histoHandle->GetZmin();\n");
            buffer.AppendFormatted("      zminStr = histoHandle->GetZminString(zminStr);\n");
            buffer.AppendFormatted("      zmax  = histoHandle->GetZmax();\n");
            buffer.AppendFormatted("      zmaxStr  = histoHandle->GetZmaxString(zmaxStr);\n");

            buffer.AppendFormatted("      histoTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(histoTitle),histoTitle,histoTitle);\n");
            buffer.AppendFormatted("      folderTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(folderTitle),folderTitle,folderTitle);\n");
            buffer.AppendFormatted("      xLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(xLabel),xLabel,xLabel);\n");
            buffer.AppendFormatted("      yLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(yLabel),yLabel,yLabel);\n");
            buffer.AppendFormatted("      zLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(zLabel),zLabel,zLabel);\n");
            buffer.AppendFormatted("      arraySize = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arraySizeStr),arraySize);\n");
            buffer.AppendFormatted("      arrayStartIndex = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arrayStartIndexStr),arrayStartIndex);\n");
            buffer.AppendFormatted("      xNbins = GetObjectInterpreterIntValue(GetObjectInterpreterCode(xNbinsStr),xNbins);\n");
            buffer.AppendFormatted("      xmin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xminStr),xmin );\n");
            buffer.AppendFormatted("      xmax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xmaxStr),xmax );\n");
            buffer.AppendFormatted("      yNbins = GetObjectInterpreterIntValue(GetObjectInterpreterCode(yNbinsStr),yNbins);\n");
            buffer.AppendFormatted("      ymin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yminStr),ymin );\n");
            buffer.AppendFormatted("      ymax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(ymaxStr),ymax);\n");
            buffer.AppendFormatted("      zNbins = GetObjectInterpreterIntValue(GetObjectInterpreterCode(zNbinsStr),zNbins);\n");
            buffer.AppendFormatted("      zmin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zminStr),zmin);\n");
            buffer.AppendFormatted("      zmax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zmaxStr),zmax);\n");

            // create histos
            buffer.AppendFormatted("      histoName = \"%s\";\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      histoName+=fHistoSuffix;\n");
            if (histoArraySize[iTask][i]=="1") {
            }
            else {
               buffer.AppendFormatted("      %s *hist%d;\n",histoType[iTask][i].Data(),i);
               buffer.AppendFormatted("      arraySizeOld = f%ss->GetEntries();\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      if (arraySize > arraySizeOld)\n");
               buffer.AppendFormatted("         f%ss->Expand(arraySize);\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      for (j=arraySizeOld;j<arraySize;j++) {\n");
               buffer.AppendFormatted("         name.SetFormatted(\"_%%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         name.Insert(0,histoName.Data());\n");
               buffer.AppendFormatted("         title.SetFormatted(\" %%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         title.Insert(0,histoTitle.Data());\n");
               if (histoType[iTask][i][2]==49) {
                  buffer.AppendFormatted("         hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax);\n",i,histoType[iTask][i].Data());
               }
               if (histoType[iTask][i][2]==50) {
                  buffer.AppendFormatted("         hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",i,histoType[iTask][i].Data());
               }
               if (histoType[iTask][i][2]==51) {
                  buffer.AppendFormatted("         hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",i,histoType[iTask][i].Data());
               }
               buffer.AppendFormatted("         f%ss->AddAt(hist%d,j);\n",histoName[iTask][i].Data(),i);
               if (!homeFolder)
                  buffer.AppendFormatted("         %sFolder->Add(f%ss->At(j));\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
               else
                  buffer.AppendFormatted("         GetHistoFolder()->Add(f%ss->At(j));\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("         hist%d->GetXaxis()->SetTitle(xLabel.Data());\n",i);
               buffer.AppendFormatted("         hist%d->GetYaxis()->SetTitle(yLabel.Data());\n",i);
               buffer.AppendFormatted("         hist%d->GetZaxis()->SetTitle(zLabel.Data());\n",i);
               buffer.AppendFormatted("      }\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // ReBook Graph
      buffer.AppendFormatted("void %sT%s_Base::ReBookGraph() {\n",shortCut.Data(),taskName[iTask].Data());
      if (numOfGraphs[iTask]>0) {
         // get graph folders
         for (i=0;i<numOfGraphs[iTask];i++) {
            if (graphFolderName[iTask][i]=="")
               continue;
            bool alreadyDefined = false;
            for (j=0;j<i;j++) {
               if (graphFolderName[iTask][i]==graphFolderName[iTask][j])
                  alreadyDefined = true;
            }
            if (!alreadyDefined) {
               buffer.AppendFormatted("      TFolder *%sFolder;\n",graphFolderName[iTask][i].Data());
               buffer.AppendFormatted("      %sFolder = static_cast<TFolder*>(GetHistoFolder()->FindObject(\"%s\"));\n",graphFolderName[iTask][i].Data(),graphFolderName[iTask][i].Data());
            }
         }
         array = false;
         for (i=0;i<numOfGraphs[iTask];i++) {
            if (graphArraySize[iTask][i]!="1") array = true;
         }
         if (array) {
            buffer.AppendFormatted("   int j;\n");
            buffer.AppendFormatted("   int arraySizeOld;\n");
            buffer.AppendFormatted("   ROMEString name;\n");
            buffer.AppendFormatted("   ROMEString title;\n");
         }
         if (numOfGraphs[iTask]>0) {
            buffer.AppendFormatted("   ROMEGraph* graphHandle;\n");
            buffer.AppendFormatted("   ROMEString graphName;\n");
            buffer.AppendFormatted("   ROMEString graphTitle;\n");
            buffer.AppendFormatted("   ROMEString folderTitle;\n");
            buffer.AppendFormatted("   ROMEString xLabel;\n");
            buffer.AppendFormatted("   ROMEString yLabel;\n");
            buffer.AppendFormatted("   ROMEString zLabel;\n");
            buffer.AppendFormatted("   int arraySize;\n");
            buffer.AppendFormatted("   ROMEString arraySizeStr;\n");
            buffer.AppendFormatted("   int arrayStartIndex;\n");
            buffer.AppendFormatted("   ROMEString arrayStartIndexStr;\n");
            buffer.AppendFormatted("   double xMin;\n");
            buffer.AppendFormatted("   ROMEString xMinStr;\n");
            buffer.AppendFormatted("   double xMax;\n");
            buffer.AppendFormatted("   ROMEString xMaxStr;\n");
            buffer.AppendFormatted("   double yMin;\n");
            buffer.AppendFormatted("   ROMEString yMinStr;\n");
            buffer.AppendFormatted("   double yMax;\n");
            buffer.AppendFormatted("   ROMEString yMaxStr;\n");
            buffer.AppendFormatted("   double zMin;\n");
            buffer.AppendFormatted("   ROMEString zMinStr;\n");
            buffer.AppendFormatted("   double zMax;\n");
            buffer.AppendFormatted("   ROMEString zMaxStr;\n");
         }
         for (i=0;i<numOfGraphs[iTask];i++) {
            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      f%sGraph->SetOriginal(\"%s\",\"%s\",%s,%s,\"%s\",\"%s\",\"%s\",%s,%s,%s,%s,%s,%s);\n",graphName[iTask][i].Data(),
               graphTitle[iTask][i].Data(),graphFolderTitle[iTask][i].Data(),graphArraySize[iTask][i].Data(),graphArrayStartIndex[iTask][i].Data(),
               graphXLabel[iTask][i].Data(),graphYLabel[iTask][i].Data(),graphZLabel[iTask][i].Data(),
               graphXmin[iTask][i].Data(),graphXmax[iTask][i].Data(),
               graphYmin[iTask][i].Data(),graphYmax[iTask][i].Data(),
               graphZmin[iTask][i].Data(),graphZmax[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
         for (i=0;i<numOfGraphs[iTask];i++) {
            bool homeFolder = false;
            if (graphFolderName[iTask][i]=="") {
               homeFolder = true;
            }

            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      graphHandle = Get%sGraph();\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      graphTitle = graphHandle->GetTitle();\n");
            buffer.AppendFormatted("      folderTitle = graphHandle->GetFolderTitle();\n");
            buffer.AppendFormatted("      xLabel = graphHandle->GetXLabel();\n");
            buffer.AppendFormatted("      yLabel = graphHandle->GetYLabel();\n");
            buffer.AppendFormatted("      zLabel = graphHandle->GetZLabel();\n");
            buffer.AppendFormatted("      arraySize = graphHandle->GetArraySize();\n");
            buffer.AppendFormatted("      arraySizeStr = graphHandle->GetArraySizeString(arraySizeStr);\n");
            buffer.AppendFormatted("      arrayStartIndex = graphHandle->GetArrayStartIndex();\n");
            buffer.AppendFormatted("      arrayStartIndexStr = graphHandle->GetArrayStartIndexString(arrayStartIndexStr);\n");
            buffer.AppendFormatted("      xMin = graphHandle->GetXmin();\n");
            buffer.AppendFormatted("      xMinStr = graphHandle->GetXminString(xMinStr);\n");
            buffer.AppendFormatted("      xMax = graphHandle->GetXmax();\n");
            buffer.AppendFormatted("      xMaxStr = graphHandle->GetXmaxString(xMaxStr);\n");
            buffer.AppendFormatted("      yMin = graphHandle->GetYmin();\n");
            buffer.AppendFormatted("      yMinStr = graphHandle->GetYminString(yMinStr);\n");
            buffer.AppendFormatted("      yMax = graphHandle->GetYmax();\n");
            buffer.AppendFormatted("      yMaxStr = graphHandle->GetYmaxString(yMaxStr);\n");
            buffer.AppendFormatted("      zMin = graphHandle->GetZmin();\n");
            buffer.AppendFormatted("      zMinStr = graphHandle->GetZminString(zMinStr);\n");
            buffer.AppendFormatted("      zMax = graphHandle->GetZmax();\n");
            buffer.AppendFormatted("      zMaxStr = graphHandle->GetZmaxString(zMaxStr);\n");

            buffer.AppendFormatted("      graphTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(graphTitle),graphTitle,graphTitle);\n");
            buffer.AppendFormatted("      folderTitle = GetObjectInterpreterCharValue(GetObjectInterpreterCode(folderTitle),folderTitle,folderTitle);\n");
            buffer.AppendFormatted("      xLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(xLabel),xLabel,xLabel);\n");
            buffer.AppendFormatted("      yLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(yLabel),yLabel,yLabel);\n");
            buffer.AppendFormatted("      zLabel = GetObjectInterpreterCharValue(GetObjectInterpreterCode(zLabel),zLabel,zLabel);\n");
            buffer.AppendFormatted("      arraySize = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arraySizeStr),arraySize);\n");
            buffer.AppendFormatted("      arrayStartIndex = GetObjectInterpreterIntValue(GetObjectInterpreterCode(arrayStartIndexStr),arrayStartIndex);\n");
            buffer.AppendFormatted("      xMin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xMinStr),xMin);\n");
            buffer.AppendFormatted("      xMax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(xMaxStr),xMax);\n");
            buffer.AppendFormatted("      yMin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yMinStr),yMin);\n");
            buffer.AppendFormatted("      yMax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(yMaxStr),yMax);\n");
            buffer.AppendFormatted("      zMin = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zMinStr),zMin);\n");
            buffer.AppendFormatted("      zMax = GetObjectInterpreterDoubleValue(GetObjectInterpreterCode(zMaxStr),zMax);\n");

            // create graphs
            buffer.AppendFormatted("      graphName = \"%s\";\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      graphName+=fGraphSuffix;\n");
            if (graphArraySize[iTask][i]=="1") {
            }
            else {
               buffer.AppendFormatted("      %s *graph%d;\n",graphType[iTask][i].Data(),i);
               buffer.AppendFormatted("      arraySizeOld = f%ss->GetEntries();\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      if (arraySize > arraySizeOld)\n");
               buffer.AppendFormatted("         f%ss->Expand(arraySize);\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      for (j=arraySizeOld;j<arraySize;j++) {\n");
               buffer.AppendFormatted("         if (f%ss->At(j))\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("            continue;\n");
               buffer.AppendFormatted("         name.SetFormatted(\"_%%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         name.Insert(0,graphName.Data());\n");
               buffer.AppendFormatted("         title.SetFormatted(\" %%0*d\",3,j+arrayStartIndex);\n");
               buffer.AppendFormatted("         title.Insert(0,graphTitle.Data());\n");
               buffer.AppendFormatted("         graph%d = new %s(1);\n",i,graphType[iTask][i].Data());
               buffer.AppendFormatted("         graph%d->SetNameTitle(\"%s\",\"%s\");\n",i,graphName[iTask][i].Data(),graphTitle[iTask][i].Data());
               buffer.AppendFormatted("         f%ss->AddAt(graph%d,j);\n",graphName[iTask][i].Data(),i);
               if (!homeFolder)
                  buffer.AppendFormatted("         %sFolder->Add(f%ss->At(j));\n",graphFolderName[iTask][i].Data(),graphName[iTask][i].Data());
               else
                  buffer.AppendFormatted("         GetHistoFolder()->Add(f%ss->At(j));\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("         graph%d->GetXaxis()->SetTitle(xLabel.Data());\n",i);
               buffer.AppendFormatted("         graph%d->GetYaxis()->SetTitle(yLabel.Data());\n",i);
               buffer.AppendFormatted("      }\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // Reset Histos
      buffer.AppendFormatted("// Reset Histos\n");
      buffer.AppendFormatted("void %sT%s_Base::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      if (numOfHistos[iTask]>0) {
         array = false;
         for (i=0;i<numOfHistos[iTask];i++) {
            if (histoArraySize[iTask][i]!="1")
               array = true;
         }
         if (array) {
            buffer.AppendFormatted("   int j;\n");
            buffer.AppendFormatted("   int nentry;\n");
         }
         for (i=0;i<numOfHistos[iTask];i++) {
            buffer.AppendFormatted("   if (f%sHisto->IsActive()) {\n",histoName[iTask][i].Data());
            if (histoArraySize[iTask][i]=="1") {
               buffer.AppendFormatted("      if (!Get%sHisto()->isAccumulation()) f%s->Reset();\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else {
               buffer.AppendFormatted("      if (!Get%sHisto()->isAccumulation()) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("          nentry = f%ss->GetEntries();\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("          for (j=0;j<nentry;j++) ((%s*)f%ss->At(j))->Reset();\n",histoType[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      }\n");
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // Histo Getters
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("// Histo Getters\n");
         for (i=0;i<numOfHistos[iTask];i++) {
            if (histoArraySize[iTask][i]=="1") {
               buffer.AppendFormatted("%s* %sT%s_Base::Get%s()\n",histoType[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!f%sHisto->IsActive()) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      Error(\"Get%s\", \"histogram %s is deactivated. Please check the state with Is%sActive() in your code.\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      return 0;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
               buffer.AppendFormatted("      f%s = (%s*)(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(\"%sT%s:%s\"));\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("   return f%s;\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else {
               buffer.AppendFormatted("%s* %sT%s_Base::Get%sAt(Int_t index)\n",histoType[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!f%sHisto->IsActive()) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      Error(\"Get%sAt\", \"histogram %s is deactivated. Please check the state with Is%sActive() in your code.\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      return 0;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor()) {\n");
               buffer.AppendFormatted("      ROMEString str;\n");
               buffer.AppendFormatted("      str.SetFormatted(\"%sT%s:%s_%%d\",index);\n",shortCut.Data(),taskName[iTask].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      f%s = (%s*)(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(str.Data()));\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
               buffer.AppendFormatted("      return f%s;\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   return (%s*)(f%ss->At(index));\n",histoType[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("TObjArray* %sT%s_Base::Get%s()\n",shortCut.Data(),taskName[iTask].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!f%sHisto->IsActive()) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      Error(\"Get%s\", \"histogram %s is deactivated. Please check the state with Is%sActive() in your code.\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      return 0;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
               buffer.AppendFormatted("      f%ss = (TObjArray*)(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(\"%sT%s:%ss\"));\n",histoName[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("   return f%ss;\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
         }

      }
      // Reset Graphs
      buffer.AppendFormatted("// Reset Graphs\n");
      buffer.AppendFormatted("void %sT%s_Base::ResetGraph() {\n",shortCut.Data(),taskName[iTask].Data());
      if (numOfGraphs[iTask]>0) {
         array = false;
         for (i=0;i<numOfGraphs[iTask];i++) {
            if (graphArraySize[iTask][i]!="1")
               array = true;
         }
         if (array) {
            buffer.AppendFormatted("   int j;\n");
            buffer.AppendFormatted("   int nentry;\n");
         }
         for (i=0;i<numOfGraphs[iTask];i++) {
            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            if (graphArraySize[iTask][i]=="1") {
               buffer.AppendFormatted("      f%s->Set(0);\n",graphName[iTask][i].Data());
            }
            else {
               buffer.AppendFormatted("       nentry = f%ss->GetEntries();\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("       for (j=0;j<nentry;j++) ((%s*)f%ss->At(j))->Set(0);\n",graphType[iTask][i].Data(),graphName[iTask][i].Data());
            }
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // Graph Getters
      if (numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("// Graph Getters\n");
         for (i=0;i<numOfGraphs[iTask];i++) {
            if (graphArraySize[iTask][i]=="1") {
               buffer.AppendFormatted("%s* %sT%s_Base::Get%s()\n",graphType[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      Error(\"Get%s\", \"graph %s is deactivated. Please check the state with Is%sActive() in your code.\");\n",graphName[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("      return 0;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
               buffer.AppendFormatted("      f%s = (%s*)(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(\"%sT%s:%s\"));\n",graphName[iTask][i].Data(),graphType[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("   return f%s;\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else {
               buffer.AppendFormatted("%s* %sT%s_Base::Get%sAt(Int_t index)\n",graphType[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      Error(\"Get%sAt\", \"graph %s is deactivated. Please check the state with Is%sActive() in your code.\");\n",graphName[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("      return 0;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor()) {\n");
               buffer.AppendFormatted("      ROMEString str;\n");
               buffer.AppendFormatted("      str.SetFormatted(\"%sT%s:%s_%%d\",index);\n",shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("      f%s = (%s*)(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(str.Data()));\n",graphName[iTask][i].Data(),graphType[iTask][i].Data());
               buffer.AppendFormatted("      return f%s;\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   return (%s*)(f%ss->At(index));\n",graphType[iTask][i].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("TObjArray* %sT%s_Base::Get%s()\n",shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("      Error(\"Get%s\", \"graph %s is deactivated. Please check the state with Is%sActive() in your code.\");\n",graphName[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("      return 0;\n");
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
               buffer.AppendFormatted("      f%ss = (TObjArray*)(gAnalyzer->GetSocketClientNetFolder()->FindObjectAny(\"%sT%s:%ss\"));\n",graphName[iTask][i].Data(),shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
               buffer.AppendFormatted("   return f%ss;\n",graphName[iTask][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            buffer.AppendFormatted("Bool_t %sT%s_Base::Register%s() {\n",shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      if (gAnalyzer->IsROMEMonitor())\n");
            buffer.AppendFormatted("         return gAnalyzer->GetSocketClientNetFolder()->RegisterObject(\"%sT%s:%s\");\n",shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   return false;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.AppendFormatted("Bool_t %sT%s_Base::UnRegister%s() {\n",shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   if (f%sGraph->IsActive()) {\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("      if (gAnalyzer->IsROMEMonitor())\n");
            buffer.AppendFormatted("         return gAnalyzer->GetSocketClientNetFolder()->UnRegisterObject(\"%sT%s:%s\");\n",shortCut.Data(),taskName[iTask].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   return false;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
         }

         ROMEString fctHead;
         fctHead.SetFormatted("void %sT%s_Base::",shortCut.Data(),taskName[iTask].Data());
      }

      // Get Object Interpreter Code
      int codeNumber = fNumberOfInterpreterCodes;
      buffer.AppendFormatted("Int_t %sT%s_Base::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   ROMEString path = objectPath;\n");
      buffer.AppendFormatted("   if (path.Index(\"/\")==-1 && path.Index(\"gAnalyzer->\")==-1)\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   if (path.Index(\"/SP\")==0 || path.Index(\"gAnalyzer->GetSP()\")==0) {\n");
      ROMEString path1 = "/SP";
      ROMEString path2 = "gAnalyzer->GetSP()";
      WriteSteeringInterpreterCode(buffer,codeNumber,0,iTask,path1,path2,1);
      buffer.AppendFormatted("      ROMEPrint::Error(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
      buffer.AppendFormatted("      ROMEPrint::Error(\"   %%s\\n\", path.Data());\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCode(objectPath);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Int Value
      codeNumber = fNumberOfInterpreterCodes;
      buffer.AppendFormatted("Int_t %sT%s_Base::GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   char *cstop;\n");
      buffer.AppendFormatted("   cstop=NULL;\n"); // to suppress unused warning
      buffer.AppendFormatted("   switch (code) {\n");
      buffer.AppendFormatted("      case -1:\n");
      buffer.AppendFormatted("         return defaultValue;\n");
      ROMEString steerPointer = "GetSP()";
      WriteSteeringInterpreterValue(buffer,"Int_t",codeNumber,0,iTask,steerPointer,1);
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterIntValue(code,defaultValue);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Double Value
      codeNumber = fNumberOfInterpreterCodes;
      buffer.AppendFormatted("Double_t %sT%s_Base::GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   char *cstop;\n");
      buffer.AppendFormatted("   cstop=NULL;\n"); // to suppress unused warning
      buffer.AppendFormatted("   switch (code) {\n");
      buffer.AppendFormatted("      case -1:\n");
      buffer.AppendFormatted("         return defaultValue;\n");
      steerPointer = "GetSP()";
      WriteSteeringInterpreterValue(buffer,"Double_t",codeNumber,0,iTask,steerPointer,1);
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterDoubleValue(code,defaultValue);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Char Value
      codeNumber = fNumberOfInterpreterCodes;
      buffer.AppendFormatted("ROMEString& %sT%s_Base::GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   switch (code) {\n");
      buffer.AppendFormatted("      case -1:\n");
      buffer.AppendFormatted("         return defaultValue;\n");
      steerPointer = "GetSP()";
      WriteSteeringInterpreterValue(buffer,"const char*",codeNumber,0,iTask,steerPointer,1);
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCharValue(code,defaultValue,buffer);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      //Write File
      WriteFile(cppFile.Data(), buffer.Data(), 6);
   }
   return true;
}

Bool_t ROMEBuilder::WriteTaskH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (!taskUsed[iTask])
         continue;

      // User H-File
      hFile.SetFormatted("%sinclude/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      if ((gSystem->AccessPathName(hFile.Data(),kFileExists))) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, taskAuthor[iTask].Data(), kFALSE);
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());
         clsName.SetFormatted("%sT%s", shortCut.Data(), taskName[iTask].Data());
         WriteDescription(buffer, clsName.Data(), 0, kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[iTask].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %sT%s(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder):%sT%s_Base(name,title,level,histoSuffix,histoFolder) {}\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   virtual ~%sT%s() {}\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("   // Event Methods\n");
         buffer.AppendFormatted("   virtual void Init();\n");
         buffer.AppendFormatted("   virtual void BeginOfRun();\n");
         buffer.AppendFormatted("   virtual void Event();\n");
         buffer.AppendFormatted("   virtual void EndOfRun();\n");
         buffer.AppendFormatted("   virtual void Terminate();\n\n");
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
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

Bool_t ROMEBuilder::WriteBaseTaskH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;
   int i;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (!taskUsed[iTask])
         continue;

      // File name
      hFile.SetFormatted("%sinclude/generated/%sT%s_Base.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, taskAuthor[iTask].Data(), kTRUE);
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
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("#include <TH1.h>\n");
         buffer.AppendFormatted("#include <TH2.h>\n");
         buffer.AppendFormatted("#include <TH3.h>\n");
         buffer.AppendFormatted("#include <TProfile.h>\n");
         buffer.AppendFormatted("#include <TProfile2D.h>\n");
      }
      if (numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("#include <TGraphMT.h>\n");
         buffer.AppendFormatted("#include <TGraph2D.h>\n");
      }
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("#include <TString.h>\n"); // TString is often used for steering parameter.
      }
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("#include \"ROMEHisto.h\"\n");
      }
      if (numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("#include \"ROMEGraph.h\"\n");
      }
      buffer.AppendFormatted("#include \"ROMETask.h\"\n");
      buffer.AppendFormatted("#include \"ROMEAnalyzer.h\"\n");

      for (i=0;i<numOfTaskInclude[iTask];i++) {
         if (taskLocalFlag[iTask][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",taskInclude[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("#include <%s>\n",taskInclude[iTask][i].Data());
         }
      }

      // Class
      buffer.AppendFormatted("\nclass %sT%s_Base : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("{\n");

      buffer.AppendFormatted("protected:\n");
      // Fields
      buffer.AppendFormatted("\n   char*      fCstop; //!\n\n");
      if (numOfSteering[iTask]>0) {
         WriteSteeringClass(buffer,0,iTask,1);
         buffer.AppendFormatted("\n");
      }

      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* fSteering; // Handle to Steering class\n\n");
      }

      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   %s*      f%s; // %s Handle\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s*        f%s;  // %s Handle (Only used for ROMEMonitor)\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* f%ss; // %s Handle\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("   ROMEHisto* f%sHisto; // Handle to histo class of %s\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("\n   ROMEString fHistoSuffix; //!\n");
      }
      for (i=0;i<numOfGraphs[iTask];i++) {
         if (graphArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   %s*      f%s; // %s Handle\n",graphType[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   %s*      f%sGraphStorage; // %s Storage Handle\n",graphType[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s*        f%s;  // %s Handle (Only used for ROMEMonitor)\n",graphType[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* f%ss; // %s Handle\n",graphName[iTask][i].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* f%sGraphsStorage; // %s Storage Handle\n",graphName[iTask][i].Data(),graphName[iTask][i].Data());
         }
         buffer.AppendFormatted("   ROMEGraph* f%sGraph; // Handle to graph class of %s\n",graphName[iTask][i].Data(),graphName[iTask][i].Data());
      }
      if (numOfGraphs[iTask]>0) {
         buffer.AppendFormatted("\n   ROMEString fGraphSuffix; //!\n");
      }

      // Methods
      buffer.AppendFormatted("public:\n");
      // Constructor and Event Methods
      buffer.AppendFormatted("   // Constructor\n");
      buffer.AppendFormatted("   %sT%s_Base(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder);\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   virtual ~%sT%s_Base() {",shortCut.Data(),taskName[iTask].Data());
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted(" SafeDelete(fSteering);");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; }\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; }\n");
      }
      // Access Methods
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   void Fill%s(double /* x */,double /* weight=1 */) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Fill%s() was declared deprecated. Use Get%s()->Fill().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   void Fill%s(double /* x */,double /* y */,double /* weight=1 */) { \n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Fill%s() was declared deprecated. Use Get%s()->Fill().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   void Fill%s(double /* x */,double /* y */,double /* z */,double /* weight=1 */) { \n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Fill%s() was declared deprecated. Use Get%s()->Fill().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            buffer.AppendFormatted("   void Draw%s() { \n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Draw%s() was declared deprecated. Use Get%s()->Draw().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
            buffer.AppendFormatted("      return;\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   %s* Get%s();\n",histoType[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   void Fill%sAt(Int_t /* index */,Double_t /* x */,Double_t /* weight=1 */) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Fill%sAt() was declared deprecated. Use Get%sAt()->Fill().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   void Fill%sAt(Int_t /* index */,Double_t /* x */,Double_t /* y */,Double_t /* weight=1 */) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Fill%sAt() was declared deprecated. Use Get%sAt()->Fill().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   void Fill%sAt(Int_t /* index */,Double_t /* x */,Double_t /* y */,Double_t /* z */,Double_t /* weight=1 */) {\n",histoName[iTask][i].Data());
               buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Fill%sAt() was declared deprecated. Use Get%sAt()->Fill().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
               buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
               buffer.AppendFormatted("      return;\n");
               buffer.AppendFormatted("   }\n");
            }
            buffer.AppendFormatted("   void Draw%sAt(Int_t /* index */) {\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      ROMEPrint::Error(\"\\nThe method Draw%sAt() was declared deprecated. Use Get%sAt()->Draw().\\nShutting down the program.\\n\");\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("      gROME->GetApplication()->Terminate(1);\n");
            buffer.AppendFormatted("      return;\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   %s* Get%sAt(Int_t index);\n",histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* Get%s();\n",histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("   ROMEHisto* Get%sHisto() { return f%sHisto; }\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   Bool_t Is%sActive() {return f%sHisto->IsActive(); }\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }

      for (i=0;i<numOfGraphs[iTask];i++) {
         if (graphArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   %s* Get%s();\n",graphType[iTask][i].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   %s* Get%sGraphStorage() { return f%sGraphStorage; } \n",graphType[iTask][i].Data(),graphName[iTask][i].Data(),graphName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s* Get%sAt(Int_t index);\n",graphType[iTask][i].Data(),graphName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* Get%s();\n",graphName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* Get%sGraphsStorage() { return f%sGraphsStorage; } \n",graphName[iTask][i].Data(),graphName[iTask][i].Data());
         }
         buffer.AppendFormatted("   ROMEGraph* Get%sGraph() { return f%sGraph; }\n",graphName[iTask][i].Data(),graphName[iTask][i].Data());
         buffer.AppendFormatted("   Bool_t Is%sActive() {return f%sGraph->IsActive(); }\n",graphName[iTask][i].Data(),graphName[iTask][i].Data());
         buffer.AppendFormatted("   Bool_t Register%s();\n",graphName[iTask][i].Data());
         buffer.AppendFormatted("   Bool_t UnRegister%s();\n",graphName[iTask][i].Data());
      }

      // Object Interpreter
      buffer.AppendFormatted("   Int_t GetObjectInterpreterCode(const char* objectPath);\n");
      buffer.AppendFormatted("   Int_t GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue);\n");
      buffer.AppendFormatted("   Double_t GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue);\n");
      buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer);\n");
      // Protected
      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("   // Event Methods\n");
      buffer.AppendFormatted("   virtual void Init() = 0;\n");
      buffer.AppendFormatted("   virtual void BeginOfRun() = 0;\n");
      buffer.AppendFormatted("   virtual void Event() = 0;\n");
      buffer.AppendFormatted("   virtual void EndOfRun() = 0;\n");
      buffer.AppendFormatted("   virtual void Terminate() = 0;\n\n");
      // Histo Methods
      buffer.AppendFormatted("   // Histo Methods\n");
      buffer.AppendFormatted("   virtual void BookHisto();\n");
      buffer.AppendFormatted("   virtual void ReBookHisto();\n");
      buffer.AppendFormatted("   virtual void ResetHisto();\n\n");
      // Graph Methods
      buffer.AppendFormatted("   // Graph Methods\n");
      buffer.AppendFormatted("   virtual void BookGraph();\n");
      buffer.AppendFormatted("   virtual void ReBookGraph();\n");
      buffer.AppendFormatted("   virtual void ResetGraph();\n\n");
      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s) // Base class of %sT%s\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("};\n\n");

      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());

      // Write File
      WriteFile(hFile.Data(),buffer.Data(),6);

   }
   return true;
}

Bool_t ROMEBuilder::WriteTabCpp()
{
   ROMEString cppFile;
   ROMEString genFile;
   ROMEString header;
   ROMEString buffer;
   Int_t i, j;
   ROMEString format;
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
      cout << "\n   Output Cpp-Files:" << endl;
   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      header.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());

      // Generated Includes
      buffer.Resize(0);
      WriteHeader(buffer, tabAuthor[iTab].Data(), kTRUE);
      genFile.SetFormatted("%sinclude/generated/%sT%sGeneratedIncludes.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
#if defined( R__VISUAL_CPLUSPLUS ) // This fixes errors in root includes on windows
      buffer.AppendFormatted("#include <RConfig.h>\n");
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#include <TGTab.h>\n");
      buffer.AppendFormatted("#include <TGeoManager.h>\n");
      buffer.AppendFormatted("#include <Windows4Root.h>\n");
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      fstream *fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      for (j=0;j<numOfFolder;j++) {
         if (accessFolder(fileBuffer,j)) {
            if (!folderUsed[j])
               continue;
            if (numOfValue[j] > 0 && !folderSupport[j]) {
               if (folderUserCode[j])
                  buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               else
                  buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
            }
         }
      }
      if (fileBuffer.Contains("GetWindow"))
         buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
      if (fileBuffer.Contains("GetGSP"))
         buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());

      for (j=0;j<daqNameArray->GetEntriesFast();j++) {
         tmp.SetFormatted("Get%sDAQ()",daqNameArray->At(j).Data());
         tmp2.SetFormatted("Get%s()",daqNameArray->At(j).Data());
         if (fileBuffer.Contains(tmp) || fileBuffer.Contains(tmp2))
            buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(j).Data(),shortCut.Data(),daqNameArray->At(j).Data());
      }

      WriteFile(genFile.Data(), buffer.Data(),6);

      // Description
      buffer.Resize(0);
      WriteHeader(header, tabAuthor[iTab].Data(), kFALSE);
      clsName.SetFormatted("%sT%s", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = "Begin_Html\n\n";
      clsDescription.AppendFormatted("Description:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",tabDescription[iTab].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("Usage:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",tabUsage[iTab].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("Status:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",tabStatus[iTab].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("To Do:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",tabToDo[iTab].Data());
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("Known Problems:\n");
      clsDescription.AppendFormatted("<p>\n");
      clsDescription.AppendFormatted("%s\n\n",tabKnownProblems[iTab].Data());
      clsDescription.AppendFormatted("<p>\n");
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
      clsDescription.AppendFormatted("Generated header file containing necessary includes\n");
      clsDescription.AppendFormatted("#include \"generated/%sT%sGeneratedIncludes.h\"\n\n", shortCut.Data(), tabName[iTab].Data());

      WriteDescription(header, clsName.Data(), clsDescription.Data(), kTRUE);

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
      buffer.AppendFormatted("void %sT%s::Init()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::EndInit()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::EventHandler()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::MenuClicked(TGPopupMenu * /* menu */, Long_t /* param */)\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::TabSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::TabUnSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("void %sT%s::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   BaseSetupPads(nx, ny, redraw);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("void %sT%s::Display(bool processEvents)\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   BaseDisplay(processEvents);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("void %sT%s::%s()\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   gSystem->Sleep(10000);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Write file
      if (tabObjectDisplay[iTab]) {
         str1.RemoveAll();
         str2.RemoveAll();
         cond.RemoveAll();
         equal.Set(2);
         str1.AddFormatted("ClassImp(%sT%s)",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("ClassImp(%sT%s)\nvoid %sT%s::Display(bool processEvents)\n{\n   BaseDisplay(processEvents);\n}",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
         cond.AddFormatted("%sT%s::Display(bool processEvents)",shortCut.Data(),tabName[iTab].Data());
         equal.AddAt(0,0);
         str1.AddFormatted("ClassImp(%sT%s)",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("ClassImp(%sT%s)\nvoid %sT%s::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)\n{\n   BaseSetupPads(nx,ny,redraw);\n}",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data());
         cond.AddFormatted("%sT%s::SetupPads(Int_t nx, Int_t ny, Bool_t redraw)",shortCut.Data(),tabName[iTab].Data());
         equal.AddAt(0,1);
         ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6,str1,str2,cond,equal);
      }
      else
         ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}

Bool_t ROMEBuilder::WriteBaseTabCpp()
{
   ROMEString cppFile;
   ROMEString buffer;
   Int_t i, j,k,index;
   ROMEString format;
   ROMEString clsDescription;
   ROMEString clsName;
   ROMEString fileBuffer;
   ROMEString str;
   ROMEString str1;
   ROMEString str2;
   ROMEString tabFile;
   fstream *fileStream;

   if (makeOutput)
      cout << "\n   Output Cpp-Files:" << endl;
   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/generated/%sT%s_Base.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());

      // Description
      WriteHeader(buffer, tabAuthor[iTab].Data(), kFALSE);
      clsName.SetFormatted("%sT%s_Base", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = tabDescription[iTab].Data();
      fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
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
         buffer.AppendFormatted("#include \"TGraphMT.h\"\n");
         buffer.AppendFormatted("#include <TGraph2D.h>\n");
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

      if (tabHeredity[iTab].Length()>0)
         buffer.AppendFormatted("#include \"tabs/%sT%s.h\"\n",shortCut.Data(),tabHeredity[iTab].Data());
      // Task class includes
      Bool_t *taskListed = new Bool_t[numOfTask];
      for (i=0;i<numOfTask;i++)
         taskListed[i] = false;
      for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
         if (!taskUsed[tabObjectDisplayTaskIndex[iTab][i]])
            continue;
         index = tabObjectDisplayTaskHierarchyIndex[iTab][i];
         while (index!=-1 && !taskListed[taskHierarchyClassIndex[index]]) {
            taskListed[taskHierarchyClassIndex[index]] = true;
            buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskHierarchyName[index].Data());
            index = taskHierarchyParentIndex[index];
         }
      }
      for (i=0;i<tabSingleObjectIndexMax[iTab];i++) {
         for (j=0;j<numOfTabSingleObjects[iTab];j++) {
            if (tabSingleObjectIndex[iTab][j]==i) {
               index = tabSingleObjectTaskHierarchyIndex[iTab][j];
               while (index!=-1 && !taskListed[taskHierarchyClassIndex[index]]) {
                  taskListed[taskHierarchyClassIndex[index]] = true;
                  buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskHierarchyName[index].Data());
                  index = taskHierarchyParentIndex[index];
               }
            }
         }
      }

      tabFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      fileStream = new fstream(tabFile.Data(),ios::in);
      fileBuffer.Resize(0);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (!taskUsed[taskHierarchyClassIndex[j]])
            continue;
         for (k=0;k<numOfGraphs[taskHierarchyClassIndex[j]];k++) {
            if (accessGraph(fileBuffer,taskHierarchyClassIndex[j],k) && !taskListed[taskHierarchyClassIndex[j]]) {
               taskListed[taskHierarchyClassIndex[j]] = true;
               buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskHierarchyName[j].Data());
            }
         }
      }
      delete taskListed;
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[iTab].Data());

      buffer.AppendFormatted("\nClassImp(%sT%s_Base)\n\n", shortCut.Data(), tabName[iTab].Data());

      // Constructor
      buffer.AppendFormatted("   // Constructor\n");
      if (tabObjectDisplay[iTab])
         buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(%sWindow* window):ArgusHistoDisplay(window) {\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data(), shortCut.Data());
      else if (tabHeredity[iTab].Length()>0)
         buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(%sWindow* window):%sT%s(window) {\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data(), shortCut.Data(),shortCut.Data(),tabHeredity[iTab].Data());
      else
         buffer.AppendFormatted("%sT%s_Base::%sT%s_Base(%sWindow* window):ArgusTab(window) {\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data(), shortCut.Data());
      buffer.AppendFormatted("   fWindow = window;\n");
      buffer.AppendFormatted("   fVersion = %s;\n", tabVersion[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   fInheritanceName = \"%s\";\n", tabName[iTab].Data());
         for (i=0;i<numOfTabObjectDisplays[iTab];i++)
            buffer.AppendFormatted("   fDrawOption->AddLast(\"\");\n");
      }
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         buffer.AppendFormatted("   fSteering = new Steering();\n");
      }
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   m%s       = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("%sT%s_Base::~%sT%s_Base() {\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   Stop%s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // InitTab
      int nx=1,ny=1;
      buffer.AppendFormatted("void %sT%s_Base::BaseInit() {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   int i,j;\n");
      buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
      buffer.AppendFormatted("   j=0;\n"); // to suppress unused warning
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   RegisterObjects();\n");
      buffer.AppendFormatted("   if (!RequestNewEvent(-1,-1)) {\n");
      buffer.AppendFormatted("      ROMEPrint::Warning(\"The analyzer doesn't provide any data in it's current state.\\n\");\n");
      buffer.AppendFormatted("      ROMEPrint::Warning(\"The monitor waits until the analyzer provides new data.\\n\\n\");\n");
      buffer.AppendFormatted("      gSystem->Sleep(100);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   while (!RequestNewEvent(-1,-1))\n");
      buffer.AppendFormatted("      gSystem->Sleep(100);\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   fNumberOfUserLines = 0;\n");
         buffer.AppendFormatted("   fNumberOfDisplayTypes = %d;\n",numOfTabObjectDisplays[iTab]);
      }
      buffer.AppendFormatted("   fWatchUser.Start(false);\n");
      buffer.AppendFormatted("   Init();\n");
      buffer.AppendFormatted("   fWatchUser.Stop();\n");
      if (numOfTabSingleObjects[iTab]>0) {
         buffer.AppendFormatted("   // Init Histos\n");
         buffer.AppendFormatted("   fGeneratedCanvas = new TRootEmbeddedCanvas(\"GeneratedCanvas\", this, 600, 600);\n");
         buffer.AppendFormatted("   AddFrame(fGeneratedCanvas,new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n");
         if (tabSingleObjectIndexMax[iTab]>1)
            ny = 2;
         if (tabSingleObjectIndexMax[iTab]>2)
            nx = 2;
         if (tabSingleObjectIndexMax[iTab]>4)
            ny = 3;
         if (tabSingleObjectIndexMax[iTab]>6)
            nx = 3;
         if (tabSingleObjectIndexMax[iTab]>9)
            ny = 4;
         if (tabSingleObjectIndexMax[iTab]>12)
            nx = 4;
         if (tabSingleObjectIndexMax[iTab]>16)
            ny = 5;
         if (tabSingleObjectIndexMax[iTab]>20)
            nx = 5;
         buffer.AppendFormatted("   fGeneratedCanvas->GetCanvas()->Divide(%d, %d);\n",nx,ny);
         for (i=0;i<tabSingleObjectIndexMax[iTab];i++) {
            for (j=0;j<numOfTabSingleObjects[iTab];j++) {
               if (tabSingleObjectIndex[iTab][j]==i) {
                  buffer.AppendFormatted("   if (true");
                  index = tabSingleObjectTaskHierarchyIndex[iTab][j];
                  while (index!=-1) {
                     buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->IsActive()",taskHierarchyName[index].Data(),taskHierarchySuffix[index].Data());
                     index = taskHierarchyParentIndex[index];
                  }
                  buffer.AppendFormatted(") {\n");
                  if (tabSingleObjectType[iTab][j]=="Histogram") {
                     if (histoArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]]=="1") {
                        buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",i+1);
                        buffer.AppendFormatted("      f%sPad%d = (TPad*)gPad;\n",tabSingleObjectName[iTab][j].Data(),j);
                        buffer.AppendFormatted("      f%sSingleObject%d = new %s();\n",tabSingleObjectName[iTab][j].Data(),j,histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data());
                        buffer.AppendFormatted("      *(f%sSingleObject%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%s());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                        buffer.AppendFormatted("      f%sSingleObject%d->Draw();\n",tabSingleObjectName[iTab][j].Data(),j);
                     }
                     else {
                        for (k=tabSingleObjectArrayIndexStart[iTab][j];k<=tabSingleObjectArrayIndexEnd[iTab][j];k++) {
                           buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",i+1+k-tabSingleObjectArrayIndexStart[iTab][j]);
                           buffer.AppendFormatted("      f%sPad%d_%d = (TPad*)gPad;\n",tabSingleObjectName[iTab][j].Data(),j,k);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d = new %s();\n",tabSingleObjectName[iTab][j].Data(),j,k,histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data());
                           buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%sAt(%d));\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data(),k);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->Draw();\n",tabSingleObjectName[iTab][j].Data(),j,k);
                        }
                     }
                  }
                  else if (tabSingleObjectType[iTab][j]=="Graph") {
                     if (graphArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]]=="1") {
                        buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",i+1);
                        buffer.AppendFormatted("      f%sPad%d = (TPad*)gPad;\n",tabSingleObjectName[iTab][j].Data(),j);
                        buffer.AppendFormatted("      f%sSingleObject%d = new %s();\n",tabSingleObjectName[iTab][j].Data(),j,graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data());
                        buffer.AppendFormatted("      *(f%sSingleObject%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%s());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                        buffer.AppendFormatted("      f%sSingleObject%d->GetXaxis()->SetLimits(gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetXmin(),gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetXmax());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data(),taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                        buffer.AppendFormatted("      f%sSingleObject%d->SetMinimum(gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetYmin());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                        buffer.AppendFormatted("      f%sSingleObject%d->SetMaximum(gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetYmax());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                        buffer.AppendFormatted("      f%sSingleObject%d->Draw(\"AL\");\n",tabSingleObjectName[iTab][j].Data(),j);
                     }
                     else {
                        for (k=tabSingleObjectArrayIndexStart[iTab][j];k<=tabSingleObjectArrayIndexEnd[iTab][j];k++) {
                           buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(%d);\n",i+1+k-tabSingleObjectArrayIndexStart[iTab][j]);
                           buffer.AppendFormatted("      f%sPad%d_%d = (TPad*)gPad;\n",tabSingleObjectName[iTab][j].Data(),j,k);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d = new %s();\n",tabSingleObjectName[iTab][j].Data(),j,k,graphType[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]].Data());
                           buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%sAt(%d));\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data(),k);
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->GetXaxis()->SetLimits(gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetXmin(),gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetXmax());\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data(),taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->SetMinimum(gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetYmin());\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->SetMaximum(gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->GetYmax());\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                           buffer.AppendFormatted("      f%sSingleObject%d_%d->Draw(\"AL\");\n",tabSingleObjectName[iTab][j].Data(),j,k);
                        }
                     }
                  }
                  buffer.AppendFormatted("   }\n");
                  break;
               }
            }
         }
      }
      if (tabObjectDisplay[iTab]) {
         for (i=0;i<numOfTabObjectDisplayObjectTypes[iTab];i++) {
            buffer.AppendFormatted("   fObjects->AddLast(new TObjArray());\n");
            buffer.AppendFormatted("   fLines->AddLast(new TObjArray());\n");
            buffer.AppendFormatted("   for (i=0 ; i<kMaxNumberOfPads ; i++) {\n");
            buffer.AppendFormatted("      if (fWindow->GetWindowId()==-1)\n");
            buffer.AppendFormatted("         str.SetFormatted(\"f%s_%%d_%%s\",i,fInheritanceName.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
            buffer.AppendFormatted("      else\n");
            buffer.AppendFormatted("         str.SetFormatted(\"fSubWindow%%d_%s_%%d_%%s\",fWindow->GetWindowId(),i,fInheritanceName.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
            if (tabObjectDisplayObjectType[iTab][i]=="TGraphMT") {
               buffer.AppendFormatted("      ((TObjArray*)fObjects->Last())->AddLast(new %s(1));\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      ((%s*)((TObjArray*)fObjects->Last())->Last())->SetTitle(str.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      ((%s*)((TObjArray*)fObjects->Last())->Last())->SetName(str.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      ((%s*)((TObjArray*)fObjects->Last())->Last())->GetHistogram();\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      ((%s*)((TObjArray*)fObjects->Last())->Last())->SetPoint(0,0,0);\n",tabObjectDisplayObjectType[iTab][i].Data());
            }
            else if (tabObjectDisplayObjectType[iTab][i].Contains("1"))
               buffer.AppendFormatted("      ((TObjArray*)fObjects->Last())->AddLast(new %s(str.Data(),\"\",1,0,1));\n",tabObjectDisplayObjectType[iTab][i].Data());
            else if (tabObjectDisplayObjectType[iTab][i].Contains("2"))
               buffer.AppendFormatted("      ((TObjArray*)fObjects->Last())->AddLast(new %s(str.Data(),\"\",1,0,1,1,0,1));\n",tabObjectDisplayObjectType[iTab][i].Data());
            else if (tabObjectDisplayObjectType[iTab][i].Contains("3"))
               buffer.AppendFormatted("      ((TObjArray*)fObjects->Last())->AddLast(new %s(str.Data(),\"\",1,0,1,1,0,1,1,0,1));\n",tabObjectDisplayObjectType[iTab][i].Data());
            buffer.AppendFormatted("      ((TObjArray*)fLines->Last())->AddLast(new TObjArray());\n");
            buffer.AppendFormatted("      for (j=0 ; j<kMaxNumberOfLines ; j++) {\n");
            buffer.AppendFormatted("         ((TObjArray*)((TObjArray*)fLines->Last())->Last())->AddLast(new TLine(-1, -1, -1, -1));\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }

         for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
            if (tabObjectDisplayTaskIndex[iTab][i]>-1) {
               if (tabObjectDisplayType[iTab][i].Index("TGraphMT")!=-1) {
                  buffer.AppendFormatted("   if (fNumberOfObjects<%s)\n",graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  buffer.AppendFormatted("      fNumberOfObjects = %s;\n",graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
               }
               else {
                  buffer.AppendFormatted("   if (fNumberOfObjects<%s)\n",histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  buffer.AppendFormatted("      fNumberOfObjects = %s;\n",histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
               }
            }
         }
         buffer.AppendFormatted("   if (fNumberOfObjects<fNumberOfUserTGraph)\n");
         buffer.AppendFormatted("      fNumberOfObjects = fNumberOfUserTGraph;\n");
         if (numOfTabObjectDisplays[iTab]>0) {
            for (i=0;i<numOfTabObjectDisplayObjectTypes[iTab];i++) {
               buffer.AppendFormatted("   fUserObjects->AddLast(new TObjArray());\n");
               buffer.AppendFormatted("   fUserLines->AddLast(new TObjArray());\n");
               buffer.AppendFormatted("   for (i=0;i<fNumberOfObjects;i++) {\n");
               buffer.AppendFormatted("      if (fWindow->GetWindowId()==-1)\n");
               buffer.AppendFormatted("         str.SetFormatted(\"fUser%s_%%d_%%s\",i,fInheritanceName.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      else\n");
               buffer.AppendFormatted("         str.SetFormatted(\"fSubWindow%%d_User%s_%%d_%%s\",fWindow->GetWindowId(),i,fInheritanceName.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
               if (tabObjectDisplayObjectType[iTab][i]=="TGraphMT") {
                  buffer.AppendFormatted("      ((TObjArray*)fUserObjects->Last())->AddLast(new %s(1));\n",tabObjectDisplayObjectType[iTab][i].Data());
                  buffer.AppendFormatted("      str.SetFormatted(\"fUser%s_%%d_%%s_Histo\",i,fInheritanceName.Data());\n",tabObjectDisplayObjectType[iTab][i].Data());
                  buffer.AppendFormatted("      ((TGraphMT*)((TObjArray*)fUserObjects->Last())->Last())->GetHistogram()->SetName(str.Data());\n");
                  buffer.AppendFormatted("      ((TGraphMT*)((TObjArray*)fUserObjects->Last())->Last())->SetTitle(str.Data());\n");
                  buffer.AppendFormatted("      ((TGraphMT*)((TObjArray*)fUserObjects->Last())->Last())->SetPoint(0,0,0);\n");
               }
               else if (tabObjectDisplayObjectType[iTab][i].Contains("1"))
                  buffer.AppendFormatted("      ((TObjArray*)fUserObjects->Last())->AddLast(new %s(str.Data(),\"\",1,0,1));\n",tabObjectDisplayObjectType[iTab][i].Data());
               else if (tabObjectDisplayObjectType[iTab][i].Contains("2"))
                  buffer.AppendFormatted("      ((TObjArray*)fUserObjects->Last())->AddLast(new %s(str.Data(),\"\",1,0,1,1,0,1));\n",tabObjectDisplayObjectType[iTab][i].Data());
               else if (tabObjectDisplayObjectType[iTab][i].Contains("3"))
                  buffer.AppendFormatted("      ((TObjArray*)fUserObjects->Last())->AddLast(new %s(str.Data(),\"\",1,0,1,1,0,1,1,0,1));\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("      ((TObjArray*)fUserLines->Last())->AddLast(new TObjArray());\n");
               buffer.AppendFormatted("      for (j=0;j<fNumberOfUserLines;j++) {\n");
               buffer.AppendFormatted("         ((TObjArray*)((TObjArray*)fUserLines->Last())->Last())->AddLast(new TLine(-1, -1, -1, -1));\n");
               buffer.AppendFormatted("      }\n");
               buffer.AppendFormatted("   }\n");
            }
            buffer.AppendFormatted("   fCurrentDisplayType = %d;\n",tabObjectDisplayObjectTypeIndex[iTab][0]);
            buffer.AppendFormatted("   fDisplayObjIndex = 0;\n");
         }
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseInit();\n");
      }
      buffer.AppendFormatted("   fWatchUser.Start(false);\n");
      buffer.AppendFormatted("   EndInit();\n");
      buffer.AppendFormatted("   fWatchUser.Stop();\n");
      buffer.AppendFormatted("   UnRegisterObjects();\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // BaseTabEventHandler
      buffer.AppendFormatted("void %sT%s_Base::BaseEventHandler() {\n", shortCut.Data(), tabName[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   int ii;\n");
         buffer.AppendFormatted("   if (RequestNewEvent(fWindow->GetCurrentRun(),fWindow->GetCurrentEvent())) {\n");
         buffer.AppendFormatted("      for (ii=0;ii<fNumberOfDisplayTypes;ii++)\n");
         buffer.AppendFormatted("         fDisplayObjLoaded->AddAt(0,ii);\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (fDisplayObjLoaded->At(fDisplayObjIndex))\n");
         buffer.AppendFormatted("      return;\n");
      }
      else {
         buffer.AppendFormatted("   if (!RequestNewEvent(fWindow->GetCurrentRun(),fWindow->GetCurrentEvent()))\n");
         buffer.AppendFormatted("      return;\n");
      }
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   fStyle->cd();\n");
         for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
            if (tabObjectDisplayTaskIndex[iTab][i]!=-1) {
               buffer.AppendFormatted("   int i;\n");
               break;
            }
         }
         buffer.AppendFormatted("   ROMEString str;\n");
         buffer.AppendFormatted("   str = \"\";\n");
         buffer.AppendFormatted("\n");
         for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
            if (tabObjectDisplayTaskIndex[iTab][i]!=-1) {
               buffer.AppendFormatted("   if (fDisplayObjIndex==%d",i);
               index = tabObjectDisplayTaskHierarchyIndex[iTab][i];
               while (index!=-1) {
                  buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->IsActive()",taskHierarchyName[index].Data(),taskHierarchySuffix[index].Data());
                  index = taskHierarchyParentIndex[index];
               }
               if (tabObjectDisplayType[iTab][i].BeginsWith("TG"))
                  buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->IsActive()",taskHierarchyName[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),graphName[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
               else
                  buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->Get%sHisto()->IsActive()",taskHierarchyName[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),histoName[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
               buffer.AppendFormatted(") {\n");
               if (tabObjectDisplayType[iTab][i].BeginsWith("TG")) {
                  buffer.AppendFormatted("      for (i=0;i<%s;i++) {\n",graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  if (graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]]=="1")
                     buffer.AppendFormatted("         *((%s*)((TObjArray*)fUserObjects->At(%d))->At(i)) = *(gAnalyzer->Get%s%sTaskBase()->Get%s());\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i],taskHierarchyName[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),graphName[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  else
                     buffer.AppendFormatted("         *((%s*)((TObjArray*)fUserObjects->At(%d))->At(i)) = *(gAnalyzer->Get%s%sTaskBase()->Get%sAt(i));\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i],taskHierarchyName[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),graphName[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  buffer.AppendFormatted("         if (((%s*)((TObjArray*)fUserObjects->At(%d))->At(i))->GetN()==0)\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i]);
                  buffer.AppendFormatted("            ((%s*)((TObjArray*)fUserObjects->At(%d))->At(i))->Set(1);\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i]);
                  buffer.AppendFormatted("      }\n");
                  buffer.AppendFormatted("      for (i=%s;i<fNumberOfObjects;i++) {\n",graphArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  buffer.AppendFormatted("         ((%s*)((TObjArray*)fUserObjects->At(%d))->At(i))->Set(1);\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i]);
                  buffer.AppendFormatted("      }\n");
               }
               else  {
                  buffer.AppendFormatted("      for (i=0;i<%s;i++) {\n",histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  if (histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]]=="1")
                     buffer.AppendFormatted("         *((%s*)((TObjArray*)fUserObjects->At(%d))->At(i)) = *(gAnalyzer->Get%s%sTaskBase()->Get%s());\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i],taskHierarchyName[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),histoName[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  else
                     buffer.AppendFormatted("         *((%s*)((TObjArray*)fUserObjects->At(%d))->At(i)) = *(gAnalyzer->Get%s%sTaskBase()->Get%sAt(i));\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i],taskHierarchyName[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data(),histoName[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  buffer.AppendFormatted("      }\n");
                  buffer.AppendFormatted("      for (i=%s;i<fNumberOfObjects;i++) {\n",histoArraySize[tabObjectDisplayTaskIndex[iTab][i]][tabObjectDisplayObjectIndex[iTab][i]].Data());
                  buffer.AppendFormatted("         ((%s*)((TObjArray*)fUserObjects->At(%d))->At(i))->Reset();\n",tabObjectDisplayType[iTab][i].Data(),tabObjectDisplayObjectTypeIndex[iTab][i]);
                  buffer.AppendFormatted("      }\n");
               }
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      for (i=0;i<tabSingleObjectIndexMax[iTab];i++) {
         for (j=0;j<numOfTabSingleObjects[iTab];j++) {
            if (tabSingleObjectIndex[iTab][j]==i) {
               buffer.AppendFormatted("   if (true");
               index = tabSingleObjectTaskHierarchyIndex[iTab][j];
               while (index!=-1) {
                  buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->IsActive()",taskHierarchyName[index].Data(),taskHierarchySuffix[index].Data());
                  index = taskHierarchyParentIndex[index];
               }
               if (tabSingleObjectType[iTab][j]=="Graph")
                  buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->IsActive()",taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
               else
                  buffer.AppendFormatted(" && gAnalyzer->Get%s%sTaskBase()->Get%sHisto()->IsActive()",taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
               buffer.AppendFormatted(") {\n");
               if (tabSingleObjectType[iTab][j]=="Histogram") {
                  if (histoArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]]=="1") {
                     buffer.AppendFormatted("      *(f%sSingleObject%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%s());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                     buffer.AppendFormatted("      f%sPad%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),j);
                  }
                  else {
                     for (k=tabSingleObjectArrayIndexStart[iTab][j];k<=tabSingleObjectArrayIndexEnd[iTab][j];k++) {
                        buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%sAt(%d));\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data(),k);
                        buffer.AppendFormatted("      f%sPad%d_%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),j,k);
                     }
                  }
               }
               else if (tabSingleObjectType[iTab][j]=="Graph") {
                  if (graphArraySize[tabSingleObjectTaskIndex[iTab][j]][tabSingleObjectObjectIndex[iTab][j]]=="1") {
                     buffer.AppendFormatted("      *(f%sSingleObject%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%s());\n",tabSingleObjectName[iTab][j].Data(),j,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data());
                     buffer.AppendFormatted("      f%sPad%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),j);
                  }
                  else {
                     for (k=tabSingleObjectArrayIndexStart[iTab][j];k<=tabSingleObjectArrayIndexEnd[iTab][j];k++) {
                        buffer.AppendFormatted("      *(f%sSingleObject%d_%d) = *(gAnalyzer->Get%s%sTaskBase()->Get%sAt(%d));\n",tabSingleObjectName[iTab][j].Data(),j,k,taskHierarchyName[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),taskHierarchySuffix[tabSingleObjectTaskHierarchyIndex[iTab][j]].Data(),tabSingleObjectName[iTab][j].Data(),k);
                        buffer.AppendFormatted("      f%sPad%d_%d->Modified();\n",tabSingleObjectName[iTab][j].Data(),j,k);
                     }
                  }
               }
               buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->cd(1);\n");
               buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->Modified();\n");
               buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->Update();\n");
               buffer.AppendFormatted("   }\n");
               break;
            }
         }
      }
      buffer.AppendFormatted("   fWatchUser.Start(false);\n");
      buffer.AppendFormatted("   fWatchUserEvent.Start(false);\n");
      buffer.AppendFormatted("   EventHandler();\n");
      buffer.AppendFormatted("   fWatchUserEvent.Stop();\n");
      buffer.AppendFormatted("   fWatchUser.Stop();\n");
      if (tabObjectDisplay[iTab])
         buffer.AppendFormatted("   Display(false);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Display
      buffer.AppendFormatted("void %sT%s_Base::BaseDisplay(bool processEvents) {\n", shortCut.Data(), tabName[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   int i,k,chn;\n");
         buffer.AppendFormatted("   ROMEString str;\n");
         buffer.AppendFormatted("   str = \"\";\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   for (i=0 ; i<fNumberOfPads ; i++) {\n");
         buffer.AppendFormatted("      if (fPadConfigActive)\n");
         buffer.AppendFormatted("         chn = fPadConfigChannel[i];\n");
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         chn = fChannelNumber+i;\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("      if (chn<((TObjArray*)fUserObjects->At(fCurrentDisplayType))->GetEntriesFast()) {\n");
         for (i=0;i<numOfTabObjectDisplayObjectTypes[iTab];i++) {
            buffer.AppendFormatted("         if (!strcmp(((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->ClassName(),\"%s\")) {\n",tabObjectDisplayObjectType[iTab][i].Data());
            if (tabObjectDisplayObjectType[iTab][i]=="TGraphMT") { // TGraph::operator= does not free existing fX and fY
            }
            buffer.AppendFormatted("            *((%s*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)) = *((%s*)((TObjArray*)fUserObjects->At(fCurrentDisplayType))->At(chn));\n",tabObjectDisplayObjectType[iTab][i].Data(),tabObjectDisplayObjectType[iTab][i].Data());
            if (tabObjectDisplayObjectType[iTab][i]=="TGraphMT") {
               buffer.AppendFormatted("            if(((%s*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->GetN()==0)\n",tabObjectDisplayObjectType[iTab][i].Data());
               buffer.AppendFormatted("               (((%s*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)))->Set(1);\n",tabObjectDisplayObjectType[iTab][i].Data());
            }
            buffer.AppendFormatted("         }\n");
         }
         buffer.AppendFormatted("         ((TNamed*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->SetTitle(((TNamed*)((TObjArray*)fUserObjects->At(fCurrentDisplayType))->At(chn))->GetTitle());\n");
         buffer.AppendFormatted("         for (k=0;k<TMath::Min(((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->GetEntriesFast(),fNumberOfUserLines);k++) {\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetX1(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetX1());\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetY1(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetY1());\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetX2(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetX2());\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetY2(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetY2());\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetLineColor(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetLineColor());\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetLineStyle(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetLineStyle());\n");
         buffer.AppendFormatted("            ((TLine*)((TObjArray*)((TObjArray*)fLines->At(fCurrentDisplayType))->At(i))->At(k))->SetLineWidth(((TLine*)((TObjArray*)((TObjArray*)fUserLines->At(fCurrentDisplayType))->At(chn))->At(k))->GetLineWidth());\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         if (!strcmp(((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->ClassName(),\"TGraphMT\"))\n");
         buffer.AppendFormatted("            SetLimits(((TGraphMT*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)));\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      else {\n");
         buffer.AppendFormatted("         if (!strcmp(((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i)->ClassName(),\"TGraphMT\"))\n");
         buffer.AppendFormatted("            ((TGraphMT*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->Set(1);\n");
         buffer.AppendFormatted("         else \n");
         buffer.AppendFormatted("            ((TH1*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->Reset();\n");
         buffer.AppendFormatted("         ((TNamed*)((TObjArray*)fObjects->At(fCurrentDisplayType))->At(i))->SetTitle(\"\");\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   Modified(processEvents);\n");
      }
      else {
         buffer.AppendFormatted("   return;\n");
         buffer.AppendFormatted("   WarningSuppression(processEvents);\n");
      }
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Thread
      buffer.AppendFormatted("Bool_t %sT%s_Base::ProcessMessage(Long_t msg, Long_t param1, Long_t param2) {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   return RunProcessMessageThread(msg, param1, param2);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("void %sT%s_Base::ThreadProcessMessageThread(void* arg) {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   ((%sT%s_Base*)((%sArgs*)arg)->inst)->ProcessMessageThread(((%sArgs*)arg)->msg, ((%sArgs*)arg)->param1, ((%sArgs*)arg)->param2);\n", shortCut.Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("Bool_t %sT%s_Base::RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2) {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   %sArgs* arg = new %sArgs();\n", tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   arg->inst   = this;\n");
      buffer.AppendFormatted("   arg->msg    = msg;\n");
      buffer.AppendFormatted("   arg->param1 = param1;\n");
      buffer.AppendFormatted("   arg->param2 = param2;\n");
      buffer.AppendFormatted("   TThread* mProcessMessageThread = new TThread(\"processMessageThread\",(void(*) (void *))&ThreadProcessMessageThread,(void*) arg);\n");
      buffer.AppendFormatted("   mProcessMessageThread->Run();\n");
      buffer.AppendFormatted("   return kTRUE;\n");
      buffer.AppendFormatted("}\n");
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("//%s\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("void %sT%s_Base::%s(", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("%s", threadFunctionArgument[iTab][i][j].Data());
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted(")\n");
         buffer.AppendFormatted("{\n");
         // following lines should be modified to use ROMEPrint instead of iostream.
         buffer.AppendFormatted("   cout<<endl\n");
         buffer.AppendFormatted("       <<\" Thread function %s is not implemented.\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
         buffer.AppendFormatted("       <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("       <<\"   void %s();\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("       <<\"   void %sT%s::%s()\"<<endl\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\"   {\"<<endl\n");
         buffer.AppendFormatted("       <<\"      cout<<\\\"Thread function %s is running.\\\"<<endl;\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("       <<\"   }\"<<endl<<endl;\n");
         buffer.AppendFormatted("   Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("void %sT%s_Base::Thread%s(void* arg) {\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   TThread::SetCancelOn();\n");
         buffer.AppendFormatted("   TThread::SetCancelDeferred();\n");
         buffer.AppendFormatted("   %sT%s_Base* inst = (%sT%s_Base*) arg;\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
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
         buffer.AppendFormatted("      if (inst->f%sNumberOfLoops != 0 && ++iLoop>=inst->f%sNumberOfLoops) {\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
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
         buffer.AppendFormatted("Bool_t %sT%s_Base::Start%s(", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("%s arg%d,", threadFunctionArgument[iTab][i][j].Data(), j);
         buffer.AppendFormatted("Int_t interval, Int_t nloop) {\n");

         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("   f%sArgument_%d = arg%d;\n", threadFunctionName[iTab][i].Data(), j, j);
         buffer.AppendFormatted("   f%sActive        = kTRUE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sNumberOfLoops = nloop;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sInterval      = interval;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   if (!m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s = new TThread(\"Thread%s\",(void(*) (void *))&Thread%s,(void*) this);\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s->Run();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %sT%s_Base::Stop%s() {\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   gSystem->Sleep(1000); // wait a while for threads to halt\n");
         buffer.AppendFormatted("   if (m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::Delete(m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   return kTRUE;\n");
         buffer.AppendFormatted("} \n");
         buffer.AppendFormatted("Bool_t %sT%s_Base::Kill%s() {\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
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
      buffer.AppendFormatted("void %sT%s_Base::BaseMenuClicked(TGPopupMenu *menu,Long_t param) {\n", shortCut.Data(), tabName[iTab].Data());
      if (tabObjectDisplay[iTab]) {
         if (numOfTabObjectDisplays[iTab]>0) {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
            buffer.AppendFormatted("   switch (param) {\n");
            for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
               if (tabObjectDisplayTaskHierarchyIndex[iTab][i]<0)
                  buffer.AppendFormatted("      case M_DISPLAY_%s:\n",tabObjectDisplayName[iTab][i].ToUpper(str));
               else
                  buffer.AppendFormatted("      case M_DISPLAY_%s%s:\n",tabObjectDisplayName[iTab][i].ToUpper(str),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               buffer.AppendFormatted("         {\n");
               buffer.AppendFormatted("            fDisplayObjIndex = %d;\n",i);
               buffer.AppendFormatted("            fCurrentDisplayType = %d;\n",tabObjectDisplayObjectTypeIndex[iTab][i]);
               buffer.AppendFormatted("            SetupPads(fNumberOfPadsX,fNumberOfPadsY,true);\n");
               buffer.AppendFormatted("            fMenuDisplay->RCheckEntry(M_DISPLAY_%s",tabObjectDisplayName[iTab][i].ToUpper(str));
               if (tabObjectDisplayTaskHierarchyIndex[iTab][i]>-1)
                  buffer.AppendFormatted(taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               buffer.AppendFormatted(",M_DISPLAY_%s",tabObjectDisplayName[iTab][0].ToUpper(str1));
               if (tabObjectDisplayTaskHierarchyIndex[iTab][0]>-1)
                  buffer.AppendFormatted(taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][0]].Data());
               buffer.AppendFormatted(",M_DISPLAY_%s",tabObjectDisplayName[iTab][numOfTabObjectDisplays[iTab]-1].ToUpper(str2));
               if (tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab]-1]>-1)
                  buffer.AppendFormatted(taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab]-1]].Data());
               buffer.AppendFormatted(");\n");
               buffer.AppendFormatted("            gAnalyzer->GetWindow()->RequestEventHandling();\n");
               buffer.AppendFormatted("            break;\n");
               buffer.AppendFormatted("         }\n");
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
      buffer.AppendFormatted("void %sT%s_Base::BaseTabSelected() {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   SetForeground(kTRUE);\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   fStyle->cd();\n");
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseTabSelected();\n");
         for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
            if (tabObjectDisplayTaskHierarchyIndex[iTab][i]<0)
               buffer.AppendFormatted("   fMenuDisplay->AddEntry(\"%s\", M_DISPLAY_%s);\n",tabObjectDisplayTitle[iTab][i].Data(),tabObjectDisplayName[iTab][i].ToUpper(str));
            else {
               if (tabObjectDisplayTaskHierarchyNumber[iTab][i]>0)
                  buffer.AppendFormatted("   fMenuDisplay->AddEntry(\"%d. %s\", M_DISPLAY_%s%s);\n",tabObjectDisplayTaskHierarchyNumber[iTab][i]+1,tabObjectDisplayTitle[iTab][i].Data(),tabObjectDisplayName[iTab][i].ToUpper(str),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
               else
                  buffer.AppendFormatted("   fMenuDisplay->AddEntry(\"%s\", M_DISPLAY_%s%s);\n",tabObjectDisplayTitle[iTab][i].Data(),tabObjectDisplayName[iTab][i].ToUpper(str),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
            }
         }
         if (numOfTabObjectDisplays[iTab]>0) {
            if (tabObjectDisplayTaskHierarchyIndex[iTab][0]<0)
               buffer.AppendFormatted("   fMenuDisplay->RCheckEntry(M_DISPLAY_%s+fDisplayObjIndex,M_DISPLAY_%s",tabObjectDisplayName[iTab][0].ToUpper(str),tabObjectDisplayName[iTab][0].ToUpper(str1));
            else
               buffer.AppendFormatted("   fMenuDisplay->RCheckEntry(M_DISPLAY_%s%s+fDisplayObjIndex,M_DISPLAY_%s%s",tabObjectDisplayName[iTab][0].ToUpper(str),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][0]].Data(),tabObjectDisplayName[iTab][0].ToUpper(str1),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][0]].Data());
            if (tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab]-1]<0)
               buffer.AppendFormatted(",M_DISPLAY_%s);\n",tabObjectDisplayName[iTab][numOfTabObjectDisplays[iTab]-1].ToUpper(str2));
            else
               buffer.AppendFormatted(",M_DISPLAY_%s%s);\n",tabObjectDisplayName[iTab][numOfTabObjectDisplays[iTab]-1].ToUpper(str2),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][numOfTabObjectDisplays[iTab]-1]].Data());
         }
      }
      // Check Task File for Folder/Histo/Graph access
      buffer.AppendFormatted("   RegisterObjects();\n");
      buffer.AppendFormatted("   while (!RequestEvent())\n");
      buffer.AppendFormatted("      gSystem->Sleep(100);\n");
      buffer.AppendFormatted("   gAnalyzer->GetWindow()->RequestEventHandling();\n");
      buffer.AppendFormatted("   if (gAnalyzer->IsProgramTerminated())\n");
      buffer.AppendFormatted("      BaseEventHandler();\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Tab Unselected
      buffer.AppendFormatted("void %sT%s_Base::BaseTabUnSelected() {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   SetForeground(kFALSE);\n");
      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("   ArgusHistoDisplay::BaseTabUnSelected();\n");
      }
      buffer.AppendFormatted("   TabUnSelected();\n");
      // Check Task File for Folder/Histo/Graph access
      buffer.AppendFormatted("   UnRegisterObjects();\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Register Objects
      buffer.AppendFormatted("void %sT%s_Base::RegisterObjects() {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor() && fRegisteringActive) {\n");
      tabFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      fileStream = new fstream(tabFile.Data(),ios::in);
      fileBuffer.Resize(0);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      for (j=0;j<numOfFolder;j++) {
         if (accessFolder(fileBuffer,j) || accessFolder(buffer,j)) {
            buffer.AppendFormatted("      gAnalyzer->Register%s();\n",folderName[j].Data());
         }
      }
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (!taskUsed[taskHierarchyClassIndex[j]])
            continue;
         for (k=0;k<numOfGraphs[taskHierarchyClassIndex[j]];k++) {
            if (accessGraph(fileBuffer,taskHierarchyClassIndex[j],k) || accessGraph(buffer,taskHierarchyClassIndex[j],k)) {
               buffer.AppendFormatted("      gAnalyzer->Get%s%sTaskBase()->Register%s();\n",taskHierarchyName[j].Data(),taskHierarchySuffix[j].Data(),graphName[taskHierarchyClassIndex[j]][k].Data());
            }
         }
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // UnRegister Objects
      buffer.AppendFormatted("void %sT%s_Base::UnRegisterObjects() {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor() && fRegisteringActive) {\n");
      tabFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      fileStream = new fstream(tabFile.Data(),ios::in);
      fileBuffer.Resize(0);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      for (j=0;j<numOfFolder;j++) {
         if (accessFolder(fileBuffer,j) || accessFolder(buffer,j)) {
            buffer.AppendFormatted("      gAnalyzer->UnRegister%s();\n",folderName[j].Data());
         }
      }
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (!taskUsed[taskHierarchyClassIndex[j]])
            continue;
         for (k=0;k<numOfGraphs[taskHierarchyClassIndex[j]];k++) {
            if (accessGraph(fileBuffer,taskHierarchyClassIndex[j],k) || accessGraph(buffer,taskHierarchyClassIndex[j],k)) {
               buffer.AppendFormatted("      gAnalyzer->Get%s%sTaskBase()->UnRegister%s();\n",taskHierarchyName[j].Data(),taskHierarchySuffix[j].Data(),graphName[taskHierarchyClassIndex[j]][k].Data());
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

Bool_t ROMEBuilder::WriteTabH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString fileBuffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString str;
   ROMEStrArray str1;
   ROMEStrArray str2;
   ROMEStrArray cond;
   TArrayI equal;

   Int_t i;
   if (makeOutput)
      cout << "\n   Output H-Files:" << endl;

   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;

      // User H-File
      hFile.SetFormatted("%sinclude/tabs/%sT%s.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      // Description
      buffer.Resize(0);
      WriteHeader(buffer, tabAuthor[iTab].Data(), kFALSE);
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
      buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("public:\n");

      // Constructor
      buffer.AppendFormatted("   %sT%s(%sWindow* window):%sT%s_Base(window)\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), shortCut.Data(), tabName[iTab].Data());
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
      buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
      if (tabShortDescription[iTab].Length())
         buffer.AppendFormatted(" // %s", tabShortDescription[iTab].Data());
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File

      // Check for deprecated constructor
      fstream *fileStream = new fstream(hFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      str.SetFormatted(" %sT%s()",shortCut.Data(),tabName[iTab].Data());
      if (fileBuffer.Contains(str)) {
         str.SetFormatted("Please replace %sT%s():%sT%s_Base() with %sT%s(%sWindow* window):%sT%s_Base(window).",shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),tabName[iTab].Data(),shortCut.Data(),shortCut.Data(),tabName[iTab].Data());
         cout << "The parameter list of the constructor of tabs has changed." << endl;
         cout << str.Data() << endl << endl;
      }
      if (tabObjectDisplay[iTab]) {
         str1.RemoveAll();
         str2.RemoveAll();
         cond.RemoveAll();
         equal.Set(2);
         str1.AddFormatted("ClassDef(%sT%s",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("void Display(bool processEvents=true);\n   ClassDef(%sT%s",shortCut.Data(),tabName[iTab].Data());
         cond.AddFormatted("void Display(bool processEvents=true);");
         equal.AddAt(0,0);
         str1.AddFormatted("ClassDef(%sT%s",shortCut.Data(),tabName[iTab].Data());
         str2.AddFormatted("void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);\n   ClassDef(%sT%s",shortCut.Data(),tabName[iTab].Data());
         cond.AddFormatted("void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);");
         equal.AddAt(0,1);
         ReplaceHeader(hFile.Data(), 0, buffer.Data(), 6,str1,str2,cond,equal);
      }
      else
         ReplaceHeader(hFile.Data(), 0, buffer.Data(), 6);
   }

   return kTRUE;
}

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
      WriteHeader(buffer, tabAuthor[iTab].Data(), kTRUE);
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
      if (numOfTabSingleObjects[iTab]>0) {
         buffer.AppendFormatted("#include <TH1.h>\n");
         buffer.AppendFormatted("#include <TH2.h>\n");
         buffer.AppendFormatted("#include <TH3.h>\n");
         buffer.AppendFormatted("#include <TProfile.h>\n");
         buffer.AppendFormatted("#include <TProfile2D.h>\n");
         buffer.AppendFormatted("#include <TGraphMT.h>\n");
         buffer.AppendFormatted("#include <TGraph2D.h>\n");
      }
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         buffer.AppendFormatted("#include <TString.h>\n"); // TString is often used for steering parameter.
      }
   #if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
   #endif // R__VISUAL_CPLUSPLUS

      if (tabObjectDisplay[iTab])
         buffer.AppendFormatted("#include \"ArgusHistoDisplay.h\"\n");
      else
         buffer.AppendFormatted("#include \"ArgusTab.h\"\n");
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
      if (tabObjectDisplay[iTab])
         buffer.AppendFormatted("\nclass %sT%s_Base : public ArgusHistoDisplay\n", shortCut.Data(), tabName[iTab].Data());
      else if (tabHeredity[iTab].Length()>0)
         buffer.AppendFormatted("\nclass %sT%s_Base : public %sT%s\n", shortCut.Data(), tabName[iTab].Data(),shortCut.Data(),tabHeredity[iTab].Data());
      else
         buffer.AppendFormatted("\nclass %sT%s_Base : public ArgusTab\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");

      if (tabObjectDisplay[iTab]) {
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   enum MenuEnumeration {\n");
         buffer.AppendFormatted("      M_DISPLAY_ROOT = 800,\n");
         for (i=0;i<numOfTabObjectDisplays[iTab];i++) {
            if (tabObjectDisplayTaskHierarchyIndex[iTab][i]<0)
               buffer.AppendFormatted("      M_DISPLAY_%s,\n",tabObjectDisplayName[iTab][i].ToUpper(str));
            else
               buffer.AppendFormatted("      M_DISPLAY_%s%s,\n",tabObjectDisplayName[iTab][i].ToUpper(str),taskHierarchySuffix[tabObjectDisplayTaskHierarchyIndex[iTab][i]].Data());
         }
         buffer = buffer(0,buffer.Length()-2);
         buffer.AppendFormatted("   \n};\n");
      }
      buffer.AppendFormatted("protected:\n");

      // Fields
      buffer.AppendFormatted("   %sWindow *fWindow;   // ! Window handle\n",shortCut.Data());
      buffer.AppendFormatted("\n");
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         WriteSteeringClass(buffer, 0, iTab+numOfTask+1, 1);
         buffer.AppendFormatted("\n");
      }
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         buffer.AppendFormatted("   Steering* fSteering; //! Handle to Steering class\n\n");
      }
      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("   TThread* m%s;         //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Bool_t   f%sActive;   //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sNumberOfLoops; //!\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   Int_t    f%sInterval; //!\n", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("   %s f%sArgument_%d; //!\n", threadFunctionArgument[iTab][i][j].Data(), threadFunctionName[iTab][i].Data(), j);
      }
      if (tabHeredity[iTab].Length()==0) {
         buffer.AppendFormatted("   Int_t    fVersion;              //! Version number\n");
      }
      if (numOfTabSingleObjects[iTab]>0) {
         buffer.AppendFormatted("   TRootEmbeddedCanvas *fGeneratedCanvas; //!\n");
         for (i=0;i<numOfTabSingleObjects[iTab];i++) {
            if (tabSingleObjectType[iTab][i]=="Histogram") {
               if (histoArraySize[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]]=="1") {
                  buffer.AppendFormatted("   %s* f%sSingleObject%d; //!\n",histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),tabSingleObjectName[iTab][i].Data(),i);
                  buffer.AppendFormatted("   TPad* f%sPad%d; //!\n",tabSingleObjectName[iTab][i].Data(),i);
               }
               else {
                  for (j=tabSingleObjectArrayIndexStart[iTab][i];j<=tabSingleObjectArrayIndexEnd[iTab][i];j++) {
                     buffer.AppendFormatted("   %s* f%sSingleObject%d_%d; //!\n",histoType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),tabSingleObjectName[iTab][i].Data(),i,j);
                     buffer.AppendFormatted("   TPad* f%sPad%d_%d; //!\n",tabSingleObjectName[iTab][i].Data(),i,j);
                  }
               }
            }
            else if (tabSingleObjectType[iTab][i]=="Graph") {
               if (graphArraySize[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]]=="1") {
                  buffer.AppendFormatted("   %s* f%sSingleObject%d; //!\n",graphType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),tabSingleObjectName[iTab][i].Data(),i);
                  buffer.AppendFormatted("   TPad* f%sPad%d; //!\n",tabSingleObjectName[iTab][i].Data(),i);
               }
               else {
                  for (j=tabSingleObjectArrayIndexStart[iTab][i];j<=tabSingleObjectArrayIndexEnd[iTab][i];j++) {
                     buffer.AppendFormatted("   %s* f%sSingleObject%d_%d; //!\n",graphType[tabSingleObjectTaskIndex[iTab][i]][tabSingleObjectObjectIndex[iTab][i]].Data(),tabSingleObjectName[iTab][i].Data(),i,j);
                     buffer.AppendFormatted("   TPad* f%sPad%d_%d; //!\n",tabSingleObjectName[iTab][i].Data(),i,j);
                  }
               }
            }
         }
      }

      // Methods
      buffer.AppendFormatted("public:\n");

      // Constructor
      buffer.AppendFormatted("   // Constructor\n");
      buffer.AppendFormatted("   %sT%s_Base(%sWindow* window);\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data());
      buffer.AppendFormatted("   virtual ~%sT%s_Base();\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("\n");

      // InitTab
      buffer.AppendFormatted("   void BaseInit();\n");
      buffer.AppendFormatted("   virtual void Init() = 0;\n");
      buffer.AppendFormatted("   virtual void EndInit() {}\n");
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
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; }\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; }\n");
      }

      // Tab Selected
      buffer.AppendFormatted("   void BaseTabSelected();\n");
      buffer.AppendFormatted("   virtual void TabSelected() = 0;\n");
      // Tab Unselected
      buffer.AppendFormatted("   void BaseTabUnSelected();\n");
      buffer.AppendFormatted("   virtual void TabUnSelected() = 0;\n");

      buffer.AppendFormatted("\n");
      // Menu Clicked
      buffer.AppendFormatted("   void BaseMenuClicked(TGPopupMenu *menu,Long_t param);\n");
      buffer.AppendFormatted("   virtual void MenuClicked(TGPopupMenu *menu,Long_t param) = 0;\n");
      buffer.AppendFormatted("\n");


      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s) // Base class of %sT%s\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File
      WriteFile(hFile.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}

Bool_t ROMEBuilder::WriteSteering(Int_t iTask)
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString tmp;
   Int_t i;

   hFile.SetFormatted("%sinclude/generated/%sGlobalSteering.h",outDir.Data(),shortCut.Data());

   if (numOfSteering[numOfTask]==-1) {
      remove(hFile.Data());
      return true;
   }

   if (makeOutput) cout << "\n   Output Files:" << endl;

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
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
   for (i=0;i<numOfGSPInclude;i++) {
      if (gspLocalFlag[i]) {
         tmp.SetFormatted("#include \"%s\"",gspInclude[i].Data());
      }
      else {
         tmp.SetFormatted("#include <%s>",gspInclude[i].Data());
      }
      if (!buffer.Contains(tmp))
         buffer.AppendFormatted("%s\n",tmp.Data());
   }
   // Folder includes
#if 0 // this is not necessary, probably
   Int_t j = 0;
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         for (j=0;j<numOfFolderInclude[i];j++) {
            if (folderLocalFlag[i][j]) {
               tmp.SetFormatted("#include \"%s\"",folderInclude[i][j].Data());
            }
            else {
               tmp.SetFormatted("#include <%s>",folderInclude[i][j].Data());
            }
            if (!buffer.Contains(tmp))
               buffer.AppendFormatted("%s\n",tmp.Data());
         }
      }
   }
#endif
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

   WriteSteeringClass(buffer,0,iTask,0);

   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());

   //Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteAnalyzerCpp()
{
   int i,j,index;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;
   ROMEString buf;
   ROMEString str;
   ROMEString pointer;

   if (makeOutput) cout << "\n   Output Cpp-File:" << endl;

   ROMEString tmp;
   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
         if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
      }
   }

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());
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
   buffer.AppendFormatted("#include <TTask.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfigToForm.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sNetFolderServer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMERint.h\"\n");
   buffer.AppendFormatted("#include \"ROMENetFolder.h\"\n");
   buffer.AppendFormatted("#include \"ROMEStr2DArray.h\"\n");
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
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\n");

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
   authors[0] = mainAuthor;
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      same = false;
      for (j=0;j<numAuthors;j++) {
         if (!authors[j].CompareTo(folderAuthor[i],TString::kIgnoreCase)) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = folderAuthor[i];
         numAuthors++;
      }
   }
   for (i=0;i<numOfTask;i++) {
      same = false;
      for (j=0;j<numAuthors;j++) {
         if (!authors[j].CompareTo(taskAuthor[i],TString::kIgnoreCase)) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = taskAuthor[i];
         numAuthors++;
      }
   }
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      same = false;
      for (j=0;j<numAuthors;j++) {
         if (!authors[j].CompareTo(tabAuthor[i],TString::kIgnoreCase)) {
            same = true;
            break;
         }
      }
      if (!same) {
         authors[numAuthors] = tabAuthor[i];
         numAuthors++;
      }
   }
   buffer.AppendFormatted("   ROMEString *authors = new ROMEString[%d];\n",numAuthors);
   for (i=0;i<numAuthors;i++) {
      buffer.AppendFormatted("   authors[%d] = \"%s\";\n",i,authors[i].Data());
   }
   buffer.AppendFormatted("   CreateSplash(3,\"%s%s\",\"%s\",authors,%d);\n",shortCut.Data(),mainProgName.Data(),romeVersion.Data(),numAuthors);
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
#endif
   buffer.AppendFormatted("\n");

   // Constructor
   buffer.AppendFormatted("%sAnalyzer::%sAnalyzer(ROMERint *app,Bool_t batch,Bool_t daemon,Bool_t nographics):ROMEAnalyzer(app,batch,daemon,nographics) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("// Folder and Task initialisation\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   fOnlineAnalyzerName = fProgramName;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gAnalyzer = this;\n");
   buffer.AppendFormatted("   gROME = static_cast<ROMEAnalyzer*>(this);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (!isNoGraphics()) {\n");
   buffer.AppendFormatted("      fWindow = new %sWindow(gClient->GetRoot(),kTRUE);\n",shortCut.Data());
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fWindow = new %sWindow();\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString name;\n");
   buffer.AppendFormatted("   name.SetFormatted(\"ARGUS - %%s\", GetProgramName());\n");
   buffer.AppendFormatted("   fWindow->SetWindowName(name.Data());\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("   f%sDAQ = NULL;\n",daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("   fGlobalSteeringParameters = new %sGlobalSteering();\n",shortCut.Data());
   }
   // Folder
   buffer.AppendFormatted("   // Folder initialisation\n");
   buffer.AppendFormatted("   fMainFolder = gROOT->GetRootFolder()->AddFolder(\"%s\",\"Root Folder of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   gROOT->GetListOfBrowsables()->Add(fMainFolder,\"%s\");\n\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (folderParentName[i]=="GetMainFolder()")
               parentt = folderParentName[i];
         else
            parentt.SetFormatted("%sFolder",folderParentName[i].Data());
         format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length());
         buffer.AppendFormatted(format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            buffer.AppendFormatted("\n");
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("   f%sFolder = new %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolder);\n",folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFolderStorage = new %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("   f%sFolders = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFolders->SetName(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   Set%sSize(1);\n",folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolders);\n",folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFoldersStorage = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   f%sFoldersStorage->SetName(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   InitTasks();\n");
   buffer.AppendFormatted("\n");
   // DBAccess
   buffer.AppendFormatted("   fDBAccess = new %sDBAccess();\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // NetFolder
   buffer.AppendFormatted("   InitNetFolders(%d);\n", numOfNetFolder);
   buffer.AppendFormatted("   fNumberOfNetFolders = %d;\n", numOfNetFolder);
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
      if (netFolderHost[i].Length())
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"%s\";\n", i, netFolderHost[i].Data());
      else
         buffer.AppendFormatted("   fNetFolderHost[%d]   = \"localhost\";\n", i);
      if (netFolderPort[i].Length())
         buffer.AppendFormatted("   fNetFolderPort[%d]   = %s;\n", i, netFolderPort[i].Data());
      else
         buffer.AppendFormatted("   fNetFolderPort[%d]   = 9090;\n", i);
      if (netFolderRoot[i].Length())
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n", i, netFolderRoot[i].Data());
      else
//         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"histos\";\n",i);
         buffer.AppendFormatted("   fNetFolderRoot[%d]   = \"%s\";\n", i, shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   buffer.AppendFormatted("\n");

   // End of Constructor
   buffer.AppendFormatted("}\n\n");

   // Destructor
   buffer.AppendFormatted("%sAnalyzer::~%sAnalyzer()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   // Folder Fields
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("   SafeDelete(f%sFolder);\n",folderName[i].Data());
               buffer.AppendFormatted("   SafeDelete(f%sFolderStorage);\n",folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("   SafeDelete(f%sFolders);\n",folderName[i].Data());
               buffer.AppendFormatted("   SafeDelete(f%sFoldersStorage);\n",folderName[i].Data());
            }
         }
      }
   }
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("   SafeDelete(fGlobalSteeringParameters);\n");
   }
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("   SafeDelete(f%sDAQ);\n",daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("   SafeDelete(fWindow);\n");
   buffer.AppendFormatted("   SafeDelete(fConfiguration);\n");
   // End of Destructor
   buffer.AppendFormatted("}\n\n");

   // Folder Getters and Setters
   buffer.AppendFormatted("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      WriteFolderGetterSource(buffer,i);
      WriteFolderSetterSource(buffer,i);
   }
   buffer.AppendFormatted("\n");

   // Set size
   ROMEString separator;
   for (i=0;i<numOfFolder;i++) {
      separator = "";
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i]!="1") {
         buffer.AppendFormatted("void %sAnalyzer::Set%sSize(Int_t number) {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("   int i;\n");
         buffer.AppendFormatted("   if (f%sFolders) f%sFolders->Delete();\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("   for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("      new((*f%sFolders)[i]) %s%s(",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if (valueIsTObject[i][j] && !isPointerType(valueType[i][j].Data())
               && !valueType[i][j].Contains("TRef") && !valueType[i][j].Contains("TString"))
            continue;
            if (valueDimension[i][j]==0) {
               buffer.AppendFormatted("%s(%s)%s",separator.Data(),valueType[i][j].Data(),valueInit[i][j].Data());
               separator = ", ";
            }
         }
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }
   buffer.AppendFormatted("\n");

   // GetDBPathWriteFlag
   buffer.AppendFormatted("Bool_t %sAnalyzer::GetDBPathWriteFlag(const char* path) {\n",shortCut.Data());
   buffer.AppendFormatted("   Int_t i,num;\n");
   buffer.AppendFormatted("   ROMEString subStr;\n");
   buffer.AppendFormatted("   ROMEString str = path;\n");
   buffer.AppendFormatted("   Int_t ind = str.Last('\"');\n");
   buffer.AppendFormatted("   if (ind>-1 && ind<str.Length()-1) {\n");
   buffer.AppendFormatted("      str = str(ind+2,str.Length()-ind+2);\n");
   buffer.AppendFormatted("      num = str.CountChar(',')+1;\n");
   buffer.AppendFormatted("      for (i=0;i<num;i++) {\n");
   buffer.AppendFormatted("         if (i<num-1)\n");
   buffer.AppendFormatted("            ind = str.First(',');\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            ind = str.Length();\n");
   buffer.AppendFormatted("         subStr = str(0,ind);\n");
   buffer.AppendFormatted("         if (GetObjectInterpreterCode(subStr.Data())==-2)\n");
   buffer.AppendFormatted("            return kFALSE;\n");
   buffer.AppendFormatted("         str = str(ind+1,str.Length()-ind+1);\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ReadUserParameter
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadUserParameter(const char* opt, const char* value, Int_t& i) {\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   ROMEString option = opt;\n");
   buffer.AppendFormatted("   ROMEString tmp;\n");
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop=NULL;\n"); // to suppress unused warning
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTask]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("fGlobalSteering");
      steerPointerT.SetFormatted("GetGSP()");
      WriteSteeringReadParameters(buffer,0,numOfTask,pointerT,steerPointerT);
   }
   // Task steering parameter
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s_Base*)GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         WriteSteeringReadParameters(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }
   // Tab steering parameter
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      Int_t index = i;
      pointer.Resize(0);
      while (index != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (numOfSteering[i+numOfTask+1] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("GetWindow()->Get%s%sTab()->GetSP()",tabName[i].Data(),tabSuffix[i].Data());
         WriteSteeringReadParameters(buffer, 0, i+numOfTask+1, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("   WarningSuppression(value);\n");
   buffer.AppendFormatted("   WarningSuppression(i);\n");
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n\n");

   // UserParameterUsage
   buffer.AppendFormatted("void %sAnalyzer::UserParameterUsage() {\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTask]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("GetGSP()");
      WriteSteeringParameterUsage(buffer,0,numOfTask,pointerT,steerPointerT);
   }
   // Task steering parameter
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("   // %s task\n",taskHierarchyName[i].Data());
      int index = i;
      pointer.Resize(0);
      while (index!=-1) {
         pointer.InsertFormatted(0,"->f%sTask",taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering",pointer.Data());
         steerPointerT.SetFormatted("((%sT%s_Base*)GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         WriteSteeringParameterUsage(buffer,0,taskHierarchyClassIndex[i],pointerT,steerPointerT);
      }
   }
   // Tab steering parameter
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   // %s tab\n", tabName[i].Data());
      Int_t index = i;
      pointer.Resize(0);
      while (index != -1) {
         pointer.InsertFormatted(0, "->f%sTab", tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (numOfSteering[i+numOfTask+1] > 0) {
         ROMEString pointerT;
         ROMEString steerPointerT;
         pointerT.SetFormatted("%s->fSteering", pointer.Data());
         steerPointerT.SetFormatted("((%sT%s_Base*)Get%s%sTab())->GetSP()", shortCut.Data(), tabName[i].Data(), tabName[i].Data(),tabSuffix[i].Data());
         WriteSteeringParameterUsage(buffer, 0, i+numOfTask+1, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("}\n\n");

   // DAQ Access Methods
   buffer.AppendFormatted("// Deprecated DAQ Access Methods\n");
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("// %s DAQ Access Methods\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("%s%sDAQ* %sAnalyzer::Get%sDAQ() {\n",daqTypeArray->At(i).Data(),daqNameArray->At(i).Data(),shortCut.Data(),daqNameArray->At(i).Data());
      buffer.AppendFormatted("   if (f%sDAQ==NULL) {\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("      ROMEPrint::Error(\"\\nYou have tried to access the %s DAQ system over a gAnalyzer->Get%sDAQ()\\nhandle but the current DAQ system is not '%s'.\\n\\nShutting down the program.\\n\");\n",daqNameArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
      buffer.AppendFormatted("      fApplication->Terminate(1);\n");
      buffer.AppendFormatted("      return NULL;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return f%sDAQ;\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("}\n\n");
   }

   // Histos
   buffer.AppendFormatted("// Histos\n");
   buffer.AppendFormatted("TH1* %sAnalyzer::GetHisto(const char* pathToHisto) {\n",shortCut.Data());
   buffer.AppendFormatted("   return ((TH1*)gROOT->FindObjectAny(pathToHisto));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("bool %sAnalyzer::ResetAllHistos() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         if (histoArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   if (Get%s%sTaskBase()->Get%sHisto()->IsActive() && !Get%s%sTaskBase()->Get%sHisto()->isAccumulation())\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      Get%s%sTaskBase()->Get%s()->Reset();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   if (Get%s%sTaskBase()->Get%sHisto()->IsActive() && !Get%s%sTaskBase()->Get%sHisto()->isAccumulation()) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      for (i=0;i<Get%s%sTaskBase()->Get%sHisto()->GetArraySize();i++)\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         Get%s%sTaskBase()->Get%sAt(i)->Reset();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // Graphs
   buffer.AppendFormatted("// Graphs\n");
   buffer.AppendFormatted("TH1* %sAnalyzer::GetGraph(const char* pathToGraph) {\n",shortCut.Data());
   buffer.AppendFormatted("   return ((TH1*)gROOT->FindObjectAny(pathToGraph));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("bool %sAnalyzer::ResetAllGraphs() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   if (Get%s%sTaskBase()->Get%sGraph()->IsActive())\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      Get%s%sTaskBase()->Get%s()->Set(0);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   if (Get%s%sTaskBase()->Get%sGraph()->IsActive()) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      for (i=0;i<Get%s%sTaskBase()->Get%sGraph()->GetArraySize();i++)\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         Get%s%sTaskBase()->Get%sAt(i)->Set(0);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Connect Socket Client NetFolder
   buffer.AppendFormatted("// Connect Socket Client NetFolder\n");
   buffer.AppendFormatted("Bool_t %sAnalyzer::ConnectSocketClientNetFolder() {\n",shortCut.Data());
   buffer.AppendFormatted("   SafeDelete(fSocketClientNetFolder);\n");
   buffer.AppendFormatted("   fSocketClientNetFolder = new ROMENetFolder(\"%s\",\"RootNetFolder\", fSocketClient, true);\n",shortCut.Data());
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
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   CloseHandle(CreateThread(NULL,1024,&SplashThread,0,0,lpThreadId));\n");
   buffer.AppendFormatted("}\n");
#endif
   buffer.AppendFormatted("\n");
#if defined( R__UNIX )
   buffer.AppendFormatted("void %sAnalyzer::startSplashScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   \n");
   buffer.AppendFormatted("}\n");
#endif
   buffer.AppendFormatted("\n");

   // Console Screen
   buffer.AppendFormatted("void %sAnalyzer::consoleStartScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEPrint::Print(%sLogo);\n", shortCut.Data());
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Folder dump and load
   buffer.AppendFormatted("Bool_t %sAnalyzer::DumpFolders(const char* filename, Bool_t only_database) {\n", shortCut.Data());
   buffer.AppendFormatted("   if(!filename) return kFALSE;\n");
   buffer.AppendFormatted("   TFile out(filename, \"RECREATE\");\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (!folderDataBase[i])
               buffer.AppendFormatted("   if (!only_database) {\n");
            else
               buffer.AppendFormatted("   {\n");
            if (folderArray[i]=="1")
               buffer.AppendFormatted("      out.WriteTObject(f%sFolder, \"%s\");\n",folderName[i].Data(),folderName[i].Data());
            else
               buffer.AppendFormatted("      out.WriteTObject(f%sFolders, \"%ss\", \"SingleKey\");\n",folderName[i].Data(),folderName[i].Data());
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

   buffer.AppendFormatted("Bool_t %sAnalyzer::LoadFolders(const char* filename, Bool_t only_database) {\n", shortCut.Data());
   buffer.AppendFormatted("   if(!filename) return kFALSE;\n");
   buffer.AppendFormatted("   TFile in(filename);\n");
   buffer.AppendFormatted("   if(in.IsZombie()) {\n");
   buffer.AppendFormatted("      Error(\"LoadFolders\", \"%%s not found\", filename);\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (!folderDataBase[i])
               buffer.AppendFormatted("   if (!only_database) {\n");
            else
               buffer.AppendFormatted("   {\n");
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("      %s%s* p%s = (%s%s*)in.FindObjectAny(\"%s\");\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data()
                                      ,shortCut.Data(),folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      if (p%s) {\n",folderName[i].Data());
               buffer.AppendFormatted("         SafeDelete(f%sFolder);\n",folderName[i].Data());
               buffer.AppendFormatted("         f%sFolder = static_cast<%s%s*>(p%s->Clone(p%s->GetName()));\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data(),folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      }\n");
            }
            else {
               buffer.AppendFormatted("      TClonesArray *p%s = (TClonesArray*)in.FindObjectAny(\"%ss\");\n",folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      if (p%s) {\n",folderName[i].Data());
               buffer.AppendFormatted("          SafeDelete(f%sFolders);\n",folderName[i].Data());
               buffer.AppendFormatted("          f%sFolders = static_cast<TClonesArray*>(p%s->Clone(p%s->GetName()));\n",folderName[i].Data(),folderName[i].Data(),folderName[i].Data());
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
   buffer.AppendFormatted("void %sAnalyzer::StartNetFolderServer() {\n", shortCut.Data());
   buffer.AppendFormatted("   if(fNetFolderServer) {\n");
   buffer.AppendFormatted("      Error(\"StartNetFolderServer\", \"Another server is already running.\");\n");
   buffer.AppendFormatted("      return;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   fNetFolderServer = new %sNetFolderServer();\n", shortCut.Data());
   buffer.AppendFormatted("   ((%sNetFolderServer*)fNetFolderServer)->StartServer(GetApplication(),GetSocketServerPortNumber(),\"ROME\");\n", shortCut.Data());
   buffer.AppendFormatted("   ROMEPrint::Print(\"Root server listening on port %%d\\n\", GetSocketServerPortNumber());\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Fill Object Storage
   buffer.AppendFormatted("void %sAnalyzer::FillObjectStorage() {\n", shortCut.Data());
   buffer.AppendFormatted("   //create a buffer where the object will be streamed\n");
   buffer.AppendFormatted("   TFile *filsav = gFile;\n");
   buffer.AppendFormatted("   gFile = 0;\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   i = 0;\n");
   buffer.AppendFormatted("   const Int_t bufsize = 10000;\n");
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0))\n");
   buffer.AppendFormatted("   TBufferFile *buffer = new TBufferFile(TBuffer::kWrite,bufsize);\n");
   buffer.AppendFormatted("#else\n");
   buffer.AppendFormatted("   TBuffer *buffer = new TBuffer(TBuffer::kWrite,bufsize);\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("   Bool_t bypassOld;\n");
   buffer.AppendFormatted("   bypassOld = kFALSE;\n"); // to suppress unused warning
   buffer.AppendFormatted("   Bool_t bypassStorageOld;\n");
   buffer.AppendFormatted("   bypassStorageOld = kFALSE;\n"); // to suppress unused warning
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderSupport[i])
         continue;
      if (numOfValue[i] > 0) {
         if (folderArray[i]=="1") {
            WriteFillObjectStorageObject(buffer,"f"+folderName[i]+"Folder","f"+folderName[i]+"FolderStorage","(("+shortCut+"NetFolderServer*)fNetFolderServer)->Get"+folderName[i]+"FolderActive(i)",false);
         }
         else {
            WriteFillObjectStorageObject(buffer,"f"+folderName[i]+"Folders","f"+folderName[i]+"FoldersStorage","(("+shortCut+"NetFolderServer*)fNetFolderServer)->Get"+folderName[i]+"FolderActive(i)",true);
         }
      }
   }
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (numOfGraphs[taskHierarchyClassIndex[i]]>0) {
         buffer.AppendFormatted("   if (Get%s%sTaskBase()->IsActive()",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
         index = taskHierarchyParentIndex[i];
         while (index!=-1) {
            buffer.AppendFormatted(" && Get%s%sTaskBase()->IsActive()",taskHierarchyName[index].Data(),taskHierarchySuffix[index].Data());
            index = taskHierarchyParentIndex[index];
         }
         buffer.AppendFormatted(") {\n");
      }
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            WriteFillObjectStorageObject(buffer,"Get"+taskHierarchyName[i]+taskHierarchySuffix[i]+"TaskBase()->Get"+graphName[taskHierarchyClassIndex[i]][j]+"()","Get"+taskHierarchyName[i]+taskHierarchySuffix[i]+"TaskBase()->Get"+graphName[taskHierarchyClassIndex[i]][j]+"GraphStorage()","(("+shortCut+"NetFolderServer*)fNetFolderServer)->Get"+taskHierarchyName[i]+taskHierarchySuffix[i]+"_"+graphName[taskHierarchyClassIndex[i]][j]+"GraphActive(i)",false);
         }
         else {
            WriteFillObjectStorageObject(buffer,"Get"+taskHierarchyName[i]+taskHierarchySuffix[i]+"TaskBase()->Get"+graphName[taskHierarchyClassIndex[i]][j]+"()","Get"+taskHierarchyName[i]+taskHierarchySuffix[i]+"TaskBase()->Get"+graphName[taskHierarchyClassIndex[i]][j]+"GraphsStorage()","(("+shortCut+"NetFolderServer*)fNetFolderServer)->Get"+taskHierarchyName[i]+taskHierarchySuffix[i]+"_"+graphName[taskHierarchyClassIndex[i]][j]+"GraphActive(i)",false);
         }
      }
      if (numOfGraphs[taskHierarchyClassIndex[i]]>0)
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

   if (makeOutput) cout << "\n   Output Cpp-File:" << endl;

   ROMEString tmp;
   ROMEString format;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer2.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TObjArray.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"tasks/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sAnalyzer::InitTasks()\n",shortCut.Data());
   buffer.AppendFormatted("{ \n");
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   fMainTask = new %sEventLoop(\"analyzer\",\"Main Task of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormatted("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   buffer.AppendFormatted("   fMainHistoFolder = fMainFolder->AddFolder(\"histos\",\"Histogram Folder\");\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fTaskObjects = new TObjArray(%d);\n",numOfTaskHierarchy);
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      // create histo folder
      if (taskHierarchyParentIndex[i]==-1)
         buffer.AppendFormatted("   fHistoFolders->AddAtAndExpand(fMainHistoFolder->AddFolder(\"%sHistos%s\",\"Histograms of Task '%s%s'\"),%d);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),i);
      else
         buffer.AppendFormatted("   fHistoFolders->AddAtAndExpand(GetHistoFolderAt(%d)->AddFolder(\"%sHistos%s\",\"Histograms of Task '%s%s'\"),%d);\n",taskHierarchyParentIndex[i],taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),i);
      // create task
      buffer.AppendFormatted("   AddTask(new %sT%s(\"%s%s\",\"%s\",%d,\"%s\",GetHistoFolderAt(%d)));\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),"",taskHierarchyLevel[i],taskHierarchySuffix[i].Data(),i);
      buffer.AppendFormatted("   GetTaskObjectAt(%d)->SetActive(false);\n",taskHierarchyObjectIndex[i]);
      if (taskHierarchyParentIndex[i]==-1)
         parentt = "GetMainTask()";
      else
         parentt.SetFormatted("GetTaskObjectAt(%d)",taskHierarchyParentIndex[i]);
      buffer.AppendFormatted("   %s->Add(GetTaskObjectAt(%d));\n",parentt.Data(),taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

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

   if (makeOutput) cout << "\n   Output Cpp-File:" << endl;

   ROMEString tmp;
   ROMEString format;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sAnalyzer3.cpp",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());
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
   for (i=0;i<numOfTask;i++) {
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
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
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

   // Count number of databases
   int ndb = 0;
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i])
         ndb++;
   }

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++) {
         if (!folderUsed[i])
            continue;
         if (folderDataBase[i] && folderArray[i]=="1" && !folderSupport[i]) {
            buffer.AppendFormatted("   if (!fDBAccess->Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ReadArrayDataBaseFolders
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadArrayDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++) {
         if (!folderUsed[i])
            continue;
         if (folderDataBase[i] && folderArray[i]!="1" && !folderSupport[i]) {
            buffer.AppendFormatted("   if (!fDBAccess->Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ShowConfigurationFile
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
   buffer.AppendFormatted("   cstop = NULL;\n"); // to suppress unused warning
   buffer.AppendFormatted("   int exitID;\n");
   buffer.AppendFormatted("   %sConfigToForm *dialog = new %sConfigToForm();\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   if((exitID=dialog->Show(gClient->GetRoot(),NULL))==1) {\n");
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
   for (i=0;i<mainParGroup->GetNumberOfSubGroups();i++) {
      buffer.AppendFormatted("      if (!Save%s(dialog)) return false;\n",mainParGroup->GetSubGroupAt(i)->GetGroupName().Data());
   }
   buffer.AppendFormatted("      GetConfiguration()->CheckConfigurationModified(0);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   SafeDelete(dialog);\n");
   buffer.AppendFormatted("   return exitID!=-1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   WriteConfigToFormSave(buffer,mainParGroup,"","","",0,1,"");
   buffer.AppendFormatted("\n");

   // Get Object Interpreter Code
   int codeNumber = 0;
   buffer.AppendFormatted("Int_t %sAnalyzer::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString path = objectPath;\n");
   buffer.AppendFormatted("   if (path.Index(\"/\")==-1 && path.Index(\"gAnalyzer->\")==-1)\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   if (path.Index(\"/GSP\")==0 || path.Index(\"gAnalyzer->GetGSP()\")==0) {\n");
   ROMEString path1 = "/GSP";
   ROMEString path2 = "GetGSP()";
   codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,0,numOfTask,path1,path2,1);
   buffer.AppendFormatted("      ROMEPrint::Error(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
   buffer.AppendFormatted("      ROMEPrint::Error(\"   %%s\\n\", path.Data());\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   }\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderArray[i]=="1" && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]==0) {
               buffer.AppendFormatted("   if (path==\"/%s/%s\")\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("      return %d;\n",codeNumber);
               buffer.AppendFormatted("   if (path.Index(\"/%s/%s\")!=-1)\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("      return -2;\n");
               buffer.AppendFormatted("   if (path==\"gAnalyzer->Get%s()->Get%s()\")\n",folderName[i].Data(),valueName[i][j].Data());
               buffer.AppendFormatted("      return %d;\n",codeNumber);
               buffer.AppendFormatted("   if (path.Index(\"gAnalyzer->Get%s()->Get%s()\")!=-1)\n",folderName[i].Data(),valueName[i][j].Data());
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

Bool_t ROMEBuilder::WriteAnalyzerH()
{
   int i,j,k;

   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString bankname;
   ROMEString format;
   ROMEString tmp;
   if (makeOutput) cout << "\n   Output H-File:" << endl;
   // max folder name length
   int nameLen = -1;
   int fieldLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (typeLen<(int)folderName[i].Length()+scl)
            typeLen = folderName[i].Length()+scl;
         if (nameLen<(int)folderName[i].Length())
            nameLen = folderName[i].Length();
         if (folderDataBase[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (fieldLen<(int)valueName[i][j].Length())
                  fieldLen = valueName[i][j].Length();
            }
         }
      }
   }

   // max task switch name length
   int switchLen = -1;
   ROMEString switchString;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      int index = taskHierarchyParentIndex[i];
      switchString = taskHierarchyName[i].Data();
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.Insert(0,taskHierarchyName[index].Data());
         index = taskHierarchyParentIndex[index];
      }
      if (switchLen<(int)switchString.Length()) switchLen = switchString.Length();
   }

   // File name
   hFile.SetFormatted("%sinclude/generated/%sAnalyzer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sAnalyzer_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sAnalyzer_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sAnalyzer", shortCut.Data());
   clsDescription.SetFormatted("Basic class for the %s%s. This class creates and manages all Folders, Tasks and Trees.",shortCut.Data(),mainProgName.Data());
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
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("class %s%sDAQ;\n",daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
   }

   // Folder class declaration
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         buffer.AppendFormatted("class %s%s;\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Task class declaration
   for (i=0;i<numOfTaskHierarchy;i++) {
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

   ROMEString prog;
   prog.SetFormatted("%s%s",shortCut.Data(),mainProgName.Data());
   int len1 = (int)((39-(double)prog.Length())/2+0.5);
   int len2 = (int)((39-(double)prog.Length())/2);
   buffer.AppendFormatted("const char* const %sLogo =\n", shortCut.Data());
   buffer.AppendFormatted("      \"*****************************************\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*");
   format.SetFormatted("%%%ds%%s%%%ds", len1, len2);
   buffer.AppendFormatted(format.Data(),"",prog.Data(),"");
   buffer.AppendFormatted("*\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*   generated by the ROME Environment   *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*             %s               *\\n\"\n",romeVersion.Data());
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*                                       *\\n\"\n");
   buffer.AppendFormatted("      \"*****************************************\\n\\n\";\n");


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
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("   %s%s* f%sFolder; // Handle to %s%s Folder\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %s%s* f%sFolderStorage; // Handle to %s%s Folder Storage\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("   TClonesArray* f%sFolders; // Handle to %s%s Folders\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   TClonesArray* f%sFoldersStorage; // Handle to %s%s Folders Storage\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Steering Parameter Fields
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("   // Steering Parameter Fields\n");
      buffer.AppendFormatted("   %sGlobalSteering* fGlobalSteeringParameters; // Handle to the GlobalSteering Class\n",shortCut.Data());
   }
   buffer.AppendFormatted("\n");

   // DAQ Handle
   buffer.AppendFormatted("   // DAQ Handle\n");
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("   %s%sDAQ* f%sDAQ; // Handle to the %s DAQ Class\n",daqTypeArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   %sDBAccess* fDBAccess; // Handle to the DBAccess Class\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sAnalyzer(ROMERint *app,Bool_t batch,Bool_t daemon,Bool_t nographics);\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sAnalyzer();\n",shortCut.Data());

   // Folder Getters and Setters
   buffer.AppendFormatted("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      WriteFolderGetterInclude(buffer,i);
      WriteFolderSetterInclude(buffer,i);
   }
   buffer.AppendFormatted("\n");

   // Storage
   buffer.AppendFormatted("   // Storage\n");
   buffer.AppendFormatted("   void FillObjectStorage();\n");

   // Set size
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i]!="1") {
         if (folderArray[i]!="variable")
            buffer.AppendFormatted("private:\n");
         buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",folderName[i].Data());
         if (folderArray[i]!="variable")
            buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   Int_t Get%sSize() { return f%sFolders->GetEntries(); }\n",folderName[i].Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Histos
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TH1* GetHisto(const char* pathToHisto);\n");
   buffer.AppendFormatted("   bool ResetAllHistos();\n");
   buffer.AppendFormatted("\n");

   // Graphs
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   TH1* GetGraph(const char* pathToGraph);\n");
   buffer.AppendFormatted("   bool ResetAllGraphs();\n");
   buffer.AppendFormatted("\n");

   // Tasks
   buffer.AppendFormatted("   // Tasks\n");
   buffer.AppendFormatted("   void InitTasks();\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      buffer.AppendFormatted("   %sT%s* Get%s%sTask() { return ((%sT%s*)GetTaskObjectAt(%d)); }\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("   %sT%s_Base* Get%s%sTaskBase() { return ((%sT%s_Base*)GetTaskObjectAt(%d)); }\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i]);
   }
   buffer.AppendFormatted("\n");

   // Tree Getters
   buffer.AppendFormatted("   // Trees\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   TTree* Get%sTree() { return ((ROMETree*)fTreeObjects->At(%d))->GetTree(); }\n",treeName[i].Data(),i);
      buffer.AppendFormatted("   TFile* Get%sFile() { return ((ROMETree*)fTreeObjects->At(%d))->GetFile(); }\n",treeName[i].Data(),i);
   }
   buffer.AppendFormatted("\n");

   // Database Getters
   buffer.AppendFormatted("   // Database\n");
   buffer.AppendFormatted("   ROMEDataBase* GetXMLDataBase() { return GetDataBase(\"XML\"); }\n");
   buffer.AppendFormatted("   ROMEDataBase* GetTextDataBase() { return GetDataBase(\"TEXT\"); }\n");
   buffer.AppendFormatted("   ROMEDataBase* GetODBDataBase() { return GetDataBase(\"ODB\"); }\n");
   if (sql)
      buffer.AppendFormatted("   ROMEDataBase* GetSQLDataBase() { return GetDataBase(\"SQL\"); }\n");
   for (i=0;i<numOfDB;i++) {
      buffer.AppendFormatted("   ROMEDataBase* Get%sDataBase() { return GetDataBase(\"%s\"); }\n",dbName[i].Data(),dbName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   buffer.AppendFormatted("   Bool_t ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   Bool_t ReadArrayDataBaseFolders();\n");
   // GetDBPathWriteFlag
   buffer.AppendFormatted("   Bool_t GetDBPathWriteFlag(const char* path);\n");
   buffer.AppendFormatted("\n");
   // DBAccess
   buffer.AppendFormatted("   %sDBAccess* GetDBAccess() { return fDBAccess; };\n",shortCut.Data());
   buffer.AppendFormatted("\n");

/*   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("   void Write%sDataBase();\n",folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");
*/
   // Object Interpreter
   buffer.AppendFormatted("   // Object Interpreter\n");
   buffer.AppendFormatted("   Int_t       GetObjectInterpreterCode(const char* objectPath);\n");
   buffer.AppendFormatted("   Int_t       GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue);\n");
   buffer.AppendFormatted("   Double_t    GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue);\n");
   buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer);\n");
   buffer.AppendFormatted("\n");

   // Steering
   if (numOfSteering[numOfTask]>0) {
      buffer.AppendFormatted("   // Steering Parameter Methodes\n");
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; }\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // DAQ Access Methods
   buffer.AppendFormatted("   // Deprecated DAQ Access Methods\n");
   if (numOfEvent>0)
      buffer.AppendFormatted("   %sMidasDAQ* GetMidas() { return GetMidasDAQ(); }\n",shortCut.Data());
   if (numOfTree>0)
      buffer.AppendFormatted("   %sRomeDAQ* GetRome() { return GetRomeDAQ(); }\n",shortCut.Data());
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("   // %s DAQ Access Methods\n",daqNameArray->At(i).Data());
      buffer.AppendFormatted("   Bool_t Is%sDAQ() { return f%sDAQ!=NULL; }\n",daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
      buffer.AppendFormatted("   %s%sDAQ* Get%sDAQ();\n",daqTypeArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
      buffer.AppendFormatted("   void     Set%sDAQ(%s%sDAQ* handle) { f%sDAQ = handle; }\n",daqNameArray->At(i).Data(),daqTypeArray->At(i).Data(),daqNameArray->At(i).Data(),daqNameArray->At(i).Data());
   }

   // Folder dump and load
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t   DumpFolders(const char* filename, Bool_t only_database = kFALSE);\n");
   buffer.AppendFormatted("   Bool_t   LoadFolders(const char* filename, Bool_t only_database = kFALSE);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t   ShowConfigurationFile();\n");
   // Config to form save
   for (i=0;i<mainParGroup->GetNumberOfSubGroups();i++) {
      buffer.AppendFormatted("   Bool_t   Save%s(%sConfigToForm *dialog);\n",mainParGroup->GetSubGroupAt(i)->GetGroupName().Data(),shortCut.Data());
      if (mainParGroup->GetSubGroupAt(i)->GetGroupName()=="Common") {
         for (j=0;j<mainParGroup->GetSubGroupAt(i)->GetNumberOfSubGroups();j++) {
            buffer.AppendFormatted("   Bool_t   Save%s(%sConfigToForm *dialog);\n",mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data(),shortCut.Data());
            if (mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName()=="Folders") {
               for (k=0;k<mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetNumberOfSubGroups();k++)
                  buffer.AppendFormatted("   Bool_t   Save%s(%sConfigToForm *dialog);\n",mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetSubGroupAt(k)->GetGroupName().Data(),shortCut.Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");


   buffer.AppendFormatted("   %sWindow *GetWindow() { return (%sWindow*)fWindow; }\n",shortCut.Data(),shortCut.Data());
   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   Bool_t ReadUserParameter(const char* opt, const char *value, Int_t& i);\n");
   buffer.AppendFormatted("   void   UserParameterUsage();\n");
   buffer.AppendFormatted("   void   startSplashScreen();\n");
   buffer.AppendFormatted("   void   consoleStartScreen();\n");
   buffer.AppendFormatted("   Bool_t ConnectSocketClientNetFolder();\n");
   buffer.AppendFormatted("   void   StartNetFolderServer();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ClassDef(%sAnalyzer,0);\n",shortCut.Data());

   // Footer
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("extern %sAnalyzer *gAnalyzer;  // global Analyzer Handle\n\n",shortCut.Data());

   buffer.AppendFormatted("#endif   // %sAnalyzer_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteFillObjectStorageObject(ROMEString &buffer,const char *objectPointer,const char *objectStoragePointer,const char *objectActivePointer,bool bypass)
{
   buffer.AppendFormatted("   for(i=0;i<kMaxSocketClients;i++) {\n");
   buffer.AppendFormatted("      if(!fNetFolderServer->GetAcceptedSockets(i) || (!%s && !fNetFolderServer->IsCopyAll())) continue;\n",objectActivePointer);
   if (bypass) {
      buffer.AppendFormatted("      bypassOld = %s->CanBypassStreamer();\n",objectPointer);
      buffer.AppendFormatted("      bypassStorageOld = %s->CanBypassStreamer();\n",objectStoragePointer);
      buffer.AppendFormatted("      %s->BypassStreamer(kTRUE);\n",objectPointer);
      buffer.AppendFormatted("      %s->BypassStreamer(kTRUE);\n",objectStoragePointer);
   }
   buffer.AppendFormatted("      CopyTObjectWithStreamer(buffer,%s,%s);\n",objectPointer,objectStoragePointer);
   if (bypass) {
      buffer.AppendFormatted("      %s->BypassStreamer(bypassOld);\n",objectPointer);
      buffer.AppendFormatted("      %s->BypassStreamer(bypassStorageOld);\n",objectStoragePointer);
   }
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   return true;
}
Bool_t ROMEBuilder::WriteWindowCpp()
{
   Int_t i, j;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString bufferTemp;
   ROMEString format;
   ROMEString menu_title;
   ROMEString buf;

   ROMEString clsDescription;
   ROMEString clsName;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sWindow.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sWindow", shortCut.Data());
   clsDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.", shortCut.Data(), mainProgName.Data());
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
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("#include \"ArgusTextDialog.h\"\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"ROMECompositeFrame.h\"\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sWindow)\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("%sWindow::%sWindow() : ArgusWindow()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   if (numOfTab>0)
      buffer.AppendFormatted("   fTabObjects = new TObjArray(%d);\n",numOfTab);
   else
      buffer.AppendFormatted("   fTabObjects = NULL;\n");
   buffer.AppendFormatted("   fStatusBarSwitch = kTRUE;\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   fTabSwitches.%s%s = kTRUE;\n",tabName[i].Data(),tabSuffix[i].Data());
   }
   buffer.AppendFormatted("   ConstructTabs();\n"); // this is necessary to handle config parameters
   buffer.AppendFormatted("}\n\n");

   buffer.AppendFormatted("%sWindow::%sWindow(const TGWindow* p,Bool_t tabWindow) : ArgusWindow(p,tabWindow)\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   fStatusBarSwitch = kTRUE;\n");
   buffer.AppendFormatted("\n");
   if (numOfTab>0)
      buffer.AppendFormatted("   fTabObjects = new TObjArray(%d);\n",numOfTab);
   else
      buffer.AppendFormatted("   fTabObjects = NULL;\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   fTabSwitches.%s%s = kTRUE;\n",tabName[i].Data(),tabSuffix[i].Data());

      if (tabHeredity[i].Length()>0) {
         if (numOfMenu[tabHeredityIndex[i]] > 0) {
            buffer.AppendFormatted("   for (i=%d;i<%d+%d;i++)\n", numOfMenu[i],numOfMenu[i],numOfMenu[tabHeredityIndex[i]]);
            buffer.AppendFormatted("      f%sMenu[i] = NULL;\n", tabName[i].Data());
         }
      }
      if (numOfMenu[i] > 0) {
         buffer.AppendFormatted("   for (i=0;i<%d;i++)\n", numOfMenu[i]);
         buffer.AppendFormatted("      f%sMenu[i] = NULL;\n", tabName[i].Data());
      }
   }

   buffer.AppendFormatted("   ConstructTabs();\n");
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");

   // Clone Window
   buffer.AppendFormatted("%sWindow* %sWindow::CreateSubWindow()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   %sWindow* window = new %sWindow(gClient->GetRoot(),kFALSE);\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("   window->SetStatusBarSwitch(fStatusBarSwitch);\n");
   buffer.AppendFormatted("   window->SetControllerActive(fControllerActive);\n");
   buffer.AppendFormatted("   window->SetWindowScale(fWindowScale);\n");
   buffer.AppendFormatted("   window->ClearEventHandlingRequest();\n");
   buffer.AppendFormatted("   window->SetCurrentEvent(fCurrentEvent);\n");
   buffer.AppendFormatted("   window->SetCurrentRun(fCurrentRun);\n");
   buffer.AppendFormatted("\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   window->GetTabSwitches()->%s%s = kFALSE;\n", tabName[i].Data(), tabSuffix[i].Data());
   }
   buffer.AppendFormatted("   return window;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // CreateTabs
   buffer.AppendFormatted("Bool_t %sWindow::CreateTabs()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i = 0; i < numOfTab; i++) {
      recursiveTabDepth = 0;
      if (!AddTab(buffer, i))
         return kFALSE;
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ProcessMessage
   buffer.AppendFormatted("Bool_t %sWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t /*param2*/)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   // Process messages coming from widgets associated with the dialog.  \n");
   buffer.AppendFormatted("   switch (GET_MSG(msg)) {\n");
   buffer.AppendFormatted("   case kC_COMMAND:    \n");
   buffer.AppendFormatted("      switch (GET_SUBMSG(msg)) {\n");
   buffer.AppendFormatted("      case kCM_MENU:\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         if (fCurrentTabID == f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data());
      if (tabHeredity[i].Length()>0) {
         if (numOfMenu[tabHeredityIndex[i]] > 0) {
            buffer.AppendFormatted("            for (i=%d;i<%d+%d;i++) {\n", numOfMenu[i],numOfMenu[i],numOfMenu[tabHeredityIndex[i]]);
            buffer.AppendFormatted("               if (f%sMenu[i]->GetEntry(param1)!=0)\n", tabName[i].Data());
            buffer.AppendFormatted("                  f%s%sTab->BaseMenuClicked(f%sMenu[i],param1);\n", tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data());
            buffer.AppendFormatted("            }\n");
         }
      }
      if (tabObjectDisplay[i]) {
         buffer.AppendFormatted("            f%s%sTab->BaseMenuClicked(NULL,param1);\n", tabName[i].Data(), tabSuffix[i].Data());
      }
      else if (numOfMenu[i] > 0) {
         buffer.AppendFormatted("            for (i=0;i<%d;i++) {\n", numOfMenu[i]);
         buffer.AppendFormatted("               if (f%sMenu[i]->GetEntry(param1)!=0)\n", tabName[i].Data());
         buffer.AppendFormatted("                  f%s%sTab->BaseMenuClicked(f%sMenu[i],param1);\n", tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data());
         buffer.AppendFormatted("            }\n");
      }
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         switch (param1) {\n");
   buffer.AppendFormatted("         case M_FILE_NEW_WINDOW:\n");
   buffer.AppendFormatted("            {\n");
   buffer.AppendFormatted("            %sWindow* newWindow = CreateSubWindow();\n",shortCut.Data());
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("            if (fCurrentTabID == f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data());
      int index = i;
      buffer.AppendFormatted("               f%s%sTab->SetRegisteringActive(kFALSE);\n", tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("               newWindow->SetWindowName(\"Argus - %s\");\n",tabTitle[i].Data());
      buffer.AppendFormatted("               newWindow->SetWindowId(fSubWindows->GetEntriesFast());\n");
      buffer.AppendFormatted("               newWindow->Get%s%sTab()->SetTabActive(kTRUE);\n", tabName[index].Data(), tabSuffix[index].Data());
      while (index!=-1) {
         buffer.AppendFormatted("               newWindow->GetTabSwitches()->%s%s = kTRUE;\n", tabName[index].Data(), tabSuffix[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("            }\n");
   }
   buffer.AppendFormatted("            newWindow->Start();\n");
   buffer.AppendFormatted("            SetSubWindowRunningAt(fSubWindows->GetEntriesFast(),kTRUE);\n");
   buffer.AppendFormatted("            fSubWindows->Add(newWindow);\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         case M_FILE_EXIT:\n");
   buffer.AppendFormatted("            if (fTabWindow)\n");
   buffer.AppendFormatted("               CloseWindow();\n");
   buffer.AppendFormatted("            else {\n");
   buffer.AppendFormatted("               ROMEString str;\n");
   buffer.AppendFormatted("               gAnalyzer->GetWindow()->SetSubWindowRunningAt(fWindowId,kFALSE);\n");
   buffer.AppendFormatted("               gAnalyzer->GetWindow()->SetSubWindowTimeStringAt(fWindowId,GetTimeStatisticsString(str));\n");
   buffer.AppendFormatted("               TGMainFrame::CloseWindow();\n");
   buffer.AppendFormatted("               ArgusTab* tab = gAnalyzer->GetWindow()->GetTabObjectAt(GetActiveTabObjectIndex());\n");
   buffer.AppendFormatted("               tab->SetRegisteringActive(true);\n");
   buffer.AppendFormatted("               if (!tab->IsTabActive())\n");
   buffer.AppendFormatted("                  tab->UnRegisterObjects();\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            break;\n");
   buffer.AppendFormatted("         case M_FILE_CONTROLLER:\n");
   buffer.AppendFormatted("            if (!fControllerActive) {\n");
   buffer.AppendFormatted("               fController = new ArgusAnalyzerController(gClient->GetRoot(),this,100,100,fControllerNetFolder);\n");
   buffer.AppendFormatted("               fControllerActive = kTRUE;\n");
   buffer.AppendFormatted("            }\n");
   buffer.AppendFormatted("            break;\n");
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("         case M_FILE_CONNECT_%s:\n", netFolderName[i].Data());
      buffer.AppendFormatted("            gAnalyzer->ConnectNetFolder(\"%s\");\n", netFolderName[i].Data());
      buffer.AppendFormatted("            break;\n");
   }
   buffer.AppendFormatted("         }\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_BUTTON:\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      case kCM_LISTBOX:\n");
   buffer.AppendFormatted("         break;      \n");
   buffer.AppendFormatted("      case kCM_TAB:\n");

   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID == f%s%sTabID && param1 != f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("            f%s%sTab->BaseTabUnSelected();\n", tabName[i].Data(), tabSuffix[i].Data());
      if (tabHeredity[i].Length()>0) {
         for (j = 0; j < numOfMenu[tabHeredityIndex[i]]; j++) {
            menu_title = menuTitle[tabHeredityIndex[i]][j];
            menu_title.ReplaceAll("&", "");
            buffer.AppendFormatted("            delete fMenuBar->RemovePopup(\"%s\");\n", menu_title.Data());
         }
      }
      for (j = 0; j < numOfMenu[i]; j++) {
         menu_title = menuTitle[i][j];
         menu_title.ReplaceAll("&", "");
         buffer.AppendFormatted("            delete fMenuBar->RemovePopup(\"%s\");\n", menu_title.Data());
      }
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID != f%s%sTabID && param1 == f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("            f%s%sTab->BaseTabSelected();\n", tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (param1 == f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data());
      if (tabHeredity[i].Length()>0) {
         for (j = 0; j < numOfMenu[tabHeredityIndex[i]]; j++) {
            buffer.AppendFormatted("            f%sMenu[%d] = new TGPopupMenu(fClient->GetRoot());\n", tabName[i].Data(), numOfMenu[i]+j);
            buffer.AppendFormatted("            f%sMenu[%d]->Associate(this);\n", tabName[i].Data(), numOfMenu[i]+j);
         }
         for (j = 0; j < numOfMenu[tabHeredityIndex[i]]; j++) {
            if (menuDepth[tabHeredityIndex[i]][j] == 1) {
               if (!AddMenuItems(buffer, tabHeredityIndex[i], j,i,j+numOfMenu[i],numOfMenu[i]))
                  return kFALSE;
               buffer.AppendFormatted("            fMenuBar->AddPopup(\"%s\", f%sMenu[%d],\n", menuTitle[tabHeredityIndex[i]][j].Data(), tabName[i].Data(), numOfMenu[i]+j);
               buffer.AppendFormatted("                               new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
            }
         }
      }
      for (j = 0; j < numOfMenu[i]; j++) {
         buffer.AppendFormatted("            f%sMenu[%d] = new TGPopupMenu(fClient->GetRoot());\n", tabName[i].Data(), j);
         buffer.AppendFormatted("            f%sMenu[%d]->Associate(this);\n", tabName[i].Data(), j);
      }
      for (j = 0; j < numOfMenu[i]; j++) {
         if (menuDepth[i][j] == 1) {
            if (!AddMenuItems(buffer, i, j,i,j,0))
               return kFALSE;
            buffer.AppendFormatted("            fMenuBar->AddPopup(\"%s\", f%sMenu[%d],\n", menuTitle[i][j].Data(), tabName[i].Data(), j);
            buffer.AppendFormatted("                               new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));\n");
         }
      }
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         if (");
      Int_t index = i;
      do {
         buffer.AppendFormatted(" param1 != f%s%sTabID &&", tabName[index].Data(), tabSuffix[i].Data());
         index = tabParentIndex[index];
      } while (index != -1);
      buffer.Remove(buffer.Length() - 2);       // remove the last "&&"
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID == f%s%sTabID && param1 != f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("            f%s%sTab->TabUnSelected();\n", tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID != f%s%sTabID && param1 == f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("            fCurrentTabID = param1;\n");
      if (tabNumOfChildren[i])
         buffer.AppendFormatted("            ProcessMessage(MK_MSG(kC_COMMAND, kCM_TAB), %d + f%s%sTabSubTab->GetCurrent(), 0);;\n", 1000 * i, tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("            f%s%sTab->TabSelected();\n", tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         MapSubwindows();\n");
   buffer.AppendFormatted("         Resize(fWidth-1,fHeight-1);\n");
   buffer.AppendFormatted("         Resize(fWidth+1,fHeight+1);\n");
   buffer.AppendFormatted("         MapWindow();\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      break;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // GetMenuHandle
   buffer.AppendFormatted("TGPopupMenu* %sWindow::GetMenuHandle(const char* menuName)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   if (fCurrentTabID == f%s%sTabID) {\n", tabName[i].Data(), tabSuffix[i].Data());
      if (tabHeredity[i].Length()>0) {
         for (j = 0; j < numOfMenu[tabHeredityIndex[i]]; j++) {
            buffer.AppendFormatted("      if (!strcmp(menuName,\"%s\"))\n", menuTitle[tabHeredityIndex[i]][j].Data());
            buffer.AppendFormatted("         return f%sMenu[%d];\n", tabName[i].Data(), j+numOfMenu[i]);
         }
      }
      for (j = 0; j < numOfMenu[i]; j++) {
         buffer.AppendFormatted("      if (!strcmp(menuName,\"%s\"))\n", menuTitle[i][j].Data());
         buffer.AppendFormatted("         return f%sMenu[%d];\n", tabName[i].Data(), j);
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   ROMEPrint::Error(\"Error: A menu (%%s) was not found.\\n\", menuName);\n");
   buffer.AppendFormatted("   return NULL;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddMenuNetFolder
   buffer.AppendFormatted("Bool_t %sWindow::AddMenuNetFolder(TGPopupMenu* menu)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   if (gAnalyzer->GetNumberOfNetFolders() <= 0 )\n");
   buffer.AppendFormatted("      return kFALSE;\n");
   buffer.AppendFormatted("   Bool_t active = kFALSE;\n");
   for (i = 0; i < numOfNetFolder; i++) {
      buffer.AppendFormatted("   if (gAnalyzer->GetNetFolderActive(%d)) {\n", i);
      buffer.AppendFormatted("      menu->AddEntry(\"%s\", M_FILE_CONNECT_%s);\n", netFolderName[i].Data(), netFolderName[i].Data());
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

   // Event Handler
   buffer.AppendFormatted("// Event Handler\n");
   buffer.AppendFormatted("void %sWindow::TriggerEventHandler()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   SetStatus(0,\"\",0);\n");
   buffer.AppendFormatted("   fWatchAll.Start(false);\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   if (fTabSwitches.%s%s && fCurrentTabID == f%s%sTabID)\n", tabName[i].Data(),tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("      f%s%sTab->ArgusEventHandler();\n", tabName[i].Data(), tabSuffix[i].Data());
   }
   buffer.AppendFormatted("   for (i=0;i<fSubWindows->GetEntriesFast();i++) {\n");
   buffer.AppendFormatted("       if (IsSubWindowRunningAt(i))\n");
   buffer.AppendFormatted("          ((%sWindow*)fSubWindows->At(i))->TriggerEventHandler();\n",shortCut.Data());
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   fWatchAll.Stop();\n");
   buffer.AppendFormatted("   SetStatus(2,\"\",0);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

Bool_t ROMEBuilder::WriteWindow2Cpp()
{
   Int_t i;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString bufferTemp;
   ROMEString format;
   ROMEString menu_title;
   ROMEString buf;

   ROMEString clsDescription;
   ROMEString clsName;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sWindow2.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sWindow", shortCut.Data());
   clsDescription.SetFormatted("Main window class for the %s%s. This class creates main window and manages Tabs.", shortCut.Data(), mainProgName.Data());
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
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"tabs/%sT%s.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sWindow::ConstructTabs()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   f%s%sTab = new %sT%s(this);\n", tabName[i].Data(), tabSuffix[i].Data(), shortCut.Data(), tabName[i].Data());
      buffer.AppendFormatted("   AddTab(f%s%sTab);\n",tabName[i].Data(),tabSuffix[i].Data());
      buffer.AppendFormatted("   f%s%sTabID = -1;\n", tabName[i].Data(),tabSuffix[i].Data());
   }
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

Bool_t ROMEBuilder::WriteWindowH()
{
   Int_t i, j, k;
   ROMEString hFile;
   ROMEString buffer;
   ROMEString buf;

   ROMEString clsDescription;
   ROMEString clsName;

   ROMEString format;

   Int_t nameLen = -1;
   Int_t typeLen = -1;
   Int_t scl = shortCut.Length();
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[i].Length() + strlen("f000Tab")));
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[i].Length() + scl + strlen("tT")));
      nameLen = TMath::Max(nameLen, static_cast<Int_t>(tabName[i].Length() + strlen("Get000Tab()")));
      typeLen = TMath::Max(typeLen, static_cast<Int_t>(tabName[i].Length() + scl + strlen("T*_Base")));
   }
   typeLen = TMath::Max(typeLen, static_cast<Int_t>(strlen("TGCompositeFrame*")));

   // max tab switch name length
   Int_t switchLen = -1;
   ROMEString switchString;
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      Int_t index = tabParentIndex[i];
      switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      if (switchLen < static_cast<Int_t>(switchString.Length()))
         switchLen = switchString.Length();
   }

   // File name
   hFile.SetFormatted("%sinclude/generated/%sWindow.h", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sWINDOW_H\n", shortCut.Data());
   buffer.AppendFormatted("#define %sWINDOW_H\n\n", shortCut.Data());
   clsName.SetFormatted("%sWindow", shortCut.Data());
   clsDescription.SetFormatted("Main window class for the %s%s.", shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include \"ArgusWindow.h\"\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("class %sT%s_Base;\n", shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Tab Switches Structure
   buffer.AppendFormatted("// Tab Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");

   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   Bool_t %s%s;  //! %s%s Tab\n", tabName[i].Data(),tabSuffix[i].Data(), tabName[i].Data(),tabSuffix[i].Data());
   }
   buffer.AppendFormatted("} TabSwitches;\n");
   buffer.AppendFormatted("\n");

   // Class
   buffer.AppendFormatted("class %sWindow : public ArgusWindow {  \n", shortCut.Data());
   // Enumeration
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   enum CommandIdentifiers{\n");
   buffer.AppendFormatted("      M_FILE_CONNECT_ROOT = 10,\n");
   for (i = 0; i < numOfNetFolder; i++)
      buffer.AppendFormatted("      M_FILE_CONNECT_%s,\n", netFolderName[i].Data());
   buffer.Remove(buffer.Length() - 2, 1);       // remove the last ','
   buffer.AppendFormatted("   };\n");

   buffer.AppendFormatted("   enum MenuEnumeration {\n");
   buffer.AppendFormatted("      M_ROOT = 10000,\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      for (j = 0; j < numOfMenu[i]; j++) {
         for (k = 0; k < numOfMenuItem[i][j]; k++) {
            if (menuItemEnumName[i][j][k].Length() > 0)
               buffer.AppendFormatted("      %s,\n", menuItemEnumName[i][j][k].Data());
         }
      }
   }

   buffer.Remove(buffer.Length() - 2, 1);       // remove the last ','
   buffer.AppendFormatted("   };\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("private:\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      if (tabNumOfChildren[i])
         buffer.AppendFormatted("   TGTab               *f%s%sTabSubTab;\n", tabName[i].Data(), tabSuffix[i].Data());
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      int numMenu = numOfMenu[i];
      if (tabHeredity[i].Length()>0)
         numMenu += numOfMenu[tabHeredityIndex[i]];
      if (numMenu > 0)
         buffer.AppendFormatted("   TGPopupMenu         *f%sMenu[%d];\n", tabName[i].Data(), numMenu);
      buffer.AppendFormatted("   Int_t               f%s%sTabID;\n", tabName[i].Data(), tabSuffix[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches fTabSwitches;               //! Tab Switches\n");
   buffer.AppendFormatted("\n");

   // Tab Fields
   buffer.AppendFormatted("   // Tab Fields\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      // cast to int to avoid inconsistency between format and argument
      buffer.AppendFormatted("   %sT%s_Base* f%s%sTab;  //! Handle to %s%s Tab\n", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("   TGCompositeFrame* t%sT%s;  // Container of %s Tab\n", shortCut.Data(), tabName[i].Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Method
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sWindow();\n", shortCut.Data());
   buffer.AppendFormatted("   %sWindow(const TGWindow *p,Bool_t tabWindow=kTRUE);\n", shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sWindow() {}\n", shortCut.Data());
   buffer.AppendFormatted("   %sWindow* CreateSubWindow();\n", shortCut.Data());
   buffer.AppendFormatted("   void   ConstructTabs();\n");
   buffer.AppendFormatted("   Bool_t CreateTabs();\n");
   buffer.AppendFormatted("   Bool_t AddMenuNetFolder(TGPopupMenu* menu);\n");
   buffer.AppendFormatted("\n");

   // Tab Switches
   buffer.AppendFormatted("   // Tab Switches\n");
   buffer.AppendFormatted("   TabSwitches* GetTabSwitches() { return &fTabSwitches; }\n");
   buffer.AppendFormatted("\n");
   // Tab Getters
   buffer.AppendFormatted("   // Tabs\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      // cast to avoid inconsistency between format and arguments.
      buffer.AppendFormatted("   %sT%s_Base* Get%s%sTab() { return f%s%sTab; }\n", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), tabSuffix[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
   }
   buffer.AppendFormatted("   Bool_t       ProcessMessage(Long_t msg, Long_t param1, Long_t param2);\n");
   buffer.AppendFormatted("   TGPopupMenu* GetMenuHandle(const char* menuName);\n");
   buffer.AppendFormatted("\n");

   // Event Handler
   buffer.AppendFormatted("   // Event Handler\n");
   buffer.AppendFormatted("   void TriggerEventHandler();\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sWindow,0)\n", shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

Bool_t ROMEBuilder::WriteDBAccessCpp()
{
   int i,j,num;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString str;

   ROMEString clsDescription;
   ROMEString clsName;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sDBAccess.cpp", outDir.Data(), shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sDBAccess", shortCut.Data());
   clsDescription.SetFormatted("Handles the database access for the %s%s.", shortCut.Data(), mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   for (i=0;i<numOfFolder;i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
      }
   }
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sDBAccess.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sDBAccess)\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("%sDBAccess::%sDBAccess() : TObject()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i,j,k;\n");
   for (i=0;i<numOfFolder;i++) {
      num = 0;
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if ( valueDimension[i][j]>1 )
               continue;
            num++;
         }
      }
      buffer.AppendFormatted("   fNumberOfValues[%d] = %d;\n",i,num);
   }
   buffer.AppendFormatted("   for (i=0;i<%d;i++) {\n",numOfFolder);
   buffer.AppendFormatted("      fDBName[i] = new ROMEString[fNumberOfValues[i]];\n");
   buffer.AppendFormatted("      fDBPath[i] = new ROMEString[fNumberOfValues[i]];\n");
   buffer.AppendFormatted("      fDBCode[i] = new TArrayI*[fNumberOfValues[i]];\n");
   buffer.AppendFormatted("      for (j=0;j<fNumberOfValues[i];j++) {\n");
   buffer.AppendFormatted("         fDBName[i][j] = \"\";\n");
   buffer.AppendFormatted("         fDBPath[i][j] = \"\";\n");
   buffer.AppendFormatted("         fDBCode[i][j] = new TArrayI(%d);\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("         for (k=0;k<%d;k++)\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("            fDBCode[i][j]->AddAt(-1,k);\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("%sDBAccess::~%sDBAccess()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i,j;\n");
   buffer.AppendFormatted("   for (i=0;i<%d;i++) {\n",numOfFolder);
   buffer.AppendFormatted("      for (j=0;j<fNumberOfValues[i];j++) {\n");
   buffer.AppendFormatted("         SafeDelete(fDBCode[i][j]);\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      SafeDelete(fDBName[i]);\n");
   buffer.AppendFormatted("      SafeDelete(fDBPath[i]);\n");
   buffer.AppendFormatted("      SafeDelete(fDBCode[i]);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("const char* %sDBAccess::GetDBNameAt(Int_t folderIndex,Int_t valueIndex) {\n",shortCut.Data());
   buffer.AppendFormatted("   if (fDBName[folderIndex][valueIndex]==\"\") {\n");
   buffer.AppendFormatted("      switch (folderIndex) {\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("         case %d:\n",i);
         buffer.AppendFormatted("         {\n");
         buffer.AppendFormatted("            switch (valueIndex) {\n");
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("               case %d:\n",j);
            buffer.AppendFormatted("                  return \"%s\";\n",valueDBName[i][j].Data());
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
   buffer.AppendFormatted("      return fDBName[folderIndex][valueIndex].Data();\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("const char* %sDBAccess::GetDBPathAt(Int_t folderIndex,Int_t valueIndex,ROMEString& path) {\n",shortCut.Data());
   buffer.AppendFormatted("   if (fDBPath[folderIndex][valueIndex]==\"\") {\n");
   buffer.AppendFormatted("      switch (folderIndex) {\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("         case %d:\n",i);
         buffer.AppendFormatted("         {\n");
         buffer.AppendFormatted("            switch (valueIndex) {\n");
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("               case %d:\n",j);
            buffer.AppendFormatted("                  path.SetFormatted(%s);\n",valueDBPath[i][j].Data());
            buffer.AppendFormatted("                  return path.Data();\n");
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
   buffer.AppendFormatted("   WarningSuppression(path.Length());\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("const char* %sDBAccess::GetDBPathOriginalAt(Int_t folderIndex,Int_t valueIndex) {\n",shortCut.Data());
   buffer.AppendFormatted("   if (fDBPath[folderIndex][valueIndex]==\"\") {\n");
   buffer.AppendFormatted("      switch (folderIndex) {\n");
   for (i=0;i<numOfFolder;i++) {
      if (folderUsed[i] && folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("         case %d:\n",i);
         buffer.AppendFormatted("         {\n");
         buffer.AppendFormatted("            switch (valueIndex) {\n");
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
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

   buffer.AppendFormatted("Int_t %sDBAccess::GetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   return fDBCode[folderIndex][valueIndex]->GetSize();\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("Int_t %sDBAccess::GetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   return fDBCode[folderIndex][valueIndex]->At(i);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sDBAccess::SetDBNameAt(Int_t folderIndex,Int_t valueIndex,const char* name)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBName[folderIndex][valueIndex] = name;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sDBAccess::SetDBPathAt(Int_t folderIndex,Int_t valueIndex,const char* path)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBPath[folderIndex][valueIndex] = path;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sDBAccess::SetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t num)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBCode[folderIndex][valueIndex]->Set(num);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sDBAccess::SetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i,Int_t code)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fDBCode[folderIndex][valueIndex]->AddAt(code,i);\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ReadDataBaseFolders
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("Bool_t %sDBAccess::Read%s() {\n",shortCut.Data(),folderName[i].Data());
         WriteReadDataBaseFolder(buffer,i,folderArray[i]=="1" ? 1 : 2);
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }


   // Write File
   WriteFile(cppFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

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
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
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
   buffer.AppendFormatted("   ROMEString *fDBName[%d];         //! Name of the database from which to read the field\n",TMath::Max(numOfFolder,1));
   buffer.AppendFormatted("   ROMEString *fDBPath[%d];         //! Database path to the value of the field\n",TMath::Max(numOfFolder,1));
   buffer.AppendFormatted("   TArrayI   **fDBCode[%d];         //! Object Interpreter codes for database path to the value of the field\n",TMath::Max(numOfFolder,1));
   buffer.AppendFormatted("   Int_t       fNumberOfValues[%d]; //! Number of values in folder\n",TMath::Max(numOfFolder,1));

   buffer.AppendFormatted("\n");

   // Method
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sDBAccess();\n", shortCut.Data());
   buffer.AppendFormatted("   ~%sDBAccess();\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   const char* GetDBNameAt(Int_t folderIndex,Int_t valueIndex);\n");
   buffer.AppendFormatted("   const char* GetDBPathAt(Int_t folderIndex,Int_t valueIndex,ROMEString& path);\n");
   buffer.AppendFormatted("   const char* GetDBPathOriginalAt(Int_t folderIndex,Int_t valueIndex);\n");
   buffer.AppendFormatted("   Int_t       GetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex);\n");
   buffer.AppendFormatted("   Int_t       GetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void SetDBNameAt(Int_t folderIndex,Int_t valueIndex,const char* name);\n");
   buffer.AppendFormatted("   void SetDBPathAt(Int_t folderIndex,Int_t valueIndex,const char* path);\n");
   buffer.AppendFormatted("   void SetDBNumberOfCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t num);\n");
   buffer.AppendFormatted("   void SetDBCodeAt(Int_t folderIndex,Int_t valueIndex,Int_t i,Int_t code);\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i])
         buffer.AppendFormatted("   Bool_t Read%s();\n",folderName[i].Data());
   }
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sDBAccess,0)\n", shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

Bool_t ROMEBuilder::AddTab(ROMEString &buffer, Int_t &i)
{
   Int_t j,k;
   ROMEString parentt;
   ROMEString format;
   Int_t depth;
   ROMEString blank;

   if (!tabUsed[i])
      return kTRUE;

   for (depth = 0; depth < recursiveTabDepth; depth++) blank += "   ";

   if (tabParentIndex[i] == -1)
      parentt = "fTab";
   else
      parentt.SetFormatted("f%s%sTabSubTab", tabName[tabParentIndex[i]].Data(), tabSuffix[tabParentIndex[i]].Data());

   buffer.AppendFormatted("%s   if (fTabSwitches.%s%s) {\n",blank.Data(), tabName[i].Data(),tabSuffix[i].Data());
   buffer.AppendFormatted("%s      if (fTabWindow) {\n",blank.Data());
   buffer.AppendFormatted("%s         t%sT%s = %s->AddTab(\"%s\");\n",blank.Data(), shortCut.Data(), tabName[i].Data(), parentt.Data(), tabTitle[i].Data());
   buffer.AppendFormatted("%s         f%s%sTab->ReparentWindow(t%sT%s, 60, 20);\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(), shortCut.Data(), tabName[i].Data());
   if (!tabNumOfChildren[i]) {
      buffer.AppendFormatted("%s      }\n",blank.Data());
      buffer.AppendFormatted("%s      else\n",blank.Data());
      buffer.AppendFormatted("%s         f%s%sTab->ReparentWindow(fMainFrame, 60, 20);\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data());
   }
   else
      blank += "   ";
   buffer.AppendFormatted("%s      f%s%sTab->ArgusInit();\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data());
   buffer.AppendFormatted("%s      f%s%sTab->SetTitle(\"%s\");\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(),tabTitle[i].Data());
   buffer.AppendFormatted("%s      f%s%sTab->SetName(\"%s\");\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(),tabName[i].Data());
   if (!tabNumOfChildren[i]) {
      buffer.AppendFormatted("%s      if (fTabWindow) {\n",blank.Data());
   }
   else
      blank = blank(0,blank.Length()-3);
   if (!tabNumOfChildren[i]) {
      buffer.AppendFormatted("%s         t%sT%s->AddFrame(f%s%sTab,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));\n",blank.Data(), shortCut.Data(), tabName[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
   }
   if (parentt == "fTab")
      buffer.AppendFormatted("%s         f%s%sTabID = %s->GetNumberOfTabs() - 1;\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(), parentt.Data());
   else
      buffer.AppendFormatted("%s         f%s%sTabID = %s->GetNumberOfTabs() - 1 + %d;\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(), parentt.Data(), 1000 * tabParentIndex[i]);
   if (tabNumOfChildren[i]) {
      buffer.AppendFormatted("%s         f%s%sTabSubTab = new TGTab(t%sT%s, (UInt_t)(600*GetWindowScale()), (UInt_t)(400*GetWindowScale()));\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(), shortCut.Data(), tabName[i].Data());
      buffer.AppendFormatted("%s         f%s%sTabSubTab->SetCommand(\"gAnalyzer->GetWindow()->ProcessMessage($MSG, $PARM1 + %d, $PARM2)\");\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data(), i * 1000);
   }
   buffer.AppendFormatted("%s      }\n",blank.Data());
   if (!tabNumOfChildren[i]) {
      buffer.AppendFormatted("%s      else {\n",blank.Data());
         buffer.AppendFormatted("%s         fMainFrame->AddFrame(f%s%sTab,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("%s         f%s%sTabID = 0;\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("%s         fCurrentTabID = f%s%sTabID;\n",blank.Data(), tabName[i].Data(), tabSuffix[i].Data());
      buffer.AppendFormatted("%s         return kTRUE;\n",blank.Data());
      buffer.AppendFormatted("%s      }\n",blank.Data());
   }
   recursiveTabDepth++;
   j = i;

   for (k=j;k < i + tabNumOfChildren[i];k++) {
      j++;
      if (!AddTab(buffer, j))
         return kFALSE;
   }
   recursiveTabDepth--;
   if (tabNumOfChildren[i]) {
      for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
      buffer.AppendFormatted("      if (fTabWindow)\n");
      for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
      buffer.AppendFormatted("         t%sT%s->AddFrame(f%s%sTabSubTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), tabSuffix[i].Data());
   }
   for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
   buffer.AppendFormatted("   }\n");
   for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
   buffer.AppendFormatted("   else {\n");
   for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
   buffer.AppendFormatted("      f%s%sTabID = -1;\n", tabName[i].Data(), tabSuffix[i].Data());
   j = i;
   for (k=j;k < i + tabNumOfChildren[i];k++) {
      j++;
      if (!InitTabDefault(buffer, j))
         return kFALSE;
   }
   for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
   buffer.AppendFormatted("   }\n");

   i = j;
   return kTRUE;
}
Bool_t ROMEBuilder::InitTabDefault(ROMEString &buffer, Int_t &i)
{
   Int_t j,depth;

   if (!tabUsed[i])
      return kTRUE;

   for (depth = 0; depth < recursiveTabDepth; depth++) buffer += "   ";
   buffer.AppendFormatted("      f%s%sTabID = -1;\n", tabName[i].Data(), tabSuffix[i].Data());
   j = i;
   while (j < i + tabNumOfChildren[i]) {
      j++;
      if (!InitTabDefault(buffer, j))
         return kFALSE;
   }
   i = j;

   return kTRUE;
}

Bool_t ROMEBuilder::AddMenuItems(ROMEString &buffer, Int_t i, Int_t j,Int_t iHeredity,Int_t jHeredity,Int_t jOffset)
{
   Int_t k;

   for (k = 0; k < numOfMenuItem[i][j]; k++) {
      if (menuItemTitle[i][j][k] == LINE_TITLE) {
         buffer.AppendFormatted("            f%sMenu[%d]->AddSeparator();\n", tabName[iHeredity].Data(), jHeredity);
      }
      else if (menuItemChildMenuIndex[i][j][k]) {
         if (!AddMenuItems(buffer, i, menuItemChildMenuIndex[i][j][k],iHeredity, jOffset+menuItemChildMenuIndex[i][j][k],jOffset+menuItemChildMenuIndex[i][j][k]))
            return kFALSE;
         buffer.AppendFormatted("            f%sMenu[%d]->AddPopup(\"%s\", f%sMenu[%d]);\n", tabName[iHeredity].Data(), jHeredity, menuTitle[i][menuItemChildMenuIndex[i][j][k]].Data()
                                , tabName[iHeredity].Data(), jOffset+menuItemChildMenuIndex[i][j][k]);
      }
      else {
         buffer.AppendFormatted("            f%sMenu[%d]->AddEntry(\"%s\", %s);\n", tabName[iHeredity].Data(), jHeredity, menuItemTitle[i][j][k].Data()
                                , menuItemEnumName[i][j][k].Data());
      }
   }

   return kTRUE;
}

Bool_t ROMEBuilder::WriteConfigToFormCpp() {
   int i;
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString format;
   ROMEString str;
   ROMEString path;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfigToForm.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sConfigToForm", shortCut.Data());
   clsDescription.SetFormatted("This class makes form for configuration for %s%s.",shortCut.Data(),mainProgName.Data());
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
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderUserCode[i])
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         else
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }

   for (i=0;i<numOfTask;i++) {
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
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      if (daqNameArray->At(i) == "Midas")
         buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
   }
   buffer.AppendFormatted("\nClassImp(%sConfigToForm)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfigToForm::%sConfigToForm():ROMEConfigToForm()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddConfig
   buffer.AppendFormatted("void %sConfigToForm::AddConfig(XMLToFormFrame *frame)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fCommentLevel = gAnalyzer->GetConfiguration()->GetCommentLevel();\n");
   for (i=0;i<mainParGroup->GetNumberOfSubGroups();i++)
      buffer.AppendFormatted("   Add%sFrame(frame);\n",mainParGroup->GetSubGroupAt(i)->GetGroupName().Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddHisto
   buffer.AppendFormatted("void %sConfigToForm::AddHisto(XMLToFormFrame *frame,ROMEHisto* histo)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString comment;\n");
   buffer.AppendFormatted("   ROMEStrArray entries;\n");
   for (i=0;i<histoParameters->GetEntriesFast();i++) {
      buffer.AppendFormatted("   // %s;\n",histoParameters->At(i).Data());
      if (histoParameterTypes->At(i)=="bool") {
         buffer.AppendFormatted("   if (histo->Is%s())\n",histoParameters->At(i).Data());
         buffer.AppendFormatted("      writeString = \"true\";\n");
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      writeString = \"false\";\n");
      }
      else
         buffer.AppendFormatted("   writeString.SetFormatted(\"%s\",histo->Get%s());\n",histoParameterTypes->At(i).Data(),histoParameters->At(i).Data());
      buffer.AppendFormatted("   comment = \"\";\n");
      buffer.AppendFormatted("   if (fCommentLevel >= %d)\n",ROMEConfig::kCommentLevelParam);
      path.SetFormatted("/xs:schema/xs:complexType[@name='HistogramDesc']/xs:sequence/xs:element[@name=Hist%s]/xs:annotation/xs:documentation", histoParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("      comment = \"%s\";\n",str.Data());
      buffer.AppendFormatted("   frame->AddElement(new XMLToFormElement(\"%s\",\"Hist%s\",writeString.Data(),\"\",0,&entries,comment.Data()));\n",histoParameterWidgetTypes->At(i).Data(),histoParameters->At(i).Data());
   }
   buffer.AppendFormatted("   // Accumulation;\n");
   buffer.AppendFormatted("   if (histo->isAccumulation())\n");
   buffer.AppendFormatted("      writeString = \"true\";\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      writeString = \"false\";\n");
   buffer.AppendFormatted("   comment = \"\";\n");
   buffer.AppendFormatted("   if (fCommentLevel >= %d)\n",ROMEConfig::kCommentLevelParam);
   path.SetFormatted("/xs:schema/xs:complexType[@name='HistogramDesc']/xs:sequence/xs:element[@name=HistAccumulate]/xs:annotation/xs:documentation");
   configXSD->GetPathValue(path, str, "");
   buffer.AppendFormatted("      comment = \"%s\";\n",str.Data());
   buffer.AppendFormatted("   frame->AddElement(new XMLToFormElement(\"CheckButton\",\"HistAccumulate\",writeString.Data(),\"\",0,&entries,comment.Data()));\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // AddGraph
   buffer.AppendFormatted("void %sConfigToForm::AddGraph(XMLToFormFrame *frame,ROMEGraph* graph)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString comment;\n");
   buffer.AppendFormatted("   ROMEStrArray entries;\n");
   for (i=0;i<graphParameters->GetEntriesFast();i++) {
      buffer.AppendFormatted("   // %s;\n",graphParameters->At(i).Data());
      if (graphParameterTypes->At(i)=="bool") {
         buffer.AppendFormatted("   if (graph->Is%s())\n",graphParameters->At(i).Data());
         buffer.AppendFormatted("      writeString = \"true\";\n");
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      writeString = \"false\";\n");
      }
      else
         buffer.AppendFormatted("   writeString.SetFormatted(\"%s\",graph->Get%s());\n",graphParameterTypes->At(i).Data(),graphParameters->At(i).Data());
      buffer.AppendFormatted("   comment = \"\";\n");
      buffer.AppendFormatted("   if (fCommentLevel >= %d)\n",ROMEConfig::kCommentLevelParam);
      path.SetFormatted("/xs:schema/xs:complexType[@name='GraphDesc']/xs:sequence/xs:element[@name=Graph%s]/xs:annotation/xs:documentation", graphParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("      comment = \"%s\";\n",str.Data());
      buffer.AppendFormatted("   frame->AddElement(new XMLToFormElement(\"%s\",\"Graph%s\",writeString.Data(),\"\",0,&entries,comment.Data()));\n",histoParameterWidgetTypes->At(i).Data(),graphParameters->At(i).Data());
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   WriteConfigToFormSubMethods(buffer,mainParGroup,"","",0,1);

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteConfigToFormSubMethods(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString tabPointer,ROMEString configPointer,int level,int tab)
{
   int i,j;
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";

   ROMEString newConfigPointer;
   ROMEString temp;
   ROMEString comment;
   if (parGroup->GetGroupIdentifier()=="Histogram") {
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0,temp.Last('-'));
      buffer.AppendFormatted("%sAddHisto(tempFrame[%d],%s);\n",sTab.Data(),level,temp.Data());
   }
   else if (parGroup->GetGroupIdentifier()=="Graph") {
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0,temp.Last('-'));
      buffer.AppendFormatted("%sAddGraph(tempFrame[%d],%s);\n",sTab.Data(),level,temp.Data());
   }
   else {
      for (i=0;i<parGroup->GetNumberOfParameters();i++) {
         if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
            buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),tabPointer.Data(),parGroup->GetParameterAt(i)->GetName());
            for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfWriteLines();j++) {
               buffer.AppendFormatted("%s%s\n",sTab.Data(),parGroup->GetParameterAt(i)->GetWriteLineAt(j));
            }
            if (parGroup->GetParameterAt(i)->GetNumberOfComboBoxEntries()>0)
               buffer.AppendFormatted("%sentries.RemoveAll();\n",sTab.Data());
            for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfComboBoxEntries();j++)
               buffer.AppendFormatted("%sentries.AddLast(\"%s\");\n",sTab.Data(),parGroup->GetParameterAt(i)->GetComboBoxEntryAt(j));
            comment = ProcessCommentString(parGroup->GetParameterAt(i)->GetComment(),temp).Data();
            buffer.AppendFormatted("%scomment = \"\";\n",sTab.Data());
            if (comment.Length()>0) {
               buffer.AppendFormatted("%sif (fCommentLevel >= %d)\n",sTab.Data(),parGroup->GetParameterAt(i)->GetCommentLevel());
               buffer.AppendFormatted("%s   comment = \"%s\";\n",sTab.Data(),comment.Data());
            }
            buffer.AppendFormatted("%stempFrame[%d]->AddElement(new XMLToFormElement(\"%s\",\"%s\",writeString.Data(),\"\",0,&entries,comment.Data()));\n",sTab.Data(),level,parGroup->GetParameterAt(i)->GetWidgetType().Data(),parGroup->GetParameterAt(i)->GetName());
         }
      }
   }

   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (level==0 || (level==1 && parGroup->GetGroupName()=="Common") || (level==2 && parGroup->GetGroupName()=="Folders")) {
         buffer.AppendFormatted("void %sConfigToForm::Add%sFrame(XMLToFormFrame *frame)\n",shortCut.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   int ind;\n");
         buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   bool writeFlag;\n");
         buffer.AppendFormatted("   writeFlag = true;\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString comment = \"\";\n");
         buffer.AppendFormatted("   ROMEString str = \"\";\n");
         buffer.AppendFormatted("   ROMEString tmp;\n");
         buffer.AppendFormatted("   tmp = \"\";\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString writeString;\n");
         buffer.AppendFormatted("   ROMEStrArray entries;\n");
         buffer.AppendFormatted("   XMLToFormFrame *tempFrame[100];\n");
         buffer.AppendFormatted("   int nTabs;\n");
         buffer.AppendFormatted("   nTabs=0;\n");
         buffer.AppendFormatted("   int i,ii[100];\n");
         buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   ii[0]=0;\n"); // to suppress unused warning
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   tempFrame[%d] = frame;\n",level);
      }
      else
         buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),tabPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      comment = ProcessCommentString(parGroup->GetSubGroupAt(i)->GetComment(),temp).Data();
      buffer.AppendFormatted("%scomment = \"\";\n",sTab.Data());
      if (comment.Length()>0) {
         buffer.AppendFormatted("%sif (fCommentLevel >= %d)\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetCommentLevel());
         buffer.AppendFormatted("%s   comment = \"%s\";\n",sTab.Data(),comment.Data());
      }
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1" || parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
         buffer.AppendFormatted("%stempFrame[%d]->AddSubFrame(new XMLToFormFrame(tempFrame[%d],\"%s\",\"\",true,XMLToFormFrame::kListTreeItem,true,%d,comment.Data()));\n",sTab.Data(),level,level,parGroup->GetSubGroupAt(i)->GetGroupName().Data(),i);
      newConfigPointer.SetFormatted("%sf%s->",configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (!(level==0 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Common") && !(level==1 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Folders")) {
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
            buffer.AppendFormatted("%stempFrame[%d] = tempFrame[%d]->GetSubFrameAt(%d);\n",sTab.Data(),level+1,level,i);
            WriteConfigToFormSubMethods(buffer,parGroup->GetSubGroupAt(i),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab);
         }
         else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%sfor (i=0;i<((%sConfig*)gAnalyzer->GetConfiguration())->fConfigData[0]->%sf%sArraySize;i++) {\n",sTab.Data(),shortCut.Data(),configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            buffer.AppendFormatted("%s   str.SetFormatted(\"%s %%d\",i);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            comment = ProcessCommentString(parGroup->GetSubGroupAt(i)->GetComment(),temp).Data();
            buffer.AppendFormatted("%s   comment = \"\";\n",sTab.Data());
            if (comment.Length()>0) {
               buffer.AppendFormatted("%s   if (fCommentLevel >= %d)\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetCommentLevel());
               buffer.AppendFormatted("%s      comment = \"%s\";\n",sTab.Data(),comment.Data());
            }
            buffer.AppendFormatted("%s   tempFrame[%d]->GetSubFrameAt(%d)->AddSubFrame(new XMLToFormFrame(tempFrame[%d]->GetSubFrameAt(%d),str.Data(),\"\",true,XMLToFormFrame::kListTreeItem,true,0,comment.Data()));\n",sTab.Data(),level,i,level,i);
            buffer.AppendFormatted("%s   tempFrame[%d] = tempFrame[%d]->GetSubFrameAt(%d)->GetSubFrameAt(i);\n",sTab.Data(),level+1,level,i);
            WriteConfigToFormSubMethods(buffer,parGroup->GetSubGroupAt(i),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab+1);
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
         else {
            buffer.AppendFormatted("%sfor (ii[%d]=0;ii[%d]<%s;ii[%d]++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            buffer.AppendFormatted("%s   str.SetFormatted(\"%s %%d\",ii[%d]);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            comment = ProcessCommentString(parGroup->GetSubGroupAt(i)->GetComment(),temp).Data();
            buffer.AppendFormatted("%s   comment = \"\";\n",sTab.Data());
            if (comment.Length()>0) {
               buffer.AppendFormatted("%s   if (fCommentLevel >= %d)\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetCommentLevel());
               buffer.AppendFormatted("%s      comment = \"%s\";\n",sTab.Data(),comment.Data());
            }
            buffer.AppendFormatted("%s   tempFrame[%d]->AddSubFrame(new XMLToFormFrame(tempFrame[%d],str.Data(),\"\",true,XMLToFormFrame::kListTreeItem,true,0,comment.Data()));\n",sTab.Data(),level,level);
            buffer.AppendFormatted("%s   tempFrame[%d] = tempFrame[%d]->GetSubFrameAt(ii[%d]);\n",sTab.Data(),level+1,level,parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            WriteConfigToFormSubMethods(buffer,parGroup->GetSubGroupAt(i),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab+1);
            buffer.AppendFormatted("%s}\n",sTab.Data());
         }
      }
      if (level==0 || (level==1 && parGroup->GetGroupName()=="Common") || (level==2 && parGroup->GetGroupName()=="Folders")) {
         if ((level==0 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Common") || (level==1 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Folders")) {
            for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups();j++)
               buffer.AppendFormatted("%sAdd%sFrame(tempFrame[%d]->GetSubFrameAt(%d));\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data(),level,i);
         }
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         if ((level==0 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Common") || (level==1 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Folders")) {
            WriteConfigToFormSubMethods(buffer,parGroup->GetSubGroupAt(i),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab);
         }
      }
   }

   return true;
}

Bool_t ROMEBuilder::WriteConfigToFormSave(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString pointer,ROMEString tabPointer,ROMEString configPointer,int level,int tab,ROMEString indexes)
{
   int i,j;
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";

   ROMEString newPointer;
   ROMEString newConfigPointer;
   ROMEString newIndexes;
   ROMEString temp;
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s   // %s%s\n",sTab.Data(),tabPointer.Data(),parGroup->GetParameterAt(i)->GetName());
         if (parGroup->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%s   str.SetFormatted(\"%s %%d/%s\"%s,i);\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName(),indexes.Data());
            buffer.AppendFormatted("%s   str = dialog->GetValue(str.Data());\n",sTab.Data());
         }
         else {
            if (indexes.Length()==0)
               buffer.AppendFormatted("%s   str = dialog->GetValue(\"%s%s\");\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
            else {
               buffer.AppendFormatted("%s   str.SetFormatted(\"%s%s\"%s);\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName(),indexes.Data());
               buffer.AppendFormatted("%s   str = dialog->GetValue(str.Data());\n",sTab.Data());
            }
         }
         if (!parGroup->GetParameterAt(i)->IsWriteLinesAlways()) {
            for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfWriteLines();j++) {
               buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetParameterAt(i)->GetWriteLineAt(j));
            }
            buffer.AppendFormatted("%s   if (str!=writeString) {\n",sTab.Data());
            buffer.AppendFormatted("%s      ((%sConfig*)fConfiguration)->fConfigData[0]->%sf%sModified = true;\n",sTab.Data(),shortCut.Data(),configPointer.Data(),parGroup->GetParameterAt(i)->GetName());
            buffer.AppendFormatted("%s      ((%sConfig*)fConfiguration)->fConfigData[0]->%sf%s = str;\n",sTab.Data(),shortCut.Data(),configPointer.Data(),parGroup->GetParameterAt(i)->GetName());
            for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfSetLines();j++) {
               temp = parGroup->GetParameterAt(i)->GetSetLineAt(j);
               temp.ReplaceAll("##","str");
               temp.ReplaceAll("configData","(("+shortCut+"Config*)fConfiguration)->fConfigData[0]");
               buffer.AppendFormatted("%s      %s\n",sTab.Data(),temp.Data());
            }
            buffer.AppendFormatted("%s   }\n",sTab.Data());
         }
         else {
            for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfSetLines();j++) {
               temp = parGroup->GetParameterAt(i)->GetSetLineAt(j);
               temp.ReplaceAll("##","str");
               temp.ReplaceAll("configData","(("+shortCut+"Config*)fConfiguration)->fConfigData[0]");
               buffer.AppendFormatted("%s   %s\n",sTab.Data(),temp.Data());
            }
         }
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (level==0 || (level==1 && parGroup->GetGroupName()=="Common") || (level==2 && parGroup->GetGroupName()=="Folders")) {
         buffer.AppendFormatted("Bool_t %sAnalyzer::Save%s(%sConfigToForm *dialog)\n",shortCut.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),shortCut.Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   int i,ii[100];\n");
         buffer.AppendFormatted("   i = 0;ii[0]=0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   bool writeFlag;\n");
         buffer.AppendFormatted("   writeFlag = true;\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString str;\n");
         buffer.AppendFormatted("   ROMEString tmp;\n");
         buffer.AppendFormatted("   tmp = \"\";\n"); // to suppress unused warning
         buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
         buffer.AppendFormatted("   ROMEString writeString;\n");
         buffer.AppendFormatted("   ROMEString path = \"\";\n");
         buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
         buffer.AppendFormatted("   int ind;\n");
         buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
         buffer.AppendFormatted("   char *cstop;\n");
         buffer.AppendFormatted("   cstop = NULL;\n"); // to suppress unused warning
         tab = 0;
         sTab = "";
      }
      newIndexes = indexes;
      newConfigPointer.SetFormatted("%sf%s->",configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      newPointer.SetFormatted("%s%s/",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (!(level==0 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Common") && !(level==1 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Folders")) {
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
            newPointer.SetFormatted("%s%s/",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            WriteConfigToFormSave(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab,newIndexes);
         }
         else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%s   for (i=0;i<((%sConfig*)gAnalyzer->GetConfiguration())->fConfigData[0]->%sf%sArraySize;i++) {\n",sTab.Data(),shortCut.Data(),configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            newPointer.SetFormatted("%s%s/%s",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            WriteConfigToFormSave(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab+1,newIndexes);
            buffer.AppendFormatted("%s   }\n",sTab.Data());
         }
         else {
            buffer.AppendFormatted("%s   for (ii[%d]=0;ii[%d]<%s;ii[%d]++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            newPointer.SetFormatted("%s%s %%d/",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            newIndexes.AppendFormatted(",ii[%d]",parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            WriteConfigToFormSave(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab+1,newIndexes);
            buffer.AppendFormatted("%s   }\n",sTab.Data());
         }
      }
      if (level==0 || (level==1 && parGroup->GetGroupName()=="Common") || (level==2 && parGroup->GetGroupName()=="Folders")) {
         if ((level==0 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Common") || (level==1 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Folders")) {
            for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups();j++)
               buffer.AppendFormatted("%s   if (!Save%s(dialog)) return false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
         }
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("   WarningSuppression(dialog);\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         if ((level==0 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Common") || (level==1 && parGroup->GetSubGroupAt(i)->GetGroupName()=="Folders")) {
            WriteConfigToFormSave(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data(),level+1,tab,newIndexes);
         }
      }
   }

   return true;
}

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
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
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
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sConfigToForm();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sConfigToForm() {}\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void AddConfig(XMLToFormFrame *frame);\n");
   buffer.AppendFormatted("   void AddHisto(XMLToFormFrame *frame,ROMEHisto* histo);\n");
   buffer.AppendFormatted("   void AddGraph(XMLToFormFrame *frame,ROMEGraph* graph);\n");
   for (i=0;i<mainParGroup->GetNumberOfSubGroups();i++) {
      buffer.AppendFormatted("   void Add%sFrame(XMLToFormFrame *frame);\n",mainParGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (mainParGroup->GetSubGroupAt(i)->GetGroupName()=="Common") {
         for (j=0;j<mainParGroup->GetSubGroupAt(i)->GetNumberOfSubGroups();j++) {
            buffer.AppendFormatted("   void Add%sFrame(XMLToFormFrame *frame);\n",mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
            if (mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName()=="Folders") {
               for (k=0;k<mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetNumberOfSubGroups();k++)
                  buffer.AppendFormatted("   void Add%sFrame(XMLToFormFrame *frame);\n",mainParGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetSubGroupAt(k)->GetGroupName().Data());
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

Bool_t ROMEBuilder::WriteConfigCpp() {
   int i;
   ROMEString str;
   ROMEString pointer;
   ROMEString path;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString cppFile;
   ROMEString buffer;

   ROMEString format;
   int iSub = 0;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sConfig.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),mainProgName.Data());
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
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderUserCode[i])
            buffer.AppendFormatted("#include \"folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         else
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
      }
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   for (i=0;i<daqNameArray->GetEntriesFast();i++) {
      buffer.AppendFormatted("#include \"%s%s%sDAQ.h\"\n",daqDirArray->At(i).Data(),daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
   }
   for (i=0;i<numOfDB;i++)
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
   buffer.AppendFormatted("%sConfig::%sConfig()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData*[1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = 0;\n");
   buffer.AppendFormatted("}\n\n");

   // Destructor
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
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfigurationFile(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   // Fill config file content\n");
   buffer.AppendFormatted("   ifstream ifile(file);\n");
   buffer.AppendFormatted("   if (ifile.good()) {\n");
   buffer.AppendFormatted("      fConfigFileName = file;\n");
   buffer.AppendFormatted("      fConfigContent.ReadFile(ifile);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(fXMLFile);\n");
   buffer.AppendFormatted("   xml->GetPathAttribute(\"/Configuration\",\"xsi:noNamespaceSchemaLocation\",fXSDFile,\"romeConfig.xsd\");\n");
   buffer.AppendFormatted("   if (!ReadProgramConfiguration(xml))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   for (i = 0; i < fNumberOfRunConfigs + 1; i++)\n");
   buffer.AppendFormatted("      SafeDelete(fConfigData[i]);\n");
   buffer.AppendFormatted("   SafeDeleteArray(fConfigData);\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = xml->NumberOfOccurrenceOfPath(\"/Configuration/RunConfiguration\");\n");
   buffer.AppendFormatted("   fConfigData = new ConfigData*[fNumberOfRunConfigs+1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   ROMEString path = \"/Configuration/MainConfiguration\";\n");
   buffer.AppendFormatted("   ReadConfiguration(xml,path,0);\n");
   buffer.AppendFormatted("   CheckConfigurationModified(0);\n");
   buffer.AppendFormatted("   if (!SetConfiguration(0,0))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   for (int i=0;i<fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      fConfigData[i+1] = new ConfigData();\n");
   buffer.AppendFormatted("      path.SetFormatted(\"/Configuration/RunConfiguration[%%d]\",i+1);\n");
   buffer.AppendFormatted("      ReadConfiguration(xml,path,i+1);\n");
   buffer.AppendFormatted("      CheckConfigurationModified(i+1);\n");
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
   buffer.AppendFormatted("Bool_t %sConfig::ReadProgramConfiguration(ROMEXML *xml) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString buffer;\n");
   buffer.AppendFormatted("   xml->GetPathValue(\"/Configuration/ProgramConfiguration/ProgramMode\",buffer,\"\");\n");
   buffer.AppendFormatted("   buffer.ToLower();\n");
   buffer.AppendFormatted("   if (gAnalyzer->isNoGraphics())\n");
   buffer.AppendFormatted("      gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      if (buffer==\"0\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("      else if (buffer==\"1\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetStandAloneARGUS();\n");
   buffer.AppendFormatted("      else if (buffer==\"2\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetROMEAndARGUS();\n");
   buffer.AppendFormatted("      else if (buffer==\"3\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetROMEMonitor();\n");
   buffer.AppendFormatted("      else {\n");
   buffer.AppendFormatted("         Error(\"ReadProgramConfiguration\",\"Program mode must be one of the following :\\n0 : analyzer\\n1 : monitor\\n2 : analyzer and monitor\\n3 : monitor connected to an analyzer.\");\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   ROMEString tempPath;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[index];\n",shortCut.Data());
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   iSub = 0;
   WriteConfigRead(buffer,mainParGroup,1,"","","configData->","",&iSub);
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration Modified
   buffer.AppendFormatted("\n// Check Configuration Modified\n");
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfigurationModified(Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   ROMEString tempPath;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[index];\n",shortCut.Data());
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   iSub = 0;
   WriteConfigCheckModified(buffer,mainParGroup,1,"","","configData->","",&iSub);
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");


   // Check Configuration
   buffer.AppendFormatted("\n// Check Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfiguration(Long64_t runNumber) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i,j,ret;\n");
   buffer.AppendFormatted("   if (fActiveConfiguration!=0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(fActiveConfiguration,0))\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   for (i=1;i<=fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      if (fConfigData[i]->fOffline->fRunNumbersModified) {\n");
   buffer.AppendFormatted("         if (fConfigData[i]->fLastRunNumberIndex!=-1) {\n");
   buffer.AppendFormatted("            for (j=fConfigData[i]->fLastRunNumberIndex;j<fConfigData[i]->fRunNumberArray.GetSize();j++) {\n");
   buffer.AppendFormatted("               ret = gAnalyzer->CheckNumber(runNumber,fConfigData[i]->fRunNumberArray);\n");
   buffer.AppendFormatted("               if (ret==-1) {\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastRunNumberIndex = -1;\n");
   buffer.AppendFormatted("                  break;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else if (ret==1) {\n");
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
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfiguration(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString fileName = file;\n");
   buffer.AppendFormatted("   int i,j;\n");
   buffer.AppendFormatted("   if (fActiveConfiguration!=0) {\n");
   buffer.AppendFormatted("      if (!SetConfiguration(fActiveConfiguration,0))\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   for (i=1;i<=fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      if (fConfigData[i]->fOffline->fInputFileNamesModified) {\n");
   buffer.AppendFormatted("         if (fConfigData[i]->fLastInputFileNameIndex!=-1) {\n");
   buffer.AppendFormatted("            for (j=fConfigData[i]->fLastInputFileNameIndex;j<fConfigData[i]->fInputFileNameArray.GetEntriesFast();j++) {\n");
   buffer.AppendFormatted("               if (fConfigData[i]->fInputFileNameArray.At(j)==fileName) {\n");
   buffer.AppendFormatted("                  if (!SetConfiguration(i,i))\n");
   buffer.AppendFormatted("                     return false;\n");
   buffer.AppendFormatted("                  fConfigData[i]->fLastInputFileNameIndex = j;\n");
   buffer.AppendFormatted("               }\n");
   buffer.AppendFormatted("               else if (fConfigData[i]->fInputFileNameArray.At(j)<fileName) {\n");
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


   // Set Configuration
   buffer.AppendFormatted("\n// Set Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::SetConfiguration(Int_t modIndex,Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[index];\n",shortCut.Data());
   buffer.AppendFormatted("   %sConfig::ConfigData *modConfigData = fConfigData[modIndex];\n",shortCut.Data());
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   char* cstop;\n");
   buffer.AppendFormatted("   cstop=NULL;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEString path = \"\";\n");
   buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
   buffer.AppendFormatted("   int ind;\n");
   buffer.AppendFormatted("   ind = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   fActiveConfiguration = index;\n");
   iSub = 0;
   WriteConfigSet(buffer,mainParGroup,1,"","",&iSub);
   // end
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");


   // Write Configuration File
   buffer.AppendFormatted("\n// Write Configuration File\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfigurationFile(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   if (!xml->OpenFileForWrite(fXMLFile))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   xml->SetTranslate(0);\n");
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
   buffer.AppendFormatted("   WriteConfiguration(xml,0);\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   for (int i=0;i<fNumberOfRunConfigs;i++) {\n");
   buffer.AppendFormatted("      xml->StartElement(\"RunConfiguration\");\n");
   buffer.AppendFormatted("      WriteConfiguration(xml,i+1);\n");
   buffer.AppendFormatted("      xml->EndElement();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   xml->EndDocument();\n");
   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write Program Configuration
   buffer.AppendFormatted("\n// Write Program Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteProgramConfiguration(ROMEXML *xml) {\n",shortCut.Data());
   buffer.AppendFormatted("   xml->StartElement(\"ProgramConfiguration\");\n");
   buffer.AppendFormatted("   if (fCommentLevel >= 2)\n");
   configXSD->GetPathValue("/xs:schema/xs:complexType[@name='ProgramConfigurationDesc']/xs:sequence/xs:element[@name=ProgramMode]/xs:annotation/xs:documentation", str, "");
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

   // Write Configuration
   buffer.AppendFormatted("\n// Write Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfiguration(ROMEXML *xml,Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   %sConfig::ConfigData *configData = fConfigData[index];\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   int ii[100];\n");
   buffer.AppendFormatted("   ii[0] = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   ROMEStrArray histoComments;\n");
   for (i=0;i<histoParameters->GetEntriesFast();i++) {
      path.SetFormatted("/xs:schema/xs:complexType[@name='HistogramDesc']/xs:sequence/xs:element[@name=Hist%s]/xs:annotation/xs:documentation", histoParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("   histoComments.AddLast(\"%s\");\n",str.Data());
   }
   buffer.AppendFormatted("   ROMEStrArray graphComments;\n");
   for (i=0;i<graphParameters->GetEntriesFast();i++) {
      path.SetFormatted("/xs:schema/xs:complexType[@name='GraphDesc']/xs:sequence/xs:element[@name=Graph%s]/xs:annotation/xs:documentation", graphParameters->At(i).Data());
      configXSD->GetPathValue(path, str, "");
      buffer.AppendFormatted("   graphComments.AddLast(\"%s\");\n",str.Data());
   }
   iSub = 0;
   WriteConfigWrite(buffer,mainParGroup,1,"","",&iSub);
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteConfigH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sConfig.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sConfig_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sConfig_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sConfig", shortCut.Data());
   clsDescription.SetFormatted("This class handles the framework configuration file for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#if !defined(__CINT__)\n");
   buffer.AppendFormatted("#   include \"TArrayL64.h\"\n");
   buffer.AppendFormatted("#   include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#   include \"ROMEStrArray.h\"\n");
   buffer.AppendFormatted("#   include \"ROMEConfigHisto.h\"\n");
   buffer.AppendFormatted("#   include \"ROMEConfigGraph.h\"\n");
   buffer.AppendFormatted("#   include \"ROMEConfig.h\"\n");
   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("class ROMEXML;\n");

   // Class
   buffer.AppendFormatted("\nclass %sConfig : public ROMEConfig\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#if !defined(__CINT__)\n");


   // sub classes
   buffer.AppendFormatted("public:\n");
   WriteConfigClass(buffer,mainParGroup,1);
   buffer.AppendFormatted("\n");

   // Fields
   buffer.AppendFormatted("   ROMEString fXSDFile;\n");
   buffer.AppendFormatted("   ROMEString fXMLFile;\n");
   buffer.AppendFormatted("   Int_t fNumberOfRunConfigs;\n");
   buffer.AppendFormatted("   Int_t fActiveConfiguration;\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sConfig();\n",shortCut.Data());

   // methods
   buffer.AppendFormatted("   Bool_t WriteConfigurationFile(const char *file);\n");
   buffer.AppendFormatted("   Bool_t ReadConfigurationFile(const char *file);\n");
   buffer.AppendFormatted("   Bool_t CheckConfiguration(Long64_t runNumber);\n");
   buffer.AppendFormatted("   Bool_t CheckConfiguration(const char *file);\n");
   buffer.AppendFormatted("   Bool_t CheckConfigurationModified(Int_t index);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   Bool_t ReadProgramConfiguration(ROMEXML *xml);\n");
   buffer.AppendFormatted("   Bool_t ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t index);\n");
   buffer.AppendFormatted("   Bool_t WriteProgramConfiguration(ROMEXML *xml);\n");
   buffer.AppendFormatted("   Bool_t WriteConfiguration(ROMEXML *xml,Int_t index);\n");
   buffer.AppendFormatted("   Bool_t SetConfiguration(Int_t modIndex,Int_t index);\n");

   buffer.AppendFormatted("\n");
   // Footer

   buffer.AppendFormatted("#endif\n");
   buffer.AppendFormatted("   ClassDef(%sConfig, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sConfig_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::AddConfigParameters()
{
   int i,j;
   int nIndex = 1;
   int *index = new int[nIndex];
   ROMEString str;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   ROMEConfigParameterGroup* subSubSubGroup;
   ROMEConfigParameterGroup* subsubGroup;
   ROMEConfigParameterGroup* subsubSubGroup;
   ROMEConfigParameterGroup* subsubSubSubGroup;
   ROMEString tmp;
   for (i=0;i<nIndex;i++)
      index[i] = -1;
   maxConfigParameterHierarchyLevel = 0;

   histoParameters = new ROMEStrArray(16);
   histoParameterTypes = new ROMEStrArray(16);
   histoParameterWidgetTypes = new ROMEStrArray(16);
   histoParameters->AddLast("Active");
   histoParameterTypes->AddLast("bool");
   histoParameterWidgetTypes->AddLast("CheckButton");
   histoParameters->AddLast("Title");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("FolderTitle");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ArraySize");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ArrayStartIndex");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("XLabel");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("YLabel");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ZLabel");
   histoParameterTypes->AddLast("%s");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("XNbins");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Xmin");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Xmax");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("YNbins");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Ymin");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Ymax");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("ZNbins");
   histoParameterTypes->AddLast("%d");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Zmin");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");
   histoParameters->AddLast("Zmax");
   histoParameterTypes->AddLast("%g");
   histoParameterWidgetTypes->AddLast("EditBox");

   graphParameters = new ROMEStrArray(16);
   graphParameterTypes = new ROMEStrArray(16);
   graphParameterWidgetTypes = new ROMEStrArray(16);
   graphParameters->AddLast("Active");
   graphParameterTypes->AddLast("bool");
   graphParameterWidgetTypes->AddLast("CheckButton");
   graphParameters->AddLast("Title");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("FolderTitle");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("ArraySize");
   graphParameterTypes->AddLast("%d");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("ArrayStartIndex");
   graphParameterTypes->AddLast("%d");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("XLabel");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("YLabel");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("ZLabel");
   graphParameterTypes->AddLast("%s");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Xmin");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Xmax");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Ymin");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Ymax");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Zmin");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");
   graphParameters->AddLast("Zmax");
   graphParameterTypes->AddLast("%g");
   graphParameterWidgetTypes->AddLast("EditBox");

   mainParGroup = new ROMEConfigParameterGroup("ConfigData");

   // Modes
   {
      subGroup = new ROMEConfigParameterGroup("Modes");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Modes/AnalyzingMode
      subGroup->AddParameter(new ROMEConfigParameter("AnalyzingMode","1","ComboBox"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##==\"online\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOnline();");
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOffline();");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isOnline())");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"online\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"offline\";");
      subGroup->GetLastParameter()->AddComboBoxEntry("offline");
      subGroup->GetLastParameter()->AddComboBoxEntry("online");
      // Modes/DAQSystem
      subGroup->AddParameter(new ROMEConfigParameter("DAQSystem","1","ComboBox"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (false) {}");
      for (i=0;i<daqNameArray->GetEntriesFast();i++) {
         str = daqNameArray->At(i);
         str.ToLower();
         subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"%s\",TString::kIgnoreCase)) {",str.Data());
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->Set%sDAQ(new %s%sDAQ());",daqNameArray->At(i).Data(),daqTypeArray->At(i).Data(),daqNameArray->At(i).Data());
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->Get%sDAQ());",daqNameArray->At(i).Data());
         subGroup->GetLastParameter()->AddSetLine("}");
      }
      subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"none\",TString::kIgnoreCase) || ##.Length()==0) {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(new ROMENoDAQSystem());");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddComboBoxEntry("none");
      for (i=0;i<daqNameArray->GetEntriesFast();i++) {
         str = daqNameArray->At(i);
         str.ToLower();
         subGroup->GetLastParameter()->AddComboBoxEntry(str.Data());
      }
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isActiveDAQSet())");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetActiveDAQ()->GetName();");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"none\";");
#if 0
   // batch mode does not work fully with configuration file,
   // because we have to know before constructor of analyzer
   // comment out for the moment.
      // Modes/BatchMode
      subGroup->AddParameter(new ROMEConfigParameter("BatchMode","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetBatchMode(true);");
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetBatchMode(false);");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isBatchMode())");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
#endif
   }

   // Offline
   {
      subGroup = new ROMEConfigParameterGroup("Offline");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // RunNumbers
      subGroup->AddParameter(new ROMEConfigParameter("RunNumbers"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("configData->fLastRunNumberIndex = 0;");
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("gAnalyzer->DecodeNumbers(configData->fOffline->fRunNumbers,configData->fRunNumberArray);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetRunNumberStringOriginal();");
      // EventNumbers
      subGroup->AddParameter(new ROMEConfigParameter("EventNumbers"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetEventNumbers(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetEventNumberStringOriginal();");
      // InputFileNames
      subGroup->AddParameter(new ROMEConfigParameter("InputFileNames"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("configData->fLastInputFileNameIndex = 0;");
      subGroup->GetLastParameter()->AddReadModifiedTrueLine("gAnalyzer->DecodeInputFileNames(configData->fOffline->fInputFileNames,configData->fInputFileNameArray);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetInputFileNamesStringOriginal();");
   }

   // Online
   {
      subGroup = new ROMEConfigParameterGroup("Online");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Host
      subGroup->AddParameter(new ROMEConfigParameter("Host"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineHost(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineHost();");
      // Experiment
      subGroup->AddParameter(new ROMEConfigParameter("Experiment"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineExperiment(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineExperiment();");
      // AnalyzerName
      subGroup->AddParameter(new ROMEConfigParameter("AnalyzerName"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##.Length())");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOnlineAnalyzerName(##.Data());");
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineAnalyzerName(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineAnalyzerName();");
      // Memory Buffer
      subGroup->AddParameter(new ROMEConfigParameter("MemoryBuffer"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##.Length())");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOnlineMemoryBuffer(##.Data());");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineMemoryBuffer();");
   }

   // Paths
   {
      subGroup = new ROMEConfigParameterGroup("Paths");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // InputFilePath
      subGroup->AddParameter(new ROMEConfigParameter("InputFilePath"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##[##.Length()-1]!='/' && ##[##.Length()-1]!='\\\\')");
      subGroup->GetLastParameter()->AddSetLine("   ##.Append(\"/\");");
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetInputDir(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetInputDir();");
      // OutputFilePath
      subGroup->AddParameter(new ROMEConfigParameter("OutputFilePath"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, subGroup->GetGroupName());
      subGroup->GetLastParameter()->AddSetLine("if (##[##.Length()-1]!='/' && ##[##.Length()-1]!='\\\\')");
      subGroup->GetLastParameter()->AddSetLine("   ##.Append(\"/\");");
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOutputDir(##);");
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOutputDir();");
   }
   // Common
   {
      subGroup = new ROMEConfigParameterGroup("Common");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Settings
      {
         subsubGroup = new ROMEConfigParameterGroup("Settings");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subsubGroup);
         // VerboseLevel
         subsubGroup->AddParameter(new ROMEConfigParameter("VerboseLevel","1","ComboBox"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"mute\")");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kMute);");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"error\")");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kErrorOnly);");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"warning\")");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kErrorAndWarning);");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"normal\")");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kNormal);");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"debug\")");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::SetVerboseLevel(ROMEPrint::kDebug);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kMute)");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"mute\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kErrorOnly)");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"error\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kErrorAndWarning)");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"warning\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kNormal)");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"normal\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else if (ROMEPrint::GetVerboseLevel() == ROMEPrint::kDebug)");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"debug\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"normal\";");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("mute");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("error");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("warning");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("normal");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("verbose");
         // ConfigCommentLevel
         subsubGroup->AddParameter(new ROMEConfigParameter("ConfigCommentLevel","1","ComboBox"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetConfiguration()->SetCommentLevel(strtol(##,&cstop,10));");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\", gAnalyzer->GetConfiguration()->GetCommentLevel());");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("0");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("1");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("2");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("3");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("4");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("5");
         // QuitMode
         subsubGroup->AddParameter(new ROMEConfigParameter("QuitMode","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetQuitMode(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetQuitMode(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isQuitMode())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // ShowSplashScreen
         subsubGroup->AddParameter(new ROMEConfigParameter("ShowSplashScreen","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSplashScreen(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSplashScreen(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isSplashScreen())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // GraphicalConfigEdit
         subsubGroup->AddParameter(new ROMEConfigParameter("GraphicalConfigEdit","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetGraphicalConfigEdit(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetGraphicalConfigEdit(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isGraphicalConfigEdit())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // PreserveConfig
         subsubGroup->AddParameter(new ROMEConfigParameter("PreserveConfig","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetPreserveConfig(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetPreserveConfig(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isPreserveConfig())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      }

      // SocketServer
      {
         subsubGroup = new ROMEConfigParameterGroup("SocketServer");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subsubGroup);
         // Active
         subsubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSocketServerActive(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSocketServerActive(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isSocketServerActive())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // PortNumber
         subsubGroup->AddParameter(new ROMEConfigParameter("PortNumber"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketServerPortNumber(##.Data());");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetSocketServerPortNumber());");
      }
      // DataBases
      {
         subsubGroup = new ROMEConfigParameterGroup("DataBase","unknown");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subsubGroup->AddReadGroupArrayInitLine("gAnalyzer->InitDataBases(configData->fCommon->fDataBaseArraySize);");
         subGroup->AddSubGroup(subsubGroup);
         // DataBase/Name
         subsubGroup->AddParameter(new ROMEConfigParameter("Name"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetDataBaseName(i,##.Data());");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBaseName(i);");
         // DataBase/Connection
         subsubGroup->AddParameter(new ROMEConfigParameter("Connection"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetDataBaseConnection(i,##.Data());");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBaseConnection(i);");
         // DataBase/Type
         subsubGroup->AddParameter(new ROMEConfigParameter("Type","1","ComboBox"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"sql\") {");
         if (sql) {
            subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMESQLDataBase());");
            subsubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",gAnalyzer->GetDataBaseConnection(i)))");
            subsubGroup->GetLastParameter()->AddSetLine("      return false;");
         }
         else{
            subsubGroup->GetLastParameter()->AddSetLine("   ROMEPrint::Error(\"%%s is not linked with sql support.\\n\", gAnalyzer->GetProgramName())\n;");
            subsubGroup->GetLastParameter()->AddSetLine("   return false;");
         }
         subsubGroup->GetLastParameter()->AddSetLine("}");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"none\" ||");
         subsubGroup->GetLastParameter()->AddSetLine("         ##==\"\") {");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMENoDataBase());");
         subsubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",\"\"))");
         subsubGroup->GetLastParameter()->AddSetLine("      return false;");
         subsubGroup->GetLastParameter()->AddSetLine("}");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"xml\") {");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMEXMLDataBase());");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEString str = gAnalyzer->GetDataBaseConnection(i);");
         subsubGroup->GetLastParameter()->AddSetLine("   int ind;");
         subsubGroup->GetLastParameter()->AddSetLine("   if ((ind=str.Index(\";\",1,0,TString::kExact))==-1) {");
         subsubGroup->GetLastParameter()->AddSetLine("      ROMEPrint::Error(\"Invalid database connection\\n\");");
         subsubGroup->GetLastParameter()->AddSetLine("      return false;");
         subsubGroup->GetLastParameter()->AddSetLine("   }");
         subsubGroup->GetLastParameter()->AddSetLine("   ROMEString path = str(0,ind);");
         subsubGroup->GetLastParameter()->AddSetLine("   if (path[path.Length()-1]!='/' && path[path.Length()-1]!='\\\\')");
         subsubGroup->GetLastParameter()->AddSetLine("      path += \"/\";");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBaseDir(i,path.Data());");
         subsubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),gAnalyzer->GetDataBaseDir(i),((TString)str(ind+1,str.Length()-ind-1)).Data()))");
         subsubGroup->GetLastParameter()->AddSetLine("      return false;");
         subsubGroup->GetLastParameter()->AddSetLine("}");
         subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"text\") {");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMETextDataBase());");
         subsubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),gAnalyzer->GetDataBaseConnection(i),\"\"))");
         subsubGroup->GetLastParameter()->AddSetLine("      return false;");
         subsubGroup->GetLastParameter()->AddSetLine("}");
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"odb\") {");
         subsubGroup->GetLastParameter()->AddSetLine("   if (gAnalyzer->isOffline())");
         subsubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetDataBase(i,new ROMEODBOfflineDataBase());");
         subsubGroup->GetLastParameter()->AddSetLine("   else");
         subsubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetDataBase(i,new ROMEODBOnlineDataBase());");
         subsubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",\"\"))");
         subsubGroup->GetLastParameter()->AddSetLine("      return false;");
         subsubGroup->GetLastParameter()->AddSetLine("}");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("none");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("sql");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("xml");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("text");
         subsubGroup->GetLastParameter()->AddComboBoxEntry("odb");
         for (i=0;i<numOfDB;i++) {
            subsubGroup->GetLastParameter()->AddSetLine("else if (##==\"%s\") {\n",dbName[i].Data());
            subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new %s%sDataBase());\n",shortCut.Data(),dbName[i].Data());
            subsubGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",gAnalyzer->GetDataBaseConnection(i)))");
            subsubGroup->GetLastParameter()->AddSetLine("      return false;");
            subsubGroup->GetLastParameter()->AddSetLine("}");
            subsubGroup->GetLastParameter()->AddComboBoxEntry(dbName[i].Data());
         }
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBase(i)->GetType();");
         // DataBase/EventBased
         subsubGroup->AddParameter(new ROMEConfigParameter("EventBased","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetEventBasedDataBase(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetEventBasedDataBase(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsEventBasedDataBase())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      }
      // Folders
      {
         subsubGroup = new ROMEConfigParameterGroup("Folders");
         subGroup->AddSubGroup(subsubGroup);
         for (i=0;i<numOfFolder;i++) {
            if (!folderUsed[i])
               continue;
            if (folderDataBase[i] && !folderSupport[i]) {
               subsubSubGroup = new ROMEConfigParameterGroup(folderName[i],"1","Folder");
               subsubSubGroup->SetComment(ROMEConfig::kCommentLevelObj, folderShortDescription[i]);
               subsubGroup->AddSubGroup(subsubSubGroup);
               for (j=0;j<numOfValue[i];j++) {
                  if (valueDimension[i][j]>1)
                     continue;
                  subsubSubSubGroup = new ROMEConfigParameterGroup(valueName[i][j],"1","Field");
                  subsubSubSubGroup->SetComment(ROMEConfig::kCommentLevelObj, valueComment[i][j]);
                  subsubSubGroup->AddSubGroup(subsubSubSubGroup);
                  // DataBaseName
                  subsubSubSubGroup->AddParameter(new ROMEConfigParameter("DataBaseName"));
                  subsubSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Field");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetDBAccess()->SetDBNameAt(%d,%d,##.Data());",i,j);
                  subsubSubSubGroup->GetLastParameter()->DontWriteLinesAlways();
                  subsubSubSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDBAccess()->GetDBNameAt(%d,%d);",i,j);
                  // DataBasePath
                  subsubSubSubGroup->AddParameter(new ROMEConfigParameter("DataBasePath"));
                  subsubSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Field");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("path = ##.Data();");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("ind = path.Last('\"');");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("if (ind==-1 || ind==path.Length()-1) {");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   subPath = path.ReplaceAll(\"\\\"\",\"\");");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetDBAccess()->SetDBPathAt(%d,%d,subPath.Data());",i,j);
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("}");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("else {");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   subPath = path(1,ind-1);");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetDBAccess()->SetDBPathAt(%d,%d,subPath.Data());",i,j);
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   path = path(ind+2,path.Length()-ind+2);");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   int num = path.CountChar(',')+1;");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   for (i=0;i<num;i++) {");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("      if (i<num-1)");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("         ind = path.First(',');");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("      else");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("         ind = path.Length();");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("      subPath = path(0,ind);");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetDBAccess()->SetDBCodeAt(%d,%d,i,gAnalyzer->GetObjectInterpreterCode(subPath.Data()));",i,j);
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("      path = path(ind+1,path.Length()-ind+1);");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("   }");
                  subsubSubSubGroup->GetLastParameter()->AddSetLine("}");
                  subsubSubSubGroup->GetLastParameter()->DontWriteLinesAlways();
                  subsubSubSubGroup->GetLastParameter()->AddWriteLine("tmp = gAnalyzer->GetDBAccess()->GetDBPathOriginalAt(%d,%d);",i,j);
                  subsubSubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetDBPathWriteFlag(tmp.Data()))");
                  subsubSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetDBAccess()->GetDBPathOriginalAt(%d,%d);",i,j);
                  subsubSubSubGroup->GetLastParameter()->AddWriteLine("else");
                  subsubSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"\";");
               }
            }
         }
      }
      // Trees
      if (numOfTree>0) {
         subsubGroup = new ROMEConfigParameterGroup("Trees");
         subGroup->AddSubGroup(subsubGroup);
         // Accumulate
         subsubGroup->AddParameter(new ROMEConfigParameter("Accumulate","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName(),"/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Trees']/xs:annotation/xs:documentation");
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetTreeAccumulation(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetTreeAccumulation(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isTreeAccumulation())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // Tree
         for (i=0;i<numOfTree;i++) {
            subsubSubGroup = new ROMEConfigParameterGroup(treeName[i],"1","Tree");
            subsubSubGroup->SetComment(ROMEConfig::kCommentLevelObj, treeDescription[i]);
            subsubGroup->AddSubGroup(subsubSubGroup);
            // Read
            subsubSubGroup->AddParameter(new ROMEConfigParameter("Read","1","CheckButton"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddSetLine("   if (##==\"true\")");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetRead(true);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("   else");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetRead(false);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("}");
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetTreeObjectAt(%d)->isRead())",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   else");
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("}");
            // Write
            subsubSubGroup->AddParameter(new ROMEConfigParameter("Write","1","CheckButton"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddSetLine("   if (##==\"true\")");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetWrite(true);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("   else");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetWrite(false);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("}");
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetTreeObjectAt(%d)->isWrite())",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   else");
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("}");
            // Fill
            subsubSubGroup->AddParameter(new ROMEConfigParameter("Fill","1","CheckButton"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddSetLine("   if ((##==\"true\") || gAnalyzer->GetTreeObjectAt(%d)->isWrite())",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetFill(true);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("   else");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetFill(false);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("}");
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetTreeObjectAt(%d)->isFill())",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   else");
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("}");
            // CompressionLevel
            subsubSubGroup->AddParameter(new ROMEConfigParameter("CompressionLevel"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subsubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetCompressionLevel(##.ToInteger());",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%d\",gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());",i);
            // MaxNumberOfEntries
            subsubSubGroup->AddParameter(new ROMEConfigParameter("MaxNumberOfEntries"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subsubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetMaxEntries(((Long_t)##.ToInteger()));",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
   #if defined( R__VISUAL_CPLUSPLUS )
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%I64d\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());",i);
   #else
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%lld\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());",i);
   #endif
            // TreeOutputFileName
            subsubSubGroup->AddParameter(new ROMEConfigParameter("TreeOutputFileName"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor())");
            subsubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetConfigFileName(##);",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor())");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetTreeObjectAt(%d)->GetConfigFileName().Data();",i);
            // SaveConfiguration
            subsubSubGroup->AddParameter(new ROMEConfigParameter("SaveConfiguration", "1", "CheckButton"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tree");
            subsubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddSetLine("   if ((##==\"true\"))");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetSaveConfig(true);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("   else");
            subsubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetSaveConfig(false);",i);
            subsubSubGroup->GetLastParameter()->AddSetLine("}");
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetTreeObjectAt(%d)->isSaveConfig())",i);
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   else");
            subsubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("}");

            // Branch
            for (j=0;j<numOfBranch[i];j++) {
               tmp = branchName[i][j];
               if (tmp.EndsWith("."))
                  tmp.Resize(tmp.Length() - 1);
               subsubSubSubGroup = new ROMEConfigParameterGroup(tmp,"1","Branch");
               subsubSubSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Branch");
               subsubSubSubGroup->SetWriteEmptyLine(kFALSE);
               subsubSubGroup->AddSubGroup(subsubSubSubGroup);
               // Active
               subsubSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
               subsubSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, "Branch");
               subsubSubSubGroup->GetLastParameter()->AddSetLine("if (!gAnalyzer->IsROMEMonitor()) {");
               subsubSubSubGroup->GetLastParameter()->AddSetLine("   if (##==\"false\")");
               subsubSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetBranchActiveAt(%d, false);",i,j);
               subsubSubSubGroup->GetLastParameter()->AddSetLine("   else");
               subsubSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetTreeObjectAt(%d)->SetBranchActiveAt(%d, true);",i,j);
               subsubSubSubGroup->GetLastParameter()->AddSetLine("}");
               subsubSubSubGroup->GetLastParameter()->AddWriteLine("if (!gAnalyzer->IsROMEMonitor()) {");
               subsubSubSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d))",i,j);
               subsubSubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
               subsubSubSubGroup->GetLastParameter()->AddWriteLine("   else");
               subsubSubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
               subsubSubSubGroup->GetLastParameter()->AddWriteLine("}");
            }
         }
      }
      // Global Steering Parameters
      {
         if (readGlobalSteeringParameters) {
            subsubGroup = new ROMEConfigParameterGroup("GlobalSteeringParameters");
            subGroup->AddSubGroup(subsubGroup);
            AddSteeringConfigParameters(subsubGroup,0,numOfTask,"gAnalyzer->GetGSP()","gAnalyzer");
         }
      }
   }
   // Analyzer
   {
      subGroup = new ROMEConfigParameterGroup("Analyzer");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // HistogramRead
      {
         subsubGroup = new ROMEConfigParameterGroup("HistogramRead");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subsubGroup);
         // Read
         subsubGroup->AddParameter(new ROMEConfigParameter("Read","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetHistosRead(true);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetHistosRead(false);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsHistosRead())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // RunNumber
         subsubGroup->AddParameter(new ROMEConfigParameter("RunNumber"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetHistosRun(##.ToInteger());");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetHistosRun());");
      }
      // Macros
      {
         subsubGroup = new ROMEConfigParameterGroup("Macros");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subsubGroup);
         // Macros/BeginOfRun
         subsubGroup->AddParameter(new ROMEConfigParameter("BeginOfRun"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetBeginOfRunMacro(##.Data());", shortCut.Data());
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetBeginOfRunMacro();", shortCut.Data());
         // Macros/BeginOfEvent
         subsubGroup->AddParameter(new ROMEConfigParameter("BeginOfEvent"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetBeginOfEventMacro(##.Data());", shortCut.Data());
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetBeginOfEventMacro();", shortCut.Data());
         // Macros/EndOfEvent
         subsubGroup->AddParameter(new ROMEConfigParameter("EndOfEvent"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetEndOfEventMacro(##.Data());", shortCut.Data());
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetEndOfEventMacro();", shortCut.Data());
         // Macros/EndOfRun
         subsubGroup->AddParameter(new ROMEConfigParameter("EndOfRun"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->SetEndOfRunMacro(##.Data());", shortCut.Data());
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = static_cast<%sEventLoop*>(gAnalyzer->GetMainTask())->GetEndOfRunMacro();", shortCut.Data());
      }
      // Tasks
      if (numOfTask>0) {
         subsubGroup = new ROMEConfigParameterGroup("Tasks");
         subGroup->AddSubGroup(subsubGroup);
         AddTaskConfigParameters(subsubGroup,-1);
      }
   }
   // Monitor
   {
      subGroup = new ROMEConfigParameterGroup("Monitor");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
      mainParGroup->AddSubGroup(subGroup);
      // Display
      if (numOfTab>0) {
         subsubGroup = new ROMEConfigParameterGroup("Display");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subsubGroup);
         // WindowScale
         subsubGroup->AddParameter(new ROMEConfigParameter("WindowScale"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetWindowScale(##.ToFloat());");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%2.1f\",gAnalyzer->GetWindow()->GetWindowScale());");
         // StatusBar
         subsubGroup->AddParameter(new ROMEConfigParameter("StatusBar","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"false\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetStatusBarSwitch(kFALSE);");
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetStatusBarSwitch(kTRUE);");
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetStatusBarSwitch())");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // UpdateFrequency
         subsubGroup->AddParameter(new ROMEConfigParameter("UpdateFrequency"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubGroup->GetGroupName());
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetWindowUpdateFrequency(##.ToInteger());");
         subsubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetWindowUpdateFrequency());");
         // AnalyzerController
         {
            subsubSubGroup = new ROMEConfigParameterGroup("AnalyzerController");
            subsubSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
            subsubGroup->AddSubGroup(subsubSubGroup);
            // Active
            subsubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, subsubSubGroup->GetGroupName());
            subsubSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
            subsubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetControllerActive(kTRUE);");
            subsubSubGroup->GetLastParameter()->AddSetLine("else");
            subsubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetControllerActive(kFALSE);");
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->IsControllerActive())");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("else");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
            // NetFolderName
            subsubSubGroup->AddParameter(new ROMEConfigParameter("NetFolderName"));
            subsubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subsubSubGroup->GetGroupName());
            subsubSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetControllerNetFolder(##.Data());");
            subsubSubGroup->GetLastParameter()->AddWriteLine("writeString = \"\";");
            subsubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetControllerNetFolder()!=NULL)");
            subsubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetWindow()->GetControllerNetFolder()->GetName();");
         }
      }
      // SocketClient
      {
         subSubGroup = new ROMEConfigParameterGroup("SocketClient");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup);
         subGroup->AddSubGroup(subSubGroup);
         // Host
         subSubGroup->AddParameter(new ROMEConfigParameter("Host"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketClientHost(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetSocketClientHost();");
         // Port
         subSubGroup->AddParameter(new ROMEConfigParameter("Port"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subSubGroup->GetGroupName());
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketClientPort(##.Data());");
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetSocketClientPort());");
      }
      // NetFolder
      for (i=0;i<numOfNetFolder;i++) {
         subsubGroup = new ROMEConfigParameterGroup(netFolderName[i],"1","NetFolder");
         subsubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"NetFolder");
         subGroup->AddSubGroup(subsubGroup);
         // Active
         subsubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, "NetFolder");
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderActive(%d,kTRUE);",i);
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderActive(%d,kFALSE);",i);
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetNetFolderActive(%d))",i);
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // Reconnect
         subsubGroup->AddParameter(new ROMEConfigParameter("Reconnect","1","CheckButton"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subsubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderReconnect(%d,kTRUE);",i);
         subsubGroup->GetLastParameter()->AddSetLine("else");
         subsubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderReconnect(%d,kFALSE);",i);
         subsubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetNetFolderReconnect(%d))",i);
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subsubGroup->GetLastParameter()->AddWriteLine("else");
         subsubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // Root
         subsubGroup->AddParameter(new ROMEConfigParameter("Root"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderRoot(%d,const_cast<char*>(##.Data()));",i);
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetNetFolderRoot(%d);",i);
         // Host
         subsubGroup->AddParameter(new ROMEConfigParameter("Host"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderHost(%d,const_cast<char*>(##.Data()));",i);
         subsubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%s\",gAnalyzer->GetNetFolderHost(%d));",i);
         // Port
         subsubGroup->AddParameter(new ROMEConfigParameter("Port"));
         subsubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "NetFolder");
         subsubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderPort(%d,const_cast<char*>(##.Data()));",i);
         subsubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetNetFolderPort(%d);",i);
      }
      // Tabs
      if (numOfTab>0) {
         subsubGroup = new ROMEConfigParameterGroup("Tabs");
         subGroup->AddSubGroup(subsubGroup);
         AddTabConfigParameters(subsubGroup,-1);
      }
   }




   // midas banks
   if (numOfEvent>0) {
      subGroup = new ROMEConfigParameterGroup("Midas");
      subGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Midas","/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Midas']/xs:annotation/xs:documentation");
      mainParGroup->AddSubGroup(subGroup);
      subGroup->AddParameter(new ROMEConfigParameter("MidasByteSwap","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, subGroup->GetGroupName(),"/xs:schema/xs:complexType[@name='ConfigurationDesc']/xs:sequence/xs:element[@name='Midas']/xs:complexType/xs:sequence/xs:element[@name=MidasByteSwap]/xs:annotation/xs:documentation");
      subGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddSetLine("   if (##==\"true\")");
      subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetMidasDAQ()->SetByteSwap(true);");
      subGroup->GetLastParameter()->AddSetLine("   else");
      subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetMidasDAQ()->SetByteSwap(false);");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
      subGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetMidasDAQ()->GetByteSwap())");
      subGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("   else");
      subGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
      subGroup->GetLastParameter()->AddWriteLine("}");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"false\";");
      subGroup->GetLastParameter()->AddAdditionalWriteLine("}");
   }
   for (i=0;i<numOfEvent;i++) {
      subSubGroup = new ROMEConfigParameterGroup(eventName[i],"1","Event");
      subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Event");
      subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->AddSubGroup(subSubGroup);
      subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, "Event");
      subSubGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
      subSubGroup->GetLastParameter()->AddSetLine("   if (##==\"true\")");
      subSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetMidasDAQ()->Set%sEventActive(true);",eventName[i].Data());
      subSubGroup->GetLastParameter()->AddSetLine("   else");
      subSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetMidasDAQ()->Set%sEventActive(false);",eventName[i].Data());
      subSubGroup->GetLastParameter()->AddSetLine("}");
      subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
      subSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetMidasDAQ()->is%sEventActive())",eventName[i].Data());
      subSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
      subSubGroup->GetLastParameter()->AddWriteLine("   else");
      subSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
      subSubGroup->GetLastParameter()->AddWriteLine("}");
      subSubGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
      subSubGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
      subSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
      subSubGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
      subSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"true\";");
      subSubGroup->GetLastParameter()->AddAdditionalWriteLine("}");
      for (j=0;j<numOfBank[i];j++) {
         subSubSubGroup = new ROMEConfigParameterGroup(bankName[i][j],"1","Bank");
         subSubSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Bank");
         subSubSubGroup->SetWriteEmptyLine(kFALSE);
         subSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
         subSubSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, "Bank");
         subSubSubGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->IsMidasDAQ()) {");
         subSubSubGroup->GetLastParameter()->AddSetLine("   if (##==\"true\")");
         subSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetMidasDAQ()->Set%sBankActive(true);",bankName[i][j].Data());
         subSubSubGroup->GetLastParameter()->AddSetLine("   else");
         subSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->GetMidasDAQ()->Set%sBankActive(false);",bankName[i][j].Data());
         subSubSubGroup->GetLastParameter()->AddSetLine("}");
         subSubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsMidasDAQ()) {");
         subSubSubGroup->GetLastParameter()->AddWriteLine("   if (gAnalyzer->GetMidasDAQ()->is%sBankActive())",bankName[i][j].Data());
         subSubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
         subSubSubGroup->GetLastParameter()->AddWriteLine("   else");
         subSubSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
         subSubSubGroup->GetLastParameter()->AddWriteLine("}");
         subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("else {");
         subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("   if (##Modified)");
         subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = ##;");
         subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("   else");
         subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("      writeString = \"true\";");
         subSubSubGroup->GetLastParameter()->AddAdditionalWriteLine("}");
         subSubGroup->AddSubGroup(subSubSubGroup);
      }
   }
   return true;
}

Bool_t ROMEBuilder::AddTaskConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex)
{
   int i,j;
   ROMEString name;
   ROMEString steerPointerT;
   ROMEString taskPointerT;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (taskHierarchyParentIndex[i]!=parentIndex)
         continue;
      name.SetFormatted("%s%s",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      subGroup = new ROMEConfigParameterGroup(name.Data(),"1","Task","TaskName","",taskHierarchyName[i].Data(),taskHierarchyMultiplicity[i]);
      subGroup->SetComment(ROMEConfig::kCommentLevelObj, taskShortDescription[taskHierarchyClassIndex[i]]);
      // Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, "Task");
      subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTaskObjectAt(%d)->SetActive(true);",taskHierarchyObjectIndex[i]);
      subGroup->GetLastParameter()->AddSetLine("else {");
      if (taskHierarchyParentIndex[i]==-1)
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetMainHistoFolder()->Remove(gAnalyzer->GetHistoFolderAt(%d));",i);
      else
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetHistoFolderAt(%d)->Remove(gAnalyzer->GetHistoFolderAt(%d));",taskHierarchyParentIndex[i],i);
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTaskObjectAt(%d)->SetActive(false);",taskHierarchyObjectIndex[i]);
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetTaskObjectAt(%d)->IsActive())",taskHierarchyObjectIndex[i]);
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      parGroup->AddSubGroup(subGroup);
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         subSubGroup = new ROMEConfigParameterGroup(histoName[taskHierarchyClassIndex[i]][j],"1","Histogram","HistName");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Histogram");
         subSubGroup->AddParameter(new ROMEConfigParameter("HistActive"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetActive(true);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetActive(false);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("if (((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->IsActive())",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         subSubGroup->AddParameter(new ROMEConfigParameter("HistTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetTitle(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetTitle();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistFolderTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetFolderTitle(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetFolderTitle();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistArraySize"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetArraySize(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetArraySize());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistArrayStartIndex"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetArrayStartIndex(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetArrayStartIndex());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetXLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetXLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetYLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetYLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetZLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetZLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXNbins"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetXNbins(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetXNbins());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetXmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetXmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetXmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetXmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYNbins"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetYNbins(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetYNbins());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetYmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetYmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetYmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetYmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZNbins"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetZNbins(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetZNbins());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetZmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetZmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetZmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->GetZmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistAccumulate","1","CheckButton"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Histogram");
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"false\")");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetAccumulation(false);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->SetAccumulation(true);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine(" ");
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("if (((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sHisto()->isAccumulation())",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         subGroup->AddSubGroup(subSubGroup);
      }
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         subSubGroup = new ROMEConfigParameterGroup(graphName[taskHierarchyClassIndex[i]][j],"1","Graph","GraphName");
         subSubGroup->ReadComment(ROMEConfig::kCommentLevelGroup,"Graph");
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphActive"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetActive(true);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetActive(false);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("if (((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->IsActive())",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetTitle(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetTitle();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphFolderTitle"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetFolderTitle(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetFolderTitle();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphArraySize"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetArraySize(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetArraySize());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphArrayStartIndex"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetArrayStartIndex(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetArrayStartIndex());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphXLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetXLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetXLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphYLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetYLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetYLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphZLabel"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetZLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetZLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphXmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetXmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetXmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphXmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetXmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetXmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphYmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetYmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetYmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphYmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetYmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetYmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphZmin"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetZmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetZmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("GraphZmax"));
         subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Graph");
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->SetZmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%g\",((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->Get%sGraph()->GetZmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],graphName[taskHierarchyClassIndex[i]][j].Data());
         subGroup->AddSubGroup(subSubGroup);
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         steerPointerT.SetFormatted("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         taskPointerT.SetFormatted("((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         AddSteeringConfigParameters(subGroup,0,taskHierarchyClassIndex[i],steerPointerT,taskPointerT);
      }
      AddTaskConfigParameters(subGroup,i);
   }

   return true;
}

Bool_t ROMEBuilder::AddTabConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex)
{
   int i,j;
   ROMEString switchString;
   ROMEString steerPointerT;
   ROMEString tabPointerT;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      if (tabParentIndex[i] != parentIndex)
         continue;
      subGroup = new ROMEConfigParameterGroup(tabName[i],"1","Tab","TabName");
      subGroup->SetComment(ROMEConfig::kCommentLevelObj, tabShortDescription[i]);
      // Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelAll, "Tab");
      subGroup->GetLastParameter()->AddSetLine("if (##==\"false\") {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->GetTabSwitches()->%s%s = kFALSE;", tabName[i].Data(),tabSuffix[i].Data());
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->Get%s%sTab()->SetTabActive(kFALSE);", tabName[i].Data(),tabSuffix[i].Data());
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddSetLine("else {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->GetTabSwitches()->%s%s = kTRUE;", tabName[i].Data(),tabSuffix[i].Data());
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->Get%s%sTab()->SetTabActive(kTRUE);", tabName[i].Data(),tabSuffix[i].Data());
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetTabSwitches()->%s%s)", tabName[i].Data(),tabSuffix[i].Data());
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      if (tabObjectDisplay[i]) {
         for (j=0;j<numOfTabObjectDisplays[i];j++) {
            subSubGroup = new ROMEConfigParameterGroup(tabObjectDisplayName[i][j],"1","ObjectDisplay","ObjectDisplayName");
            subSubGroup->ReadComment(ROMEConfig::kCommentLevelObj, "ObjectDisplay");
            // Draw Option
            subSubGroup->AddParameter(new ROMEConfigParameter("DrawOption"));
            subSubGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "ObjectDisplay");
            subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->Get%s%sTab()->SetDrawOptionAt(%d,##.Data());", tabName[i].Data(),tabSuffix[i].Data(),j);
            subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%s\",gAnalyzer->GetWindow()->Get%s%sTab()->GetDrawOptionAt(%d));", tabName[i].Data(),tabSuffix[i].Data(),j);
            subGroup->AddSubGroup(subSubGroup);
         }
         // Number Of Pads X
         subGroup->AddParameter(new ROMEConfigParameter("NumberOfPadsX"));
         subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
         subGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->Get%s%sTab()->SetNumberOfPadsX(##.ToInteger());", tabName[i].Data(),tabSuffix[i].Data());
         subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetWindow()->Get%s%sTab()->GetNumberOfPadsX());", tabName[i].Data(),tabSuffix[i].Data());
         // Number Of Pads Y
         subGroup->AddParameter(new ROMEConfigParameter("NumberOfPadsY"));
         subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
         subGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->Get%s%sTab()->SetNumberOfPadsY(##.ToInteger());", tabName[i].Data(),tabSuffix[i].Data());
         subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetWindow()->Get%s%sTab()->GetNumberOfPadsY());", tabName[i].Data(),tabSuffix[i].Data());
         // Pad Configuration
         subGroup->AddParameter(new ROMEConfigParameter("PadConfiguration","1","CheckButton"));
         subGroup->GetLastParameter()->ReadComment(ROMEConfig::kCommentLevelParam, "Tab");
         subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->Get%s%sTab()->SetPadConfigActive(kTRUE);", tabName[i].Data(),tabSuffix[i].Data());
         subGroup->GetLastParameter()->AddSetLine("else");
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->Get%s%sTab()->SetPadConfigActive(kFALSE);", tabName[i].Data(),tabSuffix[i].Data());
         subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->Get%s%sTab()->IsPadConfigActive())", tabName[i].Data(),tabSuffix[i].Data());
         subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subGroup->GetLastParameter()->AddWriteLine("else");
         subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      }
      parGroup->AddSubGroup(subGroup);
      if (numOfSteering[i+numOfTask+1]>0) {
         steerPointerT.SetFormatted("gAnalyzer->GetWindow()->Get%s%sTab()->GetSP()", tabName[i].Data(), tabSuffix[i].Data());
         tabPointerT.SetFormatted("gAnalyzer->GetWindow()->Get%s%sTab()", tabName[i].Data(), tabSuffix[i].Data());
         AddSteeringConfigParameters(subGroup,0,i+numOfTask+1,steerPointerT,tabPointerT);
      }
      AddTabConfigParameters(subGroup,i);
   }

   return true;
}

Bool_t ROMEBuilder::AddSteeringConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t numSteer,Int_t numTask,ROMEString steerPointer,ROMEString taskPointer)
{
   int i;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   ROMEString tmp;
   for (i=0;i<numOfSteerFields[numTask][numSteer];i++) {
      if (!steerFieldUsed[numTask][numSteer][i])
         continue;
      subGroup = new ROMEConfigParameterGroup(steerFieldName[numTask][numSteer][i].Data(),"1","SteeringParameterField","SPName");
      subGroup->SetWriteEmptyLine(kFALSE);
      subGroup->SetComment(ROMEConfig::kCommentLevelSP, ProcessCommentString(steerFieldShortDescription[numTask][numSteer][i],tmp).Data());
      parGroup->AddSubGroup(subGroup);
      if (subGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
         maxConfigParameterHierarchyLevel = subGroup->GetHierarchyLevel();
      if (steerFieldArraySize[numTask][numSteer][i]!="1") {
         subSubGroup = new ROMEConfigParameterGroup("SPFieldArray",steerFieldArraySize[numTask][numSteer][i].Data(),"","","SPFieldArrayIndex");
         subGroup->AddSubGroup(subSubGroup);
         if (subSubGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
            maxConfigParameterHierarchyLevel = subSubGroup->GetHierarchyLevel();
         if (isBoolType(steerFieldType[numTask][numSteer][i].Data()))
            subSubGroup->AddParameter(new ROMEConfigParameter("SPFieldArrayValue","1","CheckButton"));
         else
            subSubGroup->AddParameter(new ROMEConfigParameter("SPFieldArrayValue","1"));
         subSubGroup->GetLastParameter()->SetComment(ROMEConfig::kCommentLevelSP, ProcessCommentString(steerFieldShortDescription[numTask][numSteer][i],tmp).Data());
         setValue(&decodedValue,"","##",steerFieldType[numTask][numSteer][i].Data(),1);
         subSubGroup->GetLastParameter()->AddSetLine("if (%s)",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i]=="std::string")
            subSubGroup->GetLastParameter()->AddSetLine("   %s->Set%sAt(ii[%d],##.Data());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
         else
            subSubGroup->GetLastParameter()->AddSetLine("   %s->Set%sAt(ii[%d],(%s)%s);",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel(),steerFieldType[numTask][numSteer][i].Data(),decodedValue.Data());
         subSubGroup->GetLastParameter()->AddWriteLine("if (%s) {",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i]=="TString" || steerFieldType[numTask][numSteer][i]=="ROMEString")
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%s\",%s->Get%sAt(ii[%d]).Data());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
         else if (steerFieldType[numTask][numSteer][i]=="std::string")
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%s\",%s->Get%sAt(ii[%d]).c_str());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());

         else if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subSubGroup->GetLastParameter()->AddWriteLine("   if (%s->Get%sAt(ii[%d]))",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
            subSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
            subSubGroup->GetLastParameter()->AddWriteLine("   else");
            subSubGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
         } else {
            if (steerFieldFormat[numTask][numSteer][i].Length()) {
               subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%s\",%s->Get%sAt(ii[%d]));",steerFieldFormat[numTask][numSteer][i].Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
            } else {
               subSubGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(ROMEUtilities::GetFormat(typeid(%s)),%s->Get%sAt(ii[%d]));",steerFieldType[numTask][numSteer][i].Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
            }
         }
         subSubGroup->GetLastParameter()->AddWriteLine("}");
         subSubGroup->GetLastParameter()->AddWriteLine("else {");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString.Resize(0);");
         subSubGroup->GetLastParameter()->AddWriteLine("}");
      }
      else {
         if (isBoolType(steerFieldType[numTask][numSteer][i].Data()))
            subGroup->AddParameter(new ROMEConfigParameter("SPValue","1","CheckButton"));
         else
            subGroup->AddParameter(new ROMEConfigParameter("SPValue","1"));
         subGroup->GetLastParameter()->SetComment(ROMEConfig::kCommentLevelNever, ProcessCommentString(steerFieldShortDescription[numTask][numSteer][i],tmp).Data());
         setValue(&decodedValue,"","##",steerFieldType[numTask][numSteer][i].Data(),1);
         subGroup->GetLastParameter()->AddSetLine("if (%s)",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i]=="std::string")
            subGroup->GetLastParameter()->AddSetLine("   %s->Set%s(##.Data());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         else
            subGroup->GetLastParameter()->AddSetLine("   %s->Set%s((%s)%s);",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),steerFieldType[numTask][numSteer][i].Data(),decodedValue.Data());
         subGroup->GetLastParameter()->AddWriteLine("if (%s) {",taskPointer.Data());
         if (steerFieldType[numTask][numSteer][i]=="TString" || steerFieldType[numTask][numSteer][i]=="ROMEString")
            subGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%s\",%s->Get%s().Data());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         else if (steerFieldType[numTask][numSteer][i]=="std::string")
            subGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%%s\",%s->Get%s().c_str());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         else if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subGroup->GetLastParameter()->AddWriteLine("   if (%s->Get%s())",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
            subGroup->GetLastParameter()->AddWriteLine("      writeString = \"true\";");
            subGroup->GetLastParameter()->AddWriteLine("   else");
            subGroup->GetLastParameter()->AddWriteLine("      writeString = \"false\";");
         } else {
            if (steerFieldFormat[numTask][numSteer][i].Length()) {
               subGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(\"%s\",%s->Get%s());",steerFieldFormat[numTask][numSteer][i].Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
            } else {
               subGroup->GetLastParameter()->AddWriteLine("   writeString.SetFormatted(ROMEUtilities::GetFormat(typeid(%s)),%s->Get%s());",steerFieldType[numTask][numSteer][i].Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
            }
         }
         subGroup->GetLastParameter()->AddWriteLine("}");
         subGroup->GetLastParameter()->AddWriteLine("else {");
         subGroup->GetLastParameter()->AddWriteLine("   writeString.Resize(0);");
         subGroup->GetLastParameter()->AddWriteLine("}");
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      subGroup = new ROMEConfigParameterGroup(steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"1","SteeringParameterGroup","SPGroupName");
      parGroup->AddSubGroup(subGroup);
      if (subGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
         maxConfigParameterHierarchyLevel = subGroup->GetHierarchyLevel();
      subSubGroup = subGroup;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]!="1") {
         subSubGroup = new ROMEConfigParameterGroup("SPGroupArray",steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),"","","SPGroupArrayIndex");
         subGroup->AddSubGroup(subSubGroup);
         if (subSubGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
            maxConfigParameterHierarchyLevel = subSubGroup->GetHierarchyLevel();
         steerPointerT.SetFormatted("%s->Get%sAt(ii[%d])",steerPointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),subSubGroup->GetHierarchyLevel());
      }
      else
         steerPointerT.SetFormatted("%s->Get%s()",steerPointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      AddSteeringConfigParameters(subSubGroup,steerChildren[numTask][numSteer][i],numTask,steerPointerT,taskPointer);
   }
   return true;
}

Bool_t ROMEBuilder::CheckConfigParameters(ROMEConfigParameterGroup *parGroup)
{
   int i;
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->IsWriteLinesAlways() && parGroup->GetParameterAt(i)->GetNumberOfWriteLines()>0) {
         parGroup->SetWriteAlways();
         break;
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      CheckConfigParameters(parGroup->GetSubGroupAt(i));
      if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
         parGroup->SetWriteAlways();
   }
   return true;
}

Bool_t ROMEBuilder::WriteConfigClass(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab)
{
   int i;
   bool found = false;
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";
   buffer.AppendFormatted("%s// %s;\n",sTab.Data(),parGroup->GetGroupName().Data());
   if (parGroup->GetGroupIdentifier()=="Histogram") {
      buffer.AppendFormatted("%sROMEConfigHisto* f%s;\n",sTab.Data(),parGroup->GetGroupName().Data());
      buffer.AppendFormatted("%sBool_t f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier()=="Graph") {
      buffer.AppendFormatted("%sROMEConfigGraph* f%s;\n",sTab.Data(),parGroup->GetGroupName().Data());
      buffer.AppendFormatted("%sBool_t f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
      return true;
   }
   buffer.AppendFormatted("%sclass %s {\n",sTab.Data(),parGroup->GetGroupName().Data());
   buffer.AppendFormatted("%spublic:\n",sTab.Data());
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s   ROMEString   f%s;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
         buffer.AppendFormatted("%s   Bool_t       f%sModified;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
      }
      else {
         buffer.AppendFormatted("%s   ROMEString   f%s[%s];\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName(),parGroup->GetParameterAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sModified[%s];\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName(),parGroup->GetParameterAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sArrayModified;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
      }
   }
   if (parGroup->GetGroupName()=="ConfigData") {
      buffer.AppendFormatted("%s   Int_t        fLastRunNumberIndex;\n",sTab.Data());
      buffer.AppendFormatted("%s   TArrayL64    fRunNumberArray;\n",sTab.Data());
      buffer.AppendFormatted("%s   Int_t        fLastInputFileNameIndex;\n",sTab.Data());
      buffer.AppendFormatted("%s   ROMEStrArray fInputFileNameArray;\n",sTab.Data());
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      WriteConfigClass(buffer,parGroup->GetSubGroupAt(i),tab+1);
   }
   // Constructor
   buffer.AppendFormatted("%s   %s() {\n",sTab.Data(),parGroup->GetGroupName().Data());
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         found = true;
         break;
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups() && !found;i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1" && parGroup->GetSubGroupAt(i)->GetArraySize()!="unknown") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         break;
      }
   }
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s      f%sModified = false;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
      }
      else {
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++)\n",sTab.Data(),parGroup->GetParameterAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
         buffer.AppendFormatted("%s      f%sArrayModified = false;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Histogram") {
         buffer.AppendFormatted("%s      f%s = new ROMEConfigHisto();\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sModified = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Graph") {
         buffer.AppendFormatted("%s      f%s = new ROMEConfigGraph();\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sModified = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s      f%s = new %s();\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sModified = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%s      f%s = 0;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sModified = 0;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sArrayModified = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sArraySize = 0;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else {
         buffer.AppendFormatted("%s      f%sModified = new Bool_t[%s];\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s      f%s = new %s*[%s];\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s         f%s[i] = new %s();\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s      f%sArrayModified = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
   }
   buffer.AppendFormatted("%s   }\n",sTab.Data());
   // Destructor
   buffer.AppendFormatted("%s   ~%s() {\n",sTab.Data(),parGroup->GetGroupName().Data());
   found = false;
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         found = true;
         break;
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups() && !found;i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%s      int i;\n",sTab.Data());
         break;
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Histogram" ||
          parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Graph" ||
          parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s      SafeDelete(f%s);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%s      for (i=0;i<f%sArraySize;i++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());         buffer.AppendFormatted("%s         SafeDelete(f%s[i]);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%sModified);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%s);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else {
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s         SafeDelete(f%s[i]);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%s);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      SafeDeleteArray(f%sModified);\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
   }
   buffer.AppendFormatted("%s   }\n",sTab.Data());
   buffer.AppendFormatted("%s};\n",sTab.Data());
   if (parGroup->GetGroupName()=="ConfigData") {
      buffer.AppendFormatted("%s%s** f%s;\n",sTab.Data(),parGroup->GetGroupName().Data(),parGroup->GetGroupName().Data());
   }
   else {
      if (parGroup->GetArraySize()=="1") {
         buffer.AppendFormatted("%s%s* f%s;\n",sTab.Data(),parGroup->GetGroupName().Data(),parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sBool_t f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
      }
      else {
         buffer.AppendFormatted("%s%s** f%s;\n",sTab.Data(),parGroup->GetGroupName().Data(),parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sBool_t *f%sModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sBool_t f%sArrayModified;\n",sTab.Data(),parGroup->GetGroupName().Data());
         buffer.AppendFormatted("%sInt_t  f%sArraySize;\n",sTab.Data(),parGroup->GetGroupName().Data());
      }
   }

   return true;
}

Bool_t ROMEBuilder::WriteConfigRead(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes,Int_t *iSub)
{
   int i,j;
   ROMEString classNameT;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString indexesT;
   ROMEString temp;
   ROMEString sTabT = "";
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";

   // Parameters
   if (parGroup->GetGroupIdentifier()=="Histogram") {
      pointerT = pointer(0,pointer.Length()-2);
      buffer.AppendFormatted("%sReadHistoConfiguration(xml,path+\"/%s\",%s);\n",sTab.Data(),groupName.Data(),pointerT.Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier()=="Graph") {
      pointerT = pointer(0,pointer.Length()-2);
      buffer.AppendFormatted("%sReadGraphConfiguration(xml,path+\"/%s\",%s);\n",sTab.Data(),groupName.Data(),pointerT.Data());
      return true;
   }
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName());
      if (indexes.Length()==0)
         buffer.AppendFormatted("%sxml->GetPathValue(path+\"/%s%s\",%sf%s,\"\");\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
      else {
         buffer.AppendFormatted("%stempPath.SetFormatted(\"/%s%s\"%s);\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName(),indexes.Data());
         buffer.AppendFormatted("%sxml->GetPathValue(path+tempPath,%sf%s,\"\");\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
      }
      buffer.AppendFormatted("%s%sf%sModified = (%sf%s!=\"\");\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
      if (parGroup->GetParameterAt(i)->GetNumberOfReadModifiedTrueLines()>0) {
         buffer.AppendFormatted("%sif (%sf%s==\"\") {\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfReadModifiedTrueLines();j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetParameterAt(i)->GetReadModifiedTrueLineAt(j));
         }
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }
   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   %sf%sArraySize = xml->NumberOfOccurrenceOfPath(path+\"/%s%ss/%s\");\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfReadGroupArrayInitLines();j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetReadGroupArrayInitLineAt(j));
         }
         buffer.AppendFormatted("%s   ROMEString tempPath;\n",sTab.Data());
         buffer.AppendFormatted("%s   %sf%s = new ConfigData::%s%s*[%sf%sArraySize];\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),className.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s   %sf%sModified = new bool[%sf%sArraySize];\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s   for (i=0;i<%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      %sf%s[i] = new ConfigData::%s%s();\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),className.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (ii[%d]=0;ii[%d]<%s;ii[%d]++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
      }
      indexesT = indexes;
      classNameT.SetFormatted("%s%s::",className.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      sTabT = sTab.Data();
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
         sTabT += "      ";
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
         sTabT += "   ";
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         groupNameT.SetFormatted("%s%ss[%d]/%s[%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
         indexesT.AppendFormatted(",i+1");
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         WriteConfigRead(buffer,parGroup->GetSubGroupAt(i),tab+2,groupNameT,classNameT,pointerT,indexesT,iSub);
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         else
            groupNameT.SetFormatted("%s%s[%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         indexesT.AppendFormatted(",ii[%d]",parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         WriteConfigRead(buffer,parGroup->GetSubGroupAt(i),tab+1,groupNameT,classNameT,pointerT,indexesT,iSub);
      }
      else {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         else
            groupNameT.SetFormatted("%s%s[%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Histogram") {
            buffer.AppendFormatted("%sROMEConfigHisto* tmp%s%d;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub,pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Graph") {
            buffer.AppendFormatted("%sROMEConfigGraph* tmp%s%d;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub,pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else {
            temp = classNameT(0,classNameT.Length()-2);
            buffer.AppendFormatted("%sConfigData::%s* tmp%s%d;\n",sTab.Data(),temp.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub,pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         pointerT.SetFormatted("tmp%s%d->",parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
         (*iSub)++;
         WriteConfigRead(buffer,parGroup->GetSubGroupAt(i),tab,groupNameT,classNameT,pointerT,indexes,iSub);
      }
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
         buffer.AppendFormatted("%s}\n",sTab.Data());
   }
   return true;
}

Bool_t ROMEBuilder::WriteConfigCheckModified(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes,Int_t* iSub)
{
   int i,j;
   ROMEString classNameT;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString indexesT;
   ROMEString temp;
   ROMEString sTabT = "";
   ROMEString sTab = "";
   int iSubSave=0;
   for (i=0;i<tab;i++)
      sTab += "   ";

   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   for (i=0;i<%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (ii[%d]=0;ii[%d]<%s;ii[%d]++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
      }
      indexesT = indexes;
      classNameT.SetFormatted("%s%s::",className.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      sTabT = sTab.Data();
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
         sTabT += "      ";
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
         sTabT += "   ";
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         groupNameT.SetFormatted("%s%ss[%d]/%s[%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
         indexesT.AppendFormatted(",i+1");
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         WriteConfigCheckModified(buffer,parGroup->GetSubGroupAt(i),tab+2,groupNameT,classNameT,pointerT,indexesT,iSub);
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         else
            groupNameT.SetFormatted("%s%s[%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         indexesT.AppendFormatted(",ii[%d]",parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         WriteConfigCheckModified(buffer,parGroup->GetSubGroupAt(i),tab+1,groupNameT,classNameT,pointerT,indexesT,iSub);
      }
      else {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         else
            groupNameT.SetFormatted("%s%s[%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Histogram") {
            buffer.AppendFormatted("%sROMEConfigHisto* tmp%s%d;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub,pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Graph") {
            buffer.AppendFormatted("%sROMEConfigGraph* tmp%s%d;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub,pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else {
            temp = classNameT(0,classNameT.Length()-2);
            buffer.AppendFormatted("%sConfigData::%s* tmp%s%d;\n",sTab.Data(),temp.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
            buffer.AppendFormatted("%stmp%s%d = %sf%s;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub,pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         pointerT.SetFormatted("tmp%s%d->",parGroup->GetSubGroupAt(i)->GetGroupName().Data(),*iSub);
         iSubSave = *iSub;
         (*iSub)++;
         WriteConfigCheckModified(buffer,parGroup->GetSubGroupAt(i),tab,groupNameT,classNameT,pointerT,indexes,iSub);
      }
      if (parGroup->GetSubGroupAt(i)->GetNumberOfParameters()+parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups()>0) {
         buffer.AppendFormatted("%s// %s%s--\n",sTabT.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%sif (",sTabT.Data());
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Histogram") {
            buffer.AppendFormatted("CheckHistoConfigurationModified(%sf%s)",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier()=="Graph") {
            buffer.AppendFormatted("CheckGraphConfigurationModified(%sf%s)",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else {
            for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfParameters();j++) {
               if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
                  buffer.AppendFormatted("%sf%s[i]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetParameterAt(j)->GetName());
               else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
                  buffer.AppendFormatted("%sf%s[ii[%d]]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetParameterAt(j)->GetName());
               else
                  buffer.AppendFormatted("tmp%s%d->f%sModified",parGroup->GetSubGroupAt(i)->GetGroupName().Data(),iSubSave,parGroup->GetSubGroupAt(i)->GetParameterAt(j)->GetName());
               if (j<parGroup->GetSubGroupAt(i)->GetNumberOfParameters()-1 || parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups()!=0)
                  buffer.AppendFormatted(" ||\n%s    ",sTabT.Data());
            }
            for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups();j++) {
               if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
                  buffer.AppendFormatted("%sf%s[i]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
               else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
                  buffer.AppendFormatted("%sf%s[ii[%d]]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
               else
                  buffer.AppendFormatted("tmp%s%d->f%sModified",parGroup->GetSubGroupAt(i)->GetGroupName().Data(),iSubSave,parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
               if (j<parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups()-1)
                  buffer.AppendFormatted(" ||\n%s    ",sTabT.Data());
            }
         }
         buffer.AppendFormatted(") {\n");
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%s   %sf%sModified[i] = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            buffer.AppendFormatted("%s   %sf%sArrayModified = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
            buffer.AppendFormatted("%s   %sf%sModified[ii[%d]] = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            buffer.AppendFormatted("%s   %sf%sArrayModified = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else {
            buffer.AppendFormatted("%s   %sf%sModified = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         buffer.AppendFormatted("%s}\n",sTabT.Data());
         buffer.AppendFormatted("%selse {\n",sTabT.Data());
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%s   %sf%sModified[i] = false;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
            buffer.AppendFormatted("%s   %sf%sModified[ii[%d]] = false;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         else {
            buffer.AppendFormatted("%s   %sf%sModified = false;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         buffer.AppendFormatted("%s}\n",sTabT.Data());
      }
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
         buffer.AppendFormatted("%s}\n",sTab.Data());
   }
   return true;
}

Bool_t ROMEBuilder::WriteConfigSet(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer,Int_t* iSub)
{
   int i,j,tabT;
   ROMEString buf;
   ROMEString temp;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";
   // Parameters
   if (parGroup->GetGroupIdentifier()=="Histogram") {
      pointerT = pointer(0,pointer.Length()-2);
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0,temp.Last('-'));
      buffer.AppendFormatted("%sSetHistoConfiguration(%s,configData->%s);\n",sTab.Data(),temp.Data(),pointerT.Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier()=="Graph") {
      pointerT = pointer(0,pointer.Length()-2);
      temp = parGroup->GetParameterAt(1)->GetSetLineAt(0);
      temp = temp(0,temp.Last('-'));
      buffer.AppendFormatted("%sSetGraphConfiguration(%s,configData->%s);\n",sTab.Data(),temp.Data(),pointerT.Data());
      return true;
   }
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetNumberOfSetLines()>0) {
         buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName());
         buffer.AppendFormatted("%sif (modConfigData->%sf%sModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfSetLines();j++) {
            buf = parGroup->GetParameterAt(i)->GetSetLineAt(j);
            temp.SetFormatted("configData->%sf%s",pointer.Data(),parGroup->GetParameterAt(i)->GetName());
            buf.ReplaceAll("##",temp);
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),buf.Data());
         }
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }
   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      tabT = tab;
      groupNameT.SetFormatted("%s%s/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      pointerT.SetFormatted("%sf%s->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   if (modConfigData->%sf%sArrayModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      for (i=0;i<configData->%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s         if (modConfigData->%sf%sModified[i]) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         tabT += 4;
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (ii[%d]=0;ii[%d]<%s;ii[%d]++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         tabT += 1;
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
      }
      WriteConfigSet(buffer,parGroup->GetSubGroupAt(i),tabT,groupNameT,pointerT,iSub);
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%s         }\n",sTab.Data());
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%s}\n",sTab.Data());
      }
   }
   return true;
}

Bool_t ROMEBuilder::WriteConfigWrite(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer,Int_t* iSub)
{
   int i,j,tabT;
   ROMEString buf;
   ROMEString temp;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString sTabT;
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";
   // Parameters
   if (parGroup->GetGroupIdentifier()=="Histogram") {
      pointerT = pointer(0,pointer.Length()-2);
      buffer.AppendFormatted("%sWriteHistoConfiguration(xml,configData->%s,fCommentLevel,histoComments);\n",sTab.Data(),pointerT.Data());
      return true;
   }
   if (parGroup->GetGroupIdentifier()=="Graph") {
      pointerT = pointer(0,pointer.Length()-2);
      buffer.AppendFormatted("%sWriteGraphConfiguration(xml,configData->%s,fCommentLevel,graphComments);\n",sTab.Data(),pointerT.Data());
      return true;
   }
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName());
      if (!parGroup->GetParameterAt(i)->IsWriteLinesAlways() || parGroup->GetParameterAt(i)->GetNumberOfWriteLines()==0) {
         if (parGroup->GetParameterAt(i)->GetComment().Length()) {
            buffer.AppendFormatted("%sif (fCommentLevel >= %d && configData->%sf%sModified)\n",sTab.Data(),parGroup->GetParameterAt(i)->GetCommentLevel(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
            buffer.AppendFormatted("%s   xml->WriteComment(\"%s\");\n",sTab.Data(),ProcessCommentString(parGroup->GetParameterAt(i)->GetComment(),temp).Data());
         }
         buffer.AppendFormatted("%s",sTab.Data());
      }
      else {
         if (parGroup->GetParameterAt(i)->GetComment().Length()) {
            buffer.AppendFormatted("%sif (fCommentLevel >= %d && (configData->%sf%sModified || index==0))\n",sTab.Data(),parGroup->GetParameterAt(i)->GetCommentLevel(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
            buffer.AppendFormatted("%s   xml->WriteComment(\"%s\");\n",sTab.Data(),ProcessCommentString(parGroup->GetParameterAt(i)->GetComment(),temp).Data());
         }
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfWriteLines();j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetParameterAt(i)->GetWriteLineAt(j));
         }
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfAdditionalWriteLines();j++) {
            buf = parGroup->GetParameterAt(i)->GetAdditionalWriteLineAt(j);
            temp.SetFormatted("configData->%sf%s",pointer.Data(),parGroup->GetParameterAt(i)->GetName());
            buf.ReplaceAll("##",temp);
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),buf.Data());
         }
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",writeString.Data());\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName());
         buffer.AppendFormatted("%s}\n",sTab.Data());
         buffer.AppendFormatted("%selse ",sTab.Data());
      }
      buffer.AppendFormatted("if (configData->%sf%sModified) {\n",pointer.Data(),parGroup->GetParameterAt(i)->GetName());
      buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",configData->%sf%s.Data());\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName(),pointer.Data(),parGroup->GetParameterAt(i)->GetName());
         buffer.AppendFormatted("%s}\n",sTab.Data());
   }
   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      sTabT = sTab;
      tabT = tab;
      groupNameT.SetFormatted("%s%s/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      pointerT.SetFormatted("%sf%s->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
            buffer.AppendFormatted("%sif (configData->%sf%sArrayModified || index==0) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         else
            buffer.AppendFormatted("%sif (configData->%sf%sArrayModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         if (i != 0 && parGroup->GetSubGroupAt(i)->GetWriteEmptyLine())
            buffer.AppendFormatted("%s   xml->WriteEmptyLine();\n",sTab.Data());
         buffer.AppendFormatted("%s   xml->StartElement(\"%ss\");\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
         buffer.AppendFormatted("%s   for (i=0;i<configData->%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      if (configData->%sf%sModified[i]) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         sTabT += "      ";
         tabT += 2;
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (ii[%d]=0;ii[%d]<%s;ii[%d]++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
            buffer.AppendFormatted("%s   if (configData->%sf%sModified[ii[%d]] || index==0) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         else
            buffer.AppendFormatted("%s   if (configData->%sf%sModified[ii[%d]]) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         tabT += 1;
         sTabT += "   ";
         pointerT.SetFormatted("%sf%s[ii[%d]]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
      }
      else {
         if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
            buffer.AppendFormatted("%sif (configData->%sf%sModified || index==0) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         else
            buffer.AppendFormatted("%sif (configData->%sf%sModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfWriteStartLines();j++) {
         buffer.AppendFormatted("%s   %s\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetWriteStartLineAt(j));
      }
      if (i != 0 && parGroup->GetSubGroupAt(i)->GetWriteEmptyLine())
         buffer.AppendFormatted("%s   xml->WriteEmptyLine();\n",sTab.Data());
      if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0) {
         buffer.AppendFormatted("%s   xml->StartElement(\"%s\");\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data());
         if (parGroup->GetSubGroupAt(i)->GetComment().Length()) {
            buffer.AppendFormatted("%s   if (fCommentLevel >= %d)\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetCommentLevel());
            buffer.AppendFormatted("%s      xml->WriteComment(\"%s\");\n",sTabT.Data(),ProcessCommentString(parGroup->GetSubGroupAt(i)->GetComment(),temp).Data());
         }
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",\"%s\");\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
      }
      else {
         buffer.AppendFormatted("%s   xml->StartElement(\"%s\");\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
         if (parGroup->GetSubGroupAt(i)->GetComment().Length()) {
            buffer.AppendFormatted("%s   if (fCommentLevel >= %d)\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetCommentLevel());
            buffer.AppendFormatted("%s      xml->WriteComment(\"%s\");\n",sTabT.Data(),ProcessCommentString(parGroup->GetSubGroupAt(i)->GetComment(),temp).Data());
         }
      }
      if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1" && parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Length()) {
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
            buffer.AppendFormatted("%s   str.SetFormatted(\"%%d\",i);\n",sTabT.Data());
         else
            buffer.AppendFormatted("%s   str.SetFormatted(\"%%d\",ii[%d]);\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         if (parGroup->GetSubGroupAt(i)->GetComment().Length()) {
            buffer.AppendFormatted("%s   if (fCommentLevel >= %d)\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetCommentLevel());
            buffer.AppendFormatted("%s      xml->WriteComment(\"%s\");\n",sTabT.Data(),ProcessCommentString(parGroup->GetSubGroupAt(i)->GetComment(),temp).Data());
         }
         temp = parGroup->GetSubGroupAt(i)->GetArrayIdentifier();
         if (temp.EndsWith("="))
            temp.Resize(temp.Length()-1);
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",str.Data());\n",sTabT.Data(),temp.Data());
      }
      WriteConfigWrite(buffer,parGroup->GetSubGroupAt(i),tabT+1,groupNameT,pointerT,iSub);
      buffer.AppendFormatted("%s   xml->EndElement();\n",sTabT.Data());
      for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfWriteEndLines();j++) {
         buffer.AppendFormatted("%s   %s\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetWriteEndLineAt(j));
      }
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%s      }\n",sTab.Data());
         buffer.AppendFormatted("%s   }\n",sTab.Data());
         buffer.AppendFormatted("%s   xml->EndElement();\n",sTab.Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%s   }\n",sTab.Data());
      }
      buffer.AppendFormatted("%s}\n",sTab.Data());
   }
   return true;
}

Bool_t ROMEBuilder::WriteMidasDAQCpp() {
   int i,j,k;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString format;

   hasStructuredBank = false;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sMidasDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sMidasDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the midas odb access for %s%s.",shortCut.Data(),mainProgName.Data());
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
   buffer.AppendFormatted("%sMidasDAQ::%sMidasDAQ() {\n",shortCut.Data(),shortCut.Data());
   // Midas Banks
   buffer.AppendFormatted("   // Midas Banks\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("   f%sBankActive = true;\n",bankName[i][j].Data());
      }
      buffer.AppendFormatted("   f%sEventActive = true;\n",eventName[i].Data());
   }
   // Event Requests
   buffer.AppendFormatted("   // Event Requests\n");
   buffer.AppendFormatted("   SetNumberOfEventRequests(%d);\n",numOfEvent);
   for (i=0;i<numOfEvent;i++) {
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
   buffer.AppendFormatted("Bool_t %sMidasDAQ::InitHeader() {\n",shortCut.Data());
   if (bankHasHeader) {
      if (bankHeaderEventID!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)GetRawDataEvent())->event_id);\n",bankHeaderFolder.Data(),bankHeaderEventID.Data());
      if (bankHeaderTriggerMask!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)GetRawDataEvent())->trigger_mask);\n",bankHeaderFolder.Data(),bankHeaderTriggerMask.Data());
      if (bankHeaderSerialNumber!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)GetRawDataEvent())->serial_number);\n",bankHeaderFolder.Data(),bankHeaderSerialNumber.Data());
      if (bankHeaderTimeStamp!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)GetRawDataEvent())->time_stamp);\n",bankHeaderFolder.Data(),bankHeaderTimeStamp.Data());
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Initialize ODB
   if (midas) {
      ROMEString midType;
      buffer.AppendFormatted("// InitODB\n");
      buffer.AppendFormatted("Bool_t %sMidasDAQ::InitODB() {\n",shortCut.Data());
      ROMEString steerPath;
      buffer.AppendFormatted("   HNDLE hKey;\n");
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   ROMEString hotLinkString;\n");
      buffer.AppendFormatted("   // Hot Links\n");
      for (i=0;i<numOfTaskHierarchy;i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   str = \"/%s%s/Tasks/%s%s\";\n",shortCut.Data(),mainProgName.Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("   hotLinkString =  \"Active = BOOL : 0\\n");
         for (j=0;j<numOfSteering[i];j++) {
            for (k=0;k<numOfSteerFields[i][j];k++) {
               if (steerFieldHotLink[i][j][k]) {
                  GetSteerPath(steerPath,i,j,k,"_");
                  if (!toMidasODBType(steerFieldType[i][j][k],midType)) {
                     cout << "Steering parameter field " << steerFieldName[i][j][k].Data() << " of task " << taskHierarchyName[i].Data() << " can not be a hot link" << endl;
                     return false;
                  }
                  buffer.AppendFormatted("%s = %s : 0\\n",steerPath.Data(),midType.Data());
               }
            }
         }
         buffer.AppendFormatted("\";\n");
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)hotLinkString.Data(), TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);\n");
         buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,&f%s%sHotLinks,sizeof(%s%sHotLinks),0) != DB_SUCCESS) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot write to hot links.\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, &f%s%sHotLinks, sizeof(%s%sHotLinks), MODE_READ, HotLinksChanged, NULL) != DB_SUCCESS) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot open hot links, probably other analyzer is using it\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
      }
      bool hasHotLink = false;
      buffer.AppendFormatted("   str = \"/%s%s/Global Steering Parameters\";\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("   hotLinkString =  \"\\n");
      for (j=0;j<numOfSteering[numOfTaskHierarchy];j++) {
         for (k=0;k<numOfSteerFields[numOfTaskHierarchy][j];k++) {
            if (steerFieldHotLink[numOfTaskHierarchy][j][k]) {
               if (!toMidasODBType(steerFieldType[numOfTaskHierarchy][j][k],midType)) {
                  cout << "General steering parameter field " << steerFieldName[numOfTaskHierarchy][j][k].Data() << " can not be a hot link" << endl;
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
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)hotLinkString.Data(), TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);\n");
         buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,gAnalyzer->GetMidasDAQ()->GetGSPHotLinks(),sizeof(GSPHotLinks),0) != DB_SUCCESS) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot write to hot links.\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetMidasDAQ()->GetGSPHotLinks(), sizeof(GSPHotLinks), MODE_READ, HotLinksChanged, NULL) != DB_SUCCESS) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot open hot links, probably other analyzer is using it\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
      }
      for (i=0;i<numOfTab;i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("   str = \"/%s%s/Tabs/%s%s\";\n",shortCut.Data(),mainProgName.Data(),tabName[i].Data(),tabSuffix[i].Data());
         buffer.AppendFormatted("   hotLinkString =  \"Active = BOOL : 0\\n");
         for (j=0;j<numOfSteering[numOfTaskHierarchy+1+i];j++) {
            for (k=0;k<numOfSteerFields[numOfTaskHierarchy+1+i][j];k++) {
               if (steerFieldHotLink[numOfTaskHierarchy+1+i][j][k]) {
                  if (!toMidasODBType(steerFieldType[numOfTaskHierarchy+1+i][j][k],midType)) {
                     cout << "Steering parameter field " << steerFieldName[numOfTaskHierarchy+1+i][j][k].Data() << " of tab " << tabName[i].Data() << " can not be a hot link" << endl;
                     return false;
                  }
                  GetSteerPath(steerPath,numOfTaskHierarchy+1+i,j,k,"_");
                  buffer.AppendFormatted("%s = %s : 0\\n",steerPath.Data(),midType.Data());
               }
            }
         }
         buffer.AppendFormatted("\";\n");
         buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)hotLinkString.Data(), TRUE);\n");
         buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);\n");
         buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,&f%s%sHotLinks,sizeof(%s%sHotLinks),0) != DB_SUCCESS) {\n",tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot write to hot links.\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, &f%s%sHotLinks, sizeof(%s%sHotLinks), MODE_READ, HotLinksChanged, NULL) != DB_SUCCESS) {\n",tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Cannot open hot links, probably other analyzer is using it\\n\");\n");
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("   return true;\n");
      buffer.AppendFormatted("}\n\n");
   }

   // Midas Bank Initialisation
   buffer.AppendFormatted("// Midas Bank Initialisation\n");
   buffer.AppendFormatted("void %sMidasDAQ::InitMidasBanks() {\n",shortCut.Data());
   bool done = false;
   for (i=0;i<numOfEvent && !done;i++) {
      for (j=0;j<numOfBank[i] && !done;j++) {
         if (bankArrayDigit[i][j]>0) {
            buffer.AppendFormatted("   int i;\n");
            done = true;
         }
      }
   }
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            buffer.AppendFormatted("   for (i=%d;i<%d;i++)\n",bankArrayStart[i][j],bankArraySize[i][j]+bankArrayStart[i][j]);
            buffer.AppendFormatted("      Init%sBank(i);\n",bankName[i][j].Data());
         }
         // Single Bank
         else {
            buffer.AppendFormatted("   Init%sBank();\n",bankName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Midas Bank Methodes
   buffer.AppendFormatted("\n// Midas Bank Getters\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         buffer.AppendFormatted("// %s bank\n",bankName[i][j].Data());
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            // Functions
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(Int_t bankIndex,Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex]+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankPointer(Int_t bankIndex) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(Int_t bankIndex,Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex]+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankPointer(Int_t bankIndex) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else {
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(Int_t bankIndex,Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer[bankIndex]+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::Get%sBankPointer(Int_t bankIndex) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(Int_t bankIndex,Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer[bankIndex]+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::GetLast%sBankPointer(Int_t bankIndex) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank(Int_t bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID()==%s) {\n",eventName[i].Data(),bankName[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists[bankIndex] = f%sBankExists[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer[bankIndex] = f%sBankPointer[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength[bankIndex] = f%sBankLength[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      DWORD bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = (EVENT_HEADER*)GetRawDataEvent();\n");
            buffer.AppendFormatted("      pevent++;\n");
            buffer.AppendFormatted("      ROMEString bankname;\n");
            buffer.AppendFormatted("      bankname.SetFormatted(\"%s%%0%dd\",bankIndex);\n",bankName[i][j].Data(),bankArrayDigit[i][j]);
            buffer.AppendFormatted("      if (bk_find((BANK_HEADER*)pevent, bankname.Data(), &f%sBankLength[bankIndex], &bktype, (void**)&f%sBankPointer[bankIndex])) {\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("         f%sBankExists[bankIndex] = true;\n",bankName[i][j].Data());
            buffer.AppendFormatted("         return;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   f%sBankExists[bankIndex] = false;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankPointer[bankIndex] = NULL;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankLength[bankIndex] = 0;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   return;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");

            buffer.AppendFormatted("Int_t %sMidasDAQ::Get%sBankEntries(Int_t bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength[bankIndex];\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.AppendFormatted("Int_t %sMidasDAQ::GetLast%sBankEntries(Int_t bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength[bankIndex];\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         }
         // Single Bank
         else {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankPointer() {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankPointer() {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            else {
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::Get%sBankPointer() {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::GetLast%sBankPointer() {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("\n");
            }
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID()==%s) {\n",eventName[i].Data(),bankName[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists = f%sBankExists;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer = f%sBankPointer;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength = f%sBankLength;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      DWORD bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = (EVENT_HEADER*)GetRawDataEvent();\n");
            buffer.AppendFormatted("      pevent++;\n");
            buffer.AppendFormatted("      if (bk_find((BANK_HEADER*)pevent, \"%s\", &f%sBankLength, &bktype, (void**)&f%sBankPointer)) {\n",bankName[i][j].Data(),bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("         f%sBankExists = true;\n",bankName[i][j].Data());
            buffer.AppendFormatted("         return;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   f%sBankExists = false;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankPointer = NULL;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   f%sBankLength = 0;\n",bankName[i][j].Data());
            buffer.AppendFormatted("   return;\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");

            buffer.AppendFormatted("Int_t %sMidasDAQ::Get%sBankEntries() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength;\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("\n");
            buffer.AppendFormatted("Int_t %sMidasDAQ::GetLast%sBankEntries() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength;\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         }
      }
   }

   buffer.AppendFormatted("Bool_t %sMidasDAQ::IsActiveEventID(Int_t id) {\n",shortCut.Data());
   for (i=0;i<numOfEvent;i++) {
      buffer.AppendFormatted("   if (f%sEventActive && id==%s)\n",eventName[i].Data(),eventID[i].Data());
      buffer.AppendFormatted("      return true;\n");
   }
   buffer.AppendFormatted("   return false;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // -- Append method for byte swapping bank structures that are defined ----------
   buffer.AppendFormatted( "\n//Used for byte swapping banks which are structs\n" );
   buffer.AppendFormatted( "void* %sMidasDAQ::ByteSwapStruct( char* aName, void* pData )\n", shortCut.Data() );
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
               buffer.AppendFormatted( "    if ( strstr( aName, \"%s\" ) )\n", bankName[ iEvent ][ iBank ].Data() );
               hasStructuredBank = true;
            }
            else
            {
               buffer.AppendFormatted( "    else if ( strstr( aName, \"%s\" ) )\n", bankName[ iEvent ][ iBank ].Data() );
            }

            buffer.AppendFormatted( "    {\n" );

            // Loop through all fields for this structure and write out code to perform byte swapping.
            for ( long iField = 0; iField < numOfStructFields[ iEvent ][ iBank ]; iField++ )
            {
               if ( structFieldType[ iEvent ][ iBank ][ iField ] == "char" )
               {
                  buffer.AppendFormatted( "        pData = (void *)( ( (char *)pData ) + %s ); // %s\n",
                              bankFieldArraySize[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
               }

               else if ( bankFieldArraySize[ iEvent ][ iBank ][ iField ] == "1" )
               {
                  buffer.AppendFormatted( "        ROMEUtilities::ByteSwap( (%s *)pData ); // %s\n",
                              structFieldType[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        pData = (void *)( ( (%s *)pData ) + 1 );\n", structFieldType[ iEvent ][ iBank ][ iField ].Data() );
               }
               else
               {
                  buffer.AppendFormatted( "        for ( long i = 0; i < %s; i++ )\n", bankFieldArraySize[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        {\n" );
                  buffer.AppendFormatted( "            ROMEUtilities::ByteSwap( (%s *)pData ); // %s\n",
                              structFieldType[ iEvent ][ iBank ][ iField ].Data(), structFieldName[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "            pData = (void *)( ( (%s *)pData ) + 1 );\n", structFieldType[ iEvent ][ iBank ][ iField ].Data() );
                  buffer.AppendFormatted( "        }\n" );
               }
            }    // End loop through fields.
            buffer.AppendFormatted( "    }\n" );

         }        // End if is structured bank.
      }            // End loop through banks.
   }                // End loop through events.

   buffer.AppendFormatted( "    return( pData );\n" );
   buffer.AppendFormatted( "    WarningSuppression(aName);\n" );
   buffer.AppendFormatted( "    return( pData );\n" );
   buffer.AppendFormatted( "}\n" );
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteMidasDAQH() {
   int i,j,k;

   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sMidasDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sMidasDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sMidasDAQ_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sMidasDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the midas odb access for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMEMidasDAQ.h\"\n");
   buffer.AppendFormatted("\n");

   // bank structures
   if (numOfBank>0)
      buffer.AppendFormatted("// Bank Structures\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
            buffer.AppendFormatted("typedef struct {\n");
            for (k=0;k<numOfStructFields[i][j];k++) {
               if (bankFieldArraySize[i][j][k]=="1") {
                  if (structFieldSize[i][j][k].Length()>0)
                     buffer.AppendFormatted("   %s %s : %s;\n",structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data(),structFieldSize[i][j][k].Data());
                  else
                     buffer.AppendFormatted("   %s %s;\n",structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data());
               }
               else {
                  if (structFieldSize[i][j][k].Length()>0) {
                     format.SetFormatted("   %%s %%s[%%s] : %%s;\n");
                     buffer.AppendFormatted(format.Data(),structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data(),structFieldSize[i][j][k].Data());
                  }
                  else {
                     format.SetFormatted("   %%s %%s[%%s];\n");
                     buffer.AppendFormatted(format.Data(),structFieldType[i][j][k].Data(),structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data());
                  }
               }
            }
            buffer.AppendFormatted("} %sStruct;\n",bankName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Hot Links Structures
   if (midas) {
      ROMEString steerPath;
      buffer.AppendFormatted("// Hot Links Structures\n");
      for (i=0;i<numOfTaskHierarchy;i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("typedef struct{\n");
         buffer.AppendFormatted("   Int_t Active; //! Task Active\n");
         for (j=0;j<numOfSteering[i];j++) {
            for (k=0;k<numOfSteerFields[i][j];k++) {
               if (steerFieldHotLink[i][j][k]) {
                  GetSteerPath(steerPath,i,j,k,"_");
                  buffer.AppendFormatted("   %s %s; //! %s Steering Parameter\n",steerFieldType[i][j][k].Data(),steerPath.Data(),steerPath.Data());
               }
            }
         }
         buffer.AppendFormatted("} %s%sHotLinks; // Task %s%s\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("typedef struct{\n");
      for (j=0;j<numOfSteering[numOfTaskHierarchy];j++) {
         for (k=0;k<numOfSteerFields[numOfTaskHierarchy][j];k++) {
            if (steerFieldHotLink[numOfTaskHierarchy][j][k]) {
               GetSteerPath(steerPath,numOfTaskHierarchy,j,k,"_");
               buffer.AppendFormatted("   %s %s; //! %s Steering Parameter\n",steerFieldType[numOfTaskHierarchy][j][k].Data(),steerPath.Data(),steerPath.Data());
            }
         }
      }
      buffer.AppendFormatted("} GSPHotLinks; // Global Steering Parameters\n");
      for (i=0;i<numOfTab;i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("typedef struct{\n");
         buffer.AppendFormatted("   Int_t Active; //! Tab Active\n");
         for (j=0;j<numOfSteering[numOfTaskHierarchy+1+i];j++) {
            for (k=0;k<numOfSteerFields[numOfTaskHierarchy+1+i][j];k++) {
               if (steerFieldHotLink[numOfTaskHierarchy+1+i][j][k]) {
                  GetSteerPath(steerPath,numOfTaskHierarchy+1+i,j,k,"_");
                  buffer.AppendFormatted("   %s %s; //! %s Steering Parameter\n",steerFieldType[numOfTaskHierarchy+1+i][j][k].Data(),steerPath.Data(),steerPath.Data());
               }
            }
         }
         buffer.AppendFormatted("} %s%sHotLinks; // Tab %s%s\n",tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
      }
   }

   // Class
   buffer.AppendFormatted("\nclass %sMidasDAQ : public ROMEMidasDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Bank Fields
   int bankNameLen = -1;
   int bankTypeLen = -1;
   for (i=0;i<numOfEvent;i++) {
      if (numOfBank[i]>0) {
         for (j=0;j<numOfBank[i];j++) {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               if (bankTypeLen<(int)bankName[i][j].Length()+6) bankTypeLen = bankName[i][j].Length()+6;
            }
            else {
               if (bankTypeLen<(int)bankType[i][j].Length()) bankTypeLen = bankType[i][j].Length();
            }
            if (bankNameLen<(int)bankName[i][j].Length()+bankArrayDigit[i][j]) bankNameLen = bankName[i][j].Length()+bankArrayDigit[i][j];
         }
      }
   }
   buffer.AppendFormatted("   // Bank Fields\n");
   for (i=0;i<numOfEvent;i++) {
      for (j=0;j<numOfBank[i];j++) {
         // Bank Array
         if (bankArrayDigit[i][j]>0) {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
               format.SetFormatted("   %%sStruct*%%%ds fLast%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            }
            else {
               format.SetFormatted("   %%s*%%%ds f%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
               format.SetFormatted("   %%s*%%%ds fLast%%sBankPointer[%%d]; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            }
            format.SetFormatted("   DWORD%%%ds f%%sBankLength[%%d];  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-5,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            format.SetFormatted("   DWORD%%%ds fLast%%sBankLength[%%d];  %%%ds //! Length  of the %%s Bank of the last event\n",bankTypeLen-5,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            format.SetFormatted("   Bool_t%%%ds f%%sBankExists[%%d];  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-6,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
            format.SetFormatted("   Bool_t%%%ds fLast%%sBankExists[%%d];  %%%ds //! Exist Flags of the %%s Bank of the last event\n",bankTypeLen-6,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),bankArraySize[i][j]+bankArrayStart[i][j],"",bankName[i][j].Data());
         }
         // Single Bank
         else {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               format.SetFormatted("   %%sStruct*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
               format.SetFormatted("   %%sStruct*%%%ds fLast%%sBankPointer; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankName[i][j].Length()-6,bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankName[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            }
            else {
               format.SetFormatted("   %%s*%%%ds f%%sBankPointer; %%%ds //! Pointer to the %%s Bank\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
               format.SetFormatted("   %%s*%%%ds fLast%%sBankPointer; %%%ds //! Pointer to the %%s Bank of the last event\n",bankTypeLen-bankType[i][j].Length(),bankNameLen-bankName[i][j].Length());
               buffer.AppendFormatted(format.Data(),bankType[i][j].Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            }
            format.SetFormatted("   DWORD%%%ds f%%sBankLength;  %%%ds //! Length  of the %%s Bank\n",bankTypeLen-5,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            format.SetFormatted("   DWORD%%%ds fLast%%sBankLength;  %%%ds //! Length  of the %%s Bank of the last event\n",bankTypeLen-5,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            format.SetFormatted("   Bool_t%%%ds f%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank\n",bankTypeLen-6,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
            format.SetFormatted("   Bool_t%%%ds fLast%%sBankExists;  %%%ds //! Exist Flags of the %%s Bank of the last event\n",bankTypeLen-6,bankNameLen-bankName[i][j].Length());
            buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
         }
         format.SetFormatted("   Bool_t%%%ds f%%sBankActive;  %%%ds //! Active Flags of the %%s Bank\n",bankTypeLen-6,bankNameLen-bankName[i][j].Length());
         buffer.AppendFormatted(format.Data(),"",bankName[i][j].Data(),"",bankName[i][j].Data());
      }
      format.SetFormatted("   Bool_t%%%ds f%%sEventActive;  %%%ds //! Active Flags of the %%s Bank\n",bankTypeLen-6,bankNameLen-eventName[i].Length());
      buffer.AppendFormatted(format.Data(),"",eventName[i].Data(),"",eventName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Hot Links
   if (midas) {
      buffer.AppendFormatted("   // Hot Links\n");
      for (i=0;i<numOfTaskHierarchy;i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks f%s%sHotLinks;               //! %s%s Hot Links\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("   GSPHotLinks fGSPHotLinks;               //! GSP Hot Links\n");
      for (i=0;i<numOfTab;i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks f%s%sHotLinks;               //! %s%s Hot Links\n",tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
      }
      buffer.AppendFormatted("\n");
   }


   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sMidasDAQ();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sMidasDAQ() {}\n",shortCut.Data());

   // Banks
   buffer.AppendFormatted("   // Bank Methodes\n");
   buffer.AppendFormatted("   void InitMidasBanks();\n");
   for (i=0;i<numOfEvent;i++) {
      if (numOfBank[i]>0) {
         for (j=0;j<numOfBank[i];j++) {
            // Bank Array
            if (bankArrayDigit[i][j]>0) {
               if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
                  buffer.AppendFormatted("   %sStruct* Get%sBankAt(Int_t bankIndex,Int_t index);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* Get%sBankPointer(Int_t bankIndex);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(Int_t bankIndex,Int_t index);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankPointer(Int_t bankIndex);\n",bankName[i][j].Data(),bankName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("   %s  Get%sBankAt(Int_t bankIndex,Int_t index);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s* Get%sBankPointer(Int_t bankIndex);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s  GetLast%sBankAt(Int_t bankIndex,Int_t index);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s* GetLast%sBankPointer(Int_t bankIndex);\n",bankType[i][j].Data(),bankName[i][j].Data());
               }
               buffer.AppendFormatted("   Int_t Get%sBankEntries(Int_t bankIndex);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   Int_t GetLast%sBankEntries(Int_t bankIndex);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   void Init%sBank(Int_t bankIndex);\n",bankName[i][j].Data());
            }
            // Single Bank
            else {
               if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
                  buffer.AppendFormatted("   %sStruct* Get%sBankAt(Int_t index);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* Get%sBankPointer();\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankAt(Int_t index);\n",bankName[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %sStruct* GetLast%sBankPointer();\n",bankName[i][j].Data(),bankName[i][j].Data());
               }
               else {
                  buffer.AppendFormatted("   %s  Get%sBankAt(Int_t index);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s* Get%sBankPointer();\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s  GetLast%sBankAt(Int_t index);\n",bankType[i][j].Data(),bankName[i][j].Data());
                  buffer.AppendFormatted("   %s* GetLast%sBankPointer();\n",bankType[i][j].Data(),bankName[i][j].Data());
               }
               buffer.AppendFormatted("   Int_t Get%sBankEntries();\n",bankName[i][j].Data());
               buffer.AppendFormatted("   Int_t GetLast%sBankEntries();\n",bankName[i][j].Data());
               buffer.AppendFormatted("   void Init%sBank();\n",bankName[i][j].Data());
            }
            buffer.AppendFormatted("   bool is%sBankActive() { return f%sBankActive; }\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   void Set%sBankActive(bool flag) { f%sBankActive = flag; }\n",bankName[i][j].Data(),bankName[i][j].Data());
         }
      }
      buffer.AppendFormatted("   bool is%sEventActive() { return f%sEventActive; }\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("   void Set%sEventActive(bool flag) { f%sEventActive = flag; }\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("\n");
   }

   // Hot Links
   if (midas) {
      buffer.AppendFormatted("   // Hot Links\n");
      for (i=0;i<numOfTaskHierarchy;i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks* Get%s%sHotLinks() { return &f%s%sHotLinks; };\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      }
      buffer.AppendFormatted("   GSPHotLinks* GetGSPHotLinks() { return &fGSPHotLinks; };\n");
      for (i=0;i<numOfTab;i++) {
         if (!tabUsed[i])
            continue;
         buffer.AppendFormatted("   %s%sHotLinks* Get%s%sHotLinks() { return &f%s%sHotLinks; };\n",tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data(),tabName[i].Data(),tabSuffix[i].Data());
      }
      buffer.AppendFormatted("\n");
   }


   // protected
   buffer.AppendFormatted("protected:\n");
   if (midas) {
      buffer.AppendFormatted("   Bool_t  InitODB();\n");
   }
   buffer.AppendFormatted("   Bool_t  InitHeader();\n");
   buffer.AppendFormatted("   Bool_t  IsActiveEventID(Int_t id);\n");
   buffer.AppendFormatted("   void   *ByteSwapStruct( char* aName, void* pData );\n");

   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%sMidasDAQ, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sMidasDAQ_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteRomeDAQCpp() {
   int i ,j;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString format;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sRomeDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sRomeDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT TTree access for %s%s.",shortCut.Data(),mainProgName.Data());
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
   buffer.AppendFormatted("#include \"generated/%sRomeDAQ.h\"\n",shortCut.Data());
   for (i=0;i<numOfTree;i++) {
      for (j=0;j<numOfRunHeader[i];j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]])
            buffer.AppendFormatted("#include \"generated/%s%s.h\"\n",shortCut.Data(), runHeaderFolder[i][j].Data());
      }
   }


   buffer.AppendFormatted("\nClassImp(%sRomeDAQ)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sRomeDAQ::%sRomeDAQ() { }\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");

   // Connect Trees
   int iFold=0,k;
   bool found = false;
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.AppendFormatted("void %sRomeDAQ::ConnectTrees()\n{\n",shortCut.Data());
   for (i=0;i<numOfTree && !found;i++) {
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("   TBranchElement *bb;\n");
         found = true;
         break;
      }
   }
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   if (fROMETrees[%d]->isRead()) {\n", i);
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("     bb = (TBranchElement*)fROMETrees[%d]->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j].Data());
         buffer.AppendFormatted("     if (bb) {\n");
         buffer.AppendFormatted("        if (fROMETrees[%d]->GetBranchActiveAt(%d)) {\n",i,j);
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("           bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         else {
            buffer.AppendFormatted("           bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         buffer.AppendFormatted("        }\n");
         buffer.AppendFormatted("        else {\n");
         buffer.AppendFormatted("           fROMETrees[%d]->GetTree()->SetBranchStatus(\"%s*\", 0);\n",i,branchName[i][j].Data());
         buffer.AppendFormatted("        }\n");
         buffer.AppendFormatted("     }\n");
         buffer.AppendFormatted("     bb = (TBranchElement*)fROMETrees[%d]->GetTree()->FindBranch(\"Info\");\n",i);
         buffer.AppendFormatted("     bb->SetAddress(&fTreeInfo);\n");
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("}\n\n");

   // Read run header
   buffer.AppendFormatted("void %sRomeDAQ::ReadRunHeaders()\n{\n",shortCut.Data());
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   if (fROMETrees[%d]->isRead()) {\n", i);
      buffer.AppendFormatted("      if (fROMETrees[%d]->GetFile()) {\n", i);
      buffer.AppendFormatted("         fROMETrees[%d]->GetFile()->cd();\n", i);
      for (j=0;j<numOfRunHeader[i];j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            if (folderArray[runHeaderFolderIndex[i][j]] == "1")
               buffer.AppendFormatted("         gAnalyzer->Get%s()->Read(\"%s\");\n", runHeaderFolder[i][j].Data(), runHeaderName[i][j].Data());
            else
               buffer.AppendFormatted("         gAnalyzer->Get%ss()->Read(\"%s\");\n", runHeaderFolder[i][j].Data(), runHeaderName[i][j].Data());
         }
      }
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteRomeDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;
   Int_t i;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sRomeDAQ.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sRomeDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sRomeDAQ_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sRomeDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT TTree access for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMERomeDAQ.h\"\n");

   // Class
   buffer.AppendFormatted("\nclass %sRomeDAQ : public ROMERomeDAQ\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   buffer.AppendFormatted("public:\n");

   // Constructor
   buffer.AppendFormatted("   %sRomeDAQ();\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sRomeDAQ() {}\n",shortCut.Data());

   // Run header
   buffer.AppendFormatted("   void    ReadRunHeaders();\n");

   // File getter
   for (i=0;i<numOfTree;i++)
      buffer.AppendFormatted("   TFile* Get%sFile() { return fRootFiles ? fRootFiles[%d] : 0; }\n",treeName[i].Data(),i);

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

Bool_t ROMEBuilder::WriteRootDAQCpp() {
   int i,j;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString format;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sRootDAQ.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
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
   buffer.AppendFormatted("%sRootDAQ::%sRootDAQ()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i=0;i<numOfRootTree;i++) {
      buffer.AppendFormatted("   f%s = new %s();\n",rootTreeName[i].Data(),rootTreeName[i].Data());
      for (j=0;j<numOfRootBranch[i];j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("   f%s->f%s = new %s();\n",rootTreeName[i].Data(),rootBranchName[i][j].Data(),rootBranchClassName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Create Trees
   buffer.AppendFormatted("// Create Trees\n");
   buffer.AppendFormatted("Bool_t %sRootDAQ::CreateTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   TTree *tree;\n");
   buffer.AppendFormatted("   fTrees->RemoveAll();\n");
   for (i=0;i<numOfRootTree;i++) {
      buffer.AppendFormatted("   tree = NULL;\n");
      buffer.AppendFormatted("   for (i=0;i<gROME->GetNumberOfInputFileNames() && tree==NULL;i++) {\n");
      buffer.AppendFormatted("      tree = (TTree*)fRootFiles[i]->FindObjectAny(\"%s\");\n",rootTreeName[i].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   if (tree==NULL) {\n");
      buffer.AppendFormatted("      ROMEPrint::Warning(\"Tree '%s' not found.\\n\");\n",rootTreeName[i].Data());
      buffer.AppendFormatted("      return false;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   fTrees->AddLast(tree);\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Connect Trees
   buffer.AppendFormatted("// Connect Trees\n");
   buffer.AppendFormatted("Bool_t %sRootDAQ::ConnectTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TBranchElement *bb;\n");
   for (i=0;i<numOfRootTree;i++) {
      for (j=0;j<numOfRootBranch[i];j++) {
         buffer.AppendFormatted("   bb = ((TBranchElement*)((TTree*)fTrees->At(%d))->FindBranch(\"%s\"));\n",i,rootBranchName[i][j].Data());
         buffer.AppendFormatted("   if (!bb) {\n");
         buffer.AppendFormatted("      ROMEPrint::Warning(\"Branch '%s' not found in tree '%s'.\\n\");\n",rootBranchName[i][j].Data(),rootTreeName[i].Data());
         buffer.AppendFormatted("      return false;\n");
         buffer.AppendFormatted("   }\n");
         if (rootBranchArraySize[i][j].Length()==0) {
            buffer.AppendFormatted("   bb->SetAddress(&(Get%s()->f%s));\n",rootTreeName[i].Data(),rootBranchName[i][j].Data());
         }
         else {
            buffer.AppendFormatted("   bb->SetAddress(Get%s()->f%s);\n",rootTreeName[i].Data(),rootBranchName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

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
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sRootDAQ_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sRootDAQ_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sRootDAQ", shortCut.Data());
   clsDescription.SetFormatted("This class implements the ROOT DAQ for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"TTree.h\"\n");
   for (i=0;i<numOfRootTree;i++) {
      for (j=0;j<numOfRootBranch[i];j++) {
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

   // Fields
   for (i=0;i<numOfRootTree;i++) {
      buffer.AppendFormatted("   class %s {\n",rootTreeName[i].Data());
      buffer.AppendFormatted("   public:\n");
      for (j=0;j<numOfRootBranch[i];j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("      %s* f%s; // %s Branch\n",rootBranchClassName[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
         }
         else {
            if (rootBranchArraySize[i][j].Length()==0) {
               buffer.AppendFormatted("      %s f%s; // %s Branch\n",rootBranchType[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            }
            else {
               buffer.AppendFormatted("      %s *f%s; // %s Branch\n",rootBranchType[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("      %s() {\n",rootTreeName[i].Data());
      for (j=0;j<numOfRootBranch[i];j++) {
         if (rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            if (rootBranchArraySize[i][j].Length()>0) {
               buffer.AppendFormatted("         f%s = new %s[%s];\n",rootBranchName[i][j].Data(),rootBranchType[i][j].Data(),rootBranchArraySize[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("      }\n");
      for (j=0;j<numOfRootBranch[i];j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            buffer.AppendFormatted("      %s* Get%s() { return f%s; }\n",rootBranchClassName[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
         }
         else {
            if (rootBranchArraySize[i][j].Length()==0) {
               buffer.AppendFormatted("      %s Get%s() { return f%s; }\n",rootBranchType[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            }
            else {
               buffer.AppendFormatted("      %s Get%sAt(int i) { return f%s[i]; }\n",rootBranchType[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
               buffer.AppendFormatted("      %s* Get%s() { return f%s; }\n",rootBranchType[i][j].Data(),rootBranchName[i][j].Data(),rootBranchName[i][j].Data());
            }
         }
      }
      buffer.AppendFormatted("   };\n");
      buffer.AppendFormatted("   %s* f%s; // %s Branch\n",rootTreeName[i].Data(),rootTreeName[i].Data(),rootTreeName[i].Data());
   }
   // Methods
   buffer.AppendFormatted("public:\n");

   // Constructor
   buffer.AppendFormatted("   %sRootDAQ();\n",shortCut.Data());
   for (i=0;i<numOfRootTree;i++) {
      buffer.AppendFormatted("   %s* Get%s() { return f%s; }\n",rootTreeName[i].Data(),rootTreeName[i].Data(),rootTreeName[i].Data());
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

Bool_t ROMEBuilder::WriteRootDAQClassesH() {
   int i,j;
   for (i=0;i<numOfRootTree;i++) {
      for (j=0;j<numOfRootBranch[i];j++) {
         if (!rootBranchType[i][j].CompareTo("Class",TString::kIgnoreCase)) {
            WriteRootDAQClassH(i,j);
         }
      }
   }
   return true;
}

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
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %s_H\n",rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("#define %s_H\n\n",rootBranchClassName[iTree][iBranch].Data());
   clsName.SetFormatted("%s",rootBranchClassName[iTree][iBranch].Data());
   clsDescription.SetFormatted("This class is used for the branch %s of the tree %s of the ROOT DAQ.",rootBranchName[iTree][iBranch].Data(),rootTreeName[iTree].Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"TObject.h\"\n");

   // Class
   buffer.AppendFormatted("\nclass %s : public TObject\n",rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("protected:\n");
   for (i=0;i<numOfRootBranchField[iTree][iBranch];i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length()==0) {
         buffer.AppendFormatted("   %s %s; // %s Field\n",rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
      }
      else {
         buffer.AppendFormatted("   %s %s[%s]; // %s Field\n",rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldArraySize[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Methods
   buffer.AppendFormatted("public:\n");

   // Constructor
   buffer.AppendFormatted("   %s()\n",rootBranchClassName[iTree][iBranch].Data());
   buffer.AppendFormatted("   {\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");

   // methods
   for (i=0;i<numOfRootBranchField[iTree][iBranch];i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length()==0) {
         buffer.AppendFormatted("   %s Get%s() { return %s; }\n",rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
      }
      else {
         buffer.AppendFormatted("   %s  Get%sAt(int i) { return %s[i]; }\n",rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
         buffer.AppendFormatted("   %s* Get%s() { return %s; }\n",rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfRootBranchField[iTree][iBranch];i++) {
      if (rootBranchFieldArraySize[iTree][iBranch][i].Length()==0) {
         buffer.AppendFormatted("   void Set%s(%s %sValue) { %s = %sValue; }\n",rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
      }
      else {
         buffer.AppendFormatted("   void Set%sAt(int i,%s %sValue) { %s[i] = %sValue; }\n",rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldType[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data(),rootBranchFieldName[iTree][iBranch][i].Data());
      }
   }
   buffer.AppendFormatted("\n");
   // Footer
   buffer.AppendFormatted("   ClassDef(%s, %d)\n",rootBranchClassName[iTree][iBranch].Data(),rootBranchClassVersion[iTree][iBranch].ToInteger()+1);
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %s_H\n",rootBranchClassName[iTree][iBranch].Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteDAQCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iDAQ=0;iDAQ<numOfDAQ;iDAQ++) {
      if (!daqUsed[iDAQ])
         continue;
      // File name
      cppFile.SetFormatted("%ssrc/daqs/%s%sDAQ.cpp",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

      if (gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, mainAuthor.Data(), kFALSE);
         clsName.SetFormatted("%s%sDAQ", shortCut.Data(), daqName[iDAQ].Data());
         clsDescription.SetFormatted("%s DAQ class for %s%s.",daqName[iDAQ].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"daqs/%s%sDAQ.h\"\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
         buffer.AppendFormatted("#include \"generated/%sConfig.h\"\n",shortCut.Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("ClassImp(%s%sDAQ)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("%s%sDAQ::%s%sDAQ()\n",shortCut.Data(),daqName[iDAQ].Data(),shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("Bool_t %s%sDAQ::Init()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%sDAQ::BeginOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%sDAQ::Event(Long64_t event)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Long64_t %s%sDAQ::Seek(Long64_t event)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return -1;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%sDAQ::EndOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
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

Bool_t ROMEBuilder::WriteDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iDAQ=0;iDAQ<numOfDAQ;iDAQ++) {
      if (!daqUsed[iDAQ])
         continue;
      // File name
      hFile.SetFormatted("%sinclude/daqs/%s%sDAQ.h",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

      if (gSystem->AccessPathName(hFile.Data(),kFileExists)) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, mainAuthor.Data(), kFALSE);
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

         buffer.AppendFormatted("protected:\n");
         // Methods
         buffer.AppendFormatted("public:\n");
         // Constructor and Methods
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

         buffer.AppendFormatted("   ClassDef(%s%sDAQ, 0)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("};\n\n");


         buffer.AppendFormatted("#endif   // %s%sDAQ_H\n",shortCut.Data(),daqName[iDAQ].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
  }
   return true;
}

Bool_t ROMEBuilder::WriteDBCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout << "\n   Output Cpp-Files:" << endl;
   for (int iDB=0;iDB<numOfDB;iDB++) {
      // File name
      cppFile.SetFormatted("%ssrc/databases/%s%sDataBase.cpp",outDir.Data(),shortCut.Data(),dbName[iDB].Data());

#if 1
// this special treatment is neccesary only for several month from Feb.2006
      if (!gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
         fstream *fileStream = new fstream(cppFile.Data(),ios::in);
         ROMEString fileBuffer;
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
         if (fileBuffer.Contains("int runNumber")
             || fileBuffer.Contains("int eventNumber")
             || fileBuffer.Contains("long runNumber")
             || fileBuffer.Contains("long eventNumber")) {
            cerr<<"type of event number was changed to Long64."<<endl
                <<"Please modify."<<shortCut<<dbName[iDB]<<".h and "
                <<shortCut<<dbName[iDB]<<".cpp"<<endl;
            return false;
         }
      }
      else {
#else
      if (gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
#endif
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, mainAuthor.Data(), kFALSE);
         clsName.SetFormatted("%s%s", shortCut.Data(), dbName[iDB].Data());
         clsDescription.SetFormatted("%s database class for %s%s.",dbName[iDB].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("ClassImp(%s%sDataBase)\n",shortCut.Data(),dbName[iDB].Data());

         buffer.AppendFormatted("%s%sDataBase::%s%sDataBase()\n",shortCut.Data(),dbName[iDB].Data(),shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("%s%sDataBase::~%s%sDataBase()\n",shortCut.Data(),dbName[iDB].Data(),shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("Bool_t %s%sDataBase::Init(const char* name,const char* path,const char* connection)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%sDataBase::Read(ROMEStr2DArray *values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%sDataBase::Write(ROMEStr2DArray* values,const char *dataBasePath,Long64_t runNumber,Long64_t eventNumber)\n",shortCut.Data(),dbName[iDB].Data());
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

Bool_t ROMEBuilder::WriteDBH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iDB=0;iDB<numOfDB;iDB++) {
      // File name
      hFile.SetFormatted("%sinclude/databases/%s%sDataBase.h",outDir.Data(),shortCut.Data(),dbName[iDB].Data());

#if 1
// this special treatment is neccesary only for several month from Feb.2006
      if (!gSystem->AccessPathName(hFile.Data(),kFileExists)) {
         fstream *fileStream = new fstream(hFile.Data(),ios::in);
         ROMEString fileBuffer;
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
         if (fileBuffer.Contains("int runNumber")
             || fileBuffer.Contains("int eventNumber")
             || fileBuffer.Contains("long runNumber")
             || fileBuffer.Contains("long eventNumber")) {
            cerr<<"type of event number was changed to Long64."<<endl
                <<"Please modify."<<shortCut<<dbName[iDB]<<".h and "
                <<shortCut<<dbName[iDB]<<".cpp"<<endl;
            return false;
         }
      }
      else {
#else
      if (gSystem->AccessPathName(hFile.Data(),kFileExists)) {
#endif

         // Description
         buffer.Resize(0);
         WriteHeader(buffer, mainAuthor.Data(), kFALSE);
         buffer.AppendFormatted("#ifndef %s%sDataBase_H\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("#define %s%sDataBase_H\n\n",shortCut.Data(),dbName[iDB].Data());
         clsName.SetFormatted("%s%s", shortCut.Data(), dbName[iDB].Data());
         clsDescription.SetFormatted("%s database class for %s%s.",dbName[iDB].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"ROMEDataBase.h\"\n");

         // Class
         buffer.AppendFormatted("\nclass %s%sDataBase : public ROMEDataBase\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("{\n");

         buffer.AppendFormatted("protected:\n");
         // Methods
         buffer.AppendFormatted("public:\n");
         // Constructor and Methods
         buffer.AppendFormatted("   %s%sDataBase();\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("   virtual ~%s%sDataBase();\n",shortCut.Data(),dbName[iDB].Data());

         // Methods
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t  Init(const char* name,const char* path,const char* connection);\n");
         buffer.AppendFormatted("   Bool_t  Read(ROMEStr2DArray *values,const char *path,Long64_t runNumber,Long64_t eventNumber);\n");
         buffer.AppendFormatted("   Bool_t  Write(ROMEStr2DArray* values,const char *path,Long64_t runNumber,Long64_t eventNumber);\n");
         buffer.AppendFormatted("   char   *GetType() { return \"%s\"; }\n",dbName[iDB].Data());
         buffer.AppendFormatted("   char   *GetDescription() { return \"%s\"; }\n",dbDescription[iDB].Data());

         buffer.AppendFormatted("   ClassDef(%s%sDataBase, 0)\n",shortCut.Data(),dbName[iDB].Data());
         buffer.AppendFormatted("};\n\n");

         buffer.AppendFormatted("#endif   // %s%sDataBase_H\n",shortCut.Data(),dbName[iDB].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
  }
   return true;
}

Bool_t ROMEBuilder::WriteNetFolderServerCpp() {
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString format;
   int i,j;

   // File name
   cppFile.SetFormatted("%ssrc/generated/%sNetFolderServer.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   clsName.SetFormatted("%sNetFolderServer", shortCut.Data());
   clsDescription.SetFormatted("This class implements the NetFolderServer for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   // Header
   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TSocket.h>\n");
   buffer.AppendFormatted("#include <TServerSocket.h>\n");
   buffer.AppendFormatted("#include <TMessage.h>\n");
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))\n");
   buffer.AppendFormatted("#   include <TThread.h>\n");
   buffer.AppendFormatted("#endif // ROOT_VERSION\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("#include \"tasks/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"generated/%sNetFolderServer.h\"\n",shortCut.Data());

   buffer.AppendFormatted("\nClassImp(%sNetFolderServer)\n",shortCut.Data());

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("void %sNetFolderServer::ConstructObjects(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t id = FindId(socket);\n");
   buffer.AppendFormatted("   if(id == -1) return;\n");
   buffer.AppendFormatted("   fFolder[id] = new TObjArray(%d);\n",numOfFolder);
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            buffer.AppendFormatted("   fFolderActive[id][%d] = kFALSE;\n",i);
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("   fFolder[id]->AddAt(new %s%s(),%d);\n",shortCut.Data(),folderName[i].Data(),i);
            }
            else {
               buffer.AppendFormatted("   fFolder[id]->AddAt(new TClonesArray(\"%s%s\"),%d);\n",shortCut.Data(),folderName[i].Data(),i);
               buffer.AppendFormatted("   ((TClonesArray*)fFolder[id]->At(%d))->SetName(\"%s%s\");\n",i,shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("   f%s%s_%sGraphActive[id] = kFALSE;\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   f%s%s_%sGraph[id] = new %s();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),graphType[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   f%s%s_%sGraphs[id] = new TObjArray();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("void %sNetFolderServer::DestructObjects(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t id = FindId(socket);\n");
   buffer.AppendFormatted("   if(id == -1) return;\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            buffer.AppendFormatted("   delete fFolder[id]->At(%d);\n",i);
         }
      }
   }
   buffer.AppendFormatted("   SafeDelete(fFolder[id]);\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   SafeDelete(f%s%s_%sGraph[id]);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   SafeDelete(f%s%s_%sGraphs[id]);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("Int_t %sNetFolderServer::Register(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   Int_t id = -1;\n");
   buffer.AppendFormatted("   for (i = 0; i < kMaxSocketClients; i++) {;\n");
   buffer.AppendFormatted("      if(fAcceptedSockets[i] == 0) {\n");
   buffer.AppendFormatted("         fAcceptedSockets[i] = socket;\n");
   buffer.AppendFormatted("         id = i;\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   if(id == -1)\n");
   buffer.AppendFormatted("      Error(\"Register\", \"Number of netfolder clients exceeded the maximum (%%d)\", kMaxSocketClients);\n");
   buffer.AppendFormatted("   return id;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("void %sNetFolderServer::UnRegister(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t id = FindId(socket);\n");
   buffer.AppendFormatted("   if(id != -1) {\n");
   buffer.AppendFormatted("      fAcceptedSockets[id] = 0;\n");
   buffer.AppendFormatted("      fSocketClientRead[id] = kFALSE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("Int_t %sNetFolderServer::FindId(TSocket* socket)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   i = 0;\n"); // to suppress unused warning
   buffer.AppendFormatted("   for (i = 0; i < kMaxSocketClients; i++) {;\n");
   buffer.AppendFormatted("      if(fAcceptedSockets[i] == socket) {\n");
   buffer.AppendFormatted("         return i;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("int %sNetFolderServer::ResponseFunction(TSocket *socket) {\n",shortCut.Data());
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))\n");
   buffer.AppendFormatted("   if (!socket->IsValid())\n");
   buffer.AppendFormatted("      return 0;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   // Read Command\n");
   buffer.AppendFormatted("   char str[200];\n");
   buffer.AppendFormatted("   if (socket->Recv(str, sizeof(str)) <= 0) {\n");
   buffer.AppendFormatted("      socket->Close();\n");
   buffer.AppendFormatted("      delete socket;\n");
   buffer.AppendFormatted("      return 0;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return CheckCommand(socket,str);\n");
   buffer.AppendFormatted("#endif // ROOT_VERSION\n");
   buffer.AppendFormatted("   return 1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("int %sNetFolderServer::CheckCommand(TSocket *socket,char *str) {\n",shortCut.Data());
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))\n");
   buffer.AppendFormatted("   if (!socket->IsValid())\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   %sNetFolderServer* localThis = static_cast<%sNetFolderServer*>(gAnalyzer->GetNetFolderServer());\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   TMessage message(kMESS_OBJECT);\n");
   buffer.AppendFormatted("   // Check Command\n");
   buffer.AppendFormatted("   if (strncmp(str, \"GetCurrentRunNumber\", 19) == 0) {\n");
   buffer.AppendFormatted("      //get run number\n");
   buffer.AppendFormatted("      Long64_t runNumber = gAnalyzer->GetCurrentRunNumber();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      message<<runNumber;\n");
   buffer.AppendFormatted("      socket->Send(message);\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   Int_t id = localThis->FindId(socket);\n");
   buffer.AppendFormatted("   if(id >= 0) {\n");
   buffer.AppendFormatted("      while (!localThis->IsSocketClientRead(id))\n");
   buffer.AppendFormatted("         localThis->UpdateObjects();\n");
   buffer.AppendFormatted("      if (strncmp(str, \"FindObjectAny\", 13) == 0) {\n");
   buffer.AppendFormatted("         ReadFolderPointer(socket);\n");
   buffer.AppendFormatted("      }\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %s%s\", %d) == 0) {\n",shortCut.Data(),folderName[i].Data(),static_cast<int>(strlen("FindObjectAny ")+shortCut.Length()+folderName[i].Length()));
            }
            else {
               buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %s%ss\", %d) == 0) {\n",shortCut.Data(),folderName[i].Data(),static_cast<int>(strlen("FindObjectAny s")+shortCut.Length()+folderName[i].Length()));
            }
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
            buffer.AppendFormatted("      message<<localThis->fFolder[id]->At(%d);\n",i);
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   if (strncmp(str, \"RegisterObject %s%s\", %d) == 0) {\n",shortCut.Data(),folderName[i].Data(),static_cast<int>(strlen("RegisterObject ")+shortCut.Length()+folderName[i].Length()));
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
            buffer.AppendFormatted("      localThis->fFolderActive[id][%d] = kTRUE;\n",i);
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   if (strncmp(str, \"UnRegisterObject %s%s\", %d) == 0) {\n",shortCut.Data(),folderName[i].Data(),static_cast<int>(strlen("UnRegisterObject ")+shortCut.Length()+folderName[i].Length()));
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
            buffer.AppendFormatted("      localThis->fFolderActive[id][%d] = kFALSE;\n",i);
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         if (histoArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %sT%s:%s\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("FindObjectAny T:")+shortCut.Length()+taskHierarchyName[i].Length()+histoName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      message<< gAnalyzer->Get%s%sTask()->Get%s();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %sT%s:%s_\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("FindObjectAny T:_")+shortCut.Length()+taskHierarchyName[i].Length()+histoName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      char* cstop;\n");
            buffer.AppendFormatted("      int index = strtol(&str[%d],&cstop,10);\n",static_cast<int>(strlen("FindObjectAny T:_")+shortCut.Length()+taskHierarchyName[i].Length()+histoName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      message<< gAnalyzer->Get%s%sTask()->Get%sAt(index);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %sT%s:%ss\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("FindObjectAny T:s")+shortCut.Length()+taskHierarchyName[i].Length()+histoName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      message<< gAnalyzer->Get%s%sTask()->Get%s();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
         }
      }
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %sT%s:%s\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("FindObjectAny T:")+shortCut.Length()+taskHierarchyName[i].Length()+graphName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
            buffer.AppendFormatted("      message<<localThis->f%s%s_%sGraph[id];\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
         }
         else {
            buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %sT%s:%s_\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("FindObjectAny T:_")+shortCut.Length()+taskHierarchyName[i].Length()+graphName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      char* cstop;\n");
            buffer.AppendFormatted("      int index = strtol(&str[%d],&cstop,10);\n",static_cast<int>(strlen("FindObjectAny T:_")+shortCut.Length()+taskHierarchyName[i].Length()+graphName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
            buffer.AppendFormatted("      message<<localThis->f%s%s_%sGraphs[id]->At(index);\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   if (strncmp(str, \"FindObjectAny %sT%s:%ss\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("FindObjectAny T:s")+shortCut.Length()+taskHierarchyName[i].Length()+graphName[taskHierarchyClassIndex[i]][j].Length()));
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
            buffer.AppendFormatted("      message<<localThis->f%s%s_%sGraphs[id];\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
            buffer.AppendFormatted("      socket->Send(message);\n");
            buffer.AppendFormatted("      return 1;\n");
            buffer.AppendFormatted("   }\n");
         }
         buffer.AppendFormatted("   if (strncmp(str, \"RegisterObject %sT%s:%s\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("RegisterObject ")+shortCut.Length()+taskHierarchyName[i].Length()+graphName[taskHierarchyClassIndex[i]][j].Length()));
         buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
         buffer.AppendFormatted("      localThis->f%s%s_%sGraphActive[id] = kTRUE;\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   if (strncmp(str, \"UnRegisterObject %sT%s:%s\", %d) == 0) {\n",shortCut.Data(),taskHierarchyName[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),static_cast<int>(strlen("UnRegisterObject ")+shortCut.Length()+taskHierarchyName[i].Length()+graphName[taskHierarchyClassIndex[i]][j].Length()));
         buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->Lock();\n");
         buffer.AppendFormatted("      localThis->f%s%s_%sGraphActive[id] = kFALSE;\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("      gAnalyzer->GetSocketServerMutex()->UnLock();\n");
         buffer.AppendFormatted("   }\n");
      }
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return ROMENetFolderServer::CheckCommand(socket,str);\n");
   buffer.AppendFormatted("#endif // ROOT_VERSION\n");
   buffer.AppendFormatted("   return 1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("THREADTYPE %sNetFolderServer::Server(void *arg)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))\n");
   buffer.AppendFormatted("   TSocket *socket = (TSocket *) arg;\n");
   buffer.AppendFormatted("   %sNetFolderServer* localThis = static_cast<%sNetFolderServer*>(gAnalyzer->GetNetFolderServer());\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (!socket->IsValid())\n");
   buffer.AppendFormatted("      return THREADRETURN;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (localThis->Register(socket)==-1)\n");
   buffer.AppendFormatted("      return THREADRETURN;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   localThis->ConstructObjects(socket);\n");
   buffer.AppendFormatted("   while (%sNetFolderServer::ResponseFunction(socket))\n",shortCut.Data());
   buffer.AppendFormatted("   {}\n");
   buffer.AppendFormatted("   localThis->DestructObjects(socket);\n");
   buffer.AppendFormatted("   localThis->UnRegister(socket);\n");
   buffer.AppendFormatted("#endif // ROOT_VERSION\n");
   buffer.AppendFormatted("   return THREADRETURN;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("THREADTYPE %sNetFolderServer::ServerLoop(void *arg)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))\n");
   buffer.AppendFormatted("// Server loop listening for incoming network connections on port\n");
   buffer.AppendFormatted("// specified by command line option -s. Starts a searver_thread for\n");
   buffer.AppendFormatted("// each connection.\n");
   buffer.AppendFormatted("   int port = *(int*)arg;\n");
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
   buffer.AppendFormatted("      return THREADRETURN;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   do {\n");
   buffer.AppendFormatted("      TSocket *sock = lsock->Accept();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      TThread *thread = new TThread(\"Server\", %sNetFolderServer::Server, sock);\n",shortCut.Data());
   buffer.AppendFormatted("      thread->Run();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   } while (1);\n");
   buffer.AppendFormatted("#endif // ROOT_VERSION\n");
   buffer.AppendFormatted("   return THREADRETURN;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("void %sNetFolderServer::StartServer(TApplication *app,Int_t port,const char* serverName)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(4,1,0))\n");
   buffer.AppendFormatted("// start Socket server loop\n");
   buffer.AppendFormatted("   fApplication = app;\n");
   buffer.AppendFormatted("   fPort = port;\n");
   buffer.AppendFormatted("   fServerName = serverName;\n");
   buffer.AppendFormatted("   TThread *thread = new TThread(\"server_loop\", %sNetFolderServer::ServerLoop, &fPort);\n",shortCut.Data());
   buffer.AppendFormatted("   thread->Run();\n");
   buffer.AppendFormatted("#endif // ROOT_VERSION\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("Bool_t %sNetFolderServer::UpdateObjects()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   gAnalyzer->GetSocketServerMutex()->Lock();\n");
   buffer.AppendFormatted("   gAnalyzer->GetObjectStorageMutex()->Lock();\n");
   buffer.AppendFormatted("   //create a buffer where the object will be streamed\n");
   buffer.AppendFormatted("   TFile *filsav = gFile;\n");
   buffer.AppendFormatted("   gFile = 0;\n");
   buffer.AppendFormatted("   const Int_t bufsize = 10000;\n");
   buffer.AppendFormatted("#if (ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0))\n");
   buffer.AppendFormatted("   TBufferFile *buffer = new TBufferFile(TBuffer::kWrite,bufsize);\n");
   buffer.AppendFormatted("#else\n");
   buffer.AppendFormatted("   TBuffer *buffer = new TBuffer(TBuffer::kWrite,bufsize);\n");
   buffer.AppendFormatted("#endif\n");
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
   buffer.AppendFormatted("   for(iClient = 0; iClient < kMaxSocketClients; iClient++) {\n");
   buffer.AppendFormatted("      if(!fAcceptedSockets[iClient]) continue;\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderSupport[i])
         continue;
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("      buffer->Reset();\n");
         buffer.AppendFormatted("      buffer->SetWriteMode();\n");
         buffer.AppendFormatted("      if (fFolderActive[iClient][%d] || fCopyAll) {\n",i);
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("         gAnalyzer->CopyTObjectWithStreamer(buffer,gAnalyzer->Get%sFolderStorage(),fFolder[iClient]->At(%d));\n",folderName[i].Data(),i);
         }
         else {
            buffer.AppendFormatted("         bypassOrgOld = gAnalyzer->Get%sFoldersStorage()->CanBypassStreamer();\n",folderName[i].Data());
            buffer.AppendFormatted("         bypassOld = ((TClonesArray*)fFolder[iClient]->At(%d))->CanBypassStreamer();\n",i);
            buffer.AppendFormatted("         gAnalyzer->Get%sFoldersStorage()->BypassStreamer(kTRUE);\n",folderName[i].Data());
            buffer.AppendFormatted("         ((TClonesArray*)fFolder[iClient]->At(%d))->BypassStreamer(kTRUE);\n",i);
            buffer.AppendFormatted("         gAnalyzer->CopyTObjectWithStreamer(buffer,gAnalyzer->Get%sFoldersStorage(),fFolder[iClient]->At(%d));\n",folderName[i].Data(),i);
            buffer.AppendFormatted("         gAnalyzer->Get%sFoldersStorage()->BypassStreamer(bypassOrgOld);\n",folderName[i].Data());
            buffer.AppendFormatted("         ((TClonesArray*)fFolder[iClient]->At(%d))->BypassStreamer(bypassOld);\n",i);
         }
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("\n");
      }
   }
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (numOfGraphs[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("      if (gAnalyzer->Get%s%sTaskBase()->IsActive()) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("      buffer->Reset();\n");
         buffer.AppendFormatted("      buffer->SetWriteMode();\n");
         buffer.AppendFormatted("      if (f%s%s_%sGraphActive[iClient] || fCopyAll) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("         %s *%sOrg = gAnalyzer->Get%s%sTaskBase()->Get%sGraphStorage();\n",graphType[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            WriteUpdateObjectsObject(buffer,"f"+taskHierarchyName[i]+taskHierarchySuffix[i]+"_"+graphName[taskHierarchyClassIndex[i]][j]+"Graph[iClient]",graphName[taskHierarchyClassIndex[i]][j]+"Org",false);
         }
         else {
            buffer.AppendFormatted("         TObjArray *%ssOrg = gAnalyzer->Get%s%sTaskBase()->Get%sGraphsStorage();\n",graphName[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            WriteUpdateObjectsObject(buffer,"f"+taskHierarchyName[i]+taskHierarchySuffix[i]+"_"+graphName[taskHierarchyClassIndex[i]][j]+"Graphs[iClient]",graphName[taskHierarchyClassIndex[i]][j]+"sOrg",false);
         }
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("\n");
      }
      if (numOfGraphs[taskHierarchyClassIndex[i]]>0)
         buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("      fSocketClientRead[iClient] = kTRUE;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   gAnalyzer->GetSocketServerMutex()->UnLock();\n");
   buffer.AppendFormatted("   gAnalyzer->GetObjectStorageMutex()->UnLock();\n");
   buffer.AppendFormatted("   gFile = filsav;\n");
   buffer.AppendFormatted("   delete buffer;\n");
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteNetFolderServerH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   // File name
   hFile.SetFormatted("%sinclude/generated/%sNetFolderServer.h",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
   buffer.AppendFormatted("#ifndef %sNetFolderServer_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sNetFolderServer_H\n\n",shortCut.Data());
   clsName.SetFormatted("%sNetFolderServer", shortCut.Data());
   clsDescription.SetFormatted("This class implements the NetFolderServer for %s%s.",shortCut.Data(),mainProgName.Data());
   WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
   buffer.AppendFormatted("\n\n");

   buffer.AppendFormatted("#include \"ROMENetFolderServer.h\"\n");
   buffer.AppendFormatted("#include <TH1.h>\n");
   buffer.AppendFormatted("#include <TH2.h>\n");
   buffer.AppendFormatted("#include <TH3.h>\n");
   buffer.AppendFormatted("#include <TProfile.h>\n");
   buffer.AppendFormatted("#include <TProfile2D.h>\n");
   buffer.AppendFormatted("#include <TGraphMT.h>\n");
   buffer.AppendFormatted("#include <TGraph2D.h>\n");

   // Folder class declaration
   Int_t i,j;
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
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
   buffer.AppendFormatted("   Bool_t fFolderActive[kMaxSocketClients][%d]; //! Flag if folder is active\n",TMath::Max(numOfFolder,1));
   buffer.AppendFormatted("   TObjArray* fFolder[kMaxSocketClients]; //! Handle to Folders\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("   Bool_t f%s%s_%sGraphActive[kMaxSocketClients]; //! Flag if %s graph is active\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   %s* f%s%s_%sGraph[kMaxSocketClients]; //! Handle to %s graph\n",graphType[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   TObjArray* f%s%s_%sGraphs[kMaxSocketClients]; //! Handle to %s graphs\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   void              StartServer(TApplication *app,Int_t port,const char* serverName);\n");
   buffer.AppendFormatted("   static Int_t      ResponseFunction(TSocket *socket);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sNetFolderServer():ROMENetFolderServer(){}\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sNetFolderServer(){}\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            buffer.AppendFormatted("   Bool_t Get%sFolderActive(Int_t i) { return fFolderActive[i][%d]; }\n",folderName[i].Data(),i);
         }
      }
   }
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("   Bool_t Get%s%s_%sGraphActive(Int_t i) { return f%s%s_%sGraphActive[i]; }\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
      }
   }

   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t UpdateObjects();\n");
   buffer.AppendFormatted("   void   ConstructObjects(TSocket* socket);\n");
   buffer.AppendFormatted("   void   DestructObjects(TSocket* socket);\n");
   buffer.AppendFormatted("   Int_t  Register(TSocket* socket);\n");
   buffer.AppendFormatted("   void   UnRegister(TSocket* socket);\n");
   buffer.AppendFormatted("   Int_t  FindId(TSocket* socket);\n");
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

Bool_t ROMEBuilder::WriteUpdateObjectsObject(ROMEString &buffer,const char *objectPointer,const char *objectStoragePointer,bool bypass)
{
   if (bypass) {
      buffer.AppendFormatted("         bypassOrgOld = %s->CanBypassStreamer();\n",objectStoragePointer);
      buffer.AppendFormatted("         bypassOld = %s->CanBypassStreamer();\n",objectPointer);
      buffer.AppendFormatted("         %s->BypassStreamer(kTRUE);\n",objectStoragePointer);
      buffer.AppendFormatted("         %s->BypassStreamer(kTRUE);\n",objectPointer);
   }
   buffer.AppendFormatted("         gAnalyzer->CopyTObjectWithStreamer(buffer,%s,%s);\n",objectStoragePointer,objectPointer);
   if (bypass) {
      buffer.AppendFormatted("         %s->BypassStreamer(bypassOrgOld);\n",objectStoragePointer);
      buffer.AppendFormatted("         %s->BypassStreamer(bypassOld);\n",objectPointer);
   }
   return true;
}
Bool_t ROMEBuilder::WriteSteeringClass(ROMEString &buffer,Int_t numSteer,Int_t numTask,Int_t tab)
{
   ROMEString format;
   ROMEString sc;
   ROMEString blank="";
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfSteerFields[numTask][numSteer];i++) {
      if (!steerFieldUsed[numTask][numSteer][i])
         continue;
      if (typeLen<(int)steerFieldType[numTask][numSteer][i].Length()) typeLen = steerFieldType[numTask][numSteer][i].Length();
      if (nameLen<(int)steerFieldName[numTask][numSteer][i].Length()) nameLen = steerFieldName[numTask][numSteer][i].Length();
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (typeLen<(int)steerName[numTask][steerChildren[numTask][numSteer][i]].Length()+1) typeLen = steerName[numTask][steerChildren[numTask][numSteer][i]].Length()+1;
      if (nameLen<(int)steerName[numTask][steerChildren[numTask][numSteer][i]].Length()) nameLen = steerName[numTask][steerChildren[numTask][numSteer][i]].Length();
   }
   if (tab==0)
      sc = shortCut;
   else
      sc = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");

   buffer.AppendFormatted("\n%sclass %s%s\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("%s{\n",blank.Data());

   buffer.AppendFormatted("%spublic:\n",blank.Data());

   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      WriteSteeringClass(buffer,steerChildren[numTask][numSteer][i],numTask,tab+1);
   }

   buffer.AppendFormatted("%sprotected:\n",blank.Data());

   ROMEString tmp;

   // Fields
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen+5-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",ProcessCommentCPP(steerFieldComment[numTask][numSteer][j],tmp).Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds f%%s[%%s];%%%ds %%s\n",typeLen,nameLen+3-steerFieldName[numTask][numSteer][j].Length()-steerFieldArraySize[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldArraySize[numTask][numSteer][j].Data(),"",ProcessCommentCPP(steerFieldComment[numTask][numSteer][j],tmp).Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         format.SetFormatted("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen+1-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds **f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormatted("%s   %s%s()\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("{\n");
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]!="1") {
         buffer.AppendFormatted("int j;\n");
         break;
      }
   }
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         if (steerFieldType[numTask][numSteer][j]=="std::string")
            buffer.AppendFormatted("   f%s = %s;\n",steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
         else
            buffer.AppendFormatted("   f%s = (%s)%s;\n",steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
      }
      else {
         if (steerFieldType[numTask][numSteer][j]=="std::string")
            buffer.AppendFormatted("   for (j=0;j<%s;j++) f%s[j] = %s;\n",steerFieldArraySize[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
         else
            buffer.AppendFormatted("   for (j=0;j<%s;j++) f%s[j] = (%s)%s;\n",steerFieldArraySize[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]!="1") {
         buffer.AppendFormatted("int i;\n");
         break;
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         buffer.AppendFormatted("   f%s = new %s();\n",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         buffer.AppendFormatted("   f%s = new %s*[%s]; for (i=0;i<%s;i++) f%s[i] = new %s();\n",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Destructor
   buffer.AppendFormatted("%s   ~%s%s()\n",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   buffer.AppendFormatted("{\n");
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]!="1") {
         buffer.AppendFormatted("int i; ");
         break;
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         buffer.AppendFormatted("   delete f%s;\n",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         buffer.AppendFormatted("    for (i=0;i<%s;i++) delete f%s[i]; delete [] f%s;\n",steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("};\n");

   // Getters
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   %%-%ds Get%%s()%%%ds { return f%%s; }\n",typeLen,nameLen+7-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds Get%%sAt(Int_t i)%%%ds { return f%%s[i]; }\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         format.SetFormatted("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; }\n",typeLen-1,nameLen+7-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds *Get%%sAt(Int_t i)%%%ds { return f%%s[i]; }\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   // Setters
   buffer.AppendFormatted("\n");
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   void Set%%s(%%s %%s)%%%ds { f%%s = %%s; }\n",2*nameLen+typeLen+8-2*steerFieldName[numTask][numSteer][j].Length()-steerFieldType[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   void Set%%sAt(Int_t i,%%s %%s)%%%ds { f%%s[i] = %%s; }\n",2*nameLen+typeLen-2*steerFieldName[numTask][numSteer][j].Length()-steerFieldType[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data());
      }
   }

   // Footer
   buffer.AppendFormatted("%s};\n\n",blank.Data());

   return true;
}

Bool_t ROMEBuilder::WriteSteeringReadParameters(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,ROMEString& steerPointer)
{
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldCLOption[numTask][numSteer][k].Length()) {
         if (steerFieldArraySize[numTask][numSteer][k]=="1") {
            buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("   if (option == \"-%s\") {\n",steerFieldCLOption[numTask][numSteer][k].Data());
            if (isBoolType(steerFieldType[numTask][numSteer][k].Data())) {
               buffer.AppendFormatted("      %s->Set%s(true);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
            }
            else{
               buffer.AppendFormatted("      if (!strlen(value))\n");
               buffer.AppendFormatted("         ROMEPrint::Warning(\"warning: %s might not be specified properly.\\n\");\n",steerFieldName[numTask][numSteer][k].Data());
               setValue(&decodedValue,"","value",steerFieldType[numTask][numSteer][k].Data(),1);
               buffer.AppendFormatted("      %s->Set%s((%s)%s);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
               buffer.AppendFormatted("      i++;\n");
            }
            buffer.AppendFormatted("      return true;\n");
            buffer.AppendFormatted("   }\n");
         }
         else{
            buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTask][numSteer][k].Data());
            buffer.AppendFormatted("   for (int i_%s=0;i_%s<%s;i_%s++) {\n",steerFieldCLOption[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data(),steerFieldArraySize[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      tmp = \"-%s\";\n",steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      tmp += i_%s;\n",steerFieldCLOption[numTask][numSteer][k].Data());
            buffer.AppendFormatted("      if (option == tmp) {\n");
            if (isBoolType(steerFieldType[numTask][numSteer][k].Data())) {
               buffer.AppendFormatted("         %s->Set%sAt(i_%s,true);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data());
            }
            else{
               buffer.AppendFormatted("         if (!strlen(value))\n");
               buffer.AppendFormatted("            ROMEPrint::Warning(\"warning: %s might not be specified properly.\\n\");\n",steerFieldName[numTask][numSteer][k].Data());
               setValue(&decodedValue,"","value",steerFieldType[numTask][numSteer][k].Data(),1);
               buffer.AppendFormatted("         %s->Set%sAt(i_%s,(%s)%s);\n",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data(),steerFieldCLOption[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),decodedValue.Data());
               buffer.AppendFormatted("         i++;\n");
            }
            buffer.AppendFormatted("         return true;\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringReadParameters(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}

Bool_t ROMEBuilder::WriteSteeringParameterUsage(ROMEString &buffer,Int_t numSteer,Int_t numTask,ROMEString& pointer,ROMEString& steerPointer)
{
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   ROMEString format;
   ROMEString arrayIndex;
   int nspace;
   char *cstop=NULL;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldCLOption[numTask][numSteer][k].Length()) {
         arrayIndex = "";
         if (steerFieldArraySize[numTask][numSteer][k]!="1")
            arrayIndex.SetFormatted("[0-%ld]",strtol(steerFieldArraySize[numTask][numSteer][k].Data(),&cstop,10)-1);
         nspace = 8-steerFieldCLOption[numTask][numSteer][k].Length()-arrayIndex.Length();
         if (nspace<1)
            nspace = 1;
         format.SetFormatted("   ROMEPrint::Print(\"  -%%s%%s%%%ds%%s",nspace);
         buffer.AppendFormatted(format.Data(),steerFieldCLOption[numTask][numSteer][k].Data(),arrayIndex.Data(),"",steerFieldCLDescription[numTask][numSteer][k].Data());
         if (isBoolType(steerFieldCLOption[numTask][numSteer][k].Data()))
            buffer.AppendFormatted(" (no Argument)");
         buffer.AppendFormatted("\\n\");\n");
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      buffer.AppendFormatted("   // %s Group\n",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      WriteSteeringParameterUsage(buffer,steerChildren[numTask][numSteer][k],numTask,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}

Int_t ROMEBuilder::WriteSteeringInterpreterCode(ROMEString &buffer,Int_t codeNumber,Int_t numSteer,Int_t numTask,ROMEString& path1,ROMEString& path2,Int_t tab)
{
   ROMEString path1T;
   ROMEString path2T;
   ROMEString blank = "";
   for (int i=0;i<tab;i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   if (path==\"%s/%s\")\n",blank.Data(),path1.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return %d;\n",blank.Data(),codeNumber);
         buffer.AppendFormatted("%s   if (path.Index(\"%s/%s\")!=-1)\n",blank.Data(),path1.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return -2;\n",blank.Data());
         buffer.AppendFormatted("%s   if (path==\"%s->Get%s()\")\n",blank.Data(),path2.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return %d;\n",blank.Data(),codeNumber);
         buffer.AppendFormatted("%s   if (path.Index(\"%s->Get%s()\")!=-1)\n",blank.Data(),path2.Data(),steerFieldName[numTask][numSteer][k].Data());
         buffer.AppendFormatted("%s      return -2;\n",blank.Data());
         codeNumber++;
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         path1T = path1;
         path2T = path2;
         path1.AppendFormatted("/%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         path2.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,steerChildren[numTask][numSteer][k],numTask,path1,path2,tab);
         path1 = path1T;
         path2 = path2T;
      }
   }
   return codeNumber;
}

Int_t ROMEBuilder::WriteSteeringInterpreterValue(ROMEString &buffer,const char* type,Int_t codeNumber,Int_t numSteer,Int_t numTask,ROMEString& steerPointer,Int_t tab)
{
   ROMEString steerPointerT;
   ROMEString str;
   ROMEString blank = "";
   ROMEString stringBuffer = "buffer";
   for (int i=0;i<tab;i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      if (steerFieldArraySize[numTask][numSteer][k]=="1") {
         buffer.AppendFormatted("%s   case %d:\n",blank.Data(),codeNumber);
         str.SetFormatted("%s->Get%s()",steerPointer.Data(),steerFieldName[numTask][numSteer][k].Data());
         stringBuffer = "buffer";
         buffer.AppendFormatted("%s      return %s;\n",blank.Data(),convertType(str.Data(),steerFieldType[numTask][numSteer][k].Data(),type,stringBuffer).Data());
         codeNumber++;
      }
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][k]]=="1") {
         steerPointerT = steerPointer;
         steerPointer.AppendFormatted("->Get%s()",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
         codeNumber = WriteSteeringInterpreterValue(buffer,type,codeNumber,steerChildren[numTask][numSteer][k],numTask,steerPointer,tab);
         steerPointer = steerPointerT;
      }
   }
   return codeNumber;
}

void ROMEBuilder::WriteObjectInterpreterValue(ROMEString &buffer,const char* type,const char* fctName)
{
   int i,j;
   ROMEString tmp;
   ROMEString stringBuffer = "buffer";
   if (!strcmp(type,"ROMEString&"))
      buffer.AppendFormatted("%s %sAnalyzer::GetObjectInterpreter%sValue(Int_t code,%s defaultValue,%s %s) {\n",type,shortCut.Data(),fctName,type,type,stringBuffer.Data());
   else {
      buffer.AppendFormatted("%s %sAnalyzer::GetObjectInterpreter%sValue(Int_t code,%s defaultValue) {\n",type,shortCut.Data(),fctName,type);
      buffer.AppendFormatted("   ROMEString buffer;\n");
   }
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop=NULL;\n"); // to suppress unused warning
   buffer.AppendFormatted("   switch (code) {\n");
   buffer.AppendFormatted("      case -1:\n");
   buffer.AppendFormatted("         return defaultValue;\n");
   ROMEString steerPointer = "gAnalyzer->GetGSP()";
   int codeNumber = WriteSteeringInterpreterValue(buffer,type,0,0,numOfTask,steerPointer,1);
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderArray[i]=="1" && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]==0 && !isFolder(valueType[i][j].Data())) {
               buffer.AppendFormatted("      case %d:\n",codeNumber);
               tmp.SetFormatted("gAnalyzer->Get%s()->Get%s()",folderName[i].Data(),valueName[i][j].Data());
               stringBuffer = "buffer";
               buffer.AppendFormatted("         return %s;\n",convertType(tmp.Data(),valueType[i][j].Data(),type,stringBuffer).Data());
               codeNumber++;
            }
         }
      }
   }
   buffer.AppendFormatted("   }\n");
   if (!strcmp(type,"ROMEString&"))
      buffer.AppendFormatted("   return %s;\n",stringBuffer.Data());
   else
      buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
}

void ROMEBuilder::WriteReadDataBaseFolder(ROMEString &buffer,Int_t numFolder,Int_t type)
{
   int j,k;
   ROMEString valueString;
   ROMEString buf;
   ROMEString blank = "";
   ROMEString iValue = "0";
   ROMEString tempBuffer;
   if (type==2) {
      blank = "   ";
      iValue= "i";
      buffer.AppendFormatted("   int i;\n");
      buffer.AppendFormatted("   int nentry;\n");
   }
   buffer.AppendFormatted("   int j;\n");
   buffer.AppendFormatted("   j=0;\n"); // to suppress unused warnin
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   ROMEString path;\n");
   buffer.AppendFormatted("   ROMEString name;\n");
   buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
   buffer.AppendFormatted("   char *cstop;\n");
   buffer.AppendFormatted("   cstop=NULL;\n"); // to suppress unused warning
   for (j=0;j<numOfValue[numFolder];j++) {
      if ( valueDimension[numFolder][j]>1 || valueArray[numFolder][j][0] == "variable")
         continue;
      if (valueIsTObject[numFolder][j] && !isPointerType(valueType[numFolder][j].Data())
          && !valueType[numFolder][j].Contains("TRef") && !valueType[numFolder][j].Contains("TString"))
         continue;
      if (valueDBIf[numFolder][j].Length()) {
         buffer.AppendFormatted("   if (%s) {\n", valueDBIf[numFolder][j].Data());
      }
      else {
         buffer.AppendFormatted("   {\n");
      }
      if (folderArray[numFolder]=="1" && type==1 || folderArray[numFolder]!="1" && type==2 && !folderSupport[numFolder]) {
         buffer.AppendFormatted("      values->RemoveAll();\n");
         buffer.AppendFormatted("      if (fDBName[%d][%d].Length()==0)\n",numFolder,j);
         if (valueDBName[numFolder][j].Length())
            buffer.AppendFormatted("         name.SetFormatted(\"%s\");\n",valueDBName[numFolder][j].Data());
         else
            buffer.AppendFormatted("         name.Resize(0);\n");
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         name.SetFormatted(fDBName[%d][%d].Data());\n",numFolder,j);
         buffer.AppendFormatted("      if (fDBPath[%d][%d].Length()==0)\n",numFolder,j);
         if (valueDBPath[numFolder][j].Length())
            buffer.AppendFormatted("         path.SetFormatted(%s);\n",valueDBPath[numFolder][j].Data());
         else
            buffer.AppendFormatted("         path.Resize(0);\n");

         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         path.SetFormatted(fDBPath[%d][%d].Data()",numFolder,j);
         for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
            buffer.AppendFormatted("\n            ,gAnalyzer->GetObjectInterpreterCharValue(fDBCode[%d][%d]->At(%d),buffer[%d],buffer[%d]).Data()",numFolder,j,k,k,k);
         buffer.AppendFormatted(");\n");
         buffer.AppendFormatted("      if (name.Length() && path.Length() && gAnalyzer->isDataBaseActive(name.Data())) {\n");
         buffer.AppendFormatted("         ROMEPrint::Debug(\"Reading database %%s(type=%%s, path=%%s)\\n\",name.Data(),gAnalyzer->GetDataBase(name.Data())->GetType(),path.Data());\n");
         buffer.AppendFormatted("         if (gAnalyzer->GetDataBase(name.Data())->Read(values,path,gAnalyzer->GetCurrentRunNumber(),gAnalyzer->GetCurrentEventNumber())) {\n");
         buffer.AppendFormatted("            ROMEPrint::Debug(\"%s/%s was filled.\\n\");\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         else {\n");
         buffer.AppendFormatted("            ROMEPrint::Error(\"Error in Folder '%s' Value '%s'.\\n\");\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("            return false;\n");
         buffer.AppendFormatted("         }\n");
         if (type==2) {
            buffer.AppendFormatted("         nentry = gAnalyzer->Get%ss()->GetEntries();\n",folderName[numFolder].Data());
            buffer.AppendFormatted("         for (i=0;i<nentry;i++) {\n");
         }
         if (valueDimension[numFolder][j]==0) {
            buffer.AppendFormatted("%s         if (values->At(%s,0).Length()!=0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,0).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),valueType[numFolder][j].Data(),1);
            if (type==1) {
               buffer.AppendFormatted("%s            gAnalyzer->Get%s()->Set%s((%s)%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("%s         else\n",blank.Data());
               buffer.AppendFormatted("%s            gAnalyzer->Get%s()->Set%s(%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            else {
               buffer.AppendFormatted("               gAnalyzer->Get%sAt(i)->Set%s((%s)%s);\n",folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("            else\n");
               buffer.AppendFormatted("               gAnalyzer->Get%sAt(i)->Set%s(%s);\n",folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
         }
         else if (valueDimension[numFolder][j]==1) {
            buffer.AppendFormatted("%s         for (j=0;j<%s;j++) {\n",blank.Data(),valueArray[numFolder][j][0].Data());
            buffer.AppendFormatted("%s            if (values->At(%s,j).Length()!=0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,j).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),TArray2StandardType(valueType[numFolder][j],tempBuffer),1);
            if (type==1) {
               buffer.AppendFormatted("%s               gAnalyzer->Get%s()->Set%sAt(j,(%s)%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),TArray2StandardType(valueType[numFolder][j],tempBuffer),buf.Data());
               buffer.AppendFormatted("%s            else\n",blank.Data());
               buffer.AppendFormatted("%s               gAnalyzer->Get%s()->Set%sAt(j,%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            else {
               buffer.AppendFormatted("                  gAnalyzer->Get%sAt(i)->Set%sAt(j,(%s)%s);\n",folderName[numFolder].Data(),valueName[numFolder][j].Data(),TArray2StandardType(valueType[numFolder][j],tempBuffer),buf.Data());
               buffer.AppendFormatted("               else\n");
               buffer.AppendFormatted("                  gAnalyzer->Get%sAt(i)->Set%sAt(j,%s);\n",folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            buffer.AppendFormatted("%s         }\n",blank.Data());
         }
         else {
            cout<<"Warning: multiple dimension field can not have database connection."<<endl;
            return;
         }
         if (type==2)
            buffer.AppendFormatted("         }\n");
      }
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   values->RemoveAll();\n");
   buffer.AppendFormatted("   delete values;\n");
}

void ROMEBuilder::WriteFolderGetterInclude(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
      if (folderArray[numFolder]=="1") {
         buffer.AppendFormatted("   %s%s* Get%s();\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   %s%s** Get%sAddress();\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   %s%s* Get%sFolderStorage() { return f%sFolderStorage; } \n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
      }
      else {
         buffer.AppendFormatted("   %s%s* Get%sAt(Int_t index);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray* Get%ss();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray** Get%sAddress();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray* Get%sFoldersStorage() { return f%sFoldersStorage; }\n",folderName[numFolder].Data(),folderName[numFolder].Data());
      }
      buffer.AppendFormatted("   Bool_t Register%s();\n",folderName[numFolder].Data());
      buffer.AppendFormatted("   Bool_t UnRegister%s();\n",folderName[numFolder].Data());
   }
}

void ROMEBuilder::WriteFolderSetterInclude(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
      if (folderArray[numFolder]=="1") {
         buffer.AppendFormatted("   void Set%s(%s%s* pointer);\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
      }
      else {
         buffer.AppendFormatted("   void Set%ss(TClonesArray* pointer);\n",folderName[numFolder].Data());
      }
   }
}

void ROMEBuilder::WriteFolderGetterSource(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
      if (folderArray[numFolder]=="1") {
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%s() {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolder = (%s%s*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%s\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return f%sFolder;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("%s%s** %sAnalyzer::Get%sAddress() {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolder = (%s%s*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%s\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return &f%sFolder;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
      else if (folderArray[numFolder]=="variable") {
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%sAt(Int_t index) {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (f%sFolders->GetEntries()<=index)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("      for (int i=f%sFolders->GetEntries();i<=index;i++)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("         new((*f%sFolders)[i]) %s%s();\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return (%s%s*)f%sFolders->At(index);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray* %sAnalyzer::Get%ss() {\n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray** %sAnalyzer::Get%sAddress() {;\n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return &f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
      else {
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%sAt(Int_t index) {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (f%sFolders->GetEntriesFast()<=index) {\n",folderName[numFolder].Data());
         buffer.AppendFormatted("     ROMEPrint::Error(\"\\nYou have tried to access the %%d. item of the array folder %s\\nwhich was defined with array size %s.\\n\\nShutting down the program.\\n\",index);\n",folderName[numFolder].Data(),folderArray[numFolder].Data());
         buffer.AppendFormatted("     fApplication->Terminate(1);\n");
         buffer.AppendFormatted("     return NULL;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("  return (%s%s*)f%sFolders->At(index);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray* %sAnalyzer::Get%ss() { \n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray** %sAnalyzer::Get%sAddress() { \n",shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   if (IsROMEMonitor())\n");
         buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketClientNetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   return &f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("Bool_t %sAnalyzer::Register%s() {\n",shortCut.Data(),folderName[numFolder].Data());
      buffer.AppendFormatted("   if (IsROMEMonitor())\n");
      buffer.AppendFormatted("      return GetSocketClientNetFolder()->RegisterObject(\"%s%s\");\n",shortCut.Data(),folderName[numFolder].Data());
      buffer.AppendFormatted("   return false;\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("Bool_t %sAnalyzer::UnRegister%s() {\n",shortCut.Data(),folderName[numFolder].Data());
      buffer.AppendFormatted("   if (IsROMEMonitor())\n");
      buffer.AppendFormatted("      GetSocketClientNetFolder()->UnRegisterObject(\"%s%s\");\n",shortCut.Data(),folderName[numFolder].Data());
      buffer.AppendFormatted("   return false;\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");
   }
}

void ROMEBuilder::WriteFolderSetterSource(ROMEString &buffer,Int_t numFolder)
{
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
      if (folderArray[numFolder]=="1") {
         buffer.AppendFormatted("void %sAnalyzer::Set%s(%s%s* pointer) {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());

         buffer.AppendFormatted("   SafeDelete(f%sFolder);\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   f%sFolder = pointer;\n",folderName[numFolder].Data());

         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
      else {
         buffer.AppendFormatted("void %sAnalyzer::Set%ss(TClonesArray* pointer) { \n",shortCut.Data(),folderName[numFolder].Data());

         buffer.AppendFormatted("   SafeDelete(f%sFolders);\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   f%sFolders = pointer;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }
}

Bool_t ROMEBuilder::WriteEventLoopCpp()
{
   int i;

   ROMEString cppFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString parentt;

   bool breaking;
   int j,k,iFold=0;

   ROMEString tmp;
   ROMEString format;
   int nameLen = -1;
   int typeLen = 12;
   int scl = shortCut.Length();
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (typeLen<(int)folderName[i].Length()+scl) typeLen = folderName[i].Length()+scl;
         if (nameLen<(int)folderName[i].Length()) nameLen = folderName[i].Length();
      }
   }


   // File name
   cppFile.SetFormatted("%ssrc/generated/%sEventLoop.cpp",outDir.Data(),shortCut.Data());
   // Description
   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
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
   for (i=0;i<numOfTask;i++) {
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
   if (midas)
      buffer.AppendFormatted("#include \"generated/%sMidasDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("\nClassImp(%sEventLoop)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("%sEventLoop::%sEventLoop(const char *name,const char *title):ROMEEventLoop(name,title) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Tree Initialization
   buffer.AppendFormatted("// Tree initialization\n");
   buffer.AppendFormatted("void %sEventLoop::InitTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TFolder *treeFolder;\n");
   buffer.AppendFormatted("   treeFolder = gAnalyzer->GetMainFolder()->AddFolder(\"Trees\",\"Trees of the %s framework\");\n",shortCut.Data()); // to suppress unused warning
   if (numOfTree>0) {
      buffer.AppendFormatted("   TTree *tree;\n");
      buffer.AppendFormatted("   ROMEString fileName;\n\n");
   }
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   tree = new TTree(\"%s\",\"%s\");\n",treeName[i].Data(),treeTitle[i].Data());
      buffer.AppendFormatted("   gAnalyzer->AddTree(tree);\n");
      buffer.AppendFormatted("   treeFolder->Add(tree);\n\n");
      buffer.AppendFormatted("   gAnalyzer->GetTreeObjectAt(%d)->AllocateBranchActive(%d);\n",i,numOfBranch[i]);
      buffer.AppendFormatted("   gAnalyzer->GetTreeObjectAt(%d)->SetName(\"%s\");\n",i,treeName[i].Data());
   }
   buffer.AppendFormatted("}\n\n");

   // Add Tree Branches
   buffer.AppendFormatted("// Add branches\n");
   buffer.AppendFormatted("void %sEventLoop::AddTreeBranches()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (!gAnalyzer->IsROMEMonitor()) {\n");
   if (numOfTree>0) {
      buffer.AppendFormatted("      TTree *tree;\n");
   }
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("      tree = gAnalyzer->GetTreeObjectAt(%d)->GetTree();\n",i);
      buffer.AppendFormatted("      tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99)->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n", i);
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("      if(gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d)) {\n",i,j);
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("         tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sAddress(),%s,%s)->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),branchSplitLevel[i][j].Data(),i);
         }
         else {
            buffer.AppendFormatted("         tree->Branch(\"%s\", \"TClonesArray\", gAnalyzer->Get%sAddress(),%s,%s)->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",branchName[i][j].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),branchSplitLevel[i][j].Data(),i);
         }
         buffer.AppendFormatted("      }\n");
      }
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n\n");
   // Write run headers
   buffer.AppendFormatted("// Write run headers\n");
   buffer.AppendFormatted("void %sEventLoop::WriteRunHeaders()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   if (!gAnalyzer->IsROMEMonitor()) {\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("      if (gAnalyzer->GetTreeObjectAt(%d)->isWrite()) {\n", i);
      buffer.AppendFormatted("         if (gAnalyzer->GetTreeObjectAt(%d)->GetFile()) {\n", i);
      buffer.AppendFormatted("            gAnalyzer->GetTreeObjectAt(%d)->GetFile()->cd();\n", i);
      for (j=0;j<numOfRunHeader[i];j++) {
         if (folderUsed[runHeaderFolderIndex[i][j]]) {
            if (folderArray[runHeaderFolderIndex[i][j]] == "1")
               buffer.AppendFormatted("            gAnalyzer->Get%s()->Write(\"%s\", TObject::kOverwrite);\n", runHeaderFolder[i][j].Data(), runHeaderName[i][j].Data());
            else
               buffer.AppendFormatted("            gAnalyzer->Get%ss()->Write(\"%s\", TObject::kOverwrite | TObject::kSingleKey);\n", runHeaderFolder[i][j].Data(), runHeaderName[i][j].Data());
         }
      }
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("      }\n");
   }
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // Read run headers
   buffer.AppendFormatted("// Read run headers\n");
   buffer.AppendFormatted("void %sEventLoop::ReadRunHeaders()\n{\n",shortCut.Data());
   if (numOfTree>0) {
      buffer.AppendFormatted("   if(gAnalyzer->GetActiveDAQ()->InheritsFrom(\"%sRomeDAQ\"))\n", shortCut.Data());
      buffer.AppendFormatted("      static_cast<%sRomeDAQ*>(gAnalyzer->GetActiveDAQ())->ReadRunHeaders();\n", shortCut.Data());
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   // clean up folders
   buffer.AppendFormatted("// Delete Unused Folders\n");
   buffer.AppendFormatted("void %sEventLoop::CleanUpFolders() {\n",shortCut.Data());
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i]>0 && !folderNoReset[i] && !folderSupport[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   int i;\n");
            break;
         }
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i]>0 && !folderNoReset[i] && !folderSupport[i]) {
         if (folderArray[i]=="variable") {
            buffer.AppendFormatted("   for (i=gAnalyzer->Get%ss()->GetEntriesFast()-1;i>=0;i--) {\n",folderName[i].Data());
            buffer.AppendFormatted("      if (((%s%s*)gAnalyzer->Get%sAt(i))->isModified())\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("         break;\n");
            buffer.AppendFormatted("      gAnalyzer->Get%ss()->RemoveAt(i);\n",folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // reset folders
   buffer.AppendFormatted("// Reset Folders\n");
   buffer.AppendFormatted("void %sEventLoop::ResetFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   if (gAnalyzer->IsROMEMonitor())\n");
   buffer.AppendFormatted("      return;\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i]>0 && !folderSupport[i] && folderArray[i]!="1") {
         buffer.AppendFormatted("   int i;\n");
         buffer.AppendFormatted("   int nentry;\n");
         break;
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i]<=0 || folderSupport[i])
         continue;
      if (folderNoReset[i]) {
         if (!folderNoResetModified[i]) {
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("   gAnalyzer->Get%s()->ResetModified();\n",folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("   nentry = gAnalyzer->Get%ss()->GetEntriesFast();\n",folderName[i].Data());
               buffer.AppendFormatted("   for (i=0;i<nentry;i++) {\n");
               buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->ResetModified();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("   }\n");
            }
         }
      }
      else {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   gAnalyzer->Get%s()->Reset();\n",folderName[i].Data());
         }
         else {
            buffer.AppendFormatted("   nentry = gAnalyzer->Get%ss()->GetEntriesFast();\n",folderName[i].Data());
            buffer.AppendFormatted("   for (i=0;i<nentry;i++) {\n");
            buffer.AppendFormatted("      ((%s%s*)gAnalyzer->Get%sAt(i))->Reset();\n",shortCut.Data(),folderName[i].Data(),folderName[i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Single Folders
   buffer.AppendFormatted("void %sEventLoop::InitSingleFolders() {\n",shortCut.Data());
   ROMEString separator;
   for (i=0;i<numOfFolder;i++) {
      separator = "";
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gAnalyzer->Get%s()) %s%s(",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (isFolder(valueType[i][j].Data()))
                  continue;
               if (valueDimension[i][j]==0) {
                  buffer.AppendFormatted("%s%s",separator.Data(),valueInit[i][j].Data());
                  separator = ", ";
               }
            }
            buffer.AppendFormatted(" );\n");
         }
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Initialize Array Folders
   buffer.AppendFormatted("void %sEventLoop::InitArrayFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   i=0;\n"); // to suppress unused warning
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]!="1" && folderArray[i]!="variable")
            buffer.AppendFormatted("   gAnalyzer->Set%sSize(%s);\n",folderName[i].Data(),folderArray[i].Data());
      }
   }
   buffer.AppendFormatted("}\n\n");

   // fill trees
   buffer.AppendFormatted("// Tree Filling\n");
   buffer.AppendFormatted("void %sEventLoop::FillTrees() {\n",shortCut.Data());
   buffer.AppendFormatted("   Statistics *stat = gAnalyzer->GetTriggerStatistics();\n");
   if (numOfTree>0) {
      buffer.AppendFormatted("   ROMETree *romeTree;\n");
   }
   breaking = false;
   for (i=0;i<numOfTree && !breaking;i++) {
      for (j=0;j<numOfBranch[i] && !breaking;j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         if (folderArray[iFold]!="1") {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   int nentry;\n");
            breaking = true;;
         }
      }
   }
   buffer.AppendFormatted("   // Fill Trees;\n");
   buffer.AppendFormatted("   bool write;\n");
   buffer.AppendFormatted("   write = false;\n"); // to suppress unused warning
   buffer.AppendFormatted("   bool written;\n");
   buffer.AppendFormatted("   written = false;\n"); // to suppress unused warning
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   write = false;\n");
      buffer.AppendFormatted("   romeTree = (ROMETree*)gAnalyzer->GetTreeObjectAt(%d);\n",i);
      buffer.AppendFormatted("   if (romeTree->isFill()) {\n");
      buffer.AppendFormatted("      write = fAlwaysFillTrees;\n");
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (folderName[k]==branchFolder[i][j] && !folderSupport[k]) {
               iFold = k;
               break;
            }
         }
         if (!folderUsed[iFold])
            continue;
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("      if (!write && gAnalyzer->Get%s()->isModified()) {\n",branchFolder[i][j].Data());
            buffer.AppendFormatted("         write = true;\n");
            buffer.AppendFormatted("      }\n");
         }
         else {
            buffer.AppendFormatted("      nentry = gAnalyzer->Get%ss()->GetEntries();\n",branchFolder[i][j].Data());
            buffer.AppendFormatted("      for (i=0;!write && i<nentry;i++) {\n");
            buffer.AppendFormatted("         if (((%s%s*)gAnalyzer->Get%sAt(i))->isModified()) {\n",shortCut.Data(),branchFolder[i][j].Data(),branchFolder[i][j].Data());
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
   buffer.AppendFormatted("   if (written) {\n");
   buffer.AppendFormatted("      stat->writtenEvents++;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // get tree file names
   buffer.AppendFormatted("// Get Tree File Names\n");
   buffer.AppendFormatted("void %sEventLoop::GetTreeFileName(ROMEString& buffer,Int_t treeIndex) {\n",shortCut.Data());
   if (numOfTree>0) {
      buffer.AppendFormatted("   ROMETree *romeTree;\n");
      buffer.AppendFormatted("   switch (treeIndex) {\n");
   }
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("      case %d:\n",i);
      buffer.AppendFormatted("         romeTree = (ROMETree*)gAnalyzer->GetTreeObjectAt(%d);\n",i);
      buffer.AppendFormatted("         buffer = romeTree->GetConfigFileName();\n");
      buffer.AppendFormatted("         if (buffer.Length()==0) {\n");
      if (treeFileName[i].Length()==0)
         buffer.AppendFormatted("            buffer.SetFormatted(\"%s#.root\");\n",treeName[i].Data());
      else
         buffer.AppendFormatted("            buffer.SetFormatted(%s);\n",treeFileName[i].Data());
      buffer.AppendFormatted("         }\n");
      buffer.AppendFormatted("         gROME->ReplaceWithRunAndEventNumber(buffer);\n");
      buffer.AppendFormatted("         break;\n");
   }
   if (numOfTree>0) {
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   return;\n");
   if (numOfTree<=0) {
      buffer.AppendFormatted("   WarningSuppression(buffer);\n");
      buffer.AppendFormatted("   WarningSuppression(treeIndex);\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Hot Links
   if (midas) {
      // Initialize Hot Links
      ROMEString steerPointer;
      ROMEString steerPath;
      buffer.AppendFormatted("void %sEventLoop::InitHotLinks() {\n",shortCut.Data());
      if (numOfEvent>0) {
         buffer.AppendFormatted("   if (gAnalyzer->IsActiveDAQ(\"Midas\")) {\n");
         for (i=0;i<numOfTaskHierarchy;i++) {
            if (!taskUsed[taskHierarchyClassIndex[i]])
               continue;
            buffer.AppendFormatted("      gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->Active = gAnalyzer->GetTaskObjectAt(%d)->IsActive();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),taskHierarchyObjectIndex[i]);
            for (j=0;j<numOfSteering[i];j++) {
               for (k=0;k<numOfSteerFields[i][j];k++) {
                  if (steerFieldHotLink[i][j][k]) {
                     GetSteerPath(steerPath,i,j,k,"_");
                     GetSteerPath(steerPointer,i,j,k,"()->Get");
                     buffer.AppendFormatted("      gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->%s = ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->GetSP()->Get%s();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),steerPath.Data(),shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],steerPointer.Data());
                  }
               }
            }
         }
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("}\n\n");

      // Update Hot Links
      buffer.AppendFormatted("void %sEventLoop::UpdateHotLinks() {\n",shortCut.Data());
      if (numOfEvent>0) {
         buffer.AppendFormatted("   if (gAnalyzer->IsActiveDAQ(\"Midas\")) {\n");
         for (i=0;i<numOfTaskHierarchy;i++) {
            if (!taskUsed[taskHierarchyClassIndex[i]])
               continue;
            buffer.AppendFormatted("      if (gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->Active)\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data());
            buffer.AppendFormatted("         gAnalyzer->GetTaskObjectAt(%d)->SetActive(true);\n",taskHierarchyObjectIndex[i]);
            buffer.AppendFormatted("      else\n");
            buffer.AppendFormatted("         gAnalyzer->GetTaskObjectAt(%d)->SetActive(false);\n",taskHierarchyObjectIndex[i]);
            for (j=0;j<numOfSteering[i];j++) {
               for (k=0;k<numOfSteerFields[i][j];k++) {
                  if (steerFieldHotLink[i][j][k]) {
                     GetSteerPath(steerPath,i,j,k,"_");
                     GetSteerPath(steerPointer,i,j,k,"()->Get");
                     buffer.AppendFormatted("      ((%sT%s_Base*)gAnalyzer->GetTaskObjectAt(%d))->GetSP()->Get%s(",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],steerPointer.Data());
                     buffer[buffer.Last('G')] = 'S';
                     buffer.AppendFormatted("gAnalyzer->GetMidasDAQ()->Get%s%sHotLinks()->%s",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),steerPath.Data());
                     buffer.AppendFormatted(");\n");
                  }
               }
            }
         }
         buffer.AppendFormatted("   }\n");
      }
      buffer.AppendFormatted("}\n\n");
   }

   // Read Histograms
   buffer.AppendFormatted("void %sEventLoop::ReadHistograms() {\n",shortCut.Data());
   breaking = false;
   for (i=0;i<numOfTaskHierarchy && !breaking;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]] && !breaking;j++) {
         if (histoArraySize[taskHierarchyClassIndex[i]][j]!="1") {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   int nentry;\n");
            breaking = true;
         }
      }
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]] && !breaking;j++) {
         if (graphArraySize[taskHierarchyClassIndex[i]][j]!="1") {
            buffer.AppendFormatted("   int i;\n");
            buffer.AppendFormatted("   int nentry;\n");
            breaking = true;
         }
      }
   }
   buffer.AppendFormatted("   ROMEString filename;\n");
   buffer.AppendFormatted("   filename.SetFormatted(\"%%s%%s%%05d.root\",gAnalyzer->GetInputDir(),\"histos\",gAnalyzer->GetHistosRun());\n");
   buffer.AppendFormatted("   TFile *file = new TFile(filename.Data(),\"READ\");\n");
   buffer.AppendFormatted("   if (file->IsZombie()) {\n");
   buffer.AppendFormatted("       ROMEPrint::Warning(\"Histograms of run %%d not available.\\n\", gAnalyzer->GetHistosRun());\n");
   buffer.AppendFormatted("       ROMEPrint::Warning(\"Please check the run number and the input path.\\n\\n\");\n");
   buffer.AppendFormatted("       ROMEPrint::Warning(\"No Histogram loaded!\\n\\n\");\n");
   buffer.AppendFormatted("       return;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   file->FindObjectAny(\"histos\");\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("   if (gAnalyzer->Get%s%sTaskBase()->Get%sHisto()->IsActive()) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         if (histoArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("      %s* %sTemp_%d = ((%s*)file->FindObjectAny(\"%s\"));\n",histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),i,histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      if (%sTemp_%d==NULL)\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i);
            buffer.AppendFormatted("         ROMEPrint::Warning(\"Histogram '%s' not available in run %%d!\\n\", gAnalyzer->GetHistosRun());\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      else\n");
            buffer.AppendFormatted("         %sTemp_%d->Copy(*gAnalyzer->Get%s%sTaskBase()->Get%s());\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i,taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("      nentry = gAnalyzer->Get%s%sTaskBase()->Get%s()->GetEntries();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      for (i=0;i<nentry;i++) {\n");
            buffer.AppendFormatted("         %s* %sTemp_%d = ((%s*)file->FindObjectAny(gAnalyzer->Get%s%sTaskBase()->Get%sAt(i)->GetName()));\n",histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),i,histoType[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         if (%sTemp_%d==NULL)\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i);
            buffer.AppendFormatted("            ROMEPrint::Warning(\"Histogram '%s' not available in run %%d!\\n\", gAnalyzer->GetHistosRun());\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         else\n");
            buffer.AppendFormatted("            %sTemp_%d->Copy(*gAnalyzer->Get%s%sTaskBase()->Get%sAt(i));\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i,taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      }\n");
         }
         buffer.AppendFormatted("   }\n");
      }
      for (j=0;j<numOfGraphs[taskHierarchyClassIndex[i]];j++) {
         buffer.AppendFormatted("   if (gAnalyzer->Get%s%sTaskBase()->Get%sGraph()->IsActive()) {\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         if (graphArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("      %s* %sTemp_%d = ((%s*)file->FindObjectAny(\"%s\"));\n",graphType[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),i,graphType[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      if (%sTemp_%d==NULL)\n",graphName[taskHierarchyClassIndex[i]][j].Data(),i);
            buffer.AppendFormatted("         ROMEPrint::Warning(\"Graph '%s' not available in run %%d!\\n\", gAnalyzer->GetHistosRun());\n",graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      else\n");
            buffer.AppendFormatted("         ((TNamed*)%sTemp_%d)->Copy(*gAnalyzer->Get%s%sTaskBase()->Get%s());\n",graphName[taskHierarchyClassIndex[i]][j].Data(),i,taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("      nentry = gAnalyzer->Get%s%sTaskBase()->Get%s()->GetEntries();\n",taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      for (i=0;i<nentry;i++) {\n");
            buffer.AppendFormatted("         %s* %sTemp_%d = ((%s*)file->FindObjectAny(gAnalyzer->Get%s%sTaskBase()->Get%sAt(i)->GetName()));\n",graphType[taskHierarchyClassIndex[i]][j].Data(),graphName[taskHierarchyClassIndex[i]][j].Data(),i,graphType[taskHierarchyClassIndex[i]][j].Data(),taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         if (%sTemp_%d==NULL)\n",graphName[taskHierarchyClassIndex[i]][j].Data(),i);
            buffer.AppendFormatted("            ROMEPrint::Warning(\"Graph '%s' not available in run %%d!\\n\", gAnalyzer->GetHistosRun());\n",graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("         else\n");
            buffer.AppendFormatted("            ((TNamed*)%sTemp_%d)->Copy(*gAnalyzer->Get%s%sTaskBase()->Get%sAt(i));\n",graphName[taskHierarchyClassIndex[i]][j].Data(),i,taskHierarchyName[i].Data(),taskHierarchySuffix[i].Data(),graphName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      }\n");
         }
         buffer.AppendFormatted("   }\n");
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

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
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);
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
   buffer.AppendFormatted("protected:\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sEventLoop(const char *name,const char *title);\n",shortCut.Data());
   buffer.AppendFormatted("   virtual ~%sEventLoop() {}\n",shortCut.Data());
   buffer.AppendFormatted("   void AddTreeBranches();\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("protected:\n");

   // Folders
   buffer.AppendFormatted("   // Folder Methodes\n");
   buffer.AppendFormatted("   void InitSingleFolders();\n");
   buffer.AppendFormatted("   void InitArrayFolders();\n");
   buffer.AppendFormatted("   void ResetFolders();\n");
   buffer.AppendFormatted("   void CleanUpFolders();\n");
   buffer.AppendFormatted("\n");

   // Trees
   buffer.AppendFormatted("   // Tree Methodes\n");
   buffer.AppendFormatted("   void InitTrees();\n");
   buffer.AppendFormatted("   void FillTrees();\n");
   buffer.AppendFormatted("   void GetTreeFileName(ROMEString& buffer,Int_t treeIndex);\n");
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

   buffer.AppendFormatted("   void ReadHistograms();\n");
   buffer.AppendFormatted("\n");

   // Footer
   buffer.AppendFormatted("   ClassDef(%sEventLoop, 0)\n",shortCut.Data());
   buffer.AppendFormatted("};\n\n");

   buffer.AppendFormatted("#endif   // %sEventLoop_H\n",shortCut.Data());

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteMain()
{
   ROMEString cppFile;
   ROMEString buffer;
   ROMEString tmp, tmp2;

   cppFile.SetFormatted("%ssrc/generated/main.cpp",outDir.Data());

   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);

   buffer.AppendFormatted("#include <RConfig.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include <TROOT.h>\n");
   buffer.AppendFormatted("#include <TFolder.h>\n");
   buffer.AppendFormatted("#include <TGClient.h>\n");
   buffer.AppendFormatted("#include <TSystem.h>\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMERint.h\"\n");
   buffer.AppendFormatted("#include \"generated/%sAnalyzer.h\"\n",shortCut.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows.h>\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("const char* const kAdditionalInclude =\n");

   // Additional include path for ACLiC mode.
   TString currentDirectory = gSystem->WorkingDirectory();
   gSystem->ChangeDirectory(outDir.Data());
   TString outDirAbsolute = gSystem->WorkingDirectory();
   gSystem->ChangeDirectory(currentDirectory.Data());
   ROMEString cmd;
   ROMEString cmdRes;
   ROMEString tmpName;
   ifstream ifs;
   tmpName.SetFormatted("%s/romebuilder.%d", outDir.Data(), gSystem->GetPid());
   if (this->pgsql) {
      cmd.SetFormatted("pg_config --includedir > %s", tmpName.Data());
      gSystem->Exec(cmd);
      ifs.open(tmpName.Data());
      if (ifs.good()) {
         cmdRes.ReadFile(ifs);
         ifs.close();
         if (cmdRes.Length()) {
            cmdRes.ReplaceAll("\\\\","/");
            cmdRes.ReplaceAll("\\","/");
            if (cmdRes.EndsWith("\n"))
                cmdRes.Resize(cmdRes.Length() -1);
            buffer.AppendFormatted("      \" -I%s\"\n", cmdRes.Data());
         }
      }
      cmd.SetFormatted("rm -f %s", tmpName.Data());
      gSystem->Exec(cmd);
   }
#if defined( R__UNIX )
   if (this->mysql) {
      cmd.SetFormatted("mysql_config --include > %s", tmpName.Data());
      gSystem->Exec(cmd);
      ifs.open(tmpName.Data());
      if (ifs.good()) {
         cmdRes.ReadFile(ifs);
         ifs.close();
         if (cmdRes.Length()) {
            if (cmdRes.EndsWith("\n"))
               cmdRes.Resize(cmdRes.Length() -1);
            buffer.AppendFormatted("      \" %s\"\n", cmdRes.Data());
         }
      }
      cmd.SetFormatted("rm -f %s", tmpName.Data());
      gSystem->Exec(cmd);
   }
#endif // R__UNIX
#if defined( R__MACOSX )
   TString finkDir;
   cmd.SetFormatted("which fink 2>&1 | sed -ne \"s/\\/bin\\/fink//p\" > %s", tmpName.Data());
   gSystem->Exec(cmd);
   ifs.open(tmpName.Data());
   if (ifs.good()) {
      cmdRes.ReadFile(ifs);
      ifs.close();
      if (cmdRes.Length()) {
         if (cmdRes.EndsWith("\n"))
            cmdRes.Resize(cmdRes.Length() -1);
         finkDir = cmdRes;
      }
      cmd.SetFormatted("rm -f %s", tmpName.Data());
      gSystem->Exec(cmd);
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
   buffer.AppendFormatted("   int argn = 1;\n");
   buffer.AppendFormatted("   char arg[1][100];\n");
   buffer.AppendFormatted("   char *argp = &arg[0][0];\n");
   buffer.AppendFormatted("   strcpy(arg[0],argv[0]);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   for (i=1;i<argc;i++) {\n");
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
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gSystem->AddIncludePath(kAdditionalInclude);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (interactive) {\n");
   buffer.AppendFormatted("      TRint *intapp = new TRint(\"App\", &argc, argv, 0, 0, kTRUE);\n");
   buffer.AppendFormatted("      intapp->SetPrompt(\"%s%s [%%d] \");\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   buffer.AppendFormatted("      cout<<%sLogo<<endl;\n", shortCut.Data());
   buffer.AppendFormatted("      intapp->Run();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      gROOT->SetBatch(nographics || batch || daemon);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      ROMERint *app = new ROMERint(\"App\", &argn, &argp,NULL,0,true);\n");
   buffer.AppendFormatted("      if (!gClient || gClient->IsZombie())\n");
   buffer.AppendFormatted("         nographics = true;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      new %sAnalyzer(app,batch,daemon,nographics);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("      char str[200];\n");
   buffer.AppendFormatted("      sprintf(str,\"ROME - %%s\", gAnalyzer->GetProgramName());\n");
   buffer.AppendFormatted("      SetConsoleTitle(str);\n");
   buffer.AppendFormatted("\n");
#endif
   buffer.AppendFormatted("      TFolder *fMainFolder = gROOT->GetRootFolder()->AddFolder(\"ROME\",\"ROME Folder\");\n");
   buffer.AppendFormatted("      fMainFolder->Add(gAnalyzer);\n");
   buffer.AppendFormatted("      gAnalyzer->SetCintInitialisation(\"%sAnalyzer* gAnalyzer = ((%sAnalyzer*)((TFolder*)gROOT->FindObjectAny(\\\"ROME\\\"))->GetListOfFolders()->MakeIterator()->Next());\");\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("      app->ProcessLine(gAnalyzer->GetCintInitialisation());\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("      if (!gAnalyzer->Start(argc, argv)) {\n");
   buffer.AppendFormatted("         delete gAnalyzer;\n");
   buffer.AppendFormatted("         return 1;\n");
   buffer.AppendFormatted("      }\n");
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

void ROMEBuilder::WriteHTMLDoku()
{
   int i=0,j=0,k=0;
   ROMEString buffer;
   ROMEString parentt;
   ROMEString format;
   int depthold=0;
   int depth=0;
   bool trodd = true;

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
   }
   else{
      buffer.AppendFormatted("<STYLE>\n");
      WriteHTMLStyle(buffer);
      buffer.AppendFormatted("</STYLE>\n");
   }
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
   if (affiliationList.GetEntries() > 0)
      buffer.AppendFormatted("<li><a href=\"#affiliations\">Affiliations</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"htmldoc/ClassIndex.html\">Class Overview</A></li>\n");
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
   for (i=0;i<numOfTask;i++) {
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
   buffer.AppendFormatted("<p>\n");
   int ddelta;
   depthold=1;
   buffer.AppendFormatted("<ul>\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      depth=taskHierarchyLevel[i];
      ddelta = depth-depthold;
      if (ddelta>0) for (k=0;k<ddelta;k++)  buffer.AppendFormatted("<ul>\n");
      if (ddelta<0) for (k=0;k<-ddelta;k++) buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<li type=\"circle\"><h4><a href=\"#%sTask\">%s</a></h4></li>\n",taskHierarchyName[i].Data(),taskHierarchyName[i].Data());
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#tasks\">Tasks</a>\n");
   buffer.AppendFormatted("<p>\n\n");
   buffer.AppendFormatted("<hr size=\"1\">\n");
   buffer.AppendFormatted("<p>\n\n");
   // Tasks
   ROMEString cppFile;
   ROMEString str;
   fstream *fileStream;
   ROMEString fileBuffer;
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("<h3><a name=%sTask class=\"object\">%s</a></h3>\n",taskName[i].Data(),taskName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("contact person : %s, %s\n",taskAuthor[i].Data(),taskAuthorEmail[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Description</h4>\n");
      buffer.AppendFormatted("%s\n",taskDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Usage</h4>\n");
      buffer.AppendFormatted("%s\n",taskUsage[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Status</h4>\n");
      buffer.AppendFormatted("%s\n",taskStatus[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>To Do</h4>\n");
      buffer.AppendFormatted("%s\n",taskToDo[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Known Problems</h4>\n");
      buffer.AppendFormatted("%s\n",taskKnownProblems[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Steering Parameters</h4>\n");
      buffer.AppendFormatted("<p>\n");
      if (numOfSteerChildren[i][0]>0 || numOfSteerFields[i][0]>0) {
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
         WriteHTMLSteering(buffer,0,i,"");
         buffer.AppendFormatted("</table>\n");
      }
      else {
         buffer.AppendFormatted("This task containes no steering parameters.\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Histograms</h4>\n");
      buffer.AppendFormatted("<p>\n");
      if (numOfHistos[i]>0) {
         buffer.AppendFormatted("This task containes the following histograms :\n");
         buffer.AppendFormatted("<ul>\n");
         for (j=0;j<numOfHistos[i];j++) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",histoName[i][j].Data());
         }
         buffer.AppendFormatted("</ul>\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      }
      else {
         buffer.AppendFormatted("This task containes no histograms.\n");
      }
      if (numOfGraphs[i]>0) {
         buffer.AppendFormatted("This task containes the following graphs :\n");
         buffer.AppendFormatted("<ul>\n");
         for (j=0;j<numOfGraphs[i];j++) {
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",graphName[i][j].Data());
         }
         buffer.AppendFormatted("</ul>\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");
      }
      else {
         buffer.AppendFormatted("This task containes no graphs.\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Folders</h4>\n");
      buffer.AppendFormatted("<p>\n");
      cppFile.SetFormatted("%s/src/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[i].Data());
      fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      buffer.AppendFormatted("This task accesses data from the following folders :\n");
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfFolder;j++) {
         if (!folderUsed[j])
            continue;
         if (accessFolder(fileBuffer,j))
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[j].Data());
      }
      buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>More Info</h4>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"htmldoc/%sT%s.html\">class file</a>\n",shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#tasks\">Tasks</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0,buffer.Length()-19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Tabs
   // -----
   buffer.AppendFormatted("<h2><a name=tabs>Tabs</a></h2>\n");
   buffer.AppendFormatted("\n");
   // Index
   buffer.AppendFormatted("<ul>\n");
   for (i=0;i<numOfTab;i++) {
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
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("<h3><a name=%sTab class=\"object\">%s</a></h3>\n",tabName[i].Data(),tabName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("contact person : %s, %s\n",tabAuthor[i].Data(),tabAuthorEmail[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Description</h4>\n");
      buffer.AppendFormatted("%s\n",tabDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Usage</h4>\n");
      buffer.AppendFormatted("%s\n",tabUsage[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Status</h4>\n");
      buffer.AppendFormatted("%s\n",tabStatus[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>To Do</h4>\n");
      buffer.AppendFormatted("%s\n",tabToDo[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Known Problems</h4>\n");
      buffer.AppendFormatted("%s\n",tabKnownProblems[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>Steering Parameters</h4>\n");
      buffer.AppendFormatted("<p>\n");
      if (numOfSteerChildren[numOfTask+1+i][0]>0 || numOfSteerFields[numOfTask+1+i][0]>0) {
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
         WriteHTMLSteering(buffer,0,numOfTask+1+i,"");
         buffer.AppendFormatted("</table>\n");
      }
      else {
         buffer.AppendFormatted("This tab containes no steering parameters.\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<h4>More Info</h4>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"htmldoc/%sT%s.html\">class file</a>\n",shortCut.Data(),tabName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#tabs\">Tabs</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0,buffer.Length()-19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Folders
   // -------
   // normal folders listing
   buffer.AppendFormatted("<h2><a name=folders>Folders</a></h2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following folder structure :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   depthold=0;
   depth=0;
   // Structure
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderSupport[i])
         continue;
      depth=0;
      if (folderParentName[i]!="GetMainFolder()") {
         depth++;
         parentt = folderParentName[i];
         for (j=0;j<maxNumberOfFolders;j++) {
            for (k=0;k<numOfFolder;k++) {
               if (!folderUsed[k])
                  continue;
               if (parentt==folderName[k]) {
                  parentt = folderParentName[k];
                  break;
               }
            }
            if (k>=numOfFolder) {
               cout << "Invalid folder structure." << endl;
               return;
            }
            if (folderParentName[k]=="GetMainFolder()")
               break;
            depth++;
         }
      }
      ddelta = depth-depthold;
      if (ddelta>0) for (k=0;k<ddelta;k++)  buffer.AppendFormatted("<ul>\n");
      if (ddelta<0) for (k=0;k<-ddelta;k++) buffer.AppendFormatted("</ul>\n");
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%sFolder\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      }
      else {
         buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[i].Data());
      }
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
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
   buffer.AppendFormatted("The %s%s incorporates the following support folders :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   depthold=0;
   depth=0;
   // Structure
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i])
         continue;
      if (numOfValue[i] > 0) {
         buffer.AppendFormatted("<b>\n");
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%sFolder\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("</b>\n");
      }
      else {
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
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] <= 0) continue;
      buffer.AppendFormatted("<h3><a name=%sFolder class=\"object\">%s</a></h3>\n",folderName[i].Data(),folderName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("%s\n",folderDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table>\n");
      if (folderDataBase[i])
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Database</td><td>Description</td></tr>\n");
      else
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td>",trodd ? "odd" : "even",valueName[i][j].Data(),valueType[i][j].Data());
         if (folderDataBase[i]) {
            if (valueDBPath[i][j].Length() || valueDBName[i][j].Length())
               buffer.AppendFormatted("<td><center><a href=\"#db_%s_%s\">Yes</a></center></td>",folderName[i].Data(),valueName[i][j].Data());
            else
               buffer.AppendFormatted("<td><center>No</center></td>");
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
   buffer = buffer(0,buffer.Length()-19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Database
   // --------
   ROMEStrArray dbList;
   dbList.RemoveAll();
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] <= 0) continue;
      if (!folderDataBase[i]) continue;
      for (j=0;j<numOfValue[i];j++) {
         if (!valueDBPath[i][j].Length() && !valueDBName[i][j].Length()) continue;
         if (dbList.IndexOf(valueDBName[i][j]) < 0)
            dbList.Add(valueDBName[i][j]);
      }
   }
   if (dbList.GetEntries() > 0) {
      buffer.AppendFormatted("<h2><a name=database>Database Connections</a></h2>\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("List of defualt database connections. These values can be overwritten by configuration file.\n");
      buffer.AppendFormatted("<p>\n");
   }
   Int_t iDB;
   for (iDB=0;iDB<dbList.GetEntries();iDB++) {
      buffer.AppendFormatted("<h3>%s</h3>\n", dbList.At(iDB).Data());
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Folder</td><td>Field</td><td>Database name</td><td>Database path</td></tr>\n");
      for (i=0;i<numOfFolder;i++) {
         if (!folderUsed[i])
            continue;
         if (numOfValue[i] <= 0) continue;
         if (!folderDataBase[i]) continue;
         for (j=0;j<numOfValue[i];j++) {
            if (!valueDBPath[i][j].Length() && !valueDBName[i][j].Length()) continue;
            if (dbList.At(iDB) == valueDBName[i][j])
               buffer.AppendFormatted("<tr class=\"%s\"><td>%s</td><td>&nbsp;<a name = db_%s_%s>%s</a>&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even",folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data(),valueName[i][j].Data(),valueDBName[i][j].Data(),valueDBPath[i][j].Data());
            trodd = !trodd;
         }
      }
      buffer.AppendFormatted("</table>\n");
      buffer.AppendFormatted("<a href=\"#top\">top</a>, <a href=\"#database\">Database Connections</a>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr size=\"1\">\n");
      buffer.AppendFormatted("<p>\n\n");
   }
   buffer = buffer(0,buffer.Length()-19);
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr size=\"4\" noshade>\n");
   buffer.AppendFormatted("<p>\n");

   // Trees
   // -----
   buffer.AppendFormatted("<h2><a name=trees>Trees</a></h2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following trees and branches.\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("In brackets are the folders, which are filled to the branch.\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",treeName[i].Data());
      if (treeDescription[i].Length())
         buffer.AppendFormatted("%s\n",treeDescription[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfBranch[i];j++) {
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
   WriteHTMLSteering(buffer,0,numOfTask,"");
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
      for (i=0;i<numOfEvent;i++) {
         for (j=0;j<numOfBank[i];j++) {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankName[i][j].Data());
               buffer.AppendFormatted("<ul>\n");
               for (k=0;k<numOfStructFields[i][j];k++) {
                  if (bankFieldArraySize[i][j][k]=="1") {
                     buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",structFieldName[i][j][k].Data());
                  }
                  else {
                     buffer.AppendFormatted("<li type=\"disc\">%s[%s]</li>\n",structFieldName[i][j][k].Data(),bankFieldArraySize[i][j][k].Data());
                  }
               }
               buffer.AppendFormatted("</ul>\n");
            }
            else {
               buffer.AppendFormatted("<li type=\"disc\">%s</li>\n",bankName[i][j].Data());
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
   if (affiliationList.GetEntries() > 0 ) {
      buffer.AppendFormatted("<h2><a name=affiliations>Affiliations</a></h2>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<ul>\n");
      if (affiliations.GetEntries() == 0) {
         buffer.AppendFormatted("<li>Enabled affiliations</li><ul>\n");
         for (i=0;i<affiliationList.GetEntries();i++)
            buffer.AppendFormatted("<li><a href=\"#%saff\">%s</a></li>\n", affiliationList.At(i, 0).Data(), affiliationList.At(i, 0).Data());
         buffer.AppendFormatted("</ul><li>Disabled affiliations</li><ul>\n");
         buffer.AppendFormatted("</ul>\n");
      }
      else {
         buffer.AppendFormatted("<li>Enabled affiliations</li><ul>\n");
         for (i=0;i<affiliationList.GetEntries();i++) {
            for (j=0;j<affiliations.GetEntries();j++) {
               if (affiliationList.At(i,0)==affiliations.At(j)) {
                  buffer.AppendFormatted("<li><a href=\"#%saff\">%s</a></li>\n", affiliationList.At(i, 0).Data(), affiliationList.At(i, 0).Data());
                  break;
               }
            }
         }
         buffer.AppendFormatted("</ul><li>Disabled affiliations</li><ul>\n");
         if (affiliations.GetEntries()) {
            for (i=0;i<affiliationList.GetEntries();i++) {
               for (j=0;j<affiliations.GetEntries();j++) {
                  if (affiliationList.At(i,0) == affiliations.At(j))
                     break;
               }
               if (j == affiliations.GetEntries())
                  buffer.AppendFormatted("<li><a href=\"#%saff\">%s</a></li>\n", affiliationList.At(i, 0).Data(), affiliationList.At(i, 0).Data());
            }
         }
         buffer.AppendFormatted("</ul>\n");
      }
      buffer.AppendFormatted("</ul>\n");
      for (i=0;i<affiliationList.GetEntries();i++) {
         buffer.AppendFormatted("<h3><a name=\"%saff\" class=\"object\">%s</a></h3>\n", affiliationList.At(i, 0).Data(), affiliationList.At(i, 0).Data());
         buffer.AppendFormatted("<p>\n");
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Type</td><td>Name</td></tr>\n");
         for (j=1;j<affiliationList.GetEntriesAt(i);j+=2) {
            buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even", affiliationList.At(i, j).Data(), affiliationList.At(i, j+1).Data());
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
   buffer.AppendFormatted("<H2><a name=accessmethods>Access Methods to Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("For a description of all access methods to all types of objects in the %s%s please visit the\n",shortCut.Data(),mainProgName.Data());
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
   buffer.AppendFormatted("%s</br>\n",mainAuthor.Data());
   buffer.AppendFormatted("%s</br>\n",mainInstitute.Data());
   buffer.AppendFormatted("%s</br>\n",mainCollaboration.Data());
   buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
   buffer.AppendFormatted("<u> Contact person from ROME</u></br>\n");
   buffer.AppendFormatted("Matthias Schneebeli (PSI)</br>\n");
   buffer.AppendFormatted("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
   buffer.AppendFormatted("</ADDRESS>\n");
   buffer.AppendFormatted("</BODY>\n");
   buffer.AppendFormatted("</HTML>\n");

   // Write documentation
   ROMEString htmlFile;
   htmlFile.SetFormatted("%s%s.html",shortCut.Data(),mainProgName.Data()); // we are already in outputDir
   WriteFile(htmlFile.Data(),buffer.Data(),0);

   // Write style sheet
   ROMEString css;
   if (styleSheet.Length()) {
      css.SetFormatted("%s/%s",outDir.Data(),styleSheet.Data());
      if (gSystem->AccessPathName(css.Data(),kFileExists)) {
         buffer.Resize(0);
         WriteHTMLStyle(buffer);
         WriteFile(css.Data(),buffer.Data(),0);
      }
   }

   // Write UserHTML
/*
   ROMEString userHtmlFile;
   userHtmlFile.SetFormatted("%s/%sUserHTML.html",outDir.Data(),shortCut.Data());
   if (gSystem->AccessPathName(userHtmlFile.Data(),kFileExists)) {
      buffer.Resize(0);
      buffer.AppendFormatted("<html>\n");
      buffer.AppendFormatted("<head>\n");
      buffer.AppendFormatted("  <title>%s%s Additional Info</title>\n",shortCut.Data(),mainProgName.Data());
      if (styleSheet.Length()) {
         buffer.AppendFormatted("<LINK rel=\"stylesheet\" type=\"text/css\" href=\"%s/%s\">\n",outDir.Data(),styleSheet.Data());
      }
      else{
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
      buffer.AppendFormatted("%s</br>\n",mainAuthor.Data());
      buffer.AppendFormatted("%s</br>\n",mainInstitute.Data());
      buffer.AppendFormatted("%s</br>\n",mainCollaboration.Data());
      buffer.AppendFormatted("email: <a href=\"mailto:%s\">%s</a><p>\n",mainEmail.Data(),mainEmail.Data());
      buffer.AppendFormatted("<u> Contact person from ROME</u></br>\n");
      buffer.AppendFormatted("Matthias Schneebeli (PSI)</br>\n");
      buffer.AppendFormatted("email: <a href=\"mailto:matthias.schneebeli@psi.ch\">matthias.schneebeli@psi.ch</a><p>\n");
      buffer.AppendFormatted("</address>\n");
      buffer.AppendFormatted("</body>\n");
      buffer.AppendFormatted("</html>");
      WriteFile(userHtmlFile.Data(),buffer.Data(),0);
   }
*/
}

void ROMEBuilder::WriteHTMLSteering(ROMEString &buffer,Int_t numSteer,Int_t numTask,const char* group)
{
   int k;
   ROMEString groupName;
   ROMEString tmp;
   bool trodd = true;
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even",steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),ProcessCommentHTML(steerFieldComment[numTask][numSteer][k],tmp).Data());
      trodd = !trodd;
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTask][numSteer];k++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][k]])
         continue;
      groupName = group;
      if (groupName.Length())
         groupName.AppendFormatted("/");
      groupName.AppendFormatted("%s",steerName[numTask][steerChildren[numTask][numSteer][k]].Data());
      buffer.AppendFormatted("<tr class=\"group\"><td colspan=\"3\">&nbsp;%s&nbsp;</td></tr>\n",groupName.Data());
      WriteHTMLSteering(buffer,steerChildren[numTask][numSteer][k],numTask,groupName.Data());
   }
}

void ROMEBuilder::WriteHTMLStyle(ROMEString& buffer)
{
      buffer.AppendFormatted("   body { color: #000000; background-color: #ffffff; }\n");
      buffer.AppendFormatted("   h1 { color: blue; }\n");
      buffer.AppendFormatted("   h2 { color: green; }\n");
      buffer.AppendFormatted("   table { border: 1; }\n");
      buffer.AppendFormatted("   a.object { text-decoration: underline; font-weight: bold; }\n");
      buffer.AppendFormatted("   tr.cont { color: #ffffff; background-color: #819cc8; font-weight: bold; }\n");
      buffer.AppendFormatted("   tr.group { background-color: #d0d0ee; }\n");
      buffer.AppendFormatted("   tr.even { background-color: #e0e0e0; }\n");
      buffer.AppendFormatted("   tr.odd { background-color: #f0f0f0; }\n");
}

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
   ROMEString format;
   ROMEString tmp,tmp2,tmp3,tmp4;
   ROMEString macroDescription;
   buffer.Resize(0);
   macroDescription.Resize(0);

   // File name
   cFile.SetFormatted("%ssrc/generated/%sReadTrees.C", outDir.Data(), shortCut.Data());
   WriteHeader(buffer, mainAuthor, true);
   macroDescription.AppendFormatted("This macro shows how to read output file from %s%s.exe.\n\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   macroDescription.AppendFormatted(" Usage\n");
   macroDescription.AppendFormatted("   %% %s%s.exe -I;\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   macroDescription.AppendFormatted("   %s%s [0] gSystem->Load(\"lib%s%s.so\");\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2), shortCut.ToLower(tmp3), mainProgName.ToLower(tmp4));
   macroDescription.AppendFormatted("   %s%s [1] .L src/generated/%sReadTrees.C\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2), shortCut.Data());
   macroDescription.AppendFormatted("   %s%s [2] %sReadTrees(1, 0, 10)\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2), shortCut.Data());
   macroDescription.AppendFormatted("\n");
   macroDescription.AppendFormatted(" Arguments\n");
   macroDescription.AppendFormatted("   run_num : run number\n");
   macroDescription.AppendFormatted("   ev_1    : the first event number to print\n");
   macroDescription.AppendFormatted("   ev_2    : the last event number to print\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      macroDescription.AppendFormatted("   read%s : switch to read %s tree\n", treeName[iTree].Data(), treeName[iTree].Data());
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
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMEiostream.h\"\n");

   // check if branch
   Bool_t *isBranch = new Bool_t[maxNumberOfFolders];
   int **branchFolderNum = new int*[maxNumberOfTrees];
   for (i=0;i<maxNumberOfTrees;i++) {
      branchFolderNum[i] = new int[maxNumberOfBranches];
   }
   for (iFold = 0; iFold < numOfFolder; iFold++) {
      isBranch[iFold] = false;
      for (iTree = 0; iTree < numOfTree; iTree++) {
         for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
            branchFolderNum[iTree][iBranch] = -1;
         }
      }
      if (!folderUsed[iFold])
         continue;
      if (numOfValue[iFold] <= 0 || folderSupport[iFold])
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
   buffer.AppendFormatted("                 , Int_t ev_2 = 10\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      buffer.AppendFormatted("                 , Bool_t read%s = kTRUE\n", treeName[iTree].Data());
   buffer.AppendFormatted("                 ) {\n");
//   buffer.AppendFormatted("   gSystem->Load(\"lib%s%s.so\");\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));

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
      buffer.AppendFormatted("      %s = ((TTree*) %sFile->Get(\"%s\"));\n", treeName[iTree].Data(), treeName[iTree].Data(), treeName[iTree].Data());
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("\n");

   // Folder Fields
   const int scl = shortCut.Length();
   const int ltc = strlen("TClonesArray");
   int typeLen = ltc;
   for (iFold = 0; iFold < numOfFolder;iFold++) {
      if (!folderUsed[iFold])
         continue;
      if (!isBranch[iFold])
         continue;
      if (typeLen < folderName[iFold].Length() + scl)
         typeLen = folderName[iFold].Length() + scl;
   }
   buffer.AppendFormatted("   // Create objects to fill data\n");
   for (iFold = 0; iFold < numOfFolder; iFold++) {
      if (!folderUsed[iFold])
         continue;
      if (!isBranch[iFold])
         continue;
      if (folderArray[iFold] == "1") {
         format.SetFormatted("   %%s%%s*%%%ds %%s = new %%s%%s();\n", typeLen - folderName[iFold].Length() - scl);
         buffer.AppendFormatted(format.Data(), shortCut.Data(), folderName[iFold].Data(), "", folderName[iFold].Data(), shortCut.Data(), folderName[iFold].Data());
      }
      else {
         format.SetFormatted("   TClonesArray*%%%ds %%s = new TClonesArray(\"%%s%%s\");\n", typeLen - ltc);
         buffer.AppendFormatted(format.Data(), "", folderName[iFold].Data(), shortCut.Data(), folderName[iFold].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Set address
   buffer.AppendFormatted("   // Map branchs and objects\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      buffer.AppendFormatted("   if (read%s) {\n", treeName[iTree].Data());
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         buffer.AppendFormatted("      %s->SetBranchAddress(\"%s\", &%s);\n", treeName[iTree].Data(), branchName[iTree][iBranch].Data(), branchFolder[iTree][iBranch].Data());
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("\n");

   // Print
   buffer.AppendFormatted("   Int_t i;\n");
   buffer.AppendFormatted("   // Event loop\n");
   buffer.AppendFormatted("   for (i = ev_1; i < ev_2; i++) {\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      buffer.AppendFormatted("      if (read%s && i >= %s->GetEntries()) break;\n", treeName[iTree].Data(), treeName[iTree].Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("      // Read event\n");
   for (iTree = 0; iTree < numOfTree; iTree++)
      buffer.AppendFormatted("      if (read%s) %s->GetEntry(i);\n", treeName[iTree].Data(), treeName[iTree].Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("      // Print data\n");
   buffer.AppendFormatted("      cout<<\"*******  Event-\"<<i<<\"  *******\"<<endl;\n");
   for (iTree = 0; iTree < numOfTree; iTree++) {
      buffer.AppendFormatted("      if (read%s) {\n", treeName[iTree].Data());
      for (iBranch = 0; iBranch < numOfBranch[iTree]; iBranch++) {
         iFold = branchFolderNum[iTree][iBranch];
         if (iFold==-1)
            continue;
         if (folderArray[iFold] == "1") {
            for (iValue = 0; iValue < numOfValue[iFold]; iValue++) {
               if (isFolder(valueType[iFold][iValue].Data())) {
                  // not yet implemented
               }
               else {
                  if (valueDimension[iFold][iValue] > 0 ) {
                     buffer.AppendFormatted("//       cout<<\"   //%s/%s/%s\t\"<<%s->Get%sAt(",treeName[iTree].Data(), branchName[iTree][iBranch].Data(), valueName[iFold][iValue].Data(), folderName[iFold].Data(), valueName[iFold][iValue].Data());
                     for (iDm = 0; iDm < valueDimension[iFold][iValue]; iDm++)
                        buffer.AppendFormatted("0, ");
                     buffer.Resize(buffer.Length()-2);
                     buffer.AppendFormatted(")<<endl;\n");
                  }
                  else {
                     buffer.AppendFormatted("         cout<<\"   //%s/%s/%s\t\"<<%s->Get%s()<<endl;\n",treeName[iTree].Data(), branchName[iTree][iBranch].Data(), valueName[iFold][iValue].Data(), folderName[iFold].Data(), valueName[iFold][iValue].Data());
                  }
               }
            }
         }
         else{
            buffer.AppendFormatted("         if (%s->GetEntries()) {\n", folderName[iFold].Data());
            for (iValue = 0; iValue < numOfValue[iFold]; iValue++) {
               if (isFolder(valueType[iFold][iValue].Data())) {
                  // not yet implemented
               }
               else {
                  if (valueDimension[iFold][iValue] > 0 ) {
                     buffer.AppendFormatted("//          cout<<\"   //%s/%s/%s\t\"<<((%s%s*) %s->At(0))->Get%sAt(",treeName[iTree].Data(), branchName[iTree][iBranch].Data(), valueName[iFold][iValue].Data(), shortCut.Data(), folderName[iFold].Data(), folderName[iFold].Data(), valueName[iFold][iValue].Data());
                     for (iDm = 0; iDm < valueDimension[iFold][iValue]; iDm++)
                        buffer.AppendFormatted("0, ");
                     buffer.Resize(buffer.Length()-2);
                     buffer.AppendFormatted(")<<endl;\n");
                  }
                  else {
                     buffer.AppendFormatted("            cout<<\"   //%s/%s/%s\t\"<<((%s%s*) %s->At(0))->Get%s()<<endl;\n",treeName[iTree].Data(), branchName[iTree][iBranch].Data(), valueName[iFold][iValue].Data(), shortCut.Data(), folderName[iFold].Data(), folderName[iFold].Data(), valueName[iFold][iValue].Data());
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

   delete isBranch;
   for (i=0;i<maxNumberOfTrees;i++) {
      delete branchFolderNum[i];
   }
   delete branchFolderNum;
   return true;
}

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
         }
         else {
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
   WriteHeader(buffer, mainAuthor, true);
   headerDescription.AppendFormatted("This header contains Subversion revision code of %s%s.\n\n", shortCut.Data(), mainProgName.Data());
   headerDescription.AppendFormatted("It makes sense when you use Subversion.\n");
   headerDescription.AppendFormatted("\n");
   WriteDescription(buffer, gSystem->BaseName(hFile.Data()), headerDescription.Data(), false);
   buffer.AppendFormatted("\n\n");
   buffer.AppendFormatted("#ifndef %sVersion_H\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("#define %sVersion_H\n",shortCut.ToUpper(tmp));
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#define %s_SVN_REVISION_CODE %s\n",shortCut.ToUpper(tmp),revNumber.Data());
/*
   buffer.AppendFormatted("#define %s_RELEASE_DATE \"%s %2d %d\"\n",shortCut.ToUpper(tmp)
                          , monthName[month], day, year);
   buffer.AppendFormatted("#define %s_RELEASE_TIME \"%02d:%02d:%02d\"\n",shortCut.ToUpper(tmp)
                          , hour, min, sec);
*/
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n\n");

   // Write File
   WriteFile(hFile.Data(),buffer.Data(),6);
   return true;
}

Bool_t ROMEBuilder::WriteLinkDefHs()
{
   ROMEString filename;

/*
   filename = "dict/ROMELinkDef.h";
   if (!WriteLinkDefH(romeDictHeaders, romeLinkDefSuffix, filename.Data()))
      return false;
   filename = "dict/ARGUSLinkDef.h";
   if (!WriteLinkDefH(argusHeaders, argusLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sGeneratedLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(generatedDictHeaders, generatedLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sGeneratedFolderLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(generatedFolderDictHeaders, generatedFolderLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sGeneratedSupportFolderLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(generatedSupportFolderDictHeaders, generatedSupportFolderLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sGeneratedTaskLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(generatedTaskDictHeaders, generatedTaskLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sGeneratedTabLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(generatedTabDictHeaders, generatedTabLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sFolderLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(folderHeaders, folderLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sTaskLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(taskHeaders, taskLinkDefSuffix, filename.Data()))
      return false;
   filename.SetFormatted("dict/%sTabLinkDef.h",shortCut.Data());
   if (!WriteLinkDefH(tabHeaders, tabLinkDefSuffix, filename.Data()))
      return false;
*/

   return true;
}

Bool_t ROMEBuilder::WriteLinkDefH(ROMEStrArray *headers, ROMEStrArray *ldsuffix, const char* filename)
{
   ROMEString buffer;
   ROMEString classname;
   int i;

   buffer.SetFormatted("#ifdef __CINT__\n");
   if (headers->GetEntriesFast()>0) {
      for (i=0;i<headers->GetEntriesFast();i++) {
         classname = gSystem->BaseName(headers->At(i).Data());
         classname.Resize(classname.Length()-2); // remove ".h"
         if (ldsuffix->At(i).Length()>0)
            buffer.AppendFormatted("#pragma link C++ class %s%s;\n", classname.Data(), ldsuffix->At(i).Data());
         else
            buffer.AppendFormatted("#pragma link C++ class %s;\n", classname.Data());
      }
      buffer.AppendFormatted("#endif\n");
      WriteFile(filename, buffer.Data(), 6);
   }

   return true;
}

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
      WriteHeader(buffer, mainAuthor, true);
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

Bool_t ROMEBuilder::ReplaceHeader(const char* filename,const char* header,const char* body,Int_t nspace,const char* str1, const char* str2,const char* condition, bool replaceWhenFound)
{
   ROMEStrArray arr1;
   ROMEStrArray arr2;
   ROMEStrArray cond;
   TArrayI rep(1);
   arr1.Add(str1);
   arr2.Add(str2);
   cond.Add(condition);
   rep.AddAt((int)replaceWhenFound,0);
   return ReplaceHeader(filename,header,body,nspace,arr1,arr2,cond,rep);
}

Bool_t ROMEBuilder::ReplaceHeader(const char* filename,const char* header,const char* body,Int_t nspace,ROMEStrArray &arr1, ROMEStrArray &arr2, ROMEStrArray &condition, TArrayI &replaceWhenFound)
{
   int i;
   bool writeFile = false;
   fstream *fileStream;
   ROMEString fileBuffer;
   Ssiz_t pBuffer=-1;
   ROMEString buffer;
   if (header!=0)
      buffer = header;
   if (gSystem->AccessPathName(filename,kFileExists)) {
      writeFile = true;
      buffer += body;
   }
   else {
      if (!(fileStream = new fstream(filename,ios::in))) {
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return false;
      }
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      if (header!=0) {
         pBuffer = fileBuffer.Index(kHeaderEndMark);
         if (pBuffer<0) {
            if (makeOutput)
               cout << "\n\nWarning : File '" << filename << "' does not have header end mark. Builder does not modify this file." << endl;
            return true;
         }
         // compare old and new header
         if (fileBuffer(0,pBuffer+80) != header)
            writeFile = true;
      }
      //check if the file has string to be replaced.
      for (i=0;i<TMath::Min(TMath::Min(arr1.GetEntriesFast(),arr2.GetEntriesFast()),TMath::Min(condition.GetEntriesFast(),replaceWhenFound.GetSize()));i++) {
         if (arr1.At(i).Length()>0  && fileBuffer.Contains(arr1.At(i))) {
            if (condition.At(i).Length()>0 && (fileBuffer.Contains(condition.At(i)) && replaceWhenFound.At(i)) ||
                                              (!fileBuffer.Contains(condition.At(i)) && !replaceWhenFound.At(i))) {
               writeFile = true;
               fileBuffer.ReplaceAll(arr1.At(i), arr2.At(i));
            }
         }
      }
#if 1
// this special treatment is neccesary only for several month from Jun.2006
      //check if the file has string to be replaced.
      ROMEString filenameStr = filename;
      ROMEString analyzerInclude[4];
      analyzerInclude[0].SetFormatted("#include <generated/%sAnalyzer.h>",shortCut.Data());
      analyzerInclude[1].SetFormatted("#include <include/generated/%sAnalyzer.h>",shortCut.Data());
      analyzerInclude[2].SetFormatted("#include \"generated/%sAnalyzer.h\"",shortCut.Data());
      analyzerInclude[3].SetFormatted("#include \"include/generated/%sAnalyzer.h\"",shortCut.Data());
      if (filenameStr.Contains("src/tasks") || filenameStr.Contains("src/tabs")) {
         if (!fileBuffer.Contains(analyzerInclude[0])
            && !fileBuffer.Contains(analyzerInclude[1])
            && !fileBuffer.Contains(analyzerInclude[2])
            && !fileBuffer.Contains(analyzerInclude[3])
            ) {
            writeFile = true;
            fileBuffer.Insert(strlen(header), analyzerInclude[2]);
            fileBuffer.Insert(strlen(header), "\n");
         }
      }
      if (header==0)
         buffer = fileBuffer;
   }
#endif
   if (writeFile) {
      if (pBuffer>=0)
         buffer += fileBuffer(pBuffer+80,fileBuffer.Length());
      if (!(fileStream = new fstream(filename,ios::out | ios::trunc))) {
         if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
         return false;
      }
      if (makeOutput) cout << setw(nspace) << "" << filename << endl;
      *fileStream<<buffer.Data();
      delete fileStream;
   }
   return true;
}

Bool_t ROMEBuilder::BackUpFile(const char* filename)
{
   // return true when backup file is created.
   if (gSystem->AccessPathName(filename, kFileExists))
      return false;

   ROMEString newName;
   Int_t i;
   int errcode; // error code is int instead of Int_t
   for (i = 0; i < kMaxInt; i++) {
      if (i == 0)
         newName.SetFormatted("%s.save", filename);
      else
         newName.SetFormatted("%s.save.%d", filename, i);

      if (gSystem->AccessPathName(newName.Data(), kFileExists)) {
         errcode = gSystem->Rename(filename, newName.Data());
         if (errcode) {
            cerr<<"Error:"<<errcode<<"\t"<<gSystem->GetErrorStr()<<endl;
            return false;
         }
         else {
            if (makeOutput)
               cout<<"   renamed '"<<filename<<"' -> '"<<newName<<"'."<<endl;
            return true;
         }
      }
   }
   return true;
}

Bool_t ROMEBuilder::accessFolder(ROMEString &fileBuffer, Int_t numFolder)
{
   if (folderSupport[numFolder])
      return false;

   ROMEString str;

   // Get
   str = "Get";
   str += folderName[numFolder];
   if (folderArray[numFolder]!="1")
      str += "At";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Get s
   if (folderArray[numFolder]!="1") {
      str = "Get";
      str += folderName[numFolder];
      str += "s";
      str += "(";
      if (fileBuffer.Contains(str))
         return true;
   }

   // Get Address
   str = "Get";
   str += folderName[numFolder];
   str += "Address";
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Set size
   if (folderArray[numFolder]=="variable") {
      str = "Set";
      str += folderName[numFolder];
      str += "Size(";
      if (fileBuffer.Contains(str))
         return true;
   }

   // Set Array
   if (folderArray[numFolder]=="variable") {
      str = "Set";
      str += folderName[numFolder];
      str += "s(";
      if (fileBuffer.Contains(str))
         return true;
   }

   return false;
}
Bool_t ROMEBuilder::accessHisto(ROMEString &fileBuffer, Int_t numTask, Int_t numHisto)
{
   ROMEString str;

   // Get
   str = "Get";
   str += histoName[numTask][numHisto];
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Get At
   if (histoArraySize[numTask][numHisto]!="1") {
      str = "Get";
      str += histoName[numTask][numHisto];
      str += "At";
      str += "(";
      if (fileBuffer.Contains(str))
         return true;
   }
   return false;
}
Bool_t ROMEBuilder::accessGraph(ROMEString &fileBuffer, Int_t numTask, Int_t numGraph)
{
   ROMEString str;

   // Get
   str = "Get";
   str += graphName[numTask][numGraph];
   str += "(";
   if (fileBuffer.Contains(str))
      return true;

   // Get At
   if (graphArraySize[numTask][numGraph]!="1") {
      str = "Get";
      str += graphName[numTask][numGraph];
      str += "At";
      str += "(";
      if (fileBuffer.Contains(str))
         return true;
   }
   return false;
}

void ROMEBuilder::WriteHeader(ROMEString& buffer, const char* author, Bool_t overwrite)
{
   if (author && strlen(author))
      buffer.AppendFormatted("// Author: %s\n\n", author);

   if (overwrite) {
      buffer.AppendFormatted("/******************************************************************************\n");
      buffer.AppendFormatted(" *         ***  This file will be overwritten by the ROMEBuilder  ***         *\n");
      buffer.AppendFormatted(" *          ***      Don't make manual changes to this file      ***          *\n");
      buffer.AppendFormatted(" ******************************************************************************/\n\n");
   }
}

#define ALIGN_DESC // align description 80 chars.
void ROMEBuilder::WriteDescription(ROMEString& buffer, const char* className, const char* description, Bool_t endmark)
{
   const Int_t nc = 80;
   ROMEString format;
   ROMEString desc = description;
   ROMEStrArray descs;
   Ssiz_t p, pLast;
#if defined ( ALIGN_DESC )
   Ssiz_t pSpace;
#endif
   Int_t i;
   char *tmp = new char[desc.Length() + 1];

   // analyze description
   if (description && strlen(description)) {
      pLast = 0;
#if defined ( ALIGN_DESC )
      pSpace = 0;
#endif
      for (p = 0; p < desc.Length(); p++) {
         if (p == desc.Length() - 1) {
            strcpy(tmp, desc(pLast, p - pLast).Data());
            tmp[p - pLast + 1] = '\0';
            if (tmp[p - pLast] == '\n')
               tmp[p - pLast] = '\0';
            descs.Add(tmp);
            break;
         }
         else if (desc[p] == '\n') {
            strcpy(tmp, desc(pLast, p - 1 - pLast).Data());
            tmp[ p - 1 - pLast + 1] = '\0';
            descs.Add(tmp);
            pLast = p + 1;
            continue;
         }
#if defined ( ALIGN_DESC )
         else if (isspace(desc[p])) {
            pSpace = p;
         }
         else if (p >= pLast + nc - static_cast<Ssiz_t>(strlen("//  //")) - 1) {
            if (pLast > pSpace) {
               strcpy(tmp, desc(pLast, p - pLast).Data());
               tmp[p - pLast + 1] = '\0';
               descs.Add(tmp);
               pLast = p;
            }
            else {
               strcpy(tmp, desc(pLast, pSpace - pLast).Data());
               tmp[pSpace - pLast + 1] = '\0';
               descs.Add(tmp);
               pLast = pSpace + 1;
            }
            continue;
         }
#endif
      }
   }
   delete [] tmp;

   if (className && strlen(className)) {
      // cast to avoid inconsistency between format and arguments.
      buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
      buffer.AppendFormatted("//                                                                            //\n");
      format.SetFormatted("// %%s%%%ds //\n", (int)(nc - strlen("//  //") - strlen(className)));
      buffer.AppendFormatted(format.Data(), className, "");
      buffer.AppendFormatted("//                                                                            //\n");
      for (i = 0; i < descs.GetEntries(); i++) {
#if defined ( ALIGN_DESC )
         if (descs[i].BeginsWith("#include") || descs[i].BeginsWith("#pragma")) {
            buffer.AppendFormatted("%s\n", descs[i].Data());
         }
         else {
            format.SetFormatted("// %%s%%%ds //\n", (int)(nc - strlen("//  //") - descs[i].Length()));
            buffer.AppendFormatted(format.Data(), descs[i].Data(), "");
         }
#else
         if (descs[i].BeginsWith("#include") || descs[i].BeginsWith("#pragma")) {
            buffer.AppendFormatted("%s\n", descs[i].Data());
         }
         else {
            buffer.AppendFormatted("// %s\n", descs[i].Data());
         }
#endif
      }
      buffer.AppendFormatted("//                                                                            //\n");
      if (endmark) {
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
         buffer.AppendFormatted("/*  This header was generated by ROMEBuilder. Manual changes above the        *\n");
         buffer.AppendFormatted(" * following line will be lost next time ROMEBuilder is executed.             */\n");
         buffer.AppendFormatted(kHeaderEndMark);
      }
      else {
         buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////");
      }
   }
}

Bool_t ROMEBuilder::RemoveFile(const char* filename, const char* str)
{
   // Remove file if exists.
   // If str!=0, remove file only when it contains 'str';
   // return true when the file is removed.
   if (!filename)
      return false;

   if (!gSystem->AccessPathName(filename,kFileExists)) {
      bool removeFile = false;
      ROMEString fileBuffer;
      fstream *fileStream;
      if (!str) {
         removeFile = true;
      }
      else {
         if (!(fileStream = new fstream(filename,ios::in))) {
            if (makeOutput) cout << "\n\nError : Failed to open '" << filename << "' !!!" << endl;
            return false;
         }
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
         if (fileBuffer.Contains(str))
            removeFile = true;
      }
      if (removeFile) {
         ROMEString tmp;
         tmp.SetFormatted("rm -f %s", filename);
         gSystem->Exec(tmp.Data());
         if (makeOutput)
            cout<<"   removed '"<<filename<<"'."<<endl;
         return true;
      }
   }

   return false;
}

void ROMEBuilder::RemoveDepFiles(const char* str)
{
   // Remove .d files in 'obj' directory
   // If str!=0, remove files only when it contains 'str';
   char *direntry;
   TString afile;
   TString dirname = outDir;
   dirname += "obj";
   void *dirp = gSystem->OpenDirectory(dirname);
   if (!dirp)
      return;

   //loop on all entries of this directory
   while ((direntry = (char*)gSystem->GetDirEntry(dirp))) {
      TString afile = Form("%s/%s", dirname.Data(), direntry);
      if (afile.EndsWith(".d"))
         RemoveFile(afile.Data(), str);
   }

   return;
}

ROMEString& ROMEBuilder::ProcessCommentCPP(ROMEString& org, ROMEString& result)
{
   result = org;
   result.ReplaceAll("\n", "\n// ");
   if (! result.BeginsWith("//"))
      result.Insert(0, "// ");
   return result;
}

ROMEString& ROMEBuilder::ProcessCommentHTML(ROMEString& org, ROMEString& result)
{
   result = org;
   result.ReplaceAll("\n", "<br>\n");
   return result;
}

ROMEString& ROMEBuilder::ProcessCommentString(ROMEString& org, ROMEString& result)
{
   result = org;
   result.ReplaceAll("\"", "\\\"");
   result.ReplaceAll("\n", "\\n\"\n\"");
   return result;
}

ROMEString& ROMEBuilder::GetSteerPath(ROMEString& steerPath,int iTask,int iSteer,int iField,const char* seperator)
{
   steerPath.SetFormatted("%s",steerFieldName[iTask][iSteer][iField].Data());
   while (steerParent[iTask][iSteer]!=-1) {
      iSteer = steerParent[iTask][iSteer];
      steerPath.InsertFormatted(0,"%s%s",steerName[iTask][iSteer].Data(),seperator);
   }
   return steerPath;
}
