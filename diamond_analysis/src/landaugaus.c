/*
 * landaugaus.c
 *
 *  Created on: Apr 25, 2012
 *      Author: bachmair
 */


#include "TMath.h"
#include "TF1.h"
#include "TROOT.h"
#include <fstream>
#include <iostream>
#include <iostream>
#include "TH1F.h"

using namespace std;

Double_t langaufun(Double_t *x, Double_t *par) {

	//Fit parameters:
	//par[0]=Width (scale) parameter of Landau density
	//par[1]=Most Probable (MP, location) parameter of Landau density
	//par[2]=Total area (integral -inf to inf, normalization constant)
	//par[3]=Width (sigma) of convoluted Gaussian function
	//
	//In the Landau distribution (represented by the CERNLIB approximation),
	//the maximum is located at x=-0.22278298 with the location parameter=0.
	//This shift is corrected within this function, so that the actual
	//maximum is identical to the MP parameter.

	// Numeric constants
	Double_t invsq2pi = 0.3989422804014; // (2 pi)^(-1/2)
	Double_t mpshift = -0.22278298; // Landau maximum location

	// Control constants
	Double_t np = 100.0; // number of convolution steps
	Double_t sc = 5.0; // convolution extends to +-sc Gaussian sigmas

	// Variables
	Double_t xx;
	Double_t mpc;
	Double_t fland;
	Double_t sum = 0.0;
	Double_t xlow, xupp;
	Double_t step;
	Double_t i;

	// MP shift correction
	mpc = par[1] - mpshift * par[0];

	// Range of convolution integral
	xlow = x[0] - sc * par[3];
	xupp = x[0] + sc * par[3];

	step = (xupp - xlow) / np;

	// Convolution integral of Landau and Gaussian by sum
	for (i = 1.0; i <= np / 2; i++) {
		xx = xlow + (i - .5) * step;
		fland = TMath::Landau(xx, mpc, par[0]) / par[0];
		sum += fland * TMath::Gaus(x[0], xx, par[3]);

		xx = xupp - (i - .5) * step;
		fland = TMath::Landau(xx, mpc, par[0]) / par[0];
		sum += fland * TMath::Gaus(x[0], xx, par[3]);
	}

	return (par[2] * step * sum * invsq2pi / par[3]);
}

TF1 *langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues,
		Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams,
		Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF) {
	// Once again, here are the Landau * Gaussian parameters:
	//   par[0]=Width (scale) parameter of Landau density
	//   par[1]=Most Probable (MP, location) parameter of Landau density
	//   par[2]=Total area (integral -inf to inf, normalization constant)
	//   par[3]=Width (sigma) of convoluted Gaussian function
	//
	// Variables for langaufit call:
	//   his             histogram to fit
	//   fitrange[2]     lo and hi boundaries of fit range
	//   startvalues[4]  reasonable start values for the fit
	//   parlimitslo[4]  lower parameter limits
	//   parlimitshi[4]  upper parameter limits
	//   fitparams[4]    returns the final fit parameters
	//   fiterrors[4]    returns the final fit errors
	//   ChiSqr          returns the chi square
	//   NDF             returns ndf

	Int_t i;
	Char_t FunName[100];

	sprintf(FunName, "Fitfcn_%s", his->GetName());

	TF1 *ffitold = (TF1*) gROOT->GetListOfFunctions()->FindObject(FunName);
	if (ffitold)
		delete ffitold;

	TF1 *ffit = new TF1(FunName, langaufun, fitrange[0], fitrange[1], 4);
	ffit->SetParameters(startvalues);
	ffit->SetParNames("Width", "MP", "Area", "GSigma");

	for (i = 0; i < 4; i++) {
		ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
	}
	ffit->SetLineColor(kBlue);
	his->Fit(FunName, "RB+"); // fit within specified range, use ParLimits, do not plot

	ffit->GetParameters(fitparams); // obtain fit parameters
	for (i = 0; i < 4; i++) {
		fiterrors[i] = ffit->GetParError(i); // obtain fit parameter errors
	}
	ChiSqr[0] = ffit->GetChisquare(); // obtain chi^2
	NDF[0] = ffit->GetNDF(); // obtain ndf

	return (ffit); // return fit function

}

