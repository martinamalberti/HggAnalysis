// per compilare: g++ -Wall -o CheckGainSwitchEffect `root-config --cflags --glibs` -L $ROOTSYS/lib -lRooFit -lRooFitCore -lFoam -lHtml -lMinuit CheckGainSwitchEffect.cpp

#include "./histoFunc.h"

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

int photonCategory(float eta, float r9){
  int cat = -1;
  if (r9>0.94 && fabs(eta) < 1.4442) cat = 0;
  if (r9<0.94 && fabs(eta) < 1.4442) cat = 1;
  if (r9>0.94 && fabs(eta) > 1.4442) cat = 2;
  if (r9<0.94 && fabs(eta) > 1.4442) cat = 3;
  return cat;
}

int photonSeedEtBin(float eSeed, float eta){
  int cat = -1;
  if (eSeed >200 && eSeed < 300 && fabs(eta)<1.5) cat = 0;
  if (eSeed >300 && eSeed < 400 && fabs(eta)<1.5) cat = 1;
  if (eSeed >400 && eSeed < 500 && fabs(eta)<1.5) cat = 2;
  return cat;
}

//////  ====== MAIN ========================

int main(int argc, char** argv)
{


      
  // book histograms
  //  const int ncat = 8;
    const int ncat = 14;
  TH1F *hGainSwitchByCat = new TH1F("hGainSwitchByCat","hGainSwitchByCat",ncat,0,ncat);
  TH1F *hNeventsByCat = new TH1F("hNeventsByCat","hNeventsByCat",ncat,0,ncat);
  TH1F *hmass0[ncat];// original mass
  TH1F *hmass[ncat];// mass after applying scale correction for gain switched channels
  TH1F *hDeltaM[ncat]; // shift after applying scale correction for gain switched channels
  for (int icat =0 ; icat < ncat ; icat++){
    hmass0[icat] = new TH1F(Form("hmass0_cat%d",icat),Form("hmass0_cat%d",icat),100,100,150);
    hmass[icat] = new TH1F(Form("hmass_cat%d",icat),Form("hmass_cat%d",icat),100,100,150);
    hDeltaM[icat] = new TH1F(Form("hDeltaM_cat%d",icat),Form("hDeltaM_cat%d",icat),2000,-0.5,0.5);
  }
  
  // scale correction for gain switched channels
  float correction[3] = {1.0199, 1.052, 1.015};
  //float error[3] = { 0.0008, 0.003, 0.006};// stat error
  float error[3] = { 0.01, 0.01, 0.01};
  
  TRandom * gRandom = new TRandom();

  // trees by process and tag
  vector<string> processes;
  processes.push_back("ggh");
  processes.push_back("vbf");
  processes.push_back("vh");
  processes.push_back("tth");
  
  const int nproc = processes.size(); 
  TChain* tree[ncat][nproc]; // ncat = 12
    
  Float_t         weight;
  Int_t           candidate_id;
  Float_t         mass;
  Float_t         leadE;
  Float_t         subleadE;
  Float_t         leadEt;
  Float_t         subleadEt;
  Float_t         leadR9;
  Float_t         subleadR9;
  Float_t         leadEta;
  Float_t         subleadEta;
  Float_t         lead_eMax;
  Float_t         sublead_eMax;
  
  // load trees
  for (int iproc = 0; iproc < processes.size(); iproc++){
  
    string proc = processes[iproc].c_str();
    //string file("trees_80X_Hgg/jobsTagDumper_12122016_v2/output_");
    string file("trees_80X_Hgg/jobsTagDumper_20170213/output_");
    file+=proc;
    file+=".root";
    cout << "Analyzing MC : " << file.c_str() <<endl;
    tree[0][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_UntaggedTag_0",proc.c_str()));
    tree[1][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_UntaggedTag_1",proc.c_str()));
    tree[2][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_UntaggedTag_2",proc.c_str()));
    tree[3][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_UntaggedTag_3",proc.c_str()));
    tree[4][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VBFTag_0",proc.c_str()));
    tree[5][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VBFTag_1",proc.c_str()));
    //tree[5][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VBFTag_1",proc.c_str()));
    //tree[6][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_TTHHadronicTag",proc.c_str()));
    //tree[7][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_TTHLeptonicTag",proc.c_str()));
    tree[6][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VBFTag_2",proc.c_str()));
    tree[7][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_TTHHadronicTag",proc.c_str()));
    tree[8][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_TTHLeptonicTag",proc.c_str()));
    tree[9][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_ZHLeptonicTag",proc.c_str()));
    tree[10][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_WHLeptonicTag",proc.c_str()));
    tree[11][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VHLeptonicLooseTag",proc.c_str()));
    tree[12][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VHHadronicTag",proc.c_str()));
    tree[13][iproc] = new TChain(Form("tagsDumper/trees/%s_13TeV_VHMetTag",proc.c_str()));
        
    for (int icat = 0 ; icat < ncat ; icat++){
      tree[icat][iproc]->Add(file.c_str());
      
      tree[icat][iproc]->SetBranchAddress("weight", &weight);
      tree[icat][iproc]->SetBranchAddress("candidate_id",&candidate_id);
      tree[icat][iproc]->SetBranchAddress("mass", &mass);
      tree[icat][iproc]->SetBranchAddress("leadE", &leadE);
      tree[icat][iproc]->SetBranchAddress("subLeadE", &subleadE);
      tree[icat][iproc]->SetBranchAddress("leadPt", &leadEt);
      tree[icat][iproc]->SetBranchAddress("subleadPt", &subleadEt);
      tree[icat][iproc]->SetBranchAddress("leadEta", &leadEta);
      tree[icat][iproc]->SetBranchAddress("subLeadEta", &subleadEta);
      tree[icat][iproc]->SetBranchAddress("leadR9", &leadR9);
      tree[icat][iproc]->SetBranchAddress("subLeadR9", &subleadR9);
      tree[icat][iproc]->SetBranchAddress("lead_eMax", &lead_eMax);
      tree[icat][iproc]->SetBranchAddress("subLead_eMax", &sublead_eMax);
    }
  }    


  // throw toys
  int leadCat = -1;
  int subleadCat = -1;
  float newmass = 0;
  float dm = 0;
  float thiscorr[3] = {0., 0., 0.};
 
  int nToys = 10;
  for (int itoy = 0; itoy < nToys+1; itoy++){
      
    if (itoy%10 == 0) cout << "Toy n. " << itoy << endl;
     	
    // get corrections
    for (int i = 0; i < 3; i++){
      thiscorr[i] = gRandom -> Gaus(correction[i], error[i]);
    }
      
      
    // book histograms
    TH1F *hmass_tmp[ncat];
    for (int icat =0 ; icat < ncat ; icat++){
      hmass_tmp[icat] = new TH1F(Form("hmass_tmp%d",icat),Form("hmass_tmp%d",icat),100,100,150);
    }
      
    // loop over trees
    for (int icat = 0; icat < ncat; icat++){

      for (int iproc = 0; iproc < nproc; iproc++){
	
	// loop over events
	for (int entry = 0; entry < tree[icat][iproc]->GetEntries(); entry++){
	  
	  tree[icat][iproc]->GetEntry(entry);

	  if ( candidate_id !=0 ) continue;
	  
	  if (itoy == 0){
	    hmass0[icat]->Fill(mass,weight);
	    
	    leadCat = photonSeedEtBin(lead_eMax, leadEta);
	    subleadCat = photonSeedEtBin(sublead_eMax, subleadEta);
	    
	    if (leadCat ==-1) correction[leadCat] = 1.;
	    if (subleadCat ==-1) correction[subleadCat] = 1.;
	    
	    newmass = mass *sqrt( correction[leadCat]*correction[subleadCat]);
	    hmass[icat]->Fill(newmass, weight);


	    hNeventsByCat->Fill(icat, weight);
	    if ( (lead_eMax > 155. && fabs(leadEta) < 1.5) || (sublead_eMax > 155. && fabs(subleadEta) < 1.5) ) hGainSwitchByCat ->Fill(icat, weight);
	  }
	  else {
	    // apply residual corrections for gain switch
	    leadCat = photonSeedEtBin(lead_eMax, leadEta);
	    subleadCat = photonSeedEtBin(sublead_eMax, subleadEta);

	    if (leadCat ==-1) thiscorr[leadCat] = 1.;
	    if (subleadCat ==-1) thiscorr[subleadCat] = 1.;
	    
	    newmass = mass *sqrt( thiscorr[leadCat]*thiscorr[subleadCat]);
	    hmass_tmp[icat]->Fill(newmass,weight);
	  }

	} //end loop over events in the tree

      }//end loop over processes
      
      if (itoy !=0){
	dm =  hmass_tmp[icat]->GetMean() - hmass0[icat]->GetMean() ; 
	// cout << hmass_cat[icat]->GetMean() << " " << hmass0_cat[icat]->GetMean() << "  " << dm <<endl;
	hDeltaM[icat]->Fill(dm);
      }
      
      delete hmass_tmp[icat];
    } // end loop over categories
            
  } //end toys


  // print results
  for (int icat =0 ; icat < ncat ; icat++){

    std::cout << std::fixed;
    cout << "Category : "<< icat
	 << "   DeltaM = " << std::setprecision(4) << hDeltaM[icat]->GetMean()
	 << "   uncertanty = " << std::setprecision(4) << hDeltaM[icat]->GetRMS()
      //<< "   total = " <<  std::setprecision(3) << sqrt(  hDeltaM[icat]->GetMean()* hDeltaM[icat]->GetMean()+ hDeltaM[icat]->GetRMS()* hDeltaM[icat]->GetRMS()  )
	 <<endl;
  }
    
  hGainSwitchByCat->Divide(hGainSwitchByCat, hNeventsByCat,1,1);
  
  
  // save results
  TFile *fout = new TFile(argv[1],"recreate");
  fout ->cd();
  hGainSwitchByCat ->Write();
  for (int icat =0 ; icat < ncat ; icat++){
    hDeltaM[icat]->Write();
    hmass0[icat]->Write();
    hmass[icat]->Write();
  }

}
