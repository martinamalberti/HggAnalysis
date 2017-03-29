// per compilare: g++ -Wall -o CheckVHHadronicYields `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit CheckVHHadronicYields.cpp

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

  float lumi13 = 1.;
  float lumi8  = 19.7;

  float gghxsec13 = 48.57;////cern yellow report 4 (n3lo)
  float vbfxsec13 = 3.782;//cern yellow report 4
  float wzhxsec13 = 2.2567;//cern yellow report 4
  float tthxsec13 = 0.5071;//cern yellow report 4

  float gghxsec8 = 19.7; // cern yellow report 3
  float vbfxsec8 = 1.578;// cern yellow report 3
  float wzhxsec8 = 1.1199;// cern yellow report 3
  float tthxsec8 = 0.1293;// cern yellow report 3


  // trees by process
  vector<string> processes;
  processes.push_back("ggh");
  processes.push_back("vbf");
  processes.push_back("vh");
  processes.push_back("tth");
  processes.push_back("ggh_powheg");
  processes.push_back("diphoton");
  
  const int nproc = processes.size(); 

  // open files
  string filepath = "trees/vhHadDumper_20161223/"; 
  
  TChain* tree[nproc];
  Float_t         weight;
  Int_t           candidate_id;
  Float_t         mass;
  Float_t         diphoton_pt;
  Float_t         diphoton_mva;
  Float_t         pho1_pt;
  Float_t         pho1_eta;
  Float_t         pho1_phi;
  Float_t         pho1_energy;
  Float_t         pho2_pt;
  Float_t         pho2_eta;
  Float_t         pho2_phi;
  Float_t         pho2_energy;
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
  
  // load trees
  for (int iproc = 0; iproc < processes.size(); iproc++){
  
    string proc = processes[iproc].c_str();
    string file = filepath;
    if (iproc == 0) file+="output_ggh.root";
    if (iproc == 1) file+="output_vbf.root";
    if (iproc == 2) file+="output_vh.root";
    if (iproc == 3) file+="output_tth.root";
    if (iproc == 4) file+="output_ggh_powheg.root";
    if (iproc == 5) file+="output_diphoton.root";
    cout << "Analyzing MC : " << file.c_str() <<endl;

    tree[iproc] = new TChain(Form("vhHadTagDumper/trees/%s_13TeV_VHHadronicTag",proc.c_str()));
    tree[iproc]->Add(file.c_str());
      
    tree[iproc]->SetBranchAddress("weight", &weight);
    tree[iproc]->SetBranchAddress("candidate_id",&candidate_id);
    tree[iproc]->SetBranchAddress("mass", &mass);
    tree[iproc]->SetBranchAddress("diphoton_pt", &diphoton_pt);
    tree[iproc]->SetBranchAddress("diphoton_mva", &diphoton_mva);
    tree[iproc]->SetBranchAddress("pho1_pt", &pho1_pt);
    tree[iproc]->SetBranchAddress("pho1_eta", &pho1_eta);
    tree[iproc]->SetBranchAddress("pho1_phi", &pho1_phi);
    tree[iproc]->SetBranchAddress("pho1_energy", &pho1_energy);
    tree[iproc]->SetBranchAddress("pho2_pt", &pho2_pt);
    tree[iproc]->SetBranchAddress("pho2_eta", &pho2_eta);
    tree[iproc]->SetBranchAddress("pho2_phi", &pho2_phi);
    tree[iproc]->SetBranchAddress("pho2_energy", &pho2_energy);
    tree[iproc]->SetBranchAddress("jet1_pt", &jet1_pt);
    tree[iproc]->SetBranchAddress("jet1_eta", &jet1_eta);
    tree[iproc]->SetBranchAddress("jet1_phi", &jet1_phi);
    tree[iproc]->SetBranchAddress("jet1_energy", &jet1_energy);
    tree[iproc]->SetBranchAddress("jet1_qgl", &jet1_qgl);
    tree[iproc]->SetBranchAddress("jet1_bdisc", &jet1_bdisc);
    tree[iproc]->SetBranchAddress("jet2_pt", &jet2_pt);
    tree[iproc]->SetBranchAddress("jet2_eta", &jet2_eta);
    tree[iproc]->SetBranchAddress("jet2_phi", &jet2_phi);
    tree[iproc]->SetBranchAddress("jet2_energy", &jet2_energy);
    tree[iproc]->SetBranchAddress("jet2_qgl", &jet2_qgl);
    tree[iproc]->SetBranchAddress("jet2_bdisc", &jet2_bdisc);
  }    



      
  // book histograms
  TH1F *hptgg[nproc];
  TH1F *hpt_pho1[nproc];
  TH1F *hpt_pho2[nproc];
  TH1F *hpt_jet1[nproc];
  TH1F *hpt_jet2[nproc];
  TH1F *hmjj[nproc];
  TH1F *hcosthetastar[nproc];
  TH1F *hQGL_jet1[nproc];
  TH1F *hQGL_jet2[nproc];
  TH1F *hbdisc_jet1[nproc];
  TH1F *hbdisc_jet2[nproc];
  TH1F *hdiphotonMVA[nproc];
 
  for (int iproc = 0; iproc < nproc; iproc++){
    string p = processes[iproc];
    //hptgg[iproc] = new TH1F(Form("hptgg_%s",p.c_str()), Form("hptgg_%s",p.c_str()) ,50,0,500);
    hptgg[iproc] = new TH1F(Form("hptgg_%s",p.c_str()), Form("hptgg_%s",p.c_str()) ,50,0,5);
    hpt_pho1[iproc] = new TH1F(Form("hpt_pho1_%s",p.c_str()), Form("hpt_pho1_%s",p.c_str()) ,50,0,2);
    hpt_pho2[iproc] = new TH1F(Form("hpt_pho2_%s",p.c_str()), Form("hpt_pho2_%s",p.c_str()) ,50,0,2);
    hpt_jet1[iproc] = new TH1F(Form("hpt_jet1_%s",p.c_str()), Form("hpt_jet1_%s",p.c_str()) ,100,0,200);
    hpt_jet2[iproc] = new TH1F(Form("hpt_jet2_%s",p.c_str()), Form("hpt_jet2_%s",p.c_str()) ,100,0,200);
    hmjj[iproc] = new TH1F(Form("hmjj_%s",p.c_str()), Form("hmjj_%s",p.c_str()) ,50,0,500);
    hcosthetastar[iproc] = new TH1F(Form("hcosthetastar_%s",p.c_str()), Form("hcosthetastar_%s",p.c_str()) ,20,-1,1);
    hQGL_jet1[iproc] = new TH1F(Form("hQGL_jet1_%s",p.c_str()), Form("hQGL_jet1_%s",p.c_str()) ,20,0,1);
    hQGL_jet2[iproc] = new TH1F(Form("hQGL_jet2_%s",p.c_str()), Form("hQGL_jet2_%s",p.c_str()) ,20,0,1);
    hbdisc_jet1[iproc] = new TH1F(Form("hbdisc_jet1_%s",p.c_str()), Form("hbdisc_jet1_%s",p.c_str()) ,20,0,1);
    hbdisc_jet2[iproc] = new TH1F(Form("hbdisc_jet2_%s",p.c_str()), Form("hbdisc_jet2_%s",p.c_str()) ,20,0,1);
    hdiphotonMVA[iproc] = new TH1F(Form("hdiphotonMVA_%s",p.c_str()), Form("hdiphotonMVA_%s",p.c_str()) ,100,-1,1);
  }

  
  float nggh = 0.;  
  float nvbf = 0.;  
  float nwzh = 0.;  
  float ntth = 0.;  
  float nggh_powheg = 0.;


  int nnggh = 0.;  
  int nnvbf = 0.;  
  int nnwzh = 0.;  
  int nntth = 0.;  
  int nnggh_powheg = 0.;

  // loop over trees
  for (int iproc = 0; iproc < nproc; iproc++){
	
    // loop over events
    for (int entry = 0; entry < tree[iproc]->GetEntries(); entry++){
	  
      tree[iproc]->GetEntry(entry);

      if ( candidate_id !=0 ) continue;

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

      //if ( pho1_pt < 33. ) continue;
      //if ( pho2_pt < 25. ) continue;
 
      //hptgg[iproc] -> Fill(diphoton_pt, weight);
      hptgg[iproc] -> Fill(diphoton_pt/mass, weight);
      hpt_pho1[iproc] -> Fill(pho1_pt/mass, weight);
      hpt_pho2[iproc] -> Fill(pho2_pt/mass, weight);
      hpt_jet1[iproc]-> Fill(jet1_pt, weight);
      hpt_jet2[iproc]-> Fill(jet2_pt, weight);
      hmjj[iproc] -> Fill(mjj, weight);
      hcosthetastar[iproc] ->Fill(costhetastar, weight);
        
      // selections Run1
      if ( pho1_pt/mass < 0.50 ) continue;
      if ( pho2_pt/mass < 0.25 ) continue;
      if ( mjj < 60. || mjj > 120. ) continue;
      if ( jet1_pt < 40. || jet2_pt < 40. ) continue;
      if ( fabs(costhetastar) > 0.5 )  continue;
      if ( diphoton_pt/mass < 13./12 ) continue;

      hdiphotonMVA[iproc]->Fill(diphoton_mva,weight);
  
      hQGL_jet1[iproc]->Fill(jet1_qgl, weight);
      hQGL_jet2[iproc]->Fill(jet2_qgl, weight);

      hbdisc_jet1[iproc]->Fill(jet1_bdisc,weight);
      hbdisc_jet2[iproc]->Fill(jet2_bdisc,weight);
      	
      //// cut on QGL --> helps removing ggH
      //if (jet1_qgl<0.1) continue;
      //if (jet2_qgl<0.1) continue;

      //if ( jet1_bdisc > 0.8 || jet2_bdisc > 0.8) continue;
      //if ( jet1_bdisc < 0.8 && jet2_bdisc < 0.8) continue;
      
      if ( iproc == 0 ) nggh+=weight;
      if ( iproc == 1 ) nvbf+=weight;
      if ( iproc == 2 ) nwzh+=weight;
      if ( iproc == 3 ) ntth+=weight;
      if ( iproc == 4 ) nggh_powheg+=weight;

      if ( iproc == 0 ) nnggh++;
      if ( iproc == 1 ) nnvbf++;
      if ( iproc == 2 ) nnwzh++;
      if ( iproc == 3 ) nntth++;
      if ( iproc == 4 ) nnggh_powheg++;

    } //end loop over events in the tree
    
  }//end loop over processes
      
  nggh*=lumi13; 
  nvbf*=lumi13; 
  nwzh*=lumi13; 
  ntth*=lumi13;
  nggh_powheg*=lumi13; 
  float ntot = nggh+nvbf+nwzh+ntth;
  float ntot2 = nggh_powheg+nvbf+nwzh+ntth;
  
  
  // print results
  cout << "Number of events un-weighted" <<endl;
  cout << "ggh " << nnggh <<endl;
  cout << "vbf " << nnvbf <<endl;
  cout << "wzh " << nnwzh <<endl;
  cout << "tth " << nntth <<endl;
  cout << "ggh powheg " << nnggh_powheg <<endl;

  cout <<endl;
  
  cout << "process"  << "   Number of events 13 TeV, 1 fb-1   " << "   Fraction of total "   << "  Number of events scaled to 8 TeV, 19.7 fb-1 " << endl;
  cout << "ggh_powheg "  << nggh_powheg << "  " <<  nggh_powheg/ntot2 << "  " << nggh_powheg * (lumi8/lumi13) * (gghxsec8/gghxsec13) << endl;
  cout << "ggh        "  << nggh << "  " <<  nggh/ntot << "  " << nggh * (lumi8/lumi13) * (gghxsec8/gghxsec13) << endl;
  cout << "vbf        "  << nvbf << "  " <<  nvbf/ntot << "  " << nvbf * (lumi8/lumi13) * (vbfxsec8/vbfxsec13) << endl;
  cout << "wzh        "  << nwzh << "  " <<  nwzh/ntot << "  " << nwzh * (lumi8/lumi13) * (wzhxsec8/wzhxsec13) << endl;
  cout << "tth        "  << ntth << "  " <<  ntth/ntot << "  " << ntth * (lumi8/lumi13) * (tthxsec8/tthxsec13) << endl;
  cout << "SUM        "  << ntot <<endl;


  // save histograms
  TFile *fout = new TFile("histograms_vhHad.root","recreate");
  for (int iproc = 0; iproc < nproc ; iproc++){
    hptgg[iproc] -> Write();
    hpt_pho1[iproc] -> Write();
    hpt_pho2[iproc] -> Write();
    hpt_jet1[iproc] -> Write();
    hpt_jet2[iproc] -> Write();
    hmjj[iproc] -> Write();
    hcosthetastar[iproc] -> Write();
    hdiphotonMVA[iproc] -> Write();
    hQGL_jet1[iproc]-> Write();
    hQGL_jet2[iproc]-> Write();
    hbdisc_jet1[iproc]-> Write();
    hbdisc_jet2[iproc]-> Write();

  }
  
  fout->Close();

}
