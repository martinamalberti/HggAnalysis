#! /usr/bin/env python
import os
import glob
import math
import array
import sys
import time

import ROOT


def getSigmaEffective(h):

    testmass = 125.
    center = testmass-5.0
  
    cdfhi = 0
    cdflo = 0
    mlmin = 0.0
    mhmin = 0.0
    step  = 0.01

    minwidth = 999.0

    mlow = -999
    mhigh = -999
    binlow = -1
    binhigh = -1
    nbins = h.GetNbinsX()

    for i in range(0,1000):
            mlow = center+i*step
            binlow = h.FindBin(mlow)
            cdflo = h.Integral(1, binlow)/h.Integral(1, nbins)
            for j in range(i,1000):
                mhigh = center+j*step
                binhigh = h.FindBin(mhigh)
                cdfhi = h.Integral(1, binhigh)/h.Integral(1, nbins)
                if ( (cdfhi-cdflo)>0.684 ):
	            if ( (mhigh-mlow)<minwidth):
	                minwidth = mhigh-mlow
	                mlmin = mlow
	                mhmin = mhigh
		    break
  
    sigmaeff = minwidth/2.0
    ml = mlmin
    mh = mhmin
    print "Mmin = ", mlmin, "  Mmax = ", mhmin, "  effective sigma = ", sigmaeff 
  
    return (sigmaeff)






ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptStat(0)


lumi = 36.8

tag = sys.argv[1]

#signal
fnames = {}
fnames['ggh'] = '../histograms_%s_ggh.root'%tag
fnames['vbf'] = '../histograms_%s_vbf.root'%tag
fnames['zh']  = '../histograms_%s_zh.root'%tag
fnames['wh']  = '../histograms_%s_wh.root'%tag
fnames['tth'] = '../histograms_%s_tth.root'%tag

#data (bkg)
fnames['data'] = '../histograms_%s_data.root'%tag

h = {}
n = {}

for proc in 'ggh','vbf','wh','zh','tth','data':
    f = ROOT.TFile.Open(fnames[proc])
    if ('data' not in proc):
        h[proc] = f.Get('hmass_2').Clone('hmass_2_%s'%proc)
    else:    
        h[proc] = f.Get('hmass').Clone('hmass_%s'%proc)    
    h[proc].SetDirectory(0)
    print proc, h[proc].GetEntries(), h[proc].GetName()
    if ('data' not in proc):
        h[proc].Scale(lumi)

#get effective sigma
hSig = h['ggh'].Clone('hSig')
hSig.Add(h['vbf'])
hSig.Add(h['zh'])
hSig.Add(h['wh'])
hSig.Add(h['tth'])
hSig.Draw()

raw_input('ok?')

sigmaEff = getSigmaEffective(hSig)
print 'Effective sigma = ', sigmaEff, ' GeV'
bin1 = hSig.FindBin(125.-sigmaEff)
bin2 = hSig.FindBin(125.+sigmaEff)

# get number of signal events
for proc in 'ggh','vbf','zh','wh','tth':
    n[proc] = h[proc].Integral(bin1,bin2)
    if (n[proc] <=0):
        n[proc]=0.00000001

print 'Number of sig events in +/- 1sigma = ', (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])
print 'Number of WH sig events in +/- 1sigma = ', (+n['zh']+n['wh'])


# bkg fits
fitoptions = 'RMQ'
if tag == 'ZHLeptonicTag': fitoptions = 'LRMQ'

# fit with expo1
fitExp1 = ROOT.TF1('fitExp1','expo',2)
fitExp1.SetRange(100.,180.)
# fit with expo2
fitExp2 = ROOT.TF1('fitExp2','[1]*expo(2)+[2]*expo(2)')
fitExp2.SetRange(100.,180.)
fitExp2.SetLineStyle(4)
#fit with power law
fitPow = ROOT.TF1('fitPow','[1]*pow(x,[0])',2) #power law
fitPow.SetRange(100.,180.)
fitPow.SetLineStyle(2)
#fit with pol1
fitPol1 = ROOT.TF1('fitPol1','[1]*x+[0]',2) #pol1
fitPol1.SetRange(100.,180.)
fitPol1.SetLineStyle(3)

print 'Fit with exp1'
h['data'].Fit('fitExp1',fitoptions)
n['data']=  fitExp1.Integral(125.-sigmaEff, 125.+sigmaEff)       
print 'Chi2/NDF = ', fitExp1.GetChisquare()/fitExp1.GetNDF()
print 'Number of bkg events per GeV (SS) = ', n['data']/(2*sigmaEff)
print 'Number of bkg events in +/- effSigma = ', n['data']
print 'S/B = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/n['data']
print 'S/(S+B) = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/(n['data']+n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])

print 'Fit with exp2'
h['data'].Fit('fitExp2',fitoptions)
n['data']=  fitExp2.Integral(125.-sigmaEff, 125.+sigmaEff)       
print 'Chi2/NDF = ', fitExp2.GetChisquare()/fitExp2.GetNDF()
print 'Number of bkg events per GeV (SS) = ', n['data']/(2*sigmaEff)
print 'Number of bkg events in +/- effSigma = ', n['data']
print 'S/B = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/n['data']
print 'S/(S+B) = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/(n['data']+n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])

print 'Fit with power law'
h['data'].Fit('fitPow','QRM')
h['data'].Fit('fitPow',fitoptions)
n['data']=  fitPow.Integral(125.-sigmaEff, 125.+sigmaEff)       
print 'Chi2/NDF = ', fitPow.GetChisquare()/fitPow.GetNDF()
print 'Number of bkg events per GeV (SS) = ', n['data']/(2*sigmaEff)
print 'Number of bkg events in +/- effSigma = ', n['data']
print 'S/B = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/n['data']
print 'S/(S+B) = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/(n['data']+n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])

print 'Fit with pol1'
h['data'].Fit('fitPol1',fitoptions)
n['data']=  fitPol1.Integral(125.-sigmaEff, 125.+sigmaEff)
print 'Chi2/NDF = ', fitPol1.GetChisquare()/fitPol1.GetNDF()
print 'Number of bkg events per GeV (SS) = ', n['data']/(2*sigmaEff)
print 'Number of bkg events in +/- effSigma = ', n['data']
print 'S/B = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/n['data']
print 'S/(S+B) = ',  (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])/(n['data']+n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])


c = ROOT.TCanvas(tag+'_fitSS',tag+'_fitSS',500,450)
h['data'].GetXaxis().SetTitle('mass (GeV)')
h['data'].GetYaxis().SetTitle('events/GeV')
#h['data'].GetYaxis().SetRangeUser(0,8)
h['data'].SetMarkerStyle(20)
h['data'].SetLineColor(1)
h['data'].SetMarkerColor(1)
h['data'].Draw('e')
fitExp1.Draw('same')
fitExp2.Draw('same')
fitPow.Draw('same')
fitPol1.Draw('same')

leg = ROOT.TLegend(0.75,0.75,0.89,0.89)
leg.AddEntry(h['data'],'data (SS)','L')
leg.AddEntry(fitExp1,'exp1','L')
#leg.AddEntry(fitExp2,'exp2','L')
leg.AddEntry(fitPow,'pow','L')
leg.AddEntry(fitPol1,'pol1','L')
leg.Draw('same')

hSig.SetLineColor(4)
hSig.Draw('histosame')
raw_input('?')




c.SaveAs(c.GetName()+'.pdf')
