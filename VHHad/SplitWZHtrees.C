void SplitWZHtrees(bool saveZ, bool saveW) {

  cout << "Opening file ..." <<endl;
  //Get old file, old tree and set top branch address
  TFile *oldfile = new TFile("trees/vhHadDumper_20161223/output_vh_v2.root");
  TTree *oldtree = (TTree*)oldfile->Get("vhHadTagDumper/trees/vh_13TeV_VHHadronicTag");
  Long64_t nentries = oldtree->GetEntries();
  cout << "Number of entries = " << nentries <<endl;
  float hasZ, hasW;
  oldtree->SetBranchAddress("hasZ",&hasZ);
  oldtree->SetBranchAddress("hasW",&hasW);
  //Create a new file + a clone of old tree in new file
  TFile *newfile;
  if (saveZ) newfile = new TFile("output_zh.root","recreate");
  if (saveW) newfile = new TFile("output_wh.root","recreate");

  // keep same dir structure as original file
  TDirectory *dir1 = newfile->mkdir("vhHadTagDumper");
  dir1->cd();
  TDirectory *dir2 = dir1->mkdir("trees");
  dir2->cd();
  
  // new tree
  TTree *newtree = oldtree->CloneTree(0);
  if (saveZ) newtree->SetName("zh_13TeV_VHHadronicTag");
  if (saveW) newtree->SetName("wh_13TeV_VHHadronicTag");

  for (Long64_t i=0;i<nentries; i++) {
    oldtree->GetEntry(i);
    if ( hasZ && saveZ ) newtree->Fill();
    if ( hasW && saveW ) newtree->Fill();
  }
  newtree->Print();
  newtree->AutoSave();
  cout << "Number of entries of the new tree = " << newtree->GetEntries()<<endl;
  delete oldfile;
  delete newfile;
}
