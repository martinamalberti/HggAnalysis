#! /usr/bin/env python
import os
import glob
import math
import array
import sys
import time

import ROOT

import CMS_lumi, tdrstyle


#set the tdr style
tdrstyle.setTDRStyle()

#change the CMS_lumi variables (see CMS_lumi.py)
CMS_lumi.lumi_7TeV = "4.8 fb^{-1}"
CMS_lumi.lumi_8TeV = "18.3 fb^{-1}"
CMS_lumi.lumi_13TeV = "35.9 fb^{-1}"
CMS_lumi.writeExtraText = 1
CMS_lumi.extraText = "Preliminary"
CMS_lumi.lumi_sqrtS = "13 TeV" # used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

#iPeriod = 0
#iPos = 11
iPeriod = 4
iPos = 0
if( iPos==0 ): CMS_lumi.relPosX = 0.11

#ROOT.gStyle.SetLegendFont(43)
#ROOT.gStyle.SetTextFont(43)
#ROOT.gStyle.SetLabelFont(43,"X")
#ROOT.gStyle.SetLabelFont(43,"Y")
#ROOT.gStyle.SetTitleFont(43,"X")
#ROOT.gStyle.SetTitleFont(43,"Y")
ROOT.gStyle.SetPadTopMargin(0.08)
ROOT.gStyle.SetPadBottomMargin(0.15)
ROOT.gStyle.SetPadLeftMargin(0.10)
ROOT.gStyle.SetPadRightMargin(0.05)
ROOT.gStyle.cd()

intLumi = 35.9

#fmc = ROOT.TFile.Open("../hmc_ltbinned_v2.root")
#fmc = ROOT.TFile.Open("../hmc_ltbinned.root")
#fmc = ROOT.TFile.Open("../hmc.root")
#fmc = ROOT.TFile.Open("../hmc_rhoWeighted.root")
#fdata = ROOT.TFile.Open("../hdata.root")
#fdata = ROOT.TFile.Open("../hdata_2016B.root")
#fdata = ROOT.TFile.Open("../hdata_2016C.root")
#fdata = ROOT.TFile.Open("../hdata_2016D.root")
#fdata = ROOT.TFile.Open("../hdata_2016EtoH.root")

#fmc = ROOT.TFile.Open("../hmc_rhoWeighted2016H.root")
#fdata = ROOT.TFile.Open("../hdata_reMiniAOD_Run2016BtoH.root")

#fmc = ROOT.TFile.Open("../hmc_sigmaEcorr.root")
#fdata = ROOT.TFile.Open("../hdata_reMiniAOD.root")
fmc = ROOT.TFile.Open("../hmc_DYToEE_EGM0.root")
#fmc = ROOT.TFile.Open("../hmc_DYJetsToLL.root")
fdata = ROOT.TFile.Open("../hdata_reMiniAOD_Run2016H.root")

outdir = sys.argv[1]

