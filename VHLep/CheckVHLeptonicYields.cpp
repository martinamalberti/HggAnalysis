// per compilare: g++ -Wall -o CheckVHLeptonicYields `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit CheckVHLeptonicYields.cpp

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

  string tag = argv[1];
  
  float lumi13 = 36.8;
  float lumi8  = 19.7;

  float gghxsec13 = 48.57;////cern yellow report 4 (n3lo)
  float vbfxsec13 = 3.782;//cern yellow report 4
  float wzhxsec13 = 2.2567;//cern yellow report 4
  float whxsec13 = 1.373;//cern yellow report 4
  float zhxsec13 = 0.8839;//cern yellow report 4
  float tthxsec13 = 0.5071;//cern yellow report 4

  float gghxsec8 = 19.7; // cern yellow report 3
  float vbfxsec8 = 1.578;// cern yellow report 3
  float wzhxsec8 = 1.1199;// cern yellow report 3
  float whxsec8 = 0.7026;// cern yellow report 3
  float zhxsec8 = 0.4208;// cern yellow report 3
  float tthxsec8 = 0.1293;// cern yellow report 3


  // trees by process
  vector<string> processes;
  processes.push_back("ggh");
  processes.push_back("vbf");
  processes.push_back("vh");
  processes.push_back("wh");
  processes.push_back("zh");
  processes.push_back("tth");
  processes.push_back("ggh_powheg");
  //processes.push_back("diphoton");
  
  const int nproc = processes.size(); 

  // open files
  string filepath = "trees/vhLepDumper_20170130/"; 
  
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
  Float_t         njets;
  Float_t         jet1_pt;
  Float_t         jet1_eta;
  Float_t         jet1_phi;
  Float_t         jet2_pt;
  Float_t         jet2_eta;
  Float_t         jet2_phi;
  Float_t         jet3_pt;
  Float_t         jet3_eta;
  Float_t         jet3_phi;
  
  // load trees
  for (int iproc = 0; iproc < processes.size(); iproc++){
  
    string proc = processes[iproc].c_str();
    string file = filepath;
    if (iproc == 0) file+="output_ggh.root";
    if (iproc == 1) file+="output_vbf.root";
    if (iproc == 2) file+="output_vh.root";
    if (iproc == 3) file+="output_wh.root";
    if (iproc == 4) file+="output_zh.root";
    if (iproc == 5) file+="output_tth.root";
    if (iproc == 6) file+="output_ggh_powheg.root";
    if (iproc == 7) file+="output_diphoton.root";
    cout << "Analyzing MC : " << file.c_str() <<endl;

    if (tag == "WHLeptonic") tree[iproc] = new TChain(Form("WHLeptonicTagDumper/trees/%s_13TeV_WHLeptonicTag",proc.c_str()));
    if (tag == "ZHLeptonic")    tree[iproc] = new TChain(Form("ZHLeptonicTagDumper/trees/%s_13TeV_ZHLeptonicTag",proc.c_str()));
    if (tag == "VHLeptonicLoose")    tree[iproc] = new TChain(Form("VHLeptonicLooseTagDumper/trees/%s_13TeV_VHLeptonicLooseTagDumper",proc.c_str()));
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
    if (tag != "ZHLeptonic") {
      tree[iproc]->SetBranchAddress("njets", &njets);
      tree[iproc]->SetBranchAddress("jet1_pt", &jet1_pt);
      tree[iproc]->SetBranchAddress("jet1_eta", &jet1_eta);
      tree[iproc]->SetBranchAddress("jet1_phi", &jet1_phi);
      tree[iproc]->SetBranchAddress("jet2_pt", &jet2_pt);
      tree[iproc]->SetBranchAddress("jet2_eta", &jet2_eta);
      tree[iproc]->SetBranchAddress("jet2_phi", &jet2_phi);
      tree[iproc]->SetBranchAddress("jet3_pt", &jet3_pt);
      tree[iproc]->SetBranchAddress("jet3_eta", &jet3_eta);
      tree[iproc]->SetBranchAddress("jet3_phi", &jet3_phi);
    }
  }    



      
  // book histograms
  TH1F *hptgg[nproc];
  TH1F *hpt_pho1[nproc];
  TH1F *hpt_pho2[nproc];
  TH1F *hdiphotonMVA[nproc];
  TH1F *hnjets[nproc];
  TH1F *hpt_jet1[nproc];
  TH1F *hpt_jet2[nproc];
  TH1F *hpt_jet3[nproc];
  
  for (int iproc = 0; iproc < nproc; iproc++){
    string p = processes[iproc];
    hptgg[iproc] = new TH1F(Form("hptgg_%s",p.c_str()), Form("hptgg_%s",p.c_str()) ,50,0,5);
    hpt_pho1[iproc] = new TH1F(Form("hpt_pho1_%s",p.c_str()), Form("hpt_pho1_%s",p.c_str()) ,50,0,2);
    hpt_pho2[iproc] = new TH1F(Form("hpt_pho2_%s",p.c_str()), Form("hpt_pho2_%s",p.c_str()) ,50,0,2);
    hdiphotonMVA[iproc] = new TH1F(Form("hdiphotonMVA_%s",p.c_str()), Form("hdiphotonMVA_%s",p.c_str()) ,50,-1,1);
    hnjets[iproc] = new TH1F(Form("hnjets_%s",p.c_str()), Form("hnjets_%s",p.c_str()) ,10, 0, 10);
    hpt_jet1[iproc] = new TH1F(Form("hpt_jet1_%s",p.c_str()), Form("hpt_jet1_%s",p.c_str()) ,50,0,200);
    hpt_jet2[iproc] = new TH1F(Form("hpt_jet2_%s",p.c_str()), Form("hpt_jet2_%s",p.c_str()) ,50,0,200);
    hpt_jet3[iproc] = new TH1F(Form("hpt_jet3_%s",p.c_str()), Form("hpt_jet3_%s",p.c_str()) ,50,0,200);
  }

  

  float nggh = 0.;  
  float nvbf = 0.;  
  float nwzh = 0.;  
  float nwh = 0.;  
  float nzh = 0.;  
  float ntth = 0.;  
  float nggh_powheg = 0.;

  float sumw2_ggh = 0.;  
  float sumw2_vbf = 0.;  
  float sumw2_wzh = 0.;  
  float sumw2_wh = 0.;  
  float sumw2_zh = 0.;  
  float sumw2_tth = 0.;  
  float sumw2_ggh_powheg = 0.;


  int nnggh = 0.;  
  int nnvbf = 0.;  
  int nnwzh = 0.;  
  int nnwh = 0.;  
  int nnzh = 0.;  
  int nntth = 0.;  
  int nnggh_powheg = 0.;

  // loop over trees
  for (int iproc = 0; iproc < nproc; iproc++){
	
    // loop over events
    for (int entry = 0; entry < tree[iproc]->GetEntries(); entry++){
	  
      tree[iproc]->GetEntry(entry);

      if ( candidate_id !=0 ) continue;

      hnjets[iproc]->Fill(njets,weight);
      hptgg[iproc] -> Fill(diphoton_pt/mass, weight);
      hpt_pho1[iproc] -> Fill(pho1_pt/mass, weight);
      hpt_pho2[iproc] -> Fill(pho2_pt/mass, weight);
      hdiphotonMVA[iproc] -> Fill(diphoton_mva, weight);
      if (njets > 0 ) hpt_jet1[iproc]-> Fill(jet1_pt, weight);
      if (njets > 1 ) hpt_jet2[iproc]-> Fill(jet2_pt, weight);
      if (njets > 2 ) hpt_jet3[iproc]-> Fill(jet3_pt, weight);
	
      // apply jet veto
      if (tag != "ZHLeptonic" && njets > 2 ) continue;
      
      if ( iproc == 0 ) nggh+=weight;
      if ( iproc == 1 ) nvbf+=weight;
      if ( iproc == 2 ) nwzh+=weight;
      if ( iproc == 3 ) nwh+=weight;
      if ( iproc == 4 ) nzh+=weight;
      if ( iproc == 5 ) ntth+=weight;
      if ( iproc == 6 ) nggh_powheg+=weight;

      if ( iproc == 0 ) sumw2_ggh+=weight*weight;
      if ( iproc == 1 ) sumw2_vbf+=weight*weight;
      if ( iproc == 2 ) sumw2_wzh+=weight*weight;
      if ( iproc == 3 ) sumw2_wh+=weight*weight;
      if ( iproc == 4 ) sumw2_zh+=weight*weight;
      if ( iproc == 5 ) sumw2_tth+=weight*weight;
      if ( iproc == 6 ) sumw2_ggh_powheg+=weight*weight;

      if ( iproc == 0 ) nnggh++;
      if ( iproc == 1 ) nnvbf++;
      if ( iproc == 2 ) nnwzh++;
      if ( iproc == 3 ) nnwh++;
      if ( iproc == 4 ) nnzh++;
      if ( iproc == 5 ) nntth++;
      if ( iproc == 6 ) nnggh_powheg++;

    } //end loop over events in the tree
    
  }//end loop over processes
      
  nggh*=lumi13; 
  nvbf*=lumi13; 
  nwzh*=lumi13; 
  nwh*=lumi13; 
  nzh*=lumi13; 
  ntth*=lumi13;
  nggh_powheg*=lumi13; 
  float ntot = nggh+nvbf+nwzh+ntth;
  float ntot2 = nggh_powheg+nvbf+nwzh+ntth;
  
  
  // print results
  cout << "Number of events un-weighted" <<endl;
  cout << "ggh " << nnggh <<endl;
  cout << "vbf " << nnvbf <<endl;
  cout << "wzh " << nnwzh <<endl;
  cout << "wh  " << nnwh <<endl;
  cout << "zh  " << nnzh <<endl;
  cout << "tth " << nntth <<endl;
  cout << "ggh powheg " << nnggh_powheg <<endl;

  cout <<endl;
  
  cout << "process"  << "   Number of events 13 TeV, 1 fb-1   " << "   Fraction of total "   << "  Number of events scaled to 8 TeV, 19.7 fb-1 " << endl;
  cout << "ggh_powheg "  << nggh_powheg << "  " <<  (nggh_powheg/ntot2) << "  " << nggh_powheg * (lumi8/lumi13) * (gghxsec8/gghxsec13) << endl;
  cout << "ggh        "  << nggh << "  " <<  nggh/ntot << "  " << nggh * (lumi8/lumi13) * (gghxsec8/gghxsec13) << endl;
  cout << "vbf        "  << nvbf << "  " <<  nvbf/ntot << "  " << nvbf * (lumi8/lumi13) * (vbfxsec8/vbfxsec13) << endl;
  cout << "wzh        "  << nwzh << "  " <<  nwzh/ntot << "  " << nwzh * (lumi8/lumi13) * (wzhxsec8/wzhxsec13) << endl;
  cout << " wh        "  << nwh << "  " <<  nwh/ntot << "  " << nwh * (lumi8/lumi13) * (whxsec8/whxsec13) << endl;
  cout << " zh        "  << nzh << "  " <<  nzh/ntot << "  " << nzh * (lumi8/lumi13) * (zhxsec8/zhxsec13) << endl;
  cout << "tth        "  << ntth << "  " <<  ntth/ntot << "  " << ntth * (lumi8/lumi13) * (tthxsec8/tthxsec13) << endl;
  cout << "SUM        "  << ntot <<endl;

  cout << sumw2_ggh <<endl;
  cout << "process     n events (un-weighted)   stat err" <<endl;
  cout << "wzh        "  << nnwzh <<  "   " << nwzh << "   " << sqrt(sumw2_wzh) << " ("<< (sqrt(sumw2_wzh)/nwzh)*100 << "%)" <<endl;  
  cout << "ggh        "  << nnggh <<  "   " << nggh << "   " << sqrt(sumw2_ggh) << " ("<< (sqrt(sumw2_ggh)/nggh)*100 << "%)" <<endl;  
  cout << "ggh_powheg "  << nnggh_powheg <<  "   " << nggh_powheg << "   " << sqrt(sumw2_ggh_powheg) << " ("<< (sqrt(sumw2_ggh_powheg)/nggh_powheg)*100 << "%)" <<endl;  
  cout << "vbf        "  << nnvbf <<  "   " << nvbf << "   " << sqrt(sumw2_vbf) << " ("<< (sqrt(sumw2_vbf)/nvbf)*100 << "%)" <<endl;  
  cout << "tth        "  << nntth <<  "   " << ntth << "   " << sqrt(sumw2_tth) << " ("<< (sqrt(sumw2_tth)/ntth)*100 << "%)" <<endl;  

  
  // save histograms
  string outfilename = "histograms_";
  if (tag == "WHLeptonic") outfilename+="WHLeptonicTag";
  if (tag == "VHLeptonicLoose") outfilename+="VHLeptonicLooseTag";
  if (tag == "ZHLeptonic") outfilename+="ZHLeptonicTag";
  outfilename+=".root";
     
  TFile *fout = new TFile(outfilename.c_str(),"recreate");
  for (int iproc = 0; iproc < nproc ; iproc++){
    hnjets[iproc] ->Write();
    hptgg[iproc] -> Write();
    hpt_pho1[iproc] -> Write();
    hpt_pho2[iproc] -> Write();
    hpt_jet1[iproc] -> Write();
    hpt_jet2[iproc] -> Write();
    hpt_jet3[iproc] -> Write();
    hdiphotonMVA[iproc] -> Write();
  }
  
  fout->Close();
  
}
