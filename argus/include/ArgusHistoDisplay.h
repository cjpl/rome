////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Histogram Display.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
// $Id: ArgusHistoDisplay.h 3501 2006-05-15 15:43:23Z schneebeli_m $

#ifndef ArgusHistoDisplay_H
#define ArgusHistoDisplay_H

#include <RConfig.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // R__VISUAL_CPLUSPLUS
#include <TCanvas.h>
#include <TColor.h>
#include <TLine.h>
#include <TGTextEdit.h>
#include <TGTextEntry.h>
#include <TGListBox.h>
#include <TRootEmbeddedCanvas.h>
#include <TTimer.h>
#include <TText.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TSpline.h>
#include <TGProgressBar.h>
#include <TGButton.h>
#include <TFrame.h>
#include <TH2.h>
#include <TFile.h>
#include <TF1.h>
#if defined( R__VISUAL_CPLUSPLUS )
#pragma warning( pop )
#endif // R__VISUAL_CPLUSPLUS
#include "XMLToForm.h"
#include "ArgusTab.h"

class ArgusHistoDisplay : public ArgusTab
{
protected:
   enum {
      kMaxNumberOfPads = 120,
      kMaxNumberOfPadsX = 6,
      kMaxNumberOfPadsY = 20
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
   int M_ARGUS_DISPLAY_VIEW[kMaxNumberOfPadsX][kMaxNumberOfPadsY];

protected:
   TGMenuBar*   fMenuBar;          //! menu bar
   TGPopupMenu* fMenuDisplay; //!
   TGPopupMenu* fMenuView; //!
   TGPopupMenu* fMenuViewDivide; //!
   TGPopupMenu* fMenuView0_19; //!
   TGPopupMenu* fMenuView20_39; //!
   TGPopupMenu* fMenuView40_59; //!
   TGPopupMenu* fMenuView60_79; //!
   TGPopupMenu* fMenuViewDivideColumn[kMaxNumberOfPadsX]; //!
   TGHProgressBar *fProgress; //! 
   TRootEmbeddedCanvas *fStatus,*fStatusText; //! 
   TText *fText; //! 


   int fDisplayType; //! 
   int fDisplayTypeOld; //! 
   int fDisplayObjIndex; //! 
   bool fInherited; //! 
   ROMEString fInheritanceName; //! 
   bool fTabActive; //! 

   TGraph** fUserTGraph; //! 
   TH1F** fUserTH1F; //! 
   TH2F** fUserTH2F; //! 
   int fNumberOfUserTGraph; //!
   int fNumberOfUserTH1F; //!
   int fNumberOfUserTH2F; //!

   TRootEmbeddedCanvas *fCanvas; //! 
   TPad *fPad[kMaxNumberOfPads]; //! 

   int fNumberOfPads; //! 
   int fNumberOfPadsX; //! 
   int fNumberOfPadsY; //! 
   TH1F   *fTH1F[kMaxNumberOfPads]; //! 
   TH2F   *fTH2F[kMaxNumberOfPads]; //! 
   TGraph *fTGraph[kMaxNumberOfPads]; //! 

   XMLToForm *fDialog; //! 

   int fChannelNumber; //! 

   bool fPadConfigActive; //! 
   int fPadConfigChannel[kMaxNumberOfPads]; //! 


public:
   ArgusHistoDisplay():ArgusTab()
   {
      int i,j;
      for (i=0;i<kMaxNumberOfPadsX;i++) {
         for (j=0;j<kMaxNumberOfPadsY;j++) {
            M_ARGUS_DISPLAY_VIEW[i][j] = M_ROOT-400+i*kMaxNumberOfPadsY+j;
         }
      }
      fDisplayType = kNoDisplay;
      fDisplayTypeOld = kNoDisplay;
      fDisplayObjIndex = 0;
      fInherited = false;
      fInheritanceName = "";
      fPadConfigActive = false;
      fTabActive = false;
      fNumberOfUserTGraph = kMaxNumberOfPads;
      fNumberOfUserTH1F = kMaxNumberOfPads;
      fNumberOfUserTH2F = kMaxNumberOfPads;
   }

   ~ArgusHistoDisplay()
   {
   }

   void BaseInit();
   virtual void Init() = 0;
   virtual void EndInit() = 0;
   void BaseMenuClicked(TGPopupMenu *menu,Long_t param);
   virtual void MenuClicked(TGPopupMenu *menu,Long_t param) = 0;
   void BaseTabSelected();
   virtual void TabSelected() = 0;
   void BaseTabUnSelected();
   virtual void TabUnSelected() = 0;
   virtual void BaseEventHandler() = 0;
   virtual void EventHandler() = 0;

   virtual void Display(bool processEvents=true) = 0;

   void   SetupPads(int nx, int ny, bool redraw);
   void   SetStatus(int mode,const char *text,double progress,int sleepingTime=10);
   void   SetStatisticBox(bool flag);

   void   Modified(bool processEvents=true);

   ClassDef(ArgusHistoDisplay,1) // Base class of ARGUS histogram display
};

#endif   // ArgusHistoDisplay_H
