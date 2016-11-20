#include "LeptonAcceptance.h"
#include "Events.h"
#include "Selection.h"
#include "utils.h"
#include "utils2.h"
#include "TTree.h"
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TVector2.h" 
#include "TVector3.h"

using namespace std;

class histClass{
  double * a;
  TH1D * b_hist;

public:

  void fill(int Nhists, double * eveinfarr_, TH1D * hist_){
    a = eveinfarr_;
    b_hist=hist_;
    (*b_hist).Fill(*a);
    for(int i=1; i<=Nhists ; i++){
      (*(b_hist+i)).Fill(*(a+i),*a);
    }
  }
};


int main(int argc, char *argv[]){
  /////////////////////////////////////
  if (argc != 6)
    {
      std::cout << "Please enter something like: ./run \"filelist_WJets_PU20bx25_100_200.txt\" \"WJets_PU20bx25_100_200\" \"Results\" \"00\" \"0\" " << std::endl;
      return EXIT_FAILURE;
    }
  //get the inputs from user
  const string InRootList = argv[1];
  const string subSampleKey = argv[2];
  const string Outdir = argv[3];
  const string inputnumber = argv[4];
  const string verbosity = argv[5];
  //////////////////////////////////////
  int verbose = atoi(verbosity.c_str());

  //some varaibles
  char filenames[500];
  vector<string> filesVec;
  ifstream fin(InRootList.c_str());
  TChain *sample_AUX = new TChain("TreeMaker2/PreSelection");
  char tempname[200];
  char histname[200];
  vector<TH1D > vec;
  map<int, string> eventType;
  map<string , vector<TH1D> > cut_histvec_map;
  map<string, map<string , vector<TH1D> > > map_map;
  map<string, histClass> histobjmap;
  histClass histObj;

  // The tau response templates
  Utils * utils = new Utils();

  // Introduce two histograms to understand how often a gen tau does not match any jet
  TH1D * GenTau_Jet_all = new TH1D("GenTau_Jet_all","Pt of Gen Tau",utils->NMuPtBins(),0,utils->MaxMuPt());
  GenTau_Jet_all->Sumw2();
  TH1D * GenTau_Jet_fail = new TH1D("GenTau_Jet_fail","Pt of Gen Tau",utils->NMuPtBins(),0,utils->MaxMuPt());
  GenTau_Jet_fail->Sumw2();

  // Two histograms to find rate of btagged(mistagged) tau jets as a function of pT. 
  TH1D * B_rate_all = new TH1D("B_rate_all","Pt of all matched tau jets",utils->NMuPtBins(),0,utils->MaxMuPt());
  B_rate_all->Sumw2();
  TH1D * B_rate_tagged = new TH1D("B_rate_tagged","Pt of bTagged tau jets",utils->NMuPtBins(),0,utils->MaxMuPt());
  B_rate_tagged->Sumw2();

  Double_t ht_bins[15] = {
    0., 100.,200.,300.,400.,500.,600.,700.,800.,900.,
    1000.,1200.,1500.,2000.,5000.};
  Double_t mht_bins[13] = {0., 50.,100.,150.,200.,250.,300.,350.,400.,500.,
			   700.,1000.,5000.};

  //build a vector of histograms
  TH1D weight_hist = TH1D("weight", "Weight Distribution", 5,0,5);
  vec.push_back(weight_hist);
  TH1D RA2HT_hist = TH1D("HT","HT Distribution",50,0,5000);
  RA2HT_hist.Sumw2();
  vec.push_back(RA2HT_hist);
  TH1D RA2HT2_hist = TH1D("HT2","HT2 Distribution",14,ht_bins);
  RA2HT2_hist.Sumw2();
  vec.push_back(RA2HT2_hist);
  TH1D RA2MHT_hist = TH1D("MHT","MHT Distribution",100,0,5000);
  RA2MHT_hist.Sumw2();
  vec.push_back(RA2MHT_hist);
  TH1D RA2MHT2_hist = TH1D("MHT2","MHT Distribution",12,mht_bins);
  RA2MHT2_hist.Sumw2();
  vec.push_back(RA2MHT2_hist);
  TH1D RA2MET_hist = TH1D("MET","MET Distribution",100,0,5000);
  RA2MET_hist.Sumw2();
  vec.push_back(RA2MET_hist);   
  TH1D RA2DelPhi1_hist = TH1D("DelPhi1","DelPhi1 Distribution",50,0,5);
  RA2DelPhi1_hist.Sumw2();
  vec.push_back(RA2DelPhi1_hist);
  TH1D RA2DelPhi2_hist = TH1D("DelPhi2","DelPhi2 Distribution",50,0,5);
  RA2DelPhi2_hist.Sumw2();
  vec.push_back(RA2DelPhi2_hist);
  TH1D RA2DelPhi3_hist = TH1D("DelPhi3","DelPhi3 Distribution",50,0,5);
  RA2DelPhi3_hist.Sumw2();
  vec.push_back(RA2DelPhi3_hist);   
  TH1D RA2DelPhi4_hist = TH1D("DelPhi4","DelPhi4 Distribution",50,0,5);
  RA2DelPhi4_hist.Sumw2();
  vec.push_back(RA2DelPhi4_hist); 
  TH1D RA2NJet_hist = TH1D("NJet","Number of Jets Distribution",20,0,20);
  RA2NJet_hist.Sumw2();
  vec.push_back(RA2NJet_hist);
  TH1D RA2NBtag_hist = TH1D("NBtag","Number of Btag Distribution",20,0,20);
  RA2NBtag_hist.Sumw2();
  vec.push_back(RA2NBtag_hist);
  TH1D TauJetPt_hist = TH1D("TauJetPt","Pt of tau Jet",80,0,400);
  TauJetPt_hist.Sumw2();
  vec.push_back(TauJetPt_hist);
  TH1D TauJetPhi_hist = TH1D("TauJetPhi","Phi of tau Jet",140,-3.5,3.5);
  TauJetPhi_hist.Sumw2();
  vec.push_back(TauJetPhi_hist);
  TH1D TauJet_MHT_delPhi_hist = TH1D("TauJet_MHT_delPhi","dletaPhi of tau Jet vs MHT",350,0,3.5);
  TauJet_MHT_delPhi_hist.Sumw2();
  vec.push_back(TauJet_MHT_delPhi_hist);


  int Nhists=((int)(vec.size())-1);//-1 is because weight shouldn't be counted.

  // Introduce cutflow histogram to monior event yields for early preselection
  TH1D* cutflow_preselection = new TH1D("cutflow_preselection","cutflow_preselectoion",
					11,0.,11.);
  cutflow_preselection->GetXaxis()->SetBinLabel(1,"All Events");
  cutflow_preselection->GetXaxis()->SetBinLabel(2,"Sample based gen-selection");
  cutflow_preselection->GetXaxis()->SetBinLabel(3,"HBHE(Iso)NoiseFilter");
  cutflow_preselection->GetXaxis()->SetBinLabel(4,"eeBadScFilter");
  cutflow_preselection->GetXaxis()->SetBinLabel(5,"CSCTightHalo/EcalDeadCellTriggerPrimitive");
  cutflow_preselection->GetXaxis()->SetBinLabel(6,"GoodVtx");
  cutflow_preselection->GetXaxis()->SetBinLabel(7,"JetID Cleaning");
  cutflow_preselection->GetXaxis()->SetBinLabel(8,"Hadronic tau");
  cutflow_preselection->GetXaxis()->SetBinLabel(9,"Lepton vetoes");
  cutflow_preselection->GetXaxis()->SetBinLabel(10,"Preselection 1");
  cutflow_preselection->GetXaxis()->SetBinLabel(11,"Preselection 2");

  // Introduce search bin histogram
  map<string,int> binMap = utils2::BinMap_NoB();
  int totNbins=binMap.size();
  TH1* searchH = new TH1D("searchH","search bin histogram",totNbins,1,totNbins+1);
  searchH->Sumw2();

  // Introduce QCD histogram
  map<string,int> binMap_QCD = utils2::BinMap_QCD();
  int totNbins_QCD=binMap_QCD.size();
  TH1* QCD_Low = new TH1D("QCD_Low","QCD bin histogram",totNbins_QCD,1,totNbins_QCD+1);
  QCD_Low->Sumw2();
  TH1* QCD_Up = new TH1D("QCD_Up","QCD bin histogram",totNbins_QCD,1,totNbins_QCD+1);
  QCD_Up->Sumw2();
  TH1* searchH_lowDphi = new TH1D("searchH_lowDphi","search bin histogram",totNbins,1,totNbins+1);
  searchH_lowDphi->Sumw2();

  // Introduce search bin histogram with bTag bins
  map<string,int> binMap_b = utils2::BinMap();


  int totNbins_b=binMap_b.size();
  TH1* searchH_b = new TH1D("searchH_b","search bin histogram",totNbins_b,1,totNbins_b+1);

    
  searchH_b->Sumw2();

  // some histograms for our presentations
  map<string,int> binMap_HTMHT = utils2::BinMap_HTMHT();
  int totNbins_HTMHT=binMap_HTMHT.size();
  TH1D* hPredHTMHT0b = new TH1D("hPredHTMHT0b", ";HTMHT Box;Events / Bin", totNbins_HTMHT, 1, totNbins_HTMHT+1);
  hPredHTMHT0b->Sumw2();
  TH1D* hPredHTMHT0b_evt = static_cast<TH1D*>(hPredHTMHT0b->Clone("hPredHTMHT0b_evt"));
  //
  TH1D* hPredHTMHTwb = new TH1D("hPredHTMHTwb", ";HTMHT Box;Events / Bin", totNbins_HTMHT, 1, totNbins_HTMHT+1);
  hPredHTMHTwb->Sumw2();
  TH1D* hPredHTMHTwb_evt = static_cast<TH1D*>(hPredHTMHTwb->Clone("hPredHTMHTwb_evt"));
  //
  Double_t njetbins[4] = {3.5, 6.5, 8.5, 11.5};
  TH1D* hPredNJetBins = new TH1D("hPredNJetBins", ";N_{jets} (p_{T} > 30 GeV);Events / Bin", 3, njetbins);
  hPredNJetBins->Sumw2();
  TH1D* hPredNJetBins_evt = static_cast<TH1D*>(hPredNJetBins->Clone("hPredNJetBins_evt"));
  //
  TH1D* hPredNbBins = new TH1D("hPredNbBins", ";N_{b-jets} (p_{T} > 30 GeV);Events / Bin", 4, -0.5, 3.5);
  hPredNbBins->Sumw2();
  TH1D* hPredNbBins_evt = static_cast<TH1D*>(hPredNbBins->Clone("hPredNbBins_evt"));
  //


  // Introduce a binning for IsoTrks
  map<string,int> binMap_ForIso = utils2::BinMap_ForIso();
  int totNbins_ForIso=binMap_ForIso.size();

  map<string,int> binMap_ForAcc = utils2::BinMap_ForAcc();
  int totNbins_ForAcc=binMap_ForAcc.size();

  // Inroduce two histogram to understand the probability of a muon coming from tau.
  TH1D * hW_mu = new TH1D("hW_mu","mu from W -- search bin",totNbins,1,totNbins+1);
  hW_mu->Sumw2();
  TH1D * hTau_mu = new TH1D("hTau_mu","mu from Tau -- search bin",totNbins,1,totNbins+1);
  hTau_mu->Sumw2();

  // calculate the trigger efficiency 
  TH1D * trig_all = new TH1D("trig_all"," trigger all -- search bin",totNbins,1,totNbins+1);
  trig_all->Sumw2();
  TH1D * trig_pass = new TH1D("trig_pass"," trigger pass -- search bin",totNbins,1,totNbins+1);
  trig_pass->Sumw2();

  // calculate the acceptance
  map<string,int> binMap_mht_nj = utils2::BinMap_mht_nj();
  int totNbins_mht_nj=binMap_mht_nj.size();
  TH1* hAccAll = new TH1D("hAccAll","Acceptance -- All",totNbins_ForAcc,1,totNbins_ForAcc+1);
  TH1* hAccPass = new TH1D("hAccPass","Acceptance -- Pass",totNbins_ForAcc,1,totNbins_ForAcc+1);
  hAccAll->Sumw2();
  hAccPass->Sumw2();
  TH1* hPDFWeights = new TH1D("hPDFWeights","PDFWeights Distribution",10000,0.,100.);
  TH1* hScaleWeights = new TH1D("hScaleWeights","ScaleWeights Distribution",10000,0.,100.);
  TH1* hPDFWeights_lowDphi = new TH1D("hPDFWeights_lowDphi","PDFWeights_lowDphi Distribution",10000,0.,100.);
  TH1* hScaleWeights_lowDphi = new TH1D("hScaleWeights_lowDphi","ScaleWeights_lowDphi Distribution",10000,0.,100.);


  TH1* hAcc_0b_All = new TH1D("hAcc_0b_All","Acceptance -- All",totNbins_ForAcc,1,totNbins_ForAcc+1);
  TH1* hAcc_0b_Pass = new TH1D("hAcc_0b_Pass","Acceptance -- Pass",totNbins_ForAcc,1,totNbins_ForAcc+1);
  hAcc_0b_All->Sumw2();
  hAcc_0b_Pass->Sumw2();
  TH1* hAcc_non0b_All = new TH1D("hAcc_non0b_All","Acceptance -- All",totNbins_ForAcc,1,totNbins_ForAcc+1);
  TH1* hAcc_non0b_Pass = new TH1D("hAcc_non0b_Pass","Acceptance -- Pass",totNbins_ForAcc,1,totNbins_ForAcc+1);
  hAcc_non0b_All->Sumw2();
  hAcc_non0b_Pass->Sumw2();

  TH1* hAccAll_lowDphi = new TH1D("hAccAll_lowDphi","Acceptance -- All",totNbins_ForAcc,1,totNbins_ForAcc+1);
  TH1* hAccPass_lowDphi = new TH1D("hAccPass_lowDphi","Acceptance -- Pass",totNbins_ForAcc,1,totNbins_ForAcc+1);
  hAccAll_lowDphi->Sumw2();
  hAccPass_lowDphi->Sumw2();

  //-----
  TH1* hAcc4DAll = new TH1D("hAcc4DAll","Acceptance -- All -- Nj-Nb-HTMHT",totNbins_QCD,1,totNbins_QCD+1);
  TH1* hAcc4DPass = new TH1D("hAcc4DPass","Acceptance -- Pass -- Nj-Nb-HTMHT",totNbins_QCD,1,totNbins_QCD+1);
  hAcc4DAll->Sumw2();
  hAcc4DPass->Sumw2();

  TH1* hAcc4DAll_lowDphi = new TH1D("hAcc4DAll_lowDphi","Acceptance -- All -- Nj-Nb-HTMHT",totNbins_QCD,1,totNbins_QCD+1);
  TH1* hAcc4DPass_lowDphi = new TH1D("hAcc4DPass_lowDphi","Acceptance -- Pass -- Nj-Nb-HTMHT",totNbins_ForAcc,1,totNbins_ForAcc+1);
  hAcc4DAll_lowDphi->Sumw2();
  hAcc4DPass_lowDphi->Sumw2();

  // calculate isotrack veto efficiencies
  TH1* IsoElec_all = new TH1D("IsoElec_all","Isolated electron efficiency -- all ",totNbins,1,totNbins+1);
  IsoElec_all->Sumw2();
  TH1* IsoElec_pass = new TH1D("IsoElec_pass","Isolated electron efficiency -- pass ",totNbins,1,totNbins+1);
  IsoElec_pass->Sumw2();

  TH1* IsoMu_all = new TH1D("IsoMu_all","Isolated muon efficiency -- all ",totNbins,1,totNbins+1);
  IsoMu_all->Sumw2();
  TH1* IsoMu_pass = new TH1D("IsoMu_pass","Isolated muon efficiency -- pass ",totNbins,1,totNbins+1);
  IsoMu_pass->Sumw2();

  TH1* IsoPion_all = new TH1D("IsoPion_all","Isolated pion efficiency -- all ",totNbins,1,totNbins+1);
  IsoPion_all->Sumw2();
  TH1* IsoPion_pass = new TH1D("IsoPion_pass","Isolated pion efficiency -- pass ",totNbins,1,totNbins+1);
  IsoPion_pass->Sumw2();

  TH1* Iso_all = new TH1D("Iso_all","Isolated Trk efficiency -- all ",totNbins_ForAcc,1,totNbins_ForAcc+1);
  Iso_all->Sumw2();
  TH1* Iso_pass = new TH1D("Iso_pass","Isolated Trk efficiency -- pass ",totNbins_ForAcc,1,totNbins_ForAcc+1);
  Iso_pass->Sumw2();

  TH1* Iso_all_lowDphi = new TH1D("Iso_all_lowDphi","Isolated Trk efficiency -- all ",totNbins_ForAcc,1,totNbins_ForAcc+1);
  Iso_all_lowDphi->Sumw2();
  TH1* Iso_pass_lowDphi = new TH1D("Iso_pass_lowDphi","Isolated Trk efficiency -- pass ",totNbins_ForAcc,1,totNbins_ForAcc+1);
  Iso_pass_lowDphi->Sumw2();

  TH1* Iso_all2 = new TH1D("Iso_all2","Isolated Trk efficiency for leading tau jet -- all ",totNbins,1,totNbins+1);
  Iso_all2->Sumw2();
  TH1* Iso_pass2 = new TH1D("Iso_pass2","Isolated Trk efficiency for leading tau jet -- pass ",totNbins,1,totNbins+1);
  Iso_pass2->Sumw2();

  // isotrack veto efficiency nb dependence
  /*
    TH1* Iso_all_nb = new TH1D("Iso_all_nb","Isolated Trk efficiency -- all nb",4,-0.5,3.5);
    Iso_all_nb->Sumw2();
    TH1* Iso_pass_nb = new TH1D("Iso_pass_nb","Isolated Trk efficiency -- pass nb",4,-0.5,3.5);
    Iso_pass_nb->Sumw2();

    TH1* Iso_all_nb_lowDphi = new TH1D("Iso_all_nb_lowDphi","Isolated Trk efficiency -- all nb_lowDphi",4,-0.5,3.5);
    Iso_all_nb_lowDphi->Sumw2();
    TH1* Iso_pass_nb_lowDphi = new TH1D("Iso_pass_nb_lowDphi","Isolated Trk efficiency -- pass nb_lowDphi",4,-0.5,3.5);
    Iso_pass_nb_lowDphi->Sumw2();
  */
  TH1* Iso_all_nb_njet2 = new TH1D("Iso_all_nb_njet2","Isolated Trk efficiency -- all nb_njet2",4,-0.5,3.5);
  Iso_all_nb_njet2->Sumw2();
  TH1* Iso_pass_nb_njet2 = new TH1D("Iso_pass_nb_njet2","Isolated Trk efficiency -- pass nb_njet2",4,-0.5,3.5);
  Iso_pass_nb_njet2->Sumw2();

  TH1* Iso_all_nb_njet34 = new TH1D("Iso_all_nb_njet34","Isolated Trk efficiency -- all nb_njet34",4,-0.5,3.5);
  Iso_all_nb_njet34->Sumw2();
  TH1* Iso_pass_nb_njet34 = new TH1D("Iso_pass_nb_njet34","Isolated Trk efficiency -- pass nb_njet34",4,-0.5,3.5);
  Iso_pass_nb_njet34->Sumw2();

  TH1* Iso_all_nb_njet56 = new TH1D("Iso_all_nb_njet56","Isolated Trk efficiency -- all nb_njet56",4,-0.5,3.5);
  Iso_all_nb_njet56->Sumw2();
  TH1* Iso_pass_nb_njet56 = new TH1D("Iso_pass_nb_njet56","Isolated Trk efficiency -- pass nb_njet56",4,-0.5,3.5);
  Iso_pass_nb_njet56->Sumw2();

  TH1* Iso_all_nb_njet78 = new TH1D("Iso_all_nb_njet78","Isolated Trk efficiency -- all nb_njet78",4,-0.5,3.5);
  Iso_all_nb_njet78->Sumw2();
  TH1* Iso_pass_nb_njet78 = new TH1D("Iso_pass_nb_njet78","Isolated Trk efficiency -- pass nb_njet78",4,-0.5,3.5);
  Iso_pass_nb_njet78->Sumw2();

  TH1* Iso_all_nb_njet9 = new TH1D("Iso_all_nb_njet9","Isolated Trk efficiency -- all nb_njet9",4,-0.5,3.5);
  Iso_all_nb_njet9->Sumw2();
  TH1* Iso_pass_nb_njet9 = new TH1D("Iso_pass_nb_njet9","Isolated Trk efficiency -- pass nb_njet9",4,-0.5,3.5);
  Iso_pass_nb_njet9->Sumw2();
        
  //
  TH1* Iso_all_nb_njet2_lowDphi = new TH1D("Iso_all_nb_njet2_lowDphi","Isolated Trk efficiency -- all nb_njet2_lowDphi",4,-0.5,3.5);
  Iso_all_nb_njet2_lowDphi->Sumw2();
  TH1* Iso_pass_nb_njet2_lowDphi = new TH1D("Iso_pass_nb_njet2_lowDphi","Isolated Trk efficiency -- pass nb_njet2_lowDphi",4,-0.5,3.5);
  Iso_pass_nb_njet2_lowDphi->Sumw2();


  TH1* Iso_all_nb_njet34_lowDphi = new TH1D("Iso_all_nb_njet34_lowDphi","Isolated Trk efficiency -- all nb_njet34_lowDphi",4,-0.5,3.5);
  Iso_all_nb_njet34_lowDphi->Sumw2();
  TH1* Iso_pass_nb_njet34_lowDphi = new TH1D("Iso_pass_nb_njet34_lowDphi","Isolated Trk efficiency -- pass nb_njet34_lowDphi",4,-0.5,3.5);
  Iso_pass_nb_njet34_lowDphi->Sumw2();

  TH1* Iso_all_nb_njet56_lowDphi = new TH1D("Iso_all_nb_njet56_lowDphi","Isolated Trk efficiency -- all nb_njet56_lowDphi",4,-0.5,3.5);
  Iso_all_nb_njet56_lowDphi->Sumw2();
  TH1* Iso_pass_nb_njet56_lowDphi = new TH1D("Iso_pass_nb_njet56_lowDphi","Isolated Trk efficiency -- pass nb_njet56_lowDphi",4,-0.5,3.5);
  Iso_pass_nb_njet56_lowDphi->Sumw2();

  TH1* Iso_all_nb_njet78_lowDphi = new TH1D("Iso_all_nb_njet78_lowDphi","Isolated Trk efficiency -- all nb_njet78_lowDphi",4,-0.5,3.5);
  Iso_all_nb_njet78_lowDphi->Sumw2();
  TH1* Iso_pass_nb_njet78_lowDphi = new TH1D("Iso_pass_nb_njet78_lowDphi","Isolated Trk efficiency -- pass nb_njet78_lowDphi",4,-0.5,3.5);
  Iso_pass_nb_njet78_lowDphi->Sumw2();

  TH1* Iso_all_nb_njet9_lowDphi = new TH1D("Iso_all_nb_njet9_lowDphi","Isolated Trk efficiency -- all nb_njet9_lowDphi",4,-0.5,3.5);
  Iso_all_nb_njet9_lowDphi->Sumw2();
  TH1* Iso_pass_nb_njet9_lowDphi = new TH1D("Iso_pass_nb_njet9_lowDphi","Isolated Trk efficiency -- pass nb_njet9_lowDphi",4,-0.5,3.5);
  Iso_pass_nb_njet9_lowDphi->Sumw2();
        
  // They are filled for different bins in generated tau-lepton pt.
  std::vector<TH1*> hTauResp(utils->TauResponse_nBins_());
  std::vector<TH1*> hTauRespUp(utils->TauResponse_nBins_());
  std::vector<TH1*> hTauRespDown(utils->TauResponse_nBins_());
  std::vector<TH1*> hTauResp_x(utils->TauResponse_nBins_());
  std::vector<TH1*> hTauResp_y(utils->TauResponse_nBins_());
  std::vector<TH2*> hTauResp_xy(utils->TauResponse_nBins_());
  //*AR-Sep27,2016-This vector was added in relation with comparison with Wgun templates
  std::vector<TH1*> hGenPt(utils->TauResponse_nBins_());
  double genXmin=0.;
  double genXmax=0.;
  int genBins=0;
  for(unsigned int i = 0; i < utils->TauResponse_nBins_(); ++i){
    hTauResp.at(i) = new TH1D(utils->TauResponse_name(i),";p_{T}(visible) / p_{T}(generated-#tau);Probability",50,0.,2.5);
    hTauRespUp.at(i) = new TH1D(utils->TauResponse_name(i)+"_Up",";p_{T}(visible) / p_{T}(generated-#tau);Probability",50,0.,2.5);
    hTauRespDown.at(i) = new TH1D(utils->TauResponse_name(i)+"_Down",";p_{T}(visible) / p_{T}(generated-#tau);Probability",50,0.,2.5);
    hTauResp.at(i)->Sumw2();
    hTauResp_x.at(i) = new TH1D(utils->TauResponse_name(i)+"_x",";p_{T}(visible)_x / p_{T}(generated-#tau);Probability",50,0.,2.5);
    hTauResp_x.at(i)->Sumw2();
    hTauResp_y.at(i) = new TH1D(utils->TauResponse_name(i)+"_y",";p_{T}(visible)_y / p_{T}(generated-#tau);Probability",50,-2.5,2.5);
    hTauResp_y.at(i)->Sumw2();
    hTauResp_xy.at(i) = new TH2D(utils->TauResponse_name(i)+"_xy",";p_{T}(visible)_x / p_{T}(generated-#tau);p_{T}(visible)_y / p_{T}(generated-#tau)",50,0.,2.5,40,-1.,1.);
    hTauResp_xy.at(i)->Sumw2();
    if(i==0){
      genXmin=20.;genXmax=30.;genBins=10;
    }
    if(i==1){
      genXmin=30.;genXmax=50.;genBins=20;
    }
    if(i==2){
      genXmin=50.;genXmax=100.;genBins=50;
    }
    if(i==3){
      genXmin=100.;genXmax=400.;genBins=300;
    }
    hGenPt.at(i) = new TH1D(utils->TauPt_name(i),";p_{T}(generated-#tau);Probability",genBins,genXmin,genXmax);
  }

  TH1D * tau_GenPt = new TH1D("hGenPt","gen tau pt distribution",400,0,400);

  // a template for phi of the tau jets
  TH2D * tau_GenJetPhi = new TH2D("tau_GenJetPhi","DPhi between gen and jet tau vs. their energy ratio",utils->tau_Phi_nbinX(),utils->tau_Phi_lowX(),utils->tau_Phi_upX(),utils->tau_Phi_nbinY(),utils->tau_Phi_lowY(),utils->tau_Phi_upY());
      
  // We would like also to have the pt distribution of the tau Jets
  TH1D * tauJetPtHist = new TH1D("tauJetPtHist","Pt of the tau hadronic jets",80,0,400);

  // Because of bad reconstruction or so, sometimes no jet matches a Gen. hadronic tau. 
  // So we need to add into account the fail rate here. 
  // First open the fail rate histogram
  /*
    sprintf(tempname,"TauHad/FailRate_GenTau_jet_TTbar_%s.root",inputnumber.c_str());
    TFile * FailRateGenTau_Jet_file = new TFile(tempname,"R");
    sprintf(histname,"FailRate_GenTau_jet");
    TH1D * hFailRate_GenTau_Jet =(TH1D *) FailRateGenTau_Jet_file->Get(histname)->Clone();
  */

  // get pileup for MC
  //TFile * pileupFile = new TFile("TauHad2/pu_weights_7_4_25ns_v2.root","R");
  //TH1D * hPURatio = (TH1D*)pileupFile->Get("ratio");

  ///read the file names from the .txt files and load them to a vector.
  while(fin.getline(filenames, 500) ){filesVec.push_back(filenames);}
  cout<< "\nProcessing " << subSampleKey << " ... " << endl;
  for(unsigned int in=0; in<filesVec.size(); in++){ sample_AUX->Add(filesVec.at(in).c_str()); }


  // --- Analyse the events --------------------------------------------

  // Interface to the event content
  Events * evt = new Events(sample_AUX, subSampleKey,verbose);

  // to calculate the acceptance systematics. Here evt->PDFweights_()[0] is the pdf for the nominal acc
  // we want to calculate all the Acc correspondig with the 100 different pdf and take the RMS as systematic
  bool CalcAccSys = true;
  // define some vector of histograms corresponidng with different pdfs
  vector<TH1*> hAccAllVec, hAccPassVec, hAccAll_lowDphiVec, hAccPass_lowDphiVec;
  TH1* hSumofSquareOfDev = static_cast<TH1*>(hAccAll->Clone("hSumofSquareOfDev"));
  TH1* hSumofSquareOfDev_lowDphi = static_cast<TH1*>(hAccAll->Clone("hSumofSquareOfDev_lowDphi"));
  // this is the systematic uncertainty at the end
  TH1* hAccSysMax = static_cast<TH1*>(hAccAll->Clone("hAccSysMax"));
  TH1* hAccSysMax_lowDphi = static_cast<TH1*>(hAccAll_lowDphi->Clone("hAccSysMax_lowDphi"));
  TH1* hAccSysMin = static_cast<TH1*>(hAccAll->Clone("hAccSysMin"));
  TH1* hAccSysMin_lowDphi = static_cast<TH1*>(hAccAll_lowDphi->Clone("hAccSysMin_lowDphi"));
  //
  // scale uncertainty 
  // define some vector of histograms corresponidng with different pdfs
  vector<TH1*> hScaleAccAllVec, hScaleAccPassVec, hScaleAccAll_lowDphiVec, hScaleAccPass_lowDphiVec;
  // this is the systematic uncertainty at the end
  TH1* hScaleAccSysMax = static_cast<TH1*>(hAccAll->Clone("hScaleAccSysMax"));
  TH1* hScaleAccSysMax_lowDphi = static_cast<TH1*>(hAccAll_lowDphi->Clone("hScaleAccSysMax_lowDphi"));
  TH1* hScaleAccSysMin = static_cast<TH1*>(hAccAll->Clone("hScaleAccSysMin"));
  TH1* hScaleAccSysMin_lowDphi = static_cast<TH1*>(hAccAll_lowDphi->Clone("hScaleAccSysMin_lowDphi"));

  // number of pdfs. There is a check to make sure the number is correct. 
  int PDFsize=101;
  int Scalesize=9;
  // initialize the vectors of histograms. So the binning is the same as nominal Acc
  for(int iacc=0; iacc < PDFsize; iacc++){
    sprintf(tempname,"hAccAll_%d",iacc);
    hAccAllVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
    sprintf(tempname,"hAccPass_%d",iacc);
    hAccPassVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
    sprintf(tempname,"hAccAll_lowDphi_%d",iacc);
    hAccAll_lowDphiVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
    sprintf(tempname,"hAccPass_lowDphi_%d",iacc);
    hAccPass_lowDphiVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
  }
  for(int iacc=0; iacc < Scalesize; iacc++){
    sprintf(tempname,"hScaleAccAll_%d",iacc);
    hScaleAccAllVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
    sprintf(tempname,"hScaleAccPass_%d",iacc);
    hScaleAccPassVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
    sprintf(tempname,"hScaleAccAll_lowDphi_%d",iacc);
    hScaleAccAll_lowDphiVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
    sprintf(tempname,"hScaleAccPass_lowDphi_%d",iacc);
    hScaleAccPass_lowDphiVec.push_back(static_cast<TH1*>(hAccAll->Clone(tempname)));
  }

  // This code is to run only on MC
  if(evt->DataBool_()==true){
    cout << "Turn off the DataBool in Events.cpp \n ";
    return 2;
  }


  // Get a pointer to the Selection class
  Selection * sel = new Selection();

  // applyIsoTrk should be on/off in both places 
  if(sel->applyIsoTrk_()!=utils2::applyIsoTrk){
    cout << " \n\n\n\n Make sure IsoTrk is on/off bothe in utils2 and Selection \n\n\n\n" ;
    return 2;
  }

  // For each selection, cut, make a vector containing the same histograms as those in vec
  //std::cout<<"cutName_size"<<cutName().size()<<endl;

  for(int i=0; i<(int) sel->cutName().size();i++){
    cut_histvec_map[sel->cutName()[i]]=vec;
  }

  // Define different event categories
  eventType[0]="allEvents";

  //initialize a map between string and maps. copy the map of histvecs into each
  for(int i=0; i< eventType.size();i++){
    map_map[eventType[i]]=cut_histvec_map;
  }
  
  //initialize histobjmap
  for(map<string , vector<TH1D> >::iterator it=cut_histvec_map.begin(); it!=cut_histvec_map.end();it++){
    histobjmap[it->first]=histObj;
  }
    
  // count # events at different stages
  int nCleanEve=0,nHadTauEve=0,nNoLepEve=0;    
  int dR_lt_Pt4=0;
  vector<double> HadTauPtVec;
  vector<double> HadTauEtaVec;
  vector<double> HadTauPhiVec;

  // Determine which model to work with
  int TauHadModel=utils2::TauHadModel;


  int sampletype=-1;
  if(subSampleKey.find("TTbar_Inclusive")!=string::npos)sampletype=0; //TTbar_Inclusive
  else if(subSampleKey.find("TTbar_Tbar_SingleLep")!=string::npos || subSampleKey.find("TTbar_T_SingleLep")!=string::npos)sampletype=1;
  else if(subSampleKey.find("TTbar_DiLept")!=string::npos)sampletype=2;
  else if(subSampleKey.find("TTbar_HT_600_800")!=string::npos)sampletype=3;
  else if(subSampleKey.find("TTbar_HT_800_1200")!=string::npos)sampletype=4;
  else if(subSampleKey.find("TTbar_HT_1200_2500")!=string::npos)sampletype=5;
  else if(subSampleKey.find("TTbar_HT_2500_Inf")!=string::npos)sampletype=6;
  else if(subSampleKey.find("TTbar")!=string::npos){
    cout << " TT sample is not known. Please check the second input \n " ;
    return 2;
  }


  // Loop over the events (tree entries)
  double eventWeight = 1.0;
  int eventN=0;
  while( evt->loadNext() ){
    eventN++;
    //      std::cout<<" evt_PDFweights_size "<<evt->PDFweights_()->size()<<" PDFsize "<<PDFsize<<endl;
    if(evt->PDFweights_()->size()!= PDFsize){
      cout << " PDFweights_()->size(): " << evt->PDFweights_()->size() << endl;
      cout << " Please fix the value of the variable \"PDFsize\". \n Turning off the AccSys calculation \n " ;
      CalcAccSys = false;
    }
    // std::cout<<" evt_Scaleweights_size "<<evt->ScaleWeights_()->size()<<" Scalesize "<<Scalesize<<endl;
    if(evt->ScaleWeights_()->size()!=Scalesize){
      cout << " ScaleWeights_()->size(): " << evt->ScaleWeights_()->size() << endl;
      cout << " Please fix the value of the variable \"Scalesize\". \n Turning off the AccSys calculation \n " ;
      CalcAccSys = false;
    }

    if(eventN>50000)break;
    //if(eventN>5000)break;
    std::cout<<"eventN "<<eventN <<" *****************************First point************************ "<<endl;
     
    eventWeight = evt->weight();
    //std::cout<<" eventN "<<eventN<<" eventWeight "<<eventWeight<<endl;
    //eventWeight = evt->weight()/evt->puweight();
    //if(subSampleKey.find("TTbar_Tbar_SingleLep")!=string::npos)eventWeight = 2.984e-06;
    //if(subSampleKey.find("TTbar_DiLept")!=string::npos)eventWeight = 2.84141e-06;

    cutflow_preselection->Fill(0.,eventWeight); // keep track of all events processed

    if(!evt->DataBool_()){

      if(sampletype==0){
	if(evt->gen_ht()>600||evt->GenElecPtVec_().size()>0||evt->GenMuPtVec_().size()>0||evt->GenTauPtVec_().size()>0)continue;
      }

      if(sampletype==1){
	if(evt->gen_ht()>600)continue;
      }

      if(sampletype==2){
	if(evt->gen_ht()>600)continue;
      }

    }
    //std::cout<<" eventN "<<eventN<<" *****************************Outside************************ "<<endl;

    cutflow_preselection->Fill(1.,eventWeight);
    if(evt->HBHEIsoNoiseFilter_()==0)continue;
    if(evt->HBHENoiseFilter_()==0)continue;
    cutflow_preselection->Fill(2.,eventWeight);
    if(evt->eeBadScFilter_()==0)continue;
    cutflow_preselection->Fill(3.,eventWeight);
    //if(evt->CSCTightHaloFilter_()==0)continue;
    if(evt->EcalDeadCellTriggerPrimitiveFilter_()==0)continue;
      
    if( evt->DataBool_() && evt->BadChargedCandidateFilter_()==0) continue;
    if( evt->DataBool_() && evt->BadPFMuonFilter_()==0) continue;
    if( evt->DataBool_() && evt->globalTightHalo2016Filter_()==0) continue;
    if( evt->PFCaloMETRatioFilter_()==0) continue;
    if( evt->noMuonJet_()==0) continue;
    //if( !evt->DataBool_() && fastsim && evt->noFakeJet_()==0) continue;
     
    cutflow_preselection->Fill(4.,eventWeight);
    if(!(evt->NVtx_() >0))continue;
    cutflow_preselection->Fill(5.,eventWeight);
    // Through out an event that contains HTjets with bad id
    if(evt->JetId()==0)continue;
    cutflow_preselection->Fill(6.,eventWeight); // events passing JetID event cleaning
      
    nCleanEve++;


    // Trigger check
    bool trigPass=false;
    /*
      string triggerNameToBeUsed = "HLT_Mu15_IsoVVVL_PFHT350_PFMET70_v1";
      if (!evt->DataBool_()) triggerNameToBeUsed = "HLT_Mu15_IsoVVVL_PFHT400_PFMET70_v1";
      bool trigfound=false;
      if(verbose!=0)
      cout << "############################\n TrigSize: " << evt->TriggerNames_().size() << "PassTrigSize: " << evt->PassTrigger_().size() << endl ;
      for(int i=0; i< evt->TriggerNames_().size(); i++){
      if(verbose!=0){
      cout << evt->TriggerNames_().at(i) << endl;
      cout << " Pass: " << evt->PassTrigger_().at(i) << " \n+\n";
      }
      if( evt->TriggerNames_().at(i).find(triggerNameToBeUsed) != string::npos ){
      trigfound=true;
      if(evt->PassTrigger_().at(i))trigPass=true;
      }
      }
      if(!trigfound){
      cout << " ####\n ####\n trigger was not found \n ####\n " ;
      }
    */

    // Here we determine how often the /<<<< W --> tau --> W --> mu >>>>/
    // comparing with /<<<< w--> mu >>>>/
    // First we restrict ourselves to events with one muon and no electron
    // just like the control sample in the data driven method.
    // Introduce two histograms of Pt distributions. One to be filled in
    // any case when one muon and no electron exist. The other, when muon
    // is coming from tau.
    // At the end dividing the two histograms give the percentage as a function
    // transverse momentum.
    int muN=evt->GenMuPtVec_().size();
      
    // Since we don't acount for efficiency and acceptance of Reco. electrons,
    // for the sake of consistency, apply the same pT and eta cuts on the gen.
    // electrons as those applied to Reco. electrons.  
    int eleN=0;
    for(int i=0;i<evt->GenElecPtVec_().size();i++){
      double elept=evt->GenElecPtVec_()[i];
      double eleeta=evt->GenElecEtaVec_()[i];
      // Ahmad33        if(elept > LeptonAcceptance::electronPtMin() && std::abs(eleeta) < LeptonAcceptance::electronEtaMax())eleN++;
      eleN++; // Ahmad33
    }

    double muPt=-99;

    //we want to consider events that pass the baseline cuts
    if( sel->ht_500(evt->ht()) && sel->mht_200(evt->mht()) && sel->Njet_4(evt->nJets()) ){

      if(verbose!=0)printf("============================================================= \n eventN: %d \n ",eventN);
      if(verbose!=0 && eleN==0 && muN==1)printf("#####################\nNo elec and 1 muon event \n eventN: %d \n ",eventN);
      if(muN==1)muPt=evt->GenMuPtVec_().at(0);
    
      // If no elec and 1 muon
      // Fill the hW_mu anyways.
      // See what is the parent of the mu. if tau fill the tau hist.
      // If w, see where w is coming from, if tau again, fill the tau hist.
      if( eleN==0 && muN==1 )hW_mu->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);

      bool isTau_mu=false;

      if(muN==1){ 
	if(evt->GenMuFromTauVec_()[0]==1)isTau_mu=true;
      }
        
      if( eleN==0 && muN==1 && isTau_mu==true )hTau_mu->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
        
    } // end of baseline cuts

      
    //for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
    //if(evt->PDFweights_()->at(iacc)>1000.)
    //	std::cout<<" eventN "<<eventN<<" iacc "<<iacc<<" PDF_wt "<< evt->PDFweights_()->at(iacc)<<endl;
    //}

      // We are interested in hadronically decaying taus only
    bool hadTau=false; 
    double genTauPt=-1.;
    double genTauEta=-99.;
    double genTauPhi=-99.;
    int NuIndex = -1;
    HadTauPtVec.clear();
    HadTauEtaVec.clear();
    HadTauPhiVec.clear();
     
    if(TauHadModel>=4){
      //std::cout << evt->GenTauHadVec_().size() << std::endl;
      for(int i=0; i<evt->GenTauHadVec_().size();i++){
	if(evt->GenTauHadVec_()[i]==1){
	  double pt=evt->GenTauPtVec_()[i];
	  double eta=evt->GenTauEtaVec_()[i];
	  double phi=evt->GenTauPhiVec_()[i];
	  HadTauPtVec.push_back(pt);
	  HadTauEtaVec.push_back(eta);
	  HadTauPhiVec.push_back(phi);
	  if(pt > genTauPt){
	    genTauPt = pt;
	    NuIndex = i;
	    genTauEta = eta;
	    genTauPhi = phi;
	  }
	}
      }
    }
    else{
      for(int i=0; i<evt->GenTauHadVec_().size();i++){
	if(evt->GenTauHadVec_()[i]==1){
	  double pt=evt->GenTauPtVec_()[i];
	  double eta=evt->GenTauEtaVec_()[i];
	  double phi=evt->GenTauPhiVec_()[i];
	  HadTauPtVec.push_back(pt);
	  HadTauEtaVec.push_back(eta);
	  HadTauPhiVec.push_back(phi);
	  if(pt > genTauPt && fabs(eta)<LeptonAcceptance::muonEtaMax()){ //Ahmad33
	    genTauPt = pt;
	    NuIndex = i;
	    genTauEta = eta;
	    genTauPhi = phi;
	  }
	}
      }
    }
    if(HadTauPtVec.size()>0)hadTau=true;

    //std::cout << "eventN:1 " << eventN << std::endl;

    if(verbose!=0){      
      for(int i=0; i<evt->GenTauPtVec_().size();i++){
	printf("tau: \n ##### \n idx: %d pt: %g \n NU: \n #### \n idx: %d pt: %g \n ",i,evt->GenTauPtVec_()[i],i,evt->GenTauNuPtVec_()[i]);
      }
      printf("NuIndex: %d HadTau: %d genTauPt: %g \n ============================ \n ",NuIndex,hadTau,genTauPt); 
    }

    if(hadTau==false)continue;
    cutflow_preselection->Fill(7.,eventWeight); // hadronic tau events
    nHadTauEve++;
 
    // We want no muon and electron in the event
    if(evt->GenMuPtVec_().size()!=0 || eleN!=0)continue;
    cutflow_preselection->Fill(8.,eventWeight); // events passing lepton vetos

    nNoLepEve++;

    if(verbose!=0){
      printf(" ####################### \n event#: %d \n ",eventN-1); 
      
      printf(" @@@@\n Jets section: \n ");
      for(int i=0;i<evt->slimJetPtVec_().size();i++){
	printf("jet#: %d pt: %g eta: %g phi: %g \n ",i+1,evt->slimJetPtVec_()[i],evt->slimJetEtaVec_()[i],evt->slimJetPhiVec_()[i]);
      }
      printf(" @@@@\n Tau section: \n ");
      for(int i=0;i<evt->GenTauPtVec_().size();i++){
	printf("GenTau#: %d pt: %g eta: %g phi: %g \n ",i+1,evt->GenTauPtVec_()[i],evt->GenTauEtaVec_()[i],evt->GenTauPhiVec_()[i]);
      }
      printf(" @@@@\n TauNu section: \n ");
      for(int i=0;i<evt->GenTauNuPtVec_().size();i++){
	printf("GenTauNu # : %d pt: %g eta: %g phi: %g \n ",i+1,evt->GenTauNuPtVec_()[i],evt->GenTauNuEtaVec_()[i],evt->GenTauNuPhiVec_()[i]);
      }
      printf(" @@@@\n Mom of TauNu section: \n ");
    }
    int jet_index=-1;
    int nB = evt->nBtags();
    double deltaR = 0.4;
    // We don't write the event for nB if the matched tau jet is btaged. 
    if(utils->findMatchedObject(jet_index,genTauEta,genTauPhi, evt->JetsPtVec_(),evt->JetsEtaVec_(),evt->JetsPhiVec_(),deltaR,verbose)){
      B_rate_all->Fill(evt->JetsPtVec_()[jet_index],eventWeight);
      if(evt->csvVec()[jet_index]>evt->csv_()){
	nB=-1;
	B_rate_tagged->Fill(evt->JetsPtVec_()[jet_index],eventWeight);
      }
    }

      
    // Acceptance determination 1: Counter for all events
    // with muons at generator level
    // apply the baseline selection
    if(sel->nolep(evt->nLeptons())&&sel->Njet_4(evt->nJets())&&sel->ht_500(evt->ht())
       &&sel->mht_200(evt->mht())&&sel->dphi(evt->nJets(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->deltaPhi4())
       ){
      hAccAll->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight); // the weight has only scaling info.needed for stacking 
      if(evt->nBtags()==0)hAcc_0b_All->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
      if(evt->nBtags()>0)hAcc_non0b_All->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
      //std::cout<<"CalcAccSys "<<CalcAccSys<<endl;
      if(CalcAccSys){
        for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
	  hPDFWeights->Fill(evt->PDFweights_()->at(iacc));          
	  hAccAllVec.at(iacc)->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->PDFweights_()->at(iacc));
        }
        for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
	  hScaleWeights->Fill(evt->ScaleWeights_()->at(iacc));
          hScaleAccAllVec.at(iacc)->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->ScaleWeights_()->at(iacc));
        }
      }
      if( genTauPt > LeptonAcceptance::muonPtMin() && std::abs(genTauEta) < LeptonAcceptance::muonEtaMax() ){
	hAccPass->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
	if(evt->nBtags()==0)hAcc_0b_Pass->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
	if(evt->nBtags()>0)hAcc_non0b_Pass->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
	if(CalcAccSys){
          for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
	    hAccPassVec[iacc]->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->PDFweights_()->at(iacc));
          }
          for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
            hScaleAccPassVec[iacc]->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->ScaleWeights_()->at(iacc));
          }
	}
      } 
    }
    // Acceptance for low_Dphi region
    if(sel->nolep(evt->nLeptons())&&sel->Njet_4(evt->nJets())&&sel->ht_500(evt->ht())
       &&sel->mht_200(evt->mht())&& !(sel->dphi(evt->nJets(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->deltaPhi4()))
       ){
      hAccAll_lowDphi->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
      if(CalcAccSys){
        for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
	  hPDFWeights_lowDphi->Fill(evt->PDFweights_()->at(iacc));
          hAccAll_lowDphiVec[iacc]->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->PDFweights_()->at(iacc));
        }
        for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
	  hScaleWeights_lowDphi->Fill(evt->ScaleWeights_()->at(iacc));
	  hScaleAccAll_lowDphiVec[iacc]->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->ScaleWeights_()->at(iacc));
        }
      }
      if( genTauPt > LeptonAcceptance::muonPtMin() && std::abs(genTauEta) < LeptonAcceptance::muonEtaMax() ){
	hAccPass_lowDphi->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight);
	if(CalcAccSys){
	  for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
	    hAccPass_lowDphiVec[iacc]->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->PDFweights_()->at(iacc));
	  }
	  for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
	    hScaleAccPass_lowDphiVec[iacc]->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()] ,eventWeight*evt->ScaleWeights_()->at(iacc));
	  }
	}
      }
    }      
      
    //std::cout << "eventN:2 " << eventN << std::endl;

    // Total weight
    double totWeight = ( eventWeight )*1.;
    //double totWeight = 1.;
    
    // add pileup as weight for MC 
    if(evt->DataBool_()){
      //double pu_weight = hPURatio->GetBinContent( hPURatio->FindBin( evt->NVtx_() ) );
      //totWeight*=pu_weight;
    }

    // Apply IsoTrkVeto
    bool passIso=false;
    if(utils2::applyIsoTrk){
      if(evt->nIsoPion()==0&&evt->nIsoMu()==0&&evt->nIsoElec()==0)passIso=true;
    }
    else passIso=true;


    bool pass3=false;
    if(TauHadModel>=4)pass3=true;
    else{
      // Ahmad33 this is to remove acceptance role to check other sources of error.
      if( genTauPt > LeptonAcceptance::muonPtMin() && std::abs(genTauEta) < LeptonAcceptance::muonEtaMax() )pass3=true;
    }

    
    //std::cout<<" eventN "<<eventN<<"*************CH1 *****************"<<endl;
    

    // Find the visible part of the hadronic tau 
    TVector3 TauNu3Vec,Tau3Vec,Visible3Vec;
    for(int i=0; i < evt->GenTauNuPtVec_().size(); i++){
      TauNu3Vec.SetPtEtaPhi(evt->GenTauNuPtVec_()[NuIndex],evt->GenTauNuEtaVec_()[NuIndex],evt->GenTauNuPhiVec_()[NuIndex]);
    }
    if(genTauPt>0.)Tau3Vec.SetPtEtaPhi(genTauPt,genTauEta,genTauPhi);
    else {Tau3Vec.SetPtEtaPhi(0,0,0);/* Ahmad33 cout<<"Warning \n Warning \n Tau3Vec=0 \n "; Ahmad33 */}
    Visible3Vec=Tau3Vec-TauNu3Vec;



    if(pass3){
      cutflow_preselection->Fill(9.,eventWeight); // We may ask genTau within muon acceptance

      // Apply low delphi region
      if(sel->nolep(evt->nLeptons())&&sel->Njet_4(evt->nJets())&&sel->ht_500(evt->ht())
	 &&sel->mht_200(evt->mht())&&sel->low_dphi(evt->nJets(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->deltaPhi4())
	 ){

	Iso_all_lowDphi->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight); // the weight has only scaling info.
	//Iso_all_nb_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets() == 2) Iso_all_nb_njet2_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=3 && evt->nJets()<=4) Iso_all_nb_njet34_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=5 && evt->nJets()<=6) Iso_all_nb_njet56_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=7 && evt->nJets()<=8) Iso_all_nb_njet78_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=9                   ) Iso_all_nb_njet9_lowDphi->Fill(  utils2::findBin_NBtag(evt->nBtags()),eventWeight );

	if(evt->nIsoPion()==0&&evt->nIsoMu()==0&&evt->nIsoElec()==0){
	  Iso_pass_lowDphi->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	  //Iso_pass_nb_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()==2) Iso_pass_nb_njet2_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=3 && evt->nJets()<=4) Iso_pass_nb_njet34_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=5 && evt->nJets()<=6) Iso_pass_nb_njet56_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=7 && evt->nJets()<=8) Iso_pass_nb_njet78_lowDphi->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=9                   ) Iso_pass_nb_njet9_lowDphi->Fill(  utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	}
      }
       // Apply baseline cuts
      if(sel->nolep(evt->nLeptons())&&sel->Njet_4(evt->nJets())&&sel->ht_500(evt->ht())
	 &&sel->mht_200(evt->mht())&&sel->dphi(evt->nJets(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->deltaPhi4()) 
	 ){

	// calculate trigger efficiency 
	trig_all->Fill(binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	if(trigPass)trig_pass->Fill(binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);

	IsoElec_all->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	if(evt->nIsoElec()==0)IsoElec_pass->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	IsoMu_all->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	if(evt->nIsoMu()==0)IsoMu_pass->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	IsoPion_all->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	if(evt->nIsoPion()==0)IsoPion_pass->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);

	Iso_all->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	//Iso_all_nb->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()==2) Iso_all_nb_njet2->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=3 && evt->nJets()<=4) Iso_all_nb_njet34->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=5 && evt->nJets()<=6) Iso_all_nb_njet56->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=7 && evt->nJets()<=8) Iso_all_nb_njet78->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	if (evt->nJets()>=9                   ) Iso_all_nb_njet9->Fill(  utils2::findBin_NBtag(evt->nBtags()),eventWeight );

	if(evt->nIsoPion()==0&&evt->nIsoMu()==0&&evt->nIsoElec()==0){
	  Iso_pass->Fill( binMap_ForAcc[utils2::findBin_ForAcc(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	  //Iso_pass_nb->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()==2) Iso_pass_nb_njet2->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=3 && evt->nJets()<=4) Iso_pass_nb_njet34->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=5 && evt->nJets()<=6) Iso_pass_nb_njet56->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=7 && evt->nJets()<=8) Iso_pass_nb_njet78->Fill( utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	  if (evt->nJets()>=9                   ) Iso_pass_nb_njet9->Fill(  utils2::findBin_NBtag(evt->nBtags()),eventWeight );
	}          
	// we are also interested to see how often the leading tau jet is vetoed by IsoTrk
	Iso_all2->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight);
	int IsoElecIdx=-1, IsoMuIdx=-1, IsoPionIdx=-1;

	//std::cout << "eventN:3 " << eventN << std::endl;

	// Match directly to IsoTrk. But this wouldn't capture all 
	utils->findMatchedObject(IsoElecIdx,Visible3Vec.Eta(),Visible3Vec.Phi(),evt->IsoElecPtVec_(),evt->IsoElecEtaVec_(),evt->IsoElecPhiVec_(),0.4,verbose);
	// 
	int JetIndex=-1;
	utils->findMatchedObject(JetIndex,Visible3Vec.Eta(),Visible3Vec.Phi(), evt->slimJetPtVec_(), evt->slimJetEtaVec_(), evt->slimJetPhiVec_(),0.4,verbose);
	//printf("IsoElecIdx: %d \n ",IsoElecIdx);
	if(JetIndex!=-1)utils->findMatchedObject(IsoElecIdx,evt->slimJetEtaVec_()[JetIndex],evt->slimJetPhiVec_()[JetIndex],evt->IsoElecPtVec_(),evt->IsoElecEtaVec_(),evt->IsoElecPhiVec_(),0.4,verbose);
	//printf("IsoElecIdx: %d \n ",IsoElecIdx);

	utils->findMatchedObject(IsoMuIdx,Visible3Vec.Eta(),Visible3Vec.Phi(),evt->IsoMuPtVec_(),evt->IsoMuEtaVec_(),evt->IsoMuPhiVec_(),0.4,verbose);
	utils->findMatchedObject(IsoPionIdx,Visible3Vec.Eta(),Visible3Vec.Phi(),evt->IsoPionPtVec_(),evt->IsoPionEtaVec_(),evt->IsoPionPhiVec_(),0.4,verbose);
	if( IsoElecIdx==-1 && IsoMuIdx==-1 && IsoPionIdx==-1)
	  Iso_pass2->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],eventWeight); 

	if(passIso){
	  // Fill Search bin histogram
	  searchH->Fill( binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],totWeight);
	  QCD_Up->Fill( binMap_QCD[utils2::findBin_QCD(evt->nJets(),evt->nBtags(),evt->ht(),evt->mht()).c_str()],totWeight);
	  //	      if(!(evt->nJets()>=7 && evt->mht()<500 && evt->ht()<500))              
	  searchH_b->Fill( binMap_b[utils2::findBin(evt->nJets(),evt->nBtags(),evt->ht(),evt->mht()).c_str()],totWeight);
	  if(evt->nBtags()==0)hPredHTMHT0b->Fill( binMap_HTMHT[utils2::findBin_HTMHT(evt->ht(),evt->mht()).c_str()],totWeight);
	  if(evt->nBtags() >0)hPredHTMHTwb->Fill( binMap_HTMHT[utils2::findBin_HTMHT(evt->ht(),evt->mht()).c_str()],totWeight);
	  hPredNJetBins->Fill(evt->nJets(),totWeight);
	  hPredNbBins->Fill( evt->nBtags(),totWeight);
	}
      }

    }
    for (int i=1;i<=174;i++){
      searchH_b->GetXaxis()->SetBinLabel(i,utils2::RenameBins(i));
    }
 
    // Fill QCD histogram
    if(pass3){
      // Fill the histogram in the inverted delta phi region
      if(sel->ht_500(evt->ht()) && sel->mht_200(evt->mht()) && sel->Njet_4(evt->nJets()) && sel->low_dphi(evt->nJets(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->deltaPhi4()) ){
	// Fill QCD histograms
	if(passIso){

	  QCD_Low->Fill( binMap_QCD[utils2::findBin_QCD(evt->nJets(),evt->nBtags(),evt->ht(),evt->mht()).c_str()],totWeight);
	  searchH_lowDphi->Fill(binMap[utils2::findBin_NoB(evt->nJets(),evt->ht(),evt->mht()).c_str()],totWeight);

	}
      }
    }
     

    // for plotting purposes
    double tauPt_forPlotting=0.0; 
    double tauEta_forPlotting=0.0;
    double tauPhi_forPlotting=-99.0;
    double tau_mht_dlephi_forPlotting=-99.0;
    int tauJetIdx_forPlotting = -1;
    double deltaRMax_forPlotting = genTauPt < 50. ? 0.2 : 0.1;
    if( utils->findMatchedObject(tauJetIdx_forPlotting,Visible3Vec.Eta(),Visible3Vec.Phi(), evt->slimJetPtVec_(), evt->slimJetEtaVec_(), evt->slimJetPhiVec_(),deltaRMax_forPlotting,verbose) ){
      tauPt_forPlotting = evt->slimJetPtVec_().at(tauJetIdx_forPlotting);
      tauEta_forPlotting = evt->slimJetEtaVec_().at(tauJetIdx_forPlotting);       
      if(tauPt_forPlotting>30.&& abs(tauEta_forPlotting) < 2.4){
	//tauEta_forPlotting = evt->slimJetEtaVec_().at(tauJetIdx_forPlotting);
	tauPhi_forPlotting = evt->slimJetPhiVec_().at(tauJetIdx_forPlotting);
	tau_mht_dlephi_forPlotting = fabs(TVector2::Phi_mpi_pi( tauPhi_forPlotting - evt->mhtphi()  ));

	//printf("phi(tau,mht): %g tauJetPt: %g GenTauPt: %g \n ",tau_mht_dlephi_forPlotting,tauPt_forPlotting,genTauPt);
      }
      //printf("tauPt: %g tauPhi: %g \n ",tauPt_forPlotting,tauPhi_forPlotting);
    }


    // Build and array that contains the quantities we need a histogram for.
    // Here order is important and must be the same as RA2nocutvec
    double eveinfvec[] = {totWeight,(double) evt->ht(),(double) evt->ht(),(double) evt->mht(),(double) evt->mht(),(double)evt->met(),
			  (double)evt->deltaPhi1(),(double)evt->deltaPhi2(),(double)evt->deltaPhi3(),(double)evt->deltaPhi4(),
			  (double) evt->nJets(),(double) evt->nBtags(),
			  (double) tauPt_forPlotting,(double) tauPhi_forPlotting, (double) tau_mht_dlephi_forPlotting
    }; 


    // Ahmad33 this is to remove acceptance role to check other sources of error.       

      
    if(pass3){
	
      cutflow_preselection->Fill(10.,eventWeight); // We may ask genTau within muon acceptance - This should corresponds to "allEvents" in histogram root files

      //loop over all the different backgrounds: "allEvents", "Wlv", "Zvv"
      for(map<string, map<string , vector<TH1D> > >::iterator itt=map_map.begin(); itt!=map_map.end();itt++){//this will be terminated after the cuts

	////determine what type of background should pass
	if(itt->first=="allEvents"){//all the cuts are inside this

	  //Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts

	  //////loop over cut names and fill the histograms
	  for(map<string , vector<TH1D> >::iterator ite=cut_histvec_map.begin(); ite!=cut_histvec_map.end();ite++){

	    if(sel->checkcut(ite->first,evt->ht(),evt->mht(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->deltaPhi4(),evt->nJets(),evt->nBtags(),evt->nLeptons(),evt->nIsoElec(),evt->nIsoMu(),evt->nIsoPion())==true){
	      histobjmap[ite->first].fill(Nhists,&eveinfvec[0] ,&itt->second[ite->first][0]);
	    }
	  }//end of loop over cut names

	  ////EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts
            
	}//end of bg_type determination
      }//end of loop over all the different backgrounds: "allEvents", "Wlv", "Zvv"

      ///////////////////////////////
      ////End of Closure Test Section
      ///////////////////////////////


    }//end of loop over pass3 // Ahmad33
 
    // Do the matching
    int tauJetIdx = -1;
    int tauJetUpIdx=-1;
    int tauJetDnIdx=-1;

    double deltaRMax = genTauPt < 50. ? 0.2 : 0.1; // Increase deltaRMax at low pt to maintain high-enought matching efficiency
    if(inputnumber=="LargerDelR")deltaRMax = genTauPt < 50. ? 0.4 : 0.2; // Increase deltaRMax at low pt to maintain high-enought matching efficiency

    // Lets write all the gen tau events regardless of if they match a jet or not.
    //we want to consider events that pass the baseline cuts
    if(genTauPt >= 20. && std::abs(genTauEta) <= 2.1 && evt->nJets() >2 )GenTau_Jet_all->Fill(genTauPt,eventWeight);
      
    /*
    // 
    TVector3 TauNu3Vec,Tau3Vec,Visible3Vec;
    for(int i=0; i < evt->GenTauNuPtVec_().size(); i++){
    TauNu3Vec.SetPtEtaPhi(evt->GenTauNuPtVec_()[NuIndex],evt->GenTauNuEtaVec_()[NuIndex],evt->GenTauNuPhiVec_()[NuIndex]);
    }
    if(genTauPt>0.)Tau3Vec.SetPtEtaPhi(genTauPt,genTauEta,genTauPhi);
    else {Tau3Vec.SetPtEtaPhi(0,0,0);}
    Visible3Vec=Tau3Vec-TauNu3Vec;
    */
    std::cout<<"eventN "<<eventN <<" *****************************Second point************************ "<<endl;
    
    if(verbose!=0){      
      printf("TauNu3Vec: pt: %g eta: %g phi: %g \n Tau3Vec: pt: %g eta: %g phi: %g \n",TauNu3Vec.Pt(),TauNu3Vec.Eta(),TauNu3Vec.Phi(),Tau3Vec.Pt(),Tau3Vec.Eta(),Tau3Vec.Phi());
      printf("Visible3Vec: pt: %g eta: %g phi: %g \n ",Visible3Vec.Pt(),Visible3Vec.Eta(),Visible3Vec.Phi());
    }

    if( !utils->findMatchedObject(tauJetIdx,Visible3Vec.Eta(),Visible3Vec.Phi(), evt->slimJetPtVec_(), evt->slimJetEtaVec_(), evt->slimJetPhiVec_(),deltaRMax,verbose) ){
      std::cout<<"eventN "<<eventN <<" *****************************Third point************************ "<<endl;
    
      if(genTauPt >= 20. && std::fabs(genTauEta) <= 2.1 && evt->nJets() >2 )GenTau_Jet_fail->Fill(genTauPt,eventWeight);
      continue;
    } // this also determines tauJetIdx

    //*AR,Nov8,2016-Check dR between different jets
    
    if(tauJetIdx!=-1){
      // TLorentzVector tlv_taujet;
      //TLorentzVector tlv_jet;
      std::cout<<"eventN "<<eventN <<" tauJetIdx "<<tauJetIdx<<" *****************************Fourth point************************ "<<endl;
    
      double taujet_eta=evt->JetsEtaVec_()[tauJetIdx];
      double taujet_phi=evt->JetsPhiVec_()[tauJetIdx];
      double taujet_pt=evt->JetsPtVec_()[tauJetIdx];
      //std::cout<<" eventN "<<eventN<<" tauJetIdx "<<tauJetIdx<<" taujet_eta "<<taujet_eta<<" taujet_phi "<<taujet_phi<<" num_slimjets "<<evt->slimJetPtVec_().size()<<endl;
      		    
      //tlv_taujet.SetPtEtaPhiM(evt->JetsEtaVec_()[tauJetIdx]
      for(int jetIdx = 0; jetIdx < (int) evt->slimJetPtVec_().size(); ++jetIdx){
	if(jetIdx != tauJetIdx){
	  double jet_eta=evt->slimJetEtaVec_()[jetIdx];
	  double jet_phi=evt->slimJetPhiVec_()[jetIdx];
	  double delta_eta_square=(taujet_eta-jet_eta)*(taujet_eta-jet_eta);
	  double delta_phi_square=(taujet_phi-jet_phi)*(taujet_phi-jet_phi);
	  double dR_taujet_jet=sqrt(delta_eta_square + delta_phi_square);
	  if(dR_taujet_jet<0.4)
	    std::cout<<" eventN "<<eventN<<" tauJetIdx "<<tauJetIdx<<" taujet_pt "<<taujet_pt<<" taujet_eta "<<taujet_eta<<" taujet_phi "<<taujet_phi<<" jetIdx "<<jetIdx<<" jet_eta "<<jet_eta<<" jet_phi "<<jet_phi<<" dR "<<dR_taujet_jet<<endl;
	    
	}
      }
    }
    

    utils->findMatchedObject(tauJetUpIdx,Visible3Vec.Eta(),Visible3Vec.Phi(),evt->slimJetupPtVec_(), evt->slimJetupEtaVec_(), evt->slimJetupPhiVec_(),deltaRMax,verbose);
    utils->findMatchedObject(tauJetDnIdx,Visible3Vec.Eta(),Visible3Vec.Phi(), evt->slimJetdownPtVec_(), evt->slimJetdownEtaVec_(), evt->slimJetdownPhiVec_(),deltaRMax,verbose);
    
    //std::cout<<"eventN "<<eventN <<" *****************************Second point************************ "<<endl;

    if(verbose!=0){printf("Event: %d, tauJetIdx: %d \n",eventN,tauJetIdx);
      if(tauJetIdx!=-1){
	printf("JetEta: %g ,JetPhi(): %g ,JetPt(): %g \n genTauEta: %g, genTauPhi: %g, genTauPt: %g \n",evt->JetsEtaVec_()[tauJetIdx],evt->JetsPhiVec_()[tauJetIdx],evt->JetsPtVec_()[tauJetIdx],genTauEta,genTauPhi,genTauPt);
      }
    }
    if(tauJetUpIdx ==-1 || tauJetDnIdx ==-1)
      continue;
    /*
    // Fill tauJet Pt histogram
    double failRate = hFailRate_GenTau_Jet->GetBinContent(hFailRate_GenTau_Jet->GetXaxis()->FindBin(genTauPt));
    // We know N_tot = N_pass + N_fail. And, failRate=N_fial/N_tot ==> N_tot = 1/(1-failRate) N_pass
    double tauJetWeight = totWeight * 1/(1-failRate);
    for(int jetIdx = 0; jetIdx < (int) evt->slimJetPtVec_().size(); ++jetIdx){ // Loop over reco jets
    // Select tau jet
    if( jetIdx == tauJetIdx ) {
    // Fill the tauJetPtHist after the cut "delphi" 
    if(sel->checkcut("delphi",evt->ht(),evt->mht(),evt->deltaPhi1(),evt->deltaPhi2(),evt->deltaPhi3(),evt->nJets(),evt->nBtags(),evt->nLeptons(),evt->nIsoElec(),evt->nIsoMu(),evt->nIsoPion())==true)tauJetPtHist->Fill( evt->slimJetPtVec_().at(jetIdx), tauJetWeight);// this is tauJetPt that later is defined.

    break; // End the jet loop once the tau jet has been found
    }
    } // End of loop over reco jets
    */

    // Use only events where the tau is inside the muon acceptance
    // because lateron we will apply the response to muon+jet events
    if(verbose!=0)printf("genTauPt:%g genTauEta: %g  \n ",genTauPt,genTauEta); 

    if( genTauPt < LeptonAcceptance::muonPtMin() ) continue; // 10.
    if( std::abs(genTauEta) > LeptonAcceptance::muonEtaMax() ) continue; // 2.4

    if(verbose!=0)printf("genTauPt>20 and eta< 2.1 passed \n ");  



    // Calculate RA2 selection-variables from "cleaned" jets, i.e. jets withouth the tau-jet
    int selNJet = 0; // Number of HT jets (jets pt > 50 GeV and |eta| < 2.5)
    for(int jetIdx = 0; jetIdx <(int) evt->JetsPtVec_().size(); ++jetIdx) { // Loop over reco jets
      // Skip this jet if it is the tau
      if( jetIdx == tauJetIdx ) continue;
      // Calculate NJet
      if( evt->JetsPtVec_()[jetIdx] > 30. && std::abs(evt->JetsEtaVec_()[jetIdx]) < 2.4 ) selNJet++;
    } // End of loop over reco jets
    

      // Select only events with at least 2 HT jets
    if( selNJet < 2 ) continue;

    if(verbose!=0)printf("selNJet > 2 passed \n " );  
    // Fill histogram with relative visible energy of the tau
    // ("tau response template") for hadronically decaying taus
    //for(int jetIdx = 0; jetIdx < (int) evt->slimJetPtVec_().size(); ++jetIdx) { // Loop over reco jets
    // Select tau jet
    //std::cout<<"eventN "<<eventN<<"jetid "<<jetIdx<<"tauJetPt "<<evt->slimJetPtVec_().at(jetIdx)<<"tauJetPtUp "<<evt->slimJetJECup_().at(jetIdx).Pt()<<"tauJetPtDown "<<evt->slimJetJECdown_().at(jetIdx).Pt()<<std::endl;
    //std::cout<<"slimJetPtVec_size "<<(int)evt->slimJetPtVec_().size()<<endl;
    //std::cout<<"slimJetJECUp_size "<<evt->slimJetJECup_().size()<<endl;
    //std::cout<<"slimJetJECDown_size "<<evt->slimJetJECdown_().size()<<endl;
    //if( jetIdx == tauJetIdx ) {
    //  std::cout<<"slimJetPtVec_size "<<(int)evt->slimJetPtVec_().size()<<endl;
    //std::cout<<"slimJetJECUp_size "<<(int)evt->slimJetJECup_().size()<<endl;
    //std::cout<<"slimJetJECDown_size "<<(int)evt->slimJetJECdown_().size()<<endl;

    // Get the response pt bin for the tau
    //printf(" slimSize: %d UpSize: %d DownSize: %d \n ",evt->slimJetPtVec_().size(),evt->slimJetJECup_().size(),evt->slimJetJECdown_().size());
	 
   
    
      
      
    const double tauJetPt = evt->slimJetPtVec_().at(tauJetIdx);
    //std::cout<<" tauJetPt "<<tauJetPt<<endl; 
    //	  const double tauJetPtUp = evt->slimJetJECup_.at(jetIdx).Pt();
    //std::cout<<" tauJetPtUp "<<tauJetPtUp<<endl;
    //const double tauJetPtDown = evt->slimJetJECdown_().at(jetIdx).Pt();
    
    const double tauJetPtUp = evt->slimJetupPtVec_().at(tauJetUpIdx);
    
    const double tauJetPtDown = evt->slimJetdownPtVec_().at(tauJetDnIdx);
    
    //std::cout<<" tauJetPtDown "<<tauJetPtDown<<endl;   
    //std::cout<<"eventN "<<eventN<<"tauJetIdx "<< tauJetIdx <<"jetid "<<jetIdx<<"tauJetPt "<<tauJetPt<<"tauJetPtUp "<<tauJetPtUp<<"tauJetPtDown "<<tauJetPtDown<<std::endl;
    //std::cout<<" eventN "<<eventN<<"*****************************Check Seg Violation************************"<<endl;
    
    
	  
    const unsigned int ptBin = utils->TauResponse_ptBin(genTauPt);
    // Fill the corresponding response template
    hTauResp.at(ptBin)->Fill( tauJetPt / genTauPt ,eventWeight);
    hTauRespUp.at(ptBin)->Fill( tauJetPtUp / genTauPt ,eventWeight);

    hTauRespDown.at(ptBin)->Fill( tauJetPtDown / genTauPt ,eventWeight);
    hGenPt.at(ptBin)->Fill( genTauPt ,eventWeight);
    tau_GenPt->Fill( genTauPt ,eventWeight);

    double tauJetPhi = evt->slimJetPhiVec_().at(tauJetIdx);
    const double tauJetPt_x = tauJetPt * cos( TVector2::Phi_mpi_pi( genTauPhi - tauJetPhi) );
    const double tauJetPt_y = tauJetPt * sin( TVector2::Phi_mpi_pi( genTauPhi - tauJetPhi) ); 
    hTauResp_x.at(ptBin)->Fill( tauJetPt_x / genTauPt ,eventWeight);
    hTauResp_y.at(ptBin)->Fill( tauJetPt_y / genTauPt ,eventWeight);
	  
    hTauResp_xy.at(ptBin)->Fill(tauJetPt_x / genTauPt , tauJetPt_y / genTauPt ,eventWeight);

    if(verbose!=0)printf("ptBin: %d tauJetPt: %g genTauPt: %g \n ",ptBin,tauJetPt,genTauPt); 

    if(TVector2::Phi_mpi_pi( genTauPhi - tauJetPhi) < -1.0)tau_GenJetPhi->Fill(tauJetPt / genTauPt ,-1.0 ,eventWeight);
    else if(TVector2::Phi_mpi_pi( genTauPhi - tauJetPhi) > 1.0)tau_GenJetPhi->Fill(tauJetPt / genTauPt , 1.0 ,eventWeight);
    else tau_GenJetPhi->Fill(tauJetPt / genTauPt , TVector2::Phi_mpi_pi( genTauPhi - tauJetPhi) ,eventWeight);

    //break; // End the jet loop once the tau jet has been found
    //}
    //} // End of loop over reco jets
    //if(eventN%100==0)
    //std::cout<<" eventN "<<eventN<<"*************End of loop over event *****************"<<endl;

  } // End of loop over events  
    
  printf("nCleanEve: %d nHadTauEve: %d nNoLepEve: %d \n ",nCleanEve,nHadTauEve,nNoLepEve);

  // Calculate trigger efficiency 
  TH1* trigEff = static_cast<TH1*>(trig_pass->Clone("trigEff"));
  trigEff->Divide(trig_pass,trig_all,1,1,"B");
  sprintf(tempname,"%s/TriggerEff_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile trigFile(tempname,"RECREATE");
  trigEff->Write();
  trig_all->Write();
  trig_pass->Write();
  trigFile.Close();

  // Compute isotrack veto efficiencies
  TH1* IsoElecEff = static_cast<TH1*>(IsoElec_pass->Clone("IsoElecEff"));
  IsoElecEff->Divide(IsoElec_pass,IsoElec_all,1,1,"B");
  TH1* IsoMuEff = static_cast<TH1*>(IsoMu_pass->Clone("IsoMuEff"));
  IsoMuEff->Divide(IsoMu_pass,IsoMu_all,1,1,"B");
  TH1* IsoPionEff = static_cast<TH1*>(IsoPion_pass->Clone("IsoPionEff"));
  IsoPionEff->Divide(IsoPion_pass,IsoPion_all,1,1,"B");
  TH1* IsoEff = static_cast<TH1*>(Iso_pass->Clone("IsoEff"));
  IsoEff->Divide(Iso_pass,Iso_all,1,1,"B");
  TH1* IsoEff_lowDphi = static_cast<TH1*>(Iso_pass_lowDphi->Clone("IsoEff_lowDphi"));
  IsoEff_lowDphi->Divide(Iso_pass_lowDphi,Iso_all_lowDphi,1,1,"B");
  TH1* IsoEff2 = static_cast<TH1*>(Iso_pass2->Clone("IsoEff2"));
  IsoEff2->Divide(Iso_pass2,Iso_all2,1,1,"B");   

  // isotrack veto efficiencies - nb dependence 
  // rescaled to center around unity because they will be applied on top of already applied isotrk veto efficiency
  /*
    TH1* IsoNbEff = static_cast<TH1*>(Iso_pass_nb->Clone("IsoNbEff"));
    IsoNbEff->Divide(Iso_pass_nb,Iso_all_nb,1,1,"B");
    if (Iso_pass_nb->GetSumOfWeights()>0.) IsoNbEff->Scale(Iso_all_nb->GetSumOfWeights()/Iso_pass_nb->GetSumOfWeights()); 
    TH1* IsoNbEff_lowDphi = static_cast<TH1*>(Iso_pass_nb_lowDphi->Clone("IsoNbEff_lowDphi"));
    IsoNbEff_lowDphi->Divide(Iso_pass_nb_lowDphi,Iso_all_nb_lowDphi,1,1,"B");
    if (Iso_pass_nb_lowDphi->GetSumOfWeights()>0.) IsoNbEff_lowDphi->Scale(Iso_all_nb_lowDphi->GetSumOfWeights()/Iso_pass_nb_lowDphi->GetSumOfWeights());
  */
  TH1* IsoEff_NbNjet2 = static_cast<TH1*>(Iso_pass_nb_njet2->Clone("IsoEff_NbNjet2"));
  IsoEff_NbNjet2->Divide(Iso_pass_nb_njet2,Iso_all_nb_njet2,1,1,"B");
  if (Iso_pass_nb_njet2->GetSumOfWeights()>0.) IsoEff_NbNjet2->Scale(Iso_all_nb_njet2->GetSumOfWeights()/Iso_pass_nb_njet2->GetSumOfWeights());
  TH1* IsoEff_NbNjet34 = static_cast<TH1*>(Iso_pass_nb_njet34->Clone("IsoEff_NbNjet34"));
  IsoEff_NbNjet34->Divide(Iso_pass_nb_njet34,Iso_all_nb_njet34,1,1,"B");
  if (Iso_pass_nb_njet34->GetSumOfWeights()>0.) IsoEff_NbNjet34->Scale(Iso_all_nb_njet34->GetSumOfWeights()/Iso_pass_nb_njet34->GetSumOfWeights());
  TH1* IsoEff_NbNjet56 = static_cast<TH1*>(Iso_pass_nb_njet56->Clone("IsoEff_NbNjet56"));
  IsoEff_NbNjet56->Divide(Iso_pass_nb_njet56,Iso_all_nb_njet56,1,1,"B");
  if (Iso_pass_nb_njet56->GetSumOfWeights()>0.) IsoEff_NbNjet56->Scale(Iso_all_nb_njet56->GetSumOfWeights()/Iso_pass_nb_njet56->GetSumOfWeights());
  TH1* IsoEff_NbNjet78 = static_cast<TH1*>(Iso_pass_nb_njet78->Clone("IsoEff_NbNjet78"));
  IsoEff_NbNjet78->Divide(Iso_pass_nb_njet78,Iso_all_nb_njet78,1,1,"B");
  if (Iso_pass_nb_njet78->GetSumOfWeights()>0.) IsoEff_NbNjet78->Scale(Iso_all_nb_njet78->GetSumOfWeights()/Iso_pass_nb_njet78->GetSumOfWeights());
  TH1* IsoEff_NbNjet9 = static_cast<TH1*>(Iso_pass_nb_njet9->Clone("IsoEff_NbNjet9"));
  IsoEff_NbNjet9->Divide(Iso_pass_nb_njet9,Iso_all_nb_njet9,1,1,"B");
  if (Iso_pass_nb_njet9->GetSumOfWeights()>0.) IsoEff_NbNjet9->Scale(Iso_all_nb_njet9->GetSumOfWeights()/Iso_pass_nb_njet9->GetSumOfWeights());

  TH1* IsoEff_NbNjet2_lowDphi = static_cast<TH1*>(Iso_pass_nb_njet2_lowDphi->Clone("IsoEff_NbNjet2_lowDphi"));
  IsoEff_NbNjet2_lowDphi->Divide(Iso_pass_nb_njet2_lowDphi,Iso_all_nb_njet2_lowDphi,1,1,"B");
  if (Iso_pass_nb_njet2_lowDphi->GetSumOfWeights()>0.) IsoEff_NbNjet2_lowDphi->Scale(Iso_all_nb_njet2_lowDphi->GetSumOfWeights()/Iso_pass_nb_njet2_lowDphi->GetSumOfWeights());
  TH1* IsoEff_NbNjet34_lowDphi = static_cast<TH1*>(Iso_pass_nb_njet34_lowDphi->Clone("IsoEff_NbNjet34_lowDphi"));
  IsoEff_NbNjet34_lowDphi->Divide(Iso_pass_nb_njet34_lowDphi,Iso_all_nb_njet34_lowDphi,1,1,"B");
  if (Iso_pass_nb_njet34_lowDphi->GetSumOfWeights()>0.) IsoEff_NbNjet34_lowDphi->Scale(Iso_all_nb_njet34_lowDphi->GetSumOfWeights()/Iso_pass_nb_njet34_lowDphi->GetSumOfWeights());
  TH1* IsoEff_NbNjet56_lowDphi = static_cast<TH1*>(Iso_pass_nb_njet56_lowDphi->Clone("IsoEff_NbNjet56_lowDphi"));
  IsoEff_NbNjet56_lowDphi->Divide(Iso_pass_nb_njet56_lowDphi,Iso_all_nb_njet56_lowDphi,1,1,"B");
  if (Iso_pass_nb_njet56_lowDphi->GetSumOfWeights()>0.) IsoEff_NbNjet56_lowDphi->Scale(Iso_all_nb_njet56_lowDphi->GetSumOfWeights()/Iso_pass_nb_njet56_lowDphi->GetSumOfWeights());
  TH1* IsoEff_NbNjet78_lowDphi = static_cast<TH1*>(Iso_pass_nb_njet78_lowDphi->Clone("IsoEff_NbNjet78_lowDphi"));
  IsoEff_NbNjet78_lowDphi->Divide(Iso_pass_nb_njet78_lowDphi,Iso_all_nb_njet78_lowDphi,1,1,"B");
  if (Iso_pass_nb_njet78_lowDphi->GetSumOfWeights()>0.) IsoEff_NbNjet78_lowDphi->Scale(Iso_all_nb_njet78_lowDphi->GetSumOfWeights()/Iso_pass_nb_njet78_lowDphi->GetSumOfWeights());
  TH1* IsoEff_NbNjet9_lowDphi = static_cast<TH1*>(Iso_pass_nb_njet9_lowDphi->Clone("IsoEff_NbNjet9_lowDphi"));
  IsoEff_NbNjet9_lowDphi->Divide(Iso_pass_nb_njet9_lowDphi,Iso_all_nb_njet9_lowDphi,1,1,"B");
  if (Iso_pass_nb_njet9_lowDphi->GetSumOfWeights()>0.) IsoEff_NbNjet9_lowDphi->Scale(Iso_all_nb_njet9_lowDphi->GetSumOfWeights()/Iso_pass_nb_njet9_lowDphi->GetSumOfWeights());
 
  sprintf(tempname,"%s/IsoEfficiencies_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile outFile3(tempname,"RECREATE");
  IsoElecEff->Write();
  IsoElec_pass->Write();
  IsoElec_all->Write();
  IsoMuEff->Write();
  IsoMu_pass->Write();
  IsoMu_all->Write();
  IsoPionEff->Write();
  IsoPion_pass->Write();
  IsoPion_all->Write();
  IsoEff->Write();
  Iso_pass->Write();
  Iso_all->Write();
  IsoEff_lowDphi->Write();
  Iso_pass_lowDphi->Write();
  Iso_all_lowDphi->Write();
  IsoEff2->Write();
  Iso_pass2->Write();
  Iso_all2->Write();
  /*
    IsoNbEff->Write();
    Iso_pass_nb->Write();
    Iso_all_nb->Write();
    IsoNbEff_lowDphi->Write();
    Iso_pass_nb_lowDphi->Write();
    Iso_all_nb_lowDphi->Write();
  */
  IsoEff_NbNjet2->Write();
  Iso_pass_nb_njet2->Write();
  Iso_all_nb_njet2->Write();
  IsoEff_NbNjet34->Write();
  Iso_pass_nb_njet34->Write();
  Iso_all_nb_njet34->Write();
  IsoEff_NbNjet56->Write();
  Iso_pass_nb_njet56->Write();
  Iso_all_nb_njet56->Write();
  IsoEff_NbNjet78->Write();
  Iso_pass_nb_njet78->Write();
  Iso_all_nb_njet78->Write();
  IsoEff_NbNjet9->Write();    
  Iso_pass_nb_njet9->Write();
  Iso_all_nb_njet9->Write();
  IsoEff_NbNjet2_lowDphi->Write();
  Iso_pass_nb_njet2_lowDphi->Write();
  Iso_all_nb_njet2_lowDphi->Write();
  IsoEff_NbNjet34_lowDphi->Write();
  Iso_pass_nb_njet34_lowDphi->Write();
  Iso_all_nb_njet34_lowDphi->Write();
  IsoEff_NbNjet56_lowDphi->Write();
  Iso_pass_nb_njet56_lowDphi->Write();
  Iso_all_nb_njet56_lowDphi->Write();
  IsoEff_NbNjet78_lowDphi->Write();
  Iso_pass_nb_njet78_lowDphi->Write();
  Iso_all_nb_njet78_lowDphi->Write();
  IsoEff_NbNjet9_lowDphi->Write();    
  Iso_pass_nb_njet9_lowDphi->Write();
  Iso_all_nb_njet9_lowDphi->Write();
  outFile3.Close();

  // Compute acceptance
  TH1* hAcc = static_cast<TH1*>(hAccPass->Clone("hAcc"));
  hAcc->Divide(hAccPass,hAccAll,1,1,"B");// we use B option here because the two histograms are correlated. see TH1 page in the root manual.
  TH1* hAcc_0b_ = static_cast<TH1*>(hAcc_0b_Pass->Clone("hAcc_0b_"));
  hAcc_0b_->Divide(hAcc_0b_Pass,hAcc_0b_All,1,1,"B");// we use B option here because the two histograms are correlated. see TH1 page in the root manual.
  TH1* hAcc_non0b_ = static_cast<TH1*>(hAcc_non0b_Pass->Clone("hAcc_non0b_"));
  hAcc_non0b_->Divide(hAcc_non0b_Pass,hAcc_non0b_All,1,1,"B");// we use B option here because the two histograms are correlated. see TH1 page in the root manual.

  TH1* hAcc_lowDphi = static_cast<TH1*>(hAccPass_lowDphi->Clone("hAcc_lowDphi"));
  hAcc_lowDphi->Divide(hAccPass_lowDphi,hAccAll_lowDphi,1,1,"B");
  // some temporary histograms for acceptance systematics
  vector<TH1*> hAccVec, hAcc_lowDphiVec, hAcc_DeviationFromNomVec, hAcc_DeviationFromNom_lowDphiVec;
  vector<TH1*> hScaleAccVec, hScaleAcc_lowDphiVec, hScaleAcc_DeviationFromNomVec, hScaleAcc_DeviationFromNom_lowDphiVec;
  if(CalcAccSys){
    for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
      sprintf(tempname,"hAccVec_%d",iacc);
      hAccVec.push_back(static_cast<TH1*>(hAccPassVec[iacc]->Clone(tempname)));
      hAccVec[iacc]->Divide(hAccPassVec[iacc],hAccAllVec[iacc],1,1,"B");
      sprintf(tempname,"hAcc_lowDphiVec_%d",iacc);
      hAcc_lowDphiVec.push_back(static_cast<TH1*>(hAccPass_lowDphiVec[iacc]->Clone(tempname)));
      hAcc_lowDphiVec[iacc]->Divide(hAccPass_lowDphiVec[iacc],hAccAll_lowDphiVec[iacc],1,1,"B");
      // calculate the deviation from nominal acceptance 
      hAcc_DeviationFromNomVec.push_back(static_cast<TH1*>(hAccVec[iacc]->Clone("hAcc_DeviationFromNomVec")));// copy
      hAcc_DeviationFromNomVec[iacc]->Add(hAccVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom
      // do the same for lowDphi
      hAcc_DeviationFromNom_lowDphiVec.push_back(static_cast<TH1*>(hAcc_lowDphiVec[iacc]->Clone("hAcc_DeviationFromNom_lowDphiVec")));// copy
      hAcc_DeviationFromNom_lowDphiVec[iacc]->Add(hAcc_lowDphiVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom

    }
    for(int ibin=0; ibin < hAccSysMax->GetNbinsX()+2; ibin++){
      for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
	if(hAccSysMax->GetBinContent(ibin)<hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
	  hAccSysMax->SetBinContent(ibin,hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)); // sqrt[ sum{ (Acc - Acc_nom)^2 } ]
	}
	if(hAccSysMax_lowDphi->GetBinContent(ibin)<hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
	  hAccSysMax_lowDphi->SetBinContent(ibin,hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)); // sqrt[ sum{ (Acc - Acc_nom)^2 } ]
	}
	if(hAccSysMin->GetBinContent(ibin)>hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
	  hAccSysMin->SetBinContent(ibin,hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)); // sqrt[ sum{ (Acc - Acc_nom)^2 } ]
	}
	if(hAccSysMin_lowDphi->GetBinContent(ibin)>hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
	  hAccSysMin_lowDphi->SetBinContent(ibin,hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)); // sqrt[ sum{ (Acc - Acc_nom)^2 } ]
	}
      }
    }
    //////////////
    //////////////

    for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
      sprintf(tempname,"hScaleAccVec_%d",iacc);
      hScaleAccVec.push_back(static_cast<TH1*>(hScaleAccPassVec[iacc]->Clone(tempname)));
      hScaleAccVec[iacc]->Divide(hScaleAccPassVec[iacc],hScaleAccAllVec[iacc],1,1,"B");
      sprintf(tempname,"hScaleAcc_lowDphiVec_%d",iacc);
      hScaleAcc_lowDphiVec.push_back(static_cast<TH1*>(hScaleAccPass_lowDphiVec[iacc]->Clone(tempname)));
      hScaleAcc_lowDphiVec[iacc]->Divide(hScaleAccPass_lowDphiVec[iacc],hScaleAccAll_lowDphiVec[iacc],1,1,"B");
      // calculate the deviation from nominal acceptance
      hScaleAcc_DeviationFromNomVec.push_back(static_cast<TH1*>(hScaleAccVec[iacc]->Clone("hScaleAcc_DeviationFromNomVec")));// copy
      hScaleAcc_DeviationFromNomVec[iacc]->Add(hScaleAccVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom
      // do the same for lowDphi
      hScaleAcc_DeviationFromNom_lowDphiVec.push_back(static_cast<TH1*>(hScaleAcc_lowDphiVec[iacc]->Clone("hScaleAcc_DeviationFromNom_lowDphiVec")));// copy
      hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->Add(hScaleAcc_lowDphiVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom

    }
    for(int ibin=0; ibin < hScaleAccSysMax->GetNbinsX()+2; ibin++){
      for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
	if(hScaleAccSysMax->GetBinContent(ibin)<hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
	  hScaleAccSysMax->SetBinContent(ibin,hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin));// 
	}
	if(hScaleAccSysMax_lowDphi->GetBinContent(ibin)<hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
	  hScaleAccSysMax_lowDphi->SetBinContent(ibin,hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin));
	}
	if(hScaleAccSysMin->GetBinContent(ibin)>hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
	  hScaleAccSysMin->SetBinContent(ibin,hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin));//
	}
	if(hScaleAccSysMin_lowDphi->GetBinContent(ibin)>hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
	  hScaleAccSysMin_lowDphi->SetBinContent(ibin,hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin));
	}
      }
    }

  }

  sprintf(tempname,"%s/LostLepton2_MuonEfficienciesFrom%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile outFile2(tempname,"RECREATE");
  hAcc->Write();
  hAccAll->Write();
  hAccPass->Write();
  hAcc_0b_All->Write();
  hAcc_0b_Pass->Write();
  hAcc_non0b_All->Write();
  hAcc_non0b_Pass->Write();

  hAcc_lowDphi->Write();
  hAccAll_lowDphi->Write();
  hAccPass_lowDphi->Write();
  if(CalcAccSys){
    TDirectory *tdir = outFile2.mkdir("Systematics"); 
    tdir->cd();
    hAccSysMax->Write();
    hAccSysMax_lowDphi->Write();
    hAccSysMin->Write();
    hAccSysMin_lowDphi->Write();
    hScaleAccSysMax->Write();
    hScaleAccSysMax_lowDphi->Write();
    hScaleAccSysMin->Write();
    hScaleAccSysMin_lowDphi->Write();
    TDirectory *tdir2 = outFile2.mkdir("PdfAcc");
    tdir2->cd();
    hPDFWeights->Write();
    hPDFWeights_lowDphi->Write();
    for(int iacc=0; iacc < evt->PDFweights_()->size(); iacc++){
      hAccAllVec[iacc]->Write();
      hAccPassVec[iacc]->Write();
      hAccVec[iacc]->Write();
      hAccAll_lowDphiVec[iacc]->Write();
      hAccPass_lowDphiVec[iacc]->Write();
      hAcc_lowDphiVec[iacc]->Write();

    }
    TDirectory *tdir3 = outFile2.mkdir("ScaleAcc");
    tdir3->cd();
    hScaleWeights->Write();
    hScaleWeights_lowDphi->Write();
    for(int iacc=0; iacc < evt->ScaleWeights_()->size(); iacc++){
      hScaleAccAllVec[iacc]->Write();
      hScaleAccPassVec[iacc]->Write();
      hScaleAccVec[iacc]->Write();
      hScaleAccAll_lowDphiVec[iacc]->Write();
      hScaleAccPass_lowDphiVec[iacc]->Write();
      hScaleAcc_lowDphiVec[iacc]->Write();

    }

  }
  outFile2.Close();

  //open a file to write the histograms
  sprintf(tempname,"%s/GenInfo_HadTauEstimation_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile *resFile = new TFile(tempname, "RECREATE");
  cutflow_preselection->Write();
  searchH->Write();
  QCD_Low->Write();
  searchH_lowDphi->Write();
  QCD_Up->Write();
  searchH_b->Write();
  hPredHTMHT0b->Write();
  hPredHTMHTwb->Write();
  hPredNJetBins->Write();
  hPredNbBins->Write();
  TDirectory *cdtoitt;
  TDirectory *cdtoit;

  // A sanity check:
  // sumofweights for searchH_b and HT
  // after delphi cut should be the same
  int w1 = (int) searchH_b->GetSumOfWeights();
  int w2 = (int) map_map["allEvents"]["delphi"][2].GetSumOfWeights();
  if(w1!=w2){
    for(int j=0;j<11;j++)cout << " Warning! delphi cut is not exactly applied to searchH!\n###\n###\n###\n###\n";
    printf("searchH_b->GetSumOfWeights(): %d ht->GetSumOfWeights(): %d \n ",w1,w2);
  }

  // Loop over different event categories (e.g. "All events, Wlnu, Zll, Zvv, etc")
  for(int iet=0;iet<(int)eventType.size();iet++){
    for(map<string, map<string , vector<TH1D> > >::iterator itt=map_map.begin(); itt!=map_map.end();itt++){
      if (eventType[iet]==itt->first){
	//KH
	////std::cout << (itt->first).c_str() << std::endl;
	cdtoitt = resFile->mkdir((itt->first).c_str());
	cdtoitt->cd();
	for(int i=0; i< (int)sel->cutName().size();i++){
	  for(map<string , vector<TH1D> >::iterator it=itt->second.begin(); it!=itt->second.end();it++){
	    if (sel->cutName()[i]==it->first){
	      cdtoit = cdtoitt->mkdir((it->first).c_str());
	      cdtoit->cd();
	      int nHist = it->second.size();
	      for(int i=0; i<nHist; i++){//since we only have 4 type of histograms
		sprintf(tempname,"%s_%s_%s",it->second[i].GetName(),(it->first).c_str(),(itt->first).c_str());
		it->second[i].Write(tempname);
	      }
	      cdtoitt->cd();
	    }
	  }
	}
      }
    }
  }

  // Calculate tau mistagged(btagged) rate
  TH1D * TauBtaggedRate = static_cast<TH1D*>(B_rate_tagged->Clone("TauBtaggedRate"));
  TauBtaggedRate->Divide(B_rate_tagged,B_rate_all,1,1,"B");
  // Write the histogram 
  sprintf(tempname,"%s/TauBtaggedRate_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile btagfile(tempname,"RECREATE");
  TauBtaggedRate->Write();
  B_rate_tagged->Write();
  B_rate_all->Write();
  btagfile.Close();

  // Calculate the probability of muon coming from Tau
  TH1D * hProb_Tau_mu = static_cast<TH1D*>(hTau_mu->Clone("hProb_Tau_mu"));
  hProb_Tau_mu->Divide(hTau_mu,hW_mu,1,1,"B");
  // Write the histograms
  sprintf(tempname,"%s/Probability_Tau_mu_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile fTau_mu(tempname,"RECREATE");
  hProb_Tau_mu->Write();
  hTau_mu->Write();
  hW_mu->Write();
  fTau_mu.Close();


  // Calculate how often a gen tau does not match a jet (in a hadronic tau event)
  TH1D * FailRate_GenTau_jet = static_cast<TH1D*>(GenTau_Jet_fail->Clone("FailRate_GenTau_jet"));
  FailRate_GenTau_jet->Divide(GenTau_Jet_fail,GenTau_Jet_all,1,1,"B");
  // Write the histograms
  sprintf(tempname,"%s/FailRate_GenTau_jet_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile fgentTau_jet(tempname,"RECREATE");
  FailRate_GenTau_jet->Write();
  GenTau_Jet_fail->Write();
  GenTau_Jet_all->Write();
  fgentTau_jet.Close();



  // Normalize the response distributions to get the probability density
  for(unsigned int i = 0; i < hTauResp.size(); ++i) {
    if( hTauResp.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hTauResp.at(i)->Scale(1./hTauResp.at(i)->Integral("width"));
    }
    if( hTauRespUp.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hTauRespUp.at(i)->Scale(1./hTauRespUp.at(i)->Integral("width"));
    }
    if( hTauRespDown.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hTauRespDown.at(i)->Scale(1./hTauRespDown.at(i)->Integral("width"));
    }
    if( hTauResp_x.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hTauResp_x.at(i)->Scale(1./hTauResp_x.at(i)->Integral("width"));
    }
    if( hTauResp_y.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hTauResp_y.at(i)->Scale(1./hTauResp_y.at(i)->Integral("width"));
    }
    if( hTauResp_xy.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hTauResp_xy.at(i)->Scale(1./hTauResp_xy.at(i)->Integral("width"));
    }
    if( hGenPt.at(i)->Integral("width") > 0. ) {
      // if option "width" is specified, the integral is the sum of the bin contents multiplied by the bin width in x.
      hGenPt.at(i)->Scale(1./hGenPt.at(i)->Integral("width"));
    }
  }
  if( tau_GenPt->Integral("width") > 0. ) 
    tau_GenPt->Scale(1./tau_GenPt->Integral("width"));
  // --- Save the Histograms to File -----------------------------------
  sprintf(tempname,"%s/HadTau_TauResponseTemplates_%s_%s.root",Outdir.c_str(),subSampleKey.c_str(),inputnumber.c_str());
  TFile outFile(tempname,"RECREATE");
  TCanvas *c1 = new TCanvas("c1","TauResponseTemplates",10,10,700,900);
  for(unsigned int i = 0; i < hTauResp.size(); ++i) {
    hTauResp.at(i)->Write();
    hTauResp.at(i)->SetLineColor(i);
    hTauRespUp.at(i)->Write();
    hTauRespUp.at(i)->SetLineColor(i);
    hTauRespDown.at(i)->Write();
    hTauRespDown.at(i)->SetLineColor(i);
    hTauResp_x.at(i)->Write();
    hTauResp_x.at(i)->SetLineColor(i);
    hTauResp_y.at(i)->Write();
    hTauResp_y.at(i)->SetLineColor(i);      
    hTauResp_xy.at(i)->Write();
    hTauResp_xy.at(i)->SetLineColor(i);
    hGenPt.at(i)->Write();
    hGenPt.at(i)->SetLineColor(i);
  }
  tau_GenPt->Write();
  tauJetPtHist->Write();
  tau_GenJetPhi->Write();

}// end of main



