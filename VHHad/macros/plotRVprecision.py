#! /usr/bin/env python
import os
import glob
import math
import array
import sys
import time

import ROOT


def get1sigma(g):

    xmin =-10
    xmax = 10
    for i in range(0,g.GetN()):
        if ( g.GetX()[i]>0): continue
        if (g.GetY()[i] <= 3):
            xmin = g.GetX()[i]
            break
    for i in range(0,g.GetN()):
        if ( g.GetX()[i]<0): continue
        if (g.GetY()[i] >= 3):
            xmax = g.GetX()[i]
            break
    
    #fitfun = ROOT.TF1('fitfun','[1] + [2]*(x-[0]) + [3]*pow(x-[0],2) + [4]*pow(x-[0],3) +[5]*pow(x-[0],4)')
    #fitfun.FixParameter(0,1)
    #fitfun.FixParameter(1,0)
    fitfun = ROOT.TF1('fitfun','pol6')
    fitfun.SetRange(xmin,xmax)
    g.Fit('fitfun','RQS+')
    #fitfun.SetRange(-10.,10.)
    g.Draw('ap*')
    fitfun.Draw('same')
    g.GetHistogram().GetXaxis().SetTitle('RV')
    g.GetHistogram().GetYaxis().SetRangeUser(0,4)
    g.GetHistogram().GetYaxis().SetTitle('2*deltaNLL')    
    ROOT.gPad.SetGridy()
    ROOT.gPad.Update()
    ROOT.gPad.SaveAs(g.GetName().replace('g_','deltaNLL_')+'.pdf')
    ROOT.gPad.SaveAs(g.GetName().replace('g_','deltaNLL_')+'.png')
    raw_input('ok?')
    xlow = g.GetX()[1]
    xup  = g.GetX()[g.GetN()-1]
    #xlow = -8.
    #xup  = 8.

    d = 0.002
    xcross1 = 0
    xcross2 = 0
    ycross1 = 0
    ycross2 = 0
    # minus 1 sigma
    for i in range(0,5000):
        x = 1 - i * (1 - xlow)/5000.
        y = fitfun.Eval(x)
        #print x,y
        if ( abs(y-1.) < d ):
            xcross1 = x
            ycross1 = y
            break
    #plus 1 sigma
    for i in range(0,5000):
        x = 1 + i * (xup-1)/5000.
        y = fitfun.Eval(x)
        #print x,y
        if ( abs(y-1.) < d ):
            xcross2 = x
            ycross2 = y
            break
    #print xcross1, ycross1
    #print xcross2, ycross2    
    sigmam = abs(xcross1 - 1)
    sigmap = abs(xcross2 - 1)
    return sigmam, sigmap
                    


gSigma = ROOT.TGraph()
gSigmaP = ROOT.TGraph()
gSigmaM = ROOT.TGraph()
gSigma.SetName('gSigma')
gSigmaP.SetName('gSigmaP')
gSigmaM.SetName('gSigmaM')


cuts = [ '0.0', '0.25' , '0.5', '0.75', '1.0', '1.25', '1.5', '1.75', '2.0' ]
#cuts = [ '0.0', '0.5', '1.0', '1.5', '2.0' ]

for i, k in enumerate(cuts):
    filename = 'combineResults_pow/higgsCombine_ptgg%s.MultiDimFit.mH125.root'%k
    print filename
    if (os.path.isfile(filename) == False) :
        continue
    f = ROOT.TFile.Open(filename)    
    tree = f.Get('limit')
    print tree.GetEntries()
    n = tree.Draw('(2*deltaNLL):RV >> htemp','(2*deltaNLL)<10','')
    g = ROOT.TGraph(n, tree.GetV2(), tree.GetV1())
    g.SetName('g_%s'%k)
        
    #print g.GetName(), g.GetN()
    sigmam, sigmap = get1sigma(g)
    sigma = (sigmam+sigmap)/2.
    print k, sigmap, sigmam, sigma

    if (sigmap!=1 and sigmam!=1):
        gSigma.SetPoint(i,float(k),sigma)
    if (sigmam!=1):
        gSigmaM.SetPoint(i,float(k),sigmam)
    if (sigmap!=1):
        gSigmaP.SetPoint(i,float(k),sigmap)
    #raw_input('ok?')
    

gSigma.SetMarkerStyle(20)
gSigmaP.SetMarkerStyle(20)
gSigmaM.SetMarkerStyle(20)
gSigma.SetMarkerSize(0.5)
gSigmaP.SetMarkerSize(0.5)
gSigmaM.SetMarkerSize(0.5)

gSigma.SetMarkerColor(ROOT.kBlue)
gSigmaP.SetMarkerColor(ROOT.kGray+1)
gSigmaM.SetMarkerColor(ROOT.kGray+2)
    
c = ROOT.TCanvas('c','c',700,500)
gSigma.SetMarkerStyle(21)
gSigma.GetHistogram().GetYaxis().SetRangeUser(0.01,6.)
gSigma.GetHistogram().GetYaxis().SetTitle('#sigma_{RV}')
gSigma.GetHistogram().GetXaxis().SetTitle('pT_{#gamma#gamma}/m_{#gamma#gamma}')
gSigma.Draw('ap')
#gSigmaM.Draw('psame')
#gSigmaP.Draw('psame')

#legend = ROOT.TLegend(0.6,0.8,0.89,0.89)
#legend.AddEntry(gSigmaP,"#sigma_{RV} positive","P")
#legend.AddEntry(gSigmaM,"#sigma_{RV} negative","P")
#legend.AddEntry(gSigma,"#sigma_{RV} average","P")
#legend.Draw("same")

c.SaveAs('vhHad_sigmaRV_vs_ptgg.png')
c.SaveAs('vhHad_sigmaRV_vs_ptgg.pdf')


raw_input('ok?')
