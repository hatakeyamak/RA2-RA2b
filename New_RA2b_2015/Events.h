#ifndef EVENT_H
#define EVENT_H
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
using namespace std;

  class Events{
   
   // See if working with Data or MC
   bool DataBool;
   
   // Study Tau ID
   bool StudyTauId;

   //fastsim
   bool fastsim;
   
   // Declaration of leaf types
   //UChar_t         GoodVtx;
   Int_t           NumInteractions;
   Double_t        TrueNumInteractions;
   Int_t           NJetsISR;
   //   UChar_t         GoodVtx;
   Int_t          HBHENoiseFilter;
   Int_t          HBHEIsoNoiseFilter;
   Int_t          CSCTightHaloFilter;
   Int_t           eeBadScFilter;
   Int_t           EcalDeadCellTriggerPrimitiveFilter;

   // added on July 12, 2016
   Double_t        PFCaloMETRatio;
   Bool_t          BadChargedCandidateFilter;
   Bool_t          BadPFMuonFilter;
   Int_t           globalTightHalo2016Filter;

   UInt_t          RunNum;
   UInt_t          LumiBlockNum;
   ULong64_t       EvtNum;
   Int_t           NVtx;
//Int_t           isoTracks;
   Int_t           isoElectronTracks;
   Int_t           isoMuonTracks;
   Int_t           isoPionTracks;
   //   Int_t           Leptons;
   Int_t           NJets;
   Int_t           BTags;
   Double_t         Weight;
   Double_t        CrossSection;
   Double_t        puWeight;
   Double_t         HT;
   Double_t         MHT;
   Double_t         MHTPhi; 
//Double_t         MHTPhi;
   Double_t         MET;
   Double_t         METPhi;
   Double_t         DeltaPhi1;
   Double_t         DeltaPhi2;
   Double_t         DeltaPhi3;
   Double_t         DeltaPhi4;
   TTree *         fChain;
   int             currentEntry_;
   int             template_Entries; 
   Double_t        madHT;

   vector<TLorentzVector> *GenMus;
   vector<TLorentzVector> *GenEls;
   vector<TLorentzVector> *GenTaus;
   vector<TLorentzVector> *GenTauNu;
   vector<TLorentzVector> *GenJets;

   vector<TLorentzVector> *GenParticles;
   vector<int>     *GenParticles_PdgId;
   vector<double>  *PDFweights;
   vector<double>  *ScaleWeights;
   //vector<TLorentzVector> *Jets;
   vector<int>     *Jets_partonFlavor;
   vector<bool>    *HTJetsMask;
   vector<double>  *Jets_bDiscriminatorCSV;
   vector<double>  *Jets_chargedEmEnergyFraction;
   vector<double>  *Jets_chargedHadronEnergyFraction;
   vector<int>     *Jets_chargedHadronMultiplicity;
   vector<int>     *Jets_electronMultiplicity;

   vector<double>  *Jets_jecFactor;
   vector<double>  *softJets_jecFactor; 
   //   vector<double>  *Jets_jetArea;
   vector<double>  *Jets_muonEnergyFraction;
   vector<int>     *Jets_muonMultiplicity;
   vector<double>  *Jets_neutralEmEnergyFraction;
   vector<int>     *Jets_neutralHadronMultiplicity;
   vector<double>  *Jets_photonEnergyFraction;
   vector<int>     *Jets_photonMultiplicity;
   
   vector<TLorentzVector> *softJetsJECdown;
   vector<TLorentzVector> *softJetsJECup;
   vector<TLorentzVector> *softJets; 
   vector<TLorentzVector> *JetsJECdown;
   vector<TLorentzVector> *JetsJECup;
   vector<TLorentzVector> *Jets;

//   vector<double>  *slimmedMuonsPtVec;
   vector<TLorentzVector> *Muons;
   vector<TLorentzVector> *selectedIDMuons;
//   vector<double>  *selectedIDIsoMuonsPtVec;
//   vector<double>  *slimmedElectronsPtVec;
   vector<TLorentzVector> *Electrons;
   vector<TLorentzVector> *selectedIDElectrons;
//   vector<double>  *selectedIDIsoElectronsPtVec;
   //vector<TLorentzVector> *IsolatedElectronTracksVeto;
   //vector<TLorentzVector> *IsolatedMuonTracksVeto;
   //vector<TLorentzVector> *IsolatedPionTracksVeto;
   vector<TLorentzVector> *TAPElectronTracks;
   vector<TLorentzVector> *TAPMuonTracks;
   vector<TLorentzVector> *TAPPionTracks;

   vector<double>  *selectedIDIsoMuons_MT2Activity;
   vector<double>  *GenMu_MT2Activity;

   vector<TLorentzVector> *TauLorVec;
   vector<double>  *TauIdVecagainstElectronLooseMVA5;
   vector<double>  *TauIdVecagainstElectronMediumMVA5;
   vector<double>  *TauIdVecagainstElectronVLooseMVA5;
   vector<double>  *TauIdVecagainstMuonLoose3;
   vector<double>  *TauIdVecagainstMuonTight3;
   vector<double>  *TauIdVecbyLooseCombinedIsolationDeltaBetaCorr3Hits;
   vector<double>  *TauIdVecbyLoosePileupWeightedIsolation3Hits;
   vector<double>  *TauIdVecbyMediumCombinedIsolationDeltaBetaCorr3Hits;
   vector<double>  *TauIdVecbyMediumPileupWeightedIsolation3Hits;
   vector<double>  *TauIdVecbyTightCombinedIsolationDeltaBetaCorr3Hits;
   vector<double>  *TauIdVecbyTightPileupWeightedIsolation3Hits;
   

   vector<int>     *TriggerPass;
   vector<string>  *TriggerNames;

   vector<bool>     *GenMu_GenMuFromTau;
   vector<bool>     *GenElec_GenElecFromTau;
   vector<bool>     *GenTau_GenTauHad;
   vector<int>     *softJets_ID;
   vector<int>     *Jets_ID;
   Bool_t          JetID;
   Bool_t          JetIDloose; 

public:
//constructor
Events(TTree * ttree_, const std::string sampleKeyString="ttbar", int verbose=0);

//Functions
  int NumInteractions_() const;
  double TrueNumInteractions_() const;
  double XS() const ;
  int NJetsISR_() const;

  bool loadNext();
  int TotNEve() const;
  int Runnum() const;
  int Evtnum() const;
  int LumiBlocknum() const;
  int nJets() const;
  int nBtags() const;
  //int nBtagBin() const;
  int nLeptons() const;
  //  int nIso() const;
  int nIsoElec() const ;
  int nIsoMu() const ;
  int nIsoPion() const;
  double weight() const ;
  double puweight() const ;
  double ht() const ;
  double gen_ht() const ; 
  double mht() const ;
  double mhtphi() const ;
  double met() const ;
  double metphi() const ;
  double deltaPhi1() const ;
  double deltaPhi2() const ;
  double deltaPhi3() const ;
  double deltaPhi4() const ;
  int nGenMu() const ;
  int JetId() const ; 

  bool DataBool_() const;

   vector<double>  GenMuPtVec_() const;
   vector<double>  GenMuEtaVec_() const;
   vector<double>  GenMuPhiVec_() const;
   vector<double>  GenMuEVec_() const;

   vector<double>  GenElecPtVec_() const;
   vector<double>  GenElecEtaVec_() const;
   vector<double>  GenElecPhiVec_() const;

   vector<TLorentzVector>* GenTauLorVec() const;
   vector<double>  GenTauPtVec_() const;
   vector<double>  GenTauEtaVec_() const;
   vector<double>  GenTauPhiVec_() const;

   vector<TLorentzVector>*  GenTauNuLorVec() const;
   vector<double>  GenTauNuPtVec_() const;
   vector<double>  GenTauNuEtaVec_() const;
   vector<double>  GenTauNuPhiVec_() const;

   vector<TLorentzVector>      *genParticles_() const;
   vector<int>                 *genParticles_PDGid_() const;
   vector<double> * PDFweights_() const;
   vector<double> * ScaleWeights_() const;
   vector<double>  JetsPtVec_() const;
   vector<double>  JetsEtaVec_() const;
   vector<double>  JetsPhiVec_() const;
   vector<double>  JetsEVec_() const;
   vector<TLorentzVector> * JetsLorVec_() const;
   vector<double>  csvVec() const;
   vector<double>  Jets_chargedEmEnergyFraction_() const;
   vector<double>  Jets_chargedHadronEnergyFraction_() const;
   vector<int>     Jets_chargedHadronMultiplicity_() const;
   vector<int>     Jets_electronMultiplicity_() const;
   //   vector<double>  Jets_jetArea_() const;
   vector<double>  Jets_muonEnergyFraction_() const;
   vector<int>     Jets_muonMultiplicity_() const;
   vector<double>  Jets_neutralEmEnergyFraction_() const;
   vector<int>     Jets_neutralHadronMultiplicity_() const;
   vector<double>  Jets_photonEnergyFraction_() const; 
   vector<int>     Jets_photonMultiplicity_() const;


   vector<double>  slimJetPtVec_() const;
   vector<double>  slimJetEtaVec_() const;
   vector<double>  slimJetPhiVec_() const;
   vector<double>  slimJetupPtVec_() const;
   vector<double>  slimJetupEtaVec_() const;
   vector<double>  slimJetupPhiVec_() const;
   vector<double>  slimJetdownPtVec_() const;
   vector<double>  slimJetdownEtaVec_() const;
   vector<double>  slimJetdownPhiVec_() const;

   vector<double>  GenJetPtVec_() const;
   vector<double>  GenJetEtaVec_() const;
   vector<double>  GenJetPhiVec_() const;

   vector<int>    * Jets_partonFlavor_() const;
   vector<bool>   * HTJetsMask_() const;


   vector<int>     slimJetID_() const;
   vector<double>  slimJetjecFactor_() const;
   vector<double>  Jets_jecFactor_() const; 

   vector<TLorentzVector> slimJetJECdown_() const;
   vector<TLorentzVector> slimJetJECup_() const;
   vector<double>  slimmedMuPtVec_() const;
   vector<double>  slimmedMuEtaVec_() const;
   vector<double>  slimmedMuPhiVec_() const;
   vector<double>  MuNoIsoPtVec_() const;
   vector<double>  MuNoIsoEtaVec_() const;
   vector<double>  MuNoIsoPhiVec_() const;
   vector<double>  MuPtVec_() const;
   vector<double>  MuEtaVec_() const;
   vector<double>  MuPhiVec_() const;
   vector<double>  MuEVec_() const;

   vector<double>  slimmedElecPtVec_() const;
   vector<double>  slimmedElecEtaVec_() const;
   vector<double>  slimmedElecPhiVec_() const;
   vector<double>  ElecNoIsoPtVec_() const;
   vector<double>  ElecNoIsoEtaVec_() const;
   vector<double>  ElecNoIsoPhiVec_() const;
   vector<double>  ElecPtVec_() const;
   vector<double>  ElecEtaVec_() const;
   vector<double>  ElecPhiVec_() const;

   vector<int>     GenMuFromTauVec_() const;   
   vector<int>     GenElecFromTauVec_() const;   
   vector<int>     GenTauHadVec_() const;   
   
   vector<double>  IsoElecPtVec_() const;
   vector<double>  IsoElecEtaVec_() const;
   vector<double>  IsoElecPhiVec_() const;
   vector<double>  IsoMuPtVec_() const;
   vector<double>  IsoMuEtaVec_() const;
   vector<double>  IsoMuPhiVec_() const;
   vector<double>  IsoPionPtVec_() const;
   vector<double>  IsoPionEtaVec_() const;
   vector<double>  IsoPionPhiVec_() const;

   vector<double>  MTActivityVec_() const;
   vector<double>  GenMTActivityVec_() const;

   vector<TLorentzVector> *TauLorVec_() const;
   vector<double>* tauId1() const;
   vector<double>* tauId2() const;
   vector<double>* tauId3() const;
   vector<double>* tauId4() const;
   vector<double>* tauId5() const;
   vector<double>* tauId6() const;
   vector<double>* tauId7() const;
   vector<double>* tauId8() const;
   vector<double>* tauId9() const;
   vector<double>* tauId10() const;
   vector<double>* tauId11() const;
   map<int,int> nTauMap() const;

   vector<string>  TriggerNames_() const;
   vector<int>  PassTrigger_() const;
   double csv_() const;
   
//   int GoodVtx_() const; 
   int CSCTightHaloFilter_() const;
   int eeBadScFilter_() const;
   int HBHENoiseFilter_() const;
   int HBHEIsoNoiseFilter_() const;
   int EcalDeadCellTriggerPrimitiveFilter_() const;
   int NVtx_() const;
   // added on July 12, 2016
   int PFCaloMETRatioFilter_() const;
   int BadChargedCandidateFilter_() const;
   int BadPFMuonFilter_() const;
   int globalTightHalo2016Filter_() const;
   int noMuonJet_() const;
   int noFakeJet_() const;

};//end of class Events
#endif
