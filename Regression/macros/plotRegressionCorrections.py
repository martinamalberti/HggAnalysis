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

ROOT.gStyle.SetLegendFont(43)
#ROOT.gStyle.SetTextFont(43)
#ROOT.gStyle.SetLabelFont(43,"X")
#ROOT.gStyle.SetLabelFont(43,"Y")
#ROOT.gStyle.SetTitleFont(43,"X")
#ROOT.gStyle.SetTitleFont(43,"Y")
ROOT.gStyle.SetPadTopMargin(0.08)
ROOT.gStyle.SetPadBottomMargin(0.15)
ROOT.gStyle.SetPadLeftMargin(0.10)
ROOT.gStyle.SetPadRightMargin(0.05)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptFit(0)
ROOT.gStyle.cd()

intLumi = 35.9

f1 = ROOT.TFile.Open("../h_ggh.root")
f2 = ROOT.TFile.Open("../h_zee.root")
#f1 = ROOT.TFile.Open("../h_ggh_ichep16.root")
#f2 = ROOT.TFile.Open("../h_zee_ichep16.root")

outdir = sys.argv[1]

vars = { "regrCorr_vs_r9_EB_highR9" : "r9",
         "regrCorr_vs_r9_EB_lowR9" : "r9",
         "regrCorr_vs_sietaieta_EB_highR9" : "#sigma_{i#eta i#eta}",
         "regrCorr_vs_sietaieta_EB_lowR9" : "#sigma_{i#eta i#eta}",
         "regrCorr_vs_etaWidth_EB_highR9" : "etaWidth",
         "regrCorr_vs_etaWidth_EB_lowR9" : "etaWidth",
         
         "regrCorr_vs_r9_EE_highR9" : "r9",
         "regrCorr_vs_r9_EE_lowR9" : "r9",
         "regrCorr_vs_sietaieta_EE_highR9" : "#sigma_{i#eta i#eta}",
         "regrCorr_vs_sietaieta_EE_lowR9" : "#sigma_{i#eta i#eta}",
         "regrCorr_vs_etaWidth_EE_highR9" : "etaWidth",
         "regrCorr_vs_etaWidth_EE_lowR9" : "etaWidth",

         "regrCorr_vs_eta_highR9" : "eta",
         "regrCorr_vs_eta_lowR9" : "eta",

}

for ivar, var in enumerate(vars):
    #print var
    # prepare canvas
    #c = ROOT.TCanvas(var,var,500,500)
    #c.SetGridx()
    #c.SetGridy()
    c = ROOT.TCanvas(var,var,700,500)
    pad1 = ROOT.TPad("pad1","top pad",0.0,0.35,1,1)
    pad1.Draw()
    pad2 = ROOT.TPad("pad2","bottom pad",0.0,0.05,1,0.37)
    pad2.Draw()
    pad1.SetGridx()
    pad1.SetGridy()
    pad2.SetGridx()
    pad2.SetGridy()
    
    # get histograms
    hname = 'p_'+var
            
    h1 = f1.Get(hname)
    h2 = f2.Get(hname)

    nRe = 1
    h1.Rebin(nRe)
    h2.Rebin(nRe)
        
    
    # mc histogram attributes
    h1.SetLineColor(ROOT.kAzure+1)
    h1.GetXaxis().SetTitle(vars[var])
    h1.GetYaxis().SetTitle('<E_{regr}/E_{raw}>')
    h1.GetYaxis().SetTitleOffset(1.3)
 
    # data histogram attributes
    h2.SetLineColor(ROOT.kRed)
    
    # axis ranges
    if ('vs_r9_EB_highR9' in var): h1.GetXaxis().SetRangeUser(0.94,1)
    if ('vs_r9_EB_lowR9'  in var): h1.GetXaxis().SetRangeUser(0.5,0.94)

    if ('vs_r9_EE_highR9' in var): h1.GetXaxis().SetRangeUser(0.94,1)
    if ('vs_r9_EE_lowR9'  in var): h1.GetXaxis().SetRangeUser(0.8,0.94)

    if ('vs_sietaieta_EB' in var): h1.GetXaxis().SetRangeUser(0.007,0.012)
    if ('vs_sietaieta_EE' in var): h1.GetXaxis().SetRangeUser(0.02,0.03)

    if ('vs_etaWidth_EB' in var): h1.GetXaxis().SetRangeUser(0.005,0.02)
    if ('vs_etaWidth_EE' in var): h1.GetXaxis().SetRangeUser(0.005,0.035)


    # label plot
    label = 'EB high R9'
    if ( 'EB_lowR9' in var ) : label = 'EB low R9'
    if ( 'EE_highR9' in var ) : label = 'EE high R9'
    if ( 'EE_lowR9' in var ) : label = 'EE low R9'
    if ( 'vs_eta_highR9' in var ) : label = 'high R9'
    if ( 'vs_eta_lowR9' in var ) : label = 'low R9'
    tl = ROOT.TLatex( 0.15, 0.85, label )
    tl.SetNDC()
    tl.SetTextSize(0.06)

    # ratio plot
    hratio = h1.Clone(h1.GetName()+'_ratio')
    hratio.Divide(hratio, h2, 1., 1.)
    hratio.GetYaxis().SetRangeUser(0.9,1.1)
    hratio.GetYaxis().SetTitle('ratio')
    txtSize = h1.GetYaxis().GetLabelSize()*pad1.GetHNDC()/pad2.GetHNDC()
    hratio.GetXaxis().SetLabelSize(txtSize)
    hratio.GetXaxis().SetTitleSize(txtSize)
    hratio.GetYaxis().SetLabelSize(txtSize)
    hratio.GetYaxis().SetTitleSize(txtSize)
    hratio.GetYaxis().SetTitleOffset(0.7)
    
    #draw
    pad1.cd()
    h1.GetYaxis().SetRangeUser(0.8, 1.2)
    h1.Draw('')
    h2.Draw('same')
    pad2.cd()
    hratio.SetLineColor(1)
    hratio.Draw('histo same')
    
    print ivar
    if (ivar == 0):
        leg = ROOT.TLegend(0.67,0.8,0.89,0.89)
        leg.AddEntry(h1, 'H(125) #rightarrow #gamma#gamma', 'L')
        leg.AddEntry(h2, 'Z #rightarrow ee', 'L')
        #        leg.AddEntry(h1, 'H(90) #rightarrow #gamma#gamma', 'L')

    pad1.cd()
    leg.Draw('same')
    tl.Draw()
    
    c.Update()
    
    #CMS_lumi.CMS_lumi(pad1, iPeriod, iPos)
    
    raw_input("ok?")
    for typ in '.png','.pdf':
        c.SaveAs(outdir+'/'+c.GetName()+typ)
    
