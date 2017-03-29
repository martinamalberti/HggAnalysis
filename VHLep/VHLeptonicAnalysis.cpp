// per compilare: g++ -Wall -o VHLeptonicAnalysis `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit VHLeptonicAnalysis.cpp

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
  int maxnjets = 2;
  float minDiphoMVA = -1.;
  
  // arguments: sample (vh, vbf, ggh, etc...), tag (WHLeptonic, etc...), filepath(trees/blabla/...)
  string sample(argv[1]);
  string tag(argv[2]);
  string filepath(argv[3]);

  string filename = filepath+"/output_"+sample+".root";
  string treename = tag+"Dumper/trees/"+sample+"_13TeV_"+tag;
  //if (tag == "VHLeptonicLooseTag")
  //  treename = tag+"Dumper/trees/"+sample+"_13TeV_"+tag+"Dumper";
  cout << "Analyzing file : " << filename.c_str() <<endl;
  cout << "Analyzing tree : " << treename.c_str() <<endl;

  TFile *file = TFile::Open(filename.c_str());
    
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
      sample.find("qcd") != std::string::npos ||
      sample.find("DYJetsToLL") != std::string::npos ||
      sample.find("ZGTo2LG") != std::string::npos ||
      sample.find("WGToLNuG") != std::string::npos ||
      sample.find("WZTo2L2Q") != std::string::npos ||
      sample.find("ZZTo2L2Q") != std::string::npos ||
      sample.find("WW") != std::string::npos ||
      sample.find("TTGG_0Jets") != std::string::npos ||
      sample.find("TTGJets") != std::string::npos ||
      sample.find("TGJets") != std::string::npos ||
      sample.find("TTJets") != std::string::npos 
      )
    {
      isBkg = true;
    }

  bool doPromptFake = false;
  bool doFakeFake = false;
  
  if ( argc > 4 ){
    doPromptFake = atoi(argv[4]);
    doFakeFake = atoi(argv[5]);
  }


  TTree* tree = (TTree*)file->Get(treename.c_str());
  bool isZHLep = false;
  if (treename.find("ZHLep") != std::string::npos){
    isZHLep = true;
  }

  
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
  Float_t         njets;
  Float_t         met_pt;
  Float_t         met_phi;
  Float_t         mu1_pt;
  Float_t         mu1_eta;
  Float_t         mu1_phi;
  Float_t         mu1_energy;
  Float_t         ele1_pt;
  Float_t         ele1_eta;
  Float_t         mu2_pt;
  Float_t         mu2_eta;
  Float_t         mu2_phi;
  Float_t         mu2_energy;
  Float_t         ele2_pt;
  Float_t         ele2_eta;
  
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
  tree->SetBranchAddress("mu1_pt", &mu1_pt);
  tree->SetBranchAddress("mu1_eta", &mu1_eta);
  tree->SetBranchAddress("mu1_phi", &mu1_phi);
  tree->SetBranchAddress("mu1_energy", &mu1_energy);
  tree->SetBranchAddress("ele1_pt", &ele1_pt);
  tree->SetBranchAddress("ele1_eta", &ele1_eta);
  tree->SetBranchAddress("mu2_pt", &mu2_pt);
  tree->SetBranchAddress("mu2_eta", &mu2_eta);
  //tree->SetBranchAddress("mu2_phi", &mu2_phi);
  tree->SetBranchAddress("mu2_ph1", &mu2_phi);
  tree->SetBranchAddress("mu2_energy", &mu2_energy);
  tree->SetBranchAddress("ele2_pt", &ele2_pt);
  tree->SetBranchAddress("ele2_eta", &ele2_eta);
  if (!isZHLep){
    tree->SetBranchAddress("njets", &njets);
    tree->SetBranchAddress("met_pt", &met_pt);
    tree->SetBranchAddress("met_phi", &met_phi);
  }

  // book histograms
  TH1F *hnvtx = new TH1F("hnvtx", "hnvtx" , 50,0,50);
  TH1F *hrho = new TH1F("hrho", "hrho" ,50,0,50);
  TH1F *hptgg = new TH1F("hptgg", "hptgg" ,50,0,5);
  TH1F *hpt_pho1 = new TH1F("hpt_pho1", "hpt_pho1" ,50,0,2);
  TH1F *hpt_pho2 = new TH1F("hpt_pho2", "hpt_pho2" ,50,0,2);
  TH1F *heta_pho1 = new TH1F("heta_pho1", "heta_pho1" ,50,-2.5,2.5);
  TH1F *heta_pho2 = new TH1F("heta_pho2", "heta_pho2" ,50,-2.5,2.5);
  TH1F *hIdmva_pho1 = new TH1F("hIdmva_pho1", "hIdmva_pho1" ,50,-1,1);
  TH1F *hIdmva_pho2 = new TH1F("hIdmva_pho2", "hIdmva_pho2" ,50,-1,1);
  TH1F *hnjets = new TH1F("hnjets", "hnjets" ,10,0,10);
  TH1F *hmass = new TH1F("hmass","hmass",80,100,180);
  TH1F *hmass_2 = new TH1F("hmass_2","hmass_2",320,100,180);
  TH1F *hdiphotonMVA = new TH1F("hdiphotonMVA","hdiphotonMVA",50,-1.,1.);
  TH1F *hmet = new TH1F("hmet","hmet",50,0,250);
  TH1F *hmet_phi = new TH1F("hmet_phi","hmet_phi",30,-3.15,3.15);

  TH1F *hLeadingMuonPt = new TH1F("hLeadingMuonPt", "hLeadingMuonPt", 80, 20, 100);
  TH1F *hLeadingMuonEta = new TH1F("hLeadingMuonEta", "hLeadingMuonEta", 50, -2.5, 2.5);
  TH1F *hLeadingElectronPt = new TH1F("hLeadingElectronPt", "hLeadingElectronPt", 80, 20, 100);
  TH1F *hLeadingElectronEta = new TH1F("hLeadingElectronEta", "hLeadingElectronEta", 50, -2.5, 2.5);

  TH1F *hSubleadingMuonPt = new TH1F("hSubleadingMuonPt", "hSubleadingMuonPt", 80, 20, 100);
  TH1F *hSubleadingMuonEta = new TH1F("hSubleadingMuonEta", "hSubleadingMuonEta", 50, -2.5, 2.5);
  TH1F *hSubleadingElectronPt = new TH1F("hSubleadingElectronPt", "hSubleadingElectronPt", 80, 20, 100);
  TH1F *hSubleadingElectronEta = new TH1F("hSubleadingElectronEta", "hSubleadingElectronEta", 50, -2.5, 2.5);

  TH1F *hdRMuMu = new TH1F("hdRMuMu", "hdRMuMu", 50, 0, 5);

    
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

      // selections Run1
      if ( mass < 100. || mass > 180.) continue;
      //if ( !isZHLep  && njets > maxnjets) continue;
      if (diphoton_mva < minDiphoMVA) continue;
      
      if ((isData || isBkg)  && (mass > 115. && mass < 135. )) continue;

      hnvtx->Fill(nvtx,weight);
      hrho->Fill(rho,weight);
      hptgg -> Fill(diphoton_pt/mass, weight);
      hIdmva_pho1->Fill(pho1_idmva, weight);
      hIdmva_pho2->Fill(pho2_idmva, weight);
      hpt_pho1 -> Fill(pho1_pt/mass, weight);
      hpt_pho2 -> Fill(pho2_pt/mass, weight);
      heta_pho1-> Fill(pho1_eta, weight);
      heta_pho2-> Fill(pho2_eta, weight);
      hmass -> Fill(mass,weight);
      hmass_2 -> Fill(mass,weight);
      hdiphotonMVA -> Fill(diphoton_mva, weight);

      if (!isZHLep){
	hnjets ->Fill(njets,weight);
	hmet->Fill(met_pt,weight);
	hmet_phi->Fill(met_phi,weight);
      }


      hLeadingMuonPt->Fill(mu1_pt,weight);
      hLeadingMuonEta->Fill(mu1_eta,weight);
      hLeadingElectronPt->Fill(ele1_pt,weight);
      hLeadingElectronEta->Fill(ele1_eta,weight);

      if (isZHLep){
	hSubleadingMuonPt->Fill(mu2_pt,weight);
	hSubleadingMuonEta->Fill(mu2_eta,weight);
	hSubleadingElectronPt->Fill(ele2_pt,weight);
	hSubleadingElectronEta->Fill(ele2_eta,weight);
	if (mu1_pt > 0 && mu2_pt> 0){
	  TLorentzVector mu1, mu2;
	  mu1.SetPtEtaPhiE(mu1_pt, mu1_eta, mu1_phi, mu1_energy);
	  mu2.SetPtEtaPhiE(mu2_pt, mu2_eta, mu2_phi, mu2_energy);
	  float dr = mu1.DeltaR(mu2); 
	  hdRMuMu ->Fill( dr, weight );
	}
      }
      
      ntot+=weight; 
      n++;
      
    } //end loop over events in the tree
      
  // print results
  cout << "Number of events (un-weighted)" << n <<endl;
  cout << "Number of events (weighted)"    << ntot <<endl;
  cout <<endl;

  // save histograms
  string outfilename = "histograms_"+tag+"_"+sample+".root";
  cout << "Saving in file : " <<  outfilename.c_str() <<endl;
  TFile *fout = new TFile(outfilename.c_str(),"recreate");
  hnvtx->Write();
  hrho->Write();
  hptgg -> Write();
  hpt_pho1 -> Write();
  hpt_pho2 -> Write();
  heta_pho1 -> Write();
  heta_pho2 -> Write();
  hIdmva_pho1-> Write();
  hIdmva_pho2-> Write();
  hdiphotonMVA -> Write();
  hmass->Write();
  hmass_2->Write();

  hnjets->Write();
  hmet->Write();
  hmet_phi->Write();

  hLeadingMuonPt->Write();
  hLeadingMuonEta->Write();
  hLeadingElectronPt->Write();
  hLeadingElectronEta->Write();
  
  hSubleadingMuonPt->Write();
  hSubleadingMuonEta->Write();
  hSubleadingElectronPt->Write();
  hSubleadingElectronEta->Write();

  hdRMuMu -> Write();
  
  fout->Close();

}
