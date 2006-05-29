/********************************************************************

  ROMEBuildWriteCode.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include "RConfig.h"

#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include "TSystem.h"
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include "Riostream.h"
#include "ROMEBuilder.h"

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
      buffer.AppendFormatted("#include \"generated/%s.h\"",clsName.Data());
      buffer.AppendFormatted("\nClassImp(%s)\n",clsName.Data());

      // Constructor
      buffer.AppendFormatted("%s::%s( ",clsName.Data(),clsName.Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]==0) {
            if (isFolder(valueType[iFold][i].Data()))
               continue;
            if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString") && !isTArrayType(valueType[iFold][i]))
               continue;
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
      buffer.AppendFormatted("   %s::Class()->IgnoreTObjectStreamer();\n",clsName.Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i].Contains("*"))
            relation = "->";
         else
            relation = ".";
         if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
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
      buffer.AppendFormatted("   fModified = false;\n");
      buffer.AppendFormatted("};\n");
      buffer.AppendFormatted("\n");

      // Destructor
      buffer.AppendFormatted("%s::~%s()\n",clsName.Data(),clsName.Data());
      buffer.AppendFormatted("{\n");
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            buffer.AppendFormatted("   delete %s;\n",valueName[iFold][i].Data());
         else if (isTArrayType(valueType[iFold][i]) && valueType[iFold][i].Contains("*"))
            buffer.AppendFormatted("   delete %s;\n",valueName[iFold][i].Data());
         else if (valueDimension[iFold][i]!=0 && valueArray[iFold][i][0]=="variable")
            buffer.AppendFormatted("   delete [] %s;\n",valueName[iFold][i].Data());
      }
      buffer.AppendFormatted("};\n");
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
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t index)\n",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i]) {
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", index))\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
                  buffer.AppendFormatted("      return %s;\n",valueInit[iFold][i].Data());
               }
               buffer.AppendFormatted("   return (%s)(%s->At(index));\n",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("};\n");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* arrayToCopy)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer),"");
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s%sGetSize() || !n) return;\n",valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("   if (!arrayToCopy) arrayToCopy = new %s[n];\n",TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("   memcpy(arrayToCopy,%s%sGetArray(),n*sizeof(%s));\n",valueName[iFold][i].Data(),relation.Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
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
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t index)\n",valueType[iFold][i].Data(),shortCut.Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", index)) return %s;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               buffer.AppendFormatted("   return %s[index];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("};\n");
            }
            else if (valueArray[iFold][i][0]=="variable") {
               buffer.AppendFormatted("%s %s::Get%sAt(Int_t index)\n",valueType[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Get%sAt\", index)) return %s;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueInit[iFold][i].Data());
               buffer.AppendFormatted("   return %s[index];\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("void %s::Get%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!%s || !n) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (!array) array = new %s[n];\n",valueType[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(array,%s,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
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
            }
         }
         else {
            if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
                && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
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
            }
            else if (isFolder(valueType[iFold][i].Data())) {
               tmp = valueType[iFold][i];
               tmp.ReplaceAll("*","");
               buffer.AppendFormatted("void %s::Set%sSize(Int_t number)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   int i;\n");
               buffer.AppendFormatted("   if (%s) %s->Delete();\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   for (i=0;i<number;i++) {\n");
               buffer.AppendFormatted("      new((*%s)[i]) %s;\n",valueName[iFold][i].Data(),tmp.Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer),"");
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
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
               continue;
            else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
            }
            else if (valueArray[iFold][i][0]=="variable") {
               buffer.AppendFormatted("void %s::Set%sAt(Int_t index,%s %s_value) {\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               if (!valueNoBoundChech[iFold][i])
                  buffer.AppendFormatted("   if (!%sBoundsOk(\"Set%sAt\", index)) return;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   %s[index] = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");

               buffer.AppendFormatted("void %s::Set%sSize(Int_t number)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (number==%sSize) return;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   if (number<0) return;\n");
               buffer.AppendFormatted("   if (number>%sActualSize) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s *tmp = new %s[number];\n",valueType[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      memcpy(tmp,%s,%sSize*sizeof(%s));\n",valueName[iFold][i].Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("      delete [] %s;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s = tmp;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %sActualSize = number;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   }\n");
               buffer.AppendFormatted("   %sSize = number;\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),valueType[iFold][i].Data(),"");
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || n<=0) return;\n");
               buffer.AppendFormatted("   if (%sActualSize<n) Set%sSize(n);\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
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

               buffer.AppendFormatted("void %s::Set%sCopy(Int_t n,%s* array)\n",clsName.Data(),valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   if (!array || !n) return;\n");
               buffer.AppendFormatted("   memcpy(%s,array,n*sizeof(%s));\n",valueName[iFold][i].Data(),valueType[iFold][i].Data());
               buffer.AppendFormatted("   SetModified(true);\n");
               buffer.AppendFormatted("   return;\n");
               buffer.AppendFormatted("}\n");
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
         if (isRootClassType(valueType[iFold][i].Data())
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
               buffer.AppendFormatted("};\n");
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
               buffer.AppendFormatted("};\n");
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
      buffer.AppendFormatted("};\n");
      buffer.AppendFormatted("\n");

      // Set All
      buffer.AppendFormatted("void %s::SetAll( ",clsName.Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (isFolder(valueType[iFold][i].Data()))
            continue;
         if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
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
         if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         if (valueDimension[iFold][i]==0) {
            buffer.AppendFormatted("   %s = %s_value;\n",valueName[iFold][i].Data(),valueName[iFold][i].Data());
         }
      }
      buffer.AppendFormatted("   SetModified(true);\n");
      buffer.AppendFormatted("};\n");
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
         if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
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
         else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
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
      buffer.AppendFormatted("};\n");
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
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i)",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index %%d out of bounds (size: %%d, this: 0x%%08x)\", i, Get%sSize(), this);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
            }
            else if (valueArray[iFold][i][0]=="variable") {
#if 0 // disabled because no way to know array size when the folder is read from TFile.
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (at < 0 || at >= Get%sSize())\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i)",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where, \"index %%d out of bounds (size: %%d, this: 0x%%08x)\", i, Get%sSize(), this);\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
#else
               buffer.AppendFormatted("Bool_t %s::%sBoundsOk(const char* where, Int_t at)\n",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   return (%s == 0)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      ? %sOutOfBoundsError(where, at)\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      : kTRUE;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("Bool_t %s::%sOutOfBoundsError(const char* where, Int_t i)",clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("{\n");
               buffer.AppendFormatted("   Error(where,\"%s is not allocated. Please allocate with %s::Set%sSize(Int_t number)\");\n",valueName[iFold][i].Data(),clsName.Data(),valueName[iFold][i].Data());
               buffer.AppendFormatted("   return kFALSE;\n");
               buffer.AppendFormatted("}\n");
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

         buffer.AppendFormatted("\n\n#include \"include/folders/%s%s.h\"\n",shortCut.Data(),folderName[iFold].Data());
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
      buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4244 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"TObject.h\"\n");
      buffer.AppendFormatted("#include \"TClass.h\"\n");
      buffer.AppendFormatted("#include \"TClonesArray.h\"\n");
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
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueType[iFold][i]=="TString" || valueType[iFold][i]=="ROMEString") {
            buffer.AppendFormatted("#include \"%s.h\"\n",valueType[iFold][i].Data());
            break;
         }
         if (isTArrayType(valueType[iFold][i])) {
            str = valueType[iFold][i](0,7);
            buffer.AppendFormatted("#include \"%s.h\"\n",str.Data());
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
                  buffer.AppendFormatted("#include \"include/folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               else
                  buffer.AppendFormatted("#include \"include/generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
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
            buffer.AppendFormatted(format.Data(),"",valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
         }
         else if (valueArray[iFold][i][0]=="variable") {
            format.SetFormatted("   %%-%ds* %%s;%%%ds %%s\n",typeLen-1,nameLen-valueName[iFold][i].Length());
            buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueComment[iFold][i].Data());
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
            buffer.AppendFormatted(format.Data(),"",valueComment[iFold][i].Data());
         }
      }
      format.SetFormatted("   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-9);
      buffer.AppendFormatted(format.Data(),"Bool_t","Modified","","//! Modified Folder Flag");
      buffer.AppendFormatted("\n");

      // Methods

      buffer.AppendFormatted("public:\n");
      // Constructor
      if (folderUserCode[iFold])
         buffer.AppendFormatted("   %s%s_Base( ",shortCut.Data(),folderName[iFold].Data());
      else
         buffer.AppendFormatted("   %s%s( ",shortCut.Data(),folderName[iFold].Data());
      for (i=0;i<numOfValue[iFold];i++) {
         if (valueDimension[iFold][i]==0) {
            if (isFolder(valueType[iFold][i].Data()))
               continue;
            if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString") && !isTArrayType(valueType[iFold][i]))
               continue;
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
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"TClonesArray*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%s->GetEntries();%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (isTArrayType(valueType[iFold][i])) {
               // TArray itself checks bounds.
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index)%%%ds { return %%s%%sAt(index);%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),relation.Data(),"");
               if (valueType[iFold][i].Contains("*")) {
                  format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
                  buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               }
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%s%%sGetSize();%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"Int_t",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),relation.Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* arrayToCopy);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds* Get%%sAt();\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                     && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
            else if (valueArray[iFold][i][0]=="variable") {
               format.SetFormatted("   %%-%ds  Get%%sAt(Int_t index);\n",typeLen);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sSize()%%%ds { return %%sSize;%%%ds };\n",typeLen,lb,lb);
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
               format.SetFormatted(";%%%ds };\n",lb);
               buffer.AppendFormatted(format.Data(),"");
               format.SetFormatted("   %%-%ds  Get%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),valueType[iFold][i].Data());
            }
         }
         else {
            if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
               && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds* Get%%s()%%%ds { return &%%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return &%%s;%%%ds };\n",typeLen,lb,lb);
               buffer.AppendFormatted(format.Data(),"TRef*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   %%-%ds  Get%%s()%%%ds { return %%s;%%%ds };\n",typeLen,lb,lb);
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
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds = %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
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
               format.SetFormatted("   void Set%%sAt%%%ds(Int_t index,%%-%ds %%s_value%%%ds) { %%s%%sAddAt(%%s_value,index)%%%ds;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",TArray2StandardType(valueType[iFold][i],tempBuffer),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),relation.Data(),valueName[iFold][i].Data(),"","");               buffer.AppendFormatted("   void Set%sSize(Int_t number) {\n",valueName[iFold][i].Data());
               buffer.AppendFormatted("      %s%sSet(number);\n",valueName[iFold][i].Data(),relation.Data());
               buffer.AppendFormatted("      SetModified(true);\n");
               buffer.AppendFormatted("   }\n");
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array);\n",typeLen);
               buffer.AppendFormatted(format.Data(),"void",valueName[iFold][i].Data(),TArray2StandardType(valueType[iFold][i],tempBuffer));
            }
            else if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
               continue;
            else if (isRootClassType(valueType[iFold][i].Data()) && isPointerType(valueType[iFold][i].Data())
                    && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString")) {
               format.SetFormatted("   void Set%%sAt%%%ds(Int_t index,%%-%ds %%s_value);\n",lb,typeLen);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
            else if (valueArray[iFold][i][0]=="variable") {
               format.SetFormatted("   void Set%%sAt%%%ds(Int_t index,%%-%ds %%s_value);\n",lb,typeLen);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data());
               format.SetFormatted("   void Set%%s%%%ds(%%-%ds* %%s_value%%%ds) { %%s = %%s_value%%%ds;%%%ds SetModified(true); };\n",lb,typeLen-1,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"",valueType[iFold][i].Data(),valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),valueName[iFold][i].Data(),"","");
               buffer.AppendFormatted("   void Set%sSize(Int_t number);\n",valueName[iFold][i].Data());
               format.SetFormatted("   %%-%ds  Set%%sCopy(Int_t n,%%s* array);\n",typeLen,lb);
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
         if (isRootClassType(valueType[iFold][i].Data())
            && !valueType[iFold][i].Contains("TRef") && !valueType[iFold][i].Contains("TString"))
            continue;
         int lb = nameLen-valueName[iFold][i].Length();
         if (valueDimension[iFold][i]==0) {
            if (valueType[iFold][i]=="TRef") {
               format.SetFormatted("   void Add%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds += %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
               buffer.AppendFormatted(format.Data(),valueName[iFold][i].Data(),"","TObject*",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"",valueName[iFold][i].Data(),"");
            }
            else {
               if (!isBoolType(valueType[iFold][i].Data())) {
                  format.SetFormatted("   void Add%%s%%%ds(%%-%ds %%s_value%%%ds) { %%s%%%ds += %%s_value;%%%ds SetModified(true); };\n",lb,typeLen,lb,lb,lb);
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
               format.SetFormatted("%%-%ds %%s_value);\n",typeLen,lb);
               buffer.AppendFormatted(format.Data(),valueType[iFold][i].Data(),valueName[iFold][i].Data());
            }
         }
      }
      buffer.AppendFormatted("\n");

      // SetModified
      int lb = nameLen-8;
      format.SetFormatted("   void Set%%s%%%ds(%%-%ds %%s%%%ds) { f%%s%%%ds = %%s;%%%ds};\n",lb,typeLen,lb,lb,lb);
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
         if (isRootClassType(valueType[iFold][i].Data()) && !isPointerType(valueType[iFold][i].Data())
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
      if (folderUserCode[iFold])
         buffer.AppendFormatted("\n   ClassDef(%s%s_Base,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
      else
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
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

         buffer.AppendFormatted("#include \"include/generated/%s%s_Base.h\"\n",shortCut.Data(),folderName[iFold].Data());

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
         buffer.AppendFormatted("\n   ClassDef(%s%s,%s)\n",shortCut.Data(),folderName[iFold].Data(),folderVersion[iFold].Data());
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
            buffer.AppendFormatted("#include \"include/folders/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
         else
            buffer.AppendFormatted("#include \"include/generated/%s%s.h\"\n",shortCut.Data(),folderName[i].Data());
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
   ROMEString tmp;

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
      clsDescription = taskDescription[iTask].Data();
      clsDescription.AppendFormatted("\n\n");
      if (!taskFortran[iTask]) {
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
                  cout << "Error : Task " << taskName[iTask].Data() << " tries to access folder " << folderName[j].Data() << endl;
                  cout << "        but this folder is currently not linked into the project." << endl << endl;
                  cout << "        Please check the affiliations of the task and the folder," << endl;
                  cout << "        or modify the source code of the task." << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               clsDescription.AppendFormatted("    %s\n",folderName[j].Data());
            }
         }
         clsDescription.AppendFormatted("\n");
         if (numOfHistos[iTask]>0) {
            if (first) {
               clsDescription.AppendFormatted("Please note: The following information is only correct after executing the ROMEBuilder.\n");
               clsDescription.AppendFormatted("              \n");
               clsDescription.AppendFormatted("\n");
               first = false;
            }
            clsDescription.AppendFormatted(" This task contains the following histgrams :\n");
            for (i=0;i<numOfHistos[iTask];i++) {
               clsDescription.AppendFormatted("    %s\n",histoName[iTask][i].Data());
            }
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("The histograms are created and saved automaticaly by the task.\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("The following method can be used to fill a histogram :\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("Fill<Histogram Name>(Double_t value,Double_t weight)\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("For histogram arrays use :\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("Fill<Histogram Name>At(Int_t index,Double_t value,Double_t weight)\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("If more histogram functions are needed use the following function the get\n");
            clsDescription.AppendFormatted(" a handle to the histogram and use the root functions.\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("Get<Histogram Name>()\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("For histogram arrays use :\n");
            clsDescription.AppendFormatted("\n");
            clsDescription.AppendFormatted("Get<Histogram Name>At(Int_t index)\n");
         }
         bool folderIncludeFirst = true;
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
                  if (folderUserCode[j])
                     clsDescription.AppendFormatted("#include \"include/folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
                  else
                     clsDescription.AppendFormatted("#include \"include/generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               }
            }
         }
      }

      // fortran task
      if (taskFortran[iTask]) {
         // Header
         if (taskUserCode[iTask])
            buffer.AppendFormatted("\n\n#include \"include/tasks/%sT%s.h\"\n",shortCut.Data(),taskName[iTask].Data());
         else
            buffer.AppendFormatted("\n\n#include \"include/generated/%sT%s.h\"\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#include \"Riostream.h\"\n");

         buffer.AppendFormatted("\nClassImp(%sT%s)\n\n",shortCut.Data(),taskName[iTask].Data());

         // User Functions
#if defined( R__VISUAL_CPLUSPLUS )
         buffer.AppendFormatted("extern \"C\" void %sT%s_INIT();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("void %sT%s::Init()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_INIT();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_BEGINOFRUN();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_BEGINOFRUN();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_EVENT();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("void %sT%s::Event()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_EVENT();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_ENDOFRUN();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_ENDOFRUN();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %sT%s_TERMINATE();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("void %sT%s::Terminate()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %sT%s_TERMINATE();\n",shortCut.ToUpper(tmp),taskName[iTask].ToUpper(tmp));
         buffer.AppendFormatted("}\n\n");
#endif
#if defined( R__UNIX )
         ROMEString tmp2;
         buffer.AppendFormatted("extern \"C\" void %st%s_init_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("void %sT%s::Init()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_init_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_beginofrun_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("void %sT%s::BeginOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_beginofrun_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_event_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("void %sT%s::Event()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_event_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_endofrun_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("void %sT%s::EndOfRun()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_endofrun_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("}\n\n");
         buffer.AppendFormatted("extern \"C\" void %st%s_terminate_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("void %sT%s::Terminate()\n{\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("   %st%s_terminate_();\n",shortCut.ToLower(tmp),taskName[iTask].ToLower(tmp2));
         buffer.AppendFormatted("}\n\n");
         //Write File
         clsDescription += buffer;
         WriteFile(cppFile.Data(),header.Data(),6);
#endif
      }
      else{ // c++ task
         WriteDescription(header, clsName.Data(), clsDescription.Data(), kTRUE);

         buffer.Resize(0);
         // Header
         if (taskUserCode[iTask])
            buffer.AppendFormatted("\n\n#include \"include/tasks/%sT%s.h\"\n",shortCut.Data(),taskName[iTask].Data());
         else
            buffer.AppendFormatted("\n\n#include \"include/generated/%sT%s.h\"\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#include \"Riostream.h\"\n");
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
         if (taskUserCode[iTask]) {
            str1.SetFormatted("include/generated/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
            str2.SetFormatted("include/tasks/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
         }
         else {
            str1.SetFormatted("include/tasks/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
            str2.SetFormatted("include/generated/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
         }
         ReplaceHeader(cppFile.Data(),header.Data(),buffer.Data(),6,str1.Data(),str2.Data());
      }
   }
   return true;
}

Bool_t ROMEBuilder::WriteTaskF()
{
   ROMEString fFile;
   ROMEString buffer;

   if (makeOutput) cout << "\n   Output F-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (!taskUsed[iTask])
         continue;
      if (taskFortran[iTask]) {
         // File name
         fFile.SetFormatted("%ssrc/tasks/%sTF%s.F",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

         ROMEString shortcut(shortCut);
         shortcut.ToLower();
         ROMEString taskname(taskName[iTask]);
         taskname.ToLower();

         if (gSystem->AccessPathName(fFile.Data(),kFileExists)) {
            if (makeOutput) cout << "      " << fFile.Data() << endl;

            // Methods
            buffer.Resize(0);
            buffer.AppendFormatted("      subroutine %st%s_init()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_beginofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_event()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_endofrun()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");
            buffer.AppendFormatted("      subroutine %st%s_terminate()\n\n",shortcut.Data(),taskname.Data());
            buffer.AppendFormatted("      return\n");
            buffer.AppendFormatted("      end\n\n");

            // Write File
            WriteFile(fFile.Data(),buffer.Data(),6);
         }
      }
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
   bool changeableFlagChanged;
   int i,j;

   if (makeOutput) cout << "\n   Output H-Files:" << endl;
   for (int iTask=0;iTask<numOfTask;iTask++) {
      if (!taskUsed[iTask])
         continue;
      changeableFlagChanged = false;

      //back up old files
      if (taskUserCode[iTask]) {
         hFile.SetFormatted("%sinclude/generated/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
         changeableFlagChanged = RemoveFile(hFile.Data());
      }
      else {
         hFile.SetFormatted("%sinclude/generated/%sT%s_Base.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
         changeableFlagChanged = RemoveFile(hFile.Data());
         hFile.SetFormatted("%sinclude/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
         BackUpFile(hFile.Data());
      }
      if (changeableFlagChanged) {
         if (taskUserCode[iTask])
            hFile.SetFormatted("include/generated/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
         else
            hFile.SetFormatted("include/tasks/%sT%s.h",shortCut.Data(),taskName[iTask].Data());
         RemoveDepFiles(hFile.Data());
      }

      // File name
      if (taskUserCode[iTask])
         hFile.SetFormatted("%sinclude/generated/%sT%s_Base.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      else
         hFile.SetFormatted("%sinclude/generated/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());

      // Description
      buffer.Resize(0);
      WriteHeader(buffer, taskAuthor[iTask].Data(), kTRUE);
      if (taskUserCode[iTask]) {
         buffer.AppendFormatted("#ifndef %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_Base_H\n\n",shortCut.Data(),taskName[iTask].Data());
      }
      else {
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());
      }
      clsName.SetFormatted("%sT%s", shortCut.Data(), taskName[iTask].Data());
      clsDescription = taskDescription[iTask].Data();
      WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
      buffer.AppendFormatted("\n\n");

      buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"TH1.h\"\n");
      buffer.AppendFormatted("#include \"TH2.h\"\n");
      buffer.AppendFormatted("#include \"TH3.h\"\n");
      buffer.AppendFormatted("#include \"TProfile.h\"\n");
      buffer.AppendFormatted("#include \"TProfile2D.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"ROMETask.h\"\n");

      for (i=0;i<numOfTaskInclude[iTask];i++) {
         if (taskLocalFlag[iTask][i]) {
            buffer.AppendFormatted("#include \"%s\"\n",taskInclude[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("#include <%s>\n",taskInclude[iTask][i].Data());
         }
      }

      if (readGlobalSteeringParameters)
         buffer.AppendFormatted("#include \"include/generated/%sGlobalSteering.h\"\n",shortCut.Data());
      buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());

      // Class
      if (taskUserCode[iTask])
         buffer.AppendFormatted("\nclass %sT%s_Base : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("\nclass %sT%s : public ROMETask\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("{\n");

      buffer.AppendFormatted("protected:\n");
      // Fields
      if (numOfSteering[iTask]>0) {
         WriteSteeringClass(buffer,0,iTask,1);
         buffer.AppendFormatted("\n");
      }

      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* fSteering; // Handle to Steering class\n\n");
      }

      int nameLen = -1;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (nameLen<(int)histoName[iTask][i].Length()) nameLen = histoName[iTask][i].Length();
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
            format.SetFormatted("   %%s*      f%%s;%%%ds // %%s Handle\n",20+nameLen-histoName[iTask][i].Length()+1);
            buffer.AppendFormatted(format.Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         }
         else {
            format.SetFormatted("   TObjArray* f%%ss;%%%ds // %%s Handle\n",10+nameLen-histoName[iTask][i].Length());
            buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         }
         format.SetFormatted("   ROMEString f%%sTitle;%%%ds // Title of %%s\n",20+nameLen-histoName[iTask][i].Length()-5);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sTitleCode;%%%ds // Interpreter code of Title of %%s\n",20+nameLen-histoName[iTask][i].Length()-9);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sFolderTitle;%%%ds // Title of the Folder of %%s\n",20+nameLen-histoName[iTask][i].Length()-11);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sFolderTitleCode;%%%ds // Interpreter code of Title of the Folder of %%s\n",20+nameLen-histoName[iTask][i].Length()-15);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sArraySize;%%%ds // Array Size of %%s\n",20+nameLen-histoName[iTask][i].Length()-9);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sArraySizeCode;%%%ds // Interpreter code of Array Size of %%s\n",20+nameLen-histoName[iTask][i].Length()-13);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sArrayStartIndex;%%%ds // Array Start Index of %%s\n",20+nameLen-histoName[iTask][i].Length()-15);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sArrayStartIndexCode;%%%ds // Interpreter code of Array Start Index of %%s\n",20+nameLen-histoName[iTask][i].Length()-19);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sXLabel;%%%ds // Label of the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sXLabelCode;%%%ds // Interpreter code of Label of the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sYLabel;%%%ds // Label of the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sYLabelCode;%%%ds // Interpreter code of Label of the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   ROMEString f%%sZLabel;%%%ds // Label of the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sZLabelCode;%%%ds // Interpreter code of Label of the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sXNbins;%%%ds // Number of bins on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sXNbinsCode;%%%ds // Interpreter code of Number of bins on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Double_t   f%%sXmin;%%%ds // Minimum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sXminCode;%%%ds // Interpreter code of Minimum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Double_t   f%%sXmax;%%%ds // Maximum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sXmaxCode;%%%ds // Interpreter code of Minimum value on the X axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sYNbins;%%%ds // Number of bins on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sYNbinsCode;%%%ds // Interpreter code of Number of bins on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Double_t   f%%sYmin;%%%ds // Minimum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sYminCode;%%%ds // Interpreter code of Minimum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Double_t   f%%sYmax;%%%ds // Maximum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sYmaxCode;%%%ds // Interpreter code of Maximum value on the Y axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sZNbins;%%%ds // Number of bins on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-6);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sZNbinsCode;%%%ds // Interpreter code of Number of bins on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-10);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Double_t   f%%sZmin;%%%ds // Minimum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sZminCode;%%%ds // Interpreter code of Minimum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Double_t   f%%sZmax;%%%ds // Maximum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-4);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Int_t      f%%sZmaxCode;%%%ds // Interpreter code of Maximum value on the Z axis of %%s\n",20+nameLen-histoName[iTask][i].Length()-8);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
         format.SetFormatted("   Bool_t     f%%sAccumulation;%%%ds // Accumulation Flag for the %%s\n",20+nameLen-histoName[iTask][i].Length()-12);
         buffer.AppendFormatted(format.Data(),histoName[iTask][i].Data(),"",histoName[iTask][i].Data());
      }
      buffer.AppendFormatted("\n   ROMEString fHistoSuffix; //!\n");
      buffer.AppendFormatted("\n   char* cstop; //!\n\n");


      // Methods
      buffer.AppendFormatted("public:\n");
      // Constructor and Event Methods
      buffer.AppendFormatted("   // Constructor\n");
      if (taskUserCode[iTask])
         buffer.AppendFormatted("   %sT%s_Base(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder);\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("   %sT%s(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder);\n",shortCut.Data(),taskName[iTask].Data());
      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      // Access Methods
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   void Fill%s(double x,double weight=1) { f%s->Fill(x,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   void Fill%s(double x,double y,double weight=1) { f%s->Fill(x,y,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   void Fill%s(double x,double y,double z,double weight=1) { f%s->Fill(x,y,z,weight); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            }
            buffer.AppendFormatted("   void Draw%s() { f%s->Draw(); };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   %s* Get%s() { return f%s; };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   void Fill%sAt(Int_t index,Double_t x,Double_t weight=1) { ((%s*)f%ss->At(index))->Fill(x,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   void Fill%sAt(Int_t index,Double_t x,Double_t y,Double_t weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            else if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   void Fill%sAt(Int_t index,Double_t x,Double_t y,Double_t z,Double_t weight=1) { ((%s*)f%ss->At(index))->Fill(x,y,z,weight); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            }
            buffer.AppendFormatted("   void Draw%sAt(Int_t index) { ((%s*)f%ss->At(index))->Draw(); };\n",histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   %s* Get%sAt(Int_t index) { return (%s*)f%ss->At(index); };\n",histoType[iTask][i].Data(),histoName[iTask][i].Data(),histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   TObjArray* Get%s() { return f%ss; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         buffer.AppendFormatted("   void Set%sTitle(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sFolderTitle(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sArraySize(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sArrayStartIndex(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXLabel(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYLabel(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZLabel(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXNbins(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXmin(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sXmax(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYNbins(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYmin(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sYmax(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZNbins(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZmin(const char* value);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sZmax(const char* value);\n",histoName[iTask][i].Data());

         buffer.AppendFormatted("   const char* Get%sTitle() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sTitle==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return \"%s\";\n",histoTitle[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sTitle;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   const char* Get%sFolderTitle() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sFolderTitle==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return \"%s\";\n",histoFolderTitle[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sFolderTitle;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   const char* Get%sXLabel() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sXLabel==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return \"%s\";\n",histoXLabel[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sXLabel;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   const char* Get%sYLabel() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sYLabel==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return \"%s\";\n",histoYLabel[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sYLabel;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   const char* Get%sZLabel() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sZLabel==\"\")\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return \"%s\";\n",histoZLabel[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sZLabel;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   int Get%sArraySize() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sArraySize==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoArraySize[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sArraySize;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   int Get%sArrayStartIndex() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sArrayStartIndex==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoArrayStartIndex[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sArrayStartIndex;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   int Get%sXNbins() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sXNbins==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoXNbins[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sXNbins;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   double Get%sXmin() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sXmin==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoXmin[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sXmin;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   double Get%sXmax() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sXmax==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoXmax[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sXmax;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   int Get%sYNbins() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sYNbins==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoYNbins[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sYNbins;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   double Get%sYmin() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sYmin==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoYmin[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sYmin;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   double Get%sYmax() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sYmax==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoYmax[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sYmax;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   int Get%sZNbins() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sZNbins==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoZNbins[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sZNbins;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   double Get%sZmin() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sZmin==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoZmin[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sZmin;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");
         buffer.AppendFormatted("   double Get%sZmax() {\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      if (f%sZmax==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("         return %s;\n",histoZmax[iTask][i].Data());
         buffer.AppendFormatted("      else\n");
         buffer.AppendFormatted("         return f%sZmax;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   };\n");

         buffer.AppendFormatted("   Bool_t is%sAccumulation()                { return f%sAccumulation; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   void Set%sAccumulation(Bool_t flag)      { f%sAccumulation = flag; };\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
      }


      // Object Interpreter
      buffer.AppendFormatted("   Int_t GetObjectInterpreterCode(const char* objectPath);\n");
      buffer.AppendFormatted("   Int_t GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue);\n");
      buffer.AppendFormatted("   Double_t GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue);\n");
      buffer.AppendFormatted("   ROMEString& GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer);\n");
      // Protected
      buffer.AppendFormatted("protected:\n");
      buffer.AppendFormatted("   // Event Methods\n");
      if (taskUserCode[iTask]) {
         buffer.AppendFormatted("   virtual void Init() = 0;\n");
         buffer.AppendFormatted("   virtual void BeginOfRun() = 0;\n");
         buffer.AppendFormatted("   virtual void Event() = 0;\n");
         buffer.AppendFormatted("   virtual void EndOfRun() = 0;\n");
         buffer.AppendFormatted("   virtual void Terminate() = 0;\n\n");
      }
      else {
         buffer.AppendFormatted("   virtual void Init();\n");
         buffer.AppendFormatted("   virtual void BeginOfRun();\n");
         buffer.AppendFormatted("   virtual void Event();\n");
         buffer.AppendFormatted("   virtual void EndOfRun();\n");
         buffer.AppendFormatted("   virtual void Terminate();\n\n");
      }
      // Histo Methods
      buffer.AppendFormatted("   // Histo Methods\n");
      buffer.AppendFormatted("   virtual void BookHisto();\n");
      buffer.AppendFormatted("   virtual void ResetHisto();\n\n");
      // Footer
      if (taskUserCode[iTask])
         buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
      else
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
      buffer.AppendFormatted("};\n\n");

      // Histo Inline Methods
      // Constructor
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline %sT%s_Base::%sT%s_Base(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder):ROMETask(name,title,level) {\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline %sT%s::%sT%s(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder):ROMETask(name,title,level) {\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   fHistoSuffix = histoSuffix;\n");
      buffer.AppendFormatted("   fHistoFolder = histoFolder;\n");
      buffer.AppendFormatted("   fEventID = %s;\n",taskEventID[iTask].Data());
      buffer.AppendFormatted("   fVersion = %s;\n",taskVersion[iTask].Data());
      if (numOfHistos[iTask]>0)
         buffer.AppendFormatted("   fHasHistograms = true;\n");
      else
         buffer.AppendFormatted("   fHasHistograms = false;\n");
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormatted("   f%sTitle = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sTitleCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sFolderTitle = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sFolderTitleCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXLabel = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXLabelCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYLabel = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYLabelCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZLabel = \"\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZLabelCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArraySize = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArraySizeCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArrayStartIndex = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sArrayStartIndexCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXNbins = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXNbinsCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXmin = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXminCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXmax = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sXmaxCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYNbins = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYNbinsCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYmin = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYminCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYmax = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sYmaxCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZNbins = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZNbinsCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZmin = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZminCode = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZmax = -1;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   f%sZmaxCode = -1;\n",histoName[iTask][i].Data());
      }
      if (numOfSteering[iTask]>0) {
         buffer.AppendFormatted("   fSteering = new Steering();\n");
      }
      buffer.AppendFormatted("};\n");

      // Book
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline void %sT%s_Base::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline void %sT%s::BookHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      bool array = false;
      for (i=0;i<numOfHistos[iTask];i++) {
         if (histoArraySize[iTask][i]!="1") array = true;
      }
      if (array) {
         buffer.AppendFormatted("   int j;\n");
         buffer.AppendFormatted("   ROMEString name;\n");
         buffer.AppendFormatted("   ROMEString title;\n");
      }
      if (numOfHistos[iTask]>0) {
         buffer.AppendFormatted("   ROMEString histoName;\n");
         buffer.AppendFormatted("   ROMEString histoTitle;\n");
         buffer.AppendFormatted("   ROMEString folderTitle;\n");
         buffer.AppendFormatted("   ROMEString xLabel;\n");
         buffer.AppendFormatted("   ROMEString yLabel;\n");
         buffer.AppendFormatted("   ROMEString zLabel;\n");
         buffer.AppendFormatted("   int arraySize;\n");
         buffer.AppendFormatted("   int arrayStartIndex;\n");
         buffer.AppendFormatted("   int xNbins;\n");
         buffer.AppendFormatted("   double xmin;\n");
         buffer.AppendFormatted("   double xmax;\n");
         buffer.AppendFormatted("   int yNbins;\n");
         buffer.AppendFormatted("   double ymin;\n");
         buffer.AppendFormatted("   double ymax;\n");
         buffer.AppendFormatted("   int zNbins;\n");
         buffer.AppendFormatted("   double zmin;\n");
         buffer.AppendFormatted("   double zmax;\n");
      }
      for (i=0;i<numOfHistos[iTask];i++) {
         bool sameFolder = false;
         bool homeFolder = false;
         if (histoFolderName[iTask][i]=="") {
            homeFolder = true;
         }
         else {
            for (j=0;j<i;j++) {
               if (histoFolderName[iTask][i]==histoFolderName[iTask][j])
                  sameFolder = true;
            }
         }

         buffer.AppendFormatted("   histoTitle = Get%sTitle();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   folderTitle = Get%sFolderTitle();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xLabel = Get%sXLabel();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   yLabel = Get%sYLabel();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zLabel = Get%sZLabel();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   arraySize = Get%sArraySize();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   arrayStartIndex = Get%sArrayStartIndex();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xNbins = Get%sXNbins();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xmin = Get%sXmin();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xmax = Get%sXmax();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   yNbins = Get%sYNbins();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   ymin = Get%sYmin();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   ymax = Get%sYmax();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zNbins = Get%sZNbins();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zmin = Get%sZmin();\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zmax  = Get%sZmax();\n",histoName[iTask][i].Data());

         buffer.AppendFormatted("   histoTitle = GetObjectInterpreterCharValue(f%sTitleCode,histoTitle,histoTitle);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   folderTitle = GetObjectInterpreterCharValue(f%sFolderTitleCode,folderTitle,folderTitle);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xLabel = GetObjectInterpreterCharValue(f%sXLabelCode,xLabel,xLabel);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   yLabel = GetObjectInterpreterCharValue(f%sYLabelCode,yLabel,yLabel);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zLabel = GetObjectInterpreterCharValue(f%sZLabelCode,zLabel,zLabel);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   arraySize = GetObjectInterpreterIntValue(f%sArraySizeCode,arraySize);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   arrayStartIndex = GetObjectInterpreterIntValue(f%sArrayStartIndexCode,arrayStartIndex);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xNbins = GetObjectInterpreterIntValue(f%sXNbinsCode,xNbins);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xmin = GetObjectInterpreterDoubleValue(f%sXminCode,xmin );\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   xmax = GetObjectInterpreterDoubleValue(f%sXmaxCode,xmax );\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   yNbins = GetObjectInterpreterIntValue(f%sYNbinsCode,yNbins);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   ymin = GetObjectInterpreterDoubleValue(f%sYminCode,ymin );\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   ymax = GetObjectInterpreterDoubleValue(f%sYmaxCode,ymax);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zNbins = GetObjectInterpreterIntValue(f%sZNbinsCode,zNbins);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zmin = GetObjectInterpreterDoubleValue(f%sZminCode,zmin);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   zmax = GetObjectInterpreterDoubleValue(f%sZmaxCode,zmax);\n",histoName[iTask][i].Data());

         // create histo folder
         if (!sameFolder&&!homeFolder) {
            buffer.AppendFormatted("   TFolder *%sFolder = GetHistoFolder()->AddFolder(\"%s\",folderTitle.Data());\n",histoFolderName[iTask][i].Data(),histoFolderName[iTask][i].Data());
         }
         // create histos
         buffer.AppendFormatted("   histoName = \"%s\";\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("   histoName+=fHistoSuffix;\n");
         if (histoArraySize[iTask][i]=="1") {
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
            }
            if (histoType[iTask][i]=="TProfile") {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,ymin,ymax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
            }
            if (histoType[iTask][i]=="TProfile2D") {
               buffer.AppendFormatted("   f%s = new %s(histoName.Data(),histoTitle.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zmin,zmax);\n",histoName[iTask][i].Data(),histoType[iTask][i].Data());
            }
            if (!homeFolder)
               buffer.AppendFormatted("   %sFolder->Add(f%s);\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("   GetHistoFolder()->Add(f%s);\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetXaxis()->SetTitle(xLabel.Data());\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetYaxis()->SetTitle(yLabel.Data());\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   f%s->GetZaxis()->SetTitle(zLabel.Data());\n",histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   %s *hist%d;\n",histoType[iTask][i].Data(),i);
            buffer.AppendFormatted("   f%ss = new TObjArray(arraySize);\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("   for (j=0;j<arraySize;j++) {\n");
            buffer.AppendFormatted("      name.SetFormatted(\"_%%0*d\",3,j+arrayStartIndex);\n");
            buffer.AppendFormatted("      name.Insert(0,histoName.Data());\n");
            buffer.AppendFormatted("      title.SetFormatted(\" %%0*d\",3,j+arrayStartIndex);\n");
            buffer.AppendFormatted("      title.Insert(0,histoTitle.Data());\n");
            if (histoType[iTask][i][2]==49) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax);\n",i,histoType[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==50) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax);\n",i,histoType[iTask][i].Data());
            }
            if (histoType[iTask][i][2]==51) {
               buffer.AppendFormatted("      hist%d = new %s(name.Data(),title.Data(),xNbins,xmin,xmax,yNbins,ymin,ymax,zNbins,zmin,zmax);\n",i,histoType[iTask][i].Data());
            }
            buffer.AppendFormatted("      f%ss->Add(hist%d);\n",histoName[iTask][i].Data(),i);
            if (!homeFolder)
               buffer.AppendFormatted("      %sFolder->Add(f%ss->At(j));\n",histoFolderName[iTask][i].Data(),histoName[iTask][i].Data());
            else
               buffer.AppendFormatted("      GetHistoFolder()->Add(f%ss->At(j));\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("      hist%d->GetXaxis()->SetTitle(xLabel.Data());\n",i);
            buffer.AppendFormatted("      hist%d->GetYaxis()->SetTitle(yLabel.Data());\n",i);
            buffer.AppendFormatted("      hist%d->GetZaxis()->SetTitle(zLabel.Data());\n",i);
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      // Reset
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline void %sT%s_Base::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline void %sT%s::ResetHisto() {\n",shortCut.Data(),taskName[iTask].Data());
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
         if (histoArraySize[iTask][i]=="1") {
            buffer.AppendFormatted("   if (!is%sAccumulation()) f%s->Reset();\n",histoName[iTask][i].Data(),histoName[iTask][i].Data());
         }
         else {
            buffer.AppendFormatted("   if (!is%sAccumulation()) {\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("       nentry = f%ss->GetEntries();\n",histoName[iTask][i].Data());
            buffer.AppendFormatted("       for (j=0;j<nentry;j++) ((%s*)f%ss->At(j))->Reset();\n",histoType[iTask][i].Data(),histoName[iTask][i].Data());
            buffer.AppendFormatted("   }\n");
         }
      }
      buffer.AppendFormatted("}\n\n");

      ROMEString fctHead;
      if (taskUserCode[iTask])
         fctHead.SetFormatted("inline void %sT%s_Base::",shortCut.Data(),taskName[iTask].Data());
      else
         fctHead.SetFormatted("inline void %sT%s::",shortCut.Data(),taskName[iTask].Data());
      // Parameter Setters
      for (i=0;i<numOfHistos[iTask];i++) {
         buffer.AppendFormatted("%sSet%sTitle(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sTitleCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sTitle=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sFolderTitle(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sFolderTitleCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sFolderTitle=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sArraySize(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sArraySizeCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sArraySize=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sArrayStartIndex(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sArrayStartIndexCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sArrayStartIndex=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXLabel(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXLabelCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXLabel=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYLabel(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYLabelCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYLabel=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZLabel(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZLabelCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZLabel=value;\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXNbins(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXNbinsCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXNbins=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXmin(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXminCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXmin=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sXmax(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sXmaxCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sXmax=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYNbins(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYNbinsCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYNbins=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYmin(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYminCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYmin=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sYmax(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sYmaxCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sYmax=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZNbins(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZNbinsCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZNbins=strtol(value,&cstop,10);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZmin(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZminCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZmin=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("%sSet%sZmax(const char* value) {\n",fctHead.Data(),histoName[iTask][i].Data());
         buffer.AppendFormatted("   if ((f%sZmaxCode=GetObjectInterpreterCode(value))==-1)\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("      f%sZmax=strtod(value,&cstop);\n",histoName[iTask][i].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }


      // Get Object Interpreter Code
      int codeNumber = fNumberOfInterpreterCodes;
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline Int_t %sT%s_Base::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline Int_t %sT%s::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   ROMEString path = objectPath;\n");
      buffer.AppendFormatted("   if (path.Index(\"/\")==-1 && path.Index(\"gAnalyzer->\")==-1)\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   if (path.Index(\"/SP\")==0 || path.Index(\"gAnalyzer->GetSP()\")==0) {\n");
      ROMEString path1 = "/SP";
      ROMEString path2 = "gAnalyzer->GetSP()";
      WriteSteeringInterpreterCode(buffer,codeNumber,0,iTask,path1,path2,1);
      buffer.AppendFormatted("      gAnalyzer->PrintLine(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
      buffer.AppendFormatted("      gAnalyzer->PrintLine(\"   \"+path+\"\\n\");\n");
      buffer.AppendFormatted("      return -1;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return gAnalyzer->GetObjectInterpreterCode(objectPath);\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Get Object Interpreter Int Value
      codeNumber = fNumberOfInterpreterCodes;
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline Int_t %sT%s_Base::GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline Int_t %sT%s::GetObjectInterpreterIntValue(Int_t code,Int_t defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   char *cstop=NULL;\n");
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
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline Double_t %sT%s_Base::GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline Double_t %sT%s::GetObjectInterpreterDoubleValue(Int_t code,Double_t defaultValue) {\n",shortCut.Data(),taskName[iTask].Data());
      buffer.AppendFormatted("   char *cstop=NULL;\n");
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
      if (taskUserCode[iTask])
         buffer.AppendFormatted("inline ROMEString& %sT%s_Base::GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer) {\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("inline ROMEString& %sT%s::GetObjectInterpreterCharValue(Int_t code,ROMEString& defaultValue,ROMEString& buffer) {\n",shortCut.Data(),taskName[iTask].Data());
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


      if (taskUserCode[iTask])
         buffer.AppendFormatted("#endif   // %sT%s_Base_H\n",shortCut.Data(),taskName[iTask].Data());
      else
         buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

      // Write File
      WriteFile(hFile.Data(),buffer.Data(),6);

      // User H-File
      hFile.SetFormatted("%sinclude/tasks/%sT%s.h",outDir.Data(),shortCut.Data(),taskName[iTask].Data());
      if (taskUserCode[iTask] && (changeableFlagChanged || gSystem->AccessPathName(hFile.Data(),kFileExists))) {
         // Description
         buffer.Resize(0);
         WriteHeader(buffer, taskAuthor[iTask].Data(), kFALSE);
         buffer.AppendFormatted("#ifndef %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("#define %sT%s_H\n\n",shortCut.Data(),taskName[iTask].Data());
         clsName.SetFormatted("%sT%s", shortCut.Data(), taskName[iTask].Data());
         WriteDescription(buffer, clsName.Data(), 0, kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"include/generated/%sT%s_Base.h\"\n",shortCut.Data(),taskName[iTask].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   %sT%s(const char *name,const char *title,int level,const char *histoSuffix,TFolder *histoFolder):%sT%s_Base(name,title,level,histoSuffix,histoFolder) {};\n",shortCut.Data(),taskName[iTask].Data(),shortCut.Data(),taskName[iTask].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("   // Event Methods\n");
         buffer.AppendFormatted("   virtual void Init();\n");
         buffer.AppendFormatted("   virtual void BeginOfRun();\n");
         buffer.AppendFormatted("   virtual void Event();\n");
         buffer.AppendFormatted("   virtual void EndOfRun();\n");
         buffer.AppendFormatted("   virtual void Terminate();\n\n");
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n",shortCut.Data(),taskName[iTask].Data(),taskVersion[iTask].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sT%s_H\n",shortCut.Data(),taskName[iTask].Data());

         // Write File
         WriteFile(hFile.Data(),buffer.Data(),6);
      }
  }
   return true;
}

Bool_t ROMEBuilder::WriteTabCpp()
{
   ROMEString cppFile;
   ROMEString header;
   ROMEString buffer;
   Int_t i, j;
   ROMEString format;
   ROMEString clsDescription;
   ROMEString clsName;
   ROMEString fileBuffer;
   ROMEString str;

   if (makeOutput)
      cout << "\n   Output Cpp-Files:" << endl;
   for (Int_t iTab = 0; iTab < numOfTab; iTab++) {
      if (!tabUsed[iTab])
         continue;
      header.Resize(0);
      buffer.Resize(0);
      // File name
      cppFile.SetFormatted("%ssrc/tabs/%sT%s.cpp", outDir.Data(), shortCut.Data(), tabName[iTab].Data());

      // Description
      WriteHeader(header, tabAuthor[iTab].Data(), kFALSE);
      clsName.SetFormatted("%sT%s", shortCut.Data() ,tabName[iTab].Data());
      clsDescription = tabDescription[iTab].Data();
      fstream *fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      // Thread
      if (numOfThreadFunctions[iTab] > 0) {
         clsDescription.AppendFormatted("\n\n");
         clsDescription.AppendFormatted("This tab has following thread functions.\n");
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            clsDescription.AppendFormatted("   %s\n", threadFunctionName[iTab][i].Data());
         }
      }
      bool folderIncludeFirst = true;
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
               if (folderUserCode[j])
                  clsDescription.AppendFormatted("#include \"include/folders/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
               else
                  clsDescription.AppendFormatted("#include \"include/generated/%s%s.h\"\n",shortCut.Data(),folderName[j].Data());
            }
         }
      }
      WriteDescription(header, clsName.Data(), clsDescription.Data(), kTRUE);

      buffer.Resize(0);

      buffer.AppendFormatted("\n");
      // Header
      buffer.AppendFormatted("#include \"include/generated/%sWindow.h\"\n", shortCut.Data());
      buffer.AppendFormatted("#include \"include/tabs/%sT%s.h\"\n", shortCut.Data(), tabName[iTab].Data());

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

      buffer.AppendFormatted("void %sT%s::MenuClicked(TGPopupMenu *menu,Long_t param)\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      buffer.AppendFormatted("void %sT%s::TabSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");

      buffer.AppendFormatted("void %sT%s::TabUnSelected()\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");
      buffer.AppendFormatted("}\n");
      buffer.AppendFormatted("\n");

      // Thread
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("void %sT%s::%s()\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   gSystem->Sleep(10000);\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }

      // Write file
      ReplaceHeader(cppFile.Data(), header.Data(), buffer.Data(), 6);
   }
   return kTRUE;
}

Bool_t ROMEBuilder::WriteTabH()
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString format;
   ROMEString clsName;
   ROMEString clsDescription;
   ROMEString str;
   ROMEString str1;
   ROMEString str2;

   Int_t i, j,k;
   if (makeOutput)
      cout << "\n   Output H-Files:" << endl;

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
      buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( push )\n");
      buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"TGMenu.h\"\n");
      buffer.AppendFormatted("#include \"TThread.h\"\n");
      buffer.AppendFormatted("#include \"TRootEmbeddedCanvas.h\"\n");
      buffer.AppendFormatted("#include \"TCanvas.h\"\n");
      buffer.AppendFormatted("#include \"TPad.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
      buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
      buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
      if (readGlobalSteeringParameters)
         buffer.AppendFormatted("#include \"include/generated/%sGlobalSteering.h\"\n",shortCut.Data());
      if (tabHeredity[iTab].Length()>0)
         buffer.AppendFormatted("#include \"include/tabs/%sT%s.h\"\n",shortCut.Data(),tabHeredity[iTab].Data());
      if (tabHistoDisplay[iTab])
         buffer.AppendFormatted("#include \"ArgusHistoDisplay.h\"\n");
      else
         buffer.AppendFormatted("#include \"ArgusTab.h\"\n");
      buffer.AppendFormatted("#include \"Riostream.h\"\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("struct %sArgs{\n", tabName[iTab].Data());
      buffer.AppendFormatted("   void*  inst;\n");
      buffer.AppendFormatted("   Long_t msg;\n");
      buffer.AppendFormatted("   Long_t param1;\n");
      buffer.AppendFormatted("   Long_t param2;\n");
      buffer.AppendFormatted("};\n");

      // Class
      if (tabHistoDisplay[iTab])
         buffer.AppendFormatted("\nclass %sT%s_Base : public ArgusHistoDisplay\n", shortCut.Data(), tabName[iTab].Data());
      else if (tabHeredity[iTab].Length()>0)
         buffer.AppendFormatted("\nclass %sT%s_Base : public %sT%s\n", shortCut.Data(), tabName[iTab].Data(),shortCut.Data(),tabHeredity[iTab].Data());
      else
         buffer.AppendFormatted("\nclass %sT%s_Base : public ArgusTab\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("{\n");

      if (tabHistoDisplay[iTab]) {
         buffer.AppendFormatted("public:\n");
         buffer.AppendFormatted("   enum MenuEnumeration {\n");
         buffer.AppendFormatted("      M_DISPLAY_ROOT = 500,\n");
         for (i=0;i<numOfTabObjects[iTab];i++)
            buffer.AppendFormatted("      M_DISPLAY_%s,\n",tabObjectName[iTab][i].ToUpper(str));
         buffer = buffer(0,buffer.Length()-2);
         buffer.AppendFormatted("   \n};\n");
      }
      buffer.AppendFormatted("protected:\n");

      // Fields
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
      if (numOfTabHistos[iTab]>0) {
         buffer.AppendFormatted("   TRootEmbeddedCanvas *fGeneratedCanvas; //!\n");
         for (i=0;i<numOfTabHistos[iTab];i++) {
            if (histoArraySize[tabHistoTaskIndex[iTab][i]][tabHistoHistoIndex[iTab][i]]=="1") {
               buffer.AppendFormatted("   %s* f%sHisto%d; //!\n",histoType[tabHistoTaskIndex[iTab][i]][tabHistoHistoIndex[iTab][i]].Data(),tabHistoName[iTab][i].Data(),i);
               buffer.AppendFormatted("   TPad* f%sPad%d; //!\n",tabHistoName[iTab][i].Data(),i);
            }
            else {
               for (j=tabHistoArrayIndexStart[iTab][i];j<=tabHistoArrayIndexEnd[iTab][i];j++) {
                  buffer.AppendFormatted("   %s* f%sHisto%d_%d; //!\n",histoType[tabHistoTaskIndex[iTab][i]][tabHistoHistoIndex[iTab][i]].Data(),tabHistoName[iTab][i].Data(),i,j);
                  buffer.AppendFormatted("   TPad* f%sPad%d_%d; //!\n",tabHistoName[iTab][i].Data(),i,j);
               }
            }
         }
      }

      // Methods
      buffer.AppendFormatted("public:\n");

      // Constructor
      buffer.AppendFormatted("   // Constructor\n");
      if (tabHistoDisplay[iTab])
         buffer.AppendFormatted("   %sT%s_Base():ArgusHistoDisplay() {\n", shortCut.Data(), tabName[iTab].Data());
      else if (tabHeredity[iTab].Length()>0)
         buffer.AppendFormatted("   %sT%s_Base():%sT%s() {\n", shortCut.Data(), tabName[iTab].Data(),shortCut.Data(),tabHeredity[iTab].Data());
      else
         buffer.AppendFormatted("   %sT%s_Base():ArgusTab() {\n", shortCut.Data(), tabName[iTab].Data());
      buffer.AppendFormatted("      fVersion = %s;\n", tabVersion[iTab].Data());
      if (tabHistoDisplay[iTab])
         buffer.AppendFormatted("      fInheritanceName = \"%s\";\n", tabName[iTab].Data());
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         buffer.AppendFormatted("      fSteering = new Steering();\n");
      }
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      m%s       = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   virtual ~%sT%s_Base() {\n", shortCut.Data(), tabName[iTab].Data());
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");

      // InitTab
      int nx=1,ny=1;
      buffer.AppendFormatted("   void BaseInit() {\n");
      buffer.AppendFormatted("      int i=0;\n");
      buffer.AppendFormatted("      ROMEString str;\n");
      buffer.AppendFormatted("      Init();\n");
      if (numOfTabHistos[iTab]>0) {
         buffer.AppendFormatted("      // Init Histos\n");
         buffer.AppendFormatted("      fGeneratedCanvas = new TRootEmbeddedCanvas(\"GeneratedCanvas\", this, 600, 600);\n");
         buffer.AppendFormatted("      AddFrame(fGeneratedCanvas,new TGLayoutHints(kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n");
         if (tabHistoIndexMax[iTab]>1)
            ny = 2;
         if (tabHistoIndexMax[iTab]>2)
            nx = 2;
         if (tabHistoIndexMax[iTab]>4)
            ny = 3;
         if (tabHistoIndexMax[iTab]>6)
            nx = 3;
         if (tabHistoIndexMax[iTab]>9)
            ny = 4;
         if (tabHistoIndexMax[iTab]>12)
            nx = 4;
         if (tabHistoIndexMax[iTab]>16)
            ny = 5;
         if (tabHistoIndexMax[iTab]>20)
            nx = 5;
         buffer.AppendFormatted("      fGeneratedCanvas->GetCanvas()->Divide(%d, %d);\n",nx,ny);
         for (i=0;i<tabHistoIndexMax[iTab];i++) {
            for (j=0;j<numOfTabHistos[iTab];j++) {
               if (tabHistoIndex[iTab][j]==i) {
                  buffer.AppendFormatted("      if (gAnalyzer->Is%sActive()) {\n",tabHistoName[iTab][j].Data());
                  if (histoArraySize[tabHistoTaskIndex[iTab][j]][tabHistoHistoIndex[iTab][j]]=="1") {
                     buffer.AppendFormatted("         fGeneratedCanvas->GetCanvas()->cd(%d);\n",i+1);
                     buffer.AppendFormatted("         f%sPad%d = (TPad*)gPad;\n",tabHistoName[iTab][j].Data(),j);
                     buffer.AppendFormatted("         f%sHisto%d = gAnalyzer->Get%s();\n",tabHistoName[iTab][j].Data(),j,tabHistoName[iTab][j].Data());
                     buffer.AppendFormatted("         f%sHisto%d->Draw();\n",tabHistoName[iTab][j].Data(),j);
                  }
                  else {
                     for (k=tabHistoArrayIndexStart[iTab][j];k<=tabHistoArrayIndexEnd[iTab][j];k++) {
                        buffer.AppendFormatted("         fGeneratedCanvas->GetCanvas()->cd(%d);\n",i+1+k-tabHistoArrayIndexStart[iTab][j]);
                        buffer.AppendFormatted("         f%sPad%d_%d = (TPad*)gPad;\n",tabHistoName[iTab][j].Data(),j,k);
                        buffer.AppendFormatted("         f%sHisto%d_%d = gAnalyzer->Get%sAt(%d);\n",tabHistoName[iTab][j].Data(),j,k,tabHistoName[iTab][j].Data(),k);
                        buffer.AppendFormatted("         f%sHisto%d_%d->Draw();\n",tabHistoName[iTab][j].Data(),j,k);
                     }
                  }
                  buffer.AppendFormatted("      }\n");
                  break;
               }
            }
         }
      }
      if (tabHistoDisplay[iTab]) {
         buffer.AppendFormatted("      ArgusHistoDisplay::BaseInit();\n");
         for (i=0;i<numOfTabObjects[iTab];i++) {
            if (tabObjectTaskIndex[iTab][i]!=-1) {
               if (histoType[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]]=="TH1F") {
                  buffer.AppendFormatted("      if (fNumberOfUserTH1F<%s)\n",histoArraySize[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
                  buffer.AppendFormatted("         fNumberOfUserTH1F = %s;\n",histoArraySize[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
               }
               if (histoType[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]]=="TH2F") {
                  buffer.AppendFormatted("      if (fNumberOfUserTH2F<%s)\n",histoArraySize[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
                  buffer.AppendFormatted("         fNumberOfUserTH2F = %s;\n",histoArraySize[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
               }
            }
         }
         if (numOfTabObjects[iTab]>0) {
            buffer.AppendFormatted("      if (fNumberOfUserTH1F>0) {\n");
            buffer.AppendFormatted("         fUserTH1F = new TH1F*[fNumberOfUserTH1F+1];\n");
            buffer.AppendFormatted("         for (i=0;i<fNumberOfUserTH1F+1;i++) {\n");
            buffer.AppendFormatted("            str.SetFormatted(\"fUserTH1F_%%d_%%s\",i,fInheritanceName.Data());\n");
            buffer.AppendFormatted("            fUserTH1F[i] = new TH1F(str.Data(),\"\",1,0,1);\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      if (fNumberOfUserTH2F>0) {\n");
            buffer.AppendFormatted("         fUserTH2F = new TH2F*[fNumberOfUserTH2F+1];\n");
            buffer.AppendFormatted("         for (i=0;i<fNumberOfUserTH2F+1;i++) {\n");
            buffer.AppendFormatted("            str.SetFormatted(\"fUserTH2F_%%d_%%s\",i,fInheritanceName.Data());\n");
            buffer.AppendFormatted("            fUserTH2F[i] = new TH2F(str.Data(),\"\",1,0,1,1,0,1);\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      if (fNumberOfUserTGraph>0) {\n");
            buffer.AppendFormatted("         fUserTGraph = new TGraph*[fNumberOfUserTGraph+1];\n");
            buffer.AppendFormatted("         for (i=0;i<fNumberOfUserTGraph+1;i++) {\n");
            buffer.AppendFormatted("            str.SetFormatted(\"fUserTGraph_%%d_%%s\",i,fInheritanceName.Data());\n");
            buffer.AppendFormatted("            fUserTGraph[i] = new TGraph(1);\n");
            buffer.AppendFormatted("            fUserTGraph[i]->SetPoint(0,0,0);\n");
            buffer.AppendFormatted("         }\n");
            buffer.AppendFormatted("      }\n");
            buffer.AppendFormatted("      fDisplayType = k%sDisplay;\n",tabObjectType[iTab][0].Data());
            buffer.AppendFormatted("      fDisplayObjIndex = 0;\n");
         }
      }
      buffer.AppendFormatted("      EndInit();\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   virtual void Init() = 0;\n");
      buffer.AppendFormatted("   virtual void EndInit() {};\n");
      buffer.AppendFormatted("\n");

      // Display
      buffer.AppendFormatted("   void Display(bool processEvents=true) {\n");
      if (tabHistoDisplay[iTab]) {
         buffer.AppendFormatted("      int i,chn;\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("      for (i=0 ; i<fNumberOfPads ; i++) {\n");
         buffer.AppendFormatted("         if (fPadConfigActive)\n");
         buffer.AppendFormatted("            chn = fPadConfigChannel[i];\n");
         buffer.AppendFormatted("         else\n");
         buffer.AppendFormatted("            chn = fChannelNumber+i;\n");
         buffer.AppendFormatted("         if (fDisplayType==kTGraphDisplay) {\n");
         buffer.AppendFormatted("            if (i>=fNumberOfUserTGraph)\n");
         buffer.AppendFormatted("               *fTGraph[i] = *fUserTGraph[fNumberOfUserTGraph];\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               *fTGraph[i] = *fUserTGraph[chn];\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         if (fDisplayType==kTH1FDisplay) {\n");
         buffer.AppendFormatted("            if (i>=fNumberOfUserTH1F)\n");
         buffer.AppendFormatted("               *fTH1F[i] = *fUserTH1F[fNumberOfUserTH1F];\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               *fTH1F[i] = *fUserTH1F[chn];\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         if (fDisplayType==kTH2FDisplay) {\n");
         buffer.AppendFormatted("            if (i>=fNumberOfUserTH2F)\n");
         buffer.AppendFormatted("               *fTH2F[i] = *fUserTH2F[fNumberOfUserTH2F];\n");
         buffer.AppendFormatted("            else\n");
         buffer.AppendFormatted("               *fTH2F[i] = *fUserTH2F[chn];\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("      Modified(processEvents);\n");
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");

      // BaseTabEventHandler
      buffer.AppendFormatted("   void BaseEventHandler() {\n");
      if (tabHistoDisplay[iTab]) {
         for (i=0;i<numOfTabObjects[iTab];i++) {
            if (tabObjectTaskIndex[iTab][i]!=-1) {
               buffer.AppendFormatted("      int i;\n");
               break;
            }
         }
         buffer.AppendFormatted("\n");
         for (i=0;i<numOfTabObjects[iTab];i++) {
            if (tabObjectTaskIndex[iTab][i]!=-1) {
               buffer.AppendFormatted("      if (fDisplayObjIndex==%d && gAnalyzer->Is%sActive()) {\n",i,histoName[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
               buffer.AppendFormatted("         for (i=0;i<%s;i++)\n",histoArraySize[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
               buffer.AppendFormatted("            *fUser%s[i] = *(gAnalyzer->Get%sAt(i));\n",tabObjectType[iTab][i].Data(),histoName[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data());
               buffer.AppendFormatted("         for (i=%s;i<fNumberOfUser%s;i++)\n",histoArraySize[tabObjectTaskIndex[iTab][i]][tabObjectHistoIndex[iTab][i]].Data(),tabObjectType[iTab][i].Data());
               buffer.AppendFormatted("            *fUser%s[i] = *fUser%s[fNumberOfUser%s];\n",tabObjectType[iTab][i].Data(),tabObjectType[iTab][i].Data(),tabObjectType[iTab][i].Data());
               buffer.AppendFormatted("      }\n");
            }
         }
      }
      for (i=0;i<tabHistoIndexMax[iTab];i++) {
         for (j=0;j<numOfTabHistos[iTab];j++) {
            if (tabHistoIndex[iTab][j]==i) {
               buffer.AppendFormatted("      if (gAnalyzer->Is%sActive()) {\n",tabHistoName[iTab][j].Data());
               if (histoArraySize[tabHistoTaskIndex[iTab][j]][tabHistoHistoIndex[iTab][j]]=="1") {
                  buffer.AppendFormatted("         f%sHisto%d = gAnalyzer->Get%s();\n",tabHistoName[iTab][j].Data(),j,tabHistoName[iTab][j].Data());
                  buffer.AppendFormatted("         if (gAnalyzer->IsStandAloneARGUS() && gAnalyzer->IsSocketToROMEActive())\n");
                  buffer.AppendFormatted("            f%sHisto%d->Draw();\n",tabHistoName[iTab][j].Data(),j);
                  buffer.AppendFormatted("         f%sPad%d->Modified();\n",tabHistoName[iTab][j].Data(),j);
               }
               else {
                  for (k=tabHistoArrayIndexStart[iTab][j];k<=tabHistoArrayIndexEnd[iTab][j];k++) {
                     buffer.AppendFormatted("         f%sHisto%d_%d = gAnalyzer->Get%sAt(%d);\n",tabHistoName[iTab][j].Data(),j,k,tabHistoName[iTab][j].Data(),k);
                     buffer.AppendFormatted("         if (gAnalyzer->IsStandAloneARGUS() && gAnalyzer->IsSocketToROMEActive())\n");
                     buffer.AppendFormatted("            f%sHisto%d_%d->Draw();\n",tabHistoName[iTab][j].Data(),j,k);
                     buffer.AppendFormatted("         f%sPad%d_%d->Modified();\n",tabHistoName[iTab][j].Data(),j,k);
                  }
               }
               buffer.AppendFormatted("         fGeneratedCanvas->GetCanvas()->cd(1);\n");
               buffer.AppendFormatted("         fGeneratedCanvas->GetCanvas()->Modified();\n");
               buffer.AppendFormatted("         fGeneratedCanvas->GetCanvas()->Update();\n");
               buffer.AppendFormatted("      }\n");
               break;
            }
         }
      }
      buffer.AppendFormatted("      EventHandler();\n");
      if (tabHistoDisplay[iTab])
         buffer.AppendFormatted("      Display(false);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual void EventHandler() = 0;\n");
      buffer.AppendFormatted("\n");

      // Thread
      buffer.AppendFormatted("   virtual Bool_t ProcessMessage(Long_t msg, Long_t param1, Long_t param2) {\n");
      buffer.AppendFormatted("      return RunProcessMessageThread(msg, param1, param2);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   virtual Bool_t ProcessMessageThread(Long_t msg, Long_t param1, Long_t param2) {return kTRUE;}\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   static void ThreadProcessMessageThread(void* arg) {\n");
      buffer.AppendFormatted("      ((%sT%s_Base*)((%sArgs*)arg)->inst)->ProcessMessageThread(((%sArgs*)arg)->msg, ((%sArgs*)arg)->param1, ((%sArgs*)arg)->param2);\n", shortCut.Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("   Bool_t RunProcessMessageThread(Long_t msg, Long_t param1, Long_t param2) {\n");
      buffer.AppendFormatted("      %sArgs* arg = new %sArgs();\n", tabName[iTab].Data(), tabName[iTab].Data());
      buffer.AppendFormatted("      arg->inst   = this;\n");
      buffer.AppendFormatted("      arg->msg    = msg;\n");
      buffer.AppendFormatted("      arg->param1 = param1;\n");
      buffer.AppendFormatted("      arg->param2 = param2;\n");
      buffer.AppendFormatted("      TThread* mProcessMessageThread = new TThread(\"processMessageThread\",(void(*) (void *))&ThreadProcessMessageThread,(void*) arg);\n");
      buffer.AppendFormatted("      mProcessMessageThread->Run();\n");
      buffer.AppendFormatted("      return kTRUE;\n");
      buffer.AppendFormatted("   }\n");
      for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   //%s\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   virtual void %s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("%s", threadFunctionArgument[iTab][i][j].Data());
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted(")\n");
         buffer.AppendFormatted("   {\n");
         buffer.AppendFormatted("      cout<<endl\n");
         buffer.AppendFormatted("          <<\" Thread function %s is not implemented.\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" Please overwrite this function in derived class. For example,\"<<endl\n");
         buffer.AppendFormatted("          <<\" In %sT%s.h,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %s();\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\" In %sT%s.cpp,\"<<endl\n", shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("          <<\"   void %sT%s::%s()\"<<endl\n", shortCut.Data(), tabName[iTab].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   {\"<<endl\n");
         buffer.AppendFormatted("          <<\"      cout<<\\\"Thread function %s is running.\\\"<<endl;\"<<endl\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("          <<\"   }\"<<endl<<endl;\n");
         buffer.AppendFormatted("      Stop%s();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   static void Thread%s(void* arg) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      TThread::SetCancelOn();\n");
         buffer.AppendFormatted("      TThread::SetCancelDeferred();\n");
         buffer.AppendFormatted("      %sT%s_Base* inst = (%sT%s_Base*) arg;\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("      Int_t iLoop = 0;\n");
//         buffer.AppendFormatted("      Int_t meid=TThread::SelfId(); // get pthread id\n");
         buffer.AppendFormatted("      while (inst->f%sActive) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::CancelPoint();\n");
         buffer.AppendFormatted("         inst->%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++) {
            buffer.AppendFormatted("inst->f%sArgument_%d", threadFunctionName[iTab][i].Data(), j);
            if (j < numOfThreadFunctionArguments[iTab][i] - 1)
               buffer.AppendFormatted(",");
         }
         buffer.AppendFormatted("); // call the user defined threaded function\n");
         buffer.AppendFormatted("         if (inst->f%sNumberOfLoops != 0 && ++iLoop>=inst->f%sNumberOfLoops) {\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            inst->f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            if (inst->m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("               TThread::Delete(inst->m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("               inst->m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("            }\n");
         buffer.AppendFormatted("         }\n");
         buffer.AppendFormatted("         gSystem->Sleep(inst->f%sInterval);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Start%s(", threadFunctionName[iTab][i].Data());
         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("%s arg%d,", threadFunctionArgument[iTab][i][j].Data(), j);
         buffer.AppendFormatted("Int_t interval = 1000, Int_t nloop = 0) {\n");

         for (j = 0; j < numOfThreadFunctionArguments[iTab][i]; j++)
            buffer.AppendFormatted("      f%sArgument_%d = arg%d;\n", threadFunctionName[iTab][i].Data(), j, j);
         buffer.AppendFormatted("      f%sActive        = kTRUE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sNumberOfLoops = nloop;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sInterval      = interval;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      if (!m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = new TThread(\"Thread%s\",(void(*) (void *))&Thread%s,(void*) this);\n", threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data(), threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Run();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Stop%s() {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      gSystem->Sleep(1000); // wait a while for threads to halt\n");
         buffer.AppendFormatted("      if (m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         TThread::Delete(m%s);\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   } \n");
         buffer.AppendFormatted("   Bool_t Kill%s() {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      f%sActive = kFALSE;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      if (m%s) {\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s->Kill();\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("         m%s = 0;\n", threadFunctionName[iTab][i].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      return kTRUE;\n");
         buffer.AppendFormatted("   } \n");

         buffer.AppendFormatted("\n");
      }
      buffer.AppendFormatted("\n");

      // User Methods
      buffer.AppendFormatted("   // User Methods\n");
      if (numOfSteering[iTab+numOfTask+1] > 0) {
         buffer.AppendFormatted("   Steering* GetSteeringParameters() { return fSteering; };\n");
         buffer.AppendFormatted("   Steering* GetSP() { return fSteering; };\n");
      }
      buffer.AppendFormatted("   Bool_t       GetActive() { return fActive; };\n");
      buffer.AppendFormatted("   void         SetActive(Bool_t active) { fActive = active; };\n");

      // Tab Selected
      buffer.AppendFormatted("   void BaseTabSelected() {\n");
      if (tabHistoDisplay[iTab]) {
         buffer.AppendFormatted("      ArgusHistoDisplay::BaseTabSelected();\n");
         for (i=0;i<numOfTabObjects[iTab];i++)
            buffer.AppendFormatted("      fMenuDisplay->AddEntry(\"%s\", M_DISPLAY_%s);\n",tabObjectTitle[iTab][i].Data(),tabObjectName[iTab][i].ToUpper(str));
         buffer.AppendFormatted("      fMenuDisplay->RCheckEntry(M_DISPLAY_%s+fDisplayObjIndex,M_DISPLAY_%s,M_DISPLAY_%s);\n",tabObjectName[iTab][0].ToUpper(str),tabObjectName[iTab][0].ToUpper(str1),tabObjectName[iTab][numOfTabObjects[iTab]-1].ToUpper(str2));
      }
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   virtual void TabSelected() = 0;\n");
      // Tab Unselected
      buffer.AppendFormatted("   void BaseTabUnSelected() {\n");
      if (tabHistoDisplay[iTab]) {
         buffer.AppendFormatted("      ArgusHistoDisplay::BaseTabUnSelected();\n");
      }
      buffer.AppendFormatted("      TabUnSelected();\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   virtual void TabUnSelected() = 0;\n");

      buffer.AppendFormatted("\n");
      // Menu Clicked
      buffer.AppendFormatted("   void BaseMenuClicked(TGPopupMenu *menu,Long_t param) {\n");
      if (tabHistoDisplay[iTab]) {
         buffer.AppendFormatted("      int i=0;\n");
         buffer.AppendFormatted("      switch (param) {\n");
         for (i=0;i<numOfTabObjects[iTab];i++) {
            buffer.AppendFormatted("         case M_DISPLAY_%s:\n",tabObjectName[iTab][i].ToUpper(str));
            buffer.AppendFormatted("            {\n");
            buffer.AppendFormatted("               fDisplayObjIndex = %d;\n",i);
            buffer.AppendFormatted("               fDisplayType = k%sDisplay;\n",tabObjectType[iTab][i].Data());
            buffer.AppendFormatted("               SetupPads(fNumberOfPadsX,fNumberOfPadsY,true);\n");
            buffer.AppendFormatted("               fMenuDisplay->RCheckEntry(M_DISPLAY_%s,M_DISPLAY_%s,M_DISPLAY_%s);\n",tabObjectName[iTab][i].ToUpper(str),tabObjectName[iTab][0].ToUpper(str1),tabObjectName[iTab][numOfTabObjects[iTab]-1].ToUpper(str2));
            buffer.AppendFormatted("               break;\n");
            buffer.AppendFormatted("            }\n");
         }
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      ArgusHistoDisplay::BaseMenuClicked(menu,param);\n");
      }
      buffer.AppendFormatted("      MenuClicked(menu,param);\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   virtual void MenuClicked(TGPopupMenu *menu,Long_t param) = 0;\n");
      buffer.AppendFormatted("\n");


      // Footer
      buffer.AppendFormatted("\n   ClassDef(%sT%s_Base,%s)\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
      buffer.AppendFormatted("};\n\n");
      buffer.AppendFormatted("#endif   // %sT%s_Base_H\n", shortCut.Data(), tabName[iTab].Data());

      // Write File
      WriteFile(hFile.Data(), buffer.Data(), 6);

      // User H-File
      hFile.SetFormatted("%sinclude/tabs/%sT%s.h", outDir.Data(), shortCut.Data(), tabName[iTab].Data());
      if (gSystem->AccessPathName(hFile.Data(),kFileExists)) {
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
         buffer.AppendFormatted("#include \"include/generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[iTab].Data());

         // Class
         buffer.AppendFormatted("\nclass %sT%s : public %sT%s_Base\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("protected:\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("public:\n");

         // Constructor
         buffer.AppendFormatted("   %sT%s():%sT%s_Base()\n", shortCut.Data(), tabName[iTab].Data(), shortCut.Data(), tabName[iTab].Data());
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

         // Thread
         for (i = 0; i < numOfThreadFunctions[iTab]; i++) {
            buffer.AppendFormatted("   void %s();\n", threadFunctionName[iTab][i].Data());
         }
         buffer.AppendFormatted("\n");

         // Fields
         buffer.AppendFormatted("\n   ClassDef(%sT%s,%s)\n", shortCut.Data(), tabName[iTab].Data(), tabVersion[iTab].Data());
         buffer.AppendFormatted("};\n\n");
         buffer.AppendFormatted("#endif   // %sT%s_H\n", shortCut.Data(), tabName[iTab].Data());

         // Write File
         WriteFile(hFile.Data(), buffer.Data(), 6);
      }
   }

   return kTRUE;
}

Bool_t ROMEBuilder::WriteSteering(Int_t iTask)
{
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

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
   buffer.AppendFormatted("#include <string>\n\n");
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());

   WriteSteeringClass(buffer,0,iTask,0);

   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());

   //Write File
   WriteFile(hFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteAnalyzerCpp()
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
   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TH1.h\"\n");
   buffer.AppendFormatted("#include \"TROOT.h\"\n");
   buffer.AppendFormatted("#include \"TObjArray.h\"\n");
   buffer.AppendFormatted("#include \"TObjString.h\"\n");
   buffer.AppendFormatted("#include \"TBranchElement.h\"\n");
   buffer.AppendFormatted("#include \"TTask.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"include/generated/%sConfigToForm.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sWindow.h\"\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskUserCode[i])
         buffer.AppendFormatted("#include \"include/tasks/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
      else
         buffer.AppendFormatted("#include \"include/generated/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"include/generated/%sConfig.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());

   buffer.AppendFormatted("#include \"ROMEStr2DArray.h\"\n");
   buffer.AppendFormatted("#include \"ROMEDataBaseDAQ.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"Riostream.h\"\n");
   buffer.AppendFormatted("#include \"include/generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sAnalyzer);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%sAnalyzer *gAnalyzer;  // global Analyzer Handle\n",shortCut.Data());

   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include <Windows4Root.h>\n");
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
   buffer.AppendFormatted("%sAnalyzer::%sAnalyzer(ROMERint *app,Bool_t argus):ROMEAnalyzer(app,argus) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("// Folder and Task initialisation\n");
   buffer.AppendFormatted("   int i=0;\n");
   buffer.AppendFormatted("   char str[200];\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fProgramName = \"%s%s\";\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   gAnalyzer = this;\n");
   buffer.AppendFormatted("   gROME = static_cast<ROMEAnalyzer*>(this);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (argus) {\n");
   buffer.AppendFormatted("      sprintf(str,\"ARGUS - %%s\",fProgramName.Data());\n");
   buffer.AppendFormatted("      fWindow = new %sWindow(gClient->GetRoot(), str);\n",shortCut.Data());
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      fWindow = new %sWindow();\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fConfiguration = new %sConfig();\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fMidasDAQ = NULL;\n");
   buffer.AppendFormatted("   fRomeDAQ = NULL;\n");
   buffer.AppendFormatted("   fDataBaseDAQ = NULL;\n");
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      buffer.AppendFormatted("   f%s = NULL;\n",daqName[i].Data());
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
         if (folderParentName[i]=="GetMainFolder()") parentt = folderParentName[i];
         else parentt.SetFormatted("%sFolder",folderParentName[i].Data());
         format.SetFormatted("   TFolder* %%sFolder%%%ds = %%s->AddFolder(\"%%s\",\"%%s\");\n",nameLen-folderName[i].Length());
         buffer.AppendFormatted(format.Data(),folderName[i].Data(),"",parentt.Data(),folderName[i].Data(),folderTitle[i].Data());
      }
   }
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         if (numOfValue[i] > 0) {
            if (folderArray[i]=="1") {
               buffer.AppendFormatted("\n   f%sFolder = new %s%s();\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolder);\n",folderName[i].Data(),folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("\n   f%sFolders = new TClonesArray(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("\n   f%sFolders->SetName(\"%s%s\");\n",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
               buffer.AppendFormatted("   %sFolder->Add(f%sFolders);\n",folderName[i].Data(),folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Task
   buffer.AppendFormatted("   // Task initialisation\n");
   buffer.AppendFormatted("   fMainTask = new %sEventLoop(\"program\",\"Main Task of %s%s\");\n",shortCut.Data(),shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("   fMainFolder->Add(fMainTask);\n");
   buffer.AppendFormatted("   gROOT->GetListOfTasks()->Add(fMainTask);\n\n");

   int suffixNumber;
   int multiplicity;
   ROMEString suffix;

   buffer.AppendFormatted("   fMainHistoFolder = fMainFolder->AddFolder(\"histos\",\"Histogram Folder\");\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   fTaskObjects = new TObjArray(%d);\n",numOfTaskHierarchy);
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      // create suffix
      suffixNumber = 0;
      multiplicity = 0;
      for (j=0;j<numOfTaskHierarchy;j++) {
         if (j!=i && taskHierarchyName[i]==taskHierarchyName[j]) {
            multiplicity++;
            if (j<i)
               suffixNumber++;
         }
      }
      if (multiplicity>0) {
         if (multiplicity<=9)
            suffix.SetFormatted("_%01d",suffixNumber);
         if (multiplicity>9)
            suffix.SetFormatted("_%02d",suffixNumber);
         if (multiplicity>99)
            suffix.SetFormatted("_%03d",suffixNumber);
      }
      else
         suffix = "";
      // create histo folder
      if (taskHierarchyParentIndex[i]==-1)
         buffer.AppendFormatted("   fHistoFolders->AddAtAndExpand(fMainHistoFolder->AddFolder(\"%sHistos%s\",\"Histograms of Task '%s%s'\"),%d);\n",taskHierarchyName[i].Data(),suffix.Data(),taskHierarchyName[i].Data(),suffix.Data(),i);
      else
         buffer.AppendFormatted("   fHistoFolders->AddAtAndExpand(GetHistoFolderAt(%d)->AddFolder(\"%sHistos%s\",\"Histograms of Task '%s%s'\"),%d);\n",taskHierarchyParentIndex[i],taskHierarchyName[i].Data(),suffix.Data(),taskHierarchyName[i].Data(),suffix.Data(),i);
      // create task
      buffer.AppendFormatted("   AddTask(new %sT%s(\"%s_%d\",\"%s\",%d,\"%s\",GetHistoFolderAt(%d)));\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyName[i].Data(),i,"",taskHierarchyLevel[i],suffix.Data(),i);
      buffer.AppendFormatted("   GetTaskObjectAt(%d)->SetActive(false);\n",taskHierarchyObjectIndex[i]);
      if (taskHierarchyParentIndex[i]==-1)
         parentt = "GetMainTask()";
      else
         parentt.SetFormatted("GetTaskObjectAt(%d)",taskHierarchyParentIndex[i]);
      buffer.AppendFormatted("   %s->Add(GetTaskObjectAt(%d));\n",parentt.Data(),taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("\n");

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if ( valueDimension[i][j]>1 )
               continue;
            buffer.AppendFormatted("   f%s_%sDBName = \"\";\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   f%s_%sDBPath = \"\";\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   f%s_%sDBCode = new TArrayI(%d);\n",folderName[i].Data(),valueName[i][j].Data(),maxNumberOfPathObjectInterpreterCodes);
            buffer.AppendFormatted("   for (i=0;i<%d;i++)\n",maxNumberOfPathObjectInterpreterCodes);
            buffer.AppendFormatted("      f%s_%sDBCode->AddAt(-1,i);\n",folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }
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

   int ndb = 0;
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i])
         ndb++;
   }

   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      WriteFolderGetterSource(buffer,i,scl);
   }
   buffer.AppendFormatted("\n");

   // Set size
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i] && numOfValue[i] > 0 && folderArray[i]!="1") {
         buffer.AppendFormatted("void %sAnalyzer::Set%sSize(Int_t number) {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("   int i;\n");
         buffer.AppendFormatted("   if (f%sFolders) f%sFolders->Delete();\n",folderName[i].Data(),folderName[i].Data());
         buffer.AppendFormatted("   for (i=0;i<number;i++) {\n");
         buffer.AppendFormatted("      new((*f%sFolders)[i]) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
         for (j=0;j<numOfValue[i];j++) {
            if (isFolder(valueType[i][j].Data()))
               continue;
            if (isRootClassType(valueType[i][j].Data()) && !isPointerType(valueType[i][j].Data())
               && !valueType[i][j].Contains("TRef") && !valueType[i][j].Contains("TString"))
            continue;
            if (valueDimension[i][j]==0)
               buffer.AppendFormatted("(%s)%s,",valueType[i][j].Data(),valueInit[i][j].Data());
         }
         buffer.Resize(buffer.Length()-1);
         buffer.AppendFormatted(" );\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
   }
   buffer.AppendFormatted("\n");

   // Read each folders
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("Bool_t %sAnalyzer::Read%s() {\n",shortCut.Data(),folderName[i].Data());
         WriteReadDataBaseFolder(buffer,i,folderArray[i]=="1" ? 1 : 2);
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n\n");
      }
   }

   // ReadSingleDataBaseFolders
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadSingleDataBaseFolders() {\n",shortCut.Data());
   if (ndb>0) {
      for (i=0;i<numOfFolder;i++) {
         if (!folderUsed[i])
            continue;
         if (folderDataBase[i] && folderArray[i]=="1" && !folderSupport[i]) {
            buffer.AppendFormatted("   if (!Read%s())\n",folderName[i].Data());
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
            buffer.AppendFormatted("   if (!Read%s())\n",folderName[i].Data());
            buffer.AppendFormatted("      return false;\n");
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Database Folder Field Getters
   buffer.AppendFormatted("// Database Folder Field Getters\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("const char* %sAnalyzer::Get%s_%sDBName() {\n",shortCut.Data(),folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   if (f%s_%sDBName==\"\") {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      return \"%s\";\n",valueDBName[i][j].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      return f%s_%sDBName.Data();\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("const char* %sAnalyzer::Get%s_%sDBPath(ROMEString& path) {\n",shortCut.Data(),folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   if (f%s_%sDBPath==\"\") {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      path.SetFormatted(%s);\n",valueDBPath[i][j].Data());
            buffer.AppendFormatted("      return path.Data();\n");
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      return f%s_%sDBPath.Data();\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("const char* %sAnalyzer::Get%s_%sDBPathOriginal() {\n",shortCut.Data(),folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   if (f%s_%sDBPath==\"\") {\n",folderName[i].Data(),valueName[i][j].Data());
            str = valueDBPath[i][j];
            str.ReplaceAll("\"","\\\"");
            buffer.AppendFormatted("      return \"%s\";\n",str.Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      return f%s_%sDBPath.Data();\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("}\n");
         }
      }
   }
   buffer.AppendFormatted("\n");

   // ReadUserParameter
   buffer.AppendFormatted("Bool_t %sAnalyzer::ReadUserParameter(const char* opt, const char* value, Int_t& i) {\n",shortCut.Data());
   // Global Steering Parameter
   buffer.AppendFormatted("   ROMEString option = opt;\n");
   buffer.AppendFormatted("   ROMEString tmp;\n");
   buffer.AppendFormatted("   char *cstop=NULL;\n");
   buffer.AppendFormatted("   // global steering parameters\n");
   if (numOfSteering[numOfTask]>0) {
      ROMEString pointerT;
      ROMEString steerPointerT;
      pointerT.SetFormatted("->fGlobalSteering");
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
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
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
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
         steerPointerT.SetFormatted("gAnalyzer->GetWindow()->Get%s%03dTab()->GetSP()", tabName[i].Data(), i);
         WriteSteeringReadParameters(buffer, 0, i+numOfTask+1, pointerT, steerPointerT);
      }
   }
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
      steerPointerT.SetFormatted("gAnalyzer->GetGSP()");
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
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
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
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->Get%s%03dTab())->GetSP()", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), i);
         WriteSteeringParameterUsage(buffer, 0, i+numOfTask+1, pointerT, steerPointerT);
      }
   }
   buffer.AppendFormatted("}\n\n");

   // DAQ Access Methods
   ROMEStrArray daqNameArray(3);
   ROMEStrArray daqTypeArray(3);
   daqNameArray.AddLast("Midas");
   daqTypeArray.AddLast(shortCut.Data());
   daqNameArray.AddLast("Rome");
   daqTypeArray.AddLast(shortCut.Data());
   daqNameArray.AddLast("DataBase");
   daqTypeArray.AddLast("ROME");
   if (this->orca) {
      daqNameArray.AddLast("Orca");
      daqTypeArray.AddLast("ROME");
   }
   buffer.AppendFormatted("// Deprecated DAQ Access Methods\n");
   for (i=0;i<daqNameArray.GetEntriesFast();i++) {
      buffer.AppendFormatted("// %s DAQ Access Methods\n",daqNameArray.At(i).Data());
      buffer.AppendFormatted("%s%sDAQ* %sAnalyzer::Get%sDAQ() {\n",daqTypeArray.At(i).Data(),daqNameArray.At(i).Data(),shortCut.Data(),daqNameArray.At(i).Data());
      buffer.AppendFormatted("   if (f%sDAQ==NULL) {\n",daqNameArray.At(i).Data());
      buffer.AppendFormatted("      this->PrintLine(\"\\nYou have tried to access the %s DAQ system over a gAnalyzer->Get%sDAQ()\\nhandle but the current DAQ system is not '%s'.\\n\\nShutting down the program.\\n\");\n",daqNameArray.At(i).Data(),daqNameArray.At(i).Data(),daqNameArray.At(i).Data());
      buffer.AppendFormatted("      fApplication->Terminate(1);\n");
      buffer.AppendFormatted("      return NULL;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   return f%sDAQ;\n",daqNameArray.At(i).Data());
      buffer.AppendFormatted("};\n");
   }

   // Show Configuration
   buffer.AppendFormatted("Bool_t %sAnalyzer::ShowConfigurationFile()\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i=0;\n");
   buffer.AppendFormatted("   bool writeFlag = true;\n");
   buffer.AppendFormatted("   ROMEString str;\n");
   buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEString path = \"\";\n");
   buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
   buffer.AppendFormatted("   int ind = 0;\n");
   buffer.AppendFormatted("   char *cstop = NULL;\n");
   buffer.AppendFormatted("   int exitID;\n");
   buffer.AppendFormatted("   %sConfigToForm *dialog = new %sConfigToForm();\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   if((exitID=dialog->Show(gClient->GetRoot(),NULL))==1) {\n");
   buffer.AppendFormatted("      if (!fWindow->IsActive())\n");
   buffer.AppendFormatted("         gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("      else {\n");
   buffer.AppendFormatted("         if (dialog->IsChecked(\"ROME\") && dialog->IsChecked(\"ARGUS\"))\n");
   buffer.AppendFormatted("            gAnalyzer->SetROMEAndARGUS();\n");
   buffer.AppendFormatted("         else if (dialog->IsChecked(\"ARGUS\"))\n");
   buffer.AppendFormatted("            gAnalyzer->SetStandAloneARGUS();\n");
   buffer.AppendFormatted("         else\n");
   buffer.AppendFormatted("            gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("      gAnalyzer->SetRunNumbers(dialog->GetValue(\"RunParameters/RunNumbers\"));\n");
   buffer.AppendFormatted("      gAnalyzer->SetInputFileNames(dialog->GetValue(\"RunParameters/InputFileNames\"));\n");
   WriteConfigToFormSave(buffer,mainParGroup,"","","");
   buffer.AppendFormatted("      GetConfiguration()->CheckConfigurationModified(0);\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   delete dialog;\n");
   buffer.AppendFormatted("   return exitID!=-1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

/*   // WriteDataBaseFolders
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         buffer.AppendFormatted("void %sAnalyzer::Write%sDataBase() {\n",shortCut.Data(),folderName[i].Data());
         buffer.AppendFormatted("   int i,j;\n");
         buffer.AppendFormatted("   ROMEString path;\n");
         buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
         buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
         for (j=0;j<numOfValue[i];j++) {
            buffer.AppendFormatted("   values->RemoveAll();\n");
            if (folderArray[i]=="1") {
               if (valueArray[i][j]=="1") {
                  buf = "buffer[0]";
                  str.SetFormatted("f%sFolder->Get%s()",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("   values->SetAt(%s,0,0);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
               else {
                  buf = "buffer[0]";
                  str.SetFormatted("f%sFolder->Get%sAt(j)",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("   for (j=0;j<%s;j++)\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("      values->SetAt(%s,0,j);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
            }
            else {
               buffer.AppendFormatted("   for (i=0;i<f%sFolders->GetEntries();i++)\n",folderName[i].Data());
               if (valueArray[i][j]=="1") {
                  buf = "buffer[0]";
                  str.SetFormatted("((%s%s*)f%sFolders->At(i))->Get%s()",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      values->SetAt(%s,i,0);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
               else {
                  buf = "buffer[0]";
                  str.SetFormatted("((%s%s*)f%sFolders->At(i))->Get%sAt(j)",shortCut.Data(),folderName[i].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      for (j=0;j<%s;j++)\n",valueArray[i][j].Data());
                  buffer.AppendFormatted("         values->SetAt(%s,i,j);\n",convertType(str.Data(),valueType[i][j].Data(),"ROMEString&",buf).Data());
               }
            }
            buffer.AppendFormatted("   path.SetFormatted(gAnalyzer->Get%s_%sDBPath()",folderName[i].Data(),valueName[i][j].Data());
            for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
               buffer.AppendFormatted(",gAnalyzer->GetObjectInterpreterCharValue(gAnalyzer->Get%s_%sDBCodeAt(%d),buffer[%d],buffer[%d]).Data()",folderName[i].Data(),valueName[i][j].Data(),k,k,k);
            buffer.AppendFormatted(");\n");
            buffer.AppendFormatted("   if (!this->GetDataBase(gAnalyzer->Get%s_%sDBIndex())->Write(values,path,gAnalyzer->GetCurrentRunNumber())) {\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      gAnalyzer->PrintLine(\"   in Folder '%s' Value '%s'.\");\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("      delete values;\n");
            buffer.AppendFormatted("      return;\n");
            buffer.AppendFormatted("   }\n");
         }
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   delete values;\n");
         buffer.AppendFormatted("}\n");
      }
   }
*/

   // Get Object Interpreter Code
   int codeNumber = 0;
   buffer.AppendFormatted("Int_t %sAnalyzer::GetObjectInterpreterCode(const char* objectPath) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString path = objectPath;\n");
   buffer.AppendFormatted("   if (path.Index(\"/\")==-1 && path.Index(\"gAnalyzer->\")==-1)\n");
   buffer.AppendFormatted("      return -1;\n");
   buffer.AppendFormatted("   if (path.Index(\"/GSP\")==0 || path.Index(\"gAnalyzer->GetGSP()\")==0) {\n");
   ROMEString path1 = "/GSP";
   ROMEString path2 = "gAnalyzer->GetGSP()";
   codeNumber = WriteSteeringInterpreterCode(buffer,codeNumber,0,numOfTask,path1,path2,1);
   buffer.AppendFormatted("      gAnalyzer->PrintLine(\"\\nWrong path to a steering parameter in configuration file.\\n\");\n");
   buffer.AppendFormatted("      gAnalyzer->PrintLine(\"   \"+path+\"\\n\");\n");
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
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"\\nWrong path in configuration file.\\n\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"   \"+path+\"\\n\");\n");
   buffer.AppendFormatted("   return -1;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");
   fNumberOfInterpreterCodes = codeNumber;

   // Get Object Interpreter Values
   WriteObjectInterpreterValue(buffer,"int","Int");
   WriteObjectInterpreterValue(buffer,"double","Double");
   WriteObjectInterpreterValue(buffer,"ROMEString&","Char");

   // Tasks
   buffer.AppendFormatted("// Tasks\n");
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("bool %sAnalyzer::Is%sActive() {\n",shortCut.Data(),taskHierarchyName[i].Data());
      buffer.AppendFormatted("   return ((%sT%s*)GetTaskObjectAt(%d))->IsActive();\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("}\n");
   }
   buffer.AppendFormatted("\n");

   // Histo Getters
   buffer.AppendFormatted("// Histo\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         if (histoArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("%s* %sAnalyzer::Get%s() {\n",histoType[taskHierarchyClassIndex[i]][j].Data(),shortCut.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      return (%s*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s\"));\n",histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      return ((%sT%s*)GetTaskObjectAt(%d))->Get%s();\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("}\n");
         }
         else {
            buffer.AppendFormatted("TObjArray* %sAnalyzer::Get%s() {\n",shortCut.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      return (TObjArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s\"));\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      return ((%sT%s*)GetTaskObjectAt(%d))->Get%s();\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("%s* %sAnalyzer::Get%sAt(Int_t index) {\n",histoType[taskHierarchyClassIndex[i]][j].Data(),shortCut.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneARGUS() && IsSocketToROMEActive()) {\n");
            buffer.AppendFormatted("      ROMEString name;\n");
            buffer.AppendFormatted("      Int_t arrayStartIndex = 0;\n");
            buffer.AppendFormatted("      arrayStartIndex = ((%sT%s*)GetTaskObjectAt(%d))->Get%sArrayStartIndex();\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      name.SetFormatted(\"_%%0*d\",3,index+arrayStartIndex);\n");
            buffer.AppendFormatted("      name.Insert(0,\"%s\");\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      return (%s*)(GetSocketToROMENetFolder()->FindObjectAny(name.Data()));\n",histoType[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("   else {\n");
            buffer.AppendFormatted("      return ((%sT%s*)GetTaskObjectAt(%d))->Get%sAt(index);\n",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   }\n");
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("int %sAnalyzer::GetNumberOf%s() {\n",shortCut.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   return Get%s()->GetEntriesFast();\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("}\n");
         }
         buffer.AppendFormatted("bool %sAnalyzer::Is%sActive() {\n",shortCut.Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
         buffer.AppendFormatted("   return ((%sT%s*)GetTaskObjectAt(%d))->IsActive()",shortCut.Data(),taskHierarchyName[i].Data(),taskHierarchyObjectIndex[i]);
         int index = taskHierarchyParentIndex[i];
         while (index!=-1) {
            buffer.AppendFormatted(" && ((%sT%s*)GetTaskObjectAt(%d))->IsActive()",shortCut.Data(),taskHierarchyName[index].Data(),taskHierarchyObjectIndex[index]);
            index = taskHierarchyParentIndex[index];
         }
         buffer.AppendFormatted(";\n");
         buffer.AppendFormatted("}\n");
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("bool %sAnalyzer::ResetAllHistos() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         if (histoArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   Get%s()->Reset();\n",histoName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   for (i=0;i<GetNumberOf%s();i++)\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      Get%sAt(i)->Reset();\n",histoName[taskHierarchyClassIndex[i]][j].Data());
         }
      }
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Connect SocketToROME NetFolder
   buffer.AppendFormatted("// Connect SocketToROME NetFolder\n");
   buffer.AppendFormatted("Bool_t %sAnalyzer::ConnectSocketToROMENetFolder() {\n",shortCut.Data());
   buffer.AppendFormatted("   delete fSocketToROMENetFolder;\n");
   buffer.AppendFormatted("   fSocketToROMENetFolder = new ROMENetFolder(\"%s\",\"RootNetFolder\", fSocketToROME, true);\n",shortCut.Data());
   buffer.AppendFormatted("   if (!fSocketToROMENetFolder->GetPointer()) {\n");
   buffer.AppendFormatted("      Warning(\"ConnectSocketToROMENetFolder\", \"Failed to connect to the ROME analyzer.\");\n");
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
   ROMEString prog;
   prog.SetFormatted("%s%s",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("void %sAnalyzer::consoleStartScreen() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*****************************************\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintText(\"*\");\n");
   int len1 = (int)((39-(double)prog.Length())/2+0.5);
   int len2 = (int)((39-(double)prog.Length())/2);
   buffer.AppendFormatted("   for (i=0;i<%d;i++) gAnalyzer->PrintText(\" \");\n",len1);
   buffer.AppendFormatted("   gAnalyzer->PrintText(\"%s\");\n",prog.Data());
   buffer.AppendFormatted("   for (i=0;i<%d;i++) gAnalyzer->PrintText(\" \");\n",len2);
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*   generated by the ROME Environment   *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*             %s               *\");\n",romeVersion.Data());
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*                                       *\");\n");
   buffer.AppendFormatted("   gAnalyzer->PrintLine(\"*****************************************\\n\");\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("   \n");

   if (haveFortranTask) {
      buffer.AppendFormatted("// Interface to Fortran Tasks\n");
      for (i=0;i<numOfFolder;i++) {
         if (!folderUsed[i])
            continue;
         if (!folderSupport[i]) {
            for (j=0;j<numOfValue[i];j++) {
               if (!isNumber(valueType[i][j].Data()))
                  continue;
               if (folderArray[i]=="1") {
                  if (valueArray[i][j][0]=="1") {
                     buffer.AppendFormatted("extern \"C\" float Get%s_Get%s_C() {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%s()->Get%s();\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
                  else {
                     buffer.AppendFormatted("extern \"C\" float Get%s_Get%sAt_C(int j) {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%s()->Get%sAt(j);\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
               }
               else {
                  if (valueArray[i][j][0]=="1") {
                     buffer.AppendFormatted("extern \"C\" float Get%sAt_Get%s_C(int i) {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%sAt(i)->Get%s();\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
                  else {
                     buffer.AppendFormatted("extern \"C\" float Get%sAt_Get%sAt_C(int i,int j) {\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("   return (float)gAnalyzer->Get%sAt(j)->Get%sAt(j);\n",folderName[i].Data(),valueName[i][j].Data());
                     buffer.AppendFormatted("}\n");
                  }
               }
            }
         }
      }
   }

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
               buffer.AppendFormatted("      if (p%s)\n",folderName[i].Data());
               buffer.AppendFormatted("         *f%sFolder = *p%s;\n",folderName[i].Data(),folderName[i].Data());
            }
            else {
               buffer.AppendFormatted("      TClonesArray *p%s = (TClonesArray*)in.FindObjectAny(\"%ss\");\n",folderName[i].Data(),folderName[i].Data());
               buffer.AppendFormatted("      if (p%s)\n",folderName[i].Data());
               buffer.AppendFormatted("          p%s->Copy(*f%sFolders);\n",folderName[i].Data(),folderName[i].Data());
            }
            buffer.AppendFormatted("   }\n");
         }
      }
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteAnalyzerH()
{
   int i,j;

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
#if defined( R__UNIX )
   buffer.AppendFormatted("#include <unistd.h>\n");
#endif
   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TTask.h\"\n");
   buffer.AppendFormatted("#include \"TTree.h\"\n");
   buffer.AppendFormatted("#include \"TFolder.h\"\n");
   buffer.AppendFormatted("#include \"TClonesArray.h\"\n");
   buffer.AppendFormatted("#include \"TH1.h\"\n");
   buffer.AppendFormatted("#include \"TH2.h\"\n");
   buffer.AppendFormatted("#include \"TH3.h\"\n");
   buffer.AppendFormatted("#include \"TProfile.h\"\n");
   buffer.AppendFormatted("#include \"TProfile2D.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMETask.h\"\n");
   buffer.AppendFormatted("#include \"ROMEAnalyzer.h\"\n");

   // DAQ includes
   buffer.AppendFormatted("#include \"include/generated/%sMidasDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sRomeDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEDataBaseDAQ.h\"\n");
   if (this->orca)
      buffer.AppendFormatted("#include \"ROMEOrcaDAQ.h\"\n");
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      buffer.AppendFormatted("#include \"include/daqs/%s%s.h\"\n",shortCut.Data(),daqName[i].Data());
   }
   // Folder includes
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
   buffer.AppendFormatted("\n");

   // Folder class declaration
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         buffer.AppendFormatted("class %s%s;\n",shortCut.Data(),folderName[i].Data());
      }
   }
   buffer.AppendFormatted("\n");

   // Task Switches Structure
   buffer.AppendFormatted("// Task Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      int index = taskHierarchyParentIndex[i];
      switchString.SetFormatted("%s%03d",taskHierarchyName[i].Data(),i);
      while (index!=-1) {
         switchString.Insert(0,"_");
         switchString.InsertFormatted(0,"%s%03d",taskHierarchyName[index].Data(),index);
         index = taskHierarchyParentIndex[index];
      }
      format.SetFormatted("   Int_t %%s;%%%ds  //! %%s Task\n",switchLen-switchString.Length());
      buffer.AppendFormatted(format.Data(),switchString.Data(),"",switchString.Data());
   }
   buffer.AppendFormatted("} TaskSwitches;\n");

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
               format.SetFormatted("   %%s%%s*%%%ds f%%sFolder; %%%ds // Handle to %%s%%s Folder\n",typeLen-folderName[i].Length()-scl,nameLen-folderName[i].Length());
               buffer.AppendFormatted(format.Data(),shortCut.Data(),folderName[i].Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
            }
            else {
               format.SetFormatted("   TClonesArray*%%%ds f%%sFolders;%%%ds // Handle to %%s%%s Folders\n",typeLen-12,nameLen-folderName[i].Length());
               buffer.AppendFormatted(format.Data(),"",folderName[i].Data(),"",shortCut.Data(),folderName[i].Data());
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Database Folder Fields
   buffer.AppendFormatted("   // Database Folder Fields\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            format.SetFormatted("   ROMEString f%%s_%%sDBName;%%%ds // Name of the database from which to read the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
            format.SetFormatted("   ROMEString f%%s_%%sDBPath; %%%ds // Database path to the value of the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
            format.SetFormatted("   TArrayI   *f%%s_%%sDBCode; %%%ds // Object Interpreter codes for database path to the value of the field %%s of the folder %%s\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",valueName[i][j].Data(),folderName[i].Data());
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
   buffer.AppendFormatted("   %sMidasDAQ* fMidasDAQ; // Handle to the Midas DAQ Class\n",shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ*  fRomeDAQ; // Handle to the Rome DAQ Class\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEDataBaseDAQ*  fDataBaseDAQ; // Handle to the DataBase DAQ Class\n",shortCut.Data());
   if (this->orca)
      buffer.AppendFormatted("   ROMEOrcaDAQ* fOrcaDAQ; // Handle to the Orca DAQ Class\n");
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      buffer.AppendFormatted("   %s%s*  f%s; // Handle to the %s DAQ Class\n",shortCut.Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   TaskSwitches fTaskSwitches;               //! Task Switches\n");
   buffer.AppendFormatted("\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sAnalyzer(ROMERint *app,Bool_t argus);\n",shortCut.Data());

   // Folder Getters
   buffer.AppendFormatted("   // Folders\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      WriteFolderGetterInclude(buffer,i,scl,nameLen,typeLen);
   }
   buffer.AppendFormatted("\n");

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

   // Histo Getters
   buffer.AppendFormatted("   // Histos\n");
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      for (j=0;j<numOfHistos[i];j++) {
         if (histoArraySize[i][j]=="1")
            buffer.AppendFormatted("   %s* Get%s();\n",histoType[i][j].Data(),histoName[i][j].Data());
         else {
            buffer.AppendFormatted("   TObjArray* Get%s();\n",histoName[i][j].Data());
            buffer.AppendFormatted("   %s* Get%sAt(Int_t index);\n",histoType[i][j].Data(),histoName[i][j].Data());
            buffer.AppendFormatted("   int GetNumberOf%s();\n",histoName[i][j].Data());
         }
         buffer.AppendFormatted("   bool Is%sActive();\n",histoName[i][j].Data());
      }
   }
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   bool ResetAllHistos();\n");
   buffer.AppendFormatted("\n");

   // Tree Getters
   buffer.AppendFormatted("   // Trees\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   TTree* Get%sTree() { return ((ROMETree*)fTreeObjects->At(%d))->GetTree(); };\n",treeName[i].Data(),i);
   }
   buffer.AppendFormatted("\n");

   // Database Getters
   buffer.AppendFormatted("   // Database\n");
   buffer.AppendFormatted("   ROMEDataBase* GetXMLDataBase() { return GetDataBase(\"XML\"); };\n");
   buffer.AppendFormatted("   ROMEDataBase* GetTextDataBase() { return GetDataBase(\"TEXT\"); };\n");
   buffer.AppendFormatted("   ROMEDataBase* GetODBDataBase() { return GetDataBase(\"ODB\"); };\n");
   if (this->sql)
      buffer.AppendFormatted("   ROMEDataBase* GetSQLDataBase() { return GetDataBase(\"SQL\"); };\n");
   for (i=0;i<numOfDB;i++) {
      buffer.AppendFormatted("   ROMEDataBase* Get%sDataBase() { return GetDataBase(\"%s\"); };\n",dbName[i].Data(),dbName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Database Folder Field Getters
   buffer.AppendFormatted("   // Database Folder Field Getters\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            buffer.AppendFormatted("   const char* Get%s_%sDBName();\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   const char* Get%s_%sDBPath(ROMEString& path);\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   const char* Get%s_%sDBPathOriginal();\n",folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   Int_t       Get%s_%sDBNumberOfCode() { return f%s_%sDBCode->GetSize(); };\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
            buffer.AppendFormatted("   Int_t       Get%s_%sDBCodeAt(Int_t i) { return f%s_%sDBCode->At(i); };\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Database Folder Field Setters
   buffer.AppendFormatted("   // Database Folder Field Setters\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            format.SetFormatted("   void Set%%s_%%sDBName(const char* name)%%%ds  { f%%s_%%sDBName = name; };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBPath(const char* path)%%%ds  { f%%s_%%sDBPath = path; };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBNumberOfCode(Int_t num)%%%ds   { f%%s_%%sDBCode->Set(num); };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
            format.SetFormatted("   void Set%%s_%%sDBCodeAt(Int_t i,Int_t code)%%%ds  { f%%s_%%sDBCode->AddAt(code,i); };\n",fieldLen+nameLen-folderName[i].Length()-valueName[i][j].Length());
            buffer.AppendFormatted(format.Data(),folderName[i].Data(),valueName[i][j].Data(),"",folderName[i].Data(),valueName[i][j].Data());
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Tasks
   buffer.AppendFormatted("   // Tasks\n");
   buffer.AppendFormatted("   TaskSwitches* GetTaskSwitches() { return &fTaskSwitches; };\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("   bool Is%sActive();\n",taskHierarchyName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Data Base
   buffer.AppendFormatted("   // DataBase Methodes\n");
   // Read each folders
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i])
         buffer.AppendFormatted("   Bool_t Read%s();\n",folderName[i].Data());
   }
   buffer.AppendFormatted("   Bool_t ReadSingleDataBaseFolders();\n");
   buffer.AppendFormatted("   Bool_t ReadArrayDataBaseFolders();\n");
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
      buffer.AppendFormatted("   %sGlobalSteering* GetGSP() { return fGlobalSteeringParameters; };\n",shortCut.Data());
      buffer.AppendFormatted("\n");
   }

   // DAQ Access Methods
   ROMEStrArray daqNameArray(3);
   ROMEStrArray daqTypeArray(3);
   daqNameArray.AddLast("Midas");
   daqTypeArray.AddLast(shortCut.Data());
   daqNameArray.AddLast("Rome");
   daqTypeArray.AddLast(shortCut.Data());
   daqNameArray.AddLast("DataBase");
   daqTypeArray.AddLast("ROME");
   if (this->orca) {
      daqNameArray.AddLast("Orca");
      daqTypeArray.AddLast("ROME");
   }
   buffer.AppendFormatted("   // Deprecated DAQ Access Methods\n");
   buffer.AppendFormatted("   %sMidasDAQ* GetMidas() { return GetMidasDAQ(); };\n",shortCut.Data());
   buffer.AppendFormatted("   %sRomeDAQ* GetRome() { return GetRomeDAQ(); };\n",shortCut.Data());
   for (i=0;i<daqNameArray.GetEntriesFast();i++) {
      buffer.AppendFormatted("   // %s DAQ Access Methods\n",daqNameArray.At(i).Data());
      buffer.AppendFormatted("   Bool_t Is%sDAQ() { return f%sDAQ!=NULL; };\n",daqNameArray.At(i).Data(),daqNameArray.At(i).Data());
      buffer.AppendFormatted("   %s%sDAQ* Get%sDAQ();\n",daqTypeArray.At(i).Data(),daqNameArray.At(i).Data(),daqNameArray.At(i).Data());
      buffer.AppendFormatted("   void     Set%sDAQ(%s%sDAQ* handle) { f%sDAQ = handle; };\n",daqNameArray.At(i).Data(),daqTypeArray.At(i).Data(),daqNameArray.At(i).Data(),daqNameArray.At(i).Data());
   }
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      buffer.AppendFormatted("   %s%s*  Get%s()                 { return f%s;    };\n",shortCut.Data(),daqName[i].Data(),daqName[i].Data(),daqName[i].Data());
      buffer.AppendFormatted("   void     Set%s (%s%s*  handle) { f%s  = handle; };\n",daqName[i].Data(),shortCut.Data(),daqName[i].Data(),daqName[i].Data());
   }

   // Folder dump and load
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t   DumpFolders(const char* filename, Bool_t only_database = kFALSE);\n");
   buffer.AppendFormatted("   Bool_t   LoadFolders(const char* filename, Bool_t only_database = kFALSE);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   Bool_t   ShowConfigurationFile();\n");
   buffer.AppendFormatted("\n");


   buffer.AppendFormatted("   %sWindow *GetWindow() { return (%sWindow*)fWindow; };\n",shortCut.Data(),shortCut.Data());
   // Private
   buffer.AppendFormatted("private:\n");
   buffer.AppendFormatted("   Bool_t ReadUserParameter(const char* opt, const char *value, Int_t& i);\n");
   buffer.AppendFormatted("   void   UserParameterUsage();\n");
   buffer.AppendFormatted("   void   startSplashScreen();\n");
   buffer.AppendFormatted("   void   consoleStartScreen();\n");
   buffer.AppendFormatted("   Bool_t ConnectSocketToROMENetFolder();\n");
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

Bool_t ROMEBuilder::WriteAnalyzerF()
{
   int i,j;

   ROMEString fFile;
   ROMEString buffer;

   bool writeFortran = false;
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskFortran[i]) {
         writeFortran = true;
         break;
      }
   }
   if (!writeFortran)
      return true;

   // File name
   fFile.SetFormatted("%ssrc/generated/%sfanalyzer.F",outDir.Data(),shortCut.Data());

   // Description
   buffer.Resize(0);

   // Folder Fields
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (!folderSupport[i]) {
         for (j=0;j<numOfValue[i];j++) {
            if (!isNumber(valueType[i][j].Data()))
               continue;
            if (folderArray[i]=="1") {
               if (valueArray[i][j][0]=="1") {
                  buffer.AppendFormatted("      real function Get%s_Get%s\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%s_Get%s_C\n     &      [C, ALIAS:'_Get%s_Get%s_C'] ()\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%s_Get%s = Get%s_Get%s_C()\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%s_Set%s(value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
               else {
                  buffer.AppendFormatted("      real function Get%s_Get%sAt(j)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%s_Get%sAt_C\n     &      [C, ALIAS:'_Get%s_Get%sAt_C'] (j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("            integer j[value]\n");
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%s_Get%sAt = Get%s_Get%sAt_C(j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%s_Set%sAt(j,value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
            }
            else {
               if (valueArray[i][j][0]=="1") {
                  buffer.AppendFormatted("      real function Get%sAt_Get%s(i)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%sAt_Get%s_C\n     &      [C, ALIAS:'_Get%sAt_Get%s_C'] (i)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("            integer i[value]\n");
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%sAt_Get%s = Get%sAt_Get%s_C(i)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%sAt_Set%s(i,value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
               else {
                  buffer.AppendFormatted("      real function Get%sAt_Get%sAt(i,j)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("         interface to real function\n     &      Get%sAt_Get%sAt_C\n     &      [C, ALIAS:'_Get%sAt_Get%sAt_C'] (i,j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("            integer i[value]\n");
                  buffer.AppendFormatted("            integer j[value]\n");
                  buffer.AppendFormatted("         end\n");
                  buffer.AppendFormatted("         Get%sAt_Get%sAt = Get%sAt_Get%sAt_C(i,j)\n",folderName[i].Data(),valueName[i][j].Data(),folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      end\n");
                  buffer.AppendFormatted("      subroutine Get%sAt_Set%sAt(i,j,value)\n",folderName[i].Data(),valueName[i][j].Data());
                  buffer.AppendFormatted("      return\n");
                  buffer.AppendFormatted("      end\n");
               }
            }
         }
      }
   }
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(fFile.Data(),buffer.Data(),6);

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
   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TGMsgBox.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ArgusTextDialog.h\"\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"include/tabs/%sT%s.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("#include \"include/generated/%sWindow.h\"\n", shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n", shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("ClassImp(%sWindow);\n", shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("%sWindow::%sWindow() : ArgusWindow()\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   fStatusBarSwitch = kTRUE;\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      Int_t index = tabParentIndex[i];
      ROMEString switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   fTabSwitches.%s = kTRUE;\n", switchString.Data());
   }
   buffer.AppendFormatted("}\n\n");

   buffer.AppendFormatted("%sWindow::%sWindow(const TGWindow* p, char* title) : ArgusWindow(p,title)\n", shortCut.Data(), shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i=0;\n");
   buffer.AppendFormatted("   fStatusBarSwitch = kTRUE;\n");
   buffer.AppendFormatted("\n");
   if (numOfTab>0)
      buffer.AppendFormatted("   fTabObjects = new TObjArray(%d);\n",numOfTab);
   else
      buffer.AppendFormatted("   fTabObjects = NULL;\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      Int_t index = tabParentIndex[i];
      ROMEString switchString = tabName[i].Data();
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      buffer.AppendFormatted("   fTabSwitches.%s = kTRUE;\n", switchString.Data());

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

   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   f%s%03dTab = new %sT%s();\n", tabName[i].Data(), i, shortCut.Data(), tabName[i].Data());
      buffer.AppendFormatted("   AddTab(f%s%03dTab);\n",tabName[i].Data(),i);
   }
   buffer.AppendFormatted("}\n\n");
   buffer.AppendFormatted("\n");

   // CreateTabs
   buffer.AppendFormatted("Bool_t %sWindow::CreateTabs()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   Int_t tabID = 0;\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      recursiveTabDepth = 0;
      if (!AddTab(buffer, i))
         return kFALSE;
   }
   buffer.AppendFormatted("   return kTRUE;\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // ProcessMessage
   buffer.AppendFormatted("Bool_t %sWindow::ProcessMessage(Long_t msg, Long_t param1, Long_t param2)\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i=0;\n");
   buffer.AppendFormatted("   // Process messages coming from widgets associated with the dialog.  \n");
   buffer.AppendFormatted("   switch (GET_MSG(msg)) {\n");
   buffer.AppendFormatted("   case kC_COMMAND:    \n");
   buffer.AppendFormatted("      switch (GET_SUBMSG(msg)) {\n");
   buffer.AppendFormatted("      case kCM_MENU:\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         if (fCurrentTabID == f%sTabID) {\n", tabName[i].Data());
      if (tabHeredity[i].Length()>0) {
         if (numOfMenu[tabHeredityIndex[i]] > 0) {
            buffer.AppendFormatted("            for (i=%d;i<%d+%d;i++) {\n", numOfMenu[i],numOfMenu[i],numOfMenu[tabHeredityIndex[i]]);
            buffer.AppendFormatted("               if (f%sMenu[i]->GetEntry(param1)!=0)\n", tabName[i].Data());
            buffer.AppendFormatted("                  f%s%03dTab->BaseMenuClicked(f%sMenu[i],param1);\n", tabName[i].Data(), i, tabName[i].Data());
            buffer.AppendFormatted("            }\n");
         }
      }
      if (tabHistoDisplay[i]) {
         buffer.AppendFormatted("            f%s%03dTab->BaseMenuClicked(NULL,param1);\n", tabName[i].Data(), i);
      }
      else if (numOfMenu[i] > 0) {
         buffer.AppendFormatted("            for (i=0;i<%d;i++) {\n", numOfMenu[i]);
         buffer.AppendFormatted("               if (f%sMenu[i]->GetEntry(param1)!=0)\n", tabName[i].Data());
         buffer.AppendFormatted("                  f%s%03dTab->BaseMenuClicked(f%sMenu[i],param1);\n", tabName[i].Data(), i, tabName[i].Data());
         buffer.AppendFormatted("            }\n");
      }
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         switch (param1) {\n");
   buffer.AppendFormatted("         case M_FILE_EXIT:\n");
   buffer.AppendFormatted("            CloseWindow();\n");
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
      buffer.AppendFormatted("         if (fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->BaseTabUnSelected();\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID != f%sTabID && param1 == f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->BaseTabSelected();\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         if (fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
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
      buffer.AppendFormatted("         if (param1 == f%sTabID) {\n", tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->SetActive(kTRUE);\n", tabName[i].Data(), i);
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
         buffer.AppendFormatted(" param1 != f%sTabID &&", tabName[index].Data());
         index = tabParentIndex[index];
      } while (index != -1);
      buffer.Remove(buffer.Length() - 2);       // remove the last "&&"
      buffer.AppendFormatted(") {\n");
      buffer.AppendFormatted("            f%s%03dTab->SetActive(kFALSE);\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID == f%sTabID && param1 != f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("            f%s%03dTab->TabUnSelected();\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("         // %s\n", tabName[i].Data());
      buffer.AppendFormatted("         if (fCurrentTabID != f%sTabID && param1 == f%sTabID) {\n", tabName[i].Data(), tabName[i].Data());
      buffer.AppendFormatted("            fCurrentTabID = param1;\n");
      buffer.AppendFormatted("            f%s%03dTab->TabSelected();\n", tabName[i].Data(), i);
      buffer.AppendFormatted("         }\n");
   }
   buffer.AppendFormatted("         fCurrentTabID = param1;\n");

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
      buffer.AppendFormatted("   if (fCurrentTabID == f%sTabID) {\n", tabName[i].Data());
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
   buffer.AppendFormatted("   cout<<\"Error: A menu (\"<<menuName<<\") was not found.\"<<endl;\n");
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
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Event Handler
   buffer.AppendFormatted("// Event Handler\n");
   buffer.AppendFormatted("void %sWindow::TriggerEventHandler()\n", shortCut.Data());
   buffer.AppendFormatted("{\n");
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("   f%s%03dTab->BaseEventHandler();\n", tabName[i].Data(), i);
   }
   buffer.AppendFormatted("}\n");
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
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      buffer.AppendFormatted("#include \"include/generated/%sT%s_Base.h\"\n", shortCut.Data(), tabName[i].Data());
   }
   buffer.AppendFormatted("#include \"ArgusWindow.h\"\n");
   buffer.AppendFormatted("\n");

   // Tab Switches Structure
   buffer.AppendFormatted("// Tab Switches Structure\n");
   buffer.AppendFormatted("typedef struct{\n");

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
      format.SetFormatted("   Bool_t %%s;%%%ds  //! %%s Tab\n", switchLen - switchString.Length());
      buffer.AppendFormatted(const_cast<char*>(format.Data()), switchString.Data(), "", switchString.Data());
//      buffer.AppendFormatted("   Int_t %s;   //! %s Tab\n",switchString.Data(),switchString.Data());
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
   buffer.AppendFormatted("      M_ROOT = 1000,\n");
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
         buffer.AppendFormatted("   TGTab               *f%s%03dTabSubTab;\n", tabName[i].Data(), i);
   }
   for (i = 0; i < numOfTab; i++) {
      if (!tabUsed[i])
         continue;
      int numMenu = numOfMenu[i];
      if (tabHeredity[i].Length()>0)
         numMenu += numOfMenu[tabHeredityIndex[i]];
      if (numMenu > 0)
         buffer.AppendFormatted("   TGPopupMenu         *f%sMenu[%d];\n", tabName[i].Data(), numMenu);
      buffer.AppendFormatted("   Int_t               f%sTabID;\n", tabName[i].Data());
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
      format.SetFormatted("   %%sT%%s_Base*%%%ds f%%s%%03dTab;%%%ds  // Handle to %%s Tab\n", (int)(typeLen - tabName[i].Length() - shortCut.Length() - strlen("T*_Base")), (int)(nameLen - tabName[i].Length() - strlen("f000Tab")));
      buffer.AppendFormatted(const_cast<char*>(format.Data()), shortCut.Data(), tabName[i].Data(), "", tabName[i].Data(), i, "", tabName[i].Data());
      format.SetFormatted("   TGCompositeFrame*%%%ds t%%sT%%s;%%%ds  // Container of %%s Tab\n", (int)(typeLen - strlen("TGCompositeFrame*")), (int)(nameLen - tabName[i].Length() - scl - strlen("tT")));
      buffer.AppendFormatted(const_cast<char*>(format.Data()), "", shortCut.Data(), tabName[i].Data(), "", tabName[i].Data());
   }
   buffer.AppendFormatted("\n");

   // Method
   buffer.AppendFormatted("public:\n");
   buffer.AppendFormatted("   %sWindow();\n", shortCut.Data());
   buffer.AppendFormatted("   %sWindow(const TGWindow *p, char *title);\n", shortCut.Data());
   buffer.AppendFormatted("   ~%sWindow() {};\n", shortCut.Data());
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
      format.SetFormatted("   %%sT%%s_Base*%%%ds Get%%s%%03dTab()%%%ds { return f%%s%%03dTab;%%%ds }\n", (int)(typeLen - tabName[i].Length() - shortCut.Length() - strlen("T*_Base")), (int)(nameLen - tabName[i].Length() - strlen("Get000Tab()")), (int)(nameLen - tabName[i].Length() - strlen("f000Tab")));
      buffer.AppendFormatted(const_cast<char*>(format.Data()), shortCut.Data(), tabName[i].Data(), "", tabName[i].Data(), i, "", tabName[i].Data(), i, "");
   }
   buffer.AppendFormatted("   Bool_t       ProcessMessage(Long_t msg, Long_t param1, Long_t param2);\n");
   buffer.AppendFormatted("   TGPopupMenu* GetMenuHandle(const char* menuName);\n");
   buffer.AppendFormatted("\n");

   // Event Handler
   buffer.AppendFormatted("   // Event Handler\n");
   buffer.AppendFormatted("   void TriggerEventHandler();\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("   ClassDef(%sWindow,1)\n", shortCut.Data());
   buffer.AppendFormatted("};\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("#endif\n");

   // Write File
   WriteFile(hFile.Data(), buffer.Data(), 6);
   return kTRUE;
}

Bool_t ROMEBuilder::AddTab(ROMEString &buffer, Int_t &i)
{
   Int_t j;
   ROMEString parentt;
   ROMEString format;
   Int_t index = tabParentIndex[i];

   ROMEString switchString = tabName[i].Data();
   Int_t depth;
   while (index != -1) {
      switchString.Insert(0, "_");
      switchString.Insert(0, tabName[index].Data());
      index = tabParentIndex[index];
   }
   if (tabParentIndex[i] == -1)
      parentt = "fTab";
   else
      parentt.SetFormatted("f%s%03dTabSubTab", tabName[tabParentIndex[i]].Data(), tabParentIndex[i]);

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("   if (fTabSwitches.%s) {\n", switchString.Data());

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      t%sT%s = %s->AddTab(\"%s\");\n", shortCut.Data(), tabName[i].Data(), parentt.Data(), tabTitle[i].Data());

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->ReparentWindow(t%sT%s, 60, 20);\n", tabName[i].Data(), i, shortCut.Data(), tabName[i].Data());

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->ArgusInit();\n", tabName[i].Data(), i);

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      f%s%03dTab->SetTitle(\"%s\");\n", tabName[i].Data(), i,tabTitle[i].Data());

   if (!tabNumOfChildren[i]) {
      for (depth = 0; depth < recursiveTabDepth; depth++)
         buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTab,new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY , 0, 0, 0, 0));\n", shortCut.Data(), tabName[i].Data(), tabName[i].Data(), i);
   }

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";

   buffer.AppendFormatted("      f%sTabID = tabID++;\n", tabName[i].Data());

   if (tabNumOfChildren[i]) {
      for (depth = 0; depth < recursiveTabDepth; depth++)
         buffer += "   ";
      buffer.AppendFormatted("      f%s%03dTabSubTab = new TGTab(t%sT%s, (UInt_t)(600*GetWindowScale()), (UInt_t)(400*GetWindowScale()));\n", tabName[i].Data(), i, shortCut.Data(), tabName[i].Data());
   }
   recursiveTabDepth++;
   j = i;

   while (i < j + tabNumOfChildren[j]) {
      i++;
      if (!AddTab(buffer, i))
         return kFALSE;
   }

   recursiveTabDepth--;

   if (tabNumOfChildren[j]) {
      for (depth = 0; depth < recursiveTabDepth; depth++)
         buffer += "   ";
      buffer.AppendFormatted("      t%sT%s->AddFrame(f%s%03dTabSubTab, new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 1, 1));\n", shortCut.Data(), tabName[j].Data(), tabName[j].Data(), j);
   }

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("   }\n");

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("   else\n");

   for (depth = 0; depth < recursiveTabDepth; depth++)
      buffer += "   ";
   buffer.AppendFormatted("      f%sTabID = -1;\n", tabName[i].Data());

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
   buffer.AppendFormatted("#include \"include/generated/%sConfigToForm.h\"\n",shortCut.Data());
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"include/generated/%sGlobalSteering.h\"\n",shortCut.Data());
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskUserCode[i])
         buffer.AppendFormatted("#include \"include/tasks/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
      else
         buffer.AppendFormatted("#include \"include/generated/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
   }
   buffer.AppendFormatted("#include \"include/generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sConfig.h\"\n",shortCut.Data());

   buffer.AppendFormatted("\nClassImp(%sConfigToForm)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfigToForm::%sConfigToForm():ROMEConfigToForm()\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("void %sConfigToForm::AddTabs(XMLToFormFrame *frame)\n",shortCut.Data());
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int ind = 0;\n");
   buffer.AppendFormatted("   bool writeFlag = true;\n");
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   ROMEString subStr = \"\";\n");
   buffer.AppendFormatted("   ROMEString path;\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   ROMEStrArray entries;\n");
   buffer.AppendFormatted("   int nTabs=0;\n");
   buffer.AppendFormatted("   int i=0;\n");
   buffer.AppendFormatted("\n");
   WriteConfigToFormTabs(buffer,mainParGroup,"frame","","");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}
Bool_t ROMEBuilder::WriteConfigToFormTabs(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString pointer,ROMEString tabPointer,ROMEString configPointer)
{
   int i,j;
   ROMEString newPointer;
   ROMEString newConfigPointer;
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("   // %s%s\n",tabPointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         buffer.AppendFormatted("   path.SetFormatted(\"%%s/%s\",%s->GetFramePath().Data());\n",parGroup->GetParameterAt(i)->GetName().Data(),pointer.Data());
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfWriteLines();j++) {
            buffer.AppendFormatted("   %s\n",parGroup->GetParameterAt(i)->GetWriteLineAt(j));
         }
         buffer.AppendFormatted("   entries.RemoveAll();\n");
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfComboBoxEntries();j++)
            buffer.AppendFormatted("   entries.AddLast(\"%s\");\n",parGroup->GetParameterAt(i)->GetComboBoxEntryAt(j));
         buffer.AppendFormatted("   %s->AddElement(new XMLToFormElement(\"%s\",\"%s\",writeString.Data(),path.Data(),0,&entries));\n",pointer.Data(),parGroup->GetParameterAt(i)->GetWidgetType().Data(),parGroup->GetParameterAt(i)->GetName().Data());
      }
   }

   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      buffer.AppendFormatted("   // %s%s\n",tabPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      buffer.AppendFormatted("   %s->AddSubFrame(new XMLToFormFrame(\"%s\",%s->GetFramePath().Data(),true,true,true,%d));\n",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),pointer.Data(),i);
      newConfigPointer.SetFormatted("%sf%s->",configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
         newPointer.SetFormatted("%s->GetSubFrameAt(%d)",pointer.Data(),i);
         WriteConfigToFormTabs(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         newPointer.SetFormatted("%s->GetSubFrameAt(%d)->GetSubFrameAt(i)",pointer.Data(),i);
         buffer.AppendFormatted("   for (i=0;i<((%sConfig*)gAnalyzer->GetConfiguration())->fConfigData[0]->%sf%sArraySize;i++) {\n",shortCut.Data(),configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("      str.SetFormatted(\"%s %%d\",i);\n",parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("      %s->GetSubFrameAt(%d)->AddSubFrame(new XMLToFormFrame(str.Data(),\"\",true,false,true,0));\n",pointer.Data(),i);
         WriteConfigToFormTabs(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data());
         buffer.AppendFormatted("   }\n");
      }
   }

   return true;
}
Bool_t ROMEBuilder::WriteConfigToFormSave(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,ROMEString pointer,ROMEString tabPointer,ROMEString configPointer)
{
   int i,j;
   ROMEString newPointer;
   ROMEString newConfigPointer;
   ROMEString temp;
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("      // %s%s\n",tabPointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         if (parGroup->GetArraySize()=="unknown") {
            buffer.AppendFormatted("      str.SetFormatted(\"%s %%d/%s\",i);\n",pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
            buffer.AppendFormatted("      str = dialog->GetValue(str.Data());\n");
         }
         else
            buffer.AppendFormatted("      str = dialog->GetValue(\"%s%s\");\n",pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         if (!parGroup->GetParameterAt(i)->IsWriteLinesAlways()) {
            for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfWriteLines();j++) {
               buffer.AppendFormatted("      %s\n",parGroup->GetParameterAt(i)->GetWriteLineAt(j));
            }
            buffer.AppendFormatted("      if (str!=writeString) {\n");
            buffer.AppendFormatted("         ((%sConfig*)fConfiguration)->fConfigData[0]->%sf%sModified = true;\n",shortCut.Data(),configPointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
            buffer.AppendFormatted("         ((%sConfig*)fConfiguration)->fConfigData[0]->%sf%s = str;\n",shortCut.Data(),configPointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
            buffer.AppendFormatted("      }\n");
         }
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfSetLines();j++) {
            temp = parGroup->GetParameterAt(i)->GetSetLineAt(j);
            temp.ReplaceAll("##","str");
            temp.ReplaceAll("fConfigData[index]","(("+shortCut+"Config*)fConfiguration)->fConfigData[0]");
            buffer.AppendFormatted("      %s\n",temp.Data());
         }
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      newConfigPointer.SetFormatted("%sf%s->",configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
         newPointer.SetFormatted("%s%s/",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         WriteConfigToFormSave(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("   for (i=0;i<((%sConfig*)gAnalyzer->GetConfiguration())->fConfigData[0]->%sf%sArraySize;i++) {\n",shortCut.Data(),configPointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         newPointer.SetFormatted("%s%s/%s",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         WriteConfigToFormSave(buffer,parGroup->GetSubGroupAt(i),newPointer.Data(),tabPointer+parGroup->GetSubGroupAt(i)->GetGroupName().Data()+"/",newConfigPointer.Data());
         buffer.AppendFormatted("   }\n");
      }
   }

   return true;
}
Bool_t ROMEBuilder::WriteConfigToFormH() {
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

   buffer.AppendFormatted("#include \"ROMEConfigToForm.h\"\n");

   // Class
   buffer.AppendFormatted("\nclass %sConfigToForm : public ROMEConfigToForm\n",shortCut.Data());
   buffer.AppendFormatted("{\n");

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sConfigToForm();\n",shortCut.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   void AddTabs(XMLToFormFrame *frame);\n");

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
   ROMEString pointer;
   ROMEString path;
   ROMEString clsName;
   ROMEString clsDescription;

   ROMEString cppFile;
   ROMEString buffer;

   ROMEString format;

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

   buffer.AppendFormatted("#include \"include/generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXMLDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMETextDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOfflineDataBase.h\"\n");
   buffer.AppendFormatted("#include \"ROMEODBOnlineDataBase.h\"\n");
   if (this->sql)
      buffer.AppendFormatted("#include \"ROMESQLDataBase.h\"\n");
   for (i=0;i<numOfDB;i++)
      buffer.AppendFormatted("#include \"include/databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[i].Data());

   buffer.AppendFormatted("#include \"include/generated/%sMidasDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sRomeDAQ.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEDataBaseDAQ.h\"\n");
   if (this->orca)
      buffer.AppendFormatted("#include \"ROMEOrcaDAQ.h\"\n");
   buffer.AppendFormatted("#include \"ROMENoDAQSystem.h\"\n");
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      buffer.AppendFormatted("#include \"include/daqs/%s%s.h\"\n",shortCut.Data(),daqName[i].Data());
   }

   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      if (taskUserCode[i])
         buffer.AppendFormatted("#include \"include/tasks/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
      else
         buffer.AppendFormatted("#include \"include/generated/%sT%s.h\"\n",shortCut.Data(),taskName[i].Data());
   }

   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sConfig.h\"\n",shortCut.Data());

   buffer.AppendFormatted("\nClassImp(%sConfig)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sConfig::%sConfig() {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   fConfigData = new ConfigData*[1];\n");
   buffer.AppendFormatted("   fConfigData[0] = new ConfigData();\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = 0;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Configuration File
   buffer.AppendFormatted("\n// Read Configuration File\n");
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfigurationFile(const char *file) {\n",shortCut.Data());
   buffer.AppendFormatted("   fXMLFile = file;\n");
   buffer.AppendFormatted("   ROMEXML *xml = new ROMEXML();\n");
   buffer.AppendFormatted("   xml->OpenFileForPath(fXMLFile);\n");
   buffer.AppendFormatted("   xml->GetPathAttribute(\"/Configuration\",\"xsi:noNamespaceSchemaLocation\",fXSDFile,\"romeConfig.xsd\");\n");
   buffer.AppendFormatted("   if (!ReadProgramConfiguration(xml))\n");
   buffer.AppendFormatted("      return false;\n");
   buffer.AppendFormatted("   fNumberOfRunConfigs = xml->NumberOfOccurrenceOfPath(\"/Configuration/RunConfiguration\");\n");
   buffer.AppendFormatted("   delete [] fConfigData;\n");
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
   buffer.AppendFormatted("   if (fConfigData[0]->fRunParameters->fRunNumbersModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetRunNumbers(fConfigData[0]->fRunParameters->fRunNumbers);\n");
   buffer.AppendFormatted("   if (fConfigData[0]->fRunParameters->fInputFileNamesModified)\n");
   buffer.AppendFormatted("      gAnalyzer->SetInputFileNames(fConfigData[0]->fRunParameters->fInputFileNames);\n");

   buffer.AppendFormatted("   delete xml;\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Read Program Configuration
   buffer.AppendFormatted("\n// Read Program Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::ReadProgramConfiguration(ROMEXML *xml) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString romeBuffer;\n");
   buffer.AppendFormatted("   ROMEString argusBuffer;\n");
   buffer.AppendFormatted("   xml->GetPathValue(\"/Configuration/ProgramConfiguration/ROME/Active\",romeBuffer,\"\");\n");
   buffer.AppendFormatted("   xml->GetPathValue(\"/Configuration/ProgramConfiguration/ARGUS/Active\",argusBuffer,\"\");\n");
   buffer.AppendFormatted("   romeBuffer.ToLower();\n");
   buffer.AppendFormatted("   argusBuffer.ToLower();\n");
   buffer.AppendFormatted("   if (!gAnalyzer->GetWindow()->IsActive())\n");
   buffer.AppendFormatted("      gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("   else {\n");
   buffer.AppendFormatted("      if (romeBuffer==\"false\" && argusBuffer!=\"true\")\n");
   buffer.AppendFormatted("         return false;\n");
   buffer.AppendFormatted("      if (romeBuffer!=\"false\" && argusBuffer!=\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetStandAloneROME();\n");
   buffer.AppendFormatted("      if (romeBuffer==\"false\" && argusBuffer==\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetStandAloneARGUS();\n");
   buffer.AppendFormatted("      if (romeBuffer!=\"false\" && argusBuffer==\"true\")\n");
   buffer.AppendFormatted("         gAnalyzer->SetROMEAndARGUS();\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");


   // Read Configuration
   buffer.AppendFormatted("\n// Read Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::ReadConfiguration(ROMEXML *xml,ROMEString& path,Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   ROMEString tempPath;\n");
   for (i=0;i<maxConfigParameterHierarchyLevel+1;i++) {
      buffer.AppendFormatted("   int i%d = 0;\n",i);
   }
   WriteConfigRead(buffer,mainParGroup,1,"","","","");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Check Configuration Modified
   buffer.AppendFormatted("\n// Check Configuration Modified\n");
   buffer.AppendFormatted("Bool_t %sConfig::CheckConfigurationModified(Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   ROMEString tempPath;\n");
   for (i=0;i<maxConfigParameterHierarchyLevel+1;i++) {
      buffer.AppendFormatted("   int i%d = 0;\n",i);
   }
   WriteConfigCheckModified(buffer,mainParGroup,1,"","","","");
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
   buffer.AppendFormatted("      if (fConfigData[i]->fRunParameters->fRunNumbersModified) {\n");
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
   buffer.AppendFormatted("      if (fConfigData[i]->fRunParameters->fInputFileNamesModified) {\n");
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
   for (i=0;i<maxConfigParameterHierarchyLevel+1;i++) {
      buffer.AppendFormatted("   int i%d = 0;\n",i);
   }
   buffer.AppendFormatted("   char* cstop=NULL;\n");
   buffer.AppendFormatted("   ROMEString path = \"\";\n");
   buffer.AppendFormatted("   ROMEString subPath = \"\";\n");
   buffer.AppendFormatted("   int ind = 0;\n");
   buffer.AppendFormatted("   fActiveConfiguration = index;\n");
   WriteConfigSet(buffer,mainParGroup,1,"","");
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
   buffer.AppendFormatted("   xml->StartElement(\"Configuration\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xmlns:xsi\",\"http://www.w3.org/2001/XMLSchema-instance\");\n");
   buffer.AppendFormatted("   xml->WriteAttribute(\"xsi:noNamespaceSchemaLocation\",fXSDFile.Data());\n");
   buffer.AppendFormatted("   WriteProgramConfiguration(xml);\n");
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

   // Write Program Configuration
   buffer.AppendFormatted("\n// Write Program Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteProgramConfiguration(ROMEXML *xml) {\n",shortCut.Data());
   buffer.AppendFormatted("   xml->StartElement(\"ProgramConfiguration\");\n");
   buffer.AppendFormatted("   xml->StartElement(\"ROME\");\n");
   buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneROME() || gAnalyzer->IsROMEAndARGUS())\n");
   buffer.AppendFormatted("      xml->WriteElement(\"Active\",\"true\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteElement(\"Active\",\"false\");\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   xml->StartElement(\"ARGUS\");\n");
   buffer.AppendFormatted("   if (gAnalyzer->IsStandAloneARGUS() || gAnalyzer->IsROMEAndARGUS())\n");
   buffer.AppendFormatted("      xml->WriteElement(\"Active\",\"true\");\n");
   buffer.AppendFormatted("   else\n");
   buffer.AppendFormatted("      xml->WriteElement(\"Active\",\"false\");\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   xml->EndElement();\n");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");


   // Write Configuration
   buffer.AppendFormatted("\n// Write Configuration\n");
   buffer.AppendFormatted("Bool_t %sConfig::WriteConfiguration(ROMEXML *xml,Int_t index) {\n",shortCut.Data());
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   ROMEString writeString;\n");
   buffer.AppendFormatted("   int i;\n");
   WriteConfigWrite(buffer,mainParGroup,1,"","");
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n");

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
   buffer.AppendFormatted("#include \"TArrayI.h\"\n");
   buffer.AppendFormatted("#include \"TArrayL.h\"\n");
   buffer.AppendFormatted("#include \"ROMEXML.h\"\n");
   buffer.AppendFormatted("#endif\n");
   if (readGlobalSteeringParameters)
      buffer.AppendFormatted("#include \"include/generated/%sGlobalSteering.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"ROMEConfig.h\"\n");

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
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   ROMEConfigParameterGroup* subSubSubGroup;
   for (i=0;i<nIndex;i++)
      index[i] = -1;
   maxConfigParameterHierarchyLevel = 0;

   mainParGroup = new ROMEConfigParameterGroup("ConfigData");
   // RunParameters
   subGroup = new ROMEConfigParameterGroup("RunParameters");
   // RunNumbers
   subGroup->AddParameter(new ROMEConfigParameter("RunNumbers"));
   subGroup->GetLastParameter()->AddReadModifiedTrueLine("fConfigData[index]->fLastRunNumberIndex = 0;");
   subGroup->GetLastParameter()->AddReadModifiedTrueLine("gAnalyzer->DecodeNumbers(fConfigData[index]->fRunParameters->fRunNumbers,fConfigData[index]->fRunNumberArray);");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetRunNumberStringOriginal();");
   // EventNumbers
   subGroup->AddParameter(new ROMEConfigParameter("EventNumbers"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetEventNumbers(##);");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetEventNumberStringOriginal();");
   // InputFileNames
   subGroup->AddParameter(new ROMEConfigParameter("InputFileNames"));
   subGroup->GetLastParameter()->AddReadModifiedTrueLine("fConfigData[index]->fLastInputFileNameIndex = 0;");
   subGroup->GetLastParameter()->AddReadModifiedTrueLine("gAnalyzer->DecodeInputFileNames(fConfigData[index]->fRunParameters->fInputFileNames,fConfigData[index]->fInputFileNameArray);");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetInputFileNamesStringOriginal();");
   mainParGroup->AddSubGroup(subGroup);
   // Modes
   subGroup = new ROMEConfigParameterGroup("Modes");
   // Modes/AnalyzingMode
   subGroup->AddParameter(new ROMEConfigParameter("AnalyzingMode","1","ComboBox"));
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
   subGroup->GetLastParameter()->AddSetLine("if (!##.CompareTo(\"midas\",TString::kIgnoreCase)) {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetMidasDAQ(new %sMidasDAQ());",shortCut.Data());
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->GetMidasDAQ());");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"rome\",TString::kIgnoreCase)) {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetRomeDAQ(new %sRomeDAQ());",shortCut.Data());
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->GetRomeDAQ());");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"database\",TString::kIgnoreCase)) {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBaseDAQ(new ROMEDataBaseDAQ());");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->GetDataBaseDAQ());");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddComboBoxEntry("none");
   subGroup->GetLastParameter()->AddComboBoxEntry("rome");
   subGroup->GetLastParameter()->AddComboBoxEntry("midas");
   subGroup->GetLastParameter()->AddComboBoxEntry("database");
   if (this->orca) {
      subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"orca\",TString::kIgnoreCase)) {");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetOrcaDAQ(new ROMEOrcaDAQ());");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->GetOrcaDAQ());");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddComboBoxEntry("orca");
   }
   subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"none\",TString::kIgnoreCase) || ##.Length()==0) {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(new ROMENoDAQSystem());");
   subGroup->GetLastParameter()->AddSetLine("}");
   for (i=0;i<numOfDAQ;i++) {
      if (!daqUsed[i])
         continue;
      subGroup->GetLastParameter()->AddSetLine("else if (!##.CompareTo(\"%s\",TString::kIgnoreCase)) {",daqName[i].Data());
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->Set%s(new %s%s());",daqName[i].Data(),shortCut.Data(),daqName[i].Data());
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetActiveDAQ(gAnalyzer->Get%s());",daqName[i].Data());
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddComboBoxEntry(daqName[i].Data());
   }
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isActiveDAQSet())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetActiveDAQ()->GetName();");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"none\";");
   // Modes/BatchMode
   subGroup->AddParameter(new ROMEConfigParameter("BatchMode","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetBatchMode(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetBatchMode(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isBatchMode())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   // Modes/QuitMode
   subGroup->AddParameter(new ROMEConfigParameter("QuitMode","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetQuitMode(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetQuitMode(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isQuitMode())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   // Modes/VerboseMode
   subGroup->AddParameter(new ROMEConfigParameter("VerboseMode","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetVerboseMode(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetVerboseMode(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isVerboseMode())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   // Modes/ShowSplashScreen
   subGroup->AddParameter(new ROMEConfigParameter("ShowSplashScreen","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSplashScreen(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSplashScreen(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isSplashScreen())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   // Modes/GraphicalConfigEdit
   subGroup->AddParameter(new ROMEConfigParameter("GraphicalConfigEdit","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetGraphicalConfigEdit(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetGraphicalConfigEdit(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isGraphicalConfigEdit())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   mainParGroup->AddSubGroup(subGroup);
   // Argus
   if (numOfTab>0) {
      subGroup = new ROMEConfigParameterGroup("Argus");
      // Argus/WindowScale
      subGroup->AddParameter(new ROMEConfigParameter("WindowScale"));
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetWindowScale(##.ToFloat());");
      subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%2.1f\",gAnalyzer->GetWindow()->GetWindowScale());");
      // Argus/StatusBar
      subGroup->AddParameter(new ROMEConfigParameter("StatusBar","1","CheckButton"));
      subGroup->GetLastParameter()->AddSetLine("if (##==\"false\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetStatusBarSwitch(kFALSE);");
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetStatusBarSwitch(kTRUE);");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetStatusBarSwitch())");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      // Argus/UpdateFrequency
      subGroup->AddParameter(new ROMEConfigParameter("UpdateFrequency"));
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetUpdateFrequency(##.ToInteger());");
      subGroup->GetLastParameter()->AddSetLine("if (gAnalyzer->GetWindow()->IsActive()) {");
      for (i=0;i<numOfTab;i++) {
         if (!tabUsed[i])
            continue;
         subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->Get%s%03dTab()->SetUpdateFrequency(##.ToInteger());", tabName[i].Data(), i);
      }
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetWindow()->GetUpdateFrequency());");
      mainParGroup->AddSubGroup(subGroup);
      // Argus/AnalyzerController
      subSubGroup = new ROMEConfigParameterGroup("AnalyzerController");
      // Argus/AnalyzerController/Active
      subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetControllerActive(kTRUE);");
      subSubGroup->GetLastParameter()->AddSetLine("else");
      subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->SetControllerActive(kFALSE);");
      subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->IsControllerActive())");
      subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subSubGroup->GetLastParameter()->AddWriteLine("else");
      subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      // Argus/AnalyzerController/NetFolderName
      subSubGroup->AddParameter(new ROMEConfigParameter("NetFolderName"));
      subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetWindow()->SetControllerNetFolder(##.Data());");
      subSubGroup->GetLastParameter()->AddWriteLine("writeString = \"\";");
      subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetControllerNetFolder()!=NULL)");
      subSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->GetWindow()->GetControllerNetFolder()->GetName();");
      subGroup->AddSubGroup(subSubGroup);
      // Argus/SocketToROME
      subSubGroup = new ROMEConfigParameterGroup("SocketToROME");
      // Argus/SocketToROME/Active
      subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSocketToROMEActive(kTRUE);");
      subSubGroup->GetLastParameter()->AddSetLine("else");
      subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSocketToROMEActive(kFALSE);");
      subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsSocketToROMEActive())");
      subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subSubGroup->GetLastParameter()->AddWriteLine("else");
      subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      // Argus/SocketToROME/Host
      subSubGroup->AddParameter(new ROMEConfigParameter("Host"));
      subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketToROMEHost(##.Data());");
      subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetSocketToROMEHost();");
      // Argus/SocketToROME/Port
      subSubGroup->AddParameter(new ROMEConfigParameter("Port"));
      subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetSocketToROMEPort(##.Data());");
      subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetSocketToROMEPort());");
      subGroup->AddSubGroup(subSubGroup);
   }
   // DataBase
   subGroup = new ROMEConfigParameterGroup("DataBase","unknown");
   subGroup->AddReadGroupArrayInitLine("gAnalyzer->InitDataBases(fConfigData[index]->fDataBaseArraySize);");
   // DataBase/Name
   subGroup->AddParameter(new ROMEConfigParameter("Name"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetDataBaseName(i,##.Data());");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBaseName(i);");
   // DataBase/Connection
   subGroup->AddParameter(new ROMEConfigParameter("Connection"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetDataBaseConnection(i,##.Data());");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBaseConnection(i);");
   // DataBase/Type
   subGroup->AddParameter(new ROMEConfigParameter("Type","1","ComboBox"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"sql\") {");
   if (this->sql) {
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMESQLDataBase());");
      subGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",gAnalyzer->GetDataBaseConnection(i)))");
      subGroup->GetLastParameter()->AddSetLine("      return false;");
   }
   else{
      subGroup->GetLastParameter()->AddSetLine("   cout<<gAnalyzer->GetProgramName()<<\" is not linked with sql support.\"<<endl;");
      subGroup->GetLastParameter()->AddSetLine("   return false;");
   }
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddSetLine("else if (##==\"none\" ||");
   subGroup->GetLastParameter()->AddSetLine("         ##==\"\") {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMENoDataBase());");
   subGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",\"\"))");
   subGroup->GetLastParameter()->AddSetLine("      return false;");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddSetLine("else if (##==\"xml\") {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMEXMLDataBase());");
   subGroup->GetLastParameter()->AddSetLine("   ROMEString str = gAnalyzer->GetDataBaseConnection(i);");
   subGroup->GetLastParameter()->AddSetLine("   int ind;");
   subGroup->GetLastParameter()->AddSetLine("   if ((ind=str.Index(\";\",1,0,TString::kExact))==-1) {");
   subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->PrintLine(\"Invalid database connection\");");
   subGroup->GetLastParameter()->AddSetLine("      return false;");
   subGroup->GetLastParameter()->AddSetLine("   }");
   subGroup->GetLastParameter()->AddSetLine("   ROMEString path = str(0,ind);");
   subGroup->GetLastParameter()->AddSetLine("   if (path[path.Length()-1]!='/' && path[path.Length()-1]!='\\\\')");
   subGroup->GetLastParameter()->AddSetLine("      path += \"/\";");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBaseDir(i,path.Data());");
   subGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),gAnalyzer->GetDataBaseDir(i),((TString)str(ind+1,str.Length()-ind-1)).Data()))");
   subGroup->GetLastParameter()->AddSetLine("      return false;");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddSetLine("else if (##==\"text\") {");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new ROMETextDataBase());");
   subGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),gAnalyzer->GetDataBaseConnection(i),\"\"))");
   subGroup->GetLastParameter()->AddSetLine("      return false;");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddSetLine("if (##==\"odb\") {");
   subGroup->GetLastParameter()->AddSetLine("   if (gAnalyzer->isOffline())");
   subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetDataBase(i,new ROMEODBOfflineDataBase());");
   subGroup->GetLastParameter()->AddSetLine("   else");
   subGroup->GetLastParameter()->AddSetLine("      gAnalyzer->SetDataBase(i,new ROMEODBOnlineDataBase());");
   subGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",\"\"))");
   subGroup->GetLastParameter()->AddSetLine("      return false;");
   subGroup->GetLastParameter()->AddSetLine("}");
   subGroup->GetLastParameter()->AddComboBoxEntry("none");
   subGroup->GetLastParameter()->AddComboBoxEntry("sql");
   subGroup->GetLastParameter()->AddComboBoxEntry("xml");
   subGroup->GetLastParameter()->AddComboBoxEntry("text");
   subGroup->GetLastParameter()->AddComboBoxEntry("odb");
   for (i=0;i<numOfDB;i++) {
      subGroup->GetLastParameter()->AddSetLine("else if (##==\"%s\") {\n",dbName[i].Data());
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetDataBase(i,new %s%sDataBase());\n",shortCut.Data(),dbName[i].Data());
      subGroup->GetLastParameter()->AddSetLine("   if (!gAnalyzer->GetDataBase(i)->Init(gAnalyzer->GetDataBaseName(i),\"\",gAnalyzer->GetDataBaseConnection(i)))");
      subGroup->GetLastParameter()->AddSetLine("      return false;");
      subGroup->GetLastParameter()->AddSetLine("}");
      subGroup->GetLastParameter()->AddComboBoxEntry(dbName[i].Data());
   }
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetDataBase(i)->GetType();");
   // DataBase/EventBased
   subGroup->AddParameter(new ROMEConfigParameter("EventBased","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetEventBasedDataBase(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetEventBasedDataBase(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsEventBasedDataBase())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   mainParGroup->AddSubGroup(subGroup);
   // Online
   subGroup = new ROMEConfigParameterGroup("Online");
   // Online/Host
   subGroup->AddParameter(new ROMEConfigParameter("Host"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineHost(##.Data());");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineHost();");
   // Online/Experiment
   subGroup->AddParameter(new ROMEConfigParameter("Experiment"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOnlineExperiment(##.Data());");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOnlineExperiment();");
   mainParGroup->AddSubGroup(subGroup);
   // SocketInterface
   subGroup = new ROMEConfigParameterGroup("SocketInterface");
   // SocketInterface/PortNumber
   subGroup->AddParameter(new ROMEConfigParameter("PortNumber"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetPortNumber(##.Data());");
   subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetPortNumber());");
   // SocketInterface/AvailableOffline
   subGroup->AddParameter(new ROMEConfigParameter("AvailableOffline","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSocketOffline(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetSocketOffline(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isSocketOffline())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   mainParGroup->AddSubGroup(subGroup);
   // Paths
   subGroup = new ROMEConfigParameterGroup("Paths");
   // Paths/InputFilePath
   subGroup->AddParameter(new ROMEConfigParameter("InputFilePath"));
   subGroup->GetLastParameter()->AddSetLine("if (##[##.Length()-1]!='/' && ##[##.Length()-1]!='\\\\')");
   subGroup->GetLastParameter()->AddSetLine("   ##.Append(\"/\");");
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetInputDir(##);");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetInputDir();");
   // Paths/OutputFilePath
   subGroup->AddParameter(new ROMEConfigParameter("OutputFilePath"));
   subGroup->GetLastParameter()->AddSetLine("if (##[##.Length()-1]!='/' && ##[##.Length()-1]!='\\\\')");
   subGroup->GetLastParameter()->AddSetLine("   ##.Append(\"/\");");
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetOutputDir(##);");
   subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetOutputDir();");
   mainParGroup->AddSubGroup(subGroup);
   // Histograms
   subGroup = new ROMEConfigParameterGroup("Histograms");
   // Histograms/Read
   subGroup->AddParameter(new ROMEConfigParameter("Read","1","CheckButton"));
   subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetHistosRead(true);");
   subGroup->GetLastParameter()->AddSetLine("else");
   subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetHistosRead(false);");
   subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->IsHistosRead())");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
   subGroup->GetLastParameter()->AddWriteLine("else");
   subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
   // Histograms/RunNumber
   subGroup->AddParameter(new ROMEConfigParameter("RunNumber"));
   subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetHistosRun(##.ToInteger());");
   subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetHistosRun());");
   mainParGroup->AddSubGroup(subGroup);
   // Folders
   subGroup = new ROMEConfigParameterGroup("Folders");
   mainParGroup->AddSubGroup(subGroup);
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (folderDataBase[i] && !folderSupport[i]) {
         subSubGroup = new ROMEConfigParameterGroup(folderName[i],"1","Folder");
         subGroup->AddSubGroup(subSubGroup);
         for (j=0;j<numOfValue[i];j++) {
            if (valueDimension[i][j]>1)
               continue;
            subSubSubGroup = new ROMEConfigParameterGroup(valueName[i][j],"1","Field");
            // DataBaseName
            subSubSubGroup->AddParameter(new ROMEConfigParameter("DataBaseName"));
            subSubSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->Set%s_%sDBName(##.Data());",folderName[i].Data(),valueName[i][j].Data());
            subSubSubGroup->GetLastParameter()->DontWriteLinesAlways();
            subSubSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->Get%s_%sDBName();",folderName[i].Data(),valueName[i][j].Data());
            // DataBasePath
            subSubSubGroup->AddParameter(new ROMEConfigParameter("DataBasePath"));
            subSubSubGroup->GetLastParameter()->AddSetLine("path = ##.Data();");
            subSubSubGroup->GetLastParameter()->AddSetLine("ind = path.Last('\"');");
            subSubSubGroup->GetLastParameter()->AddSetLine("if (ind==-1 || ind==path.Length()-1) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   subPath = path.ReplaceAll(\"\\\"\",\"\");");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->Set%s_%sDBPath(subPath.Data());",folderName[i].Data(),valueName[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->AddSetLine("else {");
            subSubSubGroup->GetLastParameter()->AddSetLine("   subPath = path(1,ind-1);");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->Set%s_%sDBPath(subPath.Data());",folderName[i].Data(),valueName[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddSetLine("   path = path(ind+2,path.Length()-ind+2);");
            subSubSubGroup->GetLastParameter()->AddSetLine("   int num = path.CountChar(',')+1;");
            subSubSubGroup->GetLastParameter()->AddSetLine("   for (i=0;i<num;i++) {");
            subSubSubGroup->GetLastParameter()->AddSetLine("      if (i<num-1)");
            subSubSubGroup->GetLastParameter()->AddSetLine("         ind = path.First(',');");
            subSubSubGroup->GetLastParameter()->AddSetLine("      else");
            subSubSubGroup->GetLastParameter()->AddSetLine("         ind = path.Length();");
            subSubSubGroup->GetLastParameter()->AddSetLine("      subPath = path(0,ind);");
            subSubSubGroup->GetLastParameter()->AddSetLine("      gAnalyzer->Set%s_%sDBCodeAt(i,gAnalyzer->GetObjectInterpreterCode(subPath.Data()));",folderName[i].Data(),valueName[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddSetLine("      path = path(ind+1,path.Length()-ind+1);");
            subSubSubGroup->GetLastParameter()->AddSetLine("   }");
            subSubSubGroup->GetLastParameter()->AddSetLine("}");
            subSubSubGroup->GetLastParameter()->DontWriteLinesAlways(); 
            subSubSubGroup->GetLastParameter()->AddWriteLine("str = gAnalyzer->Get%s_%sDBPathOriginal();",folderName[i].Data(),valueName[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddWriteLine("ind = str.Last('\"');");
            subSubSubGroup->GetLastParameter()->AddWriteLine("writeFlag = true;");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (ind>-1 && ind<str.Length()-1) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   str = str(ind+2,str.Length()-ind+2);");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   int num = str.CountChar(',')+1;");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   for (i=0;i<num;i++) {");
            subSubSubGroup->GetLastParameter()->AddWriteLine("      if (i<num-1)");
            subSubSubGroup->GetLastParameter()->AddWriteLine("         ind = str.First(',');");
            subSubSubGroup->GetLastParameter()->AddWriteLine("      else");
            subSubSubGroup->GetLastParameter()->AddWriteLine("         ind = str.Length();");
            subSubSubGroup->GetLastParameter()->AddWriteLine("      subStr = str(0,ind);");
            subSubSubGroup->GetLastParameter()->AddWriteLine("      if (gAnalyzer->GetObjectInterpreterCode(subStr.Data())==-2)");
            subSubSubGroup->GetLastParameter()->AddWriteLine("         writeFlag = false;");
            subSubSubGroup->GetLastParameter()->AddWriteLine("      str = str(ind+1,str.Length()-ind+1);");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   }");
            subSubSubGroup->GetLastParameter()->AddWriteLine("}");
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (writeFlag)");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = gAnalyzer->Get%s_%sDBPathOriginal();",folderName[i].Data(),valueName[i][j].Data());
            subSubSubGroup->GetLastParameter()->AddWriteLine("else");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"\";");
            subSubGroup->AddSubGroup(subSubSubGroup);
         }
      }
   }
   // NetFolder
   for (i=0;i<numOfNetFolder;i++) {
      subGroup = new ROMEConfigParameterGroup(netFolderName[i],"1","NetFolder");
      // NetFolder/Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderActive(%d,kTRUE);",i);
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderActive(%d,kFALSE);",i);
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetNetFolderActive(%d))",i);
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      // NetFolder/Reconnect
      subGroup->AddParameter(new ROMEConfigParameter("Reconnect","1","CheckButton"));
      subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderReconnect(%d,kTRUE);",i);
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetNetFolderReconnect(%d,kFALSE);",i);
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetNetFolderReconnect(%d))",i);
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      // NetFolder/Root
      subGroup->AddParameter(new ROMEConfigParameter("Root"));
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderRoot(%d,const_cast<char*>(##.Data()));",i);
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetNetFolderHost(%d);",i);
      // NetFolder/Host
      subGroup->AddParameter(new ROMEConfigParameter("Host"));
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderHost(%d,const_cast<char*>(##.Data()));",i);
      subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetNetFolderPort(%d));",i);
      // NetFolder/Port
      subGroup->AddParameter(new ROMEConfigParameter("Port"));
      subGroup->GetLastParameter()->AddSetLine("gAnalyzer->SetNetFolderPort(%d,const_cast<char*>(##.Data()));",i);
      subGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetNetFolderRoot(%d);",i);
      mainParGroup->AddSubGroup(subGroup);
   }
   // Tasks
   if (numOfTask>0) {
      subGroup = new ROMEConfigParameterGroup("Tasks");
      mainParGroup->AddSubGroup(subGroup);
      AddTaskConfigParameters(subGroup,-1);
   }
   // Tabs
   if (numOfTab>0) {
      subGroup = new ROMEConfigParameterGroup("Tabs");
      mainParGroup->AddSubGroup(subGroup);
      AddTabConfigParameters(subGroup,-1);
   }
   // Trees
   if (numOfTree>0) {
      subGroup = new ROMEConfigParameterGroup("Trees");
      mainParGroup->AddSubGroup(subGroup);
      // Trees/Accumulate
      subGroup->AddParameter(new ROMEConfigParameter("Accumulate","1","CheckButton"));
      subGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetTreeAccumulation(true);");
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->SetTreeAccumulation(false);");
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->isTreeAccumulation())");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      for (i=0;i<numOfTree;i++) {
         subSubGroup = new ROMEConfigParameterGroup(treeName[i],"1","Tree");
         // Tree/Read
         subSubGroup->AddParameter(new ROMEConfigParameter("Read","1","CheckButton"));
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetRead(true);",i);
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetRead(false);",i);
         subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetTreeObjectAt(%d)->isRead())",i);
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // Tree/Write
         subSubGroup->AddParameter(new ROMEConfigParameter("Write","1","CheckButton"));
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetWrite(true);",i);
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetWrite(false);",i);
         subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetTreeObjectAt(%d)->isWrite())",i);
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // Tree/Fill
         subSubGroup->AddParameter(new ROMEConfigParameter("Fill","1","CheckButton"));
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"true\")");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetFill(true);",i);
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetFill(false);",i);
         subSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetTreeObjectAt(%d)->isFill())",i);
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         // Tree/CompressionLevel
         subSubGroup->AddParameter(new ROMEConfigParameter("CompressionLevel"));
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetTreeObjectAt(%d)->SetCompressionLevel(##.ToInteger());",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());",i);
         // Tree/MaxNumberOfEntries
         subSubGroup->AddParameter(new ROMEConfigParameter("MaxNumberOfEntries"));
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetTreeObjectAt(%d)->SetMaxEntries(((Long_t)##.ToInteger()));",i);
#if defined( R__VISUAL_CPLUSPLUS )
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%I64d\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());",i);
#else
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%lld\",gAnalyzer->GetTreeObjectAt(%d)->GetMaxEntries());",i);
#endif
         // Tree/TreeOutputFileName
         subSubGroup->AddParameter(new ROMEConfigParameter("TreeOutputFileName"));
         subSubGroup->GetLastParameter()->AddSetLine("gAnalyzer->GetTreeObjectAt(%d)->SetConfigFileName(##);",i);
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = gAnalyzer->GetTreeObjectAt(%d)->GetConfigFileName().Data();",i);
         subGroup->AddSubGroup(subSubGroup);
         for (j=0;j<numOfBranch[i];j++) {
            subSubSubGroup = new ROMEConfigParameterGroup(branchName[i][j],"1","Branch");
            //Tree/Branch/Active
            subSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
            subSubSubGroup->GetLastParameter()->AddSetLine("if (##==\"false\")");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetBranchActiveAt(%d, false);",i,j);
            subSubSubGroup->GetLastParameter()->AddSetLine("else");
            subSubSubGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetTreeObjectAt(%d)->SetBranchActiveAt(%d, true);",i,j);
            subSubSubGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d))",i,j);
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
            subSubSubGroup->GetLastParameter()->AddWriteLine("else");
            subSubSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
            subSubGroup->AddSubGroup(subSubSubGroup);
         }
      }
   }
   // Global Steering Parameters
   if (readGlobalSteeringParameters) {
      subGroup = new ROMEConfigParameterGroup("GlobalSteeringParameters");
      mainParGroup->AddSubGroup(subGroup);
      AddSteeringConfigParameters(subGroup,0,numOfTask,"gAnalyzer->GetGSP()");
   }
   // midas banks
   if (numOfEvent>0) {
      subGroup = new ROMEConfigParameterGroup("Midas");
      mainParGroup->AddSubGroup(subGroup);
      subGroup->AddParameter(new ROMEConfigParameter("MidasByteSwap","1","CheckButton"));
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
      subSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
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
      subGroup->AddSubGroup(subSubGroup);
      for (j=0;j<numOfBank[i];j++) {
         subSubSubGroup = new ROMEConfigParameterGroup(bankName[i][j],"1","Bank");
         subSubSubGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
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
Bool_t  ROMEBuilder::AddTaskConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex)
{
   int i,j;
   ROMEString name;
   ROMEString steerPointerT;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      if (taskHierarchyParentIndex[i]!=parentIndex)
         continue;
      name.SetFormatted("%s%03d",taskHierarchyName[i].Data(),i);
      subGroup = new ROMEConfigParameterGroup(name.Data(),"1","Task","TaskName","",taskHierarchyName[i].Data(),taskHierarchyMultiplicity[i]);
      // Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
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
         subSubGroup->AddParameter(new ROMEConfigParameter("HistTitle"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sTitle(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sTitle();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistFolderTitle"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sFolderTitle(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sFolderTitle();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistArraySize"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sArraySize(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sArraySize());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistArrayStartIndex"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sArrayStartIndex(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sArrayStartIndex());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXLabel"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sXLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sXLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYLabel"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sYLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sYLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZLabel"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sZLabel(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString = ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sZLabel();",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXNbins"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sXNbins(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sXNbins());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXmin"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sXmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%f\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sXmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistXmax"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sXmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%f\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sXmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYNbins"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sYNbins(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sYNbins());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYmin"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sYmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%f\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sYmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistYmax"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sYmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%f\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sYmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZNbins"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sZNbins(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%d\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sZNbins());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZmin"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sZmin(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%f\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sZmin());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistZmax"));
         subSubGroup->GetLastParameter()->AddSetLine("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sZmax(##.Data());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->DontWriteLinesAlways();
         subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%%f\",((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Get%sZmax());",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->AddParameter(new ROMEConfigParameter("HistAccumulate","1","CheckButton"));
         subSubGroup->GetLastParameter()->AddSetLine("if (##==\"false\")");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sAccumulation(false);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine("else");
         subSubGroup->GetLastParameter()->AddSetLine("   ((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->Set%sAccumulation(true);",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddSetLine("");
         subSubGroup->GetLastParameter()->AddWriteLine("if (((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->is%sAccumulation())",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i],histoName[taskHierarchyClassIndex[i]][j].Data());
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
         subSubGroup->GetLastParameter()->AddWriteLine("else");
         subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         subGroup->AddSubGroup(subSubGroup);
      }
      if (numOfSteering[taskHierarchyClassIndex[i]]>0) {
         steerPointerT.SetFormatted("((%sT%s*)gAnalyzer->GetTaskObjectAt(%d))->GetSP()",shortCut.Data(),taskName[taskHierarchyClassIndex[i]].Data(),taskHierarchyObjectIndex[i]);
         AddSteeringConfigParameters(subGroup,0,taskHierarchyClassIndex[i],steerPointerT);
      }
      AddTaskConfigParameters(subGroup,i);
   }

   return true;
}
Bool_t  ROMEBuilder::AddTabConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t parentIndex)
{
   int i,index;
   ROMEString switchString;
   ROMEString steerPointerT;
   ROMEConfigParameterGroup* subGroup;
   for (i=0;i<numOfTab;i++) {
      if (!tabUsed[i])
         continue;
      if (tabParentIndex[i] != parentIndex)
         continue;
      switchString = tabName[i].Data();
      index = tabParentIndex[i];
      while (index != -1) {
         switchString.Insert(0, "_");
         switchString.Insert(0, tabName[index].Data());
         index = tabParentIndex[index];
      }
      subGroup = new ROMEConfigParameterGroup(tabName[i],"1","Tab","TabName");
      // Active
      subGroup->AddParameter(new ROMEConfigParameter("Active","1","CheckButton"));
      subGroup->GetLastParameter()->AddSetLine("if (##==\"false\")");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->GetTabSwitches()->%s = kFALSE;",switchString.Data());
      subGroup->GetLastParameter()->AddSetLine("else");
      subGroup->GetLastParameter()->AddSetLine("   gAnalyzer->GetWindow()->GetTabSwitches()->%s = kTRUE;",switchString.Data());
      subGroup->GetLastParameter()->AddWriteLine("if (gAnalyzer->GetWindow()->GetTabSwitches()->%s)",switchString.Data());
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
      subGroup->GetLastParameter()->AddWriteLine("else");
      subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
      parGroup->AddSubGroup(subGroup);
      if (numOfSteering[i+numOfTask+1]>0) {
         steerPointerT.SetFormatted("gAnalyzer->GetWindow()->Get%s%03dTab()->GetSP()", tabName[i].Data(), i);
         AddSteeringConfigParameters(subGroup,0,i+numOfTask+1,steerPointerT);
      }
      AddTabConfigParameters(subGroup,i);
   }

   return true;
}
Bool_t  ROMEBuilder::AddSteeringConfigParameters(ROMEConfigParameterGroup *parGroup,Int_t numSteer,Int_t numTask,ROMEString steerPointer)
{
   int i;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString formatValue;
   ROMEConfigParameterGroup* subGroup;
   ROMEConfigParameterGroup* subSubGroup;
   for (i=0;i<numOfSteerFields[numTask][numSteer];i++) {
      if (!steerFieldUsed[numTask][numSteer][i])
         continue;
      subGroup = new ROMEConfigParameterGroup(steerFieldName[numTask][numSteer][i].Data(),"1","SteeringParameterField","SPName");
      parGroup->AddSubGroup(subGroup);
      if (subGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
         maxConfigParameterHierarchyLevel = subGroup->GetHierarchyLevel();
      if (steerFieldArraySize[numTask][numSteer][i]!="1") {
         subSubGroup = new ROMEConfigParameterGroup("SPFieldArray",steerFieldArraySize[numTask][numSteer][i].Data(),"","","SPFieldArrayIndex");
         subGroup->AddSubGroup(subSubGroup);
         if (subSubGroup->GetHierarchyLevel()>maxConfigParameterHierarchyLevel)
            maxConfigParameterHierarchyLevel = subSubGroup->GetHierarchyLevel();
         subSubGroup->AddParameter(new ROMEConfigParameter("SPFieldArrayValue","1"));
         setValue(&decodedValue,"","##",steerFieldType[numTask][numSteer][i].Data(),1);
         if (steerFieldType[numTask][numSteer][i]=="std::string")
            subSubGroup->GetLastParameter()->AddSetLine("%s->Set%sAt(i%d,##.Data());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
         else
            subSubGroup->GetLastParameter()->AddSetLine("%s->Set%sAt(i%d,(%s)%s);",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel(),steerFieldType[numTask][numSteer][i].Data(),decodedValue.Data());
         GetFormat(&formatValue,steerFieldType[numTask][numSteer][i].Data());
         if (steerFieldType[numTask][numSteer][i]=="TString" || steerFieldType[numTask][numSteer][i]=="ROMEString")
            subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%s\",%s->Get%sAt(i%d).Data());",formatValue.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
         else if (steerFieldType[numTask][numSteer][i]=="std::string")
            subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%s\",%s->Get%sAt(i%d).c_str());",formatValue.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
         else if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subSubGroup->GetLastParameter()->AddWriteLine("if (%s->Get%sAt(i%d))",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
            subSubGroup->GetLastParameter()->AddWriteLine("else");
            subSubGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         }
         else
            subSubGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%s\",%s->Get%sAt(i%d));",formatValue.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),subSubGroup->GetHierarchyLevel());
      }
      else {
         subGroup->AddParameter(new ROMEConfigParameter("SPValue","1"));
         setValue(&decodedValue,"","##",steerFieldType[numTask][numSteer][i].Data(),1);
         if (steerFieldType[numTask][numSteer][i]=="std::string")
            subGroup->GetLastParameter()->AddSetLine("%s->Set%s(##.Data());",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         else
            subGroup->GetLastParameter()->AddSetLine("%s->Set%s((%s)%s);",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data(),steerFieldType[numTask][numSteer][i].Data(),decodedValue.Data());
         GetFormat(&formatValue,steerFieldType[numTask][numSteer][i].Data());
         if (steerFieldType[numTask][numSteer][i]=="TString" || steerFieldType[numTask][numSteer][i]=="ROMEString")
            subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%s\",%s->Get%s().Data());",formatValue.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         else if (steerFieldType[numTask][numSteer][i]=="std::string")
            subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%s\",%s->Get%s().c_str());",formatValue.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
         else if (isBoolType(steerFieldType[numTask][numSteer][i].Data())) {
            subGroup->GetLastParameter()->AddWriteLine("if (%s->Get%s())",steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
            subGroup->GetLastParameter()->AddWriteLine("   writeString = \"true\";");
            subGroup->GetLastParameter()->AddWriteLine("else");
            subGroup->GetLastParameter()->AddWriteLine("   writeString = \"false\";");
         }
         else
            subGroup->GetLastParameter()->AddWriteLine("writeString.SetFormatted(\"%s\",%s->Get%s());",formatValue.Data(),steerPointer.Data(),steerFieldName[numTask][numSteer][i].Data());
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
         steerPointerT.SetFormatted("%s->Get%sAt(i%d)",steerPointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),subSubGroup->GetHierarchyLevel());
      }
      else
         steerPointerT.SetFormatted("%s->Get%s()",steerPointer.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      AddSteeringConfigParameters(subSubGroup,steerChildren[numTask][numSteer][i],numTask,steerPointerT);
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
   buffer.AppendFormatted("%sclass %s {\n",sTab.Data(),parGroup->GetGroupName().Data());
   buffer.AppendFormatted("%spublic:\n",sTab.Data());
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s   ROMEString   f%s;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sModified;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      }
      else {
         buffer.AppendFormatted("%s   ROMEString   f%s[%s];\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data(),parGroup->GetParameterAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sModified[%s];\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data(),parGroup->GetParameterAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s   Bool_t       f%sArrayModified;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
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
         buffer.AppendFormatted("%s      f%sModified = false;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      }
      else {
         buffer.AppendFormatted("%s      for (i=0;i<%s;i++)\n",sTab.Data(),parGroup->GetParameterAt(i)->GetArraySize().Data());
         buffer.AppendFormatted("%s         f%sModified[i] = false;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         buffer.AppendFormatted("%s      f%sArrayModified = false;\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="1") {
         buffer.AppendFormatted("%s      f%s = new %s();\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      f%sModified = false;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
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
   buffer.AppendFormatted("%s   };\n",sTab.Data());
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
Bool_t ROMEBuilder::WriteConfigRead(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes)
{
   int i,j;
   ROMEString classNameT;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString indexesT;
   ROMEString sTabT = "";
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";

   // Parameters
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      buffer.AppendFormatted("%stempPath.SetFormatted(\"/%s%s\"%s);\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName().Data(),indexes.Data());
      buffer.AppendFormatted("%sxml->GetPathValue(path+tempPath,fConfigData[index]->%sf%s,\"\");\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      buffer.AppendFormatted("%sif (fConfigData[index]->%sf%s==\"\")\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified = false;\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      buffer.AppendFormatted("%selse {\n",sTab.Data());
      buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified = true;\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfReadModifiedTrueLines();j++) {
         buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetParameterAt(i)->GetReadModifiedTrueLineAt(j));
      }
      buffer.AppendFormatted("%s}\n",sTab.Data());
   }
   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   fConfigData[index]->%sf%sArraySize = xml->NumberOfOccurrenceOfPath(path+\"/%ss/%s\");\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfReadGroupArrayInitLines();j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetReadGroupArrayInitLineAt(j));
         }
         buffer.AppendFormatted("%s   ROMEString tempPath;\n",sTab.Data());
         buffer.AppendFormatted("%s   fConfigData[index]->%sf%s = new ConfigData::%s%s*[fConfigData[index]->%sf%sArraySize];\n",sTab.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),className.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified = new bool[fConfigData[index]->%sf%sArraySize];\n",sTab.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s   for (i=0;i<fConfigData[index]->%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      fConfigData[index]->%sf%s[i] = new ConfigData::%s();\n",sTab.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (i%d=0;i%d<%s;i%d++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
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
         WriteConfigRead(buffer,parGroup->GetSubGroupAt(i),tab+2,groupNameT,classNameT,pointerT,indexesT);
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         else
            groupNameT.SetFormatted("%s%s[%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         indexesT.AppendFormatted(",i%d",parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         pointerT.SetFormatted("%sf%s[i%d]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         WriteConfigRead(buffer,parGroup->GetSubGroupAt(i),tab+1,groupNameT,classNameT,pointerT,indexesT);
      }
      else {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         else
            groupNameT.SetFormatted("%s%s[%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         pointerT.SetFormatted("%sf%s->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         WriteConfigRead(buffer,parGroup->GetSubGroupAt(i),tab,groupNameT,classNameT,pointerT,indexes);
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
Bool_t ROMEBuilder::WriteConfigCheckModified(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString className,ROMEString pointer,ROMEString indexes)
{
   int i,j;
   ROMEString classNameT;
   ROMEString groupNameT;
   ROMEString pointerT;
   ROMEString indexesT;
   ROMEString sTabT = "";
   ROMEString sTab = "";
   for (i=0;i<tab;i++)
      sTab += "   ";

   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         buffer.AppendFormatted("%s   for (i=0;i<fConfigData[index]->%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (i%d=0;i%d<%s;i%d++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
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
         WriteConfigCheckModified(buffer,parGroup->GetSubGroupAt(i),tab+2,groupNameT,classNameT,pointerT,indexesT);
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         else
            groupNameT.SetFormatted("%s%s[%s%%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Data());
         indexesT.AppendFormatted(",i%d",parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         pointerT.SetFormatted("%sf%s[i%d]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         WriteConfigCheckModified(buffer,parGroup->GetSubGroupAt(i),tab+1,groupNameT,classNameT,pointerT,indexesT);
      }
      else {
         if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0)
            groupNameT.SetFormatted("%s%s[%s='%s'][%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         else
            groupNameT.SetFormatted("%s%s[%d]/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data(),parGroup->GetSubGroupAt(i)->GetMultiplicity());
         pointerT.SetFormatted("%sf%s->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         WriteConfigCheckModified(buffer,parGroup->GetSubGroupAt(i),tab,groupNameT,classNameT,pointerT,indexes);
      }
      if (parGroup->GetSubGroupAt(i)->GetNumberOfParameters()+parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups()>0) {
         buffer.AppendFormatted("%s// %s%s--\n",sTabT.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%sif (",sTabT.Data());
         for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfParameters();j++) {
            if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
               buffer.AppendFormatted("fConfigData[index]->%sf%s[i]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetParameterAt(j)->GetName().Data());
            else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
               buffer.AppendFormatted("fConfigData[index]->%sf%s[i%d]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetParameterAt(j)->GetName().Data());
            else
               buffer.AppendFormatted("fConfigData[index]->%sf%s->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetParameterAt(j)->GetName().Data());
            if (j<parGroup->GetSubGroupAt(i)->GetNumberOfParameters()-1 || parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups()!=0)
               buffer.AppendFormatted(" ||\n%s    ",sTabT.Data());
         }
         for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups();j++) {
            if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
               buffer.AppendFormatted("fConfigData[index]->%sf%s[i]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
            else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
               buffer.AppendFormatted("fConfigData[index]->%sf%s[i%d]->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
            else
               buffer.AppendFormatted("fConfigData[index]->%sf%s->f%sModified",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetSubGroupAt(j)->GetGroupName().Data());
            if (j<parGroup->GetSubGroupAt(i)->GetNumberOfSubGroups()-1)
               buffer.AppendFormatted(" ||\n%s    ",sTabT.Data());
         }
         buffer.AppendFormatted(") {\n");
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified[i] = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sArrayModified = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified[i%d] = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sArrayModified = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else {
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified = true;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         buffer.AppendFormatted("%s}\n",sTabT.Data());
         buffer.AppendFormatted("%selse {\n",sTabT.Data());
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified[i] = false;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         }
         else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1")
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified[i%d] = false;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         else {
            buffer.AppendFormatted("%s   fConfigData[index]->%sf%sModified = false;\n",sTabT.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
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
Bool_t ROMEBuilder::WriteConfigSet(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer)
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
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      if (parGroup->GetParameterAt(i)->GetNumberOfSetLines()>0) {
         buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         buffer.AppendFormatted("%sif (fConfigData[modIndex]->%sf%sModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfSetLines();j++) {
            buf = parGroup->GetParameterAt(i)->GetSetLineAt(j);
            temp.SetFormatted("fConfigData[index]->%sf%s",pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
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
         buffer.AppendFormatted("%s   if (fConfigData[modIndex]->f%sArrayModified) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      for (i=0;i<fConfigData[index]->%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s         if (fConfigData[modIndex]->f%sModified[i]) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         tabT += 4;
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (i%d=0;i%d<%s;i%d++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         tabT += 1;
         pointerT.SetFormatted("%sf%s[i%d]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
      }
      WriteConfigSet(buffer,parGroup->GetSubGroupAt(i),tabT,groupNameT,pointerT);
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

Bool_t ROMEBuilder::WriteConfigWrite(ROMEString &buffer,ROMEConfigParameterGroup *parGroup,int tab,ROMEString groupName,ROMEString pointer)
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
   for (i=0;i<parGroup->GetNumberOfParameters();i++) {
      buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      if (!parGroup->GetParameterAt(i)->IsWriteLinesAlways() || parGroup->GetParameterAt(i)->GetNumberOfWriteLines()==0) {
         buffer.AppendFormatted("%s",sTab.Data());
      }
      else {
         buffer.AppendFormatted("%sif (index==0) {\n",sTab.Data());
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfWriteLines();j++) {
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),parGroup->GetParameterAt(i)->GetWriteLineAt(j));
         }
         for (j=0;j<parGroup->GetParameterAt(i)->GetNumberOfAdditionalWriteLines();j++) {
            buf = parGroup->GetParameterAt(i)->GetAdditionalWriteLineAt(j);
            temp.SetFormatted("fConfigData[index]->%sf%s",pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
            buf.ReplaceAll("##",temp);
            buffer.AppendFormatted("%s   %s\n",sTab.Data(),buf.Data());
         }
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",writeString.Data());\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data());
         buffer.AppendFormatted("%s}\n",sTab.Data());
         buffer.AppendFormatted("%selse ",sTab.Data());
      }
      buffer.AppendFormatted("if (fConfigData[index]->%sf%sModified)\n",pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
      buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",fConfigData[index]->%sf%s.Data());\n",sTab.Data(),parGroup->GetParameterAt(i)->GetName().Data(),pointer.Data(),parGroup->GetParameterAt(i)->GetName().Data());
   }
   // Sub Groups
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1" && parGroup->GetSubGroupAt(i)->GetArraySize()!="unknown") {
         buffer.AppendFormatted("%sint i%d;\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         break;
      }
   }
   for (i=0;i<parGroup->GetNumberOfSubGroups();i++) {
      sTabT = sTab;
      tabT = tab;
      groupNameT.SetFormatted("%s%s/",groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      pointerT.SetFormatted("%sf%s->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      buffer.AppendFormatted("%s// %s%s\n",sTab.Data(),groupName.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown") {
         if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
            buffer.AppendFormatted("%sif (fConfigData[index]->%sf%sArrayModified || index==0) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         else
            buffer.AppendFormatted("%sif (fConfigData[index]->%sf%sArrayModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s   xml->StartElement(\"%ss\");\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
         buffer.AppendFormatted("%s   for (i=0;i<fConfigData[index]->%sf%sArraySize;i++) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         buffer.AppendFormatted("%s      if (fConfigData[index]->%sf%sModified[i]) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         sTabT += "      ";
         tabT += 2;
         pointerT.SetFormatted("%sf%s[i]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      else if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1") {
         buffer.AppendFormatted("%sfor (i%d=0;i%d<%s;i%d++) {\n",sTab.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel(),parGroup->GetSubGroupAt(i)->GetArraySize().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
            buffer.AppendFormatted("%s   if (fConfigData[index]->%sf%sModified[i%d] || index==0) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         else
            buffer.AppendFormatted("%s   if (fConfigData[index]->%sf%sModified[i%d]) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         tabT += 1;
         sTabT += "   ";
         pointerT.SetFormatted("%sf%s[i%d]->",pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
      }
      else {
         if (parGroup->GetSubGroupAt(i)->IsWriteAlways())
            buffer.AppendFormatted("%sif (fConfigData[index]->%sf%sModified || index==0) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
         else
            buffer.AppendFormatted("%sif (fConfigData[index]->%sf%sModified) {\n",sTab.Data(),pointer.Data(),parGroup->GetSubGroupAt(i)->GetGroupName().Data());
      }
      for (j=0;j<parGroup->GetSubGroupAt(i)->GetNumberOfWriteStartLines();j++) {
         buffer.AppendFormatted("%s   %s\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetWriteStartLineAt(j));
      }
      if (parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Length()>0) {
         buffer.AppendFormatted("%s   xml->StartElement(\"%s\");\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetGroupIdentifier().Data());
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",\"%s\");\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetNameIdentifier().Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
      }
      else
         buffer.AppendFormatted("%s   xml->StartElement(\"%s\");\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetTagName().Data());
      if (parGroup->GetSubGroupAt(i)->GetArraySize()!="1" && parGroup->GetSubGroupAt(i)->GetArrayIdentifier().Length()) {
         if (parGroup->GetSubGroupAt(i)->GetArraySize()=="unknown")
            buffer.AppendFormatted("%s   str.SetFormatted(\"%%d\",i);\n",sTabT.Data());
         else
            buffer.AppendFormatted("%s   str.SetFormatted(\"%%d\",i%d);\n",sTabT.Data(),parGroup->GetSubGroupAt(i)->GetHierarchyLevel());
         temp = parGroup->GetSubGroupAt(i)->GetArrayIdentifier();
         if (temp.EndsWith("="))
            temp.Resize(temp.Length()-1);
         buffer.AppendFormatted("%s   xml->WriteElement(\"%s\",str.Data());\n",sTabT.Data(),temp.Data());
      }
      WriteConfigWrite(buffer,parGroup->GetSubGroupAt(i),tabT+1,groupNameT,pointerT);
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
   int i,j;

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
   buffer.AppendFormatted("#include \"ROMEEventLoop.h\"\n");
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sMidasDAQ.h\"\n",shortCut.Data());
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

   // Task switches call back
   buffer.AppendFormatted("// Task switches call back\n");
   buffer.AppendFormatted("void TaskSwitchesChanged(HNDLE hDB,HNDLE hKey,void *info) {\n");
   buffer.AppendFormatted("   ROMEEventLoop::fTaskSwitchesChanged = true;\n");
   buffer.AppendFormatted("}\n\n");

   // Fill Bank Header to Folder
   buffer.AppendFormatted("// Fill Bank Header to Folder\n");
   buffer.AppendFormatted("Bool_t %sMidasDAQ::InitHeader() {\n",shortCut.Data());
   if (bankHasHeader) {
      if (bankHeaderEventID!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->event_id);\n",bankHeaderFolder.Data(),bankHeaderEventID.Data());
      if (bankHeaderTriggerMask!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->trigger_mask);\n",bankHeaderFolder.Data(),bankHeaderTriggerMask.Data());
      if (bankHeaderSerialNumber!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->serial_number);\n",bankHeaderFolder.Data(),bankHeaderSerialNumber.Data());
      if (bankHeaderTimeStamp!="")
         buffer.AppendFormatted("   gAnalyzer->Get%s()->Set%s(((EVENT_HEADER*)this->GetRawDataEvent())->time_stamp);\n",bankHeaderFolder.Data(),bankHeaderTimeStamp.Data());
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

   // Initialize ODB
   buffer.AppendFormatted("// InitODB\n");
   buffer.AppendFormatted("Bool_t %sMidasDAQ::InitODB() {\n",shortCut.Data());
   if (this->midas) {
      buffer.AppendFormatted("   HNDLE hKey;\n");
      buffer.AppendFormatted("   ROMEString str;\n");
      buffer.AppendFormatted("   str = \"/%s%s/Task switches\";\n",shortCut.Data(),mainProgName.Data());
      buffer.AppendFormatted("   ROMEString taskSwitchesString =  \"");
      ROMEString switchString;
      for (i=0;i<numOfTaskHierarchy;i++) {
         if (!taskUsed[taskHierarchyClassIndex[i]])
            continue;
         int index = taskHierarchyParentIndex[i];
         switchString.SetFormatted("%s%03d",taskHierarchyName[i].Data(),i);
         while (index!=-1) {
            switchString.Insert(0,"_");
            switchString.InsertFormatted(0,"%s%03d",taskHierarchyName[index].Data(),index);
            index = taskHierarchyParentIndex[index];
         }
         buffer.AppendFormatted("%s = BOOL : 0\\n",switchString.Data());
      }
      buffer.AppendFormatted("\";\n");
      buffer.AppendFormatted("   db_check_record(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), (char*)taskSwitchesString.Data(), TRUE);\n");
      buffer.AppendFormatted("   db_find_key(gAnalyzer->GetMidasOnlineDataBase(), 0, (char*)str.Data(), &hKey);\n");
      buffer.AppendFormatted("   if (db_set_record(gAnalyzer->GetMidasOnlineDataBase(),hKey,gAnalyzer->GetTaskSwitches(),sizeof(TaskSwitches),0) != DB_SUCCESS) {\n");
      buffer.AppendFormatted("      gAnalyzer->PrintLine(\"Cannot write to task switches record.\");\n");
      buffer.AppendFormatted("      return false;\n");
      buffer.AppendFormatted("   }\n");
      buffer.AppendFormatted("   if (db_open_record(gAnalyzer->GetMidasOnlineDataBase(), hKey, gAnalyzer->GetTaskSwitches(), sizeof(TaskSwitches), MODE_READ, TaskSwitchesChanged, NULL) != DB_SUCCESS) {\n");
      buffer.AppendFormatted("      gAnalyzer->PrintLine(\"Cannot open task switches record, probably other analyzer is using it\");\n");
      buffer.AppendFormatted("      return false;\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   return true;\n");
   buffer.AppendFormatted("}\n\n");

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
            buffer.AppendFormatted("      this->Init%sBank(i);\n",bankName[i][j].Data());
         }
         // Single Bank
         else {
            buffer.AppendFormatted("   this->Init%sBank();\n",bankName[i][j].Data());
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
               buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex]+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankPointer(Int_t bankIndex) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(Int_t bankIndex,Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex]+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankPointer(Int_t bankIndex) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
            }
            else {
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(Int_t bankIndex,Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer[bankIndex]+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::Get%sBankPointer(Int_t bankIndex) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(Int_t bankIndex,Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer[bankIndex]+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::GetLast%sBankPointer(Int_t bankIndex) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists[bankIndex])\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer[bankIndex];\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
            }
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank(Int_t bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID()==%s) {\n",eventName[i].Data(),bankName[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists[bankIndex] = f%sBankExists[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer[bankIndex] = f%sBankPointer[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength[bankIndex] = f%sBankLength[bankIndex];\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      DWORD bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();\n");
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

            buffer.AppendFormatted("Int_t %sMidasDAQ::Get%sBankEntries(Int_t bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength[bankIndex];\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n");
            buffer.AppendFormatted("Int_t %sMidasDAQ::GetLast%sBankEntries(Int_t bankIndex) {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return fLast%sBankLength[bankIndex];\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n\n");
         }
         // Single Bank
         else {
            if (bankType[i][j]=="structure"||bankType[i][j]=="struct") {
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankAt(Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::Get%sBankPointer() {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankAt(Int_t index) {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer+index;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%sStruct* %sMidasDAQ::GetLast%sBankPointer() {\n",bankName[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
            }
            else {
               buffer.AppendFormatted("%s %sMidasDAQ::Get%sBankAt(Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(f%sBankPointer+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::Get%sBankPointer() {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->f%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return f%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s %sMidasDAQ::GetLast%sBankAt(Int_t index) {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return *(fLast%sBankPointer+index);\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return (%s)exp(999.);\n",bankType[i][j].Data());
               buffer.AppendFormatted("}\n");
               buffer.AppendFormatted("%s* %sMidasDAQ::GetLast%sBankPointer() {\n",bankType[i][j].Data(),shortCut.Data(),bankName[i][j].Data());
               buffer.AppendFormatted("   if (this->fLast%sBankExists)\n",bankName[i][j].Data());
               buffer.AppendFormatted("      return fLast%sBankPointer;\n",bankName[i][j].Data());
               buffer.AppendFormatted("   return NULL;\n");
               buffer.AppendFormatted("}\n");
            }
            buffer.AppendFormatted("void %sMidasDAQ::Init%sBank() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   if (f%sEventActive && f%sBankActive && gAnalyzer->GetEventID()==%s) {\n",eventName[i].Data(),bankName[i][j].Data(),eventID[i].Data());
            buffer.AppendFormatted("      fLast%sBankExists = f%sBankExists;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankPointer = f%sBankPointer;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      fLast%sBankLength = f%sBankLength;\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("      DWORD bktype;\n");
            buffer.AppendFormatted("      EVENT_HEADER *pevent = (EVENT_HEADER*)this->GetRawDataEvent();\n");
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

            buffer.AppendFormatted("Int_t %sMidasDAQ::Get%sBankEntries() {\n",shortCut.Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   return f%sBankLength;\n",bankName[i][j].Data());
            buffer.AppendFormatted("}\n");
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
   buffer.AppendFormatted( "}\n" );

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

   // Methods
   buffer.AppendFormatted("public:\n");
   // Constructor
   buffer.AppendFormatted("   %sMidasDAQ();\n",shortCut.Data());

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
            buffer.AppendFormatted("   bool is%sBankActive() { return f%sBankActive; };\n",bankName[i][j].Data(),bankName[i][j].Data());
            buffer.AppendFormatted("   void Set%sBankActive(bool flag) { f%sBankActive = flag; };\n",bankName[i][j].Data(),bankName[i][j].Data());
         }
      }
      buffer.AppendFormatted("   bool is%sEventActive() { return f%sEventActive; };\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("   void Set%sEventActive(bool flag) { f%sEventActive = flag; };\n",eventName[i].Data(),eventName[i].Data());
      buffer.AppendFormatted("\n");
   }

   // protected
   buffer.AppendFormatted("protected:\n");
   buffer.AppendFormatted("   Bool_t  InitODB();\n");
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
   int i;

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
   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TBranchElement.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sRomeDAQ.h\"\n",shortCut.Data());

   buffer.AppendFormatted("\nClassImp(%sRomeDAQ)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("\n// Constructor\n");
   buffer.AppendFormatted("%sRomeDAQ::%sRomeDAQ() { }\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("\n");

   // Connect Trees
   int iFold=0,j,k;
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
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("   bb = (TBranchElement*)gAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"%s\");\n",i,branchName[i][j].Data());
         buffer.AppendFormatted("   if (bb) {\n");
         buffer.AppendFormatted("      if (gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d)) {\n",i,j);
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("         bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         else {
            buffer.AppendFormatted("         bb->SetAddress(gAnalyzer->Get%sAddress());\n",folderName[iFold].Data());
         }
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      else {\n");
         buffer.AppendFormatted("         gAnalyzer->GetTreeObjectAt(%d)->GetTree()->SetBranchStatus(\"%s*\", 0);\n",i,branchName[i][j].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("   bb = (TBranchElement*)gAnalyzer->GetTreeObjectAt(%d)->GetTree()->FindBranch(\"Info\");\n",i);
         buffer.AppendFormatted("   bb->SetAddress(&this->fTreeInfo);\n");
      }
   }
   buffer.AppendFormatted("}\n\n");

   // Write File
   WriteFile(cppFile.Data(),buffer.Data(),6);

   return true;
}

Bool_t ROMEBuilder::WriteRomeDAQH() {
   ROMEString hFile;
   ROMEString buffer;
   ROMEString clsName;
   ROMEString clsDescription;

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
      cppFile.SetFormatted("%ssrc/daqs/%s%s.cpp",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

#if 1
// this special treatment is neccesary only for several month from Feb.2006
      if (!gSystem->AccessPathName(cppFile.Data(),kFileExists)) {
         fstream *fileStream = new fstream(cppFile.Data(),ios::in);
         ROMEString fileBuffer;
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
         if (fileBuffer.Contains("Event(int event)")
             || fileBuffer.Contains("Event(Int_t event)")
             || fileBuffer.Contains("Event(long event)")) {
            cerr<<"type of event number was changed to Long64."<<endl
                <<"Please modify "<<shortCut.Data()<<daqName[iDAQ].Data()<<".h and "
                <<shortCut.Data()<<daqName[iDAQ].Data()<<".cpp"<<endl;
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
         clsName.SetFormatted("%s%s", shortCut.Data(), daqName[iDAQ].Data());
         clsDescription.SetFormatted("%s DAQ class for %s%s.",daqName[iDAQ].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"include/daqs/%s%s.h\"\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
         buffer.AppendFormatted("#include \"include/generated/%sConfig.h\"\n",shortCut.Data());
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("ClassImp(%s%s)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("%s%s::%s%s()\n",shortCut.Data(),daqName[iDAQ].Data(),shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("\n");

         buffer.AppendFormatted("Bool_t %s%s::Init()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%s::BeginOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%s::Event(Long64_t event)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%s::EndOfRun()\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");
         buffer.AppendFormatted("   return true;\n");
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("Bool_t %s%s::Terminate()\n",shortCut.Data(),daqName[iDAQ].Data());
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
      hFile.SetFormatted("%sinclude/daqs/%s%s.h",outDir.Data(),shortCut.Data(),daqName[iDAQ].Data());

#if 1
// this special treatment is neccesary only for several month from Feb.2006
      if (!gSystem->AccessPathName(hFile.Data(),kFileExists)) {
         fstream *fileStream = new fstream(hFile.Data(),ios::in);
         ROMEString fileBuffer;
         fileBuffer.ReadFile(*fileStream);
         delete fileStream;
         if (fileBuffer.Contains("Event(int event)")
             || fileBuffer.Contains("Event(long event)")
             || fileBuffer.Contains("Event(Int_t event)")) {
            cerr<<"type of event number was changed to Long64."<<endl
                <<"Please modify."<<shortCut<<daqName[iDAQ]<<".h and "
                <<shortCut<<daqName[iDAQ]<<".cpp"<<endl;
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
         buffer.AppendFormatted("#ifndef %s%s_H\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("#define %s%s_H\n\n",shortCut.Data(),daqName[iDAQ].Data());
         clsName.SetFormatted("%s%s", shortCut.Data(), daqName[iDAQ].Data());
         clsDescription.SetFormatted("%s DAQ class for %s%s.",daqName[iDAQ].Data(),shortCut.Data(),mainProgName.Data());
         WriteDescription(buffer, clsName.Data(), clsDescription.Data(), kFALSE);
         buffer.AppendFormatted("\n\n");

         buffer.AppendFormatted("#include \"ROMEDAQSystem.h\"\n");

         // Class
         buffer.AppendFormatted("\nclass %s%s : public ROMEDAQSystem\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("{\n");

         buffer.AppendFormatted("protected:\n");
         // Methods
         buffer.AppendFormatted("public:\n");
         // Constructor and Methods
         buffer.AppendFormatted("   %s%s();\n",shortCut.Data(),daqName[iDAQ].Data());

         // Methods
         buffer.AppendFormatted("   Int_t  GetTimeStamp() { return 0; };\n");
         buffer.AppendFormatted("   const char* GetName() { return \"%s\"; };\n",daqName[iDAQ].Data());
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("   Bool_t Init();\n");
         buffer.AppendFormatted("   Bool_t BeginOfRun();\n");
         buffer.AppendFormatted("   Bool_t Event(Long64_t event);\n");
         buffer.AppendFormatted("   Bool_t EndOfRun();\n");
         buffer.AppendFormatted("   Bool_t Terminate();\n");

         buffer.AppendFormatted("   ClassDef(%s%s, 0)\n",shortCut.Data(),daqName[iDAQ].Data());
         buffer.AppendFormatted("};\n\n");


         buffer.AppendFormatted("#endif   // %s%s_H\n",shortCut.Data(),daqName[iDAQ].Data());

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

         buffer.AppendFormatted("#include \"include/databases/%s%sDataBase.h\"\n",shortCut.Data(),dbName[iDB].Data());
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

   // Fields
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen+5-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldComment[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds f%%s[%%s];%%%ds %%s\n",typeLen,nameLen+3-steerFieldName[numTask][numSteer][j].Length()-steerFieldArraySize[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldArraySize[numTask][numSteer][j].Data(),"",steerFieldComment[numTask][numSteer][j].Data());
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
   buffer.AppendFormatted("%s   %s%s() { ",blank.Data(),sc.Data(),steerName[numTask][numSteer].Data());
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]!="1") {
         buffer.AppendFormatted("int j; ");
         break;
      }
   }
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         if (steerFieldType[numTask][numSteer][j]=="std::string")
            buffer.AppendFormatted("f%s = %s; ",steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
         else
            buffer.AppendFormatted("f%s = (%s)%s; ",steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
      }
      else {
         if (steerFieldType[numTask][numSteer][j]=="std::string")
            buffer.AppendFormatted("for (j=0;j<%s;j++) f%s[j] = %s; ",steerFieldArraySize[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
         else
            buffer.AppendFormatted("for (j=0;j<%s;j++) f%s[j] = (%s)%s; ",steerFieldArraySize[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldInit[numTask][numSteer][j].Data());
      }
   }
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
         buffer.AppendFormatted("f%s = new %s(); ",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         buffer.AppendFormatted("f%s = new %s*[%s]; for (i=0;i<%s;i++) f%s[i] = new %s(); ",steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerArraySize[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   buffer.AppendFormatted("};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen+7-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds Get%%sAt(Int_t i)%%%ds { return f%%s[i]; };\n",typeLen,nameLen-steerFieldName[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data());
      }
   }
   for (i=0;i<numOfSteerChildren[numTask][numSteer];i++) {
      if (!steerUsed[numTask][steerChildren[numTask][numSteer][i]])
         continue;
      if (steerArraySize[numTask][steerChildren[numTask][numSteer][i]]=="1") {
         format.SetFormatted("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen+7-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
      else {
         format.SetFormatted("%%s   %%-%ds *Get%%sAt(Int_t i)%%%ds { return f%%s[i]; };\n",typeLen-1,nameLen-steerName[numTask][steerChildren[numTask][numSteer][i]].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),steerName[numTask][steerChildren[numTask][numSteer][i]].Data(),"",steerName[numTask][steerChildren[numTask][numSteer][i]].Data());
      }
   }
   // Setters
   buffer.AppendFormatted("\n");
   for (j=0;j<numOfSteerFields[numTask][numSteer];j++) {
      if (!steerFieldUsed[numTask][numSteer][j])
         continue;
      if (steerFieldArraySize[numTask][numSteer][j]=="1") {
         format.SetFormatted("%%s   void Set%%s(%%s %%s)%%%ds { f%%s = %%s; };\n",2*nameLen+typeLen+8-2*steerFieldName[numTask][numSteer][j].Length()-steerFieldType[numTask][numSteer][j].Length());
         buffer.AppendFormatted(format.Data(),blank.Data(),steerFieldName[numTask][numSteer][j].Data(),steerFieldType[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data(),"",steerFieldName[numTask][numSteer][j].Data(),steerFieldName[numTask][numSteer][j].Data());
      }
      else {
         format.SetFormatted("%%s   void Set%%sAt(Int_t i,%%s %%s)%%%ds { f%%s[i] = %%s; };\n",2*nameLen+typeLen-2*steerFieldName[numTask][numSteer][j].Length()-steerFieldType[numTask][numSteer][j].Length());
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
               buffer.AppendFormatted("         gAnalyzer->PrintLine(\"warning: %s might not be specified properly.\");\n",steerFieldName[numTask][numSteer][k].Data());
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
               buffer.AppendFormatted("            gAnalyzer->PrintLine(\"warning: %s might not be specified properly.\");\n",steerFieldName[numTask][numSteer][k].Data());
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
         format.SetFormatted("   gAnalyzer->PrintLine(\"  -%%s%%s%%%ds%%s",nspace);
         buffer.AppendFormatted(format.Data(),steerFieldCLOption[numTask][numSteer][k].Data(),arrayIndex.Data(),"",steerFieldCLDescription[numTask][numSteer][k].Data());
         if (isBoolType(steerFieldCLOption[numTask][numSteer][k].Data()))
            buffer.AppendFormatted(" (no Argument)");
         buffer.AppendFormatted("\");\n");
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
   buffer.AppendFormatted("   char *cstop=NULL;\n");
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
   buffer.AppendFormatted("   int j=0;\n");
   buffer.AppendFormatted("   ROMEString str = \"\";\n");
   buffer.AppendFormatted("   ROMEString path;\n");
   buffer.AppendFormatted("   ROMEString name;\n");
   buffer.AppendFormatted("   ROMEString buffer[%d];\n",maxNumberOfPathObjectInterpreterCodes);
   buffer.AppendFormatted("   ROMEStr2DArray *values = new ROMEStr2DArray(1,1);\n");
   buffer.AppendFormatted("   char *cstop=NULL;\n");
   for (j=0;j<numOfValue[numFolder];j++) {
      if ( valueDimension[numFolder][j]>1 || valueArray[numFolder][j][0] == "variable")
         continue;
      if (folderArray[numFolder]=="1" && type==1 || folderArray[numFolder]!="1" && type==2 && !folderSupport[numFolder]) {
         buffer.AppendFormatted("   values->RemoveAll();\n");
         buffer.AppendFormatted("   if (strlen(gAnalyzer->Get%s_%sDBName())==0)\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("      name.SetFormatted(\"%s\");\n",valueDBName[numFolder][j].Data());
         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      name.SetFormatted(gAnalyzer->Get%s_%sDBName());\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("   if (strlen(gAnalyzer->Get%s_%sDBPath(str))==0)\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         if (valueDBPath[numFolder][j].Length())
            buffer.AppendFormatted("      path.SetFormatted(%s);\n",valueDBPath[numFolder][j].Data());
         else
            buffer.AppendFormatted("      path.SetFormatted(\"\");\n");

         buffer.AppendFormatted("   else\n");
         buffer.AppendFormatted("      path.SetFormatted(gAnalyzer->Get%s_%sDBPath(str)",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         for (k=0;k<maxNumberOfPathObjectInterpreterCodes;k++)
            buffer.AppendFormatted("\n         ,gAnalyzer->GetObjectInterpreterCharValue(gAnalyzer->Get%s_%sDBCodeAt(%d),buffer[%d],buffer[%d]).Data()",folderName[numFolder].Data(),valueName[numFolder][j].Data(),k,k,k);
         buffer.AppendFormatted(");\n");
         buffer.AppendFormatted("   if (name.Length() && path.Length() && isDataBaseActive(name.Data())) {\n");
         buffer.AppendFormatted("      gAnalyzer->PrintVerbose(\"Reading database %%s(type=%%s, path=%%s)\",name.Data(),this->GetDataBase(name.Data())->GetType(),path.Data());\n");
         buffer.AppendFormatted("      if (this->GetDataBase(name.Data())->Read(values,path,gAnalyzer->GetCurrentRunNumber(),gAnalyzer->GetCurrentEventNumber())) {\n");
         buffer.AppendFormatted("         gAnalyzer->PrintVerbose(\"%s/%s was filled.\");\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("      }\n");
         buffer.AppendFormatted("      else {\n");
         buffer.AppendFormatted("         gAnalyzer->PrintLine(\"   in Folder '%s' Value '%s'.\");\n",folderName[numFolder].Data(),valueName[numFolder][j].Data());
         buffer.AppendFormatted("         return false;\n");
         buffer.AppendFormatted("      }\n");
         if (type==2) {
            buffer.AppendFormatted("      nentry = f%sFolders->GetEntries();\n",folderName[numFolder].Data());
            buffer.AppendFormatted("      for (i=0;i<nentry;i++) {\n");
         }
         if (valueDimension[numFolder][j]==0) {
            buffer.AppendFormatted("%s      if (values->At(%s,0).Length()!=0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,0).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),valueType[numFolder][j].Data(),1);
            if (type==1) {
               buffer.AppendFormatted("%s         f%sFolder->Set%s((%s)%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("%s      else\n",blank.Data());
               buffer.AppendFormatted("%s         f%sFolder->Set%s(%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            else {
               buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%s((%s)%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueType[numFolder][j].Data(),buf.Data());
               buffer.AppendFormatted("         else\n");
               buffer.AppendFormatted("            ((%s%s*)f%sFolders->At(i))->Set%s(%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
         }
         else if (valueDimension[numFolder][j]==1) {
            buffer.AppendFormatted("%s      for (j=0;j<%s;j++) {\n",blank.Data(),valueArray[numFolder][j][0].Data());
            buffer.AppendFormatted("%s         if (values->At(%s,j).Length()!=0)\n",blank.Data(),iValue.Data());
            valueString.SetFormatted("values->At(%s,j).Data()",iValue.Data());
            setValue(&buf,valueName[numFolder][j].Data(),valueString.Data(),TArray2StandardType(valueType[numFolder][j],tempBuffer),1);
            if (type==1) {
               buffer.AppendFormatted("%s            f%sFolder->Set%sAt(j,(%s)%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),TArray2StandardType(valueType[numFolder][j],tempBuffer),buf.Data());
               buffer.AppendFormatted("%s         else\n",blank.Data());
               buffer.AppendFormatted("%s            f%sFolder->Set%sAt(j,%s);\n",blank.Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            else {
               buffer.AppendFormatted("               ((%s%s*)f%sFolders->At(i))->Set%sAt(j,(%s)%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),TArray2StandardType(valueType[numFolder][j],tempBuffer),buf.Data());
               buffer.AppendFormatted("            else\n");
               buffer.AppendFormatted("               ((%s%s*)f%sFolders->At(i))->Set%sAt(j,%s);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data(),valueName[numFolder][j].Data(),valueInit[numFolder][j].Data());
            }
            buffer.AppendFormatted("%s      }\n",blank.Data());
         }
         else {
            cout<<"Warning: multiple dimension field can not have database connection."<<endl;
            return;
         }
         if (type==2)
            buffer.AppendFormatted("      }\n");
      }
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   values->RemoveAll();\n");
   buffer.AppendFormatted("   delete values;\n");
}

void ROMEBuilder::WriteFolderGetterInclude(ROMEString &buffer,Int_t numFolder,Int_t scl,Int_t nameLen,Int_t typeLen)
{
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
//      int lt = typeLen-folderName[numFolder].Length()-scl+nameLen-folderName[numFolder].Length();
      if (folderArray[numFolder]=="1") {
         buffer.AppendFormatted("   %s%s* Get%s();\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   %s%s** Get%sAddress();\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
      }
      else {
         buffer.AppendFormatted("   %s%s* Get%sAt(Int_t index);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray* Get%ss();\n",folderName[numFolder].Data());
         buffer.AppendFormatted("   TClonesArray** Get%sAddress();\n",folderName[numFolder].Data());
      }
   }
}

void ROMEBuilder::WriteFolderGetterSource(ROMEString &buffer,Int_t numFolder,Int_t scl)
{
   if (folderSupport[numFolder])
      return;
   ROMEString format;
   if (numOfValue[numFolder] > 0) {
      if (folderArray[numFolder]=="1") {
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%s() {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolder = (%s%s*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%s\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   return f%sFolder;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("%s%s** %sAnalyzer::Get%sAddress() {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolder = (%s%s*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%s\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   return &f%sFolder;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
      else if (folderArray[numFolder]=="variable") {
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%sAt(Int_t index) {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {

            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("  if (f%sFolders->GetEntries()<=index)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("     for (int i=f%sFolders->GetEntries();i<=index;i++)\n",folderName[numFolder].Data());
         buffer.AppendFormatted("        new((*f%sFolders)[i]) %s%s();\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("  return (%s%s*)f%sFolders->At(index);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray* %sAnalyzer::Get%ss() {\n",shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   return f%sFolders;",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray** %sAnalyzer::Get%sAddress() {;\n",shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   return &f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
      }
      else {
         buffer.AppendFormatted("%s%s* %sAnalyzer::Get%sAt(Int_t index) {\n",shortCut.Data(),folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   if (f%sFolders->GetEntriesFast()<=index) {\n",folderName[numFolder].Data());
         buffer.AppendFormatted("     ROMEString str;str.SetFormatted(\"\\nYou have tried to access the %%d. item of the array folder %s\\nwhich was defined with array size %s.\\n\\nShutting down the program.\\n\",index);\n",folderName[numFolder].Data(),folderArray[numFolder].Data());
         buffer.AppendFormatted("     this->PrintLine(str.Data());\n");
         buffer.AppendFormatted("     fApplication->Terminate(1);\n");
         buffer.AppendFormatted("     return NULL;\n");
         buffer.AppendFormatted("   }\n");
         buffer.AppendFormatted("  return (%s%s*)f%sFolders->At(index);\n",shortCut.Data(),folderName[numFolder].Data(),folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray* %sAnalyzer::Get%ss() { \n",shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   return f%sFolders;\n",folderName[numFolder].Data());
         buffer.AppendFormatted("}\n");
         buffer.AppendFormatted("\n");
         buffer.AppendFormatted("TClonesArray** %sAnalyzer::Get%sAddress() { \n",shortCut.Data(),folderName[numFolder].Data());
         if (folderNet[numFolder]) {
            buffer.AppendFormatted("   if (IsStandAloneARGUS() && IsSocketToROMEActive())\n");
            buffer.AppendFormatted("      f%sFolders = (TClonesArray*)(GetSocketToROMENetFolder()->FindObjectAny(\"%s%ss\"));\n",folderName[numFolder].Data(),shortCut.Data(),folderName[numFolder].Data());
         }
         buffer.AppendFormatted("   return &f%sFolders;\n",folderName[numFolder].Data());
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
   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TH1.h\"\n");
   buffer.AppendFormatted("#include \"TROOT.h\"\n");
   buffer.AppendFormatted("#include \"TObjArray.h\"\n");
   buffer.AppendFormatted("#include \"TObjString.h\"\n");
   buffer.AppendFormatted("#include \"TFolder.h\"\n");
   buffer.AppendFormatted("#include \"TBranchElement.h\"\n");
   buffer.AppendFormatted("#include \"TTask.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS

   buffer.AppendFormatted("#include \"ROMETree.h\"\n");
   buffer.AppendFormatted("#include \"ROMETreeInfo.h\"\n");
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sEventLoop.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sConfig.h\"\n",shortCut.Data());

   buffer.AppendFormatted("#include \"Riostream.h\"\n");
   buffer.AppendFormatted("#include \"include/generated/%sAllFolders.h\"\n",shortCut.Data());
   buffer.AppendFormatted("\n");

   buffer.AppendFormatted("\nClassImp(%sEventLoop)\n",shortCut.Data());

   // Constructor
   buffer.AppendFormatted("%sEventLoop::%sEventLoop(const char *name,const char *title):ROMEEventLoop(name,title) {\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("}\n");

   // Tree Initialization
   buffer.AppendFormatted("// Tree initialization\n");
   buffer.AppendFormatted("void %sEventLoop::InitTrees()\n{\n",shortCut.Data());
   buffer.AppendFormatted("   TFolder *treeFolder = gAnalyzer->GetMainFolder()->AddFolder(\"Trees\",\"Trees of the %s framework\");\n",shortCut.Data());
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
   if (numOfTree>0) {
      buffer.AppendFormatted("   TTree *tree;\n");
   }
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   tree = gAnalyzer->GetTreeObjectAt(%d)->GetTree();\n",i);
      buffer.AppendFormatted("   tree->Branch(\"Info\",\"ROMETreeInfo\",&fTreeInfo,32000,99);\n");
      buffer.AppendFormatted("   tree->GetBranch(\"Info\")->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",i);
      for (j=0;j<numOfBranch[i];j++) {
         for (k=0;k<numOfFolder;k++) {
            if (branchFolder[i][j]==folderName[k] && !folderSupport[k])
               iFold = k;
         }
         if (!folderUsed[iFold])
            continue;
         buffer.AppendFormatted("   if(gAnalyzer->GetTreeObjectAt(%d)->GetBranchActiveAt(%d))\n",i,j);
         if (folderArray[iFold]=="1") {
            buffer.AppendFormatted("      tree->Branch(\"%s\",\"%s%s\",gAnalyzer->Get%sAddress(),%s,%s);\n",branchName[i][j].Data(),shortCut.Data(),folderName[iFold].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),branchSplitLevel[i][j].Data());
         }
         else {
            buffer.AppendFormatted("      tree->Branch(\"%s\",\"TClonesArray\",gAnalyzer->Get%sAddress(),%s,%s);\n",branchName[i][j].Data(),branchFolder[i][j].Data(),branchBufferSize[i][j].Data(),branchSplitLevel[i][j].Data());
         }
         buffer.AppendFormatted("   tree->GetBranch(\"%s\")->SetCompressionLevel(gAnalyzer->GetTreeObjectAt(%d)->GetCompressionLevel());\n",branchName[i][j].Data(),i);
      }
   }
   buffer.AppendFormatted("}\n\n");

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
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]=="1") {
            buffer.AppendFormatted("   new(gAnalyzer->Get%s()) %s%s( ",folderName[i].Data(),shortCut.Data(),folderName[i].Data());
            for (j=0;j<numOfValue[i];j++) {
               if (isFolder(valueType[i][j].Data()))
                  continue;
               if (valueDimension[i][j]==0)
                  buffer.AppendFormatted("%s,",valueInit[i][j].Data());
            }
            buffer.Resize(buffer.Length()-1);
            buffer.AppendFormatted(" );\n");
         }
      }
   }
   buffer.AppendFormatted("};\n\n");

   // Initialize Array Folders
   buffer.AppendFormatted("void %sEventLoop::InitArrayFolders() {\n",shortCut.Data());
   buffer.AppendFormatted("   int i=0;\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] > 0 && !folderSupport[i]) {
         if (folderArray[i]!="1" && folderArray[i]!="variable")
            buffer.AppendFormatted("   gAnalyzer->Set%sSize(%s);\n",folderName[i].Data(),folderArray[i].Data());
      }
   }
   buffer.AppendFormatted("};\n\n");

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
   buffer.AppendFormatted("   bool write = false;\n");
   buffer.AppendFormatted("   bool written = false;\n");
   for (i=0;i<numOfTree;i++) {
      buffer.AppendFormatted("   write = false;\n");
      buffer.AppendFormatted("   romeTree = (ROMETree*)gAnalyzer->GetTreeObjectAt(%d);\n",i);
      buffer.AppendFormatted("   if (romeTree->isFill()) {\n");
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
      buffer.AppendFormatted("         fTreeInfo->SetSequentialNumber(fSequentialNumber);\n");
      buffer.AppendFormatted("         romeTree->GetTree()->Fill();\n");
      buffer.AppendFormatted("      }\n");
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("   if (written) {\n");
   buffer.AppendFormatted("      fSequentialNumber++;\n");
   buffer.AppendFormatted("      stat->writtenEvents++;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // get tree file names
   buffer.AppendFormatted("// Get Tree File Names\n");
   buffer.AppendFormatted("void %sEventLoop::GetTreeFileName(ROMEString& buffer,Int_t treeIndex,const char* runNumber) {\n",shortCut.Data());
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
      buffer.AppendFormatted("         buffer.ReplaceAll(\"#\",runNumber);\n");
      buffer.AppendFormatted("         break;\n");
   }
   if (numOfTree>0) {
      buffer.AppendFormatted("   }\n");
   }
   buffer.AppendFormatted("}\n");
   buffer.AppendFormatted("\n");

   // Initialize Task Switches
   ROMEString taskNameT;
   int parentInd;
   buffer.AppendFormatted("void %sEventLoop::InitTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      parentInd = taskHierarchyParentIndex[i];
      taskNameT.SetFormatted("%s%03d",taskHierarchyName[i].Data(),i);
      while (parentInd!=-1) {
         taskNameT.Insert(0,"_");
         taskNameT.InsertFormatted(0,"%s%03d",taskHierarchyName[parentInd].Data(),parentInd);
         parentInd = taskHierarchyParentIndex[parentInd];
      }
      buffer.AppendFormatted("   gAnalyzer->GetTaskSwitches()->%s = gAnalyzer->GetTaskObjectAt(%d)->IsActive();\n",taskNameT.Data(),taskHierarchyObjectIndex[i]);
   }
   buffer.AppendFormatted("};\n\n");

   // Update Task Switches
   buffer.AppendFormatted("void %sEventLoop::UpdateTaskSwitches() {\n",shortCut.Data());
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      parentInd = taskHierarchyParentIndex[i];
      taskNameT.SetFormatted("%s%03d",taskHierarchyName[i].Data(),i);
      while (parentInd!=-1) {
         taskNameT = taskNameT.Insert(0,"_");
         taskNameT.InsertFormatted(0,"%s%03d",taskHierarchyName[parentInd].Data(),parentInd);
         parentInd = taskHierarchyParentIndex[parentInd];
      }
      buffer.AppendFormatted("   if (gAnalyzer->GetTaskSwitches()->%s)\n",taskNameT.Data());
      buffer.AppendFormatted("      gAnalyzer->GetTaskObjectAt(%d)->SetActive(true);\n",taskHierarchyObjectIndex[i]);
      buffer.AppendFormatted("   else\n");
      buffer.AppendFormatted("      gAnalyzer->GetTaskObjectAt(%d)->SetActive(false);\n",taskHierarchyObjectIndex[i]);
   }
   buffer.AppendFormatted("};\n\n");

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
   }
   buffer.AppendFormatted("   ROMEString filename;\n");
   buffer.AppendFormatted("   filename.SetFormatted(\"%%s%%s%%05d.root\",gAnalyzer->GetInputDir(),\"histos\",gAnalyzer->GetHistosRun());\n");
   buffer.AppendFormatted("   TFile *file = new TFile(filename.Data(),\"READ\");\n");
   buffer.AppendFormatted("   if (file->IsZombie()) {\n");
   buffer.AppendFormatted("       cout << \"Histograms of run \" << gAnalyzer->GetHistosRun() << \" not available.\" << endl;\n");
   buffer.AppendFormatted("       cout << \"Please check the run number and the input path.\" << endl << endl;\n");
   buffer.AppendFormatted("       cout << \"No Histogram loaded!\" << endl << endl;\n");
   buffer.AppendFormatted("       return;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("   file->FindObjectAny(\"histos\");\n");
   for (i=0;i<numOfTaskHierarchy;i++) {
      if (!taskUsed[taskHierarchyClassIndex[i]])
         continue;
      for (j=0;j<numOfHistos[taskHierarchyClassIndex[i]];j++) {
         if (histoArraySize[taskHierarchyClassIndex[i]][j]=="1") {
            buffer.AppendFormatted("   %s* %sTemp_%d = ((%s*)file->FindObjectAny(\"%s\"));\n",histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),i,histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   if (%sTemp_%d==NULL)\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i);
            buffer.AppendFormatted("      cout << \"Histogram '%s' not available in run \" << gAnalyzer->GetHistosRun() << \"!\" << endl;\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   else\n");
            buffer.AppendFormatted("      %sTemp_%d->Copy(*gAnalyzer->Get%s());\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data());
         }
         else {
            buffer.AppendFormatted("   nentry = gAnalyzer->Get%s()->GetEntries();\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   for (i=0;i<nentry;i++) {\n");
            buffer.AppendFormatted("      %s* %sTemp_%d = ((%s*)file->FindObjectAny(gAnalyzer->Get%sAt(i)->GetName()));\n",histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data(),i,histoType[taskHierarchyClassIndex[i]][j].Data(),histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      if (%sTemp_%d==NULL)\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i);
            buffer.AppendFormatted("         cout << \"Histogram '%s' not available in run \" << gAnalyzer->GetHistosRun() << \"!\" << endl;\n",histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("      else\n");
            buffer.AppendFormatted("         %sTemp_%d->Copy(*gAnalyzer->Get%sAt(i));\n",histoName[taskHierarchyClassIndex[i]][j].Data(),i,histoName[taskHierarchyClassIndex[i]][j].Data());
            buffer.AppendFormatted("   };\n");
         }
      }
   }
   buffer.AppendFormatted("};\n\n");

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

   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TTree.h\"\n");
   buffer.AppendFormatted("#include \"TFolder.h\"\n");
   buffer.AppendFormatted("#include \"TClonesArray.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMEString.h\"\n");
   buffer.AppendFormatted("#include \"ROMEEventLoop.h\"\n");

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
   buffer.AppendFormatted("   void GetTreeFileName(ROMEString& buffer,Int_t treeIndex,const char* runNumber=\"\");\n");
   buffer.AppendFormatted("\n");

   // Task Switches
   buffer.AppendFormatted("   // Task Switches\n");
   buffer.AppendFormatted("   void InitTaskSwitches();\n");
   buffer.AppendFormatted("   void UpdateTaskSwitches();\n");
   buffer.AppendFormatted("\n");

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

   cppFile.SetFormatted("%ssrc/generated/main.cpp",outDir.Data());

   buffer.Resize(0);
   WriteHeader(buffer, mainAuthor.Data(), kTRUE);

   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"TFolder.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"ROMERint.h\"\n");
   buffer.AppendFormatted("#include \"include/generated/%sWindow.h\"\n",shortCut.Data());
   buffer.AppendFormatted("#include \"include/generated/%sAnalyzer.h\"\n",shortCut.Data());
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#include \"Windows.h\"\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"Riostream.h\"\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("int main(int argc, char *argv[])\n");
   buffer.AppendFormatted("{\n");
   buffer.AppendFormatted("   int i;\n");
   buffer.AppendFormatted("   bool graphics = true;\n");
   buffer.AppendFormatted("   char str[200];\n");
   buffer.AppendFormatted("   int argn = 1;\n");
   buffer.AppendFormatted("   char arg[1][100];\n");
   buffer.AppendFormatted("   char *argp = &arg[0][0];\n");
   buffer.AppendFormatted("   strcpy(arg[0],argv[0]);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   ROMERint *app = new ROMERint(\"App\", &argn, &argp,NULL,0,true);\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   for (i=1;i<argc;i++) {\n");
   buffer.AppendFormatted("      if (!strcmp(argv[i],\"-ng\")) {\n");
   buffer.AppendFormatted("         graphics = false;\n");
   buffer.AppendFormatted("         break;\n");
   buffer.AppendFormatted("      }\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   new %sAnalyzer(app,graphics);\n",shortCut.Data());
   buffer.AppendFormatted("\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("   sprintf(str,\"ROME - %%s\", gAnalyzer->GetProgramName());\n");
   buffer.AppendFormatted("   SetConsoleTitle(str);\n");
   buffer.AppendFormatted("\n");
#endif
   buffer.AppendFormatted("   TFolder *fMainFolder = gROOT->GetRootFolder()->AddFolder(\"ROME\",\"ROME Folder\");\n");
   buffer.AppendFormatted("   fMainFolder->Add(gAnalyzer);\n");
   buffer.AppendFormatted("   gAnalyzer->SetCintInitialisation(\"%sAnalyzer* gAnalyzer = ((%sAnalyzer*)((TFolder*)gROOT->FindObjectAny(\\\"ROME\\\"))->GetListOfFolders()->MakeIterator()->Next());\");\n",shortCut.Data(),shortCut.Data());
   buffer.AppendFormatted("   app->ProcessLine(gAnalyzer->GetCintInitialisation());\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   if (!gAnalyzer->Start(argc, argv)) {\n");
   buffer.AppendFormatted("      delete gAnalyzer;\n");
   buffer.AppendFormatted("      return 1;\n");
   buffer.AppendFormatted("   }\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   delete gAnalyzer;\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("   return 0;\n");
   buffer.AppendFormatted("}\n");

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
   buffer.Resize(0);
   buffer.AppendFormatted("<!--\n");
   buffer.AppendFormatted("   *** This file will be overwritten by the ROMEBuilder ***\n");
   buffer.AppendFormatted("   ***      Don't make manual changes to this file      ***\n");
   buffer.AppendFormatted("-->\n\n");

   buffer.AppendFormatted("<HTML>\n");
   buffer.AppendFormatted("<HEAD>\n");
   buffer.AppendFormatted("<TITLE>%s%s Manual</TITLE>\n",shortCut.Data(),mainProgName.Data());
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
   buffer.AppendFormatted("<H2>Table of Contents</H2>\n");
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#introduction\">Introduction</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#steers\">Steering Parameters</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#objects\">Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<ul>\n");
   buffer.AppendFormatted("<li><a href=\"#taskobjects\">Tasks</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#folderobjects\">Folders</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#treeobjects\">Trees</a></li>\n");
   if (numOfEvent > 0)
      buffer.AppendFormatted("<li><a href=\"#midasbankobjects\">Midas Banks</a></li>\n");
   buffer.AppendFormatted("</ul>\n");
   if (affiliationList.GetEntries() > 0)
      buffer.AppendFormatted("<li><a href=\"#affiliations\">Affiliations</a></li>\n");
   buffer.AppendFormatted("<li><a href=\"#accessmethods\">Access Methods to Objects in the %s%s</a></li>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"htmldoc/ClassIndex.html\">Class Overview</A></li>\n");
//   buffer.AppendFormatted("<li><A TARGET=_top HREF=\"%s/%sUserHTML.html\">Additional Info</A></li>\n",outDir.Data(),shortCut.Data());
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
   // Introduction
   buffer.AppendFormatted("<H2><a name=introduction>Introduction</a> </H2>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("%s\n",mainDescription.Data());
   buffer.AppendFormatted("<p>\n");
   // Steering parameters
   buffer.AppendFormatted("<H2><a name=steers>Steering Parameters</a> </H2>\n");
   buffer.AppendFormatted("\n");
   for (i=0;i<numOfTask+numOfTab+1;i++) {
      if (i<numOfTask && !taskUsed[i])
         continue;
      if (i>numOfTask && !tabUsed[i-numOfTask-1])
         continue;
      if (numOfSteering[i] < 1)
         continue;
      if (i<numOfTask)
         buffer.AppendFormatted("<a class=\"object\">%s</a><br>\n",taskName[i].Data());
      else if (i>numOfTask)
         buffer.AppendFormatted("%s<br>\n",tabName[i-numOfTask-1].Data());
      else
         buffer.AppendFormatted("<a class=\"object\">Global Steering Parameters</a><br>\n");
      buffer.AppendFormatted("<table>\n");
      buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      trodd = !trodd;
      WriteHTMLSteering(buffer,0,i,"");
      buffer.AppendFormatted("</table><br>\n");
   }
   buffer.AppendFormatted("\n");
   // Objects
   buffer.AppendFormatted("<H2><a name=objects>Objects in the %s%s</a> </H2>\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("All <a href=\"#taskobjects\">Tasks</a>, <a href=\"#folderobjects\">Folders</a>");
   if (numOfEvent > 0)
      buffer.AppendFormatted(", <a href=\"#midasbankobjects\">Midas Banks</a>");
   buffer.AppendFormatted(" and <a href=\"#treeobjects\">Trees</a> are described here.\n");
   // Tasks
   buffer.AppendFormatted("<h3><a name=taskobjects>Tasks</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s has the following task hierarchy :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");

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
      buffer.AppendFormatted("<li type=\"circle\"><h4><a href=\"#%s\">%sT%s</a></h4></li>\n",taskHierarchyName[i].Data(),shortCut.Data(),taskHierarchyName[i].Data());
      depthold = depth;
   }
   for (i=0;i<depth;i++) buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("</ul>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<p><b>Note</b> : The user should write code into the Init(), BeginOfRun(), Event(), EndOfRun() and Terminate() methods of the tasks methods file (.cpp). But the user may not add code to or alter code in the class file (.h). The class file (.h) is generated by the framework. Manual changes to this file will allways be overwritten.\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("In the following all tasks will be described.\n");
   buffer.AppendFormatted("<p>\n");

   ROMEString cppFile;
   ROMEString str;
   fstream *fileStream;
   ROMEString fileBuffer;
   for (i=0;i<numOfTask;i++) {
      if (!taskUsed[i])
         continue;
      buffer.AppendFormatted("<a name=%s class=\"object\">%s</a><br>\n",taskName[i].Data(),taskName[i].Data());
      buffer.AppendFormatted("%s<p>\n",taskDescription[i].Data());
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
      buffer.AppendFormatted("<p>\n");
      cppFile.SetFormatted("%s/src/tasks/%sT%s.cpp",outDir.Data(),shortCut.Data(),taskName[i].Data());
      fileStream = new fstream(cppFile.Data(),ios::in);
      fileBuffer.ReadFile(*fileStream);
      delete fileStream;
      buffer.AppendFormatted("%s accesses data from the following folders :\n",taskName[i].Data());
      buffer.AppendFormatted("<ul>\n");
      for (j=0;j<numOfFolder;j++) {
         if (!folderUsed[j])
            continue;
         if (accessFolder(fileBuffer,j))
            buffer.AppendFormatted("<li type=\"circle\">%s</li>\n",folderName[j].Data());
      }
      buffer.AppendFormatted("</ul>\n");
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("For more information take a look at the <A TARGET=_top HREF=\"htmldoc/%sT%s.html\">class file</a>\n",shortCut.Data(),taskName[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("\n");
   }
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Folders
   buffer.AppendFormatted("<h3><a name=folderobjects>Folders</a></h3>\n");
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("The %s%s incorporates the following folders :\n",shortCut.Data(),mainProgName.Data());
   buffer.AppendFormatted("\n");
   buffer.AppendFormatted("<ul>\n");

   depthold=0;
   depth=0;

   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
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
         buffer.AppendFormatted("<li type=\"circle\"><a href=\"#%s\">%s</a></li>\n",folderName[i].Data(),folderName[i].Data());
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
   buffer.AppendFormatted("In the following all folders will be described.\n");
   buffer.AppendFormatted("<p>\n");
   for (i=0;i<numOfFolder;i++) {
      if (!folderUsed[i])
         continue;
      if (numOfValue[i] <= 0) continue;
      buffer.AppendFormatted("<a name=%s class=\"object\">%s</a><br>\n",folderName[i].Data(),folderName[i].Data());
      buffer.AppendFormatted("%s\n",folderDescription[i].Data());
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<u>Fields</u>\n");
      buffer.AppendFormatted("<table>\n");
      if (folderDataBase[i])
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Database</td><td>Description</td></tr>\n");
      else
         buffer.AppendFormatted("<tr class=\"cont\"><td>Name</td><td>Type</td><td>Description</td></tr>\n");
      for (j=0;j<numOfValue[i];j++) {
         ROMEString comment = valueComment[i][j];
         if (valueComment[i][j].Length()>3) {
            if (valueComment[i][j][0]=='/') {
               comment = valueComment[i][j](3,valueComment[i][j].Length()-3);
            }
         }
         buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td>",trodd ? "odd" : "even",valueName[i][j].Data(),valueType[i][j].Data());
         if (folderDataBase[i]) {
            if (valueDBPath[i][j].Length() || valueDBName[i][j].Length())
               buffer.AppendFormatted("<td><center><a href=\"#db_%s_%s\">Yes</a></center></td>",folderName[i].Data(),valueName[i][j].Data());
            else
               buffer.AppendFormatted("<td><center>No</center></td>");
         }
         buffer.AppendFormatted("<td>&nbsp;%s&nbsp;</td></tr>\n",comment.Data());
         trodd = !trodd;
      }
      buffer.AppendFormatted("</table><br>\n");
   }
   buffer.AppendFormatted("<p>\n");

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
      buffer.AppendFormatted("List of defualt database connections. These values can be overwritten by configuration file.\n");
      buffer.AppendFormatted("<p>\n");
   }
   Int_t iDB;
   for (iDB=0;iDB<dbList.GetEntries();iDB++) {
      buffer.AppendFormatted("<u>%s</u>\n", dbList.At(iDB).Data());
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
      buffer.AppendFormatted("</table><br>\n");
   }
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Trees
   buffer.AppendFormatted("<h3><a name=treeobjects>Trees</a></h3>\n");
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
   buffer.AppendFormatted("<hr>\n");
   buffer.AppendFormatted("<p>\n");

   // Midas Banks
   if (numOfEvent > 0 ) {
      buffer.AppendFormatted("<h3><a name=midasbankobjects>Midas Banks</a></h3>\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("The %s%s incorporates the following midas banks :\n",shortCut.Data(),mainProgName.Data());
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
      buffer.AppendFormatted("<hr>\n");
      buffer.AppendFormatted("<p>\n");
   }

   // Affiliations
   if (affiliationList.GetEntries() > 0 ) {
      buffer.AppendFormatted("<h3><a name=affiliations>Affiliations</a></h3>\n");
      buffer.AppendFormatted("\n");
      buffer.AppendFormatted("The %s%s incorporates the following affiliations.\n",shortCut.Data(),mainProgName.Data());
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
         buffer.AppendFormatted("<a name=\"%saff\" class=\"object\">%s</a><br>\n", affiliationList.At(i, 0).Data(), affiliationList.At(i, 0).Data());
         buffer.AppendFormatted("<table>\n");
         buffer.AppendFormatted("<tr class=\"cont\"><td>Type</td><td>Name</td></tr>\n");
         for (j=1;j<affiliationList.GetEntriesAt(i);j+=2) {
            buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even", affiliationList.At(i, j).Data(), affiliationList.At(i, j+1).Data());
            trodd = !trodd;
         }
         buffer.AppendFormatted("</table><br>\n");
      }
      buffer.AppendFormatted("<p>\n");
      buffer.AppendFormatted("<hr>\n");
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

   // Footer
   buffer.AppendFormatted("<HR>\n");
   buffer.AppendFormatted("<p>\n");
   buffer.AppendFormatted("\n");
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
   htmlFile.SetFormatted("%s/%s%s.html",outDir.Data(),shortCut.Data(),mainProgName.Data());
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
   ROMEString comment;
   ROMEString groupName;
   bool trodd = true;
   for (k=0;k<numOfSteerFields[numTask][numSteer];k++) {
      if (!steerFieldUsed[numTask][numSteer][k])
         continue;
      comment = steerFieldComment[numTask][numSteer][k];
      if (steerFieldComment[numTask][numSteer][k].Length()>3) {
         if (steerFieldComment[numTask][numSteer][k][0]=='/') {
            comment = steerFieldComment[numTask][numSteer][k](3,steerFieldComment[numTask][numSteer][k].Length()-3);
         }
      }
      buffer.AppendFormatted("<tr class=\"%s\"><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td><td>&nbsp;%s&nbsp;</td></tr>\n",trodd ? "odd" : "even",steerFieldName[numTask][numSteer][k].Data(),steerFieldType[numTask][numSteer][k].Data(),comment.Data());
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
   ROMEString tmp,tmp2;
   ROMEString macroDescription;
   buffer.Resize(0);
   macroDescription.Resize(0);

   // File name
   cFile.SetFormatted("%ssrc/generated/%sReadTrees.C", outDir.Data(), shortCut.Data());
   WriteHeader(buffer, mainAuthor, true);
   macroDescription.AppendFormatted("This macro shows how to read output file from %s%s.exe.\n\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   macroDescription.AppendFormatted(" Usage\n");
   macroDescription.AppendFormatted("   %% make so;\n");
   macroDescription.AppendFormatted("   %% root;\n");
   macroDescription.AppendFormatted("   root [0] gSystem->Load(\"lib%s%s.so\");\n", shortCut.ToLower(tmp), mainProgName.ToLower(tmp2));
   macroDescription.AppendFormatted("   root [1] .L src/generated/%sReadTrees.C\n", shortCut.Data());
   macroDescription.AppendFormatted("   root [2] %sReadTrees(1, 0, 10)\n", shortCut.Data());
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
   buffer.AppendFormatted("#include \"RConfig.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( push )\n");
   buffer.AppendFormatted("#pragma warning( disable : 4800 )\n");
#endif // R__VISUAL_CPLUSPLUS
//   buffer.AppendFormatted("#include \"TSystem.h\"\n");
   buffer.AppendFormatted("#include \"TTree.h\"\n");
   buffer.AppendFormatted("#include \"TFile.h\"\n");
   buffer.AppendFormatted("#include \"TClonesArray.h\"\n");
#if defined( R__VISUAL_CPLUSPLUS )
   buffer.AppendFormatted("#pragma warning( pop )\n");
#endif // R__VISUAL_CPLUSPLUS
   buffer.AppendFormatted("#include \"Riostream.h\"\n");

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

Bool_t ROMEBuilder::ReplaceHeader(const char* filename,const char* header,const char* body,Int_t nspace,const char* str1, const char* str2)
{
   bool writeFile = false;
   fstream *fileStream;
   ROMEString fileBuffer;
   Ssiz_t pBuffer=-1;
   ROMEString buffer = header;
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
      pBuffer = fileBuffer.Index(kHeaderEndMark);
      if (pBuffer<0) {
         if (makeOutput)
            cout << "\n\nWarning : File '" << filename << "' does not have header end mark. Builder does not modify this file." << endl;
         return true;
      }
      // compare old and new header
      if (fileBuffer(0,pBuffer+80) != header)
         writeFile = true;
      //check if the file has string to be replaced.
      if (str1  && fileBuffer.Contains(str1)) {
         writeFile = true;
         fileBuffer.ReplaceAll(str1, str2);
      }
   }
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
   // return true when backup file is creaded
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
         if (descs[i].BeginsWith("#include")) {
            buffer.AppendFormatted("%s\n", descs[i].Data());
         }
         else {
            format.SetFormatted("// %%s%%%ds //\n", (int)(nc - strlen("//  //") - descs[i].Length()));
            buffer.AppendFormatted(format.Data(), descs[i].Data(), "");
         }
#else
         if (descs[i].BeginsWith("#include")) {
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

