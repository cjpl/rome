/********************************************************************

  BuilderSteering.cpp, Ryu Sawada

  $Log$
  Revision 1.4  2005/03/28 10:54:37  sawada
  removed tab hierarchy.
  made ReadXMLMenu.

  Revision 1.3  2005/03/01 00:24:35  sawada
  bug fix around global steering parameter.

  Revision 1.2  2005/02/24 15:04:03  sawada
  Reduced number of configuration to 1.
  Replaced ss_getchar to getchar().
  Removed LineToProcess.
  Removed bk_is32,bk_find.
  Improved help.
  Handling of midas host and experiment.

  Revision 1.1.1.1  2005/01/29 22:45:08  sawada
  Advanced Root based GUi monitoring System


********************************************************************/
#include "ArgusBuilder.h"

bool ArgusBuilder::ReadXMLSteering(int iTab) {
   // read the steering parameter definitions out of the xml file
   ROMEString tmp;
   char* name;
   int type,i;
   ROMEString currentSteeringName = "";
   int currentNumberOfSteerings;
   // count steering parameters
   numOfSteering[iTab]++;
   currentNumberOfSteerings = numOfSteering[iTab];
   if (numOfSteering[iTab]>=maxNumberOfTabs+1) {
      cout << "Maximal number of steering parameters reached : " << (maxNumberOfTabs+1) << " !" << endl;
      cout << "Terminating program." << endl;
      return false;
   }
   // initialisation
   if (numOfSteering[iTab]>0)
      steerName[iTab][numOfSteering[iTab]] = "";
   numOfSteerFields[iTab][numOfSteering[iTab]] = 0;
   numOfSteerChildren[iTab][numOfSteering[iTab]] = 0;
   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end
      if (type == 15 && !strcmp((const char*)name,"SteeringParameters"))
         return true;
      if (type == 15 && !strcmp((const char*)name,"GlobalSteeringParameters"))
         return true;
      // subgroup
      if (type == 1 && !strcmp((const char*)name,"SteeringParameterGroup")) {
         // set steering parameter group as parent for subsequent groups
         steerParent[iTab][numOfSteering[iTab]+1] = actualSteerIndex;
         actualSteerIndex = numOfSteering[iTab]+1;
         recursiveSteerDepth++;
         // read subgroup
         if (!ReadXMLSteering(iTab)) 
            return false;
         continue;
      }
      // end group
      if (type == 15 && !strcmp((const char*)name,"SteeringParameterGroup")) {
         // check input
         if (currentSteeringName=="") {
            cout << "The " << (currentNumberOfSteerings+1) << ". Steering Parameter Group has no name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         actualSteerIndex = steerParent[iTab][actualSteerIndex];
         recursiveSteerDepth--;
         return true;
      }
      // group name
      if (type == 1 && !strcmp((const char*)name,"SPGroupName")) {
         xml->GetValue(steerName[iTab][actualSteerIndex],steerName[iTab][actualSteerIndex]);
         currentSteeringName = steerName[iTab][actualSteerIndex];
         if (steerParent[iTab][numOfSteering[iTab]]!= -1) {
            steerChildren[iTab][steerParent[iTab][actualSteerIndex]][numOfSteerChildren[iTab][steerParent[iTab][actualSteerIndex]]] = actualSteerIndex;
            numOfSteerChildren[iTab][steerParent[iTab][actualSteerIndex]]++;
         }
         // output
         if (makeOutput) for (i=0;i<recursiveSteerDepth;i++) cout << "   ";
         if (makeOutput) steerName[iTab][actualSteerIndex].WriteLine();
      }
      // steering parameter field
      if (type == 1 && !strcmp((const char*)name,"SteeringParameterField")) {
         // include initialisation
         bool readName = false;
         bool readType = false;
         steerFieldName[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] = "";
         steerFieldType[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] = "";
         steerFieldComment[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] = "";
         while (xml->NextLine()) {
            type = xml->GetType();
            name = xml->GetName();
            // steering parameter field name
            if (type == 1 && !strcmp((const char*)name,"SPFieldName")) {
               readName = true;
               xml->GetValue(steerFieldName[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]],steerFieldName[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]]);
               // output
               if (makeOutput) for (i=0;i<recursiveSteerDepth+1;i++) cout << "   ";
               if (makeOutput) steerFieldName[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]].WriteLine();
            }
            // steering parameter field type
            if (type == 1 && !strcmp((const char*)name,"SPFieldType")) {
               readType = true;
               xml->GetValue(steerFieldType[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]],steerFieldType[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]]);
               if (steerFieldType[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] == "TString")
                  steerFieldInit[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] = "' '";
               else if (steerFieldType[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] == "std::string")
                  steerFieldInit[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] = "' '";
               else if (steerFieldType[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] == "TRef")
                  steerFieldInit[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]] = "NULL";
               else
                  steerFieldInit[iTab][numOfSteering[iTab]][numOfSteerFields[iTab][numOfSteering[iTab]]] = "0";
            }
            // steering parameter field initialization
            if (type == 1 && !strcmp((const char*)name,"SPFieldInitialization")) {
               if (!readName) {
                  cout << "Please specify a steering parameter field name befor the initial value in the " << (numOfSteering[iTab]+1) << ". steering parameter field in tab '" << tabName[iTab].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               if (!readType) {
                  cout << "Please specify a steering parameter field type befor the initial value in steering parameter field '" << steerFieldName[iTab][numOfSteering[iTab]][numOfSteerFields[iTab][numOfSteering[iTab]]].Data() << "' in tab  '" << tabName[iTab].Data() << "' !" << endl;
                  cout << "Terminating program." << endl;
                  return false;
               }
               xml->GetValue(steerFieldInit[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]],steerFieldInit[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]]);
            }
            // steering parameter field comment
            if (type == 1 && !strcmp((const char*)name,"SPFieldComment")) {
               xml->GetValue(steerFieldComment[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]],steerFieldComment[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]]);
               if (steerFieldComment[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]][0]!='/') {
                  steerFieldComment[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]].Insert(0,"// ");
               }
            }
            // steering parameter field end
            if (type == 15 && !strcmp((const char*)name,"SteeringParameterField"))
               break;
         }
         // check input
         if (steerFieldName[iTab][actualSteerIndex][numOfSteerFields[iTab][actualSteerIndex]]=="") {
            cout << "A steering parameter field of tab '" << tabName[iTab].Data() << "' has no Name !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         // count includes
         numOfSteerFields[iTab][actualSteerIndex]++;
         if (numOfSteerFields[iTab][actualSteerIndex]>=maxNumberOfSteering) {
            cout << "Maximal number of steering parameter fields in tab '" << tabName[iTab].Data() << "' reached : " << maxNumberOfSteering << " !" << endl;
            cout << "Terminating program." << endl;
            return false;
         }
         continue;
      }
   }
   return true;
}

