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
class TPad;
class TGraph;
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

   Int_t                fCurrentDisplayType;//! 
   Int_t                fDisplayTypeOld;  //! 
   Int_t                fDisplayObjIndex; //! 
   Bool_t               fInherited;       //! 
   ROMEString           fInheritanceName; //! 
   Bool_t               fTabActive;       //! 

   TObjArray           *fUserObjects;     //!
   TObjArray           *fObjects;         //!
   TObjArray           *fUserLines;       //!
   TObjArray           *fLines;           //!
   Int_t                fNumberOfUserTGraph;//!
   Int_t                fNumberOfUserLines;//!
   Int_t                fNumberOfLines;   //!

   Int_t                fNumberOfPads;             //! 
   Int_t                fNumberOfPadsX;            //! 
   Int_t                fNumberOfPadsY;            //! 

   TRootEmbeddedCanvas *fCanvas;                   //! 
   TPad                *fPad[kMaxNumberOfPads];    //! 

   XMLToForm           *fDialog; //! 

   Int_t                fChannelNumber; //! 

   Bool_t               fPadConfigActive;                    //! 
   Int_t                fPadConfigChannel[kMaxNumberOfPads]; //! 

public:
   ArgusHistoDisplay();
   virtual ~ArgusHistoDisplay();

   void SetNumberOfUserTGraph(Int_t number) { fNumberOfUserTGraph = number; };
   TGraph* GetUserTGraphAt(Int_t index);
   TObject* GetCurrentObjectAt(Int_t index);
   void SetNumberOfUserLines(Int_t number) { fNumberOfUserLines = number; };
   TLine* GetUserLineAt(Int_t histoIndex,Int_t lineIndex);

protected:
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
