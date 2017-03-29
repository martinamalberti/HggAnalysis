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
CMS_lumi.extraText = "  Preliminary"
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

#filename = '../histograms_SS/histograms'
filename = '../histograms'
processes = ['diphoton','gjet','qcd', 'dibosons', 'DYJetsToLL', 'Top', 'data', 'vh']
proc_label = ['#gamma#gamma+jets' ,"#gamma+jets","jj","dibosons","DY","top",'data','VH'] 

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
             'data':ROOT.kBlack,
             'DYJetsToLL':ROOT.kYellow ,
             'ZGTo2LG' : ROOT.kCyan,
             'WGToLnuG' : ROOT.kCyan,
             'ZZTo2L2Q' : ROOT.kCyan,
             'WZTo2L2Q' : ROOT.kCyan,
             'WW' : ROOT.kCyan,
             'dibosons' : ROOT.kCyan,
             'Top' : ROOT.kMagenta,
             'vh': ROOT.kRed
}

vars = ['nvtx',
        'rho',
        'mass',
        'ptgg',
        'pt_pho1',
        'pt_pho2',
        'eta_pho1',
        'eta_pho2',
        'Idmva_pho1',
        'Idmva_pho2',
        'diphotonMVA',
        'met',
        'met_phi',
        'LeadingMuonPt',
        'LeadingMuonEta',
        'SubleadingMuonPt',
        'SubleadingMuonEta',
        'LeadingElectronPt',
        'LeadingElectronEta',
        'njets',
        'dRMuMu'
]

xtitle = ['number of vertices',
          'rho',
          'mass (GeV)',
          'p^{T}_{#gamma#gamma} (GeV)',
          'leading photon p^{T}/m_{#gamma#gamma}',
          'subleading photon p^{T}/m_{#gamma#gamma}',
          'leading photon #eta',
          'subleading photon #eta',
          'leading photon IDMVA',
          'subleading photon IDMVA',
          'diphoton MVA',
          'MET (GeV)',
          'met #phi',
          'leading muon p^{T} (GeV)',
          'leading muon #eta',
          'subleading muon p^{T} (GeV)',
          'subleading muon #eta',
          'leading electron p^{T} (GeV)',
          'leading electron #eta',
          'number of jets',
          '#DeltaR(#mu#mu)' 
]


histos_by_proc = {}

legend = ROOT.TLegend(0.7,0.6,0.89,0.89)
legend.SetBorderSize(0)

for proc in processes:
    f = ROOT.TFile.Open(filename+'_%s_%s.root'%(tag,proc));
    histos_by_proc[proc] = readPlots(f,vars,proc)
    


