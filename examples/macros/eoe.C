{
    cout<<"EOE: ADC sum is "<<gAnalyzer->GetSum()->GetADCSum()<<endl;

   // skip filling trees when ADC sum is small
   if (gAnalyzer->GetSum()->GetADCSum() < 10000) {
      cout<<"skip filling trees"<<endl;
      gAnalyzer->GetActiveDAQ()->SetContinue();
   }
}
