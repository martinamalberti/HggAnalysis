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
#intLumi = 1.

#fmc1 = ROOT.TFile.Open("../hmc_rhoWeighted2016B.root")
#fmc2 = ROOT.TFile.Open("../hmc_rhoWeighted2016F.root")
#fmc3 = ROOT.TFile.Open("../hmc_rhoWeighted2016G.root")
#fdata1 = ROOT.TFile.Open("../hdata_2016B.root")
#fdata2 = ROOT.TFile.Open("../hdata_2016F.root")
#fdata3 = ROOT.TFile.Open("../hdata_2016G.root")

#fmc1 = ROOT.TFile.Open("../hmc_rhoWeighted2016E_rho0to5.root")
#fmc2 = ROOT.TFile.Open("../hmc_rhoWeighted2016G_rho0to5.root")
#fmc3 = ROOT.TFile.Open("../hmc_rhoWeighted2016H_rho0to5.root")
#fdata1 = ROOT.TFile.Open("../hdata_reminiAOD_Run2016E_rho0to5.root")
#fdata2 = ROOT.TFile.Open("../hdata_reminiAOD_Run2016G_rho0to5.root")
#fdata3 = ROOT.TFile.Open("../hdata_reminiAOD_Run2016H_rho0to5.root")

#fmc1 = ROOT.TFile.Open("../hmc_rhoWeighted2016B.root")
#fmc2 = ROOT.TFile.Open("../hmc_rhoWeighted2016D.root")
#fmc3 = ROOT.TFile.Open("../hmc_rhoWeighted2016H.root")
#fdata1 = ROOT.TFile.Open("../hdata_reminiAOD_Run2016B.root")
#fdata2 = ROOT.TFile.Open("../hdata_reminiAOD_Run2016D.root")
#fdata3 = ROOT.TFile.Open("../hdata_reminiAOD_Run2016H.root")

fmc1 = ROOT.TFile.Open("../hmc_DYJetsToLL_rhoWeighted2016H.root")
fmc2 = ROOT.TFile.Open("../hmc_DYToEE_EGM0_rhoWeighted2016H.root")
fdata1 = ROOT.TFile.Open("../hdata_reMiniAOD_Run2016H.root")
fdata2 = ROOT.TFile.Open("../hdata_reMiniAOD_Run2016H.root")

#mcfiles = [fmc1, fmc2, fmc3]
#datafiles = [fdata1, fdata2, fdata3]

mcfiles = [fmc1, fmc2]
datafiles = [fdata1, fdata2]

col = [ROOT.kRed, ROOT.kAzure, ROOT.kGreen+2 ]


outdir = sys.argv[1]

vars = {"nvtx" : "number of reconstructed vertices",
        "rho"  : "rho",

        "pt" : "p_{T} (GeV)",

        "mass" : "mass (GeV)",
        "mass_EBEB_BothHighR9" : "mass (GeV)",
        "mass_EBEB_NotBothHighR9" : "mass (GeV)",
        "mass_NotEBEB_BothHighR9" : "mass (GeV)",
        "mass_NotEBEB_NotBothHighR9" : "mass (GeV)",

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
        "lead_uncorr_full5x5_r9_EB" : "leading photon uncorrected full5x5 r9",
        "lead_uncorr_etaWidth_EB" : "leading photon uncorrected etaWidth",
        "lead_uncorr_s4_EB" : "leading photon uncorrected s4",
        "lead_full5x5_sigmaIetaIeta_EB" : "leading photon uncorrected full5x5 sigmaIetaIeta",
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
        "lead_uncorr_full5x5_r9_EE" : "leading photon uncorrected full5x5 r9",
        "lead_uncorr_etaWidth_EE" : "leading photon uncorrected etaWidth",
        "lead_uncorr_s4_EE" : "leading photon uncorrected s4",
        "lead_uncorr_full5x5_sigmaIetaIeta_EE" : "leading photon uncorrected full5x5 sigmaIetaIeta",
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

}


#vars = {"nvtx" : "number of reconstructed vertices",
#        "rho"  : "rho",
#        "lead_full5x5_r9_EB" : "leading photon full5x5 r9",
#        "lead_etaWidth_EB" : "leading photon etaWidth",
#        "lead_s4_EB" : "leading photon s4",
#        "lead_full5x5_sigmaIetaIeta_EB" : "leading photon full5x5 sigmaIetaIeta",
#        "lead_pfPhoIso03_EB" : "leading photon pfPhoIso03",
#        "lead_sigmaEoE_EB" : "leading photon sigmaE/E",
#        "lead_idmva_EB" : "leading photon idmva",
#        "lead_full5x5_r9_EE" : "leading photon full5x5 r9",
#        "lead_etaWidth_EE" : "leading photon etaWidth",
#        "lead_s4_EE" : "leading photon s4",
#        "lead_full5x5_sigmaIetaIeta_EE" : "leading photon full5x5 sigmaIetaIeta",
#        "lead_pfPhoIso03_EE" : "leading photon pfPhoIso03",
#        "lead_ESoRawE_EE" : "leading photon E_{ES}/rawE_{SC}",
#        "lead_sigmaEoE_EE" : "leading photon sigmaE/E",
#        "lead_idmva_EE" : "leading photon idmva",
#        }

