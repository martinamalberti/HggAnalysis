#! /usr/bin/env python
import os
import glob
import math
import array
import sys
import time

import ROOT

#filename = '../histograms_VHLeptonicLooseTag.root'
filename = '../histograms_WHLeptonicTag.root'
f = ROOT.TFile.Open(filename)

#processes = ['ggh', 'ggh_powheg','tth', 'vbf', 'vh', 'diphoton']
#processes = ['vh', 'ggh', 'ggh_powheg','tth', 'vbf']
processes = ['vh','vbf', 'tth']
#processes = ['ggh', 'ggh_powheg', 'vh']

colors    = {'tth':ROOT.kOrange, 
             'ggh':ROOT.kBlue, 
             'ggh_powheg':ROOT.kBlue+2, 
             'vbf':ROOT.kGreen+2, 
             'vh' :ROOT.kRed, 
             'diphoton' : ROOT.kBlack
}

vars = ['ptgg',
        'pt_pho1',
        'pt_pho2',
        'pt_jet1',
        'pt_jet2',
        'pt_jet3',
        'diphotonMVA',
        'njets'
]

xtitles = ['p^{T}_{#gamma#gamma} (GeV)',
           'p^{T}_{#gamma1}/m_{#gamma#gamma}',
           'p^{T}_{#gamma2}/m_{#gamma#gamma}',
           'leading jet p^{T} (GeV)',
           'subleading jet p^{T} (GeV)',
           'third jet p^{T} (GeV)',
           'diphoton MVA',
           'number of jets'
          ]

h = {}

legend = ROOT.TLegend(0.5,0.6,0.85,0.89)
legend.SetBorderSize(0)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

for j,var in enumerate(vars):
    c = ROOT.TCanvas(var,var,500,500)
    c.SetTickx()
    c.SetTickx()
    print var
    for i, proc in enumerate(processes):
        #print proc
        hh = f.Get('h'+var+'_'+proc)
        #print hh.GetEntries()
        h[proc] = hh
        h[proc].SetLineColor(colors[proc])
        h[proc].SetLineWidth(2)
        if ('powheg' in proc):
            h[proc].SetLineStyle(2)

        if proc == processes[0]:
            h[proc].SetMinimum(0.00001)
            mm = h[proc].GetMaximum()
            h[proc].SetMaximum(mm*1.3)
            if ('mjj' in var):
                h[proc].SetMaximum(mm*2.5)
            h[proc].GetXaxis().SetTitle(xtitles[j])
            h[proc].DrawNormalized('histo')
        else:
            h[proc].DrawNormalized('histo same')
            
        if (var == vars[0]):
            legend.AddEntry(h[proc],proc,'L')
               
    c.cd()
    legend.Draw('same')
    c.Update()

    raw_input('ok?')
    c.SaveAs('plots/'+c.GetName()+'.png')
    c.SaveAs('plots/'+c.GetName()+'.pdf')
 
    
