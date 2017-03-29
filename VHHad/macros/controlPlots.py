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


intLumi = 36.8
dataToMC = 1.0

filename = '../histograms_looseCuts/histograms'
processes = ['diphoton','pf','ff','data']
#processes = ['diphoton','gjet','qcd','data']
#processes = ['diphoton','gjet','qcd']

colors    = {'tth':ROOT.kBlue, 
             'ggh':ROOT.kBlue+1, 
             'vbf':ROOT.kBlue+2, 
             'vh':ROOT.kBlue+3, 
             'signal':ROOT.kBlue,
             'diphoton':ROOT.kGreen+1,
             'gjet':ROOT.kRed, 
             'qcd':ROOT.kAzure-4,
             'pf': ROOT.kRed,
             'ff': ROOT.kAzure-4,
             'data':ROOT.kBlack
             }

vars = ['nvtx',
        'rho',
        'mass',
        'ptgg',
        'pt_pho1',
        'pt_pho2',
        'Idmva_pho1',
        'Idmva_pho2',
        'pt_jet1',
        'pt_jet2',
        'mjj',
        'costhetastar',
        'diphotonMVA',
        'QGL_jet1',
        'QGL_jet2',
        'bdisc_jet1',
        'bdisc_jet2'
]


#vars = ['mass',
#        'Idmva_pho1',
#        'Idmva_pho2'
#        ]

xtitle = ['rho',
          'number of vertices',
          'mass (GeV)',
          'p^{T}_{#gamma#gamma} (GeV)',
          'p^{T}_{#gamma1}/m_{#gamma#gamma}',
          'p^{T}_{#gamma2}/m_{#gamma#gamma}',
          'leading photon IDMVA',
          'subleading photon IDMVA',
          'leading jet p^{T} (GeV)',
          'subleading jet p^{T} (GeV)',
          'm_{jj} (GeV)',
          'cos(#theta*)',
          'diphoton MVA',
          'leading jet QGL',
          'subleading jet QGL',
          'leading jet b-discriminant',
          'subleading jet b-discriminant'
]


histos_by_proc = {}

legend = ROOT.TLegend(0.7,0.7,0.89,0.89)
legend.SetBorderSize(1)

for proc in processes:
    f = ROOT.TFile.Open(filename+'_%s.root'%proc);
    histos_by_proc[proc] = readPlots(f,vars,proc)


for i, var in enumerate(vars):
    # prepare canvas
    c = ROOT.TCanvas(var,var,800,800)
    pad1 = ROOT.TPad("pad1","top pad",0.0,0.35,1,1)
    pad1.Draw()
    pad2 = ROOT.TPad("pad2","bottom pad",0.0,0.05,1,0.37)
    pad2.Draw()

    # get histograms
    hs = ROOT.THStack('hs_%s'%var,'hs_%s'%var)
    for proc in processes:
        histos_by_proc[proc][var].SetLineColor(colors[proc])
        histos_by_proc[proc][var].SetFillColor(colors[proc])
        if 'data' not in proc:
            histos_by_proc[proc][var].Scale(intLumi)
            histos_by_proc[proc][var].Scale(dataToMC)
            #print proc, histos_by_proc[proc][var].GetEntries(), histos_by_proc[proc][var].GetSumOfWeights()
            hs.Add(histos_by_proc[proc][var])
            if (var==vars[0]):
                legend.AddEntry(histos_by_proc[proc][var],proc,'F')
        if 'data' in proc:
            histos_by_proc[proc][var].SetMarkerStyle(20)
            histos_by_proc[proc][var].SetMarkerSize(0.8)
            if (var==vars[0]):
                legend.AddEntry(histos_by_proc[proc][var],proc,'EP')

    hdata = histos_by_proc['data'][var]

    pad1.cd()
    mm = hdata.GetMaximum()
    hdata.GetYaxis().SetRangeUser(0, mm*1.5)
    hdata.GetXaxis().SetTitle(xtitle[i])
    hdata.Draw('e') 
    hs.Draw('histo same')
    hdata.Draw('esame') 
    legend.Draw()
    CMS_lumi.CMS_lumi(pad1, iPeriod, iPos)

    #ratio data/MC
    hmc = hs.GetStack().Last()
    print hmc.GetEntries()
    hratio = hdata.Clone(hdata.GetName()+'_ratio')
    hratio.Divide(hratio, hmc,1.,1.)
    hratio.GetYaxis().SetRangeUser(0.0,2.0)
    hratio.GetYaxis().SetTitle('data/MC')
    txtSize = hmc.GetYaxis().GetLabelSize()*pad1.GetHNDC()/pad2.GetHNDC()
    hratio.GetXaxis().SetLabelSize(txtSize)
    hratio.GetXaxis().SetTitleSize(txtSize)
    hratio.GetYaxis().SetLabelSize(txtSize)
    hratio.GetYaxis().SetTitleSize(txtSize)
    hratio.GetYaxis().SetTitleOffset(0.7)

    pad2.cd()
    pad2.SetGridy()
    hratio.Draw()

    #raw_input('ok?')
    c.SaveAs('controlPlots/'+c.GetName()+'.png')
    c.SaveAs('controlPlots/'+c.GetName()+'.pdf')
   

if 'data' in processes:
    ndata = ndata = histos_by_proc['data'][vars[0]].GetEntries()

    nbkg = 0    
    print 'Summary:'
    for proc in processes:
        if 'data' not in proc:
            nbkg = nbkg + histos_by_proc[proc][vars[0]].GetSumOfWeights()
            print proc, histos_by_proc[proc][vars[0]].GetSumOfWeights()  
            print 'Tot. Bkg. = ', nbkg 
            print 'Tot. Data = ', ndata
            print 'ratio  data/mc = ', ndata/nbkg



    
    
