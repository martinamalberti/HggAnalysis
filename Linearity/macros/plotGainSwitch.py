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
CMS_lumi.lumi_13TeV = "36.8 fb^{-1}"
CMS_lumi.writeExtraText = 1
CMS_lumi.extraText = "Preliminary"
CMS_lumi.lumi_sqrtS = "13 TeV" # used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

#iPeriod = 0
#iPos = 11
iPeriod = 4
iPos = 0
if( iPos==0 ): CMS_lumi.relPosX = 0.11

ROOT.gStyle.SetPadTopMargin(0.08)
ROOT.gStyle.SetPadBottomMargin(0.15)
ROOT.gStyle.SetPadLeftMargin(0.10)
ROOT.gStyle.SetPadRightMargin(0.05)
ROOT.gStyle.SetOptStat(1110)

ROOT.gStyle.cd()

f = ROOT.TFile.Open("../checkGainSwitch.root")


hGainSwitchByCat = f.Get('hGainSwitchByCat')
c = ROOT.TCanvas('fractionGainSwitch','fractionGainSwitch',600,500)
hGainSwitchByCat.SetLineWidth(2)
hGainSwitchByCat.GetYaxis().SetTitle('fraction of events with gain switch')
hGainSwitchByCat.GetXaxis().SetTitle('category')
hGainSwitchByCat.GetXaxis().SetBinLabel(1,"Untagged0")
hGainSwitchByCat.GetXaxis().SetBinLabel(2,"Untagged1")
hGainSwitchByCat.GetXaxis().SetBinLabel(3,"Untagged2")
hGainSwitchByCat.GetXaxis().SetBinLabel(4,"Untagged3")
hGainSwitchByCat.GetXaxis().SetBinLabel(5,"VBF0")
hGainSwitchByCat.GetXaxis().SetBinLabel(6,"VBF1")
hGainSwitchByCat.GetXaxis().SetBinLabel(7,"VBF2")
hGainSwitchByCat.GetXaxis().SetBinLabel(8,"TTHHadronic")
hGainSwitchByCat.GetXaxis().SetBinLabel(9,"TTHLeptonic")
hGainSwitchByCat.GetXaxis().SetBinLabel(10,"ZHLeptonic")
hGainSwitchByCat.GetXaxis().SetBinLabel(11,"WHLeptonic")
hGainSwitchByCat.GetXaxis().SetBinLabel(12,"VHLeptonicLoose")
hGainSwitchByCat.GetXaxis().SetBinLabel(13,"VHHadronic")
hGainSwitchByCat.GetXaxis().SetBinLabel(14,"VHMet")
hGainSwitchByCat.Draw('histo')

hmass0 = []
hmass = []

legend = ROOT.TLegend(0.15, 0.8, 0.45, 0.89)
for icat in range(0,8):
    print icat
    hmass0.append(f.Get('hmass0_cat%d'%icat))
    hmass.append(f.Get('hmass_cat%d'%icat))
    cmass = ROOT.TCanvas('cmass_cat%d'%icat, 'cmass_cat%d'%icat, 500, 500)
    
    hmass0[icat].SetLineColor(2)
    mm = hmass0[icat].GetMaximum()
    hmass0[icat].GetYaxis().SetRangeUser(0, mm*1.2)
    hmass0[icat].GetXaxis().SetTitle('m_{#gamma#gamma} (GeV)')
    hmass0[icat].Draw('histo')

    hmass[icat].SetLineColor(4)
    hmass[icat].Draw('histosames')

    if (icat==0):
        legend.AddEntry(hmass0[icat],'nominal','L')
        legend.AddEntry(hmass[icat],'after gain switch scale corr.','L')

    legend.Draw('same')
    
    raw_input('ok?')
