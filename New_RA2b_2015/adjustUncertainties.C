/*
  Plots the various systematics and adjusts the bins with large fluctuations.
  > root -l -b 
  > .L adjustUncertainties.C
  > adjustUncertainties(true)  // to adjust fluctuating bins, will create ARElog49_HadTauEstimation_data_formatted_New.root and data_formatted_systematics_new.pdf 
  > adjustUncertainties(false) // for original hists, will create ARElog49_HadTauEstimation_data_formatted_NoChange.root and data_formatted_systematics_NoChange.pdf
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "TH1.h"
#include "TH1D.h"

enum icol {ic_up=kRed, ic_dn=kBlue};

bool firstPrint=true;
bool lastPrint =true;

void adjustUncertainties(bool adjust=true) {

  TFile *file = TFile::Open("ARElog58_12.9ifb_UpdatedSysAndMuSys_HadTauEstimation_data_formatted.root");

  TFile *fout;  
  if(adjust==false)
    fout = TFile::Open("ARElog58_12.9ifb_UpdatedSysAndMuSys_HadTauEstimation_data_formatted_NoChange.root", "RECREATE");
  else
    fout = TFile::Open("ARElog58_12.9ifb_UpdatedSysAndMuSys_HadTauEstimation_data_formatted_New.root", "RECREATE");
  
  std::vector<TCanvas *> cc;
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "searchBin_nominal") );
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "searchBin_nominal_fullstatuncertainty") );
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "searchBin_StatUncertainties") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_closureUncertainty") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_closureUncertainty_adhoc") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_BMistag") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_MuRecoSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_MuIsoSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_MuRecoIso") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_JECSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_MTSys") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_MtEffStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_IsoTrkVetoEffUncertaintyStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_IsoTrkVetoEffUncertaintySys") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_AccStat") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_AccSysPDF") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "searchBin_AccSysScale") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_MuFromTauStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_DileptonUncertainty") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "searchBin_TrigEffUncertainty") );

  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "QCDBin_HiDphi_nominal") );
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "QCDBin_HiDphi_nominal_fullstatuncertainty") );
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "QCDBin_HiDphi_StatUncertainties") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_closureUncertainty") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_closureUncertainty_adhoc") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_BMistag") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_MuRecoSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_MuIsoSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_MuRecoIso") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_JECSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_MTSys") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_MtEffStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_IsoTrkVetoEffUncertaintyStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_IsoTrkVetoEffUncertaintySys") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_AccStat") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_AccSysPDF") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_HiDphi_AccSysScale") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_MuFromTauStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_DileptonUncertainty") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_HiDphi_TrigEffUncertainty") );

  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "QCDBin_LowDphi_nominal") );
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "QCDBin_LowDphi_nominal_fullstatuncertainty") );
  cc.push_back( adjustDisplayStyle     (file, fout,  adjust, "QCDBin_LowDphi_StatUncertainties") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_closureUncertainty") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_closureUncertainty_adhoc") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_BMistag") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_MuRecoSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_MuIsoSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_MuRecoIso") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_JECSys") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_MTSys") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_MtEffStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_IsoTrkVetoEffUncertaintyStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_IsoTrkVetoEffUncertaintySys") );  
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_AccStat") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_AccSysPDF") );
  cc.push_back( adjustUncertaintiesUpDn(file, fout,  adjust, "QCDBin_LowDphi_AccSysScale") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_MuFromTauStat") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_DileptonUncertainty") );
  cc.push_back( adjustUncertainties    (file, fout,  adjust, "QCDBin_LowDphi_TrigEffUncertainty") );
  
  std::cout << cc.size() << std::endl;

  char pdfname[200], tempname[200];
  sprintf(pdfname, "data_formatted_systematics_new.pdf");
  if(!adjust)
      sprintf(pdfname, "data_formatted_systematics_NoChange.pdf");

  for(int ii=0; ii< cc.size(); ii++) {
    if(ii==0) {
      sprintf(tempname, "%s(", pdfname);
      cc[ii]->Print(tempname);
    } else if(ii==cc.size()-1) {
      sprintf(tempname, "%s)", pdfname);
      cc[ii]->Print(tempname);
    } else {
      sprintf(tempname, "%s", pdfname);
      cc[ii]->Print(tempname);
    }
  }

  fout->Close();
  
}

TCanvas* adjustDisplayStyle(TFile *file, TFile *fout, bool adjust, char *hname) {

  std::cout << "Adjusting only the display style of " << hname << " : "  << adjust << std::endl;

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  TH1D *h  = (TH1D*) file->FindObjectAny(hname);

  char cname[200];
  sprintf(cname, "c_%s", hname);
  TCanvas *c = new TCanvas(cname, cname, 900, 700);
  h->Draw();

  TLegend *leg = new TLegend(0.45, 0.8, 0.8, 0.88);
  
  if(hname=="searchBin_StatUncertainties" || hname=="QCDBin_HiDphi_StatUncertainties" || hname=="QCDBin_LowDphi_StatUncertainties"){

  }
  else if(hname=="searchBin_nominal_fullstatuncertainty" || hname=="QCDBin_HiDphi_nominal_fullstatuncertainty" || hname=="QCDBin_LowDphi_nominal_fullstatuncertainty"){
    c->SetLogy(1);
  }
  
  else if(hname=="searchBin_nominal" || hname=="QCDBin_HiDphi_nominal" || hname=="QCDBin_LowDphi_nominal"){
    c->SetLogy(1);
  }
    
  leg->SetHeader(hname);
  leg->SetLineColor(10);
  leg->Draw("same");

  fout->cd(); h->Write();
  return c;
  
}

TCanvas* adjustUncertainties(TFile *file, TFile *fout, bool adjust, char *hname) {

  std::cout << "Adjusting the bin contents of " << hname << " : "  << adjust << std::endl;

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  //sprintf(hname, "%sUp", hname);
  TH1D *h_up  = (TH1D*) file->FindObjectAny(hname);

  char cname[200];
  sprintf(cname, "c_%s", hname);
  TCanvas *c = new TCanvas(cname, cname, 900, 700);
  c->SetLogy(0);
  h_up ->Draw();

  TLegend *leg = new TLegend(0.45, 0.8, 0.8, 0.88);
  h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
  if(hname=="searchBin_IsoTrkVetoEffUncertaintyStat" || hname=="QCDBin_HiDphi_IsoTrkVetoEffUncertaintyStat" || hname=="QCDBin_LowDphi_IsoTrkVetoEffUncertaintyStat" ) {
    h_up->GetYaxis()->SetRangeUser(0.87, 1.2);
    if(adjust) {
      for(int ibin=0; ibin<h_up->GetNbinsX(); ibin++) {
	//if(h_up->GetBinContent(ibin)>1.05 || h_up->GetBinContent(ibin)<0.95)
	//std::cout << hname << " up: ibin " << ibin << " unc " << h_up->GetBinContent(ibin) << std::endl;
	if(h_up->GetBinContent(ibin)>1.07)
	  h_up->SetBinContent(ibin, 1.07);
      }
    }

  } else if(hname=="searchBin_MuFromTauStat" || hname=="QCDBin_HiDphi_MuFromTauStat" || hname=="QCDBin_LowDphi_MuFromTauStat") {
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    if(adjust) {
      for(int ibin=0; ibin<h_up->GetNbinsX(); ibin++) {
	//if(h_up->GetBinContent(ibin)>1.04)
	//std::cout << hname << " up: ibin " << ibin << " unc " << h_up->GetBinContent(ibin) << std::endl;
	if(h_up->GetBinContent(ibin)>1.04)
	  h_up->SetBinContent(ibin, 1.04);
      }
    }
    
  } else if(hname=="searchBin_StatUncertainties" || hname=="QCDBin_HiDphi_StatUncertainties" || hname=="QCDBin_LowDphi_StatUncertainties")
    h_up->GetYaxis()->SetRangeUser(0.2, 4.0);
  
  else if(hname=="searchBin_closureUncertainty" || hname=="QCDBin_HiDphi_closureUncertainty" || hname=="QCDBin_LowDphi_closureUncertainty")
    h_up->GetYaxis()->SetRangeUser(1.0, 2.2);

  else if(hname=="searchBin_closureUncertainty_adhoc" || hname=="QCDBin_HiDphi_closureUncertainty_adhoc" || hname=="QCDBin_LowDphi_closureUncertainty_adhoc")
    h_up->GetYaxis()->SetRangeUser(0.9, 1.4);

  else if(hname=="searchBin_nominal_fullstatuncertainty" || hname=="QCDBin_HiDphi_nominal_fullstatuncertainty" || hname=="QCDBin_LowDphi_nominal_fullstatuncertainty"){
    h_up->GetYaxis()->SetRangeUser(0.01, 10000.0);
    c->SetLogy(1);

  } else if(hname=="searchBin_IsoTrkVetoEffUncertaintySys" || hname=="QCDBin_HiDphi_IsoTrkVetoEffUncertaintySys" || hname=="QCDBin_LowDphi_IsoTrkVetoEffUncertaintySys"){
    h_up->GetYaxis()->SetRangeUser(0.7, 1.3);
    c->SetLogy(1);
  }
    
  leg->SetHeader(hname);
  leg->SetLineColor(10);
  leg->Draw("same");

  fout->cd(); h_up->Write();
  return c;
}

TCanvas* adjustUncertaintiesUpDn(TFile *file, TFile *fout, bool adjust, char *hname) {

  std::cout << "Adjusting the bin contents of " << hname << " : "  << adjust << std::endl;
  
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  
  
  char hname2[200];
  sprintf(hname2, "%sUp", hname);
  TH1D *h_up  = (TH1D*) file->FindObjectAny(hname2);
  sprintf(hname2, "%sDn", hname);
  TH1D *h_dn  = (TH1D*) file->FindObjectAny(hname2);
  h_dn->SetLineColor  (ic_dn);
  h_dn->SetMarkerColor(ic_dn);
  
  char cname[200];
  sprintf(cname, "c_%s", hname);
  TCanvas *c = new TCanvas(cname, cname, 900, 700);
  c->SetLogy(0);
  h_up ->Draw();
  h_dn ->Draw("sames");
  
  TLegend *leg = new TLegend(0.45, 0.8, 0.8, 0.88);
  h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
  h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);
  if(hname == "searchBin_BMistag" || hname == "QCDBin_HiDphi_BMistag" || hname == "QCDBin_LowDphi_BMistag") {
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    //h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    //h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_up->GetYaxis()->SetRangeUser(0.5, 1.5);
    h_dn->GetYaxis()->SetRangeUser(0.5, 1.5);

    if(adjust) {
      for(int ibin=0; ibin<h_up->GetNbinsX(); ibin++) {
	//if(h_up->GetBinContent(ibin)>1.05 || h_up->GetBinContent(ibin)<0.95)
	  //std::cout << hname << " up: ibin " << ibin << " unc " << h_up->GetBinContent(ibin) << std::endl;
	if(hname == "searchBin_BMistag" && (ibin==33 || ibin==151 || ibin==154) )
	  h_up->SetBinContent(ibin, 1.16);
	else if(hname == "QCDBin_HiDphi_BMistag" && (ibin==45 || ibin==199 || ibin==202) )
	  h_up->SetBinContent(ibin, 1.14);
	else if(hname == "QCDBin_LowDphi_BMistag" && (ibin==189 || ibin==199) )
	  h_up->SetBinContent(ibin, 1.27);
      }
      for(int ibin=0; ibin<h_dn->GetNbinsX(); ibin++) {
	//if(h_dn->GetBinContent(ibin)>1.05 || h_dn->GetBinContent(ibin)<0.95)
	//std::cout << hname << " dn: ibin " << ibin << " unc " << h_dn->GetBinContent(ibin) << std::endl;
	if(hname == "searchBin_BMistag" && (ibin==33 || ibin==151 || ibin==154) )
	  h_dn->SetBinContent(ibin, 0.84);
	else if(hname == "QCDBin_HiDphi_BMistag" && (ibin==45 || ibin==199 || ibin==202) )
	  h_dn->SetBinContent(ibin, 0.86);
	else if(hname == "QCDBin_LowDphi_BMistag" && (ibin==189 || ibin==199) )
	  h_dn->SetBinContent(ibin, 0.73);
      }
    }
      
  } else if(hname == "searchBin_MuRecoSys" || hname == "QCDBin_HiDphi_MuRecoSys" || hname == "QCDBin_LowDphi_MuRecoSys") {
    // Uncertainty adjustment: 1% for muon tracking SF systematis, 1% for ICHEP dataset extension
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);

    if(adjust) {
      double err_adjust;
      for(int ibin=0; ibin<h_up->GetNbinsX(); ibin++) {
	double err = h_up->GetBinContent(ibin+1);
	err_adjust = (1-err)*(1-err) + 0.01*0.01 + 0.01*0.01;
	h_up->SetBinContent(ibin+1,1-sqrt(err_adjust));
      }
      for(int ibin=0; ibin<h_dn->GetNbinsX(); ibin++) {
	double err = h_dn->GetBinContent(ibin+1);
	err_adjust = (err-1.)*(err-1.) + 0.01*0.01 + 0.01*0.01;
	h_dn->SetBinContent(ibin+1,sqrt(err_adjust)+1.);
      }
    }
    

  } else if(hname == "searchBin_MuIsoSys" || hname == "searchBin_MuRecoIso") {
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);
    
  } else if(hname == "searchBin_JECSys") {
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);

    if(adjust) {
      for(int ibin=0; ibin<h_up->GetNbinsX(); ibin++) {
	if(h_up->GetBinContent(ibin)>1.07 || h_up->GetBinContent(ibin)<0.93)
	  std::cout << "up: ibin " << ibin << " unc " << h_up->GetBinContent(ibin) << std::endl;
	if(ibin==131 || ibin==151 || ibin==154)
	  h_up->SetBinContent(ibin, 0.93);
      }
      for(int ibin=0; ibin<h_dn->GetNbinsX(); ibin++) {
	if(h_dn->GetBinContent(ibin)>1.07 || h_dn->GetBinContent(ibin)<0.93)
	  std::cout << "dn: ibin " << ibin << " unc " << h_dn->GetBinContent(ibin) << std::endl;
	if(ibin==131 || ibin==151 || ibin==154)
	  h_dn->SetBinContent(ibin, 1.07);
      }
    }

  } else if(hname == "searchBin_MTSys") {
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);
    
  } else if(hname == "searchBin_AccSysPDF" || hname == "QCDBin_HiDphi_AccSysPDF" || hname == "QCDBin_LowDphi_AccSysPDF") {
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);
    if(adjust) {
      for(int ibin=0; ibin<h_up->GetNbinsX(); ibin++) {
	if(h_up->GetBinContent(ibin)>1.05 || h_up->GetBinContent(ibin)<0.95)
	  std::cout << hname << " up: ibin " << ibin << " unc " << h_up->GetBinContent(ibin) << std::endl;
      }
      for(int ibin=0; ibin<h_dn->GetNbinsX(); ibin++) {
	if(h_dn->GetBinContent(ibin)>1.05 || h_dn->GetBinContent(ibin)<0.95)
	  std::cout << hname << " dn: ibin " << ibin << " unc " << h_dn->GetBinContent(ibin) << std::endl;
	if(hname == "searchBin_AccSysPDF" && (ibin==49 || ibin==50 || ibin==59 || ibin==60 || ibin==69 || ibin==70 || ibin==79 || ibin==80) )
	  h_dn->SetBinContent(ibin, 1.05);
	else if(hname == "QCDBin_HiDphi_AccSysPDF" && (ibin==64 || ibin==65 || ibin==77 || ibin==78 || ibin==90 || ibin==91 || ibin==103 || ibin==104) )
	  h_dn->SetBinContent(ibin, 1.05);
      }
    }
    
  } else if(hname == "searchBin_AccSysScale") {
    leg = new TLegend(0.45, 0.8, 0.8, 0.88);
    h_up->GetYaxis()->SetRangeUser(0.87, 1.12);
    h_dn->GetYaxis()->SetRangeUser(0.87, 1.12);
  }
  
  leg->SetHeader(hname);
  leg->SetLineColor(10);
  leg->Draw("same");

  fout->cd(); h_up->Write(); h_dn->Write();
  
  return c;
}
