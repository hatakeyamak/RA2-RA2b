// Data-based prediction. In case of W+jets MC, perform closure test

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
#include "TLorentzVector.h"
#include <stdio.h>
#include "TColor.h"
#include "TF1.h"
#include "TLegend.h"
#include "TVector3.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TVector2.h" 
#include "TCanvas.h"

#include "../LostLep/interface/utils2.h"

using namespace std;

//global variables.
vector<double> puWeights_;
const double cutCSVS = 0.814;

static const double pt30Arr[] = { -1, -1, 30, -1 };
static const double pt30Eta24Arr[] = { -1, 2.4, 30, -1 };
static const double pt50Eta24Arr[] = { -1, 2.4, 50, -1 };
static const double pt70Eta24Arr[] = { -1, 2.4, 70, -1 };
static const double dphiArr[] = { -1, 4.7, 30, -1 };
static const double bTagArr[] = { -1, 2.4, 30, -1 };
static const double pt50Arr[] = { -1, -1, 50, -1 };
static const double pt70Arr[] = { -1, -1, 70, -1 };


////Functions and Classses
int countJets(const vector<TLorentzVector> &inputJets, const double *jetCutsArr);
int countJets(const vector<TLorentzVector> &inputJets, const double minAbsEta = -1.0, const double maxAbsEta = -1.0, const double minPt = 30.0, const double maxPt = -1.0);
int countCSVS(const vector<TLorentzVector> &inputJets, const vector<double> &inputCSVS, const double CSVS, const double *jetCutsArr);
int countCSVS(const vector<TLorentzVector> &inputJets, const vector<double> &inputCSVS, const double CSVS = 0.679, const double minAbsEta = -1.0, const double maxAbsEta = -1.0, const double minPt = 30.0, const double maxPt = -1.0);
vector<double> calcDPhi(const vector<TLorentzVector> &inputJets, const double metphi, const int nDPhi, const double *jetCutsArr);
vector<double> calcDPhi(const vector<TLorentzVector> &inputJets, const double metphi, const int nDPhi = 3, const double minAbsEta = -1, const double maxAbsEta = 4.7, const double minPt = 30, const double maxPt = -1);


int initPUinput(const std::string &puDatafileName, const std::string &puDatahistName);

class histClass{
double * a;
TH1D * b_hist;
public:
void fill(int Nhists, double * eveinfarr_, TH1D * hist_){
a = eveinfarr_;
b_hist=hist_;
(*b_hist).Fill(*a);
for(int i=1; i<=Nhists ; i++){

if(i==Nhists)(*(b_hist+i)).Fill(*(a+i));
else (*(b_hist+i)).Fill(*(a+i),*a);

}
}
};

double deltaPhi(double phi1, double phi2) {
return TVector2::Phi_mpi_pi(phi1-phi2);
}

double deltaR(double eta1, double eta2, double phi1, double phi2) {
double dphi = deltaPhi(phi1,phi2);
double deta = eta1 - eta2;
return sqrt( deta*deta + dphi*dphi );
}

void TauResponse_checkPtBin(unsigned int ptBin) {
if( ptBin > 3 ) {
std::cerr << "\n\nERROR in TauResponse: pt bin " << ptBin << " out of binning" << std::endl;
throw std::exception();
}
}

TString TauResponse_name(unsigned int ptBin) {
TauResponse_checkPtBin(ptBin);
TString name = "hTauResp_";
name += ptBin;
return name;
}

unsigned int TauResponse_ptBin(double pt) {
if( pt < 10.) {
std::cerr << "\n\nERROR in TauResponse::ptBin" << std::endl;
std::cerr << " No response available for pt = " << pt << " < " << 10 << std::endl;
throw std::exception();
}

unsigned int bin = 0;
if( pt > 30. ) bin = 1;
if( pt > 50. ) bin = 2;
if( pt > 100. ) bin = 3;
return bin;
}

bool bg_type(string bg_ ,vector<TLorentzVector> * pvec){
if(bg_=="EventsWith_1RecoMuon_0RecoElectron"){return 1;}
} //end of function bg_type

