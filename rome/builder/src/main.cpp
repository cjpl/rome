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

   romeb->romeVersion = "Version 1.1";
   
   ROMEString xmlFile = "";

   if (!romeb->ReadCommandLineParameters(argc,argv))
      return 0;

   if (!romeb->CheckFileAndPath())
      return 0;

   ROMEString path;
   path.SetFormatted("%s/obj",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/dict",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/src",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/src/tasks",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/src/framework",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/include/",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/include/tasks",romeb->outDir.Data());
   romeb->MakeDir(path);
   path.SetFormatted("%s/include/framework",romeb->outDir.Data());
   romeb->MakeDir(path);

   romeb->StartBuilder();

   return 0;
}
