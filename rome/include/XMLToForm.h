// $Id$
#ifndef XMLToForm_H
#define XMLToForm_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGButton.h>
#include <TGTab.h>
#include <TGLayout.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGFrame.h>
#include <TGComboBox.h>
#include <Riostream.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

#include <ROMEXML.h>
#include "XMLToFormFrame.h"

class XMLToForm : public TObject
{
   friend class XMLToFormWindow;
protected:
   XMLToFormWindow  *fWindow;
   int fMaximalWindowWidth;
   ROMEXML *fXML;
   ROMEString fXMLFileName;
   XMLToFormFrame *fMainFrame;
   ROMEStrArray *fSubstitutes;
   ROMEStrArray *fPlaceHolders;
public:
   XMLToForm() { };
   XMLToForm(const char* xmlFileName,ROMEStrArray* substitutes=NULL);
   XMLToForm(const TGWindow * p,const TGWindow * main,const char* xmlFileName,int *exitButtonID,ROMEStrArray* substitutes=NULL);
   virtual ~ XMLToForm();

   void SaveForm();
   const char* GetValue(const char* label);
   int         GetIntValue(const char* label);
   double      GetDoubleValue(const char* label);
   int         GetSelectedIndex(const char* label);
   bool        IsChecked(const char* label);
protected:
   bool Init(const char* xmlFileName,ROMEStrArray* substitutes=NULL);
   void InitSubstitutes(ROMEStrArray* substitutes);
   bool Substitute(ROMEString& placeHolder,ROMEString& substitute);
   bool XMLToRootClass();
   void XMLToClass(XMLToFormFrame *frame);
   void FillClass(XMLToFormFrame *frame);
   void InitSubFrames(XMLToFormFrame *frame);
   bool ReadTitle(ROMEString &path,ROMEString &value);
   int  GetIndex(const char* label);
   void SaveFrame(XMLToFormFrame *frame);
   void DeleteFrame(XMLToFormFrame *frame);
   void PrintFrame(XMLToFormFrame *frame,int tab=0);

   ClassDef(XMLToForm, 0) // Create GUI form from XML file
};

#endif   // XMLToForm_H

