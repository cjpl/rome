/********************************************************************
  ROMEString.h, M. Schneebeli PSI

  $Id:$

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
   ROMEString& StripSpaces();

private:
   bool FormatString(ROMEString* string,const char* format,va_list parameters);
   int  SearchFormatType(const char* str);
};

#endif   // ROMEString_H
