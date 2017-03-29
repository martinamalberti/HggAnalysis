void SplitWZHtrees(bool saveZ, bool saveW) {

  cout << "Opening file ..." <<endl;
  //Get old file, old tree and set top branch address
  TFile *oldfile = new TFile("trees/vhLepDumper_20170324/output_vh.root");


  //Create a new file + a clone of old tree in new file
  TFile *newfile;
  if (saveZ) newfile = new TFile("output_zh.root","recreate");
  if (saveW) newfile = new TFile("output_wh.root","recreate");

  cout<<  "aaaaa"<<endl;
  oldfile->cd();
  gDirectory->ls();
  TTree *oldtree1 = (TTree*)oldfile->Get("WHLeptonicTagDumper/trees/vh_13TeV_WHLeptonicTag");
  Long64_t nentries1 = oldtree1->GetEntries();
  cout << "Number of entries = " << nentries1 <<endl;
  float hasZ, hasW;
  oldtree1->SetBranchAddress("hasZ",&hasZ);
  oldtree1->SetBranchAddress("hasW",&hasW);


  //  TTree *oldtree2 = (TTree*)oldfile->Get("VHLeptonicLooseTagDumper/trees/vh_13TeV_VHLeptonicLooseTagDumper");
  TTree *oldtree2 = (TTree*)oldfile->Get("VHLeptonicLooseTagDumper/trees/vh_13TeV_VHLeptonicLooseTag");
  Long64_t nentries2 = oldtree2->GetEntries();
  cout << "Number of entries = " << nentries2 <<endl;
  oldtree2->SetBranchAddress("hasZ",&hasZ);
  oldtree2->SetBranchAddress("hasW",&hasW);

  
  TTree *oldtree3 = (TTree*)oldfile->Get("ZHLeptonicTagDumper/trees/vh_13TeV_ZHLeptonicTag");
  Long64_t nentries3 = oldtree3->GetEntries();
  cout << "Number of entries = " << nentries3 <<endl;
  oldtree3->SetBranchAddress("hasZ",&hasZ);
  oldtree3->SetBranchAddress("hasW",&hasW);

  
  // keep same dir structure as original file
  newfile->cd();
  TDirectory *dir1 = newfile->mkdir("WHLeptonicTagDumper");
  dir1->cd();
  TDirectory *subdir1 = dir1->mkdir("trees");
  subdir1->cd();
  // new tree
  TTree *newtree1 = oldtree1->CloneTree(0);
  if (saveZ) newtree1->SetName("zh_13TeV_WHLeptonicTag");
  if (saveW) newtree1->SetName("wh_13TeV_WHLeptonicTag");

  for (Long64_t i=0;i<nentries1; i++) {
    oldtree1->GetEntry(i);
    if ( hasZ && saveZ ) newtree1->Fill();
    if ( hasW && saveW ) newtree1->Fill();
  }


  
  
  // keep same dir structure as original file
  newfile->cd();
  TDirectory *dir2 = newfile->mkdir("VHLeptonicLooseTagDumper");
  dir2->cd();
  TDirectory *subdir2 = dir2->mkdir("trees");
  subdir2->cd();
  // new tree
  TTree *newtree2 = oldtree2->CloneTree(0);
  //if (saveZ) newtree2->SetName("zh_13TeV_VHLeptonicLooseTagDumper");
  //if (saveW) newtree2->SetName("wh_13TeV_VHLeptonicLooseTagDumper");
  if (saveZ) newtree2->SetName("zh_13TeV_VHLeptonicLooseTag");
  if (saveW) newtree2->SetName("wh_13TeV_VHLeptonicLooseTag");

  for (Long64_t i=0;i<nentries2; i++) {
    oldtree2->GetEntry(i);
    if ( hasZ && saveZ ) newtree2->Fill();
    if ( hasW && saveW ) newtree2->Fill();
  }


  
  
  // keep same dir structure as original file
  newfile->cd();
  TDirectory *dir3 = newfile->mkdir("ZHLeptonicTagDumper");
  dir3->cd();
  TDirectory *subdir3 = dir3->mkdir("trees");
  subdir3->cd();
  // new tree
  TTree *newtree3 = oldtree3->CloneTree(0);
  if (saveZ) newtree3->SetName("zh_13TeV_ZHLeptonicTag");
  if (saveW) newtree3->SetName("wh_13TeV_ZHLeptonicTag");

  for (Long64_t i=0;i<nentries3; i++) {
    oldtree3->GetEntry(i);
    if ( hasZ && saveZ ) newtree3->Fill();
    if ( hasW && saveW ) newtree3->Fill();
  }



  
  newtree1->AutoSave();
  cout << "Number of entries of the new tree = " << newtree1->GetEntries()<<endl;
  newtree2->AutoSave();
  cout << "Number of entries of the new tree = " << newtree2->GetEntries()<<endl;
  newtree3->AutoSave();
  cout << "Number of entries of the new tree = " << newtree3->GetEntries()<<endl;
  delete oldfile;
  delete newfile;
}
