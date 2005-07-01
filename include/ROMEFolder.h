/********************************************************************
  ROMEFolder.h, M. Schneebeli PSI

  $Log$
  Revision 1.1  2005/07/01 07:35:20  sawada
  Allocate arrayed support folders.
  SetModified() calls parent's SetModified().
  Created ROMEFolder class.
  AddFoldername method.


********************************************************************/

#ifndef ROMEFolder_H
#define ROMEFolder_H

#include <Riostream.h>
#include <TObject.h>
#include <TClass.h>
#include <TClonesArray.h>

class ROMEFolder : public TObject
{
protected:
   Bool_t         fModified;     //! Modified Folder Flag
   ROMEFolder*    fParentFolder; //! Address of parent folder
   
public:
   ROMEFolder( )
   {
      ROMEFolder::Class()->IgnoreTObjectStreamer();
      fModified = false;
      fParentFolder = 0;
   };
   
   Bool_t      isModified() { return fModified;   };
   void        SetModified( Bool_t modified  ) {
      fModified   = modified;
      if(modified && fParentFolder)
         fParentFolder->SetModified(true);
   };
   void        SetParentFolder( ROMEFolder* parent ) { fParentFolder = parent; };
   ROMEFolder* GetParentFolder() { return fParentFolder; };
   
   ClassDef(ROMEFolder,1)
};

#endif   // ROMEFolder_H
