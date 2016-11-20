#include <vector>

void CalcAcceptanceSystematicsFromScale(){

  char tempname[200];
  //  TFile * infile = new TFile("LostLepton2_MuonEfficiencies_hadd.root","R");
  TFile * infile = new TFile("/home/arane/work/Analysis_May11/CMSSW_8_0_20/src/Sep15/RA2-RA2b-2015/New_RA2b_2015/TauHad/ARElog84_LostLepton2_MuonEfficiencies_TTbarPlusWJet_hadd.root","R");
  TH1* hScaleAccPass, * hScaleAccAll, * hScaleAccPass_lowDphi, * hScaleAccAll_lowDphi;
  vector<TH1*> hScaleAccVec, hScaleAcc_lowDphiVec, hScaleAcc_DeviationFromNomVec, hScaleAcc_DeviationFromNom_lowDphiVec;

  int Scalesize = 9; // 101 pdf
  for(int iacc=0; iacc < Scalesize; iacc++){

    // get the pass
    sprintf(tempname,"ScaleAcc/hScaleAccPass_%d",iacc);
    hScaleAccPass = (TH1*) infile->Get(tempname)->Clone();

    // get the all
    sprintf(tempname,"ScaleAcc/hScaleAccAll_%d",iacc);
    hScaleAccAll = (TH1*) infile->Get(tempname)->Clone();    
    
    // divide the pass and all to get acc
    sprintf(tempname,"hScaleAccVec_%d",iacc);
    hScaleAccVec.push_back(static_cast<TH1*>(hScaleAccPass->Clone(tempname)));
    hScaleAccVec[iacc]->Divide(hScaleAccPass,hScaleAccAll,1,1,"B");

    cout << " bin5=> acc: " << hScaleAccVec[iacc]->GetBinContent(5) << endl;

    // lowDphi
    sprintf(tempname,"ScaleAcc/hScaleAccPass_lowDphi_%d",iacc);
    hScaleAccPass_lowDphi = (TH1*) infile->Get(tempname)->Clone();

    sprintf(tempname,"ScaleAcc/hScaleAccAll_lowDphi_%d",iacc);
    hScaleAccAll_lowDphi = (TH1*) infile->Get(tempname)->Clone();

    sprintf(tempname,"hScaleAcc_lowDphiVec_%d",iacc);
    hScaleAcc_lowDphiVec.push_back(static_cast<TH1*>(hScaleAccPass_lowDphi->Clone(tempname)));
    hScaleAcc_lowDphiVec[iacc]->Divide(hScaleAccPass_lowDphi,hScaleAccAll_lowDphi,1,1,"B");


    // calculate the deviation from nominal acceptance
    hScaleAcc_DeviationFromNomVec.push_back(static_cast<TH1*>(hScaleAccVec[iacc]->Clone("hScaleAcc_DeviationFromNomVec")));// copy
    hScaleAcc_DeviationFromNomVec[iacc]->Add(hScaleAccVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom

    // do the same for lowDphi
    hScaleAcc_DeviationFromNom_lowDphiVec.push_back(static_cast<TH1*>(hScaleAcc_lowDphiVec[iacc]->Clone("hScaleAcc_DeviationFromNom_lowDphiVec")));// copy
    hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->Add(hScaleAcc_lowDphiVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom

  }// end of loop over pdf



  TH1 * hScaleAccSysMax = static_cast<TH1*>(hScaleAccAll->Clone("hScaleAccSysMax"));
  hScaleAccSysMax->Reset();
  TH1 * hScaleAccSysMax_lowDphi = static_cast<TH1*>(hScaleAccAll_lowDphi->Clone("hScaleAccSysMax_lowDphi"));
  hScaleAccSysMax_lowDphi->Reset();
  TH1 * hScaleAccSysMin = static_cast<TH1*>(hScaleAccAll->Clone("hScaleAccSysMin"));
  hScaleAccSysMin->Reset();
  TH1 * hScaleAccSysMin_lowDphi = static_cast<TH1*>(hScaleAccAll_lowDphi->Clone("hScaleAccSysMin_lowDphi"));
  hScaleAccSysMin_lowDphi->Reset();

  for(int ibin=0; ibin < hScaleAccSysMax->GetNbinsX()+2; ibin++){
    //cout << "bincontent: " <<  hScaleAccSysMax->GetBinContent(ibin) << endl;
    for(int iacc=0; iacc < Scalesize; iacc++){
      if(hScaleAccSysMax->GetBinContent(ibin)<hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
        hScaleAccSysMax->SetBinContent(ibin,hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)); // get the max of all acc
      }
      if(hScaleAccSysMax_lowDphi->GetBinContent(ibin)<hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
        hScaleAccSysMax_lowDphi->SetBinContent(ibin,hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)); 
      }
      if(hScaleAccSysMin->GetBinContent(ibin)>hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
        hScaleAccSysMin->SetBinContent(ibin,hScaleAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)); // get the max of all acc
      }
      if(hScaleAccSysMin_lowDphi->GetBinContent(ibin)>hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
        hScaleAccSysMin_lowDphi->SetBinContent(ibin,hScaleAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin));
      }
    }
  }


  TFile * outFile = new TFile("ARElog84_AcceptanceSystematicsFromScale_AllSamples.root","RECREATE");
  hScaleAccSysMax->Write();
  hScaleAccSysMax_lowDphi->Write();
  hScaleAccSysMin->Write();
  hScaleAccSysMin_lowDphi->Write();

  for(int iacc=0; iacc < Scalesize ; iacc++){
    hScaleAccVec[iacc]->Write();
    hScaleAcc_lowDphiVec[iacc]->Write();
  }

  outFile->Close();

}
