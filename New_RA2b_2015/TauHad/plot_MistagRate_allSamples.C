void plot_MistagRate_allSamples(string sample="TTbar_", int wjets_hadtau_only=0){

  //
  // icomp=0: only show own results
  //       1: show also Koushik's results
  //
  
  //
  ///////////////////////////////////////////////////////////////////////////////////////////
  ////Some cosmetic work for official documents. 
  gStyle->SetOptStat(0);  ///to avoid the stat. on the plots 
  gROOT->LoadMacro("tdrstyle.C");
  //  setTDRStyle();
  gROOT->LoadMacro("CMS_lumi_v2.C");

  char tempname[200];
  char tempname2[200];
  int W = 800;
  int H = 600;
  int H_ref = 600;
  int W_ref = 800;
  float T = 0.08*H_ref;
  float B = 0.12*H_ref;
  float L = 0.12*W_ref;
  float R = 0.04*W_ref;

  TCanvas* c1 = new TCanvas("name","name",10,10,W,H);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);
  c1->SetFrameFillStyle(0);
  c1->SetFrameBorderMode(0);
  c1->SetLeftMargin( L/W );
  c1->SetRightMargin( R/W );
  c1->SetTopMargin( T/H );
  c1->SetBottomMargin( B/H );
  c1->SetTickx(0);
  c1->SetTicky(0);
  
  Float_t legendX1 = .15; //.50;
  Float_t legendX2 = .50; //.70;
  Float_t legendY1 = .55; //.65;
  Float_t legendY2 = .88;
  TLegend* catLeg1 = new TLegend(legendX1,legendY1,legendX2,legendY2);
  catLeg1->SetTextSize(0.042);
  catLeg1->SetTextFont(42);
  catLeg1->SetFillColor(0);
  catLeg1->SetLineColor(0);
  catLeg1->SetBorderSize(0);

  TH1D * temp_tt_tauH, * temp_wj_tauH, *temp_tt_muH, * temp_wj_muH;
  
  //
  // From W+jets
  TFile file_WJet("Stack/ARElog78_TauBtaggedRate_WJet_stacked_TauMistag_Expectation.root","R");
  temp_wj_tauH = (TH1D*)file_WJet.Get("TauBtaggedRate")->Clone();
  TH1D * wj_tauH = new TH1D("wj_tauH","WJet -- Tau mistag",temp_wj_tauH->GetNbinsX(),temp_wj_tauH->GetXaxis()->GetXmin(),temp_wj_tauH->GetXaxis()->GetXmax());
  for(int ibin=0;ibin<wj_tauH->GetNbinsX()+2;ibin++){
    double con = (double)temp_wj_tauH->GetBinContent(ibin);
    double err = (double)temp_wj_tauH->GetBinError(ibin);
    cout << "con: " << con << " err: " << err << endl;
    wj_tauH->SetBinContent(ibin,con);
    wj_tauH->SetBinError(ibin,err);
  }

  //
  // From ttbar
  TFile file_TTbar("Stack/ARElog78_TauBtaggedRate_TTbar_stacked_TauMistag_Expectation.root","R");  
  temp_tt_tauH = (TH1D*)file_TTbar.Get("TauBtaggedRate")->Clone();
  TH1D * tt_tauH = new TH1D("tt_tauH","WJet -- Tau mistag",temp_tt_tauH->GetNbinsX(),temp_tt_tauH->GetXaxis()->GetXmin(),temp_tt_tauH->GetXaxis()->GetXmax());
  for(int ibin=0;ibin<tt_tauH->GetNbinsX()+2;ibin++){
    double con = (double)temp_tt_tauH->GetBinContent(ibin);
    double err = (double)temp_tt_tauH->GetBinError(ibin);
    cout << "con: " << con << " err: " << err << endl;
    tt_tauH->SetBinContent(ibin,con);
    tt_tauH->SetBinError(ibin,err);
  }

  //
  // From W+jets - predictions
  TFile file_WJet_mu("../TauHad2/Stack/ARElog78_TauBtaggedRate_WJet_stacked_MuMistag_Prediction.root","R");  
  temp_wj_muH = (TH1D*)file_WJet_mu.Get("TauBtaggedRate")->Clone();
  TH1D * wj_muH = new TH1D("wj_muH","WJet -- Tau mistag",temp_wj_muH->GetNbinsX(),temp_wj_muH->GetXaxis()->GetXmin(),temp_wj_muH->GetXaxis()->GetXmax());
  for(int ibin=0;ibin<wj_muH->GetNbinsX()+2;ibin++){
    double con = (double)temp_wj_muH->GetBinContent(ibin);
    double err = (double)temp_wj_muH->GetBinError(ibin);
    cout << "con: " << con << " err: " << err << endl;
    wj_muH->SetBinContent(ibin,con);
    wj_muH->SetBinError(ibin,err);
  }

  //
  // From ttbar - predictions
  TFile file_TTbar_mu("../TauHad2/Stack/ARElog78_TauBtaggedRate_TTbar_stacked_MuMistag_Prediction.root","R");
  temp_tt_muH = (TH1D*)file_TTbar_mu.Get("TauBtaggedRate")->Clone();
  TH1D * tt_muH = new TH1D("tt_muH","WJet -- Tau mistag",temp_tt_muH->GetNbinsX(),temp_tt_muH->GetXaxis()->GetXmin(),temp_tt_muH->GetXaxis()->GetXmax());
  for(int ibin=0;ibin<tt_muH->GetNbinsX()+2;ibin++){
    double con = (double)temp_tt_muH->GetBinContent(ibin);
    double err = (double)temp_tt_muH->GetBinError(ibin);
    cout << "con: " << con << " err: " << err << endl;
    tt_muH->SetBinContent(ibin,con);
    tt_muH->SetBinError(ibin,err);
  }

  //  catLeg1->SetHeader("Prob. of #mu from #tau ");

  //...........................................................................//
  // TTbar ....................................................................//
  //...........................................................................//
  double XUp = 350. , maxVal=1.;
  double minVal=0.;
    wj_tauH->GetXaxis()->SetRangeUser(30.,XUp);
    wj_tauH->SetMaximum(maxVal);
    wj_tauH->SetTitle("");
    
    wj_tauH->GetXaxis()->SetLabelFont(42);
    wj_tauH->GetXaxis()->SetLabelOffset(0.007);
    wj_tauH->GetXaxis()->SetLabelSize(0.04);
    wj_tauH->GetXaxis()->SetTitleSize(0.05);
    wj_tauH->GetXaxis()->SetTitleOffset(1.0);
    wj_tauH->GetXaxis()->SetTitleFont(42);
    wj_tauH->GetYaxis()->SetLabelFont(42);
    wj_tauH->GetYaxis()->SetLabelOffset(0.007);
    wj_tauH->GetYaxis()->SetLabelSize(0.04);
    wj_tauH->GetYaxis()->SetTitleSize(0.05);
    wj_tauH->GetYaxis()->SetTitleOffset(1.0);
    wj_tauH->GetYaxis()->SetTitleFont(42);
    wj_tauH->GetXaxis()->SetTitle("p_{T}(#tau jet)");
    wj_tauH->GetYaxis()->SetTitle("b-tagging mistag rate");
    wj_tauH->GetXaxis()->SetRangeUser(30.,XUp);
    wj_tauH->SetMaximum(maxVal);
    wj_tauH->SetMinimum(minVal);
    wj_tauH->SetLineColor(1);
    wj_tauH->Draw();

    tt_tauH->SetLineColor(2);
    //    thist.SetFillColor(0);
    //    thist.SetLineWidth(3);
    if (!wjets_hadtau_only) tt_tauH->Draw("same");

    wj_muH->SetLineColor(8); 
    //    wj_muH->SetMarkerStyle(20);
    if (!wjets_hadtau_only) {
      wj_muH->GetXaxis()->SetRangeUser(30.,500.);
      wj_muH->Draw("same");
    }
      
    tt_muH->SetLineColor(4);   
    if (!wjets_hadtau_only){
      tt_muH->GetXaxis()->SetRangeUser(30.,500.);
      tt_muH->Draw("same");
    }
    
    // sanity plot
    TH1 * sanityH = static_cast<TH1D*>(wj_tauH->Clone("sanity"));
    sanityH->Add(tt_muH);
    TH1 * tempH = static_cast<TH1D*>(wj_tauH->Clone("sanity"));
    tempH->Multiply(tt_muH);
    sanityH->Add(tempH,-1);
    sanityH->SetLineColor(34); 
    if (!wjets_hadtau_only){
      sanityH->GetXaxis()->SetRangeUser(30.,500.);
      sanityH->Draw("same");
    }
    
    //
    // TLegend
    sprintf(tempname,"(a) #tau_{h}-jets in t#bar{t}");
    if (!wjets_hadtau_only) catLeg1->AddEntry(tt_tauH,tempname,"l");

    if (wjets_hadtau_only)
      sprintf(tempname,"#tau_{h}-jets in W+jets");
    else 
      sprintf(tempname,"(b) #tau_{h}-jets in W+jets");
    catLeg1->AddEntry(wj_tauH,tempname,"l");    

    sprintf(tempname,"(c) #mu-jets in t#bar{t}");
    if (!wjets_hadtau_only) catLeg1->AddEntry(tt_muH,tempname,"l");

    sprintf(tempname,"(d) #mu-jets in W+jets");
    if (!wjets_hadtau_only) catLeg1->AddEntry(wj_muH,tempname,"l");

    //sprintf(tempname,"blue+(1 - blue)*red ");
    if (!wjets_hadtau_only){
      sprintf(tempname,"(e) #mu-jets (t#bar{t}) + #tau_{h}-jets (W+jets)");
      catLeg1->AddEntry(sanityH,tempname,"l");      
      TH1F *sanityHdummy = (TH1F*) sanityH->Clone();
      sanityHdummy->SetLineColor(0);
      sprintf(tempname,"= (c) + [1-(c)]#times(b)");
      catLeg1->AddEntry(sanityHdummy,tempname,"l");      
    }
    catLeg1->Draw();
    
    if (wjets_hadtau_only) {
      sprintf(tempname,"TauBtaggedRate_allSamples_hadtau_Wjets.png");
      c1->Print(tempname);
      sprintf(tempname,"TauBtaggedRate_allSamples_hadtau_Wjets.pdf");
      c1->Print(tempname);
    } else {
      sprintf(tempname,"TauBtaggedRate_allSamples.png");
      c1->Print(tempname);
      sprintf(tempname,"TauBtaggedRate_allSamples.pdf");
      c1->Print(tempname);
    }
    
}
