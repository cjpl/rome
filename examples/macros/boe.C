{
   cout<<endl<<"++++++++++++++++++++"<<endl;
   cout<<"BOE: event number is "<<gAnalyzer->GetCurrentEventNumber()<<endl;
   // skip if event number is odd
   if (gAnalyzer->GetCurrentEventNumber() % 2 != 0) {
      gAnalyzer->GetActiveDAQ()->SetContinue();
   }
}
