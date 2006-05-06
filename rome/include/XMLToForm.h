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

const int framePad = 3;
const int innerFramePad = 0;
const int elementPad = 3;

class XMLToForm : public TObject
{
protected:
   enum WidgetTypes {
      EDIT_BOX,
      COMBO_BOX,
      CHECK_BUTTON,
      BUTTON
   };

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
   const char* GetEditBoxValue(const char* editBoxLabel);
   int         GetEditBoxIntValue(const char* editBoxLabel);
   double      GetEditBoxDoubleValue(const char* editBoxLabel);
   const char* GetComboBoxValue(const char* comboBoxLabel);
   int         GetComboBoxIntValue(const char* comboBoxLabel);
   double      GetComboBoxDoubleValue(const char* comboBoxLabel);
   int         GetComboBoxSelectedIndex(const char* comboBoxLabel);
   const char* GetCheckButtonValue(const char* checkButtonLabel);
   bool        IsCheckButtonChecked(const char* checkButtonLabel);
protected:
   bool Init(const char* xmlFileName,ROMEStrArray* substitutes=NULL);
   void InitSubstitutes(ROMEStrArray* substitutes);
   bool Substitute(ROMEString& placeHolder,ROMEString& substitute);
   bool XMLToRootClass();
   void XMLToClass(XMLToFormFrame *frame);
   void FillClass(XMLToFormFrame *frame);
   void InitSubFrames(XMLToFormFrame *frame);
   bool ReadTitle(ROMEString &path,ROMEString &value);
   bool SearchWidget(const char* path,int type,XMLToFormFrame** frame,int *index);
   const char* GetValue(const char* label,int type);
   int  GetIndex(const char* label,int type);
   void SaveFrame(XMLToFormFrame *frame);
   void DeleteFrame(XMLToFormFrame *frame);
   XMLToFormFrame* SearchFrame(XMLToFormFrame *frame,const char* title,const char* editBoxLabel);
   void PrintFrame(XMLToFormFrame *frame,int tab=0);

   ClassDef(XMLToForm, 0) // Create GUI form from XML file
};

#endif   // XMLToForm_H

