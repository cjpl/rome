/********************************************************************
  ROMEString.h, M. Schneebeli PSI

  $Log$
  Revision 1.4  2005/04/01 14:56:23  schneebeli_m
  Histo moved, multiple databases, db-paths moved, InputDataFormat->DAQSystem, GetMidas() to access banks, User DAQ

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
   ROMEString& AppendFormatted(const char* format,...);
   ROMEString& InsertFormatted(int position,const char* format,...);
   ROMEString& SetFormatted(const char* format,...);
   int  NumberOfOccurrence(ROMEString& subString);
   int  NumberOfOccurrence(const char* subString);
   void Write();
   void WriteLine();
private:
   bool FormatString(ROMEString* string,const char* format,va_list parameters);
   int  SearchFormatType(const char* str);
};

#endif   // ROMEString_H
