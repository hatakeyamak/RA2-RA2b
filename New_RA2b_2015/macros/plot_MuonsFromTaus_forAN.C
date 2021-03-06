

void plot_MuonsFromTaus_forAN(){

  //
  // icomp=0: only show own results
  //       1: show also Koushik's results
  //
  
  //
  ///////////////////////////////////////////////////////////////////////////////////////////
  ////Some cosmetic work for official documents. 
  gROOT->LoadMacro("tdrstyle.C");
  setTDRStyle();
  gROOT->LoadMacro("CMS_lumi.C");

  writeExtraText = true;
  extraText   = "       Supplementary (Simulation)";  // default extra text is "Preliminary"
  lumi_8TeV  = "19.1 fb^{-1}"; // default is "19.7 fb^{-1}"
  lumi_7TeV  = "4.9 fb^{-1}";  // default is "5.1 fb^{-1}"
  lumi_sqrtS = "13 TeV";       // used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)
  //cmsTextSize  = 0.60;
  //lumiTextSize = 0.52;

  int iPeriod = 0;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)
  int iPos=0;
  
  char tempname[200];
  char tempname2[200];
  int W = 1200;
  int H = 600;
  int H_ref = 600;
  int W_ref = 800;
  float T = 0.08*H_ref;
  float B = 0.12*H_ref;
  float L = 0.12*W_ref;
  float R = 0.04*W_ref;
  
  double ymax=1.0;
  double ymin=0.0;
  double ytext=0.82;
  
  TCanvas* c1 = new TCanvas("name","name",10,10,W,H);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);
  c1->SetFrameFillStyle(0);
  c1->SetFrameBorderMode(0);
  c1->SetLeftMargin( L/W );
  c1->SetRightMargin( R/W );
  c1->SetTopMargin( T/H );
  c1->SetBottomMargin( B/H );
  c1->SetTopMargin( 0.1 );
  c1->SetLeftMargin( 0.08 );
  //c1->SetTickx(0);
  //c1->SetTicky(0);

  gStyle->SetOptStat(000000);
  gStyle->SetErrorX(0.5); 
  
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

  //
  
  //sprintf(tempname,"TauHad2/Stack/Probability_Tau_mu_stacked.root");
  sprintf(tempname,"TauHad2/Stack/Elog401_Probability_Tau_mu_stacked.root");

  TFile *file   = new TFile(tempname,"R");

  sprintf(tempname,"hProb_Tau_mu");
  thist = (TH1D*)file->Get(tempname)->Clone();

  thist_input = static_cast<TH1D*>(thist->Clone("thist_input"));
  shift_bin(thist_input,thist);
  thist_fixed = static_cast<TH1D*>(thist->Clone("thist_fixed"));

  /*
  TH1F *thist_fixed = new TH1F("thist_fixed","thist_fixed",18,1.,19.);
  for (int ibin=0;ibin<18;ibin++){
    thist_fixed->SetBinContent(ibin+1,thist->GetBinContent(ibin+1));
    thist_fixed->SetBinError(ibin+1,thist->GetBinError(ibin+1));
  }
  */
  
  thist_fixed->SetLineColor(1);
  thist_fixed->SetLineWidth(3);
  thist_fixed->SetStats(kFALSE);

  thist_fixed->SetTitle("");
  
  thist_fixed->SetMaximum(ymax);
  thist_fixed->SetMinimum(0.0);
  thist_fixed->GetXaxis()->SetLabelFont(42);
  thist_fixed->GetXaxis()->SetLabelOffset(0.007);
  thist_fixed->GetXaxis()->SetLabelSize(0.045);
  thist_fixed->GetXaxis()->SetTitleSize(0.06);
  thist_fixed->GetXaxis()->SetTitleOffset(0.9);
  thist_fixed->GetXaxis()->SetTitleFont(42);
  thist_fixed->GetYaxis()->SetLabelFont(42);
  thist_fixed->GetYaxis()->SetLabelOffset(0.007);
  thist_fixed->GetYaxis()->SetLabelSize(0.045);
  thist_fixed->GetYaxis()->SetTitleSize(0.06);
  thist_fixed->GetYaxis()->SetTitleOffset(0.60);
  thist_fixed->GetYaxis()->SetTitleFont(42);

  //KH adhoc
  thist_fixed->SetBinContent(18,0.2);
  
  thist_fixed->GetYaxis()->SetTitle("Fraction of muons from #tau decays");
  thist_fixed->GetXaxis()->SetTitle("Bin number");
  thist_fixed->SetMarkerStyle(20);
  thist_fixed->Draw();

  TLatex * ttext1 = new TLatex(6.0 , ytext , "N_{jets}=4");
  ttext1->SetTextFont(42);
  ttext1->SetTextSize(0.05);
  ttext1->SetTextAlign(22);
  ttext1->Draw();

  TLatex * ttext2 = new TLatex(17.0 , ytext , "N_{jets}=5");
  ttext2->SetTextFont(42);
  ttext2->SetTextSize(0.05);
  ttext2->SetTextAlign(22);
  ttext2->Draw();

  TLatex * ttext3 = new TLatex(28.0 , ytext , "N_{jets}=6");

  ttext3->SetTextFont(42);
  ttext3->SetTextSize(0.05);
  ttext3->SetTextAlign(22);
  ttext3->Draw();

  TLatex * ttext4 = new TLatex(36.5, ytext , "7#leqN_{jets}#leq8");
  ttext4->SetTextFont(42);
  ttext4->SetTextSize(0.05);
  ttext4->SetTextAlign(22);
  ttext4->Draw();

  TLatex * ttext5 = new TLatex(42.5 , ytext , "N_{jets}#geq9");
  ttext5->SetTextFont(42);
  ttext5->SetTextSize(0.05);
  ttext5->SetTextAlign(22);
  ttext5->Draw();

  TLine *tline_1 = new TLine(11.5,ymin,11.5,ymax);
  tline_1->SetLineStyle(2);
  tline_1->Draw();

  TLine *tline_2 = new TLine(22.5,ymin,22.5,ymax);
  tline_2->SetLineStyle(2);
  tline_2->Draw();
  
  TLine *tline_3 = new TLine(33.5,ymin,33.5,ymax);
  tline_3->SetLineStyle(2);
  tline_3->Draw();
  
  TLine *tline_4 = new TLine(39.5,ymin,39.5,ymax);
  tline_4->SetLineStyle(2);
  tline_4->Draw();

  CMS_lumi( c1, iPeriod, iPos );   // writing the lumi information and the CMS "logo"
  
  double xlatex=0.75;
  double ylatex=0.55;
  /*
  TLatex *   tex = new TLatex(xlatex,ylatex,"arXiv:1602.06581");
  tex->SetTextColor(4);
  tex->SetTextFont(61);
  tex->SetTextSize(0.055);
  tex->SetLineColor(4);
  tex->SetLineWidth(2);
  //tex->Draw();
  tex->DrawLatexNDC(xlatex,ylatex-0.2,"arXiv:1602.06581");
  */
  TPaveText pt(xlatex,ylatex,xlatex+0.19,ylatex+0.1,"NDC");
  pt.AddText("arXiv:1602.06581");
  pt.SetFillColor(0);
  pt.SetLineColor(0);
  pt.SetLineWidth(0);
  pt.SetBorderSize(0);
  pt.SetTextColor(4);
  pt.SetTextFont(61);
  pt.SetTextSize(0.055);
  pt.Draw();

  c1->Print("plot_MuonsFromTaus.pdf");

}

void shift_bin(TH1* input, TH1* output){

  char tempname[200];  
  char temptitle[200];  
  output->SetName(tempname);
  output->SetTitle(temptitle);
  output->SetBins(input->GetNbinsX(),input->GetBinLowEdge(1)-0.5,input->GetBinLowEdge(input->GetNbinsX()+1)-0.5);
  //input->Print("all");
  //output = new TH1D(tempname,temptitle,input->GetNbinsX(),input->GetBinLowEdge(1)-0.5,input->GetBinLowEdge(input->GetNbinsX()+1)-0.5); 
  // 0: underflow
  // 1: first bin [Use the lowedge of this bin]
  // input->GetNbinsX(): highest bin 
  // input->GetNbinsX()+1: overflow bin [use the lowedge of this bin]
  //

  for (int ibin=1;ibin<=input->GetNbinsX();ibin++){
    output->SetBinContent(ibin,input->GetBinContent(ibin));    
    output->SetBinError(ibin,input->GetBinError(ibin));    
    //std::cout << input->GetBinContent(ibin) << std::endl;
  }

}
