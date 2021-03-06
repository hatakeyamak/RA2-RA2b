
void plot_RatioMuFromTau(int icomp=0){

  //
  // icomp=0: only show own results
  //       1: show also Koushik's results
  //
  
  //
  ///////////////////////////////////////////////////////////////////////////////////////////
  ////Some cosmetic work for official documents. 
  gROOT->LoadMacro("tdrstyle.C");
//  setTDRStyle();
  gROOT->LoadMacro("CMS_lumi_v2.C");

  char tempname[200];
  char tempname2[200];
  int W = 600;
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
  
  Float_t legendX1 = .60; //.50;
  Float_t legendX2 = .90; //.70;
  Float_t legendY1 = .60; //.65;
  Float_t legendY2 = .90;
  TLegend* catLeg1 = new TLegend(legendX1,legendY1,legendX2,legendY2);
  catLeg1->SetTextSize(0.042);
  catLeg1->SetTextFont(42);
  catLeg1->SetFillColor(0);
  catLeg1->SetLineColor(0);
  catLeg1->SetBorderSize(0);

  
//  TFile *file_08TeV         = new TFile("tauTemplatesRes_8TeV.root","R"); 
//  TFile *file_13TeV         = new TFile("Probability_Tau_mu_stacked.root","R"); 
  TFile *file_13TeV_TTbar   = new TFile("Probability_Tau_mu_TTbar_.root","R");

  TH1D * thist, * thist2;
  catLeg1->SetHeader("Prob. of #mu from #tau ");


//...........................................................................//
// TTbar ....................................................................//
//...........................................................................//

    sprintf(tempname,"hProb_Tau_mu");
    thist = (TH1D*)file_13TeV_TTbar->Get(tempname)->Clone();
    thist->SetLineColor(1);
//    thist->SetFillColor(0);
    thist->SetLineWidth(3);

      thist->SetTitle("");    
      thist->GetXaxis()->SetLabelFont(42);
      thist->GetXaxis()->SetLabelOffset(0.007);
      thist->GetXaxis()->SetLabelSize(0.04);
      thist->GetXaxis()->SetTitleSize(0.05);
      thist->GetXaxis()->SetTitleOffset(0.9);
      thist->GetXaxis()->SetTitleFont(42);
      thist->GetYaxis()->SetLabelFont(42);
      thist->GetYaxis()->SetLabelOffset(0.007);
      thist->GetYaxis()->SetLabelSize(0.04);
      thist->GetYaxis()->SetTitleSize(0.05);
      thist->GetYaxis()->SetTitleOffset(1.25);
      thist->GetYaxis()->SetTitleFont(42);
      thist->GetXaxis()->SetTitle("p_{T_{#mu}}");
      thist->GetYaxis()->SetTitle("Probability");

    
    thist->Draw();
    sprintf(tempname,"T#bar{T}");
    catLeg1->AddEntry(thist,tempname,"l");

//...........................................................................//
// TTbar + WJet .............................................................//
//...........................................................................//
/*
    sprintf(tempname,"hProb_Tau_mu");
    thist2 = (TH1D*)file_13TeV->Get(tempname)->Clone();
    thist2->SetLineColor(2);
  //  thist2->SetFillColor(0);
    thist2->SetLineWidth(1);
    thist2->Draw("same");
    sprintf(tempname,"T#bar{T} + WJ");
    catLeg1->AddEntry(thist2,tempname,"l");
    catLeg1->Draw();

*/
    c1->Print("Plot_Probability_Tau_mu.png");

}
