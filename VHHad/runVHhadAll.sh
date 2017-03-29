#!/bin/sh

./VHhadronicAnalysis trees/vhHadDumper_20161223/output_vh.root vhHadTagDumper/trees/vh_13TeV_VHHadronicTag histograms_vh.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_zh.root vhHadTagDumper/trees/zh_13TeV_VHHadronicTag histograms_zh.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_wh.root vhHadTagDumper/trees/wh_13TeV_VHHadronicTag histograms_wh.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_ggh.root vhHadTagDumper/trees/ggh_13TeV_VHHadronicTag histograms_ggh.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_vbf.root vhHadTagDumper/trees/vbf_13TeV_VHHadronicTag histograms_vbf.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_tth.root vhHadTagDumper/trees/tth_13TeV_VHHadronicTag histograms_tth.root

./VHhadronicAnalysis trees/vhHadDumper_20161223/output_diphoton.root vhHadTagDumper/trees/diphoton_13TeV_VHHadronicTag histograms_diphoton.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_gjet.root vhHadTagDumper/trees/gjet_13TeV_VHHadronicTag histograms_gjet.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_qcd.root vhHadTagDumper/trees/qcd_13TeV_VHHadronicTag histograms_qcd.root

./VHhadronicAnalysis trees/vhHadDumper_20161223/output_gjet.root vhHadTagDumper/trees/gjet_13TeV_VHHadronicTag histograms_gjet_pf.root  1 0
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_gjet.root vhHadTagDumper/trees/gjet_13TeV_VHHadronicTag histograms_gjet_ff.root  0 1

./VHhadronicAnalysis trees/vhHadDumper_20161223/output_qcd.root vhHadTagDumper/trees/qcd_13TeV_VHHadronicTag histograms_qcd_pf.root 1 0
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_qcd.root vhHadTagDumper/trees/qcd_13TeV_VHHadronicTag histograms_qcd_ff.root 0 1

./VHhadronicAnalysis trees/vhHadDumper_20161223/output_data.root vhHadTagDumper/trees/data_13TeV_VHHadronicTag histograms_data.root
./VHhadronicAnalysis trees/vhHadDumper_20161223/output_data_cs.root vhHadTagDumper/trees/data_13TeV_VHHadronicTag histograms_data_cs.root


hadd -f histograms_pf.root histograms_qcd_pf.root histograms_gjet_pf.root

hadd -f histograms_ff.root histograms_qcd_ff.root histograms_gjet_ff.root 
