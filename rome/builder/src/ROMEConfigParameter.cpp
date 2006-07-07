/********************************************************************
  ROMEConfigParameter.cpp, M.Schneebeli

  $Id: ROMEConfigParameter.cpp 1134 2006-06-08 20:42:22Z sawada $

********************************************************************/
#include <TObject.h>
#include <TObjArray.h>
#include "ROMEString.h"
#include "ROMEBuilder.h"
#include "ROMEConfigParameter.h"

ROMEConfigParameter::ROMEConfigParameter(ROMEString name, ROMEString arraySize, ROMEString widgetType)
{
   fName = name;
   fArraySize = arraySize;
   fWidgetType = widgetType;
   fReadModifiedTrueLines = new ROMEStrArray(1);
   fSetLines = new ROMEStrArray(1);
   fWriteLines = new ROMEStrArray(1);
   fAdditionalWriteLines = new ROMEStrArray(1);
   fComboBoxEntries = new ROMEStrArray(1);
   fWriteLinesAlways = true;
}

void ROMEConfigParameter::AddSetLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fSetLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

void ROMEConfigParameter::AddWriteLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fWriteLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

void ROMEConfigParameter::AddAdditionalWriteLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fAdditionalWriteLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

void ROMEConfigParameter::AddComboBoxEntry(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fComboBoxEntries->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

ROMEConfigParameterGroup::ROMEConfigParameterGroup(ROMEString groupName,ROMEString arraySize,ROMEString groupIdentifier,ROMEString nameIdentifier,ROMEString arrayIdentifier,ROMEString tagName,int multiplicity)
{
   fGroupName = groupName;
   fArraySize = arraySize;
   fGroupIdentifier = groupIdentifier;
   if (nameIdentifier.Length()==0)
      fNameIdentifier = fGroupIdentifier+"Name";
   else
      fNameIdentifier = nameIdentifier;
   if (arrayIdentifier.Length()==0)
      fArrayIdentifier = arrayIdentifier;
   else
      fArrayIdentifier = arrayIdentifier+"=";
   if (tagName.Length()==0)
      fTagName = fGroupName;
   else
      fTagName = tagName;
   fMultiplicity = multiplicity;
   fParameters = new TObjArray(10);
   fSubGroups = new TObjArray(10);
   fReadGroupArrayInitLines = new ROMEStrArray(1);
   fWriteStartLines = new ROMEStrArray(1);
   fWriteEndLines = new ROMEStrArray(1);
   fHierarchyLevel = 0;
   fWriteAlways = false;
}