vars = {"nvtx" : "number of reconstructed vertices",
        "rho"  : "rho",

        "pt" : "p_{T} (GeV)",
        "diphotonMVA" : "diphoton BDT score",
        "diphotonMVA_EBEB" : "diphoton BDT score",
        "diphotonMVA_EEEE" : "diphoton BDT score",

        "mass" : "mass (GeV)",
        "mass_EBEB_BothHighR9" : "mass (GeV)",
        "mass_EBEB_NotBothHighR9" : "mass (GeV)",
        "mass_NotEBEB_BothHighR9" : "mass (GeV)",
        "mass_NotEBEB_NotBothHighR9" : "mass (GeV)",

        "mass_EBEB_G12G6" : "mass (GeV)",
        "mass_EBEB_G12G1" : "mass (GeV)",

        "mass_EBEB" : "mass (GeV)",
        "mass_EBEB_highR9" : "mass (GeV)",
        "mass_EBEB_highR9_inner" : "mass (GeV)",
        "mass_EBEB_highR9_outer" : "mass (GeV)",
        "mass_EBEB_lowR9" : "mass (GeV)",
        "mass_EBEB_lowR9_inner" : "mass (GeV)",
        "mass_EBEB_lowR9_outer" : "mass (GeV)",
        "mass_EEEE" : "mass (GeV)",
        "mass_EEEE_highR9" : "mass (GeV)",
        "mass_EEEE_highR9_inner" : "mass (GeV)",
        "mass_EEEE_highR9_outer" : "mass (GeV)",
        "mass_EEEE_lowR9" : "mass (GeV)",
        "mass_EEEE_lowR9_inner" : "mass (GeV)",
        "mass_EEEE_lowR9_outer" : "mass (GeV)",

        "lead_eta" : "leading photon #eta",
        "lead_eta_highR9" : "leading photon #eta",
        "lead_eta_lowR9" : "leading photon #eta",

        "lead_rawEnergy_EB" : "leading photon rawE (GeV)",
        "lead_pt_EB" : "leading photon p_{T} (GeV)",
        "lead_r9_EB" : "leading photon r9",
        "lead_full5x5_r9_EB" : "leading photon full5x5 r9",
        "lead_etaWidth_EB" : "leading photon etaWidth",
        "lead_s4_EB" : "leading photon s4",
        "lead_full5x5_sigmaIetaIeta_EB" : "leading photon full5x5 sigmaIetaIeta",
        "lead_pfPhoIso03_EB" : "leading photon pfPhoIso03",
        "lead_sigmaEoE_EB" : "leading photon sigmaE/E",
        "lead_idmva_EB" : "leading photon idmva",

        "lead_rawEnergy_EE" : "leading photon rawE (GeV)",
        "lead_pt_EE" : "leading photon p_{T} (GeV)",
        "lead_r9_EE" : "leading photon r9",
        "lead_full5x5_r9_EE" : "leading photon full5x5 r9",
        "lead_etaWidth_EE" : "leading photon etaWidth",
        "lead_s4_EE" : "leading photon s4",
        "lead_full5x5_sigmaIetaIeta_EE" : "leading photon full5x5 sigmaIetaIeta",
        "lead_pfPhoIso03_EE" : "leading photon pfPhoIso03",
        "lead_ESoRawE_EE" : "leading photon E_{ES}/rawE_{SC}",
        "lead_sigmaEoE_EE" : "leading photon sigmaE/E",
        "lead_idmva_EE" : "leading photon idmva",
        
        "sublead_eta" : "subleading photon #eta",
        "sublead_eta_highR9" : "subleading photon #eta",
        "sublead_eta_lowR9" : "subleading photon #eta",

        "sublead_rawEnergy_EB" : "subleading photon rawE (GeV)",
        "sublead_pt_EB" : "subleading photon p_{T} (GeV)",
        "sublead_r9_EB" : "subleading photon r9",
        "sublead_full5x5_r9_EB" : "subleading photon full5x5 r9",
        "sublead_etaWidth_EB" : "subleading photon etaWidth",
        "sublead_s4_EB" : "subleading photon s4",
        "sublead_full5x5_sigmaIetaIeta_EB" : "subleading photon full5x5 sigmaIetaIeta",
        "sublead_pfPhoIso03_EB" : "subleading photon pfPhoIso03",
        "sublead_sigmaEoE_EB" : "subleading photon sigmaE/E",
        "sublead_idmva_EB" : "subleading photon idmva",
        
        "sublead_rawEnergy_EE" : "subleading photon rawE (GeV)",
        "sublead_pt_EE" : "subleading photon p_{T} (GeV)",
        "sublead_r9_EE" : "subleading photon r9",
        "sublead_full5x5_r9_EE" : "subleading photon full5x5 r9",
        "sublead_etaWidth_EE" : "subleading photon etaWidth",
        "sublead_s4_EE" : "subleading photon s4",
        "sublead_full5x5_sigmaIetaIeta_EE" : "subleading photon full5x5 sigmaIetaIeta",
        "sublead_pfPhoIso03_EE" : "subleading photon pfPhoIso03",
        "sublead_ESoRawE_EE" : "subleading photon E_{ES}/rawE_{SC}",
        "sublead_sigmaEoE_EE" : "subleading photon sigmaE/E",
        "sublead_idmva_EE" : "subleading photon idmva",


        "lead_full5x5_r9_00" : "leading photon full5x5 r9",
        "lead_full5x5_r9_01" : "leading photon full5x5 r9",
        "lead_full5x5_r9_02" : "leading photon full5x5 r9",
        "lead_full5x5_r9_03" : "leading photon full5x5 r9",
        "lead_full5x5_r9_04" : "leading photon full5x5 r9",
        "lead_full5x5_r9_05" : "leading photon full5x5 r9",

        "sublead_full5x5_r9_00" : "subleading photon full5x5 r9",
        "sublead_full5x5_r9_01" : "subleading photon full5x5 r9",
        "sublead_full5x5_r9_02" : "subleading photon full5x5 r9",
        "sublead_full5x5_r9_03" : "subleading photon full5x5 r9",
        "sublead_full5x5_r9_04" : "subleading photon full5x5 r9",
        "sublead_full5x5_r9_05" : "subleading photon full5x5 r9",

}


#vars = {"nvtx" : "number of reconstructed vertices",
#         "rho" : "rho",
#         "lead_idmva_EB" : "leading photon idmva",
#         "mass_EBEB" : "mass (GeV)",
#         "mass_EEEE" : "mass (GeV)",
#         "lead_eta" : "leading photon #eta",
#         "sublead_eta" : "subleading photon #eta",
#         "lead_pt_EB" : "leading photon p_{T} (GeV)",
#         "lead_pt_EE" : "leading photon p_{T} (GeV)",
#         "sublead_pt_EB" : "subleading photon p_{T} (GeV)",
#         "sublead_pt_EE" : "subleading photon p_{T} (GeV)",
#         "pt" : "diphoton pT" 
#}


