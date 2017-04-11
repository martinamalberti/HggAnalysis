#include "./histoFunc.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TF1.h"
#include "TH1.h"
#include "TVirtualFitter.h"
#include "TMath.h"

using namespace std;

TRandom *gRandom = new TRandom();

int photonCategory(float eta, float r9){
  int cat = -1;
  if (r9>0.94 && fabs(eta) < 1.4442) cat = 0;
  if (r9<0.94 && fabs(eta) < 1.4442) cat = 1;
  if (r9>0.94 && fabs(eta) > 1.4442) cat = 2;
  if (r9<0.94 && fabs(eta) > 1.4442) cat = 3;
  return cat;
}




void DoTemplateFit(TH1F *hMC, TH1F *hDA, double xmin,  double xmax, double &scale, double &scaleErr, double &chi2, double &ndf)   
{

  double xNorm = hDA->GetSumOfWeights()/hMC->GetSumOfWeights();

  histoFunc *templateHistoFunc = new histoFunc(hMC);
  TF1 *templateFunc = new TF1("templateFunc", templateHistoFunc,xmin, xmax, 3, "histoFunc");
  templateFunc -> SetNpx(10000);
    
  templateFunc -> FixParameter(0, xNorm);
  templateFunc -> SetParameter(1, 1.0 );
  //templateFunc -> SetParameter(1,  hDA ->GetMean()/hMC->GetMean() );
  templateFunc -> FixParameter(2, 0.);
  
  TFitResultPtr rp;
  int fStatus; 
  for (int trial=0;trial<10;trial++) {
    rp = hDA -> Fit("templateFunc", "QRS");
    fStatus = rp;
    //cout << trial << " " << hDA ->GetMean() << " " << templateFunc->GetParameter(1) << "  " << fStatus<<endl;
    if (fStatus == 0) {
      break;
    }
    //templateFunc -> SetParameter(1,  0.99 + trial * 0.002);
    templateFunc -> SetParameter(1,  1. + gRandom->Uniform(-0.01,0.01));
  }

  if (fStatus > 0) {
    scale = -1;
    scaleErr = -1;
  }

  scale = 1./templateFunc->GetParameter(1);
  scaleErr = templateFunc->GetParError(1)/scale/scale;
  chi2 = templateFunc->GetChisquare();
  ndf  = templateFunc->GetNDF();

}



/*int FindEtBin(float et){
  int bin  = -1;
  if (et > 30. && et < 34.) bin = 0;
  if (et > 34. && et < 38.) bin = 1;
  if (et > 38. && et < 40.) bin = 2;
  if (et > 40. && et < 42.) bin = 3;
  if (et > 42. && et < 45.) bin = 4;
  if (et > 45. && et < 60.) bin = 5;
  if (et > 60. ) bin = 6;
  return bin;
  }*/

int FindHtBin(float ht, vector<double> binEdges){
  
  int mybin = -1;

  for (int i = 0 ; i < binEdges.size()-1; i++ ){
    if (ht >= binEdges[i] && ht < binEdges[i+1]) {
      mybin = i;
    }
  }
  
  if (ht >= binEdges[binEdges.size()-1]) {
    mybin = binEdges.size()-1;
    //cout << "Et = " << et << "   bin = " << mybin <<endl;
  }  
  // cout << binEdges[binEdges.size()-1] <<endl;
  //cout << "Et = " << et << "   bin = " << mybin <<endl;

  return mybin;
}




void defineBins(TH1F *h, int n, vector<double> &binEdges, float minHt){
  
  binEdges.push_back(minHt);
  
  int tempBin = 1;
  
  for (int ibin = 1; ibin < h->GetNbinsX()+1 ; ibin++){
    
    if (ibin < tempBin) continue;
    if (h->Integral(tempBin,ibin) >= n){
      //cout << h->Integral(tempBin,ibin) << "  " << h->GetBinCenter(ibin)<<endl;
      binEdges.push_back(h->GetBinCenter(ibin));
      tempBin = ibin;
    }
  }

  int binLastEdge  = h->FindBin(binEdges.back());
  if (  h->Integral(binLastEdge, h->GetNbinsX()+1) < n/2. ) 
    binEdges.pop_back();

}
