/********************************************************************

  main.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
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

   ROMEBuilder* romeb = new ROMEBuilder();

   romeb->romeVersion = "Version 2.4";
   
   ROMEString xmlFile = "";

   if (!romeb->ReadCommandLineParameters(argc,argv))
      return 0;

   if (!romeb->CheckFileAndPath())
      return 0;

   if (!romeb->StartBuilder())
      return 0;

   bool noLink = romeb->GetNoLink();
   bool makeOutput = romeb->GetMakeOutput();

   delete romeb;

   if (!noLink) {
      if(makeOutput)
         cout<<"Linking the executable binary."<<endl;
#if defined( R__UNIX )
      return (gSystem->Exec("make -e")) ? EXIT_FAILURE : EXIT_SUCCESS;
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      return gSystem->Exec("nmake -f Makefile.win");
#endif
   }

   return 0;
}