vars  = {
    "rho" : "rho",
    "diphotonMVA" : "diphoton BDT score",
    "diphotonMVA_EBEB" : "diphoton BDT score",
    "diphotonMVA_EEEE" : "diphoton BDT score",
    "lead_idmva_EB" : "leading photon idmva",
    "lead_idmva_EE" : "leading photon idmva",
    "lead_pfPhoIso03_EB" : "leading photon pfPhoIso03",
    "lead_pfPhoIso03_EE" : "leading photon pfPhoIso03",
}

for var in vars:
    #print var
    # prepare canvas
    c = ROOT.TCanvas(var,var,700,800)
    pad1 = ROOT.TPad("pad1","top pad",0.0,0.35,1,1)
    pad1.Draw()
    pad2 = ROOT.TPad("pad2","bottom pad",0.0,0.05,1,0.37)
    pad2.Draw()

    # get histograms
    hname = 'h'+var
    if 'lead' in hname:
        hname.replace('hlead','hlead_')
        hname.replace('hsublead','hsublead_')
            
    hmc   = fmc.Get(hname)
    hdata = fdata.Get(hname)

    hmc.Scale(intLumi)
    
    #print "Ratio data/MC = ", hdata.GetSumOfWeights()/hmc.GetSumOfWeights()

            
    if 'mass' in var:
        if 'G12G1' in var:
            hmc.Rebin(5)
            hdata.Rebin(5)
        else:
            hmc.Rebin(2)
            hdata.Rebin(2)

    if 'nvtx' in var:
        print 'N.MC    = ', hmc.GetSumOfWeights()
        print 'N. data = ', hdata.GetSumOfWeights()
        print 'ratio data/MC = ', hdata.GetSumOfWeights()/hmc.GetSumOfWeights()
        ks = hmc.KolmogorovTest(hdata)
        print 'ks = ',ks
    
    # mc histogram attributes
    hmc.SetFillColor(ROOT.kAzure+1)
    #hmc.GetYaxis().SetExponentOffset(-0.01,0.01)
    hmc.GetYaxis().SetNoExponent()
    hmc.GetXaxis().SetTitle(vars[var])
    hmc.SetMinimum(0.)

    # data histogram attributes
    hdata.SetMarkerStyle(20)
    hdata.SetLineColor(ROOT.kBlack)
    hdata.SetMarkerColor(ROOT.kBlack)
    hdata.SetMarkerSize(0.8)
    
    #ratio data/MC
    norm = 1
    if (hmc.GetSumOfWeights()==0):
        continue
    
    norm = hdata.GetSumOfWeights()/hmc.GetSumOfWeights()
    if 'mass' in var:
        bin1 = hdata.FindBin(87.)
        bin2 = hdata.FindBin(93.)
        norm = hdata.Integral(bin1,bin2)/hmc.Integral(bin1,bin2)
    if 'diphotonMVA' in var:
        bin1 = hdata.FindBin(-0.4)
        bin2 = hdata.FindBin(1.0)
        norm = hdata.Integral(bin1,bin2)/hmc.Integral(bin1,bin2)
        
    hratio = hdata.Clone(hdata.GetName()+'_ratio')
    hratio.Divide(hratio, hmc, 1., norm)
    hratio.GetYaxis().SetRangeUser(0.5,1.5)
    hratio.GetYaxis().SetTitle('data/MC')
    hratio.GetXaxis().SetTitle(vars[var])
    txtSize = hmc.GetYaxis().GetLabelSize()*pad1.GetHNDC()/pad2.GetHNDC()
    hratio.GetXaxis().SetLabelSize(txtSize)
    hratio.GetXaxis().SetTitleSize(txtSize)
    hratio.GetYaxis().SetLabelSize(txtSize)
    hratio.GetYaxis().SetTitleSize(txtSize)
    hratio.GetYaxis().SetTitleOffset(0.7)


    #draw
    pad1.cd()
    hmc.Scale(norm)
    hmc.SetMinimum(0.)
    hmc.SetMaximum(hmc.GetMaximum()*1.5)
    if 'Iso' in var:
        hmc.SetMinimum(1.)
        pad1.cd().SetLogy()
    if 'nvtx' in var:
        hmc.GetXaxis().SetRangeUser(0,40)
        hratio.GetXaxis().SetRangeUser(0,40)
    if 'mass' in var:
        hmc.GetXaxis().SetRangeUser(80,100)
        hratio.GetXaxis().SetRangeUser(80,100)

    hmc.Draw("histo")
    hdata.Draw("esame")

    CMS_lumi.CMS_lumi(pad1, iPeriod, iPos)


    pad2.cd()
    pad2.SetGridy()
    hratio.Draw()
    
    #raw_input("ok?")
    for typ in '.png','.pdf':
        c.SaveAs(outdir+'/'+c.GetName()+typ)
    
