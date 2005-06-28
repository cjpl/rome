/********************************************************************
  ROMEString.h, M. Schneebeli PSI

  $Log$
  Revision 1.8  2005/06/28 07:12:31  sawada
  replaced atdarg.h with Varargs.h. It is more safe under some platform.

  Revision 1.7  2005/06/27 07:26:21  schneebeli_m
  added stdarg include

  Revision 1.6  2005/05/18 09:49:32  schneebeli_m
  removed run & event number error, implemented FileRead in ROMEString

  Revision 1.5  2005/04/20 15:12:40  schneebeli_m
  histo and path in definition xml

  Revision 1.4  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

  Revision 1.3  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMEString_H
#define ROMEString_H

#include <TString.h>
#include <Varargs.h>

class ROMEString : public TString
{
public:
   ROMEString() : TString() {};
   ROMEString(int s) : TString(s) {};
   ROMEString(char c) : TString(c) {};
   ROMEString(const char* s) : TString(s) {};
   ROMEString(const TString& s) : TString(s) {};
   ROMEString(const TSubString& s) : TString(s) {};
   ROMEString& AppendFormatted(const char* format=NULL,...);
   ROMEString& InsertFormatted(int position,const char* format=NULL,...);
   ROMEString& SetFormatted(const char* format=NULL,...);
   int  NumberOfOccurrence(ROMEString& subString);
   int  NumberOfOccurrence(const char* subString);
   void Write();
   void WriteLine();
   istream& ReadFile(istream& str);
private:
   bool FormatString(ROMEString* string,const char* format,va_list parameters);
   int  SearchFormatType(const char* str);
};

#endif   // ROMEString_H
