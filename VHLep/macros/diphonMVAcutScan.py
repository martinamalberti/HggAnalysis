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

iPeriod = 4
iPos = 0
if( iPos==0 ): CMS_lumi.relPosX = 0.10
ROOT.gStyle.SetPadTopMargin(0.08)
ROOT.gStyle.SetPadBottomMargin(0.15)
ROOT.gStyle.SetPadLeftMargin(0.10)
ROOT.gStyle.SetPadRightMargin(0.05)
ROOT.gStyle.cd()
    
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)


def readPlots(f, names, proc):
    h = {}
    for name in names:
        #print name
        h[name] = f.Get('h'+name).Clone('h'+name+'_%s'%proc)
        h[name].SetDirectory(0)
    return h


intLumi = 35.9
mcNormToData = 1.0

tag = sys.argv[1]

sig = 'signal'
sig = 'vh'

filename = '../histograms'
processes = ['diphoton','gjet','qcd', 'dibosons', 'DYJetsToLL', 'Top', sig]
#processes = ['diphoton','gjet','qcd', 'dibosons', 'Top', sig]

colors    = {'tth':ROOT.kBlue, 
             'ggh':ROOT.kBlue+1, 
             'vbf':ROOT.kBlue+2, 
             sig:ROOT.kBlue+3, 
             'signal':ROOT.kBlue,
             'diphoton':ROOT.kGreen+1,
             'gjet':ROOT.kRed, 
             'qcd':ROOT.kAzure-4,
             'pf': ROOT.kRed,
             'ff': ROOT.kAzure-4,
             'data':ROOT.kBlack,
             'DYJetsToLL':ROOT.kYellow ,
             'ZGTo2LG' : ROOT.kCyan,
             'WGToLnuG' : ROOT.kCyan,
             'ZZTo2L2Q' : ROOT.kCyan,
             'WZTo2L2Q' : ROOT.kCyan,
             'WW' : ROOT.kCyan,
             'dibosons' : ROOT.kCyan,
             'Top' : ROOT.kMagenta
}

histos_by_proc = {}

legend = ROOT.TLegend(0.7,0.6,0.89,0.89)
legend.SetBorderSize(0)


vars = ['diphotonMVA']

for proc in processes:
    f = ROOT.TFile.Open(filename+'_%s_%s.root'%(tag,proc));
    histos_by_proc[proc] = readPlots(f,vars,proc)
    



var = 'diphotonMVA'
hs = ROOT.THStack('hs_%s'%var,'hs_%s'%var)
# get histograms
for proc in processes:
    print var, proc
    histos_by_proc[proc][var].SetLineColor(colors[proc])
    #histos_by_proc[proc][var].SetFillColor(colors[proc])
    if sig not in proc:
        histos_by_proc[proc][var].Scale(intLumi)
        histos_by_proc[proc][var].Scale(mcNormToData)
        hs.Add(histos_by_proc[proc][var])

hbkg = hs.GetStack().Last().Clone('hbkg')
hsig = histos_by_proc[sig][var].Clone('hsig')


hbkg.DrawNormalized('e')
hsig.DrawNormalized('e same')
raw_input('ok?')


hbkg.Rebin(5)
hsig.Rebin(5)

nbins = hbkg.GetNbinsX()

print nbins
gEffSig = ROOT.TGraph()
gEffBkg = ROOT.TGraph()

for ibin in range(1,nbins):
    diphomva = hsig.GetBinCenter(ibin)
    effSig = hsig.Integral(ibin,nbins)/hsig.Integral(1,nbins)
    effBkg = hbkg.Integral(ibin,nbins)/hbkg.Integral(1,nbins)

    gEffSig.SetPoint(ibin-1,diphomva,effSig)
    gEffBkg.SetPoint(ibin-1,diphomva,effBkg)


gEffSig.SetMarkerColor(ROOT.kBlue)
gEffSig.SetMarkerStyle(20)
gEffBkg.SetMarkerColor(ROOT.kRed)
gEffBkg.SetMarkerStyle(20)


# prepare canvas
c = ROOT.TCanvas('eff_vs_diphoMVA','eff_vs_diphoMVA',600,500)
c.SetGridx()
c.SetGridy()
gEffSig.GetHistogram().GetXaxis().SetRangeUser(-1.0,1.0)
gEffSig.GetHistogram().GetYaxis().SetRangeUser(0,1.2)
gEffSig.GetHistogram().GetXaxis().SetTitle('diphotonMVA threshold')
gEffSig.GetHistogram().GetYaxis().SetTitle('efficiency')
gEffSig.GetHistogram().GetYaxis().SetTitleOffset(0.85)
gEffSig.Draw('ap')
gEffBkg.Draw('psame')

legend = ROOT.TLegend(0.15,0.2,0.4,0.4)
legend.SetBorderSize(1)
legend.AddEntry(gEffSig, 'VH(125)','P')    
legend.AddEntry(gEffBkg, 'background','P')
legend.Draw('same')

tl = ROOT.TLatex( 0.15, 0.85, '%s'%tag)
tl.SetNDC()
tl.SetTextSize(0.03)
tl.Draw()

    
raw_input('ok?')
c.SaveAs('controlPlots_'+tag+'/'+c.GetName()+'.png')
c.SaveAs('controlPlots_'+tag+'/'+c.GetName()+'.pdf')
   


    
    
