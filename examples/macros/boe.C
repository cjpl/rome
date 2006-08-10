{
   cout<<endl<<"++++++++++++++++++++"<<endl;
   cout<<"BOE: event number is "<<gAnalyzer->GetCurrentEventNumber()<<endl;

   // skip analysis if event number is odd
   if (gAnalyzer->GetCurrentEventNumber() % 2 != 0) {
      cout<<"skip analysis"<<endl;
      gAnalyzer->GetActiveDAQ()->SetContinue();
   }
}
