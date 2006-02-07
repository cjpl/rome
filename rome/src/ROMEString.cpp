// Author: Matthias Schneebeli
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// ROMEString                                                           //
//                                                                      //
//  Handles character strings.
//  Derived from TString.
//                                                                      //
//  $Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <TString.h>
#include <TMath.h>
#include <TVirtualMutex.h>
#include <Riostream.h>
#include "ROMEString.h"

ROMEString& ROMEString::AppendFormatted(const char* va_(fmt),...)
{
   if (va_(fmt)==NULL)
      return *this;
   va_list ap;
   va_start(ap,va_(fmt));
   this->Append(Format(va_(fmt), ap));
   va_end(ap);
   return *this;
}

ROMEString& ROMEString::InsertFormatted(Ssiz_t position,const char* va_(fmt),...)
{
   if (va_(fmt)==NULL)
      return *this;
   va_list ap;
   va_start(ap,va_(fmt));
   this->Insert(position,Format(va_(fmt), ap));
   va_end(ap);
   return *this;
}

ROMEString& ROMEString::SetFormatted(const char* va_(fmt),...)
{
   if (va_(fmt)==NULL)
      return *this;
   va_list ap;
   va_start(ap,va_(fmt));
   *this = Format(va_(fmt), ap);
   va_end(ap);
   return *this;
}

void ROMEString::Write() {
   cout << this->Data();
}


void ROMEString::WriteLine() {
   cout << this->Data() << endl;
}

Int_t ROMEString::NumberOfOccurrence(ROMEString& subString)
{
   return this->NumberOfOccurrence(subString.Data());
}

Int_t ROMEString::NumberOfOccurrence(const char* subString)
{
   Int_t numberOfOccurrence = 0;
   const char* str = this->Data();
   str = strstr(str,subString);
   while (str!=NULL) {
      str++;
      numberOfOccurrence++;
      str = strstr(str,subString);
   }
   return numberOfOccurrence;
}

Int_t ROMEString::SearchFormatType(const char* str)
{
   const char numberOfTypes = 17;
   int i,j;
   char c[numberOfTypes] = {'c','C','d','i','o','u','x','X','e','E','f','g','G','n','p','s','S'};
   for (i=0;i<(int)strlen(str);i++) {
      for (j=0;j<numberOfTypes;j++) {
         if (str[i]==c[j]) return i;;
      }
   }
   return -1;
}

istream& ROMEString::ReadFile(istream& str) {
   this->Resize(0);
   char *buffer = "";
   int bufferSize = 2000;
   int bufferLength = bufferSize-1;
   while (bufferLength==bufferSize-1) {
      bufferSize*=2;
      buffer = new char[bufferSize];
      str.get(buffer,bufferSize,0);
      bufferLength = strlen(buffer);
      this->Append(buffer);
      delete buffer;
   }
   return str;
}

istream& ROMEString::ReadLine(istream& str) {
   this->Resize(0);
   char *buffer = "";
   int bufferSize = 2000;
   int bufferLength = bufferSize-1;
   while (bufferLength==bufferSize-1) {
      bufferSize*=2;
      buffer = new char[bufferSize];
      str.getline(buffer,bufferSize);
      bufferLength = strlen(buffer);
      this->Append(buffer);
      delete buffer;
   }
   return str;
}

Int_t ROMEString::ToInteger()
{
   char *cstop;
   return strtol(this->Data(),&cstop,10);
}

Double_t ROMEString::ToDouble()
{
   char *cstop;
   return strtod(this->Data(),&cstop);
}

void ROMEString::ToLower() {
   ((TString*)this)->ToLower();
}

const char* ROMEString::ToLower(ROMEString& destination)
{
   destination = this->Data();
   destination.ToLower();
   return destination.Data();
}

void ROMEString::ToUpper() {
   ((TString*)this)->ToUpper();
}

const char* ROMEString::ToUpper(ROMEString& destination)
{
   destination = this->Data();
   destination.ToUpper();
   return destination.Data();
}

// Strip space,tab and new line at both sides
ROMEString& ROMEString::StripSpaces(){
   Ssiz_t start = 0;             // Index of first character
   Ssiz_t end = Length();        // One beyond last character
   const char *direct = Data();  // Avoid a dereference w dumb compiler

   while (start < end && direct[start] <= ' ')
      ++start;

   while (start < end && direct[end] <= ' ')
      --end;

   this->Remove(end+1,Length()-end-1);
   this->Remove(0,start);
   return *this;
}

// formatting functions copied from TString.cxx
#if defined( R__VISUAL_CPLUSPLUS )
#define vsnprintf _vsnprintf
#endif
static const int cb_size  = 4096;
static const int fld_size = 2048;
static char gFormbuf[cb_size];       // some slob for form overflow
static char *gBfree  = gFormbuf;
static char *gEndbuf = &gFormbuf[cb_size-1];
char *ROMEString::SlowFormat(const char *format, va_list ap, int hint)
{
   // Format a string in a formatting buffer (using a printf style
   // format descriptor).

   static char *slowBuffer  = 0;
   static int   slowBufferSize = 0;

   R__LOCKGUARD2(gStringMutex);

   if (hint == -1) hint = fld_size;
   if (hint > slowBufferSize) {
      delete [] slowBuffer;
      slowBufferSize = 2 * hint;
      if (hint < 0 || slowBufferSize < 0) {
         slowBufferSize = 0;
         slowBuffer = 0;
         return 0;
      }
      slowBuffer = new char[slowBufferSize];
   }

   int n = vsnprintf(slowBuffer, slowBufferSize, format, ap);
   // old vsnprintf's return -1 if string is truncated new ones return
   // total number of characters that would have been written
   if (n == -1 || n >= slowBufferSize) {
      if (n == -1) n = 2 * slowBufferSize;
      if (n == slowBufferSize) n++;
      if (n <= 0) return 0; // int overflow!
      return SlowFormat(format, ap, n);
   }

   return slowBuffer;
}

char* ROMEString::Format(const char *format, va_list ap)
{
   // Format a string in a circular formatting buffer (using a printf style
   // format descriptor).

   R__LOCKGUARD2(gStringMutex);

   char *buf = gBfree;

   if (buf+fld_size > gEndbuf)
      buf = gFormbuf;

   int n = vsnprintf(buf, fld_size, format, ap);
   // old vsnprintf's return -1 if string is truncated new ones return
   // total number of characters that would have been written
   if (n == -1 || n >= fld_size) {
      return SlowFormat(format, ap, n);
   }

   gBfree = buf+n+1;
   return buf;
}
// end of formatting functions copied from TString.cxx
