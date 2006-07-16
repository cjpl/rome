/********************************************************************
  ROMEConfigParameter.cpp, M.Schneebeli

  $Id$

********************************************************************/
#include <TObject.h>
#include <TObjArray.h>
#include "ROMEString.h"
#include "ROMEBuilder.h"
#include "ROMEXML.h"
#include "ROMEConfigParameter.h"

ROMEConfigParameter::ROMEConfigParameter(ROMEString name, ROMEString arraySize, ROMEString widgetType)
{
   SetName(name);
   fArraySize = arraySize;
   fWidgetType = widgetType;
   fReadModifiedTrueLines = new ROMEStrArray(1);
   fSetLines = new ROMEStrArray(1);
   fWriteLines = new ROMEStrArray(1);
   fAdditionalWriteLines = new ROMEStrArray(1);
   fComboBoxEntries = new ROMEStrArray(1);
   fWriteLinesAlways = true;
}

void ROMEConfigParameter::ReadComment(Int_t level, const char* parentName, const char* path)
{
   // Read comment from romeConfig.xsd
   // if(path==NULL) read use default path.
   // if(path!=NULL) use given path
   fCommentLevel = level;

   ROMEString pathString;
   if (!path)
      pathString.SetFormatted("/xs:schema/xs:complexType[@name='%sDesc']/xs:sequence/xs:element[@name=%s]/xs:annotation/xs:documentation", parentName, fName.Data());
   else
      pathString = path;
   configXSD->GetPathValue(pathString, fComment, "");
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

ROMEConfigParameterGroup::ROMEConfigParameterGroup(ROMEString groupName,ROMEString arraySize,ROMEString groupIdentifier,ROMEString nameIdentifier,ROMEString arrayIdentifier,ROMEString tagName,int multiplicity,Bool_t emptyline)
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
   fWriteEmptyLine = emptyline;
}

void ROMEConfigParameterGroup::ReadComment(Int_t level, const char* tag, const char* path)
{
   // Read comment from romeConfig.xsd
   // if (path==NULL) read use default path.
   // if (path!=NULL) use given path
   fCommentLevel = level;

   ROMEString pathString;
   if (!path) {
      if (tag)
         pathString.SetFormatted("/xs:schema/xs:complexType[@name='%sDesc']/xs:annotation/xs:documentation", tag);
      else
         pathString.SetFormatted("/xs:schema/xs:complexType[@name='%sDesc']/xs:annotation/xs:documentation", fGroupName.Data());
   }
   else {
      pathString = path;
   }
   configXSD->GetPathValue(pathString, fComment, "");
}
