// per compilare: g++ -Wall -o TestShowerShapesOnRegression `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit TestShowerShapesOnRegression.cpp
 
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPaveStats.h"
#include "TLegend.h"
#include "TChain.h"
#include "TVirtualFitter.h"
#include "TLatex.h"
#include "TMath.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <ctime>
#include <map>
#include <algorithm>
#include <cmath>


using namespace std;

float GetNvtxWeight(int nvtx, TH1F *h){
  
  int bin = h->FindBin(nvtx);
  float w = h->GetBinContent(bin);
  //cout << nvtx <<endl;
  return w;
  
}



int main(int argc, char** argv)
{
  float minEt1   = 40.;
  float minEt2   = 30.;
  float minMass  = 100.;
  float maxMass  = 200.;
  float r9cut    = 0.94;
  float minDiphotonMVA = -0.405;

  float etaBoundaryEB[3] = {0.000, 1.000, 1.4442};
  float etaBoundaryEE[3] = {1.566, 2.000, 2.5000};
  
  
  TFile *file = TFile::Open(argv[1]);
  cout << "Analyzing " << argv[1] <<endl;

  string sample(argv[1]);
  if ( sample.find("DYJetsToLL") != std::string::npos || sample.find("90") != std::string::npos) {
    minMass = 80;
    maxMass = 100;
  }

  
  TTree* tree = (TTree*)file->Get("diphotonDumper/trees/tree_13TeV_All");
  
  // Declaration of leaf types
  Int_t           candidate_id;
  Float_t         weight;
  Float_t         mass;
  Float_t         diphotonMVA;
  Float_t         ele1_et;
  Float_t         ele1_energy;
  Float_t         ele1_rawEnergy;
  Float_t         ele1_esEnergy;
  Float_t         ele1_scEta;
  Float_t         ele1_scPhi;
  Float_t         ele1_eta;
  Float_t         ele1_phi;
  Float_t         ele1_full5x5_sigmaIetaIeta;
  Float_t         ele1_full5x5_r9;
  Float_t         ele1_r9;
  Float_t         ele1_etaWidth;
  Float_t         ele1_reco_E;
  Float_t         ele1_reco_regr_E;
  Float_t         ele1_beforeShShTransf_regr_E;
  Float_t         ele1_afterShShTransf_regr_E;
  Float_t         ele2_et;
  Float_t         ele2_rawEnergy;
  Float_t         ele2_esEnergy;
  Float_t         ele2_energy;
  Float_t         ele2_scEta;
  Float_t         ele2_scPhi;
  Float_t         ele2_eta;
  Float_t         ele2_phi;
  Float_t         ele2_full5x5_sigmaIetaIeta;
  Float_t         ele2_full5x5_r9;
  Float_t         ele2_r9;
  Float_t         ele2_etaWidth;
  Float_t         ele2_beforeShShTransf_regr_E;
  Float_t         ele2_afterShShTransf_regr_E;
  
  tree->SetBranchAddress("weight", &weight);
  tree->SetBranchAddress("candidate_id", &candidate_id);

  tree->SetBranchAddress("mass", &mass);
  tree->SetBranchAddress("diphotonMVA", &diphotonMVA);

  tree->SetBranchAddress("ele1_rawEnergy", &ele1_rawEnergy);
  tree->SetBranchAddress("ele1_esEnergy", &ele1_esEnergy);
  tree->SetBranchAddress("ele1_energy", &ele1_energy);
  tree->SetBranchAddress("ele1_et", &ele1_et);
  tree->SetBranchAddress("ele1_scEta", &ele1_scEta);
  tree->SetBranchAddress("ele1_scPhi", &ele1_scPhi);
  tree->SetBranchAddress("ele1_eta", &ele1_eta);
  tree->SetBranchAddress("ele1_phi", &ele1_phi);
  tree->SetBranchAddress("ele1_full5x5_sigmaIetaIeta", &ele1_full5x5_sigmaIetaIeta);
  tree->SetBranchAddress("ele1_full5x5_r9", &ele1_full5x5_r9);
  tree->SetBranchAddress("ele1_r9", &ele1_r9);
  tree->SetBranchAddress("ele1_etaWidth", &ele1_etaWidth);
  tree->SetBranchAddress("ele1_reco_E", &ele1_reco_E);
  tree->SetBranchAddress("ele1_reco_regr_E", &ele1_reco_regr_E);
  tree->SetBranchAddress("ele1_beforeShShTransf_regr_E", &ele1_beforeShShTransf_regr_E);
  tree->SetBranchAddress("ele1_afterShShTransf_regr_E", &ele1_afterShShTransf_regr_E);
    
  tree->SetBranchAddress("ele2_rawEnergy", &ele2_rawEnergy);
  tree->SetBranchAddress("ele2_esEnergy", &ele2_esEnergy);
  tree->SetBranchAddress("ele2_energy", &ele2_energy);
  tree->SetBranchAddress("ele2_et", &ele2_et);
  tree->SetBranchAddress("ele2_scEta", &ele2_scEta);
  tree->SetBranchAddress("ele2_scPhi", &ele2_scPhi);
  tree->SetBranchAddress("ele2_eta", &ele2_eta);
  tree->SetBranchAddress("ele2_phi", &ele2_phi);
  tree->SetBranchAddress("ele2_full5x5_sigmaIetaIeta", &ele2_full5x5_sigmaIetaIeta);
  tree->SetBranchAddress("ele2_full5x5_r9", &ele2_full5x5_r9);
  tree->SetBranchAddress("ele2_r9", &ele2_r9);
  tree->SetBranchAddress("ele2_etaWidth", &ele2_etaWidth);
  tree->SetBranchAddress("ele2_beforeShShTransf_regr_E", &ele2_beforeShShTransf_regr_E);
  tree->SetBranchAddress("ele2_afterShShTransf_regr_E", &ele2_afterShShTransf_regr_E);

  // book histograms

  // for sanity check that the energy stored for teh photon is the one before shower shape corrections: these smearings should correspond to those in shervn's files.
  TH2F *hExtraSmearings_vs_eta_highr9 = new TH2F("hExtraSmearings_vs_eta_highr9","hExtraSmearings_vs_eta_highr9", 30, 0.0 , 3.0 , 100, -0.10, 0.10);
  TH2F *hExtraSmearings_vs_eta_lowr9 = new TH2F("hExtraSmearings_vs_eta_lowr9","hExtraSmearings_vs_eta_lowr9", 30, 0.0 , 3.0 , 100, -0.10, 0.10);

  TH1F *hExtraSmearing_EB_LowEta_HR9 = new TH1F("hExtraSmearing_EB_LowEta_HR9","hExtraSmearing_EB_LowEta_HR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EB_LowEta_LR9 = new TH1F("hExtraSmearing_EB_LowEta_LR9","hExtraSmearing_EB_LowEta_LR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EB_HighEta_HR9 = new TH1F("hExtraSmearing_EB_HighEta_HR9","hExtraSmearing_EB_HighEta_HR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EB_HighEta_LR9 = new TH1F("hExtraSmearing_EB_HighEta_LR9","hExtraSmearing_EB_HighEta_LR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EE_LowEta_HR9 = new TH1F("hExtraSmearing_EE_LowEta_HR9","hExtraSmearing_EE_LowEta_HR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EE_LowEta_LR9 = new TH1F("hExtraSmearing_EE_LowEta_LR9","hExtraSmearing_EE_LowEta_LR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EE_HighEta_HR9 = new TH1F("hExtraSmearing_EE_HighEta_HR9","hExtraSmearing_EE_HighEta_HR9",100,-0.1,0.1);
  TH1F *hExtraSmearing_EE_HighEta_LR9 = new TH1F("hExtraSmearing_EE_HighEta_LR9","hExtraSmearing_EE_HighEta_LR9",100,-0.1,0.1);

  TH1F *hDeltaErecoEregr = new TH1F("hDeltaErecoEregr","hDeltaErecoEregr",400,0.0,2.0);
  
  // mass ratio before/after shower shapes transf.
  TH1F *hmassRatio_EBEB        = new TH1F("hmassRatio_EBEB","hmassRatio_EBEB",500,0.975,1.025);
  TH1F *hmassRatio_EBEB_highR9 = new TH1F("hmassRatio_EBEB_highR9","hmassRatio_EBEB_highR9",500,0.975,1.025);
  TH1F *hmassRatio_EBEB_highR9_inner = new TH1F("hmassRatio_EBEB_highR9_inner","hmassRatio_EBEB_highR9_inner",500,0.975,1.025);
  TH1F *hmassRatio_EBEB_highR9_outer = new TH1F("hmassRatio_EBEB_highR9_outer","hmassRatio_EBEB_highR9_outer",500,0.975,1.025);
  TH1F *hmassRatio_EBEB_lowR9  = new TH1F("hmassRatio_EBEB_lowR9","hmassRatio_EBEB_lowR9",500,0.975,1.025);
  TH1F *hmassRatio_EBEB_lowR9_inner = new TH1F("hmassRatio_EBEB_lowR9_inner","hmassRatio_EBEB_lowR9_inner",500,0.975,1.025);
  TH1F *hmassRatio_EBEB_lowR9_outer = new TH1F("hmassRatio_EBEB_lowR9_outer","hmassRatio_EBEB_lowR9_outer",500,0.975,1.025);

  TH1F *hmassRatio_EEEE        = new TH1F("hmassRatio_EEEE","hmassRatio_EEEE",500,0.975,1.025);
  TH1F *hmassRatio_EEEE_highR9 = new TH1F("hmassRatio_EEEE_highR9","hmassRatio_EEEE_highR9",500,0.975,1.025);
  TH1F *hmassRatio_EEEE_highR9_inner = new TH1F("hmassRatio_EEEE_highR9_inner","hmassRatio_EEEE_highR9_inner",500,0.975,1.025);
  TH1F *hmassRatio_EEEE_highR9_outer = new TH1F("hmassRatio_EEEE_highR9_outer","hmassRatio_EEEE_highR9_outer",500,0.975,1.025);
  TH1F *hmassRatio_EEEE_lowR9  = new TH1F("hmassRatio_EEEE_lowR9","hmassRatio_EEEE_lowR9",500,0.975,1.025);
  TH1F *hmassRatio_EEEE_lowR9_inner = new TH1F("hmassRatio_EEEE_lowR9_inner","hmassRatio_EEEE_lowR9_inner",500,0.975,1.025);
  TH1F *hmassRatio_EEEE_lowR9_outer = new TH1F("hmassRatio_EEEE_lowR9_outer","hmassRatio_EEEE_lowR9_outer",500,0.975,1.025);

  // r9
  TH1F *hr9_EB = new TH1F("hr9_EB","hr9_EB",100,0.5,1.0);
  TH1F *hr9_EE = new TH1F("hr9_EE","hr9_EE",100,0.5,1.0);
  
  // regression corr
  TProfile *p_regrCorr_vs_r9_EB_highR9 = new TProfile("p_regrCorr_vs_r9_EB_highR9","p_regrCorr_vs_r9_EB_highR9",500,0.5,1.0,0,10);
  TProfile *p_regrCorr_vs_r9_EB_lowR9 = new TProfile("p_regrCorr_vs_r9_EB_lowR9","p_regrCorr_vs_r9_EB_lowR9",500,0.5,1.0,0,10);

  TProfile *p_regrCorr_vs_r9_EE_highR9 = new TProfile("p_regrCorr_vs_r9_EE_highR9","p_regrCorr_vs_r9_EE_highR9",400,0.8,1.0,0,10);
  TProfile *p_regrCorr_vs_r9_EE_lowR9 = new TProfile("p_regrCorr_vs_r9_EE_lowR9","p_regrCorr_vs_r9_EE_lowR9",400,0.8,1.0,0,10);

  TProfile *p_regrCorr_vs_sietaieta_EB_highR9 = new TProfile("p_regrCorr_vs_sietaieta_EB_highR9","p_regrCorr_vs_sietaieta_EB_highR9",100,0.004,0.014,0,10);
  TProfile *p_regrCorr_vs_sietaieta_EB_lowR9 = new TProfile("p_regrCorr_vs_sietaieta_EB_lowR9","p_regrCorr_vs_sietaieta_EB_lowR9",100,0.004,0.014,0,10);

  TProfile *p_regrCorr_vs_sietaieta_EE_highR9 = new TProfile("p_regrCorr_vs_sietaieta_EE_highR9","p_regrCorr_vs_sietaieta_EE_highR9",200,0.015,0.035,0,10);
  TProfile *p_regrCorr_vs_sietaieta_EE_lowR9 = new TProfile("p_regrCorr_vs_sietaieta_EE_lowR9","p_regrCorr_vs_sietaieta_EE_lowR9",200,0.015,0.035,0,10);

  TProfile *p_regrCorr_vs_etaWidth_EB_highR9 = new TProfile("p_regrCorr_vs_etaWidth_EB_highR9","p_regrCorr_vs_etaWidth_EB_highR9",100,0.0,0.05,0,10);
  TProfile *p_regrCorr_vs_etaWidth_EB_lowR9 = new TProfile("p_regrCorr_vs_etaWidth_EB_lowR9","p_regrCorr_vs_etaWidth_EB_lowR9",100,0.0,0.05,0,10);

  TProfile *p_regrCorr_vs_etaWidth_EE_highR9 = new TProfile("p_regrCorr_vs_etaWidth_EE_highR9","p_regrCorr_vs_etaWidth_EE_highR9",100,0.0,0.05,0,10);
  TProfile *p_regrCorr_vs_etaWidth_EE_lowR9 = new TProfile("p_regrCorr_vs_etaWidth_EE_lowR9","p_regrCorr_vs_etaWidth_EE_lowR9",100,0.0,0.05,0,10);

  TProfile *p_regrCorr_vs_eta_highR9 = new TProfile("p_regrCorr_vs_eta_highR9","p_regrCorr_vs_eta_highR9",250,0.0,2.5,0,10);
  TProfile *p_regrCorr_vs_eta_lowR9 = new TProfile("p_regrCorr_vs_eta_lowR9","p_regrCorr_vs_eta_lowR9",250,0.0,2.5,0,10);

  
  /*
  TH1F *hdE_EB        = new TH1F("hdE_EB","hdE_EB",100,-0.10,0.10);
  TH1F *hdE_EB_highR9 = new TH1F("hdE_EB_highR9","hdE_EB_highR9",100,-0.10,0.10);
  TH1F *hdE_EB_highR9_inner = new TH1F("hdE_EB_highR9_inner","hdE_EB_highR9_inner",100,-0.10,0.10);
  TH1F *hdE_EB_highR9_outer = new TH1F("hdE_EB_highR9_outer","hdE_EB_highR9_outer",100,-0.10,0.10);
  TH1F *hdE_EB_lowR9  = new TH1F("hdE_EB_lowR9","hdE_EB_lowR9",100,-0.10,0.10);
  TH1F *hdE_EB_lowR9_inner = new TH1F("hdE_EB_lowR9_inner","hdE_EB_lowR9_inner",100,-0.10,0.10);
  TH1F *hdE_EB_lowR9_outer = new TH1F("hdE_EB_lowR9_outer","hdE_EB_lowR9_outer",100,-0.10,0.10);
  */
  
  cout << "Number of events to be analyzed : " << tree->GetEntries() <<endl;
  cout <<endl;

  for (int i=0; i<tree->GetEntries(); i++) {

    tree -> GetEntry(i);
    if (i%1000000==0) cout << "Analyzing event "<< i <<endl;

    // take only the first diphoton pair (highest SumpT) if there is more than one diphoton candidate
    if (candidate_id !=0 ) continue; 

    // trigger
    //    if (isData && !HLT_Ele35_WPLoose_Gsf_v1 && !HLT_Ele35_WPLoose_Gsf_v2 && !HLT_Ele35_WPLoose_Gsf_v3 && !HLT_Ele35_WPLoose_Gsf_v4 && !HLT_Ele35_WPLoose_Gsf_v5) continue;

    // weights
    float w = weight;
    
    // selections                  
    //if ( ele1_et < minEt1  ) continue;                                                         
    //if ( ele2_et < minEt2  ) continue;
    if ( ele1_et < mass/3. ) continue;                                                         
    if ( ele2_et < mass/4. ) continue;                                                         
    if ( diphotonMVA < minDiphotonMVA) continue;
    if ( mass < minMass ) continue;
    if ( mass > maxMass ) continue;
    

    float regressionCorr1 = ele1_beforeShShTransf_regr_E/(ele1_rawEnergy+ele1_esEnergy);
    float regressionCorr2 = ele2_beforeShShTransf_regr_E/(ele2_rawEnergy+ele2_esEnergy);

    float dRecoERegrE = ele1_reco_E/ele1_reco_regr_E;
      
    float deltaE1_smear = ele1_energy - ele1_beforeShShTransf_regr_E;
    float deltaE2_smear = ele2_energy - ele2_beforeShShTransf_regr_E;
    float dm = sqrt( (ele1_afterShShTransf_regr_E*ele2_afterShShTransf_regr_E) / (ele1_beforeShShTransf_regr_E*ele2_beforeShShTransf_regr_E) );

    // sanity check
    hDeltaErecoEregr ->Fill(dRecoERegrE, w);
      
    if ( ele1_full5x5_r9 > r9cut ) hExtraSmearings_vs_eta_highr9 ->Fill(fabs(ele1_scEta), deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( ele2_full5x5_r9 > r9cut ) hExtraSmearings_vs_eta_highr9 ->Fill(fabs(ele2_scEta), deltaE2_smear/ele2_beforeShShTransf_regr_E, w);
    if ( ele1_full5x5_r9 < r9cut ) hExtraSmearings_vs_eta_lowr9 ->Fill(fabs(ele1_scEta), deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( ele2_full5x5_r9 < r9cut ) hExtraSmearings_vs_eta_lowr9 ->Fill(fabs(ele2_scEta), deltaE2_smear/ele2_beforeShShTransf_regr_E, w);
    
    if ( fabs(ele1_scEta)>etaBoundaryEB[0] && fabs(ele1_scEta)<etaBoundaryEB[1] && ele1_full5x5_r9 > r9cut ) hExtraSmearing_EB_LowEta_HR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( fabs(ele1_scEta)>etaBoundaryEB[0] && fabs(ele1_scEta)<etaBoundaryEB[1] && ele1_full5x5_r9 < r9cut ) hExtraSmearing_EB_LowEta_LR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( fabs(ele1_scEta)>etaBoundaryEB[1] && fabs(ele1_scEta)<etaBoundaryEB[2] && ele1_full5x5_r9 > r9cut ) hExtraSmearing_EB_HighEta_HR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( fabs(ele1_scEta)>etaBoundaryEB[1] && fabs(ele1_scEta)<etaBoundaryEB[2] && ele1_full5x5_r9 < r9cut ) hExtraSmearing_EB_HighEta_LR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);

    if ( fabs(ele1_scEta)>etaBoundaryEE[0] && fabs(ele1_scEta)<etaBoundaryEE[1] && ele1_full5x5_r9 > r9cut ) hExtraSmearing_EE_LowEta_HR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( fabs(ele1_scEta)>etaBoundaryEE[0] && fabs(ele1_scEta)<etaBoundaryEE[1] && ele1_full5x5_r9 < r9cut ) hExtraSmearing_EE_LowEta_LR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( fabs(ele1_scEta)>etaBoundaryEE[1]   && fabs(ele1_scEta)<etaBoundaryEE[2] && ele1_full5x5_r9 > r9cut ) hExtraSmearing_EE_HighEta_HR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);
    if ( fabs(ele1_scEta)>etaBoundaryEE[1]   && fabs(ele1_scEta)<etaBoundaryEE[2] && ele1_full5x5_r9 < r9cut ) hExtraSmearing_EE_HighEta_LR9 -> Fill(deltaE1_smear/ele1_beforeShShTransf_regr_E, w);

    // r9 distributions
    if ( fabs(ele1_scEta) < etaBoundaryEB[2] ) hr9_EB -> Fill(ele1_full5x5_r9, w);
    if ( fabs(ele2_scEta) < etaBoundaryEB[2] ) hr9_EB -> Fill(ele2_full5x5_r9, w);
    if ( fabs(ele1_scEta) > etaBoundaryEE[0] ) hr9_EE -> Fill(ele1_full5x5_r9, w);
    if ( fabs(ele2_scEta) > etaBoundaryEE[0] ) hr9_EE -> Fill(ele2_full5x5_r9, w);


    // regression corrections
    if ( fabs(ele1_scEta) < etaBoundaryEB[2] && ele1_full5x5_r9 > r9cut){ 
      p_regrCorr_vs_r9_EB_highR9->Fill(ele1_r9,regressionCorr1,w);
      p_regrCorr_vs_sietaieta_EB_highR9->Fill(ele1_full5x5_sigmaIetaIeta,regressionCorr1,w);
      p_regrCorr_vs_etaWidth_EB_highR9->Fill(ele1_etaWidth,regressionCorr1,w);
      p_regrCorr_vs_eta_highR9->Fill(fabs(ele1_scEta),regressionCorr1,w);
    }

    if ( fabs(ele1_scEta) < etaBoundaryEB[2] && ele1_full5x5_r9 < r9cut){ 
      p_regrCorr_vs_r9_EB_lowR9->Fill(ele1_r9,regressionCorr1,w);
      p_regrCorr_vs_sietaieta_EB_lowR9->Fill(ele1_full5x5_sigmaIetaIeta,regressionCorr1,w);
      p_regrCorr_vs_etaWidth_EB_lowR9->Fill(ele1_etaWidth,regressionCorr1,w);
      p_regrCorr_vs_eta_lowR9->Fill(fabs(ele1_scEta),regressionCorr1,w);
    }
    
    if ( fabs(ele1_scEta) > etaBoundaryEE[0] && ele1_full5x5_r9 > r9cut){ 
      p_regrCorr_vs_r9_EE_highR9->Fill(ele1_r9,regressionCorr1,w);
      p_regrCorr_vs_sietaieta_EE_highR9->Fill(ele1_full5x5_sigmaIetaIeta,regressionCorr1,w);
      p_regrCorr_vs_etaWidth_EE_highR9->Fill(ele1_etaWidth,regressionCorr1,w);
      p_regrCorr_vs_eta_highR9->Fill(fabs(ele1_scEta),regressionCorr1,w);
    }

    if ( fabs(ele1_scEta) > etaBoundaryEE[0] && ele1_full5x5_r9 < r9cut){ 
      p_regrCorr_vs_r9_EE_lowR9->Fill(ele1_r9,regressionCorr1,w);
      p_regrCorr_vs_sietaieta_EE_lowR9->Fill(ele1_full5x5_sigmaIetaIeta,regressionCorr1,w);
      p_regrCorr_vs_etaWidth_EE_lowR9->Fill(ele1_etaWidth,regressionCorr1,w);
      p_regrCorr_vs_eta_lowR9->Fill(fabs(ele1_scEta),regressionCorr1,w);
    }


    
    // both electrons in the same cat.
    // EB-EB
    if (fabs(ele1_scEta)<etaBoundaryEB[2] && fabs(ele2_scEta)<etaBoundaryEB[2]  ) {
      hmassRatio_EBEB->Fill(dm,w);
      // high R9
      if (ele1_full5x5_r9 > r9cut && ele2_full5x5_r9 > r9cut) {
	hmassRatio_EBEB_highR9->Fill(dm,w);
	if (fabs(ele1_scEta)<etaBoundaryEB[1] && fabs(ele2_scEta)<etaBoundaryEB[1]) hmassRatio_EBEB_highR9_inner->Fill(dm,w);
	if (fabs(ele1_scEta)>etaBoundaryEB[1] && fabs(ele2_scEta)>etaBoundaryEB[1]) hmassRatio_EBEB_highR9_outer->Fill(dm,w);
      }
      // low R9
      if (ele1_full5x5_r9 < r9cut && ele2_full5x5_r9 < r9cut) {
	hmassRatio_EBEB_lowR9->Fill(dm,w);
	if (fabs(ele1_scEta)<etaBoundaryEB[1] && fabs(ele2_scEta)<etaBoundaryEB[1]) hmassRatio_EBEB_lowR9_inner->Fill(dm,w);
        if (fabs(ele1_scEta)>etaBoundaryEB[1] && fabs(ele2_scEta)>etaBoundaryEB[1]) hmassRatio_EBEB_lowR9_outer->Fill(dm,w);
      }
    }
    // EE-EE
    if (fabs(ele1_scEta)>etaBoundaryEE[0] && fabs(ele2_scEta)>etaBoundaryEE[0] &&
	fabs(ele1_scEta)<etaBoundaryEE[2] && fabs(ele2_scEta)<etaBoundaryEE[2]) {
      hmassRatio_EEEE->Fill(dm,w);
      // high R9
      if (ele1_full5x5_r9 > r9cut && ele2_full5x5_r9 > r9cut) {
	hmassRatio_EEEE_highR9->Fill(dm,w);
	if (fabs(ele1_scEta)<etaBoundaryEE[1] && fabs(ele2_scEta)<etaBoundaryEE[1]) hmassRatio_EEEE_highR9_inner->Fill(dm,w);
        if (fabs(ele1_scEta)>etaBoundaryEE[1] && fabs(ele2_scEta)>etaBoundaryEE[1]) hmassRatio_EEEE_highR9_outer->Fill(dm,w);
      }
      // low R9
      if (ele1_full5x5_r9 < r9cut && ele2_full5x5_r9 < r9cut) {
	hmassRatio_EEEE_lowR9->Fill(dm,w);
     	if (fabs(ele1_scEta)<etaBoundaryEE[1] && fabs(ele2_scEta)<etaBoundaryEE[1]) hmassRatio_EEEE_lowR9_inner->Fill(dm,w);
        if (fabs(ele1_scEta)>etaBoundaryEE[1] && fabs(ele2_scEta)>etaBoundaryEE[1]) hmassRatio_EEEE_lowR9_outer->Fill(dm,w);
      }
    }

  }// end loop over events
  
  
  TFile *fout = new TFile(argv[2],"recreate");

  hDeltaErecoEregr -> Write();
  
  hExtraSmearings_vs_eta_highr9->Write();
  hExtraSmearings_vs_eta_lowr9->Write();

  hExtraSmearing_EB_LowEta_HR9->Write();
  hExtraSmearing_EB_LowEta_LR9->Write();
  hExtraSmearing_EB_HighEta_HR9->Write();
  hExtraSmearing_EB_HighEta_LR9->Write();

  hExtraSmearing_EE_LowEta_HR9->Write();
  hExtraSmearing_EE_LowEta_LR9->Write();
  hExtraSmearing_EE_HighEta_HR9->Write();
  hExtraSmearing_EE_HighEta_LR9->Write();

  hr9_EB->Write();
  hr9_EE->Write();
  
  
  hmassRatio_EBEB->Write();
  hmassRatio_EBEB_highR9->Write();
  hmassRatio_EBEB_highR9_inner->Write();
  hmassRatio_EBEB_highR9_outer->Write();
  hmassRatio_EBEB_lowR9->Write();
  hmassRatio_EBEB_lowR9_inner->Write();
  hmassRatio_EBEB_lowR9_outer->Write();
  hmassRatio_EEEE->Write();
  hmassRatio_EEEE_highR9->Write();
  hmassRatio_EEEE_highR9_inner->Write();
  hmassRatio_EEEE_highR9_outer->Write();
  hmassRatio_EEEE_lowR9->Write();
  hmassRatio_EEEE_lowR9_inner->Write();
  hmassRatio_EEEE_lowR9_outer->Write();

  p_regrCorr_vs_r9_EB_highR9->Write();
  p_regrCorr_vs_r9_EE_highR9->Write();
  p_regrCorr_vs_sietaieta_EB_highR9->Write();
  p_regrCorr_vs_sietaieta_EE_highR9->Write();
  p_regrCorr_vs_etaWidth_EB_highR9->Write();
  p_regrCorr_vs_etaWidth_EE_highR9->Write();
  p_regrCorr_vs_r9_EB_lowR9->Write();
  p_regrCorr_vs_r9_EE_lowR9->Write();
  p_regrCorr_vs_sietaieta_EB_lowR9->Write();
  p_regrCorr_vs_sietaieta_EE_lowR9->Write();
  p_regrCorr_vs_etaWidth_EB_lowR9->Write();
  p_regrCorr_vs_etaWidth_EE_lowR9->Write();
  p_regrCorr_vs_eta_highR9->Write();
  p_regrCorr_vs_eta_lowR9->Write();

   
  cout << "Closing file..." << endl;
  fout->Close();

  
}
