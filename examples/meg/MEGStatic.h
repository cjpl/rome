#ifndef MEGStatic_H
#define MEGStatic_H
#include <TArrayI.h>
#include <Riostream.h>
#include <strstream>
#include <stdio.h>

#if defined( _MSC_VER )
#include <conio.h>
#endif
#if defined ( __linux__ )
#include <unistd.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/ioctl.h>
#endif

class MEGStatic
{
public:
   static int dfprintf(char *str, char *filename)
   {
      ofstream ofile;
      ofile.open(filename,ofstream::out | ofstream::app);
      ofile<<str;
      ofile.close();
//      cout<<str;
      return 0;
   }
   static int dfclear(char *filename) 
   {
      ofstream ofile;
      ofile.open(filename,ofstream::out);
      ofile.close();
      return 0;
   }
};

#endif   // MEGStatic_H
