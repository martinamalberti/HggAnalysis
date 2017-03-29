// per compilare: g++ -Wall -o ZeeValidation `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit ZeeValidation.cpp

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
  return w;
  
}


float GetRhoWeight(float rho, TH1F *h){
  
  int bin = h->FindBin(rho);
  float w = h->GetBinContent(bin);
  return w;
  
}


// ****************** MAIN ******************
int main(int argc, char** argv)
{
  float minEt1   = 35.;
  float minEt2   = 25.;
  float minMass = 70.;
  float maxMass = 110.;
  float r9cut = 0.94;
  float minrho = -999999.;
  float maxrho = 999999.;
  //float minrho = 0.;
  //float maxrho = 10.;
  
  float  dIDMVA_EB = 0.03; // for phoID syst.
  float  dIDMVA_EE = 0.03; // for phoID syst.


  
  TFile *file = TFile::Open(argv[1]);
  cout << "Analyzing " << argv[1] <<endl;

  bool isData = atoi(argv[3]);

  /*bool doNvtxWeights = false;
  TFile *fw;
  TH1F *hweights;
  if (doNvtxWeights){

    string fname = argv[1];
    if (fname.find("Run2016B")!= std::string::npos) fw = TFile::Open("macros/nvtx_weights_2016BTo2016B.root");
    if (fname.find("Run2016C")!= std::string::npos) fw = TFile::Open("macros/nvtx_weights_2016CTo2016B.root");
    if (fname.find("Run2016D")!= std::string::npos) fw = TFile::Open("macros/nvtx_weights_2016DTo2016B.root");
    if (fname.find("Run2016E")!= std::string::npos) fw = TFile::Open("macros/nvtx_weights_2016ETo2016B.root");
    if (fname.find("Run2016F")!= std::string::npos) fw = TFile::Open("macros/nvtx_weights_2016FTo2016B.root");
    if (fname.find("Run2016G")!= std::string::npos) fw = TFile::Open("macros/nvtx_weights_2016GTo2016B.root");
    if (fname.find("DY")      != std::string::npos) fw = TFile::Open("macros/nvtx_weights_MCTo2016B.root");
    hweights = (TH1F*)fw->Get("hweights");
    }*/
  
  bool doRhoWeights = true;
  TFile *fw;
  TH1F *hweights;
  if (!isData && doRhoWeights){
    string weightTo = argv[4];
    if (weightTo.find("2016BtoH")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016.root");
    if (weightTo.find("2016B")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016B.root");
    if (weightTo.find("2016C")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016C.root");
    if (weightTo.find("2016D")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016D.root");
    if (weightTo.find("2016E")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016E.root");
    if (weightTo.find("2016F")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016F.root");
    if (weightTo.find("2016G")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016G.root");
    //if (weightTo.find("2016H")!= std::string::npos) fw = TFile::Open("macros/rho_weights_MCTo2016H.root");
    if (weightTo.find("2016H")!= std::string::npos) fw = TFile::Open("macros/rho_weights_DYJetsToLLTo2016H.root");
    //if (weightTo.find("2016H")!= std::string::npos) fw = TFile::Open("macros/rho_weights_DYToEE_EGM0_To2016H.root");
    cout << "Weighting MC to " << weightTo << " data rho, using file :" << fw->GetName() <<endl;    
    hweights = (TH1F*)fw->Get("hweights");
  }
  
 
  TTree* tree = (TTree*)file->Get("diphotonDumper/trees/tree_13TeV_All");

  
  // Declaration of leaf types
  UInt_t           run;
  Int_t           candidate_id;
  Float_t         weight;
  Float_t         mass;
  Float_t         pt;
  Float_t         diphotonMVA;
  Float_t         ele1_et;
  Float_t         ele1_rawEnergy;
  Float_t         ele1_energy;
  Float_t         ele1_scEta;
  Float_t         ele1_scPhi;
  Float_t         ele1_eta;
  Float_t         ele1_phi;
  Float_t         ele1_r9;
  Float_t         ele1_full5x5_r9;
  Float_t         ele1_full5x5_sigmaIetaIeta;
  Float_t         ele1_etaWidth;
  Float_t         ele1_s4;
  Float_t         ele1_sigmaEoE;
  Float_t         ele1_pfPhoIso03;
  Float_t         ele1_ESoRawE;
  Float_t         ele1_idmva;
  Float_t         ele1_kHasSwitchToGain6;
  Float_t         ele1_kHasSwitchToGain1;
  Float_t         ele1_uncorr_full5x5_r9;
  Float_t         ele1_uncorr_full5x5_sigmaIetaIeta;
  Float_t         ele1_uncorr_etaWidth;
  Float_t         ele1_uncorr_s4;

  
  Float_t         ele2_et;
  Float_t         ele2_rawEnergy;
  Float_t         ele2_energy;
  Float_t         ele2_scEta;
  Float_t         ele2_scPhi;
  Float_t         ele2_eta;
  Float_t         ele2_phi;
  Float_t         ele2_full5x5_r9;
  Float_t         ele2_full5x5_sigmaIetaIeta;
  Float_t         ele2_etaWidth;
  Float_t         ele2_s4;
  Float_t         ele2_sigmaEoE;
  Float_t         ele2_pfPhoIso03;
  Float_t         ele2_ESoRawE;
  Float_t         ele2_r9;
  Float_t         ele2_idmva;
  Float_t         ele2_kHasSwitchToGain6;
  Float_t         ele2_kHasSwitchToGain1;
  Float_t         ele2_uncorr_full5x5_r9;
  Float_t         ele2_uncorr_full5x5_sigmaIetaIeta;
  Float_t         ele2_uncorr_etaWidth;
  Float_t         ele2_uncorr_s4;
  
  Int_t           nvtx;
  Float_t         rho;

  Bool_t          HLT_Ele27_WPTight_Gsf_v1;
  Bool_t          HLT_Ele27_WPTight_Gsf_v2;
  Bool_t          HLT_Ele27_WPTight_Gsf_v3;
  Bool_t          HLT_Ele27_WPTight_Gsf_v4;
  Bool_t          HLT_Ele27_WPTight_Gsf_v5;
  Bool_t          HLT_Ele27_WPTight_Gsf_v6;
  Bool_t          HLT_Ele27_WPTight_Gsf_v7;

  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v1;
  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v2;
  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v3;
  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v4;
  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v5;
  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v6;
  Bool_t          HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v7;
  
  tree->SetBranchAddress("weight", &weight);
  tree->SetBranchAddress("run", &run);
  tree->SetBranchAddress("nvtx", &nvtx);
  tree->SetBranchAddress("rho", &rho);
  tree->SetBranchAddress("candidate_id", &candidate_id);

  tree->SetBranchAddress("mass", &mass);
  tree->SetBranchAddress("pt", &pt);
  tree->SetBranchAddress("diphotonMVA", &diphotonMVA);

  tree->SetBranchAddress("ele1_rawEnergy", &ele1_rawEnergy);
  tree->SetBranchAddress("ele1_energy", &ele1_energy);
  tree->SetBranchAddress("ele1_et", &ele1_et);
  tree->SetBranchAddress("ele1_scEta", &ele1_scEta);
  tree->SetBranchAddress("ele1_scPhi", &ele1_scPhi);
  tree->SetBranchAddress("ele1_eta", &ele1_eta);
  tree->SetBranchAddress("ele1_phi", &ele1_phi);
  tree->SetBranchAddress("ele1_r9", &ele1_r9);
  tree->SetBranchAddress("ele1_full5x5_r9", &ele1_full5x5_r9);
  tree->SetBranchAddress("ele1_full5x5_sigmaIetaIeta", &ele1_full5x5_sigmaIetaIeta);
  tree->SetBranchAddress("ele1_etaWidth", &ele1_etaWidth);
  tree->SetBranchAddress("ele1_s4", &ele1_s4);
  tree->SetBranchAddress("ele1_sigmaEoE", &ele1_sigmaEoE);
  tree->SetBranchAddress("ele1_pfPhoIso03", &ele1_pfPhoIso03);
  tree->SetBranchAddress("ele1_ESoRawE", &ele1_ESoRawE);
  tree->SetBranchAddress("ele1_idmva", &ele1_idmva);
  tree->SetBranchAddress("ele1_kHasSwitchToGain6", &ele1_kHasSwitchToGain6);
  tree->SetBranchAddress("ele1_kHasSwitchToGain1", &ele1_kHasSwitchToGain1);
  tree->SetBranchAddress("ele2_rawEnergy", &ele2_rawEnergy);
  tree->SetBranchAddress("ele2_energy", &ele2_energy);
  tree->SetBranchAddress("ele2_et", &ele2_et);
  tree->SetBranchAddress("ele2_scEta", &ele2_scEta);
  tree->SetBranchAddress("ele2_scPhi", &ele2_scPhi);
  tree->SetBranchAddress("ele2_eta", &ele2_eta);
  tree->SetBranchAddress("ele2_phi", &ele2_phi);
  tree->SetBranchAddress("ele2_r9", &ele2_r9);
  tree->SetBranchAddress("ele2_full5x5_r9", &ele2_full5x5_r9);
  tree->SetBranchAddress("ele2_full5x5_sigmaIetaIeta", &ele2_full5x5_sigmaIetaIeta);
  tree->SetBranchAddress("ele2_etaWidth", &ele2_etaWidth);
  tree->SetBranchAddress("ele2_s4", &ele2_s4);
  tree->SetBranchAddress("ele2_sigmaEoE", &ele2_sigmaEoE);
  tree->SetBranchAddress("ele2_pfPhoIso03", &ele2_pfPhoIso03);
  tree->SetBranchAddress("ele2_ESoRawE", &ele2_ESoRawE);
  tree->SetBranchAddress("ele2_idmva", &ele2_idmva);
  tree->SetBranchAddress("ele2_kHasSwitchToGain6", &ele2_kHasSwitchToGain6);
  tree->SetBranchAddress("ele2_kHasSwitchToGain1", &ele2_kHasSwitchToGain1);
 
  if (!isData){
    tree->SetBranchAddress("ele1_uncorr_full5x5_r9", &ele1_uncorr_full5x5_r9);
    tree->SetBranchAddress("ele1_uncorr_full5x5_sigmaIetaIeta", &ele1_uncorr_full5x5_sigmaIetaIeta);
    tree->SetBranchAddress("ele1_uncorr_etaWidth", &ele1_uncorr_etaWidth);
    tree->SetBranchAddress("ele1_uncorr_s4", &ele1_uncorr_s4);
    tree->SetBranchAddress("ele2_uncorr_full5x5_r9", &ele2_uncorr_full5x5_r9);
    tree->SetBranchAddress("ele2_uncorr_full5x5_sigmaIetaIeta", &ele2_uncorr_full5x5_sigmaIetaIeta);
    tree->SetBranchAddress("ele2_uncorr_etaWidth", &ele2_uncorr_etaWidth);
    tree->SetBranchAddress("ele2_uncorr_s4", &ele2_uncorr_s4);
  }
 
  //tree->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v1", &HLT_Ele35_WPLoose_Gsf_v1);
  //tree->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v2", &HLT_Ele35_WPLoose_Gsf_v2);
  //tree->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v3", &HLT_Ele35_WPLoose_Gsf_v3);
  //tree->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v4", &HLT_Ele35_WPLoose_Gsf_v4);
  //tree->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v5", &HLT_Ele35_WPLoose_Gsf_v5);
  //tree->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v6", &HLT_Ele35_WPLoose_Gsf_v6);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v1", &HLT_Ele27_WPTight_Gsf_v1);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v2", &HLT_Ele27_WPTight_Gsf_v2);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v3", &HLT_Ele27_WPTight_Gsf_v3);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v4", &HLT_Ele27_WPTight_Gsf_v4);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v5", &HLT_Ele27_WPTight_Gsf_v5);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v6", &HLT_Ele27_WPTight_Gsf_v6);
  tree->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v7", &HLT_Ele27_WPTight_Gsf_v7);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v1", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v1);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v2", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v2);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v3", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v3);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v4", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v4);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v5", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v5);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v6", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v6);
  tree->SetBranchAddress("HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v7", &HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v7);

  // book histograms
  TH1F *hnvtx = new TH1F("hnvtx","hnvtx",100,0,100);
  hnvtx->GetXaxis()->SetTitle("number of reconstructed vertices");
  
  TH1F *hrho = new TH1F("hrho","rho",50,0,50);
  hrho->GetXaxis()->SetTitle("rho");

  TH2F *hrho_vs_nvtx = new TH2F("hrho_vs_nvtx","hrho_vs_nvtx",50,0,50,50,0,50);
  TProfile *rho_vs_nvtx = new TProfile("rho_vs_nvtx","rho_vs_nvtx",50,0,50,0,50);
  
  TH1F *hpt   = new TH1F("hpt","hpt",200,0,200);
  TH1F *hdiphotonMVA   = new TH1F("hdiphotonMVA","hdiphotonMVA",100,-1,1);
  TH1F *hdiphotonMVA_EBEB   = new TH1F("hdiphotonMVA_EBEB","hdiphotonMVA_EBEB",100,-1,1);
  TH1F *hdiphotonMVA_EEEE   = new TH1F("hdiphotonMVA_EEEE","hdiphotonMVA_EEEE",100,-1,1);
  TH1F *hmass = new TH1F("hmass","hmass",160,70,110);

  TH1F *hmass_EBEB_BothHighR9 = new TH1F("hmass_EBEB_BothHighR9","hmass_EBEB_BothHighR9",160,70,110);
  TH1F *hmass_EBEB_NotBothHighR9 = new TH1F("hmass_EBEB_NotBothHighR9","hmass_EBEB_NotBothHighR9",160,70,110);
  TH1F *hmass_NotEBEB_BothHighR9 = new TH1F("hmass_NotEBEB_BothHighR9","hmass_NotEBEB_BothHighR9",160,70,110);
  TH1F *hmass_NotEBEB_NotBothHighR9 = new TH1F("hmass_NotEBEB_NotBothHighR9","hmass_NotEBEB_NotBothHighR9",160,70,110);


  TH1F *hmass_EBEB        = new TH1F("hmass_EBEB","hmass_EBEB",160,70,110);
  TH1F *hmass_EBEB_highR9 = new TH1F("hmass_EBEB_highR9","hmass_EBEB_highR9",160,70,110);
  TH1F *hmass_EBEB_highR9_inner = new TH1F("hmass_EBEB_highR9_inner","hmass_EBEB_highR9_inner",160,70,110);
  TH1F *hmass_EBEB_highR9_outer = new TH1F("hmass_EBEB_highR9_outer","hmass_EBEB_highR9_outer",160,70,110);
  TH1F *hmass_EBEB_lowR9  = new TH1F("hmass_EBEB_lowR9","hmass_EBEB_lowR9",160,70,110);
  TH1F *hmass_EBEB_lowR9_inner = new TH1F("hmass_EBEB_lowR9_inner","hmass_EBEB_lowR9_inner",160,70,110);
  TH1F *hmass_EBEB_lowR9_outer = new TH1F("hmass_EBEB_lowR9_outer","hmass_EBEB_lowR9_outer",160,70,110);

  TH1F *hmass_EBEB_G12G6 = new TH1F("hmass_EBEB_G12G6","hmass_EBEB_G12G6",160,70,110);
  TH1F *hmass_EBEB_G12G1 = new TH1F("hmass_EBEB_G12G1","hmass_EBEB_G12G1",160,70,110);
  
  TH1F *hmass_EEEE = new TH1F("hmass_EEEE","hmass_EEEE",160,70,110);
  TH1F *hmass_EEEE_highR9 = new TH1F("hmass_EEEE_highR9","hmass_EEEE_highR9",160,70,110);
  TH1F *hmass_EEEE_highR9_inner = new TH1F("hmass_EEEE_highR9_inner","hmass_EEEE_highR9_inner",160,70,110);
  TH1F *hmass_EEEE_highR9_outer = new TH1F("hmass_EEEE_highR9_outer","hmass_EEEE_highR9_outer",160,70,110);
  TH1F *hmass_EEEE_lowR9  = new TH1F("hmass_EEEE_lowR9","hmass_EEEE_lowR9",160,70,110);
  TH1F *hmass_EEEE_lowR9_inner = new TH1F("hmass_EEEE_lowR9_inner","hmass_EEEE_lowR9_inner",160,70,110);
  TH1F *hmass_EEEE_lowR9_outer = new TH1F("hmass_EEEE_lowR9_outer","hmass_EEEE_lowR9_outer",160,70,110);

  TH1F *hlead_eta = new TH1F("hlead_eta","hlead_eta",100,-3,3);
  TH1F *hlead_eta_highR9 = new TH1F("hlead_eta_highR9","hlead_eta_highR9",100,-3,3);
  TH1F *hlead_eta_lowR9 = new TH1F("hlead_eta_lowR9","hlead_eta_lowR9",100,-3,3);

  TH1F *hlead_pt_EB = new TH1F("hlead_pt_EB","hlead_pt_EB",70,30,100);
  TH1F *hlead_pt_EE = new TH1F("hlead_pt_EE","hlead_pt_EE",70,30,100);

  TH1F *hlead_rawEnergy_EB = new TH1F("hlead_rawEnergy_EB","hlead_rawEnergy_EB",500,0,500);
  TH1F *hlead_rawEnergy_EE = new TH1F("hlead_rawEnergy_EE","hlead_rawEnergy_EE",500,0,500);
  
  TH1F *hlead_r9_EB = new TH1F("hlead_r9_EB","hlead_r9_EB",200,0.5,1);
  TH1F *hlead_r9_EE = new TH1F("hlead_r9_EE","hlead_r9_EE",200,0.5,1);

  TH1F *hlead_full5x5_r9_EB = new TH1F("hlead_full5x5_r9_EB","hlead_full5x5_r9_EB",200,0.5,1);
  TH1F *hlead_full5x5_r9_EE = new TH1F("hlead_full5x5_r9_EE","hlead_full5x5_r9_EE",200,0.5,1);
  
  TH1F *hlead_etaWidth_EB = new TH1F("hlead_etaWidth_EB","hlead_etaWidth_EB",200,0.0,0.02);
  TH1F *hlead_etaWidth_EE = new TH1F("hlead_etaWidth_EE","hlead_etaWidth_EE",200,0.0,0.05);

  TH1F *hlead_s4_EB = new TH1F("hlead_s4_EB","hlead_s4_EB",200,0.0,1.0);
  TH1F *hlead_s4_EE = new TH1F("hlead_s4_EE","hlead_s4_EE",200,0.0,1.0);

  TH1F *hlead_full5x5_sigmaIetaIeta_EB = new TH1F("hlead_full5x5_sigmaIetaIeta_EB","hlead_full5x5_sigmaIetaIeta_EB",200,0.0,0.015);
  TH1F *hlead_full5x5_sigmaIetaIeta_EE = new TH1F("hlead_full5x5_sigmaIetaIeta_EE","hlead_full5x5_sigmaIetaIeta_EE",200,0.0,0.035);

  TH1F *hlead_uncorr_full5x5_r9_EB = new TH1F("hlead_uncorr_full5x5_r9_EB","hlead_uncorr_full5x5_r9_EB",200,0.5,1);
  TH1F *hlead_uncorr_full5x5_r9_EE = new TH1F("hlead_uncorr_full5x5_r9_EE","hlead_uncorr_full5x5_r9_EE",200,0.5,1);
  
  TH1F *hlead_uncorr_etaWidth_EB = new TH1F("hlead_uncorr_etaWidth_EB","hlead_uncorr_etaWidth_EB",200,0.0,0.02);
  TH1F *hlead_uncorr_etaWidth_EE = new TH1F("hlead_uncorr_etaWidth_EE","hlead_uncorr_etaWidth_EE",200,0.0,0.05);

  TH1F *hlead_uncorr_s4_EB = new TH1F("hlead_uncorr_s4_EB","hlead_uncorr_s4_EB",200,0.0,1.0);
  TH1F *hlead_uncorr_s4_EE = new TH1F("hlead_uncorr_s4_EE","hlead_uncorr_s4_EE",200,0.0,1.0);

  TH1F *hlead_uncorr_full5x5_sigmaIetaIeta_EB = new TH1F("hlead_uncorr_full5x5_sigmaIetaIeta_EB","hlead_uncorr_full5x5_sigmaIetaIeta_EB",200,0.0,0.015);
  TH1F *hlead_uncorr_full5x5_sigmaIetaIeta_EE = new TH1F("hlead_uncorr_full5x5_sigmaIetaIeta_EE","hlead_uncorr_full5x5_sigmaIetaIeta_EE",200,0.0,0.035);
  
  TH1F *hlead_pfPhoIso03_EB = new TH1F("hlead_pfPhoIso03_EB","hlead_pfPhoIso03_EB",200,0.0,10.0);
  TH1F *hlead_pfPhoIso03_EE = new TH1F("hlead_pfPhoIso03_EE","hlead_pfPhoIso03_EE",200,0.0,10.0);

  TH1F *hlead_ESoRawE_EB = new TH1F("hlead_ESoRawE_EB","hlead_ESoRawE_EB",200,0.0,1.0);
  TH1F *hlead_ESoRawE_EE = new TH1F("hlead_ESoRawE_EE","hlead_ESoRawE_EE",200,0.0,1.0);

  TH1F *hlead_sigmaEoE_EB = new TH1F("hlead_sigmaEoE_EB","hlead_sigmaEoE_EB",200,0.0,0.06);
  TH1F *hlead_sigmaEoE_EE = new TH1F("hlead_sigmaEoE_EE","hlead_sigmaEoE_EE",200,0.0,0.06);
  
  TH1F *hlead_idmva_EB = new TH1F("hlead_idmva_EB","hlead_idmva_EB",200,-1,1);
  TH1F *hlead_idmva_EE = new TH1F("hlead_idmva_EE","hlead_idmva_EE",200,-1,1);

  TH1F *hlead_idmva_EB_sysUp = new TH1F("hlead_idmva_EB_sysUp","hlead_idmva_EB_sysUp",200,-1,1);
  TH1F *hlead_idmva_EE_sysUp = new TH1F("hlead_idmva_EE_sysUp","hlead_idmva_EE_sysUp",200,-1,1);

  TH1F *hlead_idmva_EB_sysDown = new TH1F("hlead_idmva_EB_sysDown","hlead_idmva_EB_sysDown",200,-1,1);
  TH1F *hlead_idmva_EE_sysDown = new TH1F("hlead_idmva_EE_sysDown","hlead_idmva_EE_sysDown",200,-1,1);

  
  TH1F *hsublead_eta = new TH1F("hsublead_eta","hsublead_eta",100,-3,3);
  TH1F *hsublead_eta_highR9 = new TH1F("hsublead_eta_highR9","hsublead_eta_highR9",100,-3,3);
  TH1F *hsublead_eta_lowR9 = new TH1F("hsublead_eta_lowR9","hsublead_eta_lowR9",100,-3,3);

  TH1F *hsublead_pt_EB = new TH1F("hsublead_pt_EB","hsublead_pt_EB",80,20,100);
  TH1F *hsublead_pt_EE = new TH1F("hsublead_pt_EE","hsublead_pt_EE",80,20,100);

  TH1F *hsublead_rawEnergy_EB = new TH1F("hsublead_rawEnergy_EB","hsublead_rawEnergy_EB",500,0,500);
  TH1F *hsublead_rawEnergy_EE = new TH1F("hsublead_rawEnergy_EE","hsublead_rawEnergy_EE",500,0,500);
  
  TH1F *hsublead_r9_EB = new TH1F("hsublead_r9_EB","hsublead_r9_EB",200,0.5,1);
  TH1F *hsublead_r9_EE = new TH1F("hsublead_r9_EE","hsublead_r9_EE",200,0.5,1);
  
  TH1F *hsublead_full5x5_r9_EB = new TH1F("hsublead_full5x5_r9_EB","hsublead_full5x5_r9_EB",200,0.5,1);
  TH1F *hsublead_full5x5_r9_EE = new TH1F("hsublead_full5x5_r9_EE","hsublead_full5x5_r9_EE",200,0.5,1);

  TH1F *hsublead_etaWidth_EB = new TH1F("hsublead_etaWidth_EB","hsublead_etaWidth_EB",200,0.0,0.02);
  TH1F *hsublead_etaWidth_EE = new TH1F("hsublead_etaWidth_EE","hsublead_etaWidth_EE",200,0.0,0.05);

  TH1F *hsublead_s4_EB = new TH1F("hsublead_s4_EB","hsublead_s4_EB",200,0.0,1.0);
  TH1F *hsublead_s4_EE = new TH1F("hsublead_s4_EE","hsublead_s4_EE",200,0.0,1.0);

  TH1F *hsublead_full5x5_sigmaIetaIeta_EB = new TH1F("hsublead_full5x5_sigmaIetaIeta_EB","hsublead_full5x5_sigmaIetaIeta_EB",200,0.0,0.015);
  TH1F *hsublead_full5x5_sigmaIetaIeta_EE = new TH1F("hsublead_full5x5_sigmaIetaIeta_EE","hsublead_full5x5_sigmaIetaIeta_EE",200,0.0,0.035);

  TH1F *hsublead_uncorr_full5x5_r9_EB = new TH1F("hsublead_uncorr_full5x5_r9_EB","hsublead_uncorr_full5x5_r9_EB",200,0.5,1);
  TH1F *hsublead_uncorr_full5x5_r9_EE = new TH1F("hsublead_uncorr_full5x5_r9_EE","hsublead_uncorr_full5x5_r9_EE",200,0.5,1);

  TH1F *hsublead_uncorr_etaWidth_EB = new TH1F("hsublead_uncorr_etaWidth_EB","hsublead_uncorr_etaWidth_EB",200,0.0,0.02);
  TH1F *hsublead_uncorr_etaWidth_EE = new TH1F("hsublead_uncorr_etaWidth_EE","hsublead_uncorr_etaWidth_EE",200,0.0,0.05);

  TH1F *hsublead_uncorr_s4_EB = new TH1F("hsublead_uncorr_s4_EB","hsublead_uncorr_s4_EB",200,0.0,1.0);
  TH1F *hsublead_uncorr_s4_EE = new TH1F("hsublead_uncorr_s4_EE","hsublead_uncorr_s4_EE",200,0.0,1.0);

  TH1F *hsublead_uncorr_full5x5_sigmaIetaIeta_EB = new TH1F("hsublead_uncorr_full5x5_sigmaIetaIeta_EB","hsublead_uncorr_full5x5_sigmaIetaIeta_EB",200,0.0,0.015);
  TH1F *hsublead_uncorr_full5x5_sigmaIetaIeta_EE = new TH1F("hsublead_uncorr_full5x5_sigmaIetaIeta_EE","hsublead_uncorr_full5x5_sigmaIetaIeta_EE",200,0.0,0.035);

  TH1F *hsublead_pfPhoIso03_EB = new TH1F("hsublead_pfPhoIso03_EB","hsublead_pfPhoIso03_EB",200,0.0,10.0);
  TH1F *hsublead_pfPhoIso03_EE = new TH1F("hsublead_pfPhoIso03_EE","hsublead_pfPhoIso03_EE",200,0.0,10.0);

  TH1F *hsublead_ESoRawE_EB = new TH1F("hsublead_ESoRawE_EB","hsublead_ESoRawE_EB",200,0.0,1.0);
  TH1F *hsublead_ESoRawE_EE = new TH1F("hsublead_ESoRawE_EE","hsublead_ESoRawE_EE",200,0.0,1.0);
  
  TH1F *hsublead_sigmaEoE_EB = new TH1F("hsublead_sigmaEoE_EB","hsublead_sigmaEoE_EB",200,0.0,0.06);
  TH1F *hsublead_sigmaEoE_EE = new TH1F("hsublead_sigmaEoE_EE","hsublead_sigmaEoE_EE",200,0.0,0.06);

  TH1F *hsublead_idmva_EB = new TH1F("hsublead_idmva_EB","hsublead_idmva_EB",200,-1,1);
  TH1F *hsublead_idmva_EE = new TH1F("hsublead_idmva_EE","hsublead_idmva_EE",200,-1,1);

  TH1F *hlead_full5x5_r9[6];
  TH1F *hsublead_full5x5_r9[6];
  TH1F *hlead_r9[6];
  TH1F *hsublead_r9[6];
  for (int i = 0; i < 6; i++){
    hlead_full5x5_r9[i] = new TH1F(Form("hlead_full5x5_r9_%02d",i), Form("hlead_full5x5_r9_%02d",i),200,0.5,1);
    hsublead_full5x5_r9[i] = new TH1F(Form("hsublead_full5x5_r9_%02d",i), Form("hsublead_full5x5_r9_%02d",i),200,0.5,1);
    hlead_r9[i] = new TH1F(Form("hlead_r9_%02d",i), Form("hlead_r9_%02d",i),200,0.5,1);
    hsublead_r9[i] = new TH1F(Form("hsublead_r9_%02d",i), Form("hsublead_r9_%02d",i),200,0.5,1);
  }
  
  cout << "Number of events to be analyzed : " << tree->GetEntries() <<endl;
  cout <<endl;

    
  for (int i=0; i<tree->GetEntries(); i++) {

    tree -> GetEntry(i);
    if (i%1000000==0) cout << "Analyzing event "<< i <<endl;

    if (isData){
    //if (isData && (run<272007 || run >275376)) continue; //2016B
    //if (isData && (run<275657 || run >276283)) continue; //2016C
    //if (isData && (run<276315 || run >276811)) continue; //2016D
    //if (isData && (run<276831 || run >277420)) continue; //2016E
    //if (isData && (run<277772 || run >278808)) continue; //2016F
    //if (isData && (run<278820 || run >280385)) continue; //2016G
    //if (isData && (run<280919 || run >284044)) continue; //2016H
    //if (isData && (run < 276811) ) continue; //2016EtoH
    }

    
    // take only the first diphoton pair (highest SumpT) if there is more than one diphoton candidate
    if (candidate_id !=0 ) continue; 

    // trigger
    //if (isData && !HLT_Ele35_WPLoose_Gsf_v1 && !HLT_Ele35_WPLoose_Gsf_v2 && !HLT_Ele35_WPLoose_Gsf_v3 && !HLT_Ele35_WPLoose_Gsf_v4 && !HLT_Ele35_WPLoose_Gsf_v5 && !HLT_Ele35_WPLoose_Gsf_v6) continue;
    //if (isData && !HLT_Ele27_WPTight_Gsf_v1 && !HLT_Ele27_WPTight_Gsf_v2 && !HLT_Ele27_WPTight_Gsf_v3 && !HLT_Ele27_WPTight_Gsf_v4 && !HLT_Ele27_WPTight_Gsf_v5 && !HLT_Ele27_WPTight_Gsf_v6 && !HLT_Ele27_WPTight_Gsf_v7 ) continue;
    if (!HLT_Ele27_WPTight_Gsf_v1 && !HLT_Ele27_WPTight_Gsf_v2 && !HLT_Ele27_WPTight_Gsf_v3 && !HLT_Ele27_WPTight_Gsf_v4 && !HLT_Ele27_WPTight_Gsf_v5 && !HLT_Ele27_WPTight_Gsf_v6 && !HLT_Ele27_WPTight_Gsf_v7 ) continue;
    /*if (!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v1 &&
	!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v2 &&
	!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v3 &&
	!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v4 &&
	!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v5 &&
	!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v6 &&
	!HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v7 ) continue;*/

    // weights
    float w = weight;

    if (isData) weight = 1;
    
    //if (!isData && doNvtxWeights) w*=GetNvtxWeight(nvtx,hweights);
    if (!isData && doRhoWeights) w*=GetRhoWeight(rho,hweights);

    float eta1 = ele1_scEta;
    float eta2 = ele2_scEta;

    // selections                  
    if ( ele1_et < minEt1  ) continue;                                                         
    if ( ele2_et < minEt2  ) continue;                                                         
    if ( mass    < minMass ) continue;
    if ( mass    > maxMass ) continue;

    if (rho < minrho || rho > maxrho) continue;

   
    hnvtx->Fill(nvtx,w);
    hrho->Fill(rho,w);
    hrho_vs_nvtx->Fill(nvtx, rho, w);
    rho_vs_nvtx->Fill(nvtx, rho, w);
    
    hpt ->Fill(pt,w);
    hmass ->Fill(mass,w);
    hdiphotonMVA->Fill(diphotonMVA,w);
    if (fabs(eta1)<1.5 && fabs(eta2)<1.5){
      hdiphotonMVA_EBEB->Fill(diphotonMVA,w);
    }
    if (fabs(eta1)>1.5 && fabs(eta2)>1.5){
      hdiphotonMVA_EEEE->Fill(diphotonMVA,w);
    }
      
      
    //mass in 4 cats
    if (fabs(eta1)<1.5 && fabs(eta2)<1.5){
      if (ele1_full5x5_r9 > r9cut && ele2_full5x5_r9 > r9cut) 
	hmass_EBEB_BothHighR9 ->Fill(mass,w);
      else 
	hmass_EBEB_NotBothHighR9 ->Fill(mass,w);
    }
    else{
      if (ele1_full5x5_r9 > r9cut && ele2_full5x5_r9 > r9cut) 
	hmass_NotEBEB_BothHighR9 ->Fill(mass,w);
      else 
	hmass_NotEBEB_NotBothHighR9 ->Fill(mass,w);
    }

    // plots to check events with gainSwitch
    if (fabs(eta1)<1.5 && fabs(eta2)<1.5){
      // G12-G6 or G6-G12
      if (ele1_kHasSwitchToGain6 && !ele1_kHasSwitchToGain1 && !ele2_kHasSwitchToGain6 && !ele2_kHasSwitchToGain1) hmass_EBEB_G12G6 -> Fill(mass,w);
      if (ele2_kHasSwitchToGain6 && !ele2_kHasSwitchToGain1 && !ele1_kHasSwitchToGain6 && !ele1_kHasSwitchToGain1) hmass_EBEB_G12G6 -> Fill(mass,w);

      // G12-G1 or G1-G12
      if (ele1_kHasSwitchToGain1 && !ele1_kHasSwitchToGain6 && !ele2_kHasSwitchToGain1 && !ele2_kHasSwitchToGain6) hmass_EBEB_G12G1 -> Fill(mass,w);
      if (ele2_kHasSwitchToGain1 && !ele2_kHasSwitchToGain6 && !ele1_kHasSwitchToGain1 && !ele1_kHasSwitchToGain6) hmass_EBEB_G12G1 -> Fill(mass,w);
    }
    
    
    // both electrons in the same cat.
    if (fabs(eta1)<1.5 && fabs(eta2)<1.5 ) {
      hmass_EBEB->Fill(mass,w);
      if (ele1_full5x5_r9 > r9cut && ele2_full5x5_r9 > r9cut) {
	hmass_EBEB_highR9->Fill(mass,w);
	if (fabs(eta1)<1.0 && fabs(eta2)<1.0) hmass_EBEB_highR9_inner->Fill(mass,w);
	if (fabs(eta1)>1.0 && fabs(eta2)>1.0) hmass_EBEB_highR9_outer->Fill(mass,w);
      }
      if (ele1_full5x5_r9 < r9cut && ele2_full5x5_r9 < r9cut) {
	hmass_EBEB_lowR9->Fill(mass,w);
	if (fabs(eta1)<1.0 && fabs(eta2)<1.0) hmass_EBEB_lowR9_inner->Fill(mass,w);
        if (fabs(eta1)>1.0 && fabs(eta2)>1.0) hmass_EBEB_lowR9_outer->Fill(mass,w);
      }
    }
    if (fabs(eta1)>1.5 && fabs(eta2)>1.5 ) {
      hmass_EEEE->Fill(mass,w);
      if (ele1_full5x5_r9 > r9cut && ele2_full5x5_r9 > r9cut) {
	hmass_EEEE_highR9->Fill(mass,w);
	if (fabs(eta1)<2.0 && fabs(eta2)<2.0) hmass_EEEE_highR9_inner->Fill(mass,w);
        if (fabs(eta1)>2.0 && fabs(eta2)>2.0) hmass_EEEE_highR9_outer->Fill(mass,w);
      }	
      if (ele1_full5x5_r9 < r9cut && ele2_full5x5_r9 < r9cut) {
	hmass_EEEE_lowR9->Fill(mass,w);
     	if (fabs(eta1)<2.0 && fabs(eta2)<2.0) hmass_EEEE_lowR9_inner->Fill(mass,w);
        if (fabs(eta1)>2.0 && fabs(eta2)>2.0) hmass_EEEE_lowR9_outer->Fill(mass,w);
      }
    }


    // photon ID vars
    if (fabs(eta1)<1.5){
      hlead_rawEnergy_EB ->Fill(ele1_rawEnergy,w);
      hlead_pt_EB ->Fill(ele1_et,w);
      hlead_r9_EB   ->Fill(ele1_r9,w);
      hlead_full5x5_r9_EB   ->Fill(ele1_full5x5_r9,w);
      hlead_etaWidth_EB   ->Fill(ele1_etaWidth,w);
      hlead_s4_EB   ->Fill(ele1_s4,w);
      hlead_full5x5_sigmaIetaIeta_EB   ->Fill(ele1_full5x5_sigmaIetaIeta,w);
      if (!isData) {
	hlead_uncorr_full5x5_r9_EB   ->Fill(ele1_uncorr_full5x5_r9,w);
	hlead_uncorr_etaWidth_EB   ->Fill(ele1_uncorr_etaWidth,w);
	hlead_uncorr_s4_EB   ->Fill(ele1_uncorr_s4,w);
	hlead_uncorr_full5x5_sigmaIetaIeta_EB   ->Fill(ele1_uncorr_full5x5_sigmaIetaIeta,w);
      }
      else{
	hlead_uncorr_full5x5_r9_EB   ->Fill(ele1_full5x5_r9,w);
	hlead_uncorr_etaWidth_EB   ->Fill(ele1_etaWidth,w);
	hlead_uncorr_s4_EB   ->Fill(ele1_s4,w);
	hlead_uncorr_full5x5_sigmaIetaIeta_EB   ->Fill(ele1_full5x5_sigmaIetaIeta,w);
      }
      hlead_sigmaEoE_EB   ->Fill(ele1_sigmaEoE,w);
      hlead_pfPhoIso03_EB   ->Fill(ele1_pfPhoIso03,w);
      hlead_ESoRawE_EB   ->Fill(ele1_ESoRawE,w);
      hlead_idmva_EB   ->Fill(ele1_idmva,w);
      hlead_idmva_EB_sysUp -> Fill(ele1_idmva + dIDMVA_EB,w);
      hlead_idmva_EB_sysDown -> Fill(ele1_idmva - dIDMVA_EB,w);
      hlead_eta   ->Fill(eta1,w);
      if (ele1_full5x5_r9 > r9cut)
	hlead_eta_highR9   ->Fill(eta1,w);
      else
	 hlead_eta_lowR9   ->Fill(eta1,w);
    }
    else{
      hlead_rawEnergy_EE ->Fill(ele1_rawEnergy,w);
      hlead_pt_EE ->Fill(ele1_et,w);
      hlead_r9_EE   ->Fill(ele1_r9,w);
      hlead_full5x5_r9_EE   ->Fill(ele1_full5x5_r9,w);
      hlead_etaWidth_EE   ->Fill(ele1_etaWidth,w);
      hlead_s4_EE   ->Fill(ele1_s4,w);
      hlead_full5x5_sigmaIetaIeta_EE   ->Fill(ele1_uncorr_full5x5_sigmaIetaIeta,w);
      if (!isData) {
	hlead_uncorr_full5x5_r9_EE   ->Fill(ele1_uncorr_full5x5_r9,w);
	hlead_uncorr_etaWidth_EE   ->Fill(ele1_uncorr_etaWidth,w);
	hlead_uncorr_s4_EE   ->Fill(ele1_uncorr_s4,w);
	hlead_uncorr_full5x5_sigmaIetaIeta_EE   ->Fill(ele1_uncorr_full5x5_sigmaIetaIeta,w);
      }
      else{
	hlead_uncorr_full5x5_r9_EE   ->Fill(ele1_full5x5_r9,w);
	hlead_uncorr_etaWidth_EE   ->Fill(ele1_etaWidth,w);
	hlead_uncorr_s4_EE   ->Fill(ele1_s4,w);
	hlead_uncorr_full5x5_sigmaIetaIeta_EE   ->Fill(ele1_full5x5_sigmaIetaIeta,w);
      }
      hlead_sigmaEoE_EE   ->Fill(ele1_sigmaEoE,w);
      hlead_pfPhoIso03_EE   ->Fill(ele1_pfPhoIso03,w);
      hlead_ESoRawE_EE   ->Fill(ele1_ESoRawE,w);
      hlead_idmva_EE   ->Fill(ele1_idmva,w);
      hlead_idmva_EE_sysUp -> Fill(ele1_idmva + dIDMVA_EE,w);
      hlead_idmva_EE_sysDown -> Fill(ele1_idmva - dIDMVA_EE,w);
      hlead_eta   ->Fill(eta1,w);
      if (ele1_full5x5_r9 > r9cut)
	   hlead_eta_highR9   ->Fill(eta1,w);
      else
	 hlead_eta_lowR9   ->Fill(eta1,w);
    }

    if (fabs(eta2)<1.5){
      hsublead_rawEnergy_EB ->Fill(ele2_rawEnergy,w);
      hsublead_pt_EB ->Fill(ele2_et,w);
      hsublead_r9_EB   ->Fill(ele2_r9,w);
      hsublead_full5x5_r9_EB   ->Fill(ele2_full5x5_r9,w);
      hsublead_etaWidth_EB   ->Fill(ele2_etaWidth,w);
      hsublead_s4_EB   ->Fill(ele2_s4,w);
      hsublead_full5x5_sigmaIetaIeta_EB   ->Fill(ele2_full5x5_sigmaIetaIeta,w);
      hsublead_uncorr_full5x5_r9_EB   ->Fill(ele2_uncorr_full5x5_r9,w);
      hsublead_uncorr_etaWidth_EB   ->Fill(ele2_uncorr_etaWidth,w);
      hsublead_uncorr_s4_EB   ->Fill(ele2_uncorr_s4,w);
      hsublead_uncorr_full5x5_sigmaIetaIeta_EB   ->Fill(ele2_uncorr_full5x5_sigmaIetaIeta,w);
      hsublead_sigmaEoE_EB   ->Fill(ele2_sigmaEoE,w);
      hsublead_pfPhoIso03_EB   ->Fill(ele2_pfPhoIso03,w);
      hsublead_ESoRawE_EB   ->Fill(ele2_ESoRawE,w);
      hsublead_idmva_EB   ->Fill(ele2_idmva,w);
      hsublead_eta   ->Fill(eta2,w);
      if (ele2_full5x5_r9 > r9cut)
	hsublead_eta_highR9   ->Fill(eta2,w);
      else
	 hsublead_eta_lowR9   ->Fill(eta2,w);
    }
    else{
      hsublead_rawEnergy_EE ->Fill(ele2_rawEnergy,w);
      hsublead_pt_EE ->Fill(ele2_et,w);
      hsublead_r9_EE   ->Fill(ele2_r9,w);
      hsublead_full5x5_r9_EE   ->Fill(ele2_full5x5_r9,w);
      hsublead_etaWidth_EE   ->Fill(ele2_etaWidth,w);
      hsublead_s4_EE   ->Fill(ele2_s4,w);
      hsublead_full5x5_sigmaIetaIeta_EE   ->Fill(ele2_full5x5_sigmaIetaIeta,w);
      hsublead_uncorr_full5x5_r9_EE   ->Fill(ele2_uncorr_full5x5_r9,w);
      hsublead_uncorr_etaWidth_EE   ->Fill(ele2_uncorr_etaWidth,w);
      hsublead_uncorr_s4_EE   ->Fill(ele2_uncorr_s4,w);
      hsublead_uncorr_full5x5_sigmaIetaIeta_EE   ->Fill(ele2_uncorr_full5x5_sigmaIetaIeta,w);
      hsublead_sigmaEoE_EE   ->Fill(ele2_sigmaEoE,w);
      hsublead_pfPhoIso03_EE   ->Fill(ele2_pfPhoIso03,w);
      hsublead_ESoRawE_EE   ->Fill(ele2_ESoRawE,w);
      hsublead_idmva_EE   ->Fill(ele2_idmva,w);
      hsublead_eta   ->Fill(eta2,w);
      if (ele2_full5x5_r9 > r9cut)
	hsublead_eta_highR9   ->Fill(eta2,w);
      else
	 hsublead_eta_lowR9   ->Fill(eta2,w);
    }


    // r9 in finer eta bins
   
    if ( fabs(eta1) >= 0.0 && fabs(eta1) < 0.4 ) hlead_full5x5_r9[0]->Fill(ele1_full5x5_r9,w);
    if ( fabs(eta1) >= 0.4 && fabs(eta1) < 0.8 ) hlead_full5x5_r9[1]->Fill(ele1_full5x5_r9,w);
    if ( fabs(eta1) >= 0.8 && fabs(eta1) < 1.2 ) hlead_full5x5_r9[2]->Fill(ele1_full5x5_r9,w);
    if ( fabs(eta1) >= 1.2 && fabs(eta1) < 1.4 ) hlead_full5x5_r9[3]->Fill(ele1_full5x5_r9,w);
    if ( fabs(eta1) >= 1.5 && fabs(eta1) < 2.0 ) hlead_full5x5_r9[4]->Fill(ele1_full5x5_r9,w);
    if ( fabs(eta1) >= 2.0 && fabs(eta1) < 2.5 ) hlead_full5x5_r9[5]->Fill(ele1_full5x5_r9,w);

    if ( fabs(eta2) >= 0.0 && fabs(eta2) < 0.4 ) hsublead_full5x5_r9[0]->Fill(ele2_full5x5_r9,w);
    if ( fabs(eta2) >= 0.4 && fabs(eta2) < 0.8 ) hsublead_full5x5_r9[1]->Fill(ele2_full5x5_r9,w);
    if ( fabs(eta2) >= 0.8 && fabs(eta2) < 1.2 ) hsublead_full5x5_r9[2]->Fill(ele2_full5x5_r9,w);
    if ( fabs(eta2) >= 1.2 && fabs(eta2) < 1.4 ) hsublead_full5x5_r9[3]->Fill(ele2_full5x5_r9,w);
    if ( fabs(eta2) >= 1.5 && fabs(eta2) < 2.0 ) hsublead_full5x5_r9[4]->Fill(ele2_full5x5_r9,w);
    if ( fabs(eta2) >= 2.0 && fabs(eta2) < 2.5 ) hsublead_full5x5_r9[5]->Fill(ele2_full5x5_r9,w);


    if ( fabs(eta1) >= 0.0 && fabs(eta1) < 0.4 ) hlead_r9[0]->Fill(ele1_r9,w);
    if ( fabs(eta1) >= 0.4 && fabs(eta1) < 0.8 ) hlead_r9[1]->Fill(ele1_r9,w);
    if ( fabs(eta1) >= 0.8 && fabs(eta1) < 1.2 ) hlead_r9[2]->Fill(ele1_r9,w);
    if ( fabs(eta1) >= 1.2 && fabs(eta1) < 1.4 ) hlead_r9[3]->Fill(ele1_r9,w);
    if ( fabs(eta1) >= 1.5 && fabs(eta1) < 2.0 ) hlead_r9[4]->Fill(ele1_r9,w);
    if ( fabs(eta1) >= 2.0 && fabs(eta1) < 2.5 ) hlead_r9[5]->Fill(ele1_r9,w);

    if ( fabs(eta2) >= 0.0 && fabs(eta2) < 0.4 ) hsublead_r9[0]->Fill(ele2_r9,w);
    if ( fabs(eta2) >= 0.4 && fabs(eta2) < 0.8 ) hsublead_r9[1]->Fill(ele2_r9,w);
    if ( fabs(eta2) >= 0.8 && fabs(eta2) < 1.2 ) hsublead_r9[2]->Fill(ele2_r9,w);
    if ( fabs(eta2) >= 1.2 && fabs(eta2) < 1.4 ) hsublead_r9[3]->Fill(ele2_r9,w);
    if ( fabs(eta2) >= 1.5 && fabs(eta2) < 2.0 ) hsublead_r9[4]->Fill(ele2_r9,w);
    if ( fabs(eta2) >= 2.0 && fabs(eta2) < 2.5 ) hsublead_r9[5]->Fill(ele2_r9,w);



    
  }// end loop over events
  
  
  TFile *fout = new TFile(argv[2],"recreate");

  hnvtx->Write();
  hrho->Write();
  hrho_vs_nvtx->Write();
  rho_vs_nvtx->Write();
  
  hpt->Write();
  hdiphotonMVA->Write();
  hdiphotonMVA_EBEB->Write();
  hdiphotonMVA_EEEE->Write();
  hmass->Write();
  hmass_EBEB_BothHighR9->Write();
  hmass_EBEB_NotBothHighR9->Write();
  hmass_NotEBEB_BothHighR9->Write();
  hmass_NotEBEB_NotBothHighR9->Write();


  hmass_EBEB->Write();
  hmass_EBEB_highR9->Write();
  hmass_EBEB_highR9_inner->Write();
  hmass_EBEB_highR9_outer->Write();
  hmass_EBEB_lowR9->Write();
  hmass_EBEB_lowR9_inner->Write();
  hmass_EBEB_lowR9_outer->Write();
  hmass_EEEE->Write();
  hmass_EEEE_highR9->Write();
  hmass_EEEE_highR9_inner->Write();
  hmass_EEEE_highR9_outer->Write();
  hmass_EEEE_lowR9->Write();
  hmass_EEEE_lowR9_inner->Write();
  hmass_EEEE_lowR9_outer->Write();

  hmass_EBEB_G12G6 ->Write();
  hmass_EBEB_G12G1 ->Write();
  
  
  hlead_eta->Write();
  hlead_eta_highR9->Write();
  hlead_eta_lowR9->Write();

  hlead_rawEnergy_EB -> Write();
  hlead_pt_EB -> Write();
  hlead_r9_EB -> Write();
  hlead_full5x5_r9_EB -> Write();
  hlead_etaWidth_EB -> Write();
  hlead_s4_EB -> Write();
  hlead_full5x5_sigmaIetaIeta_EB -> Write();
  hlead_uncorr_full5x5_r9_EB -> Write();
  hlead_uncorr_etaWidth_EB -> Write();
  hlead_uncorr_s4_EB -> Write();
  hlead_uncorr_full5x5_sigmaIetaIeta_EB -> Write();
  hlead_sigmaEoE_EB -> Write();
  hlead_pfPhoIso03_EB -> Write();
  hlead_ESoRawE_EB -> Write();
  hlead_idmva_EB -> Write();
  hlead_idmva_EB_sysUp -> Write();
  hlead_idmva_EB_sysDown -> Write();

  hlead_rawEnergy_EE -> Write();
  hlead_pt_EE -> Write();
  hlead_r9_EE -> Write();
  hlead_full5x5_r9_EE -> Write();
  hlead_etaWidth_EE -> Write();
  hlead_s4_EE -> Write();
  hlead_full5x5_sigmaIetaIeta_EE -> Write();
  hlead_uncorr_full5x5_r9_EE -> Write();
  hlead_uncorr_etaWidth_EE -> Write();
  hlead_uncorr_s4_EE -> Write();
  hlead_uncorr_full5x5_sigmaIetaIeta_EE -> Write();
  hlead_sigmaEoE_EE -> Write();
  hlead_pfPhoIso03_EE -> Write();
  hlead_ESoRawE_EE -> Write();
  hlead_idmva_EE -> Write();
  hlead_idmva_EE_sysUp -> Write();
  hlead_idmva_EE_sysDown -> Write();

  
  hsublead_eta->Write();
  hsublead_eta_highR9->Write();
  hsublead_eta_lowR9->Write();

  hsublead_rawEnergy_EB -> Write();
  hsublead_pt_EB -> Write();
  hsublead_r9_EB -> Write();

  hsublead_full5x5_r9_EB -> Write();
  hsublead_etaWidth_EB -> Write();
  hsublead_s4_EB -> Write();
  hsublead_full5x5_sigmaIetaIeta_EB -> Write();
  hsublead_uncorr_full5x5_r9_EB -> Write();
  hsublead_uncorr_etaWidth_EB -> Write();
  hsublead_uncorr_s4_EB -> Write();
  hsublead_uncorr_full5x5_sigmaIetaIeta_EB -> Write();
  hsublead_sigmaEoE_EB -> Write();
  hsublead_pfPhoIso03_EB -> Write();
  hsublead_ESoRawE_EB -> Write();
  hsublead_idmva_EB -> Write();

  hsublead_rawEnergy_EE -> Write();
  hsublead_pt_EE -> Write();
  hsublead_r9_EE -> Write();
  hsublead_full5x5_r9_EE -> Write();
  hsublead_etaWidth_EE -> Write();
  hsublead_s4_EE -> Write();
  hsublead_full5x5_sigmaIetaIeta_EE -> Write();
  hsublead_uncorr_full5x5_r9_EE -> Write();
  hsublead_uncorr_etaWidth_EE -> Write();
  hsublead_uncorr_s4_EE -> Write();
  hsublead_uncorr_full5x5_sigmaIetaIeta_EE -> Write();
  hsublead_pfPhoIso03_EE -> Write();
  hsublead_ESoRawE_EE -> Write();
  hsublead_sigmaEoE_EE -> Write();
  hsublead_idmva_EE -> Write();


  for (int i=0; i < 6; i++){
    hlead_full5x5_r9[i]->Write();
    hsublead_full5x5_r9[i]->Write();

    hlead_r9[i]->Write();
    hsublead_r9[i]->Write();
  }
  
  cout << "Closing file..." << endl;
  fout->Close();

  
}
