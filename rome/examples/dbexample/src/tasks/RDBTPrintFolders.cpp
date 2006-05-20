// Author: Ryu Sawada

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// RDBTPrintFolders                                                           //
//                                                                            //
// This task shows containts of folders.                                      //
//                                                                            //
// The event methods have been written by Ryu Sawada.                         //
//                                                                            //
// Please note: The following information is only correct after executing     //
// the ROMEBuilder.                                                           //
//                                                                            //
// This task accesses the following folders :                                 //
//     SingleFolder                                                           //
//     ArrayFolder                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
/*  This header was generated by ROMEBuilder. Manual changes above the        *
 * following line will be lost next time ROMEBuilder is executed.             */
/////////////////////////////////////----///////////////////////////////////////

#include <include/generated/RDBTPrintFolders.h>
#include <Riostream.h>
#include <TTimeStamp.h>

ClassImp(RDBTPrintFolders)

void RDBTPrintFolders::Init()
{
}

void RDBTPrintFolders::BeginOfRun()
{
}

void RDBTPrintFolders::Event()
{
   cout<<" --------------- SingleFolder  ---------------"<<endl;
   cout<<"RunNumber :"<<gAnalyzer->GetCurrentRunNumber()<<endl;
   cout<<"bbb       :"<<gAnalyzer->GetSingleFolder()->Getbbb()<<endl<<endl;
   cout<<"ggg       :"<<gAnalyzer->GetSingleFolder()->Getggg()<<endl<<endl;
   Int_t i,j;
   cout << "1" << endl;
   for(i=0;i<gAnalyzer->GetSingleFolder()->Getbbb();i++){
      cout<<" -------------- ArrayFolder("<<i<<")  --------------"<<endl;
   cout << i<<" "<<gAnalyzer->GetArrayFolderAt(i) << endl;
      cout<<"ccc1      :"<<gAnalyzer->GetArrayFolderAt(i)->Getccc1()<<endl;
      cout<<"ccc2      :"<<gAnalyzer->GetArrayFolderAt(i)->Getccc2()<<endl;
   cout << "13" << endl;
      cout<<"ccc3      :"<<gAnalyzer->GetArrayFolderAt(i)->Getccc3()<<endl;
      for(j=0;j<4;j++)
         cout<<"ddd("<<j<<")    :"<<gAnalyzer->GetArrayFolderAt(i)->GetdddAt(j)<<endl;
      cout<<"jjj1      :"<<gAnalyzer->GetArrayFolderAt(i)->Getjjj1()<<endl;
      cout<<"jjj2      :"<<gAnalyzer->GetArrayFolderAt(i)->Getjjj2()<<endl;
   }

}

void RDBTPrintFolders::EndOfRun()
{
}

void RDBTPrintFolders::Terminate()
{
}