bool ArgusBuilder::WriteSteering(int iTab) {
   int i;
   ROMEString hFile;
   ROMEString buffer;
   char fileBuffer[bufferLength];
   hFile.SetFormatted("%s/include/monitor/%sGlobalSteering.h",outDir.Data(),shortCut.Data());
   if (numOfSteering[numOfTab]==-1) {
      remove(hFile.Data());
      return true;
   }
   if (makeOutput) cout << "\n   Output Files:" << endl;
   // Description
   buffer.Resize(0);
   buffer.AppendFormatted("//// Author: %s\n",mainAuthor.Data());
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// Contains the global Steering Parameters                                    //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("// This file has been generated by the ArgusBuilder.                          //\n");
   buffer.AppendFormatted("// Manual changes to this file will always be overwritten by the builder.     //\n");
   buffer.AppendFormatted("//                                                                            //\n");
   buffer.AppendFormatted("////////////////////////////////////////////////////////////////////////////////\n\n");
   // Header
   buffer.AppendFormatted("#ifndef %sGlobalSteering_H\n",shortCut.Data());
   buffer.AppendFormatted("#define %sGlobalSteering_H\n\n",shortCut.Data());
   buffer.AppendFormatted("#include <string>\n\n");
   WriteSteeringClass(buffer,0,iTab,0);
   buffer.AppendFormatted("#endif   // %sGlobalSteering_H\n",shortCut.Data());
   int fileHandle = open(hFile.Data(),O_RDONLY);
   int nb = read(fileHandle,&fileBuffer, sizeof(fileBuffer));
   bool identical = true;
   if (nb==(int)buffer.Length()) {
      for (i=0;i<nb&&i<(int)buffer.Length();i++) {
         if (buffer[i] != fileBuffer[i]) {
            identical = false;
            break;
         }
      }
   }
   else
      identical = false;
   if (!identical) {
      fileHandle = open(hFile.Data(),O_TRUNC  | O_CREAT,S_IREAD | S_IWRITE  );
      close(fileHandle);
      fileHandle = open(hFile.Data(),O_RDWR  | O_CREAT,S_IREAD | S_IWRITE  );
      if (makeOutput) cout << "      " << hFile.Data() << endl;
      nb = write(fileHandle,buffer.Data(), buffer.Length());
      close(fileHandle);
   }
   return true;
}

