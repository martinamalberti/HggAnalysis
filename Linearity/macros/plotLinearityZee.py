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
CMS_lumi.extraText = " Preliminary"
CMS_lumi.lumi_sqrtS = "13 TeV" # used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

#iPeriod = 0
#iPos = 11
iPeriod = 4
iPos = 0
if( iPos==0 ): CMS_lumi.relPosX = 0.12

#ROOT.gStyle.SetLegendFont(43)
#ROOT.gStyle.SetTextFont(43)
#ROOT.gStyle.SetLabelFont(43,"X")
#ROOT.gStyle.SetLabelFont(43,"Y")
#ROOT.gStyle.SetTitleFont(43,"X")
#ROOT.gStyle.SetTitleFont(43,"Y")
ROOT.gStyle.SetPadTopMargin(0.08)
ROOT.gStyle.SetPadBottomMargin(0.15)
ROOT.gStyle.SetPadLeftMargin(0.10)
ROOT.gStyle.SetPadRightMargin(0.05)
ROOT.gStyle.cd()

ROOT.gStyle.SetOptFit(0111)
ROOT.gStyle.SetFitFormat("5.4g")


doFitPlots = True
doHtPlots = False

f = ROOT.TFile.Open("../linearityZee_2016_reminiAOD.root")

text = []

nl    = {}

for i in range(0,4):
    for j in range(0,4):

        #if ( i != j): continue
        
        c = ROOT.TCanvas("linearity%d%d"%(i,j),"linearity_%d%d"%(i,j),800,700)

        g  = f.Get("g_cat%dcat%d"%(i,j))
        g0 = f.Get("g_cat%dcat%d"%(i,j))
        if ( g.GetN() == 0 ) : continue

        print "Category ",i,j


        # fit
        fitfunc = ROOT.TF1("fitfunc", "[0]+[1]*(x-45.)", 20, 2000)
        fitfunc.SetLineColor(ROOT.kBlue)
        fitfunc.SetLineWidth(1)
        fitfunc.SetNpx(1000)
        fitfunc.SetParameter(0,1)
        fitfunc.SetParameter(1,0.00005)
        g.Fit("fitfunc", "REQS")
        print "Original Chi2/NDF = ", fitfunc.GetChisquare()/fitfunc.GetNDF()
    
        # scale errors to get chi2/ndf = 1
        #for j in range(0,g.GetN()):
        #    newerr = g.GetErrorY(j)*math.sqrt(fitfunc.GetChisquare()/fitfunc.GetNDF())
        #    g.SetPointError(j, g.GetErrorX(j), newerr)
        
        #add syst error to get chi2/ndf = 1
        maxtrials = 1000
        dd = 0.01
        syst =  0.0001
        for k in range(0,maxtrials):
            chi2  =  fitfunc.GetChisquare()/fitfunc.GetNDF()
            #print k, chi2
            if ( math.fabs(chi2-1.) < dd):
                break
            if ( math.fabs(chi2-1.) >= dd ):
                if ( chi2 > 1. ):
                    for l in range(0,g.GetN()):
                        newerr = math.sqrt(g.GetErrorY(l)*g.GetErrorY(l) + syst*syst)
                        g.SetPointError(l, g.GetErrorX(l), newerr)
                    g.Fit("fitfunc", "REQ")


        g.Fit("fitfunc", "REQS")
        print "Slope    = ", fitfunc.GetParameter(1)
        print "Chi2/NDF = ", fitfunc.GetChisquare()/fitfunc.GetNDF()

        
        nl['%d%d'%(i,j)] = [fitfunc.GetParameter(1), fitfunc.GetParError(1)]


        # get fit error bands
        h = ROOT.TH1F("h%d%d"%(i,j),"h",10000,25.,1025.)
        fitter = ROOT.TVirtualFitter.GetFitter()                       
        fitter.GetConfidenceIntervals(h, 0.68)

        h.SetFillColor(ROOT.kAzure -9)    
        h.SetLineColor(ROOT.kAzure -9)     
        h.SetMarkerColor(0);     
        
        #Draw
        g.SetMarkerColor(2)
        g.SetMarkerSize(0.8)    
        g.SetMarkerStyle(20)    
        g.SetLineColor(ROOT.kGray+2)    
        g.SetLineWidth(1)    

        g0.SetMarkerColor(2)
        g0.SetMarkerSize(0.8)    
        g0.SetMarkerStyle(20)    
        g0.SetLineWidth(1)    

        h.GetXaxis().SetRangeUser(30,160)
        h.GetXaxis().SetTitle("H_{T}/2 (GeV)")
        h.GetYaxis().SetTitle("Relative response")
        h.GetYaxis().SetRangeUser(0.98,1.02)
        
        h.Draw("e3")
        g.Draw("psame")
        g0.Draw("psame")
        text.append(ROOT.TLatex(0.18,0.2,("Cat%d Cat%d")%(i,j)))
        text[-1].SetTextFont(43)
        text[-1].SetTextSize(20)
        text[-1].SetNDC()
        text[-1].Draw()
        
        c.Update()
        st = g.FindObject("stats")
        st.SetX1NDC(0.65) 
        st.SetX2NDC(0.92) 
        st.SetY1NDC(0.75) 
        st.SetY2NDC(0.9) 
        ROOT.gStyle.SetFitFormat("5.4g")
        
        CMS_lumi.CMS_lumi(c, iPeriod, iPos)    
        
        for typ in '.png','.pdf':
            c.SaveAs('plots/'+c.GetName()+typ)

    
        #raw_input("OK?")

