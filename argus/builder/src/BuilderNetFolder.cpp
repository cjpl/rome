/********************************************************************

  BuilderNetFolder.cpp, Ryu Sawada

  $Log$
  Revision 1.2  2005/10/05 19:13:24  sawada
  better color of documents.
  steering parameters in documents.
  CSS suport.
  ROMETextDataBase support.
  userclean in Makefile.
  removed unused variables.

  Revision 1.1  2005/05/26 14:26:54  sawada
  Lots of changes.
  Made ArgusBuilder an inheriting class of ROMEBuilder.
  Remove ROMEFolder and added NetFolers.
  Added ArgusWindow class.
  and so on.


********************************************************************/
#include "ArgusBuilder.h"


Bool_t ArgusBuilder::ReadXMLNetFolder() {
   // read the net folder definitions out of the xml file
   Int_t   type;
   Char_t* name;
   ROMEString currentNetFolderName = "";

   // count netFolders
   numOfNetFolder++;
   if (numOfNetFolder>=maxNumberOfNetFolders) {
      cout << "Maximal number of net folders reached : " << maxNumberOfNetFolders << " !" << endl;
      cout << "Terminating program." << endl;
      return kFALSE;
   }

   // initialization
   netFolderName[numOfNetFolder] = "";
   netFolderTitle[numOfNetFolder] = "";
   netFolderHost[numOfNetFolder] = "localhost";
   netFolderPort[numOfNetFolder] = "9090";
   netFolderRoot[numOfNetFolder] = shortCut;

   while (xml->NextLine()) {
      type = xml->GetType();
      name = xml->GetName();
      // end net object
      if (type == 15 && !strcmp((const Char_t*)name,"NetFolder")) {
         // check input
         if (netFolderName[numOfNetFolder]=="") {
            cout << "The " << (numOfNetFolder+1) << ". NetFolder has no name !" << endl;
            cout << "Terminating program." << endl;
            return kFALSE;
         }
         return kTRUE;
      }

      // net object name
      if (type == 1 && !strcmp((const Char_t*)name,"NetFolderName")) {
         xml->GetValue(netFolderName[numOfNetFolder],netFolderName[numOfNetFolder]);
         // output
         if (makeOutput) netFolderName[numOfNetFolder].WriteLine();
      }

      // net object title
      if (type == 1 && !strcmp((const Char_t*)name,"NetFolderTitle"))
         xml->GetValue(netFolderTitle[numOfNetFolder],netFolderTitle[numOfNetFolder]);

      // net object host
      if (type == 1 && !strcmp((const Char_t*)name,"NetFolderHost"))
         xml->GetValue(netFolderHost[numOfNetFolder],netFolderHost[numOfNetFolder]);

      // net object port
      if (type == 1 && !strcmp((const Char_t*)name,"NetFolderPort"))
         xml->GetValue(netFolderPort[numOfNetFolder],netFolderPort[numOfNetFolder]);

      // net object Root
      if (type == 1 && !strcmp((const Char_t*)name,"NetFolderRoot"))
         xml->GetValue(netFolderRoot[numOfNetFolder],netFolderRoot[numOfNetFolder]);
   }
   return kTRUE;
}