bool ArgusBuilder::WriteSteeringClass(ROMEString &buffer,int numSteer,int numTab,int tab) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank="";
   int j,i;
   int typeLen = -1;
   int nameLen = -1;
   for (i=0;i<numOfSteerFields[numTab][numSteer];i++) {
      if (typeLen<(int)steerFieldType[numTab][numSteer][i].Length()) typeLen = steerFieldType[numTab][numSteer][i].Length();
      if (nameLen<(int)steerFieldName[numTab][numSteer][i].Length()) nameLen = steerFieldName[numTab][numSteer][i].Length();
   }
   for (i=0;i<numOfSteerChildren[numTab][numSteer];i++) {
      if (typeLen<(int)steerName[numTab][steerChildren[numTab][numSteer][i]].Length()+1) typeLen = steerName[numTab][steerChildren[numTab][numSteer][i]].Length()+1;
      if (nameLen<(int)steerName[numTab][steerChildren[numTab][numSteer][i]].Length()) nameLen = steerName[numTab][steerChildren[numTab][numSteer][i]].Length();
   }
   if (tab==0)
      sc = shortCut;
   else 
      sc = "";
   for (i=0;i<tab;i++)
      blank.Append("   ");   
   buffer.AppendFormatted("\n%sclass %s%s\n",blank.Data(),sc.Data(),steerName[numTab][numSteer].Data());
   buffer.AppendFormatted("%s{\n",blank.Data());
   buffer.AppendFormatted("%sprotected:\n",blank.Data());
   for (i=0;i<numOfSteerChildren[numTab][numSteer];i++) {
      WriteSteeringClass(buffer,steerChildren[numTab][numSteer][i],numTab,tab+1);
   }
   // Fields
   for (j=0;j<numOfSteerFields[numTab][numSteer];j++) {
      format.SetFormatted("%%s   %%-%ds f%%s;%%%ds %%s\n",typeLen,nameLen-steerFieldName[numTab][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldType[numTab][numSteer][j].Data(),steerFieldName[numTab][numSteer][j].Data(),"",steerFieldComment[numTab][numSteer][j].Data());
   }
   for (i=0;i<numOfSteerChildren[numTab][numSteer];i++) {
      format.SetFormatted("%%s   %%-%ds *f%%s;%%%ds // Handle to %%s Class\n",typeLen-1,nameLen-steerName[numTab][steerChildren[numTab][numSteer][i]].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][i]].Data(),steerName[numTab][steerChildren[numTab][numSteer][i]].Data(),"",steerName[numTab][steerChildren[numTab][numSteer][i]].Data());
   }
   buffer.AppendFormatted("\n%spublic:\n",blank.Data());
   // Constructor
   buffer.AppendFormatted("%s   %s%s() { ",blank.Data(),sc.Data(),steerName[numTab][numSteer].Data());
   for (j=0;j<numOfSteerFields[numTab][numSteer];j++) {
      buffer.AppendFormatted("f%s = %s; ",steerFieldName[numTab][numSteer][j].Data(),steerFieldInit[numTab][numSteer][j].Data());
   }
   for (i=0;i<numOfSteerChildren[numTab][numSteer];i++) {
      buffer.AppendFormatted("f%s = new %s(); ",steerName[numTab][steerChildren[numTab][numSteer][i]].Data(),steerName[numTab][steerChildren[numTab][numSteer][i]].Data());
   }
   buffer.AppendFormatted("};\n");
   // Getters
   for (j=0;j<numOfSteerFields[numTab][numSteer];j++) {
      format.SetFormatted("%%s   %%-%ds Get%%s()%%%ds { return f%%s; };\n",typeLen,nameLen-steerFieldName[numTab][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldType[numTab][numSteer][j].Data(),steerFieldName[numTab][numSteer][j].Data(),"",steerFieldName[numTab][numSteer][j].Data());
   }
   for (i=0;i<numOfSteerChildren[numTab][numSteer];i++) {
      format.SetFormatted("%%s   %%-%ds *Get%%s()%%%ds { return f%%s; };\n",typeLen-1,nameLen-steerName[numTab][steerChildren[numTab][numSteer][i]].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][i]].Data(),steerName[numTab][steerChildren[numTab][numSteer][i]].Data(),"",steerName[numTab][steerChildren[numTab][numSteer][i]].Data());
   }
   // Setters
   buffer.AppendFormatted("\n");
   for (j=0;j<numOfSteerFields[numTab][numSteer];j++) {
      format.SetFormatted("%%s   void Set%%-%ds(%%-%ds %%s)%%%ds { f%%s = %%s; };\n",nameLen,typeLen,nameLen-steerFieldName[numTab][numSteer][j].Length());
      buffer.AppendFormatted((char*)format.Data(),blank.Data(),steerFieldName[numTab][numSteer][j].Data(),steerFieldType[numTab][numSteer][j].Data(),steerFieldName[numTab][numSteer][j].Data(),"",steerFieldName[numTab][numSteer][j].Data(),steerFieldName[numTab][numSteer][j].Data());
   }
   // Footer
   buffer.AppendFormatted("%s};\n\n",blank.Data());
   return true;
}

