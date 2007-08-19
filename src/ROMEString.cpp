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
#include <map>
#include <TMath.h>
#include <TVirtualMutex.h>
#include "ROMEiostream.h"
#include "ROMEString.h"

ClassImp(ROMEString)

static TVirtualMutex *fgROMEStringMutex = 0;

//______________________________________________________________________________
ROMEString& ROMEString::AppendFormatted(const char* va_(fmt), ...)
{
   if (!va_(fmt)) {
      return *this;
   }
   va_list ap;
   va_start(ap, va_(fmt));
   Append(Format(va_(fmt), ap));
   va_end(ap);
   return *this;
}

//______________________________________________________________________________
ROMEString& ROMEString::InsertFormatted(Ssiz_t position, const char* va_(fmt), ...)
{
   if (!va_(fmt)) {
      return *this;
   }
   va_list ap;
   va_start(ap, va_(fmt));
   Insert(position, Format(va_(fmt), ap));
   va_end(ap);
   return *this;
}

//______________________________________________________________________________
ROMEString& ROMEString::SetFormatted(const char* va_(fmt), ...)
{
   if (!va_(fmt)) {
      return *this;
   }
   va_list ap;
   va_start(ap, va_(fmt));
   *this = Format(va_(fmt), ap);
   va_end(ap);
   return *this;
}

//______________________________________________________________________________
void ROMEString::Write() const
{
   cout << Data();
}

//______________________________________________________________________________
void ROMEString::WriteLine() const
{
   cout << Data() << endl;
}

//______________________________________________________________________________
Int_t ROMEString::NumberOfOccurrence(ROMEString& subString) const
{
   return NumberOfOccurrence(subString.Data());
}

//______________________________________________________________________________
Int_t ROMEString::NumberOfOccurrence(const char* subString) const
{
   Int_t numberOfOccurrence = 0;
   const char* str = Data();
   str = strstr(str, subString);
   while (str) {
      str++;
      numberOfOccurrence++;
      str = strstr(str, subString);
   }
   return numberOfOccurrence;
}

static map<string, string> fgReadFileCache;
//______________________________________________________________________________
Int_t ROMEString::ReadFile(const char *filename, Bool_t useCache)
{
   if (useCache && fgReadFileCache[filename].length()) {
      *this = fgReadFileCache[filename].data();
      return Length();
   }

   fstream *fileStream = new fstream(filename, ios::in);
   if (fileStream && fileStream->good()) {
      ReadFile(*fileStream);
      fgReadFileCache[filename] = Data();
      delete fileStream;
      return Length();
   } else {
      delete fileStream;
      Resize(0);
      return -1;
   }
}

//______________________________________________________________________________
istream& ROMEString::ReadFile(istream& str)
{
   Resize(0);
   char *buffer;
   int bufferSize = 2000;
   int bufferLength = bufferSize - 1;
   while (bufferLength == bufferSize - 1) {
      bufferSize *= 2;
      buffer = new char[bufferSize];
      str.get(buffer, bufferSize, 0);
      bufferLength = strlen(buffer);
      Append(buffer);
      delete [] buffer;
   }
   return str;
}

//______________________________________________________________________________
istream& ROMEString::ReadLine(istream& str)
{
   Resize(0);
   char *buffer;
   int bufferSize = 2000;
   int bufferLength = bufferSize - 1;
   while (bufferLength == bufferSize - 1) {
      bufferSize*=2;
      buffer = new char[bufferSize];
      str.getline(buffer, bufferSize);
      bufferLength = strlen(buffer);
      Append(buffer);
      delete [] buffer;
   }
   return str;
}

//______________________________________________________________________________
Int_t ROMEString::ToInteger() const
{
   char *cstop;
   return strtol(Data(), &cstop, 10);
}

//______________________________________________________________________________
Long_t ROMEString::ToLong() const
{
   char *cstop;
   return strtol(Data(), &cstop, 10);
}

//______________________________________________________________________________
Long64_t ROMEString::ToLong64() const
{
   char *cstop;
#if defined( R__UNIX )
   return strtoll(Data(), &cstop, 10);
#else
   return _strtoi64(Data(), &cstop, 10);
#endif
}

//______________________________________________________________________________
Double_t ROMEString::ToDouble() const
{
   char *cstop;
   return strtod(Data(), &cstop);
}

//______________________________________________________________________________
Float_t ROMEString::ToFloat() const
{
   char *cstop;
   return static_cast<Float_t>(strtod(Data(), &cstop));
}

//______________________________________________________________________________
Bool_t ROMEString::ToBool() const
{
   if (CompareTo("true", TString::kIgnoreCase)) {
      return true;
   }
   if (CompareTo("false", TString::kIgnoreCase)) {
      return false;
   }
   return ToInteger() != 0;
}

//______________________________________________________________________________
void ROMEString::ToLower()
{
   static_cast<TString*>(this)->ToLower();
}

//______________________________________________________________________________
const char* ROMEString::ToLower(ROMEString& destination) const
{
   destination = Data();
   destination.ToLower();
   return destination.Data();
}

//______________________________________________________________________________
void ROMEString::ToUpper()
{
   static_cast<TString*>(this)->ToUpper();
}

//______________________________________________________________________________
const char* ROMEString::ToUpper(ROMEString& destination) const
{
   destination = Data();
   destination.ToUpper();
   return destination.Data();
}

//______________________________________________________________________________
ROMEString& ROMEString::StripSpaces()
{
// Strip space, tab and new line at both sides
   Ssiz_t start = 0;             // Index of first character
   Ssiz_t end = Length();        // One beyond last character
   const char *direct = Data();  // Avoid a dereference w dumb compiler

   while (start < end && direct[start] <= ' ') {
      ++start;
   }

   while (start < end && direct[end] <= ' ') {
      --end;
   }

   Remove(end + 1, Length() - end - 1);
   Remove(0, start);
   return *this;
}

// formatting functions copied from TString.cxx
#if defined( R__VISUAL_CPLUSPLUS )
#define vsnprintf _vsnprintf
#endif
const int cb_size  = 4096;
const int fld_size = 2048;

namespace {
   char gFormbuf[cb_size];       // some slob for form overflow
   char *gBfree  = gFormbuf;
   char *gEndbuf = &gFormbuf[cb_size - 1];
}

//______________________________________________________________________________
char* ROMEString::SlowFormat(const char *format, va_list ap, int hint)
{
   // Format a string in a formatting buffer (using a printf style
   // format descriptor).

   static char *slowBuffer  = 0;
   static int   slowBufferSize = 0;

   ROME_LOCKGUARD(fgROMEStringMutex);

   if (hint == -1) {
      hint = fld_size;
   }
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
      if (n == -1) {
         n = 2 * slowBufferSize;
      }
      if (n == slowBufferSize) {
         n++;
      }
      if (n <= 0) {
         return 0; // int overflow!
      }
      return SlowFormat(format, ap, n);
   }

   return slowBuffer;
}

//______________________________________________________________________________
char* ROMEString::Format(const char *format, va_list ap)
{
   // Format a string in a circular formatting buffer (using a printf style
   // format descriptor).

   ROME_LOCKGUARD(fgROMEStringMutex);

   char *buf = gBfree;

   if (buf + fld_size > gEndbuf) {
      buf = gFormbuf;
   }

   int n = vsnprintf(buf, fld_size, format, ap);
   // old vsnprintf's return -1 if string is truncated new ones return
   // total number of characters that would have been written
   if (n == -1 || n >= fld_size) {
      return SlowFormat(format, ap, n);
   }

   gBfree = buf + n + 1;
   return buf;
}
// end of formatting functions copied from TString.cxx
