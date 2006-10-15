/********************************************************************

  main.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TROOT.h>
#include <TSystem.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#if defined( R__UNIX )
#   include <unistd.h>
#endif
#ifndef R__MACOSX
#   if defined( R__VISUAL_CPLUSPLUS )
#      include <io.h>
#   endif
#   if defined( R__UNIX )
#      include <sys/io.h>
#   endif
#endif
#include <Riostream.h>
#include "ROMEBuilder.h"
#include "ROMEVersion.h"

int main(int argc, char *argv[])
{
   char* romesys = getenv("ROMESYS");
   if (romesys==NULL) {
      cout << "Please set the environment variable ROMESYS to the ROME root-directory." << endl;
      return 0;
   }
   char* rootsys = getenv("ROOTSYS");
   if (rootsys==NULL) {
      cout << "Please set the environment variable ROOTSYS to the ROOT root-directory." << endl;
      return 0;
   }

   gROOT->SetBatch();
   ROMEBuilder* romeb = new ROMEBuilder();

   romeb->romeVersion = "Version ";
   romeb->romeVersion += ROME_RELEASE;

   ROMEString xmlFile = "";

   if (!romeb->ReadCommandLineParameters(argc,argv))
      return 0;

   if (!romeb->CheckFileAndPath())
      return 0;

   if (!romeb->StartBuilder())
      return 0;

   bool noLink = romeb->GetNoLink();
   bool makeOutput = romeb->GetMakeOutput();
   ROMEString *makeFlag = romeb->GetMakeFlag();
   ROMEString makeCommand;
#if defined( R__UNIX )
   makeCommand = "make";
#else
   makeCommand = "nmake -f Makefile.win";
#endif
   if (makeFlag->Length()) {
      makeCommand += " ";
      makeCommand += makeFlag->Data();
   }

   delete romeb;

   if (!noLink) {
      if(makeOutput)
         cout<<"Linking the executable binary."<<endl;
#if defined( R__UNIX )
      return (gSystem->Exec(makeCommand.Data())) ? EXIT_FAILURE : EXIT_SUCCESS;
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      return gSystem->Exec(makeCommand.Data());
#endif
   }

   return 0;
}
