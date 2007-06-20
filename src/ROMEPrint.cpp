// Author: Ryu Sawada
//////////////////////////////////////////////////////////////////////////
//
// ROMEPrint
//
//  Message system in ROME
//
//  $Id$
//
//////////////////////////////////////////////////////////////////////////

#include <map>
#include <TArrayI.h>
#if defined( HAVE_MIDAS )
#   include "midas.h"
#endif
#include "Riostream.h"
#include "ROMEPrint.h"
#include "ROMEString.h"
#include "ROMEStrArray.h"

Int_t ROMEPrint::gVerboseLevel = kNormal;
Int_t ROMEPrint::gReportMaxCount = 20;

namespace {

map<string, Int_t> fgReportMap;              // index of reports
TArrayI            fgReportLine;             // line number of reports
TArrayI            fgReportPrintCount;       // print count of reports
TArrayI            fgReportLevel;            // verbose level of reports
ROMEStrArray       fgReportFile;             // function name of reports
ROMEStrArray       fgReportFunction;         // function name of reports
ROMEStrArray       fgReportMessage;          // function name of reports

//______________________________________________________________________________
void PrintSummary(Int_t level, Int_t reportIndex)
{
   ROMEString funcFileLine;
   ROMEString report;
   Int_t messageLength;

   const Int_t kCountLength = 6;

   if (fgReportLevel.At(reportIndex) == level) {
      funcFileLine.SetFormatted("%s:%d:%s", gSystem->BaseName(fgReportFile.At(reportIndex).Data()),
                                fgReportLine.At(reportIndex), fgReportFunction.At(reportIndex).Data());
      report = fgReportMessage.At(reportIndex);
      report.ReplaceAll("\n", " ");
      messageLength = 76 - kCountLength - funcFileLine.Length() - 3;
      if (report.Length() > messageLength + 1) {
         report.Resize(messageLength + 1);
         report[messageLength - 2] = report[messageLength - 1] = report[messageLength] = '.';
      }
      if (!report.EndsWith("\n")) {
         report += '\n';
      }
      cout<<setw(kCountLength)<<fgReportPrintCount.At(reportIndex)<<" "
          <<kReportLevelLetter[fgReportLevel.At(reportIndex)]<<" "
          <<funcFileLine<<" "
          <<report<<flush;
   }
}

} // namespace

