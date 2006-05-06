// $Id: XMLToForm.h 1041 2006-05-04 18:20:33Z sawada $
#ifndef XMLToFormFrame_H
#define XMLToFormFrame_H

class XMLToFormFrame : public TObject
{
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

   ClassDef(XMLToFormFrame, 0) // Frame for XMLToForm
};

#endif   // XMLToFormFrame_H