Int_t langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM) {

	// Seaches for the location (x value) at the maximum of the
	// Landau-Gaussian convolute and its full width at half-maximum.
	//
	// The search is probably not very efficient, but it's a first try.

	Double_t p, x, fy, fxr, fxl;
	Double_t step;
	Double_t l, lold;
	Int_t i = 0;
	Int_t MAXCALLS = 10000;

	// Search for maximum

	p = params[1] - 0.1 * params[0];
	step = 0.05 * params[0];
	lold = -2.0;
	l = -1.0;

	while ((l != lold) && (i < MAXCALLS)) {
		i++;

		lold = l;
		x = p + step;
		l = langaufun(&x, params);

		if (l < lold)
			step = -step / 10;

		p += step;
	}

	if (i == MAXCALLS)
		return (-1);

	maxx = x;

	fy = l / 2;

	// Search for right x location of fy

	p = maxx + params[0];
	step = params[0];
	lold = -2.0;
	l = -1e300;
	i = 0;

	while ((l != lold) && (i < MAXCALLS)) {
		i++;

		lold = l;
		x = p + step;
		l = TMath::Abs(langaufun(&x, params) - fy);

		if (l > lold)
			step = -step / 10;

		p += step;
	}

	if (i == MAXCALLS)
		return (-2);

	fxr = x;

	// Search for left x location of fy

	p = maxx - 0.5 * params[0];
	step = -params[0];
	lold = -2.0;
	l = -1e300;
	i = 0;

	while ((l != lold) && (i < MAXCALLS)) {
		i++;

		lold = l;
		x = p + step;
		l = TMath::Abs(langaufun(&x, params) - fy);

		if (l > lold)
			step = -step / 10;

		p += step;
	}

	if (i == MAXCALLS)
		return (-3);

	fxl = x;

	FWHM = fxr - fxl;
	return (0);
}

//LandauGaus:
//	par[0]=Width (scale) parameter of Landau density
//	par[1]=Most Probable (MP, location) parameter of Landau density
//	par[2]=Total area (integral -inf to inf, normalization constant)
//	par[3]=Width (sigma) of convoluted Gaussian function
//Landau:
//	par[0]:Constant
//  par[1]:MPV
//  par[2]:sigma
TF1* doLandauGaussFit(TH1F* inputHisto) {
	if (inputHisto == 0)
		return 0;
	cout << "DO Landau Gauss Fit for " << inputHisto->GetName() << endl;
	float min= inputHisto->GetXaxis()->GetXmin();
	float max = inputHisto->GetXaxis()->GetXmax();
	TF1* landau = new TF1("landau","landau",min, max);
	landau->SetLineColor(kRed);
	inputHisto->Fit(landau,"");

	Double_t fr[2];
	Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
	//range
	fr[0] = 0.3 * inputHisto->GetMean();
	fr[1] = 3.0 * inputHisto->GetMean();
	cout<<"Set Fit Range: ["<<fr[0]<<","<<fr[1]<<"]"<<endl;
	//low end
	pllo[0] = 0.1*landau->GetParameter(2);
	pllo[1] = 0.1*landau->GetParameter(1);
	pllo[2] = 0.2* inputHisto->Integral();
	pllo[3] = 0.5;
	//High end
	plhi[0] = 5.0*landau->GetParameter(2);
	plhi[1] = 10*landau->GetParameter(1);
	plhi[2] = 10* inputHisto->Integral();
	plhi[3] = 100;
	//startValue
	sv[0] = landau->GetParameter(2);
	sv[1] =  landau->GetParameter(1);
	sv[2] = inputHisto->Integral();
	sv[3] = 5.0;
	cout<<"Set Width Range: ["<<pllo[0]<<","<<plhi[0]<<"]\tstart:"<<sv[0]<<endl;
	cout<<"Set Mean  Range: ["<<pllo[1]<<","<<plhi[1]<<"]\tstart:"<<sv[1]<<endl;
	cout<<"Set Area  Range: ["<<pllo[2]<<","<<plhi[2]<<"]\tstart:"<<sv[2]<<endl;
	cout<<"Set Sigma Range: ["<<pllo[3]<<","<<plhi[3]<<"]\tstart:"<<sv[3]<<endl;

	Double_t chisqr;
	Int_t ndf;
	cout<<"DO FIT:"<<endl;
	TF1 *fitsnr = langaufit(inputHisto, fr, sv, pllo, plhi, fp, fpe, &chisqr,
			&ndf);

	Double_t SNRPeak, SNRFWHM;
	cout<<"DO SEARCH"<<endl;
	langaupro(fp, SNRPeak, SNRFWHM);
	cout<<"RETURN: \n\t"<<fitsnr->GetParameter(0)<<"\n\t"<<fitsnr->GetParameter(1)<<"\n\t"<<\
			fitsnr->GetParameter(2)<<"\n\t"<<fitsnr->GetParameter(3)<<endl;
	return fitsnr;
}

void landaugaus(){

}