for i, var in enumerate(vars):
    # prepare canvas
    c = ROOT.TCanvas(var,var,600,500)
    #pad1 = ROOT.TPad("pad1","top pad",0.0,0.30,1,1)
    #pad1.Draw()
    #pad2 = ROOT.TPad("pad2","bottom pad",0.0,0.03,1,0.33)
    #pad2.Draw()

    # get histograms
    hs = ROOT.THStack('hs_%s'%var,'hs_%s'%var)
    for j, proc in enumerate(processes):
        if ('Pt' in var ): histos_by_proc[proc][var].Rebin(4)
        #if ('Eta' in var ): histos_by_proc[proc][var].Rebin(2)
        #if ('MVA' in var or 'mva' in var): histos_by_proc[proc][var].Rebin(2)
        
        histos_by_proc[proc][var].SetLineColor(colors[proc])
        if ('data' not in proc and 'vh' not in proc):
            histos_by_proc[proc][var].SetFillColor(colors[proc])
            
        if ('data' not in proc and 'vh' not in proc):
            histos_by_proc[proc][var].Scale(intLumi)
            histos_by_proc[proc][var].Scale(mcNormToData)
            #print proc, histos_by_proc[proc][var].GetEntries(), histos_by_proc[proc][var].GetSumOfWeights()
            hs.Add(histos_by_proc[proc][var])
            if (var==vars[0]):
                legend.AddEntry(histos_by_proc[proc][var],proc_label[j],'F')

        if 'vh' in proc:
            ss = 1
            if (histos_by_proc['vh'][var].GetSumOfWeights()!=0): ss = histos_by_proc['data'][var].GetSumOfWeights()/histos_by_proc['vh'][var].GetSumOfWeights()
            histos_by_proc[proc][var].Scale(ss)
            #if (var==vars[0]):
            #    legend.AddEntry(histos_by_proc[proc][var],'VH(125) x 10','L')
                
        if 'data' in proc:
            histos_by_proc[proc][var].SetMarkerStyle(20)
            histos_by_proc[proc][var].SetMarkerSize(0.8)
            #if (var==vars[0]):
            #    legend.AddEntry(histos_by_proc[proc][var],proc,'EP')

    hsig  = histos_by_proc['vh'][var]
    hdata = histos_by_proc['data'][var]
    hallmc = hs.GetStack().Last().Clone('hallmc')
    hallmc.SetLineColor(1)
    hallmc.SetFillColor(1)
    hallmc.SetFillStyle(3004)
    if (var==vars[0]):
        legend.AddEntry(hallmc,'MC stat. unc.','F')
        legend.AddEntry(hdata,'data','EP')
    
    #pad1.cd()
    mm_mc = hs.GetMaximum()
    mm_da = hdata.GetMaximum()
    mm = mm_mc
    if (mm_da > mm_mc): mm = mm_da
    hdata.GetYaxis().SetRangeUser(0, mm*2.0)
    if (var == 'met'):
        c.SetLogy()
        hdata.GetYaxis().SetRangeUser(0.1, 1000)
    hdata.GetXaxis().SetTitle(xtitle[i])
    hdata.Draw('e') 
    hs.Draw('histo same')
    hallmc.Draw('E2 same')
    hdata.Draw('esame')
    if (var == 'diphotonMVA' ): hsig.Draw('histo same')
    legend.Draw()
    CMS_lumi.CMS_lumi(c, iPeriod, iPos)


    tl = ROOT.TLatex( 0.15, 0.85,'%s'%(tag.replace('Tag','')) )
    tl.SetNDC()
    tl.SetTextSize(0.03)
    tl.Draw()
    
    #ratio data/MC
    #hmc = hs.GetStack().Last()
    #print hmc.GetEntries()
    #hratio = hdata.Clone(hdata.GetName()+'_ratio')
    #hratio.Divide(hratio, hmc,1.,1.)
    #hratio.GetYaxis().SetRangeUser(0.0,2.0)
    #hratio.GetYaxis().SetTitle('data/MC')
    #txtSize = hmc.GetYaxis().GetLabelSize()*pad1.GetHNDC()/pad2.GetHNDC()
    #hratio.GetXaxis().SetLabelSize(txtSize)
    #hratio.GetXaxis().SetTitleSize(txtSize)
    #hratio.GetYaxis().SetLabelSize(txtSize)
    #hratio.GetYaxis().SetTitleSize(txtSize)
    #hratio.GetYaxis().SetTitleOffset(0.7)

    #pad2.cd()
    #pad2.SetGridy()
    #hratio.Draw()

    #raw_input('ok?')
    c.SaveAs('controlPlots_'+tag+'/'+c.GetName()+'.png')
    c.SaveAs('controlPlots_'+tag+'/'+c.GetName()+'.pdf')
   

if 'data' in processes:
    ndata = ndata = histos_by_proc['data'][vars[0]].GetEntries()

    nbkg = 0    
    print 'Summary:'
    for proc in processes:
        if 'data' not in proc and 'vh' not in proc:
            nbkg = nbkg + histos_by_proc[proc][vars[0]].GetSumOfWeights()
            print proc, histos_by_proc[proc][vars[0]].GetSumOfWeights()  
            print 'Tot. Bkg. = ', nbkg 
            print 'Tot. Data = ', ndata
            print 'ratio  data/mc = ', ndata/nbkg



    
    
