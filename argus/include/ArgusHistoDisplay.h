////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Histogram Display.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// $Id$

#ifndef ArgusHistoDisplay_H
#define ArgusHistoDisplay_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 4244)
#endif // R__VISUAL_CPLUSPLUS
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "ROMEString.h"
#include "ArgusTab.h"

class TGMenuBar;
class TGPopupMenu;
class TGHProgressBar;
class TRootEmbeddedCanvas;
class TText;
class TGraph;
class TH1F;
class TH2F;
class TPad;
class TLine;
class XMLToForm;

class ArgusHistoDisplay : public ArgusTab
{
protected:
   enum {
      kMaxNumberOfPads = 120,
      kMaxNumberOfPadsX = 6,
      kMaxNumberOfPadsY = 20,
      kMaxNumberOfLines = 5
   };
   enum DisplayTypes {
      kNoDisplay,
      kTGraphDisplay,
      kTH1FDisplay,
      kTH2FDisplay
   };
   enum MenuEnumeration {
      M_ROOT = 900,
      M_ARGUS_DISPLAY_VIEW_NEXT,
      M_ARGUS_DISPLAY_VIEW_PREVIOUS,
      M_ARGUS_DISPLAY_VIEW_RESET,
      M_ARGUS_DISPLAY_VIEW_PAD_CONFIG,
      M_ARGUS_DISPLAY_VIEW_SELECT
   };
   Int_t M_ARGUS_DISPLAY_VIEW[kMaxNumberOfPadsX][kMaxNumberOfPadsY];

protected:
   TGMenuBar           *fMenuBar;       //! menu bar
   TGPopupMenu         *fMenuDisplay;   //!
   TGPopupMenu         *fMenuView;      //!
   TGPopupMenu         *fMenuViewDivide;//!
   TGPopupMenu         *fMenuView0_19;  //!
   TGPopupMenu         *fMenuView20_39; //!
   TGPopupMenu         *fMenuView40_59; //!
   TGPopupMenu         *fMenuView60_79; //!
   TGPopupMenu         *fMenuViewDivideColumn[kMaxNumberOfPadsX]; //!
   TGHProgressBar      *fProgress;      //! 
   TRootEmbeddedCanvas *fStatus;        //! 
   TRootEmbeddedCanvas *fStatusText;    //! 
   TText               *fText;          //! 

   Int_t                fDisplayType;     //! 
   Int_t                fDisplayTypeOld;  //! 
   Int_t                fDisplayObjIndex; //! 
   Bool_t               fInherited;       //! 
   ROMEString           fInheritanceName; //! 
   Bool_t               fTabActive;       //! 

   TGraph             **fUserTGraph;         //! 
   TH1F               **fUserTH1F;           //! 
   TH2F               **fUserTH2F;           //! 
   Int_t                fNumberOfUserTGraph; //!
   Int_t                fNumberOfUserTH1F;   //!
   Int_t                fNumberOfUserTH2F;   //!

   TLine             ***fUserTGraphLines;    //!
   TLine             ***fUserTH1FLines;      //!
   TLine             ***fUserTH2FLines;      //!
   Int_t                fNumberOfUserTGraphLines; //!
   Int_t                fNumberOfUserTH1FLines;   //!
   Int_t                fNumberOfUserTH2FLines;   //!

   TH1F                *fTH1F[kMaxNumberOfPads];   //! 
   TH2F                *fTH2F[kMaxNumberOfPads];   //! 
   TGraph              *fTGraph[kMaxNumberOfPads]; //! 
   Int_t                fNumberOfPads;             //! 
   Int_t                fNumberOfPadsX;            //! 
   Int_t                fNumberOfPadsY;            //! 

   TLine               *fTGraphLines[kMaxNumberOfPads][kMaxNumberOfLines]; //!
   TLine               *fTH1FLines[kMaxNumberOfPads][kMaxNumberOfLines];   //!
   TLine               *fTH2FLines[kMaxNumberOfPads][kMaxNumberOfLines];   //!
   Int_t                fNumberOfTGraphLines; //!
   Int_t                fNumberOfTH1FLines;   //!
   Int_t                fNumberOfTH2FLines;   //!

   TRootEmbeddedCanvas *fCanvas;                   //! 
   TPad                *fPad[kMaxNumberOfPads];    //! 

   XMLToForm           *fDialog; //! 

   Int_t                fChannelNumber; //! 

   Bool_t               fPadConfigActive;                    //! 
   Int_t                fPadConfigChannel[kMaxNumberOfPads]; //! 

public:
   ArgusHistoDisplay();
   virtual ~ArgusHistoDisplay();

   void BaseInit();
   void BaseMenuClicked(TGPopupMenu *menu,Long_t param);
   void BaseTabSelected();
   void BaseTabUnSelected();
   void SetupPads(Int_t nx, Int_t ny, Bool_t redraw);
   void SetStatus(Int_t mode,const char *text,double progress,Int_t sleepingTime=10);
   void SetStatisticBox(Bool_t flag);
   void Modified(Bool_t processEvents=true);
   void SetLimits(TGraph *g);

   virtual void Init() = 0;
   virtual void EndInit() = 0;
   virtual void MenuClicked(TGPopupMenu *menu,Long_t param) = 0;
   virtual void TabSelected() = 0;
   virtual void TabUnSelected() = 0;
   virtual void BaseEventHandler() = 0;
   virtual void EventHandler() = 0;
   virtual void Display(Bool_t processEvents=true) = 0;

   ClassDef(ArgusHistoDisplay,0) // Base class of ARGUS histogram display
};

#endif   // ArgusHistoDisplay_H
