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

#f1 = ROOT.TFile.Open("../h_ggh_ichep16.root")
#f2 = ROOT.TFile.Open("../h_zee_ichep16.root")
f1 = ROOT.TFile.Open("../h_ggh.root")
f2 = ROOT.TFile.Open("../h_zee.root")

#f2 = ROOT.TFile.Open("../h_ggh90.root")

outdir = sys.argv[1]

vars = {
    
        "massRatio_EBEB" : "m_{transf}/m_{nominal}",
        "massRatio_EBEB_highR9" : "m_{transf}/m_{nominal}",
        "massRatio_EBEB_highR9_inner" : "m_{transf}/m_{nominal}",
        "massRatio_EBEB_highR9_outer" : "m_{transf}/m_{nominal}",
        "massRatio_EBEB_lowR9" : "m_{transf}/m_{nominal}",
        "massRatio_EBEB_lowR9_inner" : "m_{transf}/m_{nominal}",
        "massRatio_EBEB_lowR9_outer" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE_highR9" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE_highR9_inner" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE_highR9_outer" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE_lowR9" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE_lowR9_inner" : "m_{transf}/m_{nominal}",
        "massRatio_EEEE_lowR9_outer" : "m_{transf}/m_{nominal}"
}


for ivar, var in enumerate(vars):
    #print var
    # prepare canvas
    c = ROOT.TCanvas(var,var,500,500)

    # get histograms
    hname = 'h'+var
            
    h1 = f1.Get(hname)
    h2 = f2.Get(hname)

    nRe = 2
    if ('EEEE' in var):
        nRe = 4
    h1.Rebin(nRe)
    h2.Rebin(nRe)
        
    h1.Scale(1./h1.GetSumOfWeights())
    h2.Scale(1./h2.GetSumOfWeights())
    
    
    # mc histogram attributes
    h1.SetLineColor(ROOT.kAzure+1)
    h1.GetXaxis().SetTitle(vars[var])
    h1.SetMinimum(0.)

    # data histogram attributes
    h2.SetLineColor(ROOT.kRed)

    k = 2.
   
    #get mean from gaus fit
    #mean = h1.GetMean()
    mean = h1.GetBinCenter(h1.GetMaximumBin()) 
    minx = mean - k*h1.GetRMS()
    maxx = mean + k*h1.GetRMS()    
    fit1 = ROOT.TF1('fit1%s'%var,'gaus')
    fit1.SetRange(minx,maxx)
    fit1.SetLineColor(ROOT.kAzure+1)
    h1.Fit('fit1%s'%var,'QSR+')

    #mean = h2.GetMean()
    mean = h2.GetBinCenter(h2.GetMaximumBin()) 
    minx = mean - k*h2.GetRMS()
    maxx = mean + k*h2.GetRMS()
    fit2 = ROOT.TF1('fit2%s'%var,'gaus')
    fit2.SetRange(minx,maxx)
    fit2.SetLineColor(ROOT.kRed+1)
    h2.Fit('fit2%s'%var,'QSR+')

    scale_pho = fit1.GetParameter(1)
    scale_ele = fit2.GetParameter(1)

    scaleDiff = (scale_pho/scale_ele) - 1
    scaleDiffError = math.sqrt(fit1.GetParError(1)*fit1.GetParError(1) + fit2.GetParError(1)*fit2.GetParError(1))

    
    #print 'cat','Hgg', 'Zee', 'Relative scale diff.'
    print '%s %.4f %.4f %.4f +/-  %.4f'%(  var.replace('massRatio_',''), scale_pho, scale_ele, scaleDiff, scaleDiffError )
    print '%s %.4f %.4f %.4f'%(  var.replace('massRatio_',''), h1.GetMean(), h2.GetMean(), ( h1.GetMean() - h2.GetMean()  )  )

    tl = ROOT.TLatex( 0.15, 0.73,'#gamma/e scale diff. = %.2f +/- %.2f'%( scaleDiff*100, scaleDiffError*100)+'%' )
    #tl = ROOT.TLatex( 0.15, 0.73,'#gamma/e scale diff. = %.2f'%( scaleDiff*100)+'%' )
    tl.SetNDC()
    tl.SetTextSize(0.03)

    tl2 = ROOT.TLatex( 0.15, 0.85, var.replace('massRatio_','').replace('_',' ').replace('inner','low #eta').replace('outer','high #eta'))
    tl2.SetNDC()
    tl2.SetTextSize(0.03)

    tl3 = ROOT.TLatex( 0.15, 0.81,'#gamma - scale = %.4f'%( scale_pho ) )
    tl3.SetNDC()
    tl3.SetTextSize(0.03)

    tl4 = ROOT.TLatex( 0.15, 0.77,'e - scale = %.4f'%( scale_ele ) )
    tl4.SetNDC()
    tl4.SetTextSize(0.03)
    
    print ivar
    if (ivar == 0):
        leg = ROOT.TLegend(0.67,0.8,0.89,0.89)
        leg.AddEntry(h1, 'H(125) #rightarrow #gamma#gamma', 'L')
        leg.AddEntry(h2, 'Z #rightarrow ee', 'L')
#        leg.AddEntry(h1, 'H(90) #rightarrow #gamma#gamma', 'L')
    
    #draw
    h1.GetXaxis().SetRangeUser(0.98, 1.02)
    h1.SetMinimum(0.)
    h1.SetMaximum(h1.GetMaximum()*1.4)
  
    h1.Draw("histo")
    fit1.Draw('same')

    h2.Draw('histo same')
    fit2.Draw('same')

    tl.Draw()
    tl2.Draw()
    tl3.Draw()
    tl4.Draw()
    leg.Draw('same')
    
    c.Update()
    
    #CMS_lumi.CMS_lumi(pad1, iPeriod, iPos)
    
    raw_input("ok?")
    for typ in '.png','.pdf':
        c.SaveAs(outdir+'/'+c.GetName()+typ)
    