//______________________________________________________________________________
void ROMEPrint::PrintAlways(const char* va_(fmt),...)
{
// Print always independently of verbose level
// STDOUT
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap) <<flush;
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Debug(const char* va_(fmt),...)
{
// Print when verbose level >= kDebug
// STDOUT
   if (gVerboseLevel < kDebug)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Print(const char* va_(fmt),...)
{
// Print when verbose level >= kNormal
// STDOUT
   if (gVerboseLevel < kNormal)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Info(const char* va_(fmt),...)
{
// Print when verbose level >= kNormal
// STDOUT and midas message
   if (gVerboseLevel < kNormal)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MINFO, "ROMEPrint::Info", text.Data());
   cout<<text.Data()<<flush;
#else
   cout<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Warning(const char* va_(fmt),...)
{
// Print when verbose level >= kWarning
// STDERR and midas message
   if (gVerboseLevel < kWarning)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MINFO, "ROMEPrint::Warning", text.Data());
   cerr<<text.Data()<<flush;
#else
   cerr<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Error(const char* va_(fmt),...)
{
// Print when verbose level >= kError
// STDERR and midas message
   if (gVerboseLevel < kError)
      return;
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
#if defined( HAVE_MIDAS )
   ROMEString text = ROMEString::Format(va_(fmt), ap);
   cm_msg(MERROR, "ROMEPrint::Error", text.Data());
   cerr<<text.Data()<<flush;
#else
   cerr<<ROMEString::Format(va_(fmt), ap)<<flush;
#endif
   va_end(ap);
}

//______________________________________________________________________________
void ROMEPrint::Report(const Int_t verboseLevel, const char* fileName, const char *funcName, Int_t lineNumber,
                       const Long64_t run, const Long64_t event, const char* va_(fmt),...)
{
   // Messaging system of ROME. This function is called from user code.
   // ROME framework or generated code does not call this.
   //
   // Example.
   //
   // #include "ROMEPrint.h"
   //
   // void function() {
   //    Int_t status = otherFunction();
   //    if (!status) {
   //       Report(R_WARNING, "Status is %d", status);
   //    }
   // }
   //
   // Available verbose levels are R_ERROR, R_WARNING, R_INFO, R_VERBOSE and R_DEBUG.

   if (gVerboseLevel < verboseLevel) {
      return;
   }

   if (va_(fmt) == 0) {
      return;
   }

   ROMEString fileLine;
   ROMEString runEvent;

   fileLine.SetFormatted("%s:%d", gSystem->BaseName(fileName), lineNumber);
#if defined(R__UNIX)
   runEvent.SetFormatted("%lld-%lld", run, event);
#else
   runEvent.SetFormatted("%I64d-%I64d", run, event);
#endif

   Int_t indx = fgReportMap[fileLine.Data()];

   if (!indx) { // this report is not registered
      fgReportMap[fileLine.Data()] = indx = fgReportMap.size();
      fgReportLine.Set(indx + 1);
      fgReportPrintCount.Set(indx + 1);
      fgReportLevel.Set(indx + 1);
      fgReportLine.AddAt(lineNumber, indx);
      fgReportPrintCount.AddAt(0, indx);
      fgReportLevel.AddAt(verboseLevel, indx);
      fgReportFile.AddAt(fileName, indx);
      fgReportFunction.AddAt(funcName, indx);
      fgReportMessage.AddAt(va_(fmt), indx);
   }

   fgReportPrintCount.AddAt(fgReportPrintCount.At(indx) + 1, indx); // increment

   if (gReportMaxCount <= 0 || fgReportPrintCount.At(indx) <= gReportMaxCount) {
      // print
      va_list ap;
      va_start(ap,va_(fmt));
      ROMEString report = ROMEString::Format(va_(fmt), ap);
      if (!report.EndsWith("\n")) {
         report += '\n';
      }
      ROMEString text;
      text.SetFormatted("[%c,%s,%s]", kReportLevelLetter[verboseLevel], runEvent.Data(), fileLine.Data());
      if (fgReportPrintCount.At(indx) == gReportMaxCount) {
         text.Insert(0, "The following message will be suppressed\n");
      }

      if (report.Length() + text.Length() >= 78 ||
          report.Index("\n", 1, 0, TString::kExact) != report.Length() - 1) {
         text += '\n';
      } else {
         text += ' ';
      }
      text += report;
      if (verboseLevel <= kWarning) {
         cerr<<text<<flush;
      } else {
         cout<<text<<flush;
      }
#if defined( HAVE_MIDAS )
      if (verboseLevel <= kWarning) {
         cm_msg(MERROR, "ROMEPrint::Report", text.Data());
      } else {
         cm_msg(MINFO, "ROMEPrint::Report", text.Data());
      }
#endif
      va_end(ap);
   }
}

//______________________________________________________________________________
void ROMEPrint::ReportSummary()
{
   Int_t i;
   Int_t n = fgReportMap.size();

   if (!n) {
      // probably, the user is not using 'Report'.
      return;
   }

   ROMEString fileLine;
   ROMEString report;

   cout<<"*************************** Report Message Summary ***************************"<<endl<<endl;
   // error
   for (i = 1; i <= n; i++) {
      PrintSummary(kError, i);
   }
   // warning
   for (i = 1; i <= n; i++) {
      PrintSummary(kWarning, i);
   }
   if (gVerboseLevel >= kVerbose) {
      // info
      for (i = 1; i <= n; i++) {
         PrintSummary(kNormal, i);
      }
      // verbose
      for (i = 1; i <= n; i++) {
         PrintSummary(kVerbose, i);
      }
      // debug
      for (i = 1; i <= n; i++) {
         PrintSummary(kDebug, i);
      }
   }
   cout<<endl<<"******************************************************************************"<<endl;
}
