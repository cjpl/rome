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

//______________________________________________________________________________
ROMEConfigParameter::ROMEConfigParameter(ROMEString name, ROMEString arraySize, ROMEString widgetType)
:TNamed(name.Data(), "")
,fArraySize(arraySize)
,fWidgetType(widgetType)
,fComment("")
,fCommentLevel(0)
,fReadModifiedTrueLines(new ROMEStrArray(1))
,fSetLines(new ROMEStrArray(1))
,fWriteLines(new ROMEStrArray(1))
,fAdditionalWriteLines(new ROMEStrArray(1))
,fComboBoxEntries(new ROMEStrArray(1))
,fWriteLinesAlways(kTRUE)
{
}

//______________________________________________________________________________
ROMEConfigParameter::~ROMEConfigParameter()
{
   SafeDelete(fComboBoxEntries);
   SafeDelete(fAdditionalWriteLines);
   SafeDelete(fWriteLines);
   SafeDelete(fSetLines);
   SafeDelete(fReadModifiedTrueLines);
}

//______________________________________________________________________________
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

//______________________________________________________________________________
void ROMEConfigParameter::SetDeclaration(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fDeclaration = ROMEString::Format(va_(fmt), ap);
   va_end(ap);
}

//______________________________________________________________________________
void ROMEConfigParameter::AddSetLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fSetLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

//______________________________________________________________________________
void ROMEConfigParameter::AddWriteLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fWriteLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

//______________________________________________________________________________
void ROMEConfigParameter::AddAdditionalWriteLine(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fAdditionalWriteLines->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

//______________________________________________________________________________
void ROMEConfigParameter::AddComboBoxEntry(const char* va_(fmt),...) {
   if (va_(fmt)==NULL)
      return;
   va_list ap;
   va_start(ap,va_(fmt));
   fComboBoxEntries->AddLast(ROMEString::Format(va_(fmt), ap));
   va_end(ap);
}

//______________________________________________________________________________
ROMEConfigParameterGroup::ROMEConfigParameterGroup(ROMEString groupName, ROMEString arraySize,
                                                   ROMEString groupIdentifier, ROMEString nameIdentifier,
                                                   ROMEString arrayIdentifier, ROMEString tagName,
                                                   Int_t multiplicity, Bool_t emptyline,
                                                   ROMEString info)
:TObject()
,fGroupName(groupName)
,fArraySize(arraySize)
,fGroupIdentifier(groupIdentifier)
,fNameIdentifier("")
,fArrayIdentifier("")
,fTagName("")
,fComment("")
,fInfo(info)
,fCommentLevel(0)
,fMultiplicity(multiplicity)
,fParameters(new TObjArray(10))
,fSubGroups(new TObjArray(10))
,fReadGroupArrayInitLines(new ROMEStrArray(1))
,fWriteStartLines(new ROMEStrArray(1))
,fWriteEndLines(new ROMEStrArray(1))
,fHierarchyLevel(0)
,fWriteAlways(kFALSE)
,fWriteEmptyLine(emptyline)
{
   if (nameIdentifier.Length()==0) {
      fNameIdentifier = fGroupIdentifier+"Name";
   } else {
      fNameIdentifier = nameIdentifier;
   }

   if (arrayIdentifier.Length()==0) {
      fArrayIdentifier = arrayIdentifier;
   } else {
      fArrayIdentifier = arrayIdentifier+"=";
   }

   if (tagName.Length()==0) {
      fTagName = fGroupName;
   } else {
      fTagName = tagName;
   }
}

//______________________________________________________________________________
ROMEConfigParameterGroup::~ROMEConfigParameterGroup()
{
   SafeDelete(fWriteEndLines);
   SafeDelete(fWriteStartLines);
   SafeDelete(fReadGroupArrayInitLines);
   SafeDelete(fSubGroups);
   SafeDelete(fParameters);
}

//______________________________________________________________________________
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
