//// Author: Matthias Schneebeli
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// ROMEConfigToForm                                                           //
//                                                                            //
// This class generates a dialog window dynamically out of a
// ROME configuration file.
//                                                                            //
//                                                                            //
/////////////////////////////////////----///////////////////////////////////////
// $Id$

#include "TError.h"
#include "ROMEConfigToForm.h"
#include "XMLToFormWindow.h"
#include "XMLToFormFrame.h"
#include "XMLToFormElement.h"
#include "XMLToFormElementSignal.h"
#include "ROMEAnalyzer.h"

ClassImp(ROMEConfigToForm)

//______________________________________________________________________________
ROMEConfigToForm::ROMEConfigToForm()
:XMLToForm()
,fCommentLevel(0)
{
}

//______________________________________________________________________________
ROMEConfigToForm::~ROMEConfigToForm()
{
}

//______________________________________________________________________________
int ROMEConfigToForm::Show(const TGWindow * p, const TGWindow * main)
{
   int exitButtonID = 1;
   if (!Init())
      return -1;
   gErrorIgnoreLevel = 3000;
   fWindow = new XMLToFormWindow(p,main,fMainFrame,&exitButtonID,fMaximalWindowWidth);
   gErrorIgnoreLevel = 0;
   return exitButtonID;
}

//______________________________________________________________________________
bool ROMEConfigToForm::Init()
{
   // read xml
   if (!XMLToRootClass())
      return false;

   XMLToClass(fMainFrame);
   return true;
}

//______________________________________________________________________________
bool ROMEConfigToForm::XMLToRootClass()
{
   ROMEString value;
   ROMEString valueT;
   fMaximalWindowWidth = 1000;

   fMainFrame = new XMLToFormFrame(NULL,"","",true,XMLToFormFrame::kFrame,true,0);

   return true;
}

//______________________________________________________________________________
void ROMEConfigToForm::XMLToClass(XMLToFormFrame *frame)
{
   ROMEString currentPath;
   int nFrames;
   nFrames = 0;

   // Run Mode Frame
   frame->AddSubFrame(new XMLToFormFrame(frame,"RunMode",frame->GetFramePath().Data(),true,XMLToFormFrame::kFrame,true,nFrames));
   FillRunModeFrame(frame->GetSubFrameAt(nFrames));
   nFrames++;
   // Tab Frame
   frame->AddSubFrame(new XMLToFormFrame(frame,"Config",frame->GetFramePath().Data(),true,XMLToFormFrame::kListTree,true,nFrames));
   AddConfig(frame->GetSubFrameAt(nFrames));
   nFrames++;
   // Button Frame
   frame->AddSubFrame(new XMLToFormFrame(frame,"Buttons",frame->GetFramePath().Data(),true,XMLToFormFrame::kFrame,true,nFrames));
   FillButtonFrame(frame->GetSubFrameAt(nFrames));
   nFrames++;
}

//______________________________________________________________________________
void ROMEConfigToForm::FillRunModeFrame(XMLToFormFrame *frame)
{
   XMLToFormElementSignal *signal;
   ROMEString rome = "false";
   ROMEString argus = "false";
   ROMEString romeandargus = "false";
   ROMEString romemonitor = "false";
   if (gROME->IsStandAloneROME())
      rome = "true";
   if (gROME->IsStandAloneARGUS())
      argus = "true";
   if (gROME->IsROMEAndARGUS())
      romeandargus = "true";
   if (gROME->IsROMEMonitor())
      romemonitor = "true";
   // ROME
   signal = new XMLToFormElementSignal("Clicked()","State");
   signal->AddReceiver("Tasks","Enable");
   frame->AddElement(new XMLToFormElement("RadioButton","Analyzer",rome.Data(),"",0,10,"Check, if you want to run a stand alone analyzer",signal));
   // ARGUS
   signal = new XMLToFormElementSignal("Clicked()","State");
   signal->AddReceiver("Tabs","Enable");
   signal->AddReceiver("Argus","Enable");
   frame->AddElement(new XMLToFormElement("RadioButton","Monitor",argus.Data(),"",0,11,"Check, if you want to run a stand alone monitor",signal));
   // ROME and ARGUS
   signal = new XMLToFormElementSignal("Clicked()","State");
   signal->AddReceiver("Tasks","Enable");
   signal->AddReceiver("Tabs","Enable");
   signal->AddReceiver("Argus","Enable");
   frame->AddElement(new XMLToFormElement("RadioButton","Analyzer with Monitor",romeandargus.Data(),"",0,12,"Check, if you want to run an analyzer with a monitor",signal));
   // ROME Monitor
   signal = new XMLToFormElementSignal("Clicked()","State");
   signal->AddReceiver("Tabs","Enable");
   signal->AddReceiver("Argus","Enable");
   frame->AddElement(new XMLToFormElement("RadioButton","Monitor connecting to Analyzer",romemonitor.Data(),"",0,13,"Check, if you want to run a monitor connecting to a rome analyzer",signal));
}

//______________________________________________________________________________
void ROMEConfigToForm::FillButtonFrame(XMLToFormFrame *frame)
{
   // Ok
   frame->AddElement(new XMLToFormElement("Button","Ok","","",0,1));
   // Cancel
   frame->AddElement(new XMLToFormElement("Button","Cancel","","",0,0));
   // Terminate
   frame->AddElement(new XMLToFormElement("Button","Terminate","","",0,-1));
}
