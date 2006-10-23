/********************************************************************
  ROMEConfigGraph.h, M. Schneebeli PSI

  $Id: ROMEConfigGraph.h 1433 2006-10-10 17:11:13Z sawada $

********************************************************************/
#ifndef ROMEConfigGraph_H
#define ROMEConfigGraph_H

#include <ROMEString.h>
#include <TObject.h>

class ROMEConfigGraph : public TObject
{
public:
   ROMEString   fGraphTitle;
   Bool_t       fGraphTitleModified;
   ROMEString   fGraphFolderTitle;
   Bool_t       fGraphFolderTitleModified;
   ROMEString   fGraphArraySize;
   Bool_t       fGraphArraySizeModified;
   ROMEString   fGraphArrayStartIndex;
   Bool_t       fGraphArrayStartIndexModified;
   ROMEString   fGraphXLabel;
   Bool_t       fGraphXLabelModified;
   ROMEString   fGraphYLabel;
   Bool_t       fGraphYLabelModified;
   ROMEString   fGraphZLabel;
   Bool_t       fGraphZLabelModified;
   ROMEString   fGraphXmin;
   Bool_t       fGraphXminModified;
   ROMEString   fGraphXmax;
   Bool_t       fGraphXmaxModified;
   ROMEString   fGraphYmin;
   Bool_t       fGraphYminModified;
   ROMEString   fGraphYmax;
   Bool_t       fGraphYmaxModified;
   ROMEString   fGraphZmin;
   Bool_t       fGraphZminModified;
   ROMEString   fGraphZmax;
   Bool_t       fGraphZmaxModified;
   ROMEConfigGraph() {
      fGraphTitleModified = false;
      fGraphFolderTitleModified = false;
      fGraphArraySizeModified = false;
      fGraphArrayStartIndexModified = false;
      fGraphXLabelModified = false;
      fGraphYLabelModified = false;
      fGraphZLabelModified = false;
      fGraphXminModified = false;
      fGraphXmaxModified = false;
      fGraphYminModified = false;
      fGraphYmaxModified = false;
      fGraphZminModified = false;
      fGraphZmaxModified = false;
   }
   virtual ~ROMEConfigGraph() {}

   ClassDef(ROMEConfigGraph, 0) // Graph configuration class
};

#endif   // ROMEConfigGraph_H