class templatePlotsFunc{///this is the main class

///Some functions

int findMatchedObject(double genTauEta, double genTauPhi,vector<TLorentzVector> vecLvec, double deltaRMax){

int matchedObjIdx=-1;
for(int objIdx = 0; objIdx < (int) vecLvec.size(); ++objIdx){
const double dr = deltaR(genTauEta,vecLvec[objIdx].Eta(),genTauPhi,vecLvec[objIdx].Phi());
if( dr < deltaRMax ){
matchedObjIdx = objIdx;
break;
}
}//end of loop over vec_Jet_30_24_Lvec
return matchedObjIdx;
}

double getRandom(double muPt_){
int bin = TauResponse_ptBin(muPt_);
return vec_resp[bin]->GetRandom();
}

///Some variables
int template_run, template_event, template_lumi, template_nm1, template_n0, template_np1, template_vtxSize;
double template_avg_npv, template_tru_npv;
int template_nJets , nbtag ;
double template_evtWeight;
double template_met, template_metphi;
double template_mht, template_ht, template_mhtphi;
int template_nMuons, template_nElectrons, template_nIsoTrks_CUT,nIsoTrk_,nLeptons;
double dPhi0, dPhi1, dPhi2; /// delta phi of first three jet with respect to MHT?????????
//double delphi12;
char tempname[200];
char histname[200];
vector<TH1D > vec;
map<int, string> cutname;
map<int, string> eventType;
map<string , vector<TH1D> > cut_histvec_map;
map<string, map<string , vector<TH1D> > > map_map;
map<string, histClass> histobjmap;
histClass histObj;
int Nhists,n_elec_mu,n_elec_mu_tot,n_tau_had,n_tau_had_tot,nLostLepton, n_tau_had_tot_fromData;

int loose_nIsoTrks; // number of isolated tracks with Pt>5 GeV and relIso < 0.5
vector<double> *loose_isoTrks_charge; // charge of the loose isolated tracks (see loose_nIsoTrks)
vector<double> *loose_isoTrks_iso; // isolation values (divided by Pt to get relIso) for the loose isolated tracks
vector<int> *loose_isoTrks_pdgId; // pdg id of the loose isolated tracks
vector<TLorentzVector> * template_loose_isoTrksLVec; // TLorentzVector of the loose isolated tracks (see loose_nIsoTrks)
//
int nIsoTrks_CUT; // number of isolated tracks with Pt>10 GeV, dR<0.3, dz<0.05 and relIso<0.1
vector<int> *forVetoIsoTrksidx; // indices of the isolated tracks (see nIsoTrks_CUT) (pointing to pfCandidate collection)
//
vector<double> *trksForIsoVeto_charge; // charges of the charged tracks for isolated veto studies
vector<int> *trksForIsoVeto_pdgId; // pdg id of the charged tracks for isolated veto studies
vector<int> *trksForIsoVeto_idx; // indices of the charged tracks for isolated veto studies (pointing to pfCandidate collection)
//
vector<double> *trksForIsoVeto_dz; // dz of the charged tracks for isolated veto studies
vector<double> *loose_isoTrks_dz; // dz of the loose isolated tracks
vector<double> *loose_isoTrks_mtw; // MT of the loose isolated tracks and MET
vector<int> *loose_isoTrks_idx; // indices of the loose isolated tracks (pointing to pfCandidate collection)
vector<TLorentzVector> *trksForIsoVetoLVec; // TLorentzVector of the charged tracks for isolated veto studies
bool isData;
string keyString;
vector<TLorentzVector> *template_oriJetsVec;
vector<TLorentzVector> *muonsLVec;
vector<double>  *muonsMtw;
vector<double>  *muonsRelIso;
vector<TLorentzVector> *elesLVec;
vector<double>  *elesMtw;
vector<double>  *elesRelIso;
vector<double> *template_recoJetsBtagCSVS;
vector<TLorentzVector> *template_genDecayLVec;
vector<int> *template_genDecayPdgIdVec, *template_genDecayIdxVec, *template_genDecayMomIdxVec;
vector<string>*template_genDecayStrVec,*template_smsModelFileNameStrVec,*template_smsModelStrVec;
double template_smsModelMotherMass, template_smsModelDaughterMass;
vector<TLorentzVector> *template_genJetsLVec_myak5GenJetsNoNu, *template_genJetsLVec_myak5GenJetsNoNuNoStopDecays, *template_genJetsLVec_myak5GenJetsNoNuOnlyStopDecays;
TTree * template_AUX;
ofstream evtlistFile;
double puWeight, totWeight, delphi12, HT;
int cntCSVS, cntNJetsPt30, cntNJetsPt30Eta24, cntNJetsPt50Eta24, cntNJetsPt70Eta24, cntgenTop, cntleptons;
TLorentzVector metLVec;
vector<double> dPhiVec;

vector<TLorentzVector> vec_Jet_30_24_Lvec;
TLorentzVector tempLvec;
int TauResponse_nBins;
vector<TLorentzVector> vec_recoMuonLvec;
vector<TLorentzVector> vec_recoElecLvec;
double muPt;
double muEta;
double muPhi;
double muMtW;
vector<double> vec_recoMuMTW;
vector<TH1*> vec_resp;
double simTauJetPt; 
double simTauJetEta;
double simTauJetPhi;

//define different cuts here
bool ht_500(){if(HT>=500) return true; return false;}
bool ht_500_800(){if(HT>=500 && HT<800) return true; return false;}
bool ht_500_1200(){if(HT>=500 && HT<1200)return true; return false;}
bool ht_800_1200(){if(HT>=800 && HT<1200)return true; return false;}
bool ht_800(){if(HT>=800)return true; return false;}
bool ht_1200(){if(HT>=1200)return true; return false;}
bool mht_200(){if(template_mht>=200)return true; return false;}
bool mht_200_500(){if(template_mht>=200 && template_mht<500)return true; return false;}
bool mht_500_750(){if(template_mht>=500 && template_mht<750)return true; return false;}
bool mht_750(){if(template_mht>=750)return true; return false;}
bool dphi(){if(dPhi0>0.5 && dPhi1>0.3 && dPhi2>0.3)return true; return false;}
bool Njet_4(){if(cntNJetsPt30Eta24 >= 4)return true; return false;}
bool Njet_4_6(){if(cntNJetsPt30Eta24 >= 4 && cntNJetsPt30Eta24 <= 6)return true; return false;}
bool Njet_7_8(){if(cntNJetsPt30Eta24 >= 7 && cntNJetsPt30Eta24 <= 8)return true; return false;}
bool Njet_9(){if(cntNJetsPt30Eta24 >= 9)return true; return false;}
bool btag_0(){if(nbtag == 0)return true; return false;}
bool btag_1(){if(nbtag == 1)return true; return false;}
bool btag_2(){if(nbtag == 2)return true; return false;}
bool btag_3(){if(nbtag >= 3)return true; return false;}
bool isoTrk(){if(nIsoTrk_ ==0)return true; return false;}

///apply the cuts here
bool checkcut(string ss){
if(ss == cutname[0])return true;

if(ss== cutname[1]){if(Njet_4())return true;}
if(ss== cutname[2]){if(Njet_4() && ht_500())return true;}
if(ss== cutname[3]){if(Njet_4()&&ht_500()&&mht_200())return true;}
if(ss== cutname[4]){if(Njet_4()&&ht_500()&&mht_200()&&dphi())return true;}
if(ss== cutname[5]){if(Njet_4()&&ht_500()&&mht_200()&&dphi()&&isoTrk())return true;}

if(ss== cutname[6]){if(Njet_4()&&ht_500()&&mht_200()&&dphi()&&btag_0())return true;}
if(ss== cutname[7]){if(Njet_4()&&ht_500()&&mht_200()&&dphi()&&btag_1())return true;}
if(ss== cutname[8]){if(Njet_4()&&ht_500()&&mht_200()&&dphi()&&btag_2())return true;}
if(ss== cutname[9]){if(Njet_4()&&ht_500()&&mht_200()&&dphi()&&btag_3())return true;}

return false;
}


public:
//constructor
templatePlotsFunc(TTree * ttree_, const std::string sampleKeyString="ttbar", int verbose=0, string Outdir="Results", string inputnumber="00"){

TauResponse_nBins=4;

//build a vector of histograms
TH1D weight_hist = TH1D("weight", "Weight Distribution", 5,0,5);
vec.push_back(weight_hist);
TH1D RA2HT_hist = TH1D("HT","HT Distribution",50,0,5000);
RA2HT_hist.Sumw2();
vec.push_back(RA2HT_hist);
TH1D RA2MHT_hist = TH1D("MHT","MHT Distribution",100,0,5000);
RA2MHT_hist.Sumw2();
vec.push_back(RA2MHT_hist);
TH1D RA2NJet_hist = TH1D("NJet","Number of Jets Distribution",20,0,20);
RA2NJet_hist.Sumw2();
vec.push_back(RA2NJet_hist);
TH1D RA2NBtag_hist = TH1D("NBtag","Number of Btag Distribution",20,0,20);
RA2NBtag_hist.Sumw2();
vec.push_back(RA2NBtag_hist);
TH1D RA2MuonPt_hist = TH1D("MuonPt","Pt of muon Distribution",80,0,400);
RA2MuonPt_hist.Sumw2();
vec.push_back(RA2MuonPt_hist);
TH1D simTauJetPt_hist = TH1D("simTauJetPt","Pt of simulated tau Jet",80,0,400);
simTauJetPt_hist.Sumw2();
vec.push_back(simTauJetPt_hist);
TH1D RA2MtW_hist = TH1D("MtW","Mt of W Distribution",10,0,120);
RA2MtW_hist.Sumw2();
vec.push_back(RA2MtW_hist);
TH1D Bjet_mu_hist = TH1D("Bjet_mu_hist","Is Muon from Bjet? ",2,0,2);
Bjet_mu_hist.Sumw2();
vec.push_back(Bjet_mu_hist);

Nhists=((int)(vec.size())-1);//-1 is because weight shouldn't be counted.

//initialize a map between string=cutnames and histvecs. copy one histvec into all of them. The histograms, though, will be filled differently.
cutname[0]="nocut";cutname[1]="Njet_4";cutname[2]="ht_500" ;cutname[3]="mht_200";
cutname[4]="delphi";cutname[5]="iso";
/////////////////////////////////////////////////////////////////////////////////
cutname[6]="CSVM_0";
cutname[7]="CSVM_1";
cutname[8]="CSVM_2";
cutname[9]="CSVM_3";
//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
for(int i=0; i<(int) cutname.size();i++){
cut_histvec_map[cutname[i]]=vec;
}

////////////////////////////////////////////////////////////////////////////////////////
eventType[0]="EventsWith_1RecoMuon_0RecoElectron";

//initialize a map between string and maps. copy the map of histvecs into each
for(int i=0; i< eventType.size();i++){
map_map[eventType[i]]=cut_histvec_map;
}

//initialize histobjmap
for(map<string , vector<TH1D> >::iterator it=cut_histvec_map.begin(); it!=cut_histvec_map.end();it++){
histobjmap[it->first]=histObj;
}



////Open some files and get the histograms ........................................//

  // Probability of muon coming from Tau
  TFile * Prob_Tau_mu_file = new TFile("../TauHad/Probability_Tau_mu_stacked.root","R");
  sprintf(histname,"hProb_Tau_mu");
  TH1D * hProb_Tau_mu =(TH1D *) Prob_Tau_mu_file->Get(histname)->Clone();

  // Acceptance and efficiencies 
  TFile * MuEffAcc_file = new TFile("../LostLep/LostLepton2_MuonEfficiencies_stacked.root","R");
  sprintf(histname,"hAcc");
  TH1D * hAcc =(TH1D *) MuEffAcc_file->Get(histname)->Clone();
  TH1D * hEff =(TH1D *) MuEffAcc_file->Get("hEff")->Clone();
  // get the map between the strings repressenting the bins and the bin numbers
//  map<string,int> binMap = utils2::BinMap();
  map<string,int> binMap = utils2::BinMap_NoB();

TFile * resp_file = new TFile("../TauHad/HadTau_TauResponseTemplates_stacked.root","R");
for(int i=0; i<TauResponse_nBins; i++){
sprintf(histname,"hTauResp_%d",i);
vec_resp.push_back( (TH1D*) resp_file->Get( histname )->Clone() );
}

const bool isMC = true;

// --- Declare the Output Histograms ---------------------------------
const TString title = isMC ? "Hadronic-Tau Closure Test" : "Hadronic-Tau Prediction";

/*
 // Control plot: muon pt in the control sample
TH1* hMuonPt = new TH1D("hMuonPt",title+";p_{T}(#mu^{gen}) [GeV];N(events)",50,0.,500.);
hMuonPt->Sumw2();

 // Predicted distributions
TH1* hPredHt = new TH1D("hPredHt",title+";H_{T} [GeV];N(events)",25,500.,3000.);
hPredHt->Sumw2();
TH1* hPredMht = new TH1D("hPredMht",title+";#slash{H}_{T} [GeV];N(events)",20,200.,1200.);
hPredMht->Sumw2();
TH1* hPredNJets = new TH1D("hPredNJets",title+";N(jets);N(events)",9,3,12);
hPredNJets->Sumw2();
// In case of MC: true distributions
TH1* hTrueHt = static_cast<TH1*>(hPredHt->Clone("hTrueHt")); /// here "hTrueHt" is the new name. 
TH1* hTrueMht = static_cast<TH1*>(hPredMht->Clone("hTrueMht"));
TH1* hTrueNJets = static_cast<TH1*>(hPredNJets->Clone("hTrueNJets"));

 // Event yields in the different RA2 search bins
// First bin (around 0) is baseline selection
TH1* hPredYields = new TH1D("hPredYields",title+";;N(events)",37,-0.5,36.5);
hPredYields->Sumw2();
hPredYields->GetXaxis()->SetBinLabel(1,"baseline");
for(int bin = 2; bin <= hPredYields->GetNbinsX(); ++bin) {
TString label = "Bin ";
label += bin-1;
hPredYields->GetXaxis()->SetBinLabel(bin,label);
}
TH1* hTrueYields = static_cast<TH1*>(hPredYields->Clone("hTrueYields"));
*/



/////////////////////////////////////////////
isData = false;
keyString = sampleKeyString;
TString keyStringT(keyString);
//KH---begins - prepares a few vectors
template_loose_isoTrksLVec = new vector<TLorentzVector>();
muonsLVec  = new vector<TLorentzVector>();
muonsMtw = 0;
muonsRelIso = 0;
elesLVec  = new vector<TLorentzVector>();
elesMtw = 0;
elesRelIso = 0;
loose_isoTrks_iso = new vector<double>();
loose_isoTrks_mtw = new vector<double>();
//KH---ends
template_oriJetsVec = new vector<TLorentzVector>();template_recoJetsBtagCSVS = new vector<double>();
template_genDecayLVec =0;
template_genDecayPdgIdVec =0;template_genDecayIdxVec =0;template_genDecayMomIdxVec =0;
vector<string> *template_genDecayStrVec =0, *template_smsModelFileNameStrVec =0, *template_smsModelStrVec =0;
template_genJetsLVec_myak5GenJetsNoNu =0; template_genJetsLVec_myak5GenJetsNoNuNoStopDecays =0; template_genJetsLVec_myak5GenJetsNoNuOnlyStopDecays =0;
template_AUX = ttree_;
template_AUX->SetBranchStatus("*", 0);

///the variables

template_AUX->SetBranchStatus("run", 1); template_AUX->SetBranchAddress("run", &template_run);
template_AUX->SetBranchStatus("event", 1); template_AUX->SetBranchAddress("event", &template_event);
template_AUX->SetBranchStatus("lumi", 1); template_AUX->SetBranchAddress("lumi", &template_lumi);
template_AUX->SetBranchStatus("avg_npv", 1); template_AUX->SetBranchAddress("avg_npv", &template_avg_npv);
template_AUX->SetBranchStatus("nm1", 1); template_AUX->SetBranchAddress("nm1", &template_nm1);
template_AUX->SetBranchStatus("n0", 1); template_AUX->SetBranchAddress("n0", &template_n0);
template_AUX->SetBranchStatus("np1", 1); template_AUX->SetBranchAddress("np1", &template_np1);
template_AUX->SetBranchStatus("tru_npv", 1); template_AUX->SetBranchAddress("tru_npv", &template_tru_npv);
template_AUX->SetBranchStatus("vtxSize", 1); template_AUX->SetBranchAddress("vtxSize", &template_vtxSize);
template_AUX->SetBranchStatus("nJets", 1); template_AUX->SetBranchAddress("nJets", &template_nJets);
template_AUX->SetBranchStatus("jetsLVec", 1); template_AUX->SetBranchAddress("jetsLVec", &template_oriJetsVec);
template_AUX->SetBranchStatus("recoJetsBtag_0", 1); template_AUX->SetBranchAddress("recoJetsBtag_0", &template_recoJetsBtagCSVS);
template_AUX->SetBranchStatus("evtWeight", 1); template_AUX->SetBranchAddress("evtWeight", &template_evtWeight);
template_AUX->SetBranchStatus("met", 1); template_AUX->SetBranchAddress("met", &template_met);
template_AUX->SetBranchStatus("nIsoTrks_CUT",1); template_AUX->SetBranchAddress("nIsoTrks_CUT", &template_nIsoTrks_CUT);
template_AUX->SetBranchStatus("metphi", 1); template_AUX->SetBranchAddress("metphi", &template_metphi);
template_AUX->SetBranchStatus("mhtphi", 1); template_AUX->SetBranchAddress("mhtphi", &template_mhtphi);
template_AUX->SetBranchStatus("nMuons_CUT", 1); template_AUX->SetBranchAddress("nMuons_CUT", &template_nMuons);
template_AUX->SetBranchStatus("nElectrons_CUT", 1); template_AUX->SetBranchAddress("nElectrons_CUT", &template_nElectrons);
template_AUX->SetBranchStatus("genDecayLVec", 1); template_AUX->SetBranchAddress("genDecayLVec", &template_genDecayLVec);
template_AUX->SetBranchStatus("genDecayPdgIdVec", 1); template_AUX->SetBranchAddress("genDecayPdgIdVec", &template_genDecayPdgIdVec);
template_AUX->SetBranchStatus("genDecayIdxVec", 1); template_AUX->SetBranchAddress("genDecayIdxVec", &template_genDecayIdxVec);
template_AUX->SetBranchStatus("genDecayMomIdxVec", 1); template_AUX->SetBranchAddress("genDecayMomIdxVec", &template_genDecayMomIdxVec);
template_AUX->SetBranchStatus("genDecayStrVec", 1); template_AUX->SetBranchAddress("genDecayStrVec", &template_genDecayStrVec);
template_AUX->SetBranchStatus("ht", 1); template_AUX->SetBranchAddress("ht", &template_ht);
template_AUX->SetBranchStatus("mht", 1); template_AUX->SetBranchAddress("mht", &template_mht);
template_AUX->SetBranchStatus("loose_isoTrksLVec","1");template_AUX->SetBranchAddress("loose_isoTrksLVec", &template_loose_isoTrksLVec);
template_AUX->SetBranchStatus("loose_isoTrks_iso","1");template_AUX->SetBranchAddress("loose_isoTrks_iso", &loose_isoTrks_iso);
template_AUX->SetBranchStatus("loose_isoTrks_mtw", "1");template_AUX->SetBranchAddress("loose_isoTrks_mtw", &loose_isoTrks_mtw);
template_AUX->SetBranchStatus("loose_nIsoTrks", "1");template_AUX->SetBranchAddress("loose_nIsoTrks", &loose_nIsoTrks);

template_AUX->SetBranchStatus("muonsLVec","1");template_AUX->SetBranchAddress("muonsLVec", &muonsLVec);
template_AUX->SetBranchStatus("muonsMtw","1");template_AUX->SetBranchAddress("muonsMtw", &muonsMtw);
template_AUX->SetBranchStatus("muonsRelIso","1");template_AUX->SetBranchAddress("muonsRelIso", &muonsRelIso);
template_AUX->SetBranchStatus("elesLVec","1");template_AUX->SetBranchAddress("elesLVec", &elesLVec);
template_AUX->SetBranchStatus("elesMtw","1");template_AUX->SetBranchAddress("elesMtw", &elesMtw);
template_AUX->SetBranchStatus("elesRelIso","1");template_AUX->SetBranchAddress("elesRelIso", &elesRelIso);



//template_AUX->SetBranchStatus("nIsoTrks_CUT", "1");template_AUX->SetBranchAddress("nIsoTrks_CUT", &nIsoTrks_CUT);
//template_AUX->SetBranchStatus("trksForIsoVeto_charge","1");template_AUX->SetBranchAddress("trksForIsoVeto_charge", &trksForIsoVeto_charge);
//template_AUX->SetBranchStatus("trksForIsoVeto_dz","1");template_AUX->SetBranchAddress("trksForIsoVeto_dz", &trksForIsoVeto_dz);
//template_AUX->SetBranchStatus("loose_isoTrks_charge","1");template_AUX->SetBranchAddress("loose_isoTrks_charge", &loose_isoTrks_charge);
//template_AUX->SetBranchStatus("loose_isoTrks_dz","1");template_AUX->SetBranchAddress("loose_isoTrks_dz", &loose_isoTrks_dz);
//####template_AUX->SetBranchStatus("trksForIsoVeto_pdgId","1");template_AUX->SetBranchAddress("trksForIsoVeto_pdgId", &trksForIsoVeto_pdgId);
//template_AUX->SetBranchStatus("trksForIsoVeto_idx","1");template_AUX->SetBranchAddress("trksForIsoVeto_idx", &trksForIsoVeto_idx);
//####template_AUX->SetBranchStatus("loose_isoTrks_pdgId","1");template_AUX->SetBranchAddress("loose_isoTrks_pdgId", &loose_isoTrks_pdgId);
//template_AUX->SetBranchStatus("loose_isoTrks_idx","1");template_AUX->SetBranchAddress("loose_isoTrks_idx", &loose_isoTrks_idx);
//####template_AUX->SetBranchStatus("forVetoIsoTrksidx","1");template_AUX->SetBranchAddress("forVetoIsoTrksidx", &forVetoIsoTrksidx);
//template_AUX->SetBranchStatus("trksForIsoVetoLVec","1");template_AUX->SetBranchAddress("trksForIsoVetoLVec", &trksForIsoVetoLVec);

int template_Entries = template_AUX->GetEntries();
cout<<"\n\n"<<keyString.c_str()<<"_Entries : "<<template_Entries<<endl;

if( keyStringT.Contains("Data") ) evtlistFile.open("evtlistData_aftAllCuts.txt");

n_elec_mu_tot=0;
n_tau_had_tot=0;
n_tau_had_tot_fromData=0;

  printf("Note that the last histogram in each subdirectory will be written withoug weight. \n This histogram shows the number of times muon is from Bjet.  \n ");

////Loop over all events
for(int ie=0; ie<template_Entries; ie++){

//////////////
//Temporary
//int ie;
//for(int is=0; is < 26; is++){
//ie=spike[is];
/////////////

template_AUX->GetEntry(ie);


//A counter
if(ie % 10000 ==0 )printf("-------------------- %d \n",ie);

//if(ie>100000)break;

puWeight = 1.0;
if( !keyStringT.Contains("Signal") && !keyStringT.Contains("Data") ){
// puWeight = weightTruNPV(NumPUInteractions);
}

if( template_oriJetsVec->size() != template_recoJetsBtagCSVS->size() ){
std::cout<<"template_oriJetsVec->size : "<<template_oriJetsVec->size()<<" template_recoJetsBtagCSVS : "<<template_recoJetsBtagCSVS->size()<<std::endl;
}

metLVec.SetPtEtaPhiM(template_met, 0, template_metphi, 0);
if(template_oriJetsVec->size()==0)cntCSVS=0;else cntCSVS = countCSVS((*template_oriJetsVec), (*template_recoJetsBtagCSVS), cutCSVS, bTagArr);
if(template_oriJetsVec->size()==0)cntNJetsPt30=0;else cntNJetsPt30 = countJets((*template_oriJetsVec), pt30Arr);
if(template_oriJetsVec->size()==0)cntNJetsPt30Eta24=0;else cntNJetsPt30Eta24 = countJets((*template_oriJetsVec), pt30Eta24Arr);
if(template_oriJetsVec->size()==0)cntNJetsPt50Eta24=0;else cntNJetsPt50Eta24 = countJets((*template_oriJetsVec), pt50Eta24Arr);
if(template_oriJetsVec->size()==0)cntNJetsPt70Eta24=0;else cntNJetsPt70Eta24 = countJets((*template_oriJetsVec), pt70Eta24Arr);
if(template_oriJetsVec->size()==0){
dPhi0=-99.; dPhi1 =-99.;dPhi2 =-99.;delphi12=-99.;
}
else{ 
dPhiVec = calcDPhi((*template_oriJetsVec), template_mhtphi, 3, dphiArr);
//dPhiVec = calcDPhi((*template_oriJetsVec), template_metphi, 3, dphiArr);
dPhi0 = dPhiVec[0]; dPhi1 = dPhiVec[1]; dPhi2 = dPhiVec[2];
if(template_oriJetsVec->size() > 1)delphi12= fabs(template_oriJetsVec->at(0).Phi()-template_oriJetsVec->at(1).Phi());else delphi12=-99.;
}

if(verbose!=0)printf("\n############ \n event: %d \n ",ie);

///HT calculation. This is because the predefined HT,template_ht, is calculated with jets with pt>50 and eta>2.5. 
HT=0;
vec_Jet_30_24_Lvec.clear();
for(int i=0; i< template_oriJetsVec->size();i++){
double pt=template_oriJetsVec->at(i).Pt();
double eta=template_oriJetsVec->at(i).Eta();
double phi=template_oriJetsVec->at(i).Phi();
double e=template_oriJetsVec->at(i).E();
if(verbose!=0)printf(" \n Jets: \n pt: %g eta: %g phi: %g \n ",pt,eta,phi);
if(pt>30. && fabs(eta)<2.4){
HT+=pt;
tempLvec.SetPtEtaPhiE(pt,eta,phi,e);
vec_Jet_30_24_Lvec.push_back(tempLvec); // this vector contains the lorentz vector of reconstructed jets with pt>30 and eta< 2.4
}
}

// Parsing the gen information ...
cntgenTop = 0, cntleptons =0;
for(int iv=0; iv<(int)template_genDecayLVec->size(); iv++){
int pdgId = template_genDecayPdgIdVec->at(iv);
if( abs(pdgId) == 6 ) cntgenTop ++;
if( abs(pdgId) == 11 || abs(pdgId) == 13 || abs(pdgId) == 15 ) cntleptons++;
}
if( verbose >=1 ){
std::cout<<"\nie : "<<ie<<std::endl;
std::cout<<"genDecayStr : "<<template_genDecayStrVec->front().c_str()<<std::endl;
printf("((pdgId,index/MomIndex):(E/Pt)) \n");
for(int iv=0; iv<(int)template_genDecayLVec->size(); iv++){
int pdgId = template_genDecayPdgIdVec->at(iv);
printf("((%d,%d/%d):(%6.2f/%6.2f)) ", pdgId, template_genDecayIdxVec->at(iv), template_genDecayMomIdxVec->at(iv), template_genDecayLVec->at(iv).E(), template_genDecayLVec->at(iv).Pt());
}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////Isolated track section
/*if(ie<100){
 * cout << "event#: " << ie << endl;
 * printf("loose_nIsoTrks: %d, nIsoTrks_CUT: %d, trksForIsoVeto_charge.size(): %d, loose_isoTrks_charge.size(): %d, loose_isoTrks_iso.size(): %d, trksForIsoVeto_pdgId->size(): %d, loose_isoTrks_pdgId->size(): %d, forVetoIsoTrksidx->size(): %d, loose_isoTrksLVec->size(): %d \n",loose_nIsoTrks,nIsoTrks_CUT,trksForIsoVeto_charge->size(),loose_isoTrks_charge->size(),loose_isoTrks_iso->size(),trksForIsoVeto_pdgId->size(),loose_isoTrks_pdgId->size(), forVetoIsoTrksidx->size(),template_loose_isoTrksLVec->size());
 * }
 * */

///In this part we would like to identify lost leptons and hadronic taus. To do so we use the generator truth information. We first check how many leptons(e and mu) are in the event and compare with the isolated+reconstructed ones.
n_elec_mu=0;
for(int iv=0; iv<(int)template_genDecayLVec->size(); iv++){
int pdgId = template_genDecayPdgIdVec->at(iv);
if( abs(pdgId) == 11 || abs(pdgId) == 13 ) n_elec_mu++;
}
n_elec_mu_tot+=n_elec_mu;
/*if(ie < 100){
 * printf("event#: %d, #recElec: %d, #recMu: %d, #trueElecMu: %d \n", ie , template_nElectrons , template_nMuons , n_elec_mu);
 * }*/


n_tau_had=0;
for(int iv=0; iv<(int)template_genDecayLVec->size(); iv++){
int pdgId = template_genDecayPdgIdVec->at(iv);
if( abs(pdgId) == 15 ){
int index=template_genDecayIdxVec->at(iv);
for(int ivv=0; ivv<(int)template_genDecayLVec->size(); ivv++){
int secpdg = template_genDecayPdgIdVec->at(ivv);
int MomIndex=template_genDecayMomIdxVec->at(ivv);
if(MomIndex==index && abs(secpdg) > 40){ ///pdgID of hadrons are higher than 40. 
//printf("This is a tau. TauIndex: %d, TauDaughterID: %d \n",MomIndex, secpdg);
n_tau_had++;
}
}
}
}

n_tau_had_tot+=n_tau_had;


/////////////////////////////////////////////////////////////////////////////////////
 // Select the control sample:
// - select events with exactly one well-reconstructed, isolated muon
// Use the muon to predict the energy deposits of the
// hadronically decaying tau:
// - scale the muon pt by a random factor drawn from the
// tau-reponse template to simulate the tau measurement
// - use the simulated tau-pt to predict HT, MHT, and N(jets)

///select muons with pt>20. eta<2.1 relIso<.2
vec_recoMuMTW.clear();
vec_recoMuonLvec.clear();
for(int i=0; i< muonsLVec->size(); i++){ 
double pt=muonsLVec->at(i).Pt();
double eta=muonsLVec->at(i).Eta();
double phi=muonsLVec->at(i).Phi();
double e=muonsLVec->at(i).E();
double relIso=muonsRelIso->at(i);
double mu_mt_w =muonsMtw->at(i); 
if(pt>20. && fabs(eta)< 2.1 && relIso < 0.2 && mu_mt_w < 100.){
if(verbose!=0)printf(" \n Muons: \n pt: %g eta: %g phi: %g \n ",pt,eta,phi);
tempLvec.SetPtEtaPhiE(pt,eta,phi,e);
vec_recoMuonLvec.push_back(tempLvec);
vec_recoMuMTW.push_back(mu_mt_w);
}
}

///select eles with pt>10. eta<2.5 relIso<.2
vec_recoElecLvec.clear();
for(int i=0; i< elesLVec->size(); i++){
double pt=elesLVec->at(i).Pt();
double eta=elesLVec->at(i).Eta();
double phi=elesLVec->at(i).Phi();
double e=elesLVec->at(i).E();
double relIso=elesRelIso->at(i);
if(pt>10. && fabs(eta)<2.5/* && relIso < 0.2*/){
tempLvec.SetPtEtaPhiE(pt,eta,phi,e);
vec_recoElecLvec.push_back(tempLvec);
}
}

if(verbose!=0)printf(" \n **************************************** \n #Muons: %d #Electrons: %d \n  ****************************** \n ",vec_recoMuonLvec.size(),vec_recoElecLvec.size());

//printf("@@@@@@@@@@@@@@@@@@@@@@@@ \n eventN: %d \n ",ie);

//printf(" \n **************************************** \n #Muons: %d #Electrons: %d \n  ****************************** \n ",vec_recoMuonLvec.size(),vec_recoElecLvec.size());

//if( template_nMuons == 1 && template_nElectrons == 0 ) {
if( vec_recoMuonLvec.size() == 1 && vec_recoElecLvec.size() == 0 ){
muPt =  vec_recoMuonLvec[0].Pt();
muEta = vec_recoMuonLvec[0].Eta();
muPhi = vec_recoMuonLvec[0].Phi();
muMtW = vec_recoMuMTW[0];

//printf("muPt: %g muEta: %g muPhi: %g \n ",muPt,muEta,muPhi);

// Get random number from tau-response template
// The template is chosen according to the muon pt
const double scale = getRandom(muPt);

simTauJetPt = scale * muPt;
simTauJetEta = muEta;
simTauJetPhi = muPhi;

printf("simTauJetPt: %g simTauJetEta: %g simTauJetPhi: %g \n ",simTauJetPt,simTauJetEta,simTauJetPhi);


///The muon we are using is already part of a jet. (Note: the muon is isolated by 0.2 but jet is much wider.) And, its momentum is used in HT and MHT calculation. We need to subtract this momentum and add the contribution from the simulated tau jet. 

  //Identify the jet containing the muon
  const double deltaRMax = muPt < 50. ? 0.2 : 0.1; // Increase deltaRMax at low pt to maintain high-enought matching efficiency
  int JetIdx=findMatchedObject(muEta,muPhi,* template_oriJetsVec,deltaRMax);

  if(verbose!=0){printf(" \n **************************************** \n JetIdx: %d \n CSVS: %g. It is B if larger than %g \n ",JetIdx,template_recoJetsBtagCSVS->at(JetIdx),cutCSVS);
  }


  // See if muon is coming from Bjet
  // This determines how often muon comes from a Bjet
  int num_NonW_mu=0;
  if(JetIdx!=-1 && template_recoJetsBtagCSVS->at(JetIdx) > cutCSVS )num_NonW_mu=1;

//New HT:
HT=HT+simTauJetPt-muPt;

//New MHT
double mhtX = template_mht*cos(template_mhtphi)-(simTauJetPt-muPt)*cos(simTauJetPhi);///the minus sign is because of Mht definition.
double mhtY = template_mht*sin(template_mhtphi)-(simTauJetPt-muPt)*sin(simTauJetPhi);

//printf("############ \n  mhtX: %g, mhtY: %g \n",mhtX,mhtY);
//printf("template_mht: %g, template_mhtphi: %g, simTauJetPt: %g, simTauJetPhi: %g \n",template_mht,template_mhtphi,simTauJetPt,simTauJetPhi);

template_mht = sqrt(pow(mhtX,2)+pow(mhtY,2));
if(mhtX>0)template_mhtphi = atan(mhtY/mhtX);
else{
if(mhtY>0) template_mhtphi = 3.14+atan(mhtY/mhtX);
else template_mhtphi = -3.14+atan(mhtY/mhtX);
}

//printf("HT: %g template_mht: %g, template_mhtphi: %g \n ",HT,template_mht,template_mhtphi);

//add the simTau Jet to the list if it satisfy the conditions
if(fabs(simTauJetEta)<2.4 && (simTauJetPt-muPt)>30.)cntNJetsPt30Eta24+=1;


n_tau_had_tot_fromData+=1;




/////////////////////////////////////////////////////////////////////////////////////

nbtag=0;
//Number of B-jets
for(int i=0; i<template_recoJetsBtagCSVS->size();i++){
double pt=template_oriJetsVec->at(i).Pt();
double eta=template_oriJetsVec->at(i).Eta();
if(template_recoJetsBtagCSVS->at(i) > cutCSVS && pt > 30 && fabs(eta)<2.4 )nbtag+=1;
}//end of the loop
nLeptons= (int)(template_nElectrons+template_nMuons);

  // get the effieciencies and acceptance
    // if baseline cuts on the main variables are passed then calculate the efficiencies otherwise simply take 0.75 as the efficiency.
    double Eff;
    if(cntNJetsPt30Eta24>=4 && HT >= 500 && template_mht >= 200){
//      Eff = hEff->GetBinContent(binMap[utils2::findBin(cntNJetsPt30Eta24,nbtag,HT,template_mht)]); 
      Eff = hEff->GetBinContent(binMap[utils2::findBin_NoB(cntNJetsPt30Eta24,HT,template_mht)]);
    }else{
      Eff=0.75;
    }

    // if baseline cuts on the main variables are passed then calculate the acceptance otherwise simply take 0.9 as the acceptance.
    double Acc;
    if(cntNJetsPt30Eta24>=4 && HT >= 500 && template_mht >= 200){
//      Acc = hAcc->GetBinContent(binMap[utils2::findBin(cntNJetsPt30Eta24,nbtag,HT,template_mht)]);
      Acc = hAcc->GetBinContent(binMap[utils2::findBin_NoB(cntNJetsPt30Eta24,HT,template_mht)]);
    }else{
      Acc=0.9;
    }

    if(verbose!=0)printf("Eff: %g Acc: %g njet: %d nbtag: %d ht: %g mht: %g binN: %d \n ",Eff,Acc, cntNJetsPt30Eta24,nbtag,HT,template_mht,                                              binMap[utils2::findBin_NoB(cntNJetsPt30Eta24,HT,template_mht)]);  

if(Acc==0 || Eff==0){printf("ie: %d Acc or Eff =0 \n Eff: %g Acc: %g njet: %d nbtag: %d ht: %g mht: %g \n ",ie,Eff,Acc, cntNJetsPt30Eta24,nbtag,HT,template_mht);}
if(Acc==0)Acc=0.9;
if(Eff==0)Eff=0.75;

  // Not all the muons are coming from W. Some of them are coming from Tau which should not be considered in our estimation. 
  double Prob_Tau_mu = hProb_Tau_mu->GetBinContent(hProb_Tau_mu->GetXaxis()->FindBin(muPt));  

totWeight=template_evtWeight*puWeight*0.64*(1/(Acc*Eff))*(1-Prob_Tau_mu);//the 0.56 is because only 56% of tau's decay hadronically. Here 0.9 is acceptance and 0.75 is efficiencies of both reconstruction and isolation. 

//build and array that contains the quantities we need a histogram for. Here order is important and must be the same as RA2nocutvec
  
  double eveinfvec[9];

  if(num_NonW_mu==0){
    eveinfvec = {totWeight, HT, template_mht ,(double) cntNJetsPt30Eta24,(double) nbtag,(double) muPt, simTauJetPt, (double) muMtW                                  , (double)num_NonW_mu};
  }else if(num_NonW_mu==1){
    // We don't want Bjet_Muon in our control sample.
    eveinfvec = {totWeight, -99., -99., -99., -99., -99., -99., -99., (double)num_NonW_mu};   
  }

//loop over all the different backgrounds: "allEvents", "Wlv", "Zvv"
for(map<string, map<string , vector<TH1D> > >::iterator itt=map_map.begin(); itt!=map_map.end();itt++){//this will be terminated after the cuts

////determine what type of background should pass
if(bg_type(itt->first , template_genDecayLVec)==true){//all the cuts are inside this
//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts//Cuts

//////loop over cut names and fill the histograms
for(map<string , vector<TH1D> >::iterator ite=cut_histvec_map.begin(); ite!=cut_histvec_map.end();ite++){

  if(checkcut(ite->first)==true){histobjmap[ite->first].fill(Nhists,&eveinfvec[0] ,&itt->second[ite->first][0]);}

}//end of loop over cut names

////EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts//EndOfCuts

}//end of bg_type determination

}//end of loop over all the different backgrounds: "allEvents", "Wlv", "Zvv"


/// nIsoTrk_ calculation.
//printf("\n  loose_isoTrksLVec->size(): %d ,loose_isoTrks_mtw->size(): %d ,loose_isoTrks_iso->size(): %d ",template_loose_isoTrksLVec->size(),loose_isoTrks_mtw->size(),loose_isoTrks_iso->size());
nIsoTrk_ =0;
for(int i=0;i< (int) template_loose_isoTrksLVec->size();i++){
double pt = template_loose_isoTrksLVec->at(i).Pt();
double eta = fabs(template_loose_isoTrksLVec->at(i).Eta());
double reliso = loose_isoTrks_iso->at(i);
double mt_w = loose_isoTrks_mtw->at(i);//This is transverse mass of track and missing Et. 
if(pt > 15 && eta < 2.4 && reliso < 0.1 && mt_w < 100)nIsoTrk_++;
}
//cout << "nIso: " << nIsoTrk_ << endl;

} // End if exactly one muon
}////end of loop over all events


//open a file to write the histograms
sprintf(tempname,"%s/HadTauEstimation_%s_%s.root",Outdir.c_str(),sampleKeyString.c_str(),inputnumber.c_str());
TFile *resFile = new TFile(tempname, "RECREATE");
TDirectory *cdtoitt;
TDirectory *cdtoit;
// Loop over different event categories (e.g. "All events, Wlnu, Zll, Zvv, etc")
for(int iet=0;iet<(int)eventType.size();iet++){
for(map<string, map<string , vector<TH1D> > >::iterator itt=map_map.begin(); itt!=map_map.end();itt++){
if (eventType[iet]==itt->first){
//KH
////std::cout << (itt->first).c_str() << std::endl;
cdtoitt = resFile->mkdir((itt->first).c_str());
cdtoitt->cd();
for(int i=0; i< (int)cutname.size();i++){
for(map<string , vector<TH1D> >::iterator it=itt->second.begin(); it!=itt->second.end();it++){
if (cutname[i]==it->first){
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



delete resp_file;

}//end of class constructor templatePlotsFunc
};//end of class templatePlotsFunc



int main(int argc, char *argv[]){

/////////////////////////////////////
if (argc != 6)
{
std::cout << "Please enter something like:  ./main \"filelist_WJets_PU20bx25_100_200.txt\" \"WJets_PU20bx25_100_200\" \"Results\" \"00\" \"0\" " << std::endl;
return EXIT_FAILURE;
}
const string InRootList = argv[1];
const string subSampleKey = argv[2];
const string OutDir = argv[3];
const string inputNum = argv[4];
const string verbosity = argv[5];
//////////////////////////////////////


char filenames[500];
vector<string> filesVec;
///read the file names from the .txt files and load them to a vector.
ifstream fin(InRootList.c_str());while(fin.getline(filenames, 500) ){filesVec.push_back(filenames);}
cout<< "\nProcessing " << subSampleKey << " ... " << endl;
TChain *sample_AUX = new TChain("stopTreeMaker/AUX");
for(unsigned int in=0; in<filesVec.size(); in++){ sample_AUX->Add(filesVec.at(in).c_str()); }

templatePlotsFunc(sample_AUX, subSampleKey,atoi(verbosity.c_str()),OutDir,inputNum);

}





int countCSVS(const vector<TLorentzVector> &inputJets, const vector<double> &inputCSVS, const double CSVS, const double *jetCutsArr){
return countCSVS(inputJets, inputCSVS, CSVS, jetCutsArr[0], jetCutsArr[1], jetCutsArr[2], jetCutsArr[3]);
}
int countCSVS(const vector<TLorentzVector> &inputJets, const vector<double> &inputCSVS, const double CSVS, const double minAbsEta, const double maxAbsEta, const double minPt, const double maxPt){
int cntNJets =0;
for(unsigned int ij=0; ij<inputJets.size(); ij++){
double perjetpt = inputJets[ij].Pt(), perjeteta = inputJets[ij].Eta();
if( ( minAbsEta == -1 || fabs(perjeteta) >= minAbsEta )
&& ( maxAbsEta == -1 || fabs(perjeteta) < maxAbsEta )
&& ( minPt == -1 || perjetpt >= minPt )
&& ( maxPt == -1 || perjetpt < maxPt ) ){
if( inputCSVS[ij] > CSVS ) cntNJets ++;
}
}
return cntNJets;
}

int countJets(const vector<TLorentzVector> &inputJets, const double *jetCutsArr){
return countJets(inputJets, jetCutsArr[0], jetCutsArr[1], jetCutsArr[2], jetCutsArr[3]);
}
int countJets(const vector<TLorentzVector> &inputJets, const double minAbsEta, const double maxAbsEta, const double minPt, const double maxPt){
int cntNJets =0;
for(unsigned int ij=0; ij<inputJets.size(); ij++){
double perjetpt = inputJets[ij].Pt(), perjeteta = inputJets[ij].Eta();
if( ( minAbsEta == -1 || fabs(perjeteta) >= minAbsEta )
&& ( maxAbsEta == -1 || fabs(perjeteta) < maxAbsEta )
&& ( minPt == -1 || perjetpt >= minPt )
&& ( maxPt == -1 || perjetpt < maxPt ) ){
cntNJets ++;
}
}
return cntNJets;
}
vector<double> calcDPhi(const vector<TLorentzVector> &inputJets, const double metphi, const int nDPhi, const double *jetCutsArr){
return calcDPhi(inputJets, metphi, nDPhi, jetCutsArr[0], jetCutsArr[1], jetCutsArr[2], jetCutsArr[3]);
}
vector<double> calcDPhi(const vector<TLorentzVector> &inputJets, const double metphi, const int nDPhi, const double minAbsEta, const double maxAbsEta, const double minPt, const double maxPt){
int cntNJets =0;
vector<double> outDPhiVec(nDPhi, 999);
for(unsigned int ij=0; ij<inputJets.size(); ij++){
double perjetpt = inputJets[ij].Pt(), perjeteta = inputJets[ij].Eta();
if( ( minAbsEta == -1 || fabs(perjeteta) >= minAbsEta )
&& ( maxAbsEta == -1 || fabs(perjeteta) < maxAbsEta )
&& ( minPt == -1 || perjetpt >= minPt )
&& ( maxPt == -1 || perjetpt < maxPt ) ){
if( cntNJets < nDPhi ){
double perDPhi = fabs(TVector2::Phi_mpi_pi( inputJets[ij].Phi() - metphi ));
outDPhiVec[cntNJets] = perDPhi;
}
cntNJets ++;
}
}
return outDPhiVec;///this is a vector whose components are delta phi of each jet with met.
}

