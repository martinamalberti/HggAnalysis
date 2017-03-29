// per compilare: g++ -Wall -o LinearityZee `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit LinearityZee.cpp

#include "./histoFunc.h"
#include "Utilities.h"

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

float GetWeight(int nvtx, TH1F *h){

  int bin = h->FindBin(nvtx);
  float w = h->GetBinContent(bin);
  //cout << nvtx <<endl;
  return w;

}

//////  ====== MAIN ========================

int main(int argc, char** argv)
{

  bool scaleDataErrors = true;

  float minEt1   = 35.;
  float minEt2   = 25.;
  float minMass = 70.; 
  float maxDphi = 2.5;
  float MZ = 91.188;
  float intLumi = 35.9;
  
  bool applyHLT = true;// for 80X require single electron trigger

  bool useRawEnergy = false;
  
  bool useOddEvents = false;
  bool useEvenEvents = false;

  bool doNvtxWeights = false;
  TFile *fw;
  TH1F *hweights;
  if (doNvtxWeights){
    fw = TFile::Open("macros/nvtx_weights.root");
    hweights = (TH1F*)fw->Get("hweights");
  }

  bool doZptWeights = false;
  TFile *fw2;
  TH1F *hweightsPt;
  if (doZptWeights){
    fw2 = TFile::Open("macros/zpt_weights.root");
    hweightsPt = (TH1F*)fw2->Get("hweights");
  }


  string fileDA = argv[1];
  cout << "Analyzing Data : " << argv[1] <<endl;

  string fileMC= argv[2];
  cout << "Analyzing MC : " << argv[2] <<endl;

  TChain* treeMC = new TChain("diphotonDumper/trees/tree_13TeV_All");
  //TChain* treeMC = new TChain("DYToEE_13TeV_UntaggedTag");
  treeMC->Add(fileMC.c_str());

  TChain* treeDA = new TChain("diphotonDumper/trees/tree_13TeV_All");
  //TChain* treeDA = new TChain("tagsDumper/trees/Data_13TeV_UntaggedTag");
  treeDA->Add(fileDA.c_str());

  
  // Declaration of leaf types
  Int_t           candidate_id;
  Float_t         weight;
  Float_t         mass;
  Float_t         pt;
  Float_t         diphotonMVA;
  Float_t         ele1_et;
  Float_t         ele1_energy;
  Float_t         ele1_rawEnergy;
  Float_t         ele1_esEnergy;
  Float_t         ele1_scEta;
  Float_t         ele1_scPhi;
  Float_t         ele1_eta;
  Float_t         ele1_phi;
  Float_t         ele1_full5x5_r9;
  Float_t         ele2_et;
  Float_t         ele2_energy;
  Float_t         ele2_rawEnergy;
  Float_t         ele2_esEnergy;
  Float_t         ele2_scEta;
  Float_t         ele2_scPhi;
  Float_t         ele2_eta;
  Float_t         ele2_phi;
  Float_t         ele2_full5x5_r9;
  Int_t           nvtx;
  //Bool_t          HLT_Ele35_WPLoose_Gsf_v1;
  //Bool_t          HLT_Ele35_WPLoose_Gsf_v2;
  //Bool_t          HLT_Ele35_WPLoose_Gsf_v3;
  //Bool_t          HLT_Ele35_WPLoose_Gsf_v4;
  //Bool_t          HLT_Ele35_WPLoose_Gsf_v5;

  Bool_t          HLT_Ele27_WPTight_Gsf_v1;
  Bool_t          HLT_Ele27_WPTight_Gsf_v2;
  Bool_t          HLT_Ele27_WPTight_Gsf_v3;
  Bool_t          HLT_Ele27_WPTight_Gsf_v4;
  Bool_t          HLT_Ele27_WPTight_Gsf_v5;
  Bool_t          HLT_Ele27_WPTight_Gsf_v6;
  Bool_t          HLT_Ele27_WPTight_Gsf_v7;
  
  // MC tree
  treeMC->SetBranchAddress("weight", &weight);
  treeMC->SetBranchAddress("nvtx", &nvtx);
  treeMC->SetBranchAddress("candidate_id", &candidate_id);

  treeMC->SetBranchAddress("mass", &mass);
  treeMC->SetBranchAddress("pt", &pt);
  treeMC->SetBranchAddress("diphotonMVA", &diphotonMVA);

  treeMC->SetBranchAddress("ele1_energy", &ele1_energy);
  treeMC->SetBranchAddress("ele1_rawEnergy", &ele1_rawEnergy);
  treeMC->SetBranchAddress("ele1_esEnergy", &ele1_esEnergy);
  treeMC->SetBranchAddress("ele1_et", &ele1_et);
  treeMC->SetBranchAddress("ele1_scEta", &ele1_scEta);
  treeMC->SetBranchAddress("ele1_scPhi", &ele1_scPhi);
  treeMC->SetBranchAddress("ele1_eta", &ele1_eta);
  treeMC->SetBranchAddress("ele1_phi", &ele1_phi);
  treeMC->SetBranchAddress("ele1_full5x5_r9", &ele1_full5x5_r9);

  treeMC->SetBranchAddress("ele2_energy", &ele2_energy);
  treeMC->SetBranchAddress("ele2_rawEnergy", &ele2_rawEnergy);
  treeMC->SetBranchAddress("ele2_esEnergy", &ele2_esEnergy);
  treeMC->SetBranchAddress("ele2_et", &ele2_et);
  treeMC->SetBranchAddress("ele2_scEta", &ele2_scEta);
  treeMC->SetBranchAddress("ele2_scPhi", &ele2_scPhi);
  treeMC->SetBranchAddress("ele2_eta", &ele2_eta);
  treeMC->SetBranchAddress("ele2_phi", &ele2_phi);
  treeMC->SetBranchAddress("ele2_full5x5_r9", &ele2_full5x5_r9);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v1", &HLT_Ele27_WPTight_Gsf_v1);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v2", &HLT_Ele27_WPTight_Gsf_v2);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v3", &HLT_Ele27_WPTight_Gsf_v3);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v4", &HLT_Ele27_WPTight_Gsf_v4);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v5", &HLT_Ele27_WPTight_Gsf_v5);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v6", &HLT_Ele27_WPTight_Gsf_v6);
  treeMC->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v7", &HLT_Ele27_WPTight_Gsf_v7);
			   
  // DATA tree
  treeDA->SetBranchAddress("weight", &weight);
  treeDA->SetBranchAddress("nvtx", &nvtx);
  treeDA->SetBranchAddress("candidate_id", &candidate_id);

  treeDA->SetBranchAddress("mass", &mass);
  treeDA->SetBranchAddress("pt", &pt);
  treeDA->SetBranchAddress("diphotonMVA", &diphotonMVA);

  treeDA->SetBranchAddress("ele1_energy", &ele1_energy);
  treeDA->SetBranchAddress("ele1_rawEnergy", &ele1_rawEnergy);
  treeDA->SetBranchAddress("ele1_esEnergy", &ele1_esEnergy);
  treeDA->SetBranchAddress("ele1_et", &ele1_et);
  treeDA->SetBranchAddress("ele1_scEta", &ele1_scEta);
  treeDA->SetBranchAddress("ele1_scPhi", &ele1_scPhi);
  treeDA->SetBranchAddress("ele1_eta", &ele1_eta);
  treeDA->SetBranchAddress("ele1_phi", &ele1_phi);
  treeDA->SetBranchAddress("ele1_full5x5_r9", &ele1_full5x5_r9);

  treeDA->SetBranchAddress("ele2_energy", &ele2_energy);
  treeDA->SetBranchAddress("ele2_rawEnergy", &ele2_rawEnergy);
  treeDA->SetBranchAddress("ele2_esEnergy", &ele2_esEnergy);
  treeDA->SetBranchAddress("ele2_et", &ele2_et);
  treeDA->SetBranchAddress("ele2_scEta", &ele2_scEta);
  treeDA->SetBranchAddress("ele2_scPhi", &ele2_scPhi);
  treeDA->SetBranchAddress("ele2_eta", &ele2_eta);
  treeDA->SetBranchAddress("ele2_phi", &ele2_phi);
  treeDA->SetBranchAddress("ele2_full5x5_r9", &ele2_full5x5_r9);
  // treeDA->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v1", &HLT_Ele35_WPLoose_Gsf_v1);
  //treeDA->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v2", &HLT_Ele35_WPLoose_Gsf_v2);
  //treeDA->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v3", &HLT_Ele35_WPLoose_Gsf_v3);
  //treeDA->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v4", &HLT_Ele35_WPLoose_Gsf_v4);
  //treeDA->SetBranchAddress("HLT_Ele35_WPLoose_Gsf_v5", &HLT_Ele35_WPLoose_Gsf_v5);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v1", &HLT_Ele27_WPTight_Gsf_v1);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v2", &HLT_Ele27_WPTight_Gsf_v2);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v3", &HLT_Ele27_WPTight_Gsf_v3);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v4", &HLT_Ele27_WPTight_Gsf_v4);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v5", &HLT_Ele27_WPTight_Gsf_v5);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v6", &HLT_Ele27_WPTight_Gsf_v6);
  treeDA->SetBranchAddress("HLT_Ele27_WPTight_Gsf_v7", &HLT_Ele27_WPTight_Gsf_v7);
  
  cout << "Number of events to be analyzed in MC   : " << treeMC->GetEntries() <<endl;
  cout << "Number of events to be analyzed in DATA : " << treeDA->GetEntries() <<endl;

  cout <<endl;

  // define HT bins
  TH1F* h_Ht[4][4];
  for (int i=0; i<4;i++){
    for (int j=0; j<4;j++){
      h_Ht[i][j] = new TH1F(Form("h_Ht_%d%d",i,j),Form("h_Ht_%d%d",i,j),10000,30,2030);      
    }
  }

  for (int i=0; i<treeDA->GetEntries(); i++) {

    treeDA -> GetEntry(i);
    if (i%1000000==0) cout << "Analyzing event "<< i <<endl;
    
    // take only the first wenu candidate (highest SumpT) if there is more than one candidate                    
    if (candidate_id !=0 ) continue;
    
    // dphi(ele1,ele2)                                                       
    float dphi = fabs(ele1_phi - ele2_phi);
    if ( dphi > TMath::Pi() ) dphi = 2*TMath::Pi() - dphi;

    // selections
    //if (applyHLT && !HLT_Ele35_WPLoose_Gsf_v1 && !HLT_Ele35_WPLoose_Gsf_v2 && !HLT_Ele35_WPLoose_Gsf_v3 && !HLT_Ele35_WPLoose_Gsf_v4 && !HLT_Ele35_WPLoose_Gsf_v5) continue;
    if (applyHLT && !HLT_Ele27_WPTight_Gsf_v1 && !HLT_Ele27_WPTight_Gsf_v2 && !HLT_Ele27_WPTight_Gsf_v3 && !HLT_Ele27_WPTight_Gsf_v4 && !HLT_Ele27_WPTight_Gsf_v5 && !HLT_Ele27_WPTight_Gsf_v6 && !HLT_Ele27_WPTight_Gsf_v7 ) continue;
    if (ele1_et < minEt1) continue;                                                         
    if (ele2_et < minEt2) continue;                                                         
    if ( dphi > maxDphi ) continue;
    if ( mass < minMass ) continue;
    
    // get photons categories
    int cat1 = photonCategory(ele1_scEta, ele1_full5x5_r9); 
    int cat2 = photonCategory(ele2_scEta, ele2_full5x5_r9); 
    
    float w = 1;
    
    float ht = (ele1_et+ele2_et)/2;

    if (cat1==cat2){
      h_Ht[cat1][cat2]->Fill(ht,w);
    }
    else{
      // symm cats : cat1cat2 = cat2cat1
      if (cat1<cat2)
	h_Ht[cat1][cat2]->Fill(ht,w);
      else
	h_Ht[cat2][cat1]->Fill(ht,w);
    }
  }


 for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      cout << "Category : " << i <<""<<j<<"  Number of events = " <<h_Ht[i][j]->GetSumOfWeights()<<endl;
    } 
 }

  // define HT bins and book histograms
  int nPoints = 10000;
  vector<double> binEdges[4][4];
  int nHtBins[4][4] ;
  TH1F** h_Ht_MC[4][4];
  TH1F** h_Ht_DA[4][4];
  TH1F** h_Mee_MC[4][4];
  TH1F** h_Mee_DA[4][4];

  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      binEdges[i][j].clear();
      
      if (h_Ht[i][j]->GetEntries()==0) continue;
    
      //if ( (i>1 && j>1) || (i!=j) ){ // both in EE or mixed categories
      //	nPoints = 3000;
      //}
      if ( i > 1 ){
	nPoints = 3000;
      }
      else{
	 nPoints = 10000;
      }
      
      defineBins( h_Ht[i][j], nPoints, binEdges[i][j], minEt2 );
      nHtBins[i][j] = binEdges[i][j].size();
      cout << "Category "<< i << "" << j << "  Number of HT bins = " <<  nHtBins[i][j]<<endl;
      h_Ht_MC[i][j] = new TH1F*[nHtBins[i][j]];
      h_Ht_DA[i][j] = new TH1F*[nHtBins[i][j]];
      h_Mee_MC[i][j] = new TH1F*[nHtBins[i][j]];
      h_Mee_DA[i][j] = new TH1F*[nHtBins[i][j]];
      for (int htbin = 0; htbin < nHtBins[i][j]; htbin++){
	h_Ht_MC[i][j][htbin] = new TH1F(Form("h_Ht_MC_%d%d_%d",i,j,htbin), Form("h_Ht_MC_%d%d_%d",i,j,htbin), 500, 0,2000); 
	h_Ht_DA[i][j][htbin] = new TH1F(Form("h_Ht_DA_%d%d_%d",i,j,htbin), Form("h_Ht_DA_%d%d_%d",i,j,htbin), 500, 0,2000); 
	h_Mee_MC[i][j][htbin] = new TH1F(Form("h_Mee_MC_%d%d_%d",i,j,htbin), Form("h_Mee_MC_%d%d_%d",i,j,htbin), 200, 0.5,1.5); 
	h_Mee_DA[i][j][htbin] = new TH1F(Form("h_Mee_DA_%d%d_%d",i,j,htbin), Form("h_Mee_DA_%d%d_%d",i,j,htbin), 200, 0.5,1.5); 
      }
    }
  }

  TH1F *hnvtxMC = new TH1F("hnvtxMC","Number of vertices", 30,0,30);
  TH1F *hnvtxDA = new TH1F("hnvtxDA","Number of vertices", 30,0,30);

  float bins[12] = {0., 10., 20., 30., 40., 50., 70. , 100., 150. , 200., 500., 1000.};
  float binnum = sizeof(bins)/sizeof(float) - 1;
  TH1F *hZPtMC = new TH1F("hZptMC","hZPt", binnum, bins);
  TH1F *hZPtDA = new TH1F("hZptDA","hZPt", binnum, bins);
  
  // loop over MC events
  for (int i=0; i<treeMC->GetEntries(); i++) {
    
    treeMC -> GetEntry(i);
    if (i%1000000==0) cout << "Analyzing MC event "<< i <<endl;

    if (useOddEvents  && i%2 == 0) continue; 
    if (useEvenEvents && i%2 == 1) continue; 
      

    // take only the first wenu candidate (highest SumpT) if there is more than one candidate                    
    if (candidate_id !=0 ) continue;
    
    // dphi(ele1,ele2)                                                       
    float dphi = fabs(ele1_phi - ele2_phi);
    if ( dphi > TMath::Pi() ) dphi = 2*TMath::Pi() - dphi;

    // selections
    if (applyHLT && !HLT_Ele27_WPTight_Gsf_v1 && !HLT_Ele27_WPTight_Gsf_v2 && !HLT_Ele27_WPTight_Gsf_v3 && !HLT_Ele27_WPTight_Gsf_v4 && !HLT_Ele27_WPTight_Gsf_v5 && !HLT_Ele27_WPTight_Gsf_v6 && !HLT_Ele27_WPTight_Gsf_v7 ) continue;
    if (ele1_et < minEt1) continue;                                                         
    if (ele2_et < minEt2) continue;                                                         
    if ( dphi > maxDphi ) continue;
    if ( mass < minMass ) continue;
    
    // get photons categories
    int cat1 = photonCategory(ele1_scEta, ele1_full5x5_r9); 
    int cat2 = photonCategory(ele2_scEta, ele2_full5x5_r9); 
    
    float w = weight*intLumi;
    if (doNvtxWeights) w*=GetWeight(nvtx,hweights);
    if (doZptWeights) w*=GetWeight(nvtx,hweightsPt);
    
    float ht = (ele1_et+ele2_et)/2;
    
    if (useRawEnergy){
      float rawE1 = ele1_rawEnergy +ele1_esEnergy;
      float rawE2 = ele2_rawEnergy +ele2_esEnergy;
      mass = mass / sqrt(ele1_energy*ele2_energy)*sqrt(rawE1*rawE2);
    }
    
    // fill MC histograms
    hnvtxMC ->Fill(nvtx, w);
    hZPtMC->Fill(pt,w);
  
    if (cat1==cat2){
      int htbin =  FindHtBin(ht, binEdges[cat1][cat2]);
      h_Ht_MC[cat1][cat2][htbin]->Fill(ht,w);
      h_Mee_MC[cat1][cat2][htbin]->Fill(mass/MZ,w);
    }
    else{
      // symm cats : cat1cat2 = cat2cat1
      if (cat1<cat2){
	int htbin =  FindHtBin(ht, binEdges[cat1][cat2]);
	h_Ht_MC[cat1][cat2][htbin]->Fill(ht,w);
	h_Mee_MC[cat1][cat2][htbin]->Fill(mass/MZ,w);
      }
      else{
	int htbin =  FindHtBin(ht, binEdges[cat2][cat1]);
	h_Ht_MC[cat2][cat1][htbin]->Fill(ht,w);
	h_Mee_MC[cat2][cat1][htbin]->Fill(mass/MZ,w);
      }
    }
  }// end loop over MC events



  // loop over DATA events
  for (int i=0; i<treeDA->GetEntries(); i++) {

    treeDA -> GetEntry(i);
    if (i%1000000==0) cout << "Analyzing DATA event "<< i <<endl;

    if (useOddEvents  && i%2 == 0) continue; 
    if (useEvenEvents && i%2 == 1) continue; 

    // take only the first wenu candidate (highest SumpT) if there is more than one candidate                    
    if (candidate_id !=0 ) continue;
    
    // dphi(ele1,ele2)                                                       
    float dphi = fabs(ele1_phi - ele2_phi);
    if ( dphi > TMath::Pi() ) dphi = 2*TMath::Pi() - dphi;

    // selections
    if (applyHLT && !HLT_Ele27_WPTight_Gsf_v1 && !HLT_Ele27_WPTight_Gsf_v2 && !HLT_Ele27_WPTight_Gsf_v3 && !HLT_Ele27_WPTight_Gsf_v4 && !HLT_Ele27_WPTight_Gsf_v5 && !HLT_Ele27_WPTight_Gsf_v6 && !HLT_Ele27_WPTight_Gsf_v7 ) continue;
    if (ele1_et < minEt1) continue;                                                         
    if (ele2_et < minEt2) continue;                                                         
    if ( dphi > maxDphi ) continue;
    if ( mass < minMass ) continue;
    
    // get photons categories
    int cat1 = photonCategory(ele1_scEta, ele1_full5x5_r9); 
    int cat2 = photonCategory(ele2_scEta, ele2_full5x5_r9); 
    
    float w = 1.;
        
    float ht = (ele1_et+ele2_et)/2;

    if (useRawEnergy){
      float rawE1 = ele1_rawEnergy +ele1_esEnergy;
      float rawE2 = ele2_rawEnergy +ele2_esEnergy;
      mass = mass / sqrt(ele1_energy*ele2_energy)*sqrt(rawE1*rawE2);
    }
	
    // fill MC histograms
    hnvtxDA ->Fill(nvtx, w);
    hZPtDA->Fill(pt,w);
    
    if (cat1==cat2){
      int htbin =  FindHtBin(ht, binEdges[cat1][cat2]);
      h_Ht_DA[cat1][cat2][htbin]->Fill(ht,w);
      h_Mee_DA[cat1][cat2][htbin]->Fill(mass/MZ,w);
    }
    else{
      // symm cats : cat1cat2 = cat2cat1
      if (cat1<cat2){
	int htbin =  FindHtBin(ht, binEdges[cat1][cat2]);
	h_Ht_DA[cat1][cat2][htbin]->Fill(ht,w);
	h_Mee_DA[cat1][cat2][htbin]->Fill(mass/MZ,w);
      }
      else{
	int htbin =  FindHtBin(ht, binEdges[cat2][cat1]);
	h_Ht_DA[cat2][cat1][htbin]->Fill(ht,w);
	h_Mee_DA[cat2][cat1][htbin]->Fill(mass/MZ,w);
      }
    }
  }// end loop over DATA events
  
  
  //scale data errors to take into account that MC stat is finite --> sum MC error to data
  if (scaleDataErrors){
    double newerr, errDA, errMC, norm = 0;
    for (int i = 0; i < 4; i++){
      for (int j = 0; j < 4; j++){
	for (int htbin = 0; htbin < nHtBins[i][j]; htbin++){     
	  for (int ibin = 1; ibin < h_Mee_DA[i][j][htbin] -> GetNbinsX()+1; ibin++){
	    norm   = h_Mee_DA[i][j][htbin] -> GetSumOfWeights()/h_Mee_MC[i][j][htbin] -> GetSumOfWeights();
	    h_Mee_MC[i][j][htbin] -> Scale(norm);
	    errDA  = h_Mee_DA[i][j][htbin] -> GetBinError(ibin);
	    errMC  = h_Mee_MC[i][j][htbin] -> GetBinError(ibin);
	    newerr = sqrt(errDA*errDA+errMC*errMC);
	    h_Mee_DA[i][j][htbin] -> SetBinError(ibin, newerr);
	  }
	}
      }
      }
  }

  //template fit
  TGraphErrors *g[4][4];
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      g[i][j] = new TGraphErrors();
      g[i][j]->SetName(Form("g_cat%dcat%d",i,j));
      g[i][j]->SetMarkerStyle(4);
      g[i][j]->SetMarkerSize(1.);
      g[i][j]->SetMarkerColor(kRed);
    }
  }

  TH1F *hChi2 = new TH1F("hChi2","hChi2",100,0,10);


  double scale, scaleErr, chi2, ndf;
  double xx, xxErr;
  int nRe = 2;
    

  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      cout << "Fitting cat " << i << ", cat "<< j <<endl;
  
      for (int ibin = 0; ibin < nHtBins[i][j]; ibin++){  
	if (h_Mee_DA[i][j][ibin] ->GetSumOfWeights() == 0 || h_Mee_MC[i][j][ibin] ->GetSumOfWeights()==0) continue;
	h_Mee_MC[i][j][ibin] -> Rebin(nRe);   
	h_Mee_DA[i][j][ibin] -> Rebin(nRe);   
	xx = h_Ht_MC[i][j][ibin]->GetMean();
	xxErr =  h_Ht_MC[i][j][ibin]->GetMeanError();
	//xx = 0.5*(h_Et_EB_HighR9_MC[ibin]->GetMean() + h_Et_EB_HighR9_DA[ibin]->GetMean());
	//xxErr = fabs( 0.5*(h_Et_EB_HighR9_MC[ibin]->GetMean() - h_Et_EB_HighR9_DA[ibin]->GetMean()) );
	DoTemplateFit( h_Mee_MC[i][j][ibin], h_Mee_DA[i][j][ibin], 0.90, 1.10, scale, scaleErr, chi2, ndf);
	if (scale > 0 && scaleErr > 0) {
	  if (scaleErr < 0.0001) { // protection against too small errors
	    float newerr = sqrt(h_Mee_DA[i][j][ibin]->GetMeanError()*h_Mee_DA[i][j][ibin]->GetMeanError()+h_Mee_MC[i][j][ibin]->GetMeanError()*h_Mee_MC[i][j][ibin]->GetMeanError());
	    cout << "Ht/2 bin "<< ibin << "  scale = " << scale << " +/-" << scaleErr 
		 << " ==> Setting error to "<< newerr << endl;
	    scaleErr =  newerr;
	    
	  }
	  g[i][j] -> SetPoint(ibin, xx, scale);
	  g[i][j] -> SetPointError(ibin, xxErr, scaleErr);
	  hChi2->Fill(chi2/ndf);
	}
      }

      
    }
  }

  cout << "Average Chi2/NDF of template fits = " << hChi2-> GetMean() <<endl;


  TFile *fout = new TFile(argv[3],"recreate");

  hnvtxMC -> Write();
  hnvtxDA -> Write(); 

  hZPtMC->Write();
  hZPtDA->Write();

  hChi2->Write();

  
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
      g[i][j] -> Write();
    }
  }
 
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 4; j++){
       for (int ibin = 0; ibin < nHtBins[i][j]; ibin++){  
	 //h_Ht[i][j]->Write();
	 h_Mee_MC[i][j][ibin]->Write();
	 h_Mee_DA[i][j][ibin]->Write();
	 h_Ht_DA[i][j][ibin]->Write();
	 h_Ht_MC[i][j][ibin]->Write();
       }
    }
  }
 


  cout << "Closing file..." << endl;
  fout->Close();

  
}