#compare diagonal and off-diagonal categories
#print nl

enl_off = {}

enl_off["01"] = [ 0.5*(nl["00"][0]+nl["11"][0]) , 0.5*math.sqrt(nl["00"][1]*nl["00"][1]+nl["11"][1]*nl["11"][1]) ]
enl_off["02"] = [ 0.5*(nl["00"][0]+nl["22"][0]) , 0.5*math.sqrt(nl["00"][1]*nl["00"][1]+nl["22"][1]*nl["22"][1]) ]
enl_off["03"] = [ 0.5*(nl["00"][0]+nl["33"][0]) , 0.5*math.sqrt(nl["00"][1]*nl["00"][1]+nl["33"][1]*nl["33"][1]) ]
enl_off["12"] = [ 0.5*(nl["11"][0]+nl["22"][0]) , 0.5*math.sqrt(nl["11"][1]*nl["11"][1]+nl["22"][1]*nl["22"][1]) ]
enl_off["13"] = [ 0.5*(nl["11"][0]+nl["33"][0]) , 0.5*math.sqrt(nl["11"][1]*nl["11"][1]+nl["33"][1]*nl["33"][1]) ]
enl_off["23"] = [ 0.5*(nl["22"][0]+nl["33"][0]) , 0.5*math.sqrt(nl["22"][1]*nl["22"][1]+nl["33"][1]*nl["33"][1]) ]

bins = {"00":1,"01":2,"02":3,"03":4,"11":5,"12":6,"13":7,"22":8,"23":9,"33":10}
hdiff = ROOT.TH1F("hdiff","hdiff",10, 0, 10)
for k in bins:
    hdiff.GetXaxis().SetBinLabel(bins[k], k)
for k in enl_off:
    diff  = nl[k][0] - enl_off[k][0] 
    ediff = math.sqrt(enl_off[k][1]*enl_off[k][1] + nl[k][1]*nl[k][1])
    #print k, diff
    hdiff.Fill(k,diff)
    hdiff.SetBinError(bins[k], ediff)
    
cdiff = ROOT.TCanvas("cdiff", "cdiff",600,500)
cdiff.SetGridy()
hdiff.GetYaxis().SetRangeUser(-0.0005, 0.0005)
hdiff.GetXaxis().SetTitle("category")
hdiff.GetYaxis().SetTitle("diff")
hdiff.Draw("p")
hdiff.Fit("pol0")
for typ in '.png','.pdf':
    cdiff.SaveAs('plots/'+cdiff.GetName()+typ)
raw_input("OK?") 

# draw fits
hDA = []
hHtDA = {}
hHtMC = {}

for k in ROOT.gDirectory.GetListOfKeys():
    if( "h_Mee_DA" in k.ReadObj().GetName()):
        hDA.append( k.ReadObj() )
    if( "h_Ht_DA" in k.ReadObj().GetName()):
        hHtDA[k.ReadObj().GetName()] = k.ReadObj()
    if( "h_Ht_MC" in k.ReadObj().GetName()):
        hHtMC[k.ReadObj().GetName()] = k.ReadObj()

if doFitPlots:
    for i, h in enumerate(hDA):
        cname = h.GetName().replace("h_Mee_DA","Mee")
        c = ROOT.TCanvas(cname,cname,500,500)
        h.SetMarkerStyle(20)
        h.SetMarkerSize(0.8)
        h.GetXaxis().SetRangeUser(0.8,1.2)
        h.GetXaxis().SetTitle("M_{ee}/M_{Z}")
        h.Draw("e1")
        
        ROOT.gStyle.SetOptFit(111)
        c.Update()
        st = h.FindObject("stats")
        st.SetX1NDC(0.65) 
        st.SetX2NDC(0.92) 
        st.SetY1NDC(0.8) 
        st.SetY2NDC(0.9) 
        
        for typ in '.png','.pdf':
            c.SaveAs('plots/Fits/'+c.GetName()+typ)

if (doHtPlots):
    for hname, h in hHtMC.iteritems():
        
        cname = h.GetName().replace("h_Ht_MC","Ht")
        c = ROOT.TCanvas(cname,cname,500,500)

        hda = hHtDA[hname.replace("MC","DA")]

        hda.SetMarkerStyle(20)
        hda.SetMarkerSize(0.8)
        hda.GetXaxis().SetTitle("H_{T}/2")
        hda.GetXaxis().SetRangeUser(25.,400.)
        hda.Draw("e1s")
        
        norm = hda.GetSumOfWeights()/h.GetSumOfWeights()
        h.Scale(norm)
        h.Draw("hsames")
        
        #c.Update()
        #st = h.FindObject("stats")
        #st.SetX1NDC(0.65) 
        #st.SetX2NDC(0.92) 
        #st.SetY1NDC(0.8) 
        #st.SetY2NDC(0.9) 
        
        for typ in '.png','.pdf':
            c.SaveAs('plots/'+c.GetName()+typ)



