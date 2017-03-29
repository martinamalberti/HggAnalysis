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








lumi = 36.8

cut = sys.argv[1]

#signal
fnames = {}
fnames['ggh'] = '../histograms_ptgg/histograms_ggh_%s.root'%cut
fnames['vbf'] = '../histograms_ptgg/histograms_vbf_%s.root'%cut
fnames['zh']  = '../histograms_ptgg/histograms_zh_%s.root'%cut
fnames['wh']  = '../histograms_ptgg/histograms_wh_%s.root'%cut
fnames['tth'] = '../histograms_ptgg/histograms_tth_%s.root'%cut

#data (bkg)
fnames['data'] = '../histograms_ptgg/histograms_data_%s.root'%cut
fnames['data_cs'] = '../histograms_ptgg/histograms_data_cs_%s.root'%cut

h = {}
n = {}

for proc in 'ggh','vbf','wh','zh','tth','data','data_cs':
#for proc in 'ggh','vbf','vh','tth':
    f = ROOT.TFile.Open(fnames[proc])
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

sigmaEff = getSigmaEffective(hSig)
print 'Effective sigma = ', sigmaEff, ' GeV'
bin1 = hSig.FindBin(125.-sigmaEff)
bin2 = hSig.FindBin(125.+sigmaEff)


#normalize control samples to sidebands
b1 = h['data'].FindBin(115.)
b2 = h['data'].FindBin(135.)

nss = h['data'].Integral(1,b1)+h['data'].Integral(b2,80)
ncs = h['data_cs'].Integral(1,b1)+h['data_cs'].Integral(b2,80)
h['data_cs'].Scale(nss/ncs)

# fit with expo
#fitfun = ROOT.TF1('fitfun','expo',2)
#fit with power law
fitfun = ROOT.TF1('fitfun','[1]*pow(x,[0])',2) #power law
#fitfun.SetParLimits(0,-99999.,0)

fitfun.SetRange(100.,180.)
h['data_cs'].Fit('fitfun','SR+')
n['data_cs']=  fitfun.Integral(125.-sigmaEff, 125.+sigmaEff)       
print 'Number of bkg events per GeV (CS) = ', n['data_cs']/(2*sigmaEff)
h['data'].Fit('fitfun','SR+')
n['data']=  fitfun.Integral(125.-sigmaEff, 125.+sigmaEff)       
print 'Number of bkg events per GeV (SS) = ', n['data']/(2*sigmaEff)


h['data'].SetMarkerStyle(20)
h['data'].SetLineColor(1)
h['data'].SetMarkerColor(1)
h['data'].Draw('e')
h['data_cs'].Draw('histo same s')
#h['data_cs'].Draw('e')
fitfun.Draw('same')

raw_input('?')

# get number of signal events
for proc in 'ggh','vbf','zh','wh','tth':
    n[proc] = h[proc].Integral(bin1,bin2)
    if (n[proc] <=0):
        n[proc]=0.00000001

print 'Number of sig events in +/- 1sigma = ', (n['ggh']+n['vbf']+n['zh']+n['wh']+n['tth'])
print 'Number of WH sig events in +/- 1sigma = ', (+n['zh']+n['wh'])

#split 2:1 between wh/zh (temporary fix perche' non ho salvato separatamente wh/zh)
#n['wh'] = n['vh']/3.*2
#n['zh'] = n['vh']/3.*1

    
cat = 'VHhad'

datacard = open("datacards/datacard_hgg_13TeV_SimpleCounting_VHHad_%s.txt"%cut, "w")
datacard.write('# Counting experiment\n')
datacard.write('------------\n')
datacard.write('bin             %s_13TeV\n'%cat)
datacard.write('observation     1\n')
datacard.write('------------\n')
datacard.write('bin                %s_13TeV   %s_13TeV  %s_13TeV  %s_13TeV  %s_13TeV %s_13TeV\n'%(cat, cat, cat, cat, cat, cat))
datacard.write('process            ggH      qqH     ZH     WH    ttH     bkg\n')
datacard.write('process            -4        -3       -2      -1     0       1\n'  )
datacard.write('rate               %.6f     %.6f    %.6f   %.6f  %.6f    %.3f\n' % ( n['ggh'], n['vbf'], n['zh'], n['wh'], n['tth'], n['data_cs']) ) 
datacard.write('# add systs\n')
datacard.write('------------\n')
datacard.write('lumi            lnN    1.062   1.062   1.062   1.062   1.062   -\n')
datacard.write('QCDscale_gg     lnN    1.046/0.933 - - - 0.942/1.092 -\n')
datacard.write('QCDscale_qqbar  lnN    - 1.004/0.997 1.005/0.993 1.038/0.969 - -\n')
datacard.write('JEC             lnN    1.10 - 1.01 1.01 1.10 -\n')
datacard.write('UEPS            lnN    1.20 - - - - -\n')
datacard.write('------------\n')
datacard.write('RF param 1.0 0.10')
datacard.close()

print datacard
