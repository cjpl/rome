// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMERunTable                                                         //
//                                                                      //
//  RunTable Object 
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "ROMERunTable.h"

ClassImp(ROMERunTable)

TString*  ROMERunTable::GetFile()           
{ 
   return fFile;           
}

void ROMERunTable::SetFile(TString* file)
{
   fFile = new TString[fNumberOfFiles];
   for (int i=0;i<fNumberOfFiles;i++) {
      fFile[i] = file[i];
   }
}

void ROMERunTable::SetFolder(TString* folder)
{ 
/*   fFolder = new TString[fNumberOfFiles];
   for (int i=0;i<fNumberOfFiles;i++) {
      fFolder[i] = folder[i];
   }*/
}
