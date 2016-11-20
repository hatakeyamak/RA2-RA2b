#include <vector>

void CalcAcceptanceSystematicsFromPDF(){

  char tempname[200];
  TFile * infile = new TFile("/home/arane/work/Analysis_May11/CMSSW_8_0_20/src/Sep15/RA2-RA2b-2015/New_RA2b_2015/TauHad/ARElog84_LostLepton2_MuonEfficiencies_TTbarPlusWJet_hadd.root","R");
  //  TFile * infile = new TFile("LostLepton2_MuonEfficienciesFromAll_Elog408_.root","R");
  TH1* hAccPass, * hAccAll, * hAccPass_lowDphi, * hAccAll_lowDphi;
  vector<TH1*> hAccVec, hAcc_lowDphiVec, hAcc_DeviationFromNomVec, hAcc_DeviationFromNom_lowDphiVec;

  int PDFsize = 101; // 101 pdf
  for(int iacc=0; iacc < PDFsize; iacc++){

    // get the pass
    sprintf(tempname,"PdfAcc/hAccPass_%d",iacc);
    hAccPass = (TH1*) infile->Get(tempname)->Clone();

    // get the all
    sprintf(tempname,"PdfAcc/hAccAll_%d",iacc);
    hAccAll = (TH1*) infile->Get(tempname)->Clone();    
    
    // divide the pass and all to get acc
    sprintf(tempname,"hAccVec_%d",iacc);
    hAccVec.push_back(static_cast<TH1*>(hAccPass->Clone(tempname)));
    hAccVec[iacc]->Divide(hAccPass,hAccAll,1,1,"B");

    cout << " bin5=> acc: " << hAccVec[iacc]->GetBinContent(5) << endl;

    // lowDphi
    sprintf(tempname,"PdfAcc/hAccPass_lowDphi_%d",iacc);
    hAccPass_lowDphi = (TH1*) infile->Get(tempname)->Clone();

    sprintf(tempname,"PdfAcc/hAccAll_lowDphi_%d",iacc);
    hAccAll_lowDphi = (TH1*) infile->Get(tempname)->Clone();

    sprintf(tempname,"hAcc_lowDphiVec_%d",iacc);
    hAcc_lowDphiVec.push_back(static_cast<TH1*>(hAccPass_lowDphi->Clone(tempname)));
    hAcc_lowDphiVec[iacc]->Divide(hAccPass_lowDphi,hAccAll_lowDphi,1,1,"B");


    // calculate the deviation from nominal acceptance
    hAcc_DeviationFromNomVec.push_back(static_cast<TH1*>(hAccVec[iacc]->Clone("hAcc_DeviationFromNomVec")));// copy
    hAcc_DeviationFromNomVec[iacc]->Add(hAccVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom

    // do the same for lowDphi
    hAcc_DeviationFromNom_lowDphiVec.push_back(static_cast<TH1*>(hAcc_lowDphiVec[iacc]->Clone("hAcc_DeviationFromNom_lowDphiVec")));// copy
    hAcc_DeviationFromNom_lowDphiVec[iacc]->Add(hAcc_lowDphiVec[0],-1.0); // subtract the nominal from each acceptance: Acc - Acc_nom

  }// end of loop over pdf



  TH1 * hAccSysMax = static_cast<TH1*>(hAccAll->Clone("hAccSysMax"));
  hAccSysMax->Reset();
  TH1 * hAccSysMax_lowDphi = static_cast<TH1*>(hAccAll_lowDphi->Clone("hAccSysMax_lowDphi"));
  hAccSysMax_lowDphi->Reset();
  TH1 * hAccSysMin = static_cast<TH1*>(hAccAll->Clone("hAccSysMin"));
  hAccSysMin->Reset();
  TH1 * hAccSysMin_lowDphi = static_cast<TH1*>(hAccAll_lowDphi->Clone("hAccSysMin_lowDphi"));
  hAccSysMin_lowDphi->Reset();

  for(int ibin=0; ibin < hAccSysMax->GetNbinsX()+2; ibin++){
    //cout << "bincontent: " <<  hAccSysMax->GetBinContent(ibin) << endl;
    for(int iacc=0; iacc < PDFsize; iacc++){
      if(hAccSysMax->GetBinContent(ibin)<hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
        hAccSysMax->SetBinContent(ibin,hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)); // get the max of all acc
      }
      if(hAccSysMax_lowDphi->GetBinContent(ibin)<hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
        hAccSysMax_lowDphi->SetBinContent(ibin,hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)); 
      }
      if(hAccSysMin->GetBinContent(ibin)>hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)){
        hAccSysMin->SetBinContent(ibin,hAcc_DeviationFromNomVec[iacc]->GetBinContent(ibin)); // get the max of all acc
      }
      if(hAccSysMin_lowDphi->GetBinContent(ibin)>hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin)){
        hAccSysMin_lowDphi->SetBinContent(ibin,hAcc_DeviationFromNom_lowDphiVec[iacc]->GetBinContent(ibin));
      }
    }
  }



  TFile * outFile = new TFile("ARElog84_AcceptanceSystematicsFromPDF_AllSamples.root","RECREATE");
  hAccSysMax->Write();
  hAccSysMax_lowDphi->Write();
  hAccSysMin->Write();
  hAccSysMin_lowDphi->Write();

  for(int iacc=0; iacc < PDFsize ; iacc++){
    hAccVec[iacc]->Write();
    hAcc_lowDphiVec[iacc]->Write();

  }


  outFile->Close();









}