#vars = {
#        "rho"  : "rho",
#        "lead_full5x5_r9_EB" : "leading photon full5x5 r9",
#        "lead_pfPhoIso03_EB" : "leading photon pfPhoIso03",
#        "lead_idmva_EB" : "leading photon idmva",
#        "lead_full5x5_r9_EE" : "leading photon full5x5 r9",
#        "lead_pfPhoIso03_EE" : "leading photon pfPhoIso03",
#        "lead_idmva_EE" : "leading photon idmva",
#        }

for ivar,var in enumerate(vars):
    print var
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

    #get mc histograms
    hmc = []    
    for i, f in enumerate(mcfiles):    
        htemp = f.Get(hname)
        hmc.append( htemp )
        hmc[i].SetLineColor(col[i])
        hmc[i].GetYaxis().SetNoExponent()
        hmc[i].GetXaxis().SetTitle(vars[var])
        hmc[i].SetMinimum(0.)

        if 'rawEnergy' in hmc[i].GetName():
            hmc[i].Rebin(5)

    
    #get data histograms
    hdata = []
    hratio = []
    for j, f in enumerate(datafiles):
        htemp = f.Get(hname)
        hdata.append(htemp)
        hdata[j].SetLineColor(col[j])
        hdata[j].SetMarkerColor(col[j])
        hdata[j].SetMarkerStyle(20)
        hdata[j].SetMarkerSize(0.5)
        if 'rawEnergy' in hdata[j].GetName():
            hdata[j].Rebin(5)
            
        #ratio data/MC
        norm = hdata[j].GetSumOfWeights()/hmc[j].GetSumOfWeights()
        if 'mass' in var:
            bin1 = hdata[j].FindBin(87.)
            bin2 = hdata[j].FindBin(93.)
            norm = hdata[j].Integral(bin1,bin2)/hmc[j].Integral(bin1,bin2)

        hratio.append(hdata[j].Clone(hdata[j].GetName()+'_ratio_%d'%j))
        hratio[j].Divide(hratio[j], hmc[j], 1., norm)
        hratio[j].GetYaxis().SetRangeUser(0.5,1.5)
        hratio[j].GetYaxis().SetTitle('data/MC')
        hratio[j].GetXaxis().SetTitle(vars[var])
        txtSize = hmc[j].GetYaxis().GetLabelSize()*pad1.GetHNDC()/pad2.GetHNDC()
        hratio[j].GetXaxis().SetLabelSize(txtSize)
        hratio[j].GetXaxis().SetTitleSize(txtSize)
        hratio[j].GetYaxis().SetLabelSize(txtSize)
        hratio[j].GetYaxis().SetTitleSize(txtSize)
        hratio[j].GetYaxis().SetTitleOffset(0.7)


        #draw
        pad1.cd()
        #hmc[j].Scale(norm1)
        hmc[j].SetMinimum(0.)
        hmc[j].SetMaximum(hmc[j].GetMaximum()*1.5)
        if 'Iso' in var:
            hmc[j].SetMinimum(1.)
            pad1.cd().SetLogy()
        if 'nvtx' in var:
            hmc[j].GetXaxis().SetRangeUser(0,40)
            hratio[j].GetXaxis().SetRangeUser(0,40)
        if 'mass' in var:
            hmc[j].GetXaxis().SetRangeUser(80,100)
            hratio[j].GetXaxis().SetRangeUser(80,100)
        if 'r9' in var and 'EE' in var:
            hmc[j].GetXaxis().SetRangeUser(0.8,1.0)
            hratio[j].GetXaxis().SetRangeUser(0.8,1.0)
        if 'r9' in var and 'EB' in var:
            hmc[j].GetXaxis().SetRangeUser(0.5,1.0)
            hratio[j].GetXaxis().SetRangeUser(0.5,1.0)

        if (j == 0 ):        
            hmc[j].DrawNormalized("histo")
            hdata[j].DrawNormalized("esame")
        else:
            hmc[j].DrawNormalized("histo same")
            hdata[j].DrawNormalized("esame")


        CMS_lumi.CMS_lumi(pad1, iPeriod, iPos)

        pad2.cd()
        pad2.SetGridy()
        if (j==0):
            hratio[j].Draw()
        else:
            hratio[j].Draw('esame')

    
    if (ivar == 0): 
        legend = ROOT.TLegend(0.60, 0.75, 0.9, 0.9)
        legend.SetTextSize(0.04)
        #legend.AddEntry(hdata[0],'Data - Run2016B','PL')
        #legend.AddEntry(hdata[1],'Data - Run2016D','PL')
        #legend.AddEntry(hdata[2],'Data - Run2016H','PL')
        legend.AddEntry(hmc[0],'DYJetsToLL','F')
        legend.AddEntry(hmc[1],'DYToEE_EGM0','F')

    pad1.cd()
    legend.Draw('same')
    
    
    #raw_input("ok?")
    for typ in '.png','.pdf':
        c.SaveAs(outdir+'/'+c.GetName()+typ)
    
