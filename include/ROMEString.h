/********************************************************************
  ROMEString.h, M. Schneebeli PSI

  $Log$
  Revision 1.3  2004/09/25 01:34:48  schneebeli_m
  implemented FW dependent EventLoop and DataBase classes

********************************************************************/
#ifndef ROMEString_H
#define ROMEString_H

#include<TString.h>

class ROMEString : public TString
{
public:
   ROMEString() : TString() {};
   ROMEString(int s) : TString(s) {};
   ROMEString(char c) : TString(c) {};
   ROMEString(const char* s) : TString(s) {};
   ROMEString(const TString& s) : TString(s) {};
   ROMEString(const TSubString& s) : TString(s) {};
   bool AppendFormatted(char* format,...);
   bool InsertFormatted(int position,char* format,...);
   bool SetFormatted(char* format,...);
   int  NumberOfOccurrence(ROMEString& subString);
   int  NumberOfOccurrence(char* subString);
   void Write();
   void WriteLine();
private:
   bool FormatString(ROMEString* string,char* format,va_list parameters);
   int  SearchFormatType(char* str);
};

#endif   // ROMEString_H
