#include "TFile.h"
#include "TTree.h"


// Run this example with:
// .x treeCreationMakro.C+ 
//
// You can not run under CINT because an interpreted class cannot derive from a compiled class.
// Therefore 
// .x treeCreationMakro.C 
// does not work!!

class Branch1  : public TObject {
public:
   Double_t Px;
   Double_t Py;
   Double_t Pz;
   Int_t Np;

   Branch1() {};

   ClassDef(Branch1,1)
};
void treeCreationMakro() {
// Case 1:
   //create the file, the Tree and a few branches
   Double_t px, py, pz;
   Int_t i,np;
   TFile f1("tree1.root","recreate");
   TTree t1("T1","a simple Tree with simple variables");
   t1.Branch("Px",&px,"px/D");
   t1.Branch("Py",&py,"py/D");
   t1.Branch("Pz",&pz,"pz/D");
   t1.Branch("Np",&np,"np/I");

   //fill the tree
   for (i=0;i<100;i++) {
      px = i;
      py = 2*i;
      pz = px + py;
      np = i;
      t1.Fill(); }
   t1.Write();

// Case 2:
   //create the file, the Tree and a branch
   Branch1* b1 = new Branch1;
   TTree t2("T2","a Tree with a class");
   t2.Branch("Branch1","Branch1",&b1,8000,1);

   //fill the tree
   for (i=0;i<100;i++) {
      b1->Px = i;
      b1->Py = 2*i;
      b1->Pz = b1->Px + b1->Py;
      b1->Np = i;
      t2.Fill();
   }
   t2.Write();
}
