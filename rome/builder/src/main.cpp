/********************************************************************

  main.cpp, M. Schneebeli PSI

  $Id$

********************************************************************/
#include <RConfig.h>
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
#include <ROMEBuilder.h>
#include <Riostream.h>
#include <TSystem.h>

int main(int argc, char *argv[])
{
   char* romesys = getenv("ROMESYS");
   if (romesys==NULL) {
      cout << "Please set the environment variable ROMESYS to the ROME root-directory." << endl;
      return 1;
   }
   char* rootsys = getenv("ROOTSYS");
   if (rootsys==NULL) {
      cout << "Please set the environment variable ROOTSYS to the ROOT root-directory." << endl;
      return 1;
   }

   ROMEBuilder* romeb = new ROMEBuilder();

   romeb->romeVersion = "Version 2.4";
   
   ROMEString xmlFile = "";

   if (!romeb->ReadCommandLineParameters(argc,argv))
      return 0;

   if (!romeb->CheckFileAndPath())
      return 0;

   romeb->StartBuilder();

   bool noLink = romeb->GetNoLink();
   bool makeOutput = romeb->GetMakeOutput();

   delete romeb;

   if (!noLink) {
      if(makeOutput)
         cout<<"Linking the executable binary."<<endl;
#if defined( R__UNIX )
      return gSystem->Exec("make -e");
#endif
#if defined( R__VISUAL_CPLUSPLUS )
      return gSystem->Exec("nmake -f Makefile.win");
#endif
   }

   return 0;
}
