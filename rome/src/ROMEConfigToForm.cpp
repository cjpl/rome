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

ROMEConfigToForm::ROMEConfigToForm()
{
   fWindow = NULL;
   fXML = NULL;
   fSubstitutes = NULL;
   fPlaceHolders = NULL;
   fMainFrame = NULL;
}

ROMEConfigToForm::~ROMEConfigToForm()
{
}

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

bool ROMEConfigToForm::Init()
{
   // read xml
   if (!XMLToRootClass())
      return false;

   XMLToClass(fMainFrame);
   return true;
}

bool ROMEConfigToForm::XMLToRootClass()
{
   ROMEString value;
   ROMEString valueT;
   fMaximalWindowWidth = 1000;

   fMainFrame = new XMLToFormFrame(NULL,"","",true,XMLToFormFrame::kFrame,true,0);

   return true;
}

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
void ROMEConfigToForm::FillRunModeFrame(XMLToFormFrame *frame)
{
   XMLToFormElementSignal *signal;
   ROMEString rome = "false";
   ROMEString argus = "false";
   if (gROME->IsROMEAndARGUS() || gROME->IsStandAloneROME())
      rome = "true";
   if (gROME->IsROMEAndARGUS() || gROME->IsStandAloneARGUS())
      argus = "true";
   // ROME
   signal = new XMLToFormElementSignal("Clicked()","State");
   signal->AddReceiver("Tasks","Enable");
   frame->AddElement(new XMLToFormElement("CheckButton","ROME",rome.Data(),"",0,0,"Check, if you want to run an analyzer",signal));
   // ARGUS
   signal = new XMLToFormElementSignal("Clicked()","State");
   signal->AddReceiver("Tabs","Enable");
   signal->AddReceiver("Argus","Enable");
   frame->AddElement(new XMLToFormElement("CheckButton","ARGUS",argus.Data(),"",0,1,"Check, if you want to run a monitor",signal));
}

void ROMEConfigToForm::FillButtonFrame(XMLToFormFrame *frame)
{
   // Ok
   frame->AddElement(new XMLToFormElement("Button","Ok","","",0,1));
   // Cancel
   frame->AddElement(new XMLToFormElement("Button","Cancel","","",0,0));
   // Terminate
   frame->AddElement(new XMLToFormElement("Button","Terminate","","",0,-1));
}
