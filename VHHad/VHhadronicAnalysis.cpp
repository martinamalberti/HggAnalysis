// per compilare: g++ -Wall -o VHhadronicAnalysis `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit VHhadronicAnalysis.cpp

#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"
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
#include "TVector2.h"
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


//////  ====== MAIN ========================

int main(int argc, char** argv)
{

  // selections:
  float minMjj = 60.;
  float maxMjj = 110.;
  float minPtLeadJet = 40.;
  float minPtSubLeadJet = 40.;
  float maxCosthetastar = 0.5;
  float minPtgg = 1.; 
  float minDiphoMVA = 0.75;
  
  
  // loose selections
  /*float minMjj = 0.;
  float maxMjj = 9999.;
  float minPtLeadJet = 30.;
  float minPtSubLeadJet = 30.;
  float maxCosthetastar = 9999.;
  float minPtgg = 0.5; 
  float minDiphoMVA = -1.;
  */
  
  TFile *file = TFile::Open(argv[1]);
  cout << "Analyzing " << argv[1] <<endl;
  string sample(argv[1]);

  bool skipTwoPromptPhotons = false;
  if (sample.find("qcd") != std::string::npos ||
      sample.find("gjet") != std::string::npos)
    {
    skipTwoPromptPhotons = true;
    }
  
  bool isData = false;
  if (sample.find("data") != std::string::npos &&
      !(sample.find("data_cs") != std::string::npos)) {
    isData = true;
  }

  bool isDataCS = false;
  if (sample.find("data_cs") != std::string::npos){
    isDataCS = true;
  }

  bool isBkg = false;
  if (sample.find("diphoton") != std::string::npos ||
      sample.find("gjet") != std::string::npos ||
      sample.find("qcd") != std::string::npos )
    {
      isBkg = true;
    }

  bool doPromptFake = false;
  bool doFakeFake = false;
  
  if ( argc > 4 ){
    doPromptFake = atoi(argv[4]);
    doFakeFake = atoi(argv[5]);
    if ( argc > 6 ) minPtgg = atof(argv[6]);
  }


  // weights to be applied to the control sample
  TFile *fWeights =  TFile::Open("2DPtEtaWeights.root");
  TH2F *h2pt = (TH2F*)fWeights->Get("h2pt");
  TH2F *h2eta = (TH2F*)fWeights->Get("h2eta");
  
  TTree* tree = (TTree*)file->Get(argv[2]);

  Float_t         weight;
  Int_t           nvtx;
  Float_t         rho;
  Int_t           candidate_id;
  Float_t         mass;
  Float_t         diphoton_pt;
  Float_t         diphoton_mva;
  Float_t         pho1_pt;
  Float_t         pho1_eta;
  Float_t         pho1_phi;
  Float_t         pho1_energy;
  Float_t         pho1_idmva;  
  Float_t         pho1_genMatchType;
  Float_t         pho2_pt;
  Float_t         pho2_eta;
  Float_t         pho2_phi;
  Float_t         pho2_energy;
  Float_t         pho2_idmva;  
  Float_t         pho2_genMatchType;
  Float_t         jet1_pt;
  Float_t         jet1_eta;
  Float_t         jet1_phi;
  Float_t         jet1_energy;
  Float_t         jet1_qgl;
  Float_t         jet1_bdisc;
  Float_t         jet2_pt;
  Float_t         jet2_eta;
  Float_t         jet2_phi;
  Float_t         jet2_energy;
  Float_t         jet2_qgl;
  Float_t         jet2_bdisc;
  
  tree->SetBranchAddress("weight", &weight);
  tree->SetBranchAddress("nvtx", &nvtx);
  tree->SetBranchAddress("rho", &rho);
  tree->SetBranchAddress("candidate_id",&candidate_id);
  tree->SetBranchAddress("mass", &mass);
  tree->SetBranchAddress("diphoton_pt", &diphoton_pt);
  tree->SetBranchAddress("diphoton_mva", &diphoton_mva);
  tree->SetBranchAddress("pho1_pt", &pho1_pt);
  tree->SetBranchAddress("pho1_eta", &pho1_eta);
  tree->SetBranchAddress("pho1_phi", &pho1_phi);
  tree->SetBranchAddress("pho1_energy", &pho1_energy);
  tree->SetBranchAddress("pho1_idmva", &pho1_idmva);
  tree->SetBranchAddress("pho1_genMatchType", &pho1_genMatchType);
  tree->SetBranchAddress("pho2_pt", &pho2_pt);
  tree->SetBranchAddress("pho2_eta", &pho2_eta);
  tree->SetBranchAddress("pho2_phi", &pho2_phi);
  tree->SetBranchAddress("pho2_energy", &pho2_energy);
  tree->SetBranchAddress("pho2_idmva", &pho2_idmva);
  tree->SetBranchAddress("pho2_genMatchType", &pho2_genMatchType);
  tree->SetBranchAddress("jet1_pt", &jet1_pt);
  tree->SetBranchAddress("jet1_eta", &jet1_eta);
  tree->SetBranchAddress("jet1_phi", &jet1_phi);
  tree->SetBranchAddress("jet1_energy", &jet1_energy);
  tree->SetBranchAddress("jet1_qgl", &jet1_qgl);
  tree->SetBranchAddress("jet1_bdisc", &jet1_bdisc);
  tree->SetBranchAddress("jet2_pt", &jet2_pt);
  tree->SetBranchAddress("jet2_eta", &jet2_eta);
  tree->SetBranchAddress("jet2_phi", &jet2_phi);
  tree->SetBranchAddress("jet2_energy", &jet2_energy);
  tree->SetBranchAddress("jet2_qgl", &jet2_qgl);
  tree->SetBranchAddress("jet2_bdisc", &jet2_bdisc);


  // book histograms
  TH1F *hnvtx = new TH1F("hnvtx", "hnvtx" ,50,0,50);
  TH1F *hrho = new TH1F("hrho", "hrho" ,50,0,50);
  TH1F *hptgg = new TH1F("hptgg", "hptgg" ,50,0,5);
  TH1F *hpt_pho1 = new TH1F("hpt_pho1", "hpt_pho1" ,50,0,2);
  TH1F *hpt_pho2 = new TH1F("hpt_pho2", "hpt_pho2" ,50,0,2);
  TH1F *hIdmva_pho1 = new TH1F("hIdmva_pho1", "hIdmva_pho1" ,50,-1,1);
  TH1F *hIdmva_pho2 = new TH1F("hIdmva_pho2", "hIdmva_pho2" ,50,-1,1);
  TH1F *hpt_jet1 = new TH1F("hpt_jet1", "hpt_jet1" ,40,0,200);
  TH1F *hpt_jet2 = new TH1F("hpt_jet2", "hpt_jet2" ,40,0,200);
  TH1F *hmjj = new TH1F("hmjj", "hmjj" ,50,0,500);
  TH1F *hcosthetastar = new TH1F("hcosthetastar", "hcosthetastar" ,20,-1,1);
  TH1F *hQGL_jet1 = new TH1F("hQGL_jet1", "hQGL_jet1" ,20,0,1);
  TH1F *hQGL_jet2 = new TH1F("hQGL_jet2", "hQGL_jet2" ,20,0,1);
  TH1F *hbdisc_jet1 = new TH1F("hbdisc_jet1", "hbdisc_jet1" ,40,-1,1);
  TH1F *hbdisc_jet2 = new TH1F("hbdisc_jet2", "hbdisc_jet2" ,40,-1,1);
  TH1F *hmass = new TH1F("hmass","hmass",80,100,180);
  TH1F *hdiphotonMVA = new TH1F("hdiphotonMVA","hdiphotonMVA",100,-1.,1.);

  int n =0;
  float ntot = 0;
  
  // loop over events
  for (int entry = 0; entry < tree->GetEntries(); entry++){
	  
      tree->GetEntry(entry);

      // remove overlap 2 photons QCD/gjet samples
      if (skipTwoPromptPhotons && pho1_genMatchType == 1 &&  pho2_genMatchType == 1 ) continue;

      // keep only if one candidate
      if ( candidate_id !=0 ) continue;


      // do prompt-fake only components
      if ( doPromptFake ) {
	if ( pho1_genMatchType != 1 && pho2_genMatchType != 1) continue;
      }

      // do fake-fake component
      if ( doFakeFake ) {
	if ( pho1_genMatchType == 1 || pho2_genMatchType == 1) continue;
      }

      // weights for control sample
      int bin = h2pt->FindBin(pho1_pt, pho2_pt);
      float wPt = h2pt->GetBinContent(bin);
      bin = h2eta->FindBin(pho1_eta, pho2_eta);
      float wEta = h2eta->GetBinContent(bin);

      if ( isDataCS ) weight*=(wPt*wEta);

      TLorentzVector jet1, jet2, dijet, pho1, pho2, diphoton, vstar;

      pho1.SetPtEtaPhiE(pho1_pt, pho1_eta, pho1_phi, pho1_energy);
      pho2.SetPtEtaPhiE(pho2_pt, pho2_eta, pho2_phi, pho2_energy);
      jet1.SetPtEtaPhiE(jet1_pt, jet1_eta, jet1_phi, jet1_energy);
      jet2.SetPtEtaPhiE(jet2_pt, jet2_eta, jet2_phi, jet2_energy);

      diphoton = pho1 + pho2;
      dijet = jet1 + jet2;
      vstar = diphoton + dijet;
      diphoton.Boost( -vstar.BoostVector() );
      float costhetastar = -diphoton.CosTheta();
      float mjj = dijet.M();
        
      // selections Run1
      if ( mass < 100. || mass > 180.) continue;
      if ( pho1_pt/mass < 0.50 ) continue;
      if ( pho2_pt/mass < 0.25 ) continue;
      if ( mjj < minMjj || mjj > maxMjj ) continue;
      if ( jet1_pt < minPtLeadJet || jet2_pt < minPtSubLeadJet) continue;
      if ( fabs(costhetastar) > maxCosthetastar )  continue;
      if ( diphoton_pt/mass < minPtgg) continue;

      if (diphoton_mva < minDiphoMVA) continue;
      
      if ((isData || isBkg)  && (mass > 115. && mass < 135. )) continue;

      hnvtx->Fill(nvtx,weight);
      hrho->Fill(rho,weight);
      hptgg -> Fill(diphoton_pt/mass, weight);
      hIdmva_pho1->Fill(pho1_idmva, weight);
      hIdmva_pho2->Fill(pho2_idmva, weight);
      hpt_pho1 -> Fill(pho1_pt/mass, weight);
      hpt_pho2 -> Fill(pho2_pt/mass, weight);
      hpt_jet1-> Fill(jet1_pt, weight);
      hpt_jet2-> Fill(jet2_pt, weight);
      hmjj -> Fill(mjj, weight);
      hcosthetastar ->Fill(costhetastar, weight);
      hQGL_jet1->Fill(jet1_qgl, weight);
      hQGL_jet2->Fill(jet2_qgl, weight);
      hbdisc_jet1->Fill(jet1_qgl, weight);
      hbdisc_jet2->Fill(jet2_qgl, weight);
      hmass -> Fill(mass,weight);
      hdiphotonMVA -> Fill(diphoton_mva, weight);
    
      //// cut on QGL --> helps removing ggH
      //if (jet1_qgl<0.1) continue;
      //if (jet2_qgl<0.1) continue;
      
      ntot+=weight; 
      n++;
      
    } //end loop over events in the tree
      
  // print results
  cout << "Number of events (un-weighted)" << n <<endl;
  cout << "Number of events (weighted)"    << ntot <<endl;
  cout <<endl;

  // save histograms
  TFile *fout = new TFile(argv[3],"recreate");
  hnvtx->Write();
  hrho->Write();
  hptgg -> Write();
  hpt_pho1 -> Write();
  hpt_pho2 -> Write();
  hIdmva_pho1-> Write();
  hIdmva_pho2-> Write();
  hpt_jet1 -> Write();
  hpt_jet2 -> Write();
  hmjj -> Write();
  hcosthetastar -> Write();
  hQGL_jet1-> Write();
  hQGL_jet2-> Write();
  hbdisc_jet1-> Write();
  hbdisc_jet2-> Write();
  hmass->Write();
  hdiphotonMVA->Write();
  
  fout->Close();

}
