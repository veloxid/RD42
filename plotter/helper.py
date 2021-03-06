#! /usr/bin/python
import math
import pickle
import os
import sys
import ROOT
from array import array


def open_rootFile(path, opt = 'READ') :
	if not os.path.exists(path) :
		print '%s does not exist!' % path
		sys.exit(1)
	else :
		return ROOT.TFile.Open(path, opt)


def ratioWithBinomErrors(numerator, denominator) :
	num = float(numerator)
	den = float(denominator)
	ratio = num / den
	error = math.sqrt(num * (1. - num/den)) / den
	return (ratio, error)


def ratioWithPoissErrors(numerator, denominator) :
	num = float(numerator)
	den = float(denominator)
	ratio = num / den
	error = math.sqrt(num*num*(den+num) / (den*den*den))
	return (ratio, error)


def getGraphPoissonErrors(histo, nSigma = 1., xErrType = '0') :

	graph = ROOT.TGraphAsymmErrors(0)

	for iBin in range(1, histo.GetXaxis().GetNbins()+1) :

		x = histo.GetBinCenter(iBin)

		if xErrType == '0' :
			xerr = 0.
		elif xErrType == 'binWidth' :
			xerr = histo.GetBinWidth(iBin)/2.
		elif xErrType == 'sqrt12' :
			xerr = histo.GetBinWidth(iBin)/math.sqrt(12.)
		else :
			print '[WARNING] Unkown xErrType %s. Setting to bin width.'
			xerr = histo.GetBinWidth(iBin)

		y = int(histo.GetBinContent(iBin))
		ym = array('d', [0])
		yp = array('d', [0])

		ROOT.RooHistError.instance().getPoissonInterval(y, ym, yp, nSigma)

		yerrplus = yp[0] - y
		yerrminus = y - ym[0]

		thisPoint = graph.GetN()
		graph.SetPoint(thisPoint, x, y )
		graph.SetPointError(thisPoint, xerr, xerr, yerrminus, yerrplus )

	return graph


def getGraphPoissonErrors_new(histo, x_errors = False) :
	'''Asymmetric Error Bars for Poisson Event Counts'''

	alpha = 1 - 0.6827
	graph = ROOT.TGraphAsymmErrors(histo)
	for i in range(0, graph.GetN()) :
		N = graph.GetY()[i]
		L = 0
		if N > 0 : L = ROOT.Math.gamma_quantile(alpha/2,N,1.)
		U =  ROOT.Math.gamma_quantile_c(alpha/2,N+1,1)
		graph.SetPointEYlow(i, N-L)
		graph.SetPointEYhigh(i, U-N)
		if not x_errors :
			graph.SetPointEXlow(i, 0.)
			graph.SetPointEXhigh(i, 0.)

	return graph


def save_object(obj, filepath) :
	dir = os.path.dirname(filepath)
	mkdir(dir)
	with open(filepath, 'w') as file :
		pickle.dump(obj, file, pickle.HIGHEST_PROTOCOL)


def load_object(filepath) :
	if not os.path.exists(filepath) :
		print '[ERROR] %s does not exist!' % (filepath)
		return -1
	with open(filepath, 'r') as file :
		return pickle.load(file)


def mkdir(path) :
	if not os.path.exists(path) :
		os.makedirs(path)

