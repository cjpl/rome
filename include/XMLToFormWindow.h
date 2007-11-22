// $Id$
#ifndef XMLToFormWindow_H
#define XMLToFormWindow_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TGFrame.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS

class XMLToFormFrame;
class TGListTreeItem;
class ROMEXML;
class ROMEString;

class XMLToFormWindow : public TGTransientFrame {
public:
   enum WidgetTypes {
      EDIT_BOX,
      COMBO_BOX,
      CHECK_BUTTON,
      RADIO_BUTTON,
      BUTTON
   };
protected:
   const TGWindow *fMainWindow;
   Int_t          *fExitID;
   Bool_t          fFirstEdit;
   Int_t           fMaximalWindowWidth;
   Int_t           fListTreeWidth;
   Int_t           fListTreeHeight;
   XMLToFormFrame *fMainFrame;
   XMLToFormFrame *fTreeListActiveFrame;
   ROMEXML        *fXML;

private:
   XMLToFormWindow(const XMLToFormWindow &window); // not implemented
   XMLToFormWindow &operator=(const XMLToFormWindow &rhs); // not implemented

public:
   XMLToFormWindow();
   XMLToFormWindow(const TGWindow * p,const TGWindow * main, XMLToFormFrame *frame, Int_t *exitButtonID, Int_t windowWidth=200,ROMEXML *xml=NULL);
   virtual ~ XMLToFormWindow();

   void   SignalHandler();
   Bool_t SearchWidget(const char* path,XMLToFormFrame** frame,Int_t *index,XMLToFormFrame* mainFrame);
   Bool_t SearchWidget(Int_t id,XMLToFormFrame** frame,Int_t *index);
   Bool_t ListTreeClicked(TGListTreeItem* item,Int_t btn);
   void   DeleteFrame(XMLToFormFrame *frame);
   void   SaveFrame(XMLToFormFrame *frame,ROMEXML *xml);

protected:
   XMLToFormFrame* SearchFrame(XMLToFormFrame *frame,const char* title,const char* editBoxLabel);

   void   CheckSignals(XMLToFormFrame *frame);
   void   PlaceWindow(const TGWindow * main);
   void   BuildForm(XMLToFormFrame *frame);
   void   CreateFrame(XMLToFormFrame *frame);
   void   BuildFrame(XMLToFormFrame *frame);
   void   BuildSubFrames(XMLToFormFrame *frame);
   void   SaveCurrentValues(XMLToFormFrame *frame);
   void   AddFrame(XMLToFormFrame *frame);
   void   AddSubFrames(XMLToFormFrame *frame);
   void   AddFrame(TGFrame* f, TGLayoutHints* l = 0) { TGTransientFrame::AddFrame(f,l); };
   void   HideFrame(XMLToFormFrame *frame);
   void   HideFrame(TGFrame *frame) { TGCompositeFrame::HideFrame(frame); }
   void   GetFirstPathItem(const char* path,ROMEString& firstItem,ROMEString& rest) const;
   Bool_t ReloadValues(const char* xmlFileName);
   Bool_t ReloadValues(XMLToFormFrame *frame);
   void   UpdateFileSelector(const char *xmlFileName);

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(XMLToFormWindow, 0) // Window for XMLToForm
};

#endif   // XMLToFormWindow_H

