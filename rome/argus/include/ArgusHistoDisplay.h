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
#include "TGraphMT.h"

class ROMEStrArray;
class TGMenuBar;
class TGPopupMenu;
class TGHProgressBar;
class TRootEmbeddedCanvas;
class TText;
class TPad;
class TLine;
class TH1;
class XMLToForm;
class TStyle;
class TArrayI;

class ArgusHistoDisplay : public ArgusTab
{
protected:
   enum {
      kMaxNumberOfPads = 120,
      kMaxNumberOfPadsX = 6,
      kMaxNumberOfPadsY = 20,
      kNumberOfDisplayViewSelections = 1000,
      kMaxNumberOfLines = 20
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
   TStyle              *fStyle;         //!
   TGMenuBar           *fMenuBar;       //! menu bar
   TGPopupMenu         *fMenuDisplay;   //!
   TGPopupMenu         *fMenuView;      //!
   TGPopupMenu         *fMenuViewDivide;//!
   TGPopupMenu         *fMenuViewSelect;//!
   TGPopupMenu         *fMenuView10[10][10];  //!
   TGPopupMenu         *fMenuView100[10]; //!
   TGPopupMenu         *fMenuViewDivideColumn[kMaxNumberOfPadsX]; //!

   Int_t                fCurrentDisplayType;//!
   Int_t                fNumberOfDisplayTypes;//!
   Int_t                fDisplayTypeOld;  //!
   Int_t                fDisplayObjIndex; //!
   TArrayI             *fDisplayObjLoaded;//!
   Bool_t               fInherited;       //!
   ROMEString           fInheritanceName; //!
   Bool_t               fTabActive;       //!

   TObjArray           *fUserObjects;     //!
   TObjArray           *fObjects;         //!
   Int_t                fNumberOfObjects; //!
   TObjArray           *fUserLines;       //!
   TObjArray           *fLines;           //!
   ROMEStrArray        *fDrawOption;      //!
   Int_t                fNumberOfUserTGraph;//!
   Int_t                fNumberOfUserLines;//!
   Int_t                fNumberOfLines;   //!

   Int_t                fNumberOfPads;             //!
   Int_t                fNumberOfPadsX;            //!
   Int_t                fNumberOfPadsY;            //!

   Bool_t               fStatisticBoxFlag; //!

   TRootEmbeddedCanvas *fCanvas;                   //!
   TPad                *fPad[kMaxNumberOfPads];    //!

   XMLToForm           *fDialog; //!

   Int_t                fChannelNumber; //!

   Bool_t               fPadConfigActive;                    //!
   Int_t                fPadConfigChannel[kMaxNumberOfPads]; //!

public:
   ArgusHistoDisplay(ArgusWindow* window);
   virtual ~ArgusHistoDisplay();

   void SetNumberOfUserTGraph(Int_t number) { fNumberOfUserTGraph = number; };
   TGraphMT* GetUserTGraphAt(Int_t index);
   TH1* GetUserHistoAt(Int_t index,const char* type);
   TObject* GetCurrentObjectAt(Int_t index);
   void SetNumberOfUserLines(Int_t number) { fNumberOfUserLines = number; };
   Int_t GetNumberOfUserLines() { return fNumberOfUserLines; };
   TLine* GetUserLineAt(Int_t histoIndex,Int_t lineIndex);
   void SetDrawOptionAt(Int_t displayTypeIndex,const char* option);
   const char* GetDrawOptionAt(Int_t displayTypeIndex);

   void SetNumberOfPadsX(int n) { fNumberOfPadsX = n; };
   int  GetNumberOfPadsX() { return fNumberOfPadsX; };
   void SetNumberOfPadsY(int n) { fNumberOfPadsY = n; };
   int  GetNumberOfPadsY() { return fNumberOfPadsY; };
   void   SetPadConfigActive(Bool_t flag) { fPadConfigActive = flag; };
   Bool_t IsPadConfigActive() { return fPadConfigActive; };

   virtual void RegisterObjects() = 0;
   virtual void UnRegisterObjects() = 0;

protected:
   void BaseInit();
   void BaseMenuClicked(TGPopupMenu *menu,Long_t param);
   void BaseTabSelected();
   void BaseTabUnSelected();
   void BaseSetupPads(Int_t nx, Int_t ny, Bool_t redraw);
   void SetStatisticBox(Bool_t flag);
   void Modified(Bool_t processEvents=true);
   void SetLimits(TGraphMT *g);

   virtual void Init() = 0;
   virtual void EndInit() = 0;
   virtual void MenuClicked(TGPopupMenu *menu,Long_t param) = 0;
   virtual void TabSelected() = 0;
   virtual void TabUnSelected() = 0;
   virtual void BaseEventHandler() = 0;
   virtual void EventHandler() = 0;
   virtual void Display(Bool_t processEvents=true) = 0;
   virtual void SetupPads(Int_t nx, Int_t ny, Bool_t redraw) = 0;

   ClassDef(ArgusHistoDisplay,0) // Base class of ARGUS histogram display
};

#endif   // ArgusHistoDisplay_H
