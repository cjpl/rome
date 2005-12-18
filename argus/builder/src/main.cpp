/********************************************************************

  main.cpp, Ryu Sawada

  $Id$

*******************************************************************/
#include "ArgusBuilder.h"

int main(int argc, char *argv[])
{
   Char_t *argussys = getenv("ARGUSSYS");
   if (argussys == NULL) {
      cout << "Please set the environment variable ARGUSSYS to the Argus root-directory." << endl;
      return 1;
   }
   Char_t *romesys = getenv("ROMESYS");
   if (romesys == NULL) {
      cout << "Please set the environment variable ROMESYS to the ROME root-directory." << endl;
      return 1;
   }
   Char_t *rootsys = getenv("ROOTSYS");
   if (rootsys == NULL) {
      cout << "Please set the environment variable ROOTSYS to the ROOT root-directory." << endl;
      return 1;
   }

   ArgusBuilder *argusb = new ArgusBuilder();
   argusb->argusVersion = ARGUS_RELEASE;

   if (!argusb->ReadCommandLineParameters(argc, argv))
      return 0;

   if (!argusb->CheckFileAndPath())
      return 0;

   argusb->StartBuilder();
   return 0;
}
