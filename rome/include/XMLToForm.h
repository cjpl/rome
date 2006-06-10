// $Id$
#ifndef XMLToForm_H
#define XMLToForm_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TObject.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMEString.h"

class TGWindow;
class XMLToFormWindow;
class ROMEXML;
class XMLToFormFrame;
class ROMEStrArray;

class XMLToForm : public TObject
{
   friend class XMLToFormWindow;

protected:
   XMLToFormWindow  *fWindow;
   Int_t             fMaximalWindowWidth;
   ROMEXML          *fXML;
   ROMEString        fXMLFileName;
   XMLToFormFrame   *fMainFrame;
   ROMEStrArray     *fSubstitutes;
   ROMEStrArray     *fPlaceHolders;

public:
   XMLToForm() { };
   XMLToForm(const char* xmlFileName,ROMEStrArray* substitutes=NULL);
   XMLToForm(const TGWindow* p,const TGWindow* main,const char* xmlFileName,Int_t *exitButtonID,ROMEStrArray* substitutes=NULL);
   virtual ~XMLToForm();

   void        SaveForm();
   const char* GetValue(const char* label);
   Int_t       GetIntValue(const char* label);
   double      GetDoubleValue(const char* label);
   Int_t       GetSelectedIndex(const char* label);
   Bool_t      IsChecked(const char* label);

protected:
   Bool_t      Init(const char* xmlFileName,ROMEStrArray* substitutes=NULL);
   void        InitSubstitutes(ROMEStrArray* substitutes);
   Bool_t      Substitute(ROMEString& placeHolder,ROMEString& substitute);
   Bool_t      XMLToRootClass();
   void        XMLToClass(XMLToFormFrame *frame);
   void        FillClass(XMLToFormFrame *frame);
   void        InitSubFrames(XMLToFormFrame *frame);
   Bool_t      ReadTitle(ROMEString &path,ROMEString &value);
   Int_t       GetIndex(const char* label);
   void        SaveFrame(XMLToFormFrame *frame);
   void        DeleteFrame(XMLToFormFrame *frame);
   void        PrintFrame(XMLToFormFrame *frame,Int_t tab=0);

   ClassDef(XMLToForm, 0) // Create GUI form from XML file
};

#endif   // XMLToForm_H

