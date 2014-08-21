#! /usr/bin/python
import ROOT
import sys
import os
import ConfigParser
from rd42Style import rd42Style


class plotter(object) :

	def __init__(selfi, config) :
		print 'init'
		rd42Style()


	def plot(self) :
		canvas = ROOT.TCanvas('canvas', 'canvas')
		histo = self.get_histo('DiaTranspAnaPulseHeightOf2HighestIn10Strips')
		canvas.cd()
		histo.Draw()
		self.draw_rd42Line()
		raw_input('ok?')
#		raw_input('ok?')
		canvas.Update()
		canvas.Print('test.pdf')


	def get_histo(self, name) :
#		histo_file = ROOT.TFile('histograms.root', 'READ')
		histo_file = ROOT.TFile('cDiaTranspAnaPulseHeightOf2HighestIn10Strips.root', 'READ')
		histo = histo_file.Get('c%s' % name).GetPrimitive('h%s' % name)
		histo.GetFunction('Fitfcn_h%s' % name).SetBit(ROOT.TF1.kNotDraw)
		histo.GetFunction('fMeanCalculationArea').SetBit(ROOT.TF1.kNotDraw)
		return histo


	def draw_rd42Line(self) :
		latex = ROOT.TLatex()
		latex.SetNDC()
		latex.SetTextFont(62)
		latex.SetTextSize(0.04)
		latex.SetTextAlign(13)
		latex.DrawLatex(0.15, 0.93, 'RD42')


if __name__ == '__main__' :
	args = sys.argv

	if ('--help' in args) or ('-h' in args) :
		print 'usage: ..'
		sys.exit(1)

	if ('-c' in args) and (args.index('-c')+1 < len(args)) and (not args[args.index('--name')+1].startswith('-')) :
		config_file = args[args.index('--name')+1]
	else :
		config_file = '%s/config.cfg' % os.path.dirname(os.path.realpath(__file__))

	pl = plotter('bla')
	pl.plot()

##	name = 'DiaTranspAnaPulseHeightOf2HighestIn10Strips'
##	file = ROOT.TFile('c%s.root' % name, 'READ')
##	#TCanvas *can = (TCanvas*)file->Get('cDiaTranspAnaPulseHeightOf2HighestIn10Strips')
##	#TH1F* histo = (TH1F*)can->GetPrimitive('hDiaTranspAnaPulseHeightOf2HighestIn10Strips')
##	canvas = file.Get('c%s' % name)
##	histo = canvas.GetPrimitive('h%s' % name)
##	#TH1F* histo = (TH1F*)can->GetPrimitive('hDiaTranspAnaPulseHeightOf2HighestIn10Strips')
##	canvas = ROOT.TCanvas('bla', 'bla', 600, 600)
##	histo.GetFunction('Fitfcn_h%s' % name).SetBit(ROOT.TF1.kNotDraw)
##	histo.GetFunction('fMeanCalculationArea').SetBit(ROOT.TF1.kNotDraw)
##	#histo->GetMaximum()
##	#histo->SetMaximum(1.4*5.21000000000000000e+02)
##	#histo->GetMaximum()
##	canvas.SetLeftMargin(0.12)
##	canvas.SetRightMargin(0.04)
##	canvas.SetTopMargin(0.04)
##	canvas.SetBottomMargin(0.12)
##	ROOT.gPad.SetTicks(1,1)
##	canvas.cd()
##	histo.Draw()
##	ROOT.gStyle.SetOptTitle(0)
##	ROOT.gStyle.SetOptStat(0)
##	ROOT.gStyle.SetOptFit(0)
##	histo.SetXTitle('ADC Counts')
##	histo.SetYTitle('Events')
##	histo.GetXaxis().SetTitleOffset(1.25)
##	histo.GetYaxis().SetTitleOffset(1.25)
##	histo.GetXaxis().SetTitleSize(0.046)
##	histo.GetYaxis().SetTitleSize(0.046)
##	histo.GetXaxis().SetLabelSize(0.04)
##	histo.GetYaxis().SetLabelSize(0.04)
##	histo.GetXaxis().SetLabelOffset(0.012)
##	histo.GetYaxis().SetLabelOffset(0.012)
##	histo.Draw()
##	canvas.Print('%s.pdf' % name)
##	raw_input('ok?')
##	raw_input('ok?')