bool ArgusBuilder::WriteSteeringConfigClass(ROMEString &buffer,int numSteer,int numTab,int tab) {
   ROMEString format;
   ROMEString sc;
   ROMEString blank="";
   int i,k;
   for (i=0;i<tab;i++)
      blank.Append("   ");   
   for (k=0;k<numOfSteerFields[numTab][numSteer];k++) {
      buffer.AppendFormatted("%s   ROMEString  f%s;\n",blank.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s   bool        f%sModified;\n",blank.Data(),steerFieldName[numTab][numSteer][k].Data());
   }
   for (k=0;k<numOfSteerChildren[numTab][numSteer];k++) {
      buffer.AppendFormatted("%s   class %s {\n",blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      buffer.AppendFormatted("%s   public:\n",blank.Data());
      WriteSteeringConfigClass(buffer,steerChildren[numTab][numSteer][k],numTab,tab+1);
      buffer.AppendFormatted("%s   };\n",blank.Data());
      buffer.AppendFormatted("%s   %s *f%s;\n",blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      buffer.AppendFormatted("%s   bool f%sModified;\n",blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
   }
   buffer.AppendFormatted("%spublic:\n",blank.Data());
   buffer.AppendFormatted("%s   %s() {\n",blank.Data(),steerName[numTab][numSteer].Data());
   for (k=0;k<numOfSteerFields[numTab][numSteer];k++) {
      buffer.AppendFormatted("%s      f%sModified = false;\n",blank.Data(),steerFieldName[numTab][numSteer][k].Data());
   }
   for (k=0;k<numOfSteerChildren[numTab][numSteer];k++) {
      buffer.AppendFormatted("%s      f%s = new %s;\n",blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
   }
   buffer.AppendFormatted("%s   }\n",blank.Data());
   return true;
}

bool ArgusBuilder::WriteSteeringConfigRead(ROMEString &buffer,int numSteer,int numTab,ROMEXML *xml,ROMEString& path,ROMEString& pointer,ROMEString& classPath) {
   int k;
   ROMEString pathT;
   ROMEString pointerT;
   ROMEString classPathT;
   buffer.AppendFormatted("   %sModified = false;\n",pointer.Data());
   // Fields
   for (k=0;k<numOfSteerFields[numTab][numSteer];k++) {
      buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("   xml->GetPathValue(%s/child::SteeringParameterField[child::SPName='%s']/SPValue\",%s->f%s,\"\");\n",path.Data(),steerFieldName[numTab][numSteer][k].Data(),pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("   if (%s->f%s==\"\")\n",pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("      %s->f%sModified = false;\n",pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("   else {\n");
      buffer.AppendFormatted("      %s->f%sModified = true;\n",pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("      %sModified = true;\n",pointer.Data());
      buffer.AppendFormatted("   }\n");
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTab][numSteer];k++) {
      buffer.AppendFormatted("   // %s Group\n",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      buffer.AppendFormatted("   %s->f%s = new %s::%s();\n",pointer.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data(),classPath.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      pathT = path;
      pointerT = pointer;
      classPathT = classPath;
      path.AppendFormatted("/child::SteeringParameterGroup[child::SPGroupName='%s']",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      pointer.AppendFormatted("->f%s",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      classPath.AppendFormatted("::%s",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      WriteSteeringConfigRead(buffer,steerChildren[numTab][numSteer][k],numTab,xml,path,pointer,classPath);
      path = pathT;
      pointer = pointerT;
      classPath = classPathT;
      buffer.AppendFormatted("   if (%s->f%sModified)\n",pointer.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      buffer.AppendFormatted("      %sModified = true;\n",pointer.Data());
   }
   return true;
}

bool ArgusBuilder::WriteSteeringConfigWrite(ROMEString &buffer,int numSteer,int numTab,ROMEString& pointer,ROMEString& steerPointer,int tab) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   ROMEString blank = "";
   for (int i=0;i<tab;i++)
      blank.Append("   ");
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTab][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Field\n",blank.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s   if (%s->f%sModified || index==0) {\n",blank.Data(),pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterField\");\n",blank.Data());
      buffer.AppendFormatted("%s      xml->WriteElement(\"SPName\",\"%s\");\n",blank.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s      if (index==0) {\n",blank.Data());
      GetFormat(&value,(char*)steerFieldType[numTab][numSteer][k].Data());
      if (steerFieldType[numTab][numSteer][k]=="TString")
         buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s().Data());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      else if (steerFieldType[numTab][numSteer][k]=="std::string")
         buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s().c_str());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      else
         buffer.AppendFormatted("%s         value.SetFormatted(\"%s\",%s->Get%s());\n",blank.Data(),value.Data(),steerPointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"SPValue\",(char*)value.Data());\n",blank.Data());
      buffer.AppendFormatted("%s      }\n",blank.Data());
      buffer.AppendFormatted("%s      else if (%s->f%sModified)\n",blank.Data(),pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s         xml->WriteElement(\"SPValue\",(char*)%s->f%s.Data());\n",blank.Data(),pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
      buffer.AppendFormatted("%s   }\n",blank.Data());
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTab][numSteer];k++) {
      buffer.AppendFormatted("%s   // %s Group\n",blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      buffer.AppendFormatted("%s   if (%s->f%sModified || index==0) {\n",blank.Data(),pointer.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      buffer.AppendFormatted("%s      xml->StartElement(\"SteeringParameterGroup\");\n",blank.Data());
      buffer.AppendFormatted("%s      xml->WriteElement(\"SPGroupName\",\"%s\");\n",blank.Data(),steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      WriteSteeringConfigWrite(buffer,steerChildren[numTab][numSteer][k],numTab,pointer,steerPointer,tab+1);
      pointer = pointerT;
      steerPointer = steerPointerT;
      buffer.AppendFormatted("%s      xml->EndElement();\n",blank.Data());
      buffer.AppendFormatted("%s   }\n",blank.Data());
   }
   return true;
}

bool ArgusBuilder::WriteSteeringConfigSet(ROMEString &buffer,int numSteer,int numTab,ROMEString& pointer,ROMEString& steerPointer) {
   ROMEString pointerT;
   ROMEString steerPointerT;
   ROMEString decodedValue;
   ROMEString value;
   int k;
   // Fields
   for (k=0;k<numOfSteerFields[numTab][numSteer];k++) {
      buffer.AppendFormatted("   // %s Field\n",steerFieldName[numTab][numSteer][k].Data());
      buffer.AppendFormatted("   if (fConfigData%s->f%sModified) {\n",pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      value.SetFormatted("fConfigData%s->f%s",pointer.Data(),steerFieldName[numTab][numSteer][k].Data());
      setValue(&decodedValue,"",(char*)value.Data(),(char*)steerFieldType[numTab][numSteer][k].Data(),1);
      if (steerFieldType[numTab][numSteer][k]=="std::string")
         buffer.AppendFormatted("      %s->Set%s(%s.Data());\n",steerPointer.Data(),steerFieldName[numTab][numSteer][k].Data(),value.Data());
      else
         buffer.AppendFormatted("      %s->Set%s((%s)%s);\n",steerPointer.Data(),steerFieldName[numTab][numSteer][k].Data(),steerFieldType[numTab][numSteer][k].Data(),decodedValue.Data());
      buffer.AppendFormatted("   }\n");
   }
   // Groups
   for (k=0;k<numOfSteerChildren[numTab][numSteer];k++) {
      buffer.AppendFormatted("   // %s Group\n",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      pointerT = pointer;
      steerPointerT = steerPointer;
      pointer.AppendFormatted("->f%s",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      steerPointer.AppendFormatted("->Get%s()",steerName[numTab][steerChildren[numTab][numSteer][k]].Data());
      WriteSteeringConfigSet(buffer,steerChildren[numTab][numSteer][k],numTab,pointer,steerPointer);
      pointer = pointerT;
      steerPointer = steerPointerT;
   }
   return true;
}
