// $Id: XMLToForm.h 3402 2006-04-30 13:10:16Z Matthias Schneebeli $
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

const int framePad = 3;
const int innerFramePad = 0;
const int elementPad = 3;

class XMLToFormFrame {
friend class XMLToFormWindow;
public:
   // Frame
   TGLayoutHints *fLFrame;
   TGLayoutHints *fLInnerFrame;
   TGLayoutHints *fLInnerCheckButtonFrame;
   XMLToFormFrame *fParentFrame;

   TGCompositeFrame *fFrame;
   bool fVerticalFrame;
   ROMEString *fFramePath;
   int fIndex;
   bool fIsTab;
   int fTabIndex;
   bool fVisible;
   // Tab
   TGTab *fTab;
   // Title
   TGLabel *fTitleLabel;
   TGHotString *fTitleLabelHotString;
   TGLayoutHints *fLTitleLabel;
   // SubFrames
   XMLToFormFrame **fSubFrames;
   int fNumberOfSubFrames;
   // VFrames
   TGHorizontalFrame *fHFrame;
   TGHorizontalFrame **fHHFrames;
   TGVerticalFrame **fVFrames;
   int fNumberOfHHFrames;
   int fNumberOfVFrames;
   int fNumberOfElements;
   // EditBox
   TGVerticalFrame **fEditVFrames;
   TGLabel **fEditLabel;
   TGHotString **fEditLabelHotString;
   TGLayoutHints *fLEditLabel;
   TGTextEntry **fEditBox;
   TGTextBuffer **fEditBoxBuffer;
   TGLayoutHints *fLEditBox;
   ROMEString **fEditBoxPaths;
   int *fEditBoxWidth;
   int *fEditBoxElementIndex;
   int fNumberOfEditBoxes;
   // Button
   TGTextButton **fButton;
   int *fButtonID;
   TGHotString **fButtonHotString;
   TGLayoutHints *fLButton;
   ROMEString **fButtonPaths;
   int *fButtonWidth;
   int *fButtonElementIndex;
   int fNumberOfButtons;
   // ComboBox
   TGVerticalFrame **fComboVFrames;
   TGLabel **fComboLabel;
   TGHotString **fComboLabelHotString;
   TGLayoutHints *fLComboLabel;
   TGComboBox **fComboBox;
   TGLayoutHints *fLComboBox;
   ROMEStrArray **fComboBoxEntries;
   int *fComboBoxSelectedEntry;
   ROMEString **fComboBoxPaths;
   int *fComboBoxWidth;
   int *fComboBoxElementIndex;
   int fNumberOfComboBoxes;
   // CheckButton
   TGVerticalFrame **fCheckButtonVFrames;
   TGLabel **fCheckButtonLabel;
   TGHotString **fCheckButtonLabelHotString;
   TGLayoutHints *fLCheckButtonLabel;
   TGCheckButton **fCheckButton;
   bool *fCheckButtonChecked;
   TGLayoutHints *fLCheckButton;
   ROMEString **fCheckButtonPaths;
   int *fCheckButtonWidth;
   int *fCheckButtonElementIndex;
   int fNumberOfCheckButtons;

public:
   XMLToFormFrame() 
   {
      fIsTab = false;
      fVisible = true;
      fLFrame = NULL;
      fLInnerFrame = NULL;
      fLInnerCheckButtonFrame = NULL;
      fParentFrame = NULL;
      fFrame = NULL;
      fFramePath = NULL;
      fTab = NULL;
      fTitleLabel = NULL;
      fTitleLabelHotString = NULL;
      fLTitleLabel = NULL;
      fNumberOfSubFrames = 0;
      fHFrame = NULL;
      fHHFrames = NULL;
      fVFrames = NULL;
      fSubFrames = NULL;
      fNumberOfHHFrames = 0;
      fNumberOfVFrames = 0;
      fNumberOfElements = 0;
      fNumberOfEditBoxes = 0;
      fNumberOfButtons = 0;
      fNumberOfComboBoxes = 0;
      fNumberOfCheckButtons = 0;
   };
};


class XMLToForm {
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
};

#endif   // XMLToForm_H

