/*
 * LandauGaussFit.cpp
 *
 *  Created on: Apr 24, 2012
 *      Author: bachmair
 */

#include "../include/LandauGaussFit.hh"

using namespace std;
LandauGaussFit::LandauGaussFit() {
	Int_t error=0;
	gROOT->LoadMacro("$(HOME)/sdvlp/diamondStripAnalyiss/src/landaugaus.c",&error,true);
}

LandauGaussFit::~LandauGaussFit() {
}


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

TF1 *LandauGaussFit::langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues,
		Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams,
		Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF,bool verbose) {
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
	if(verbose)
		his->Fit(FunName, "RB+");
	else
	his->Fit(FunName, "RQB+"); // fit within specified range, use ParLimits, do not plot

	ffit->GetParameters(fitparams); // obtain fit parameters
	for (i = 0; i < 4; i++) {
		fiterrors[i] = ffit->GetParError(i); // obtain fit parameter errors
	}
	ChiSqr[0] = ffit->GetChisquare(); // obtain chi^2
	NDF[0] = ffit->GetNDF(); // obtain ndf

	return (ffit); // return fit function

}


TF1 *LandauGaussFit::langaufitFixedNoise(TH1F *his, Double_t *fitrange, Double_t *startvalues,
		Double_t *parlimitslo, Double_t *parlimitshi, Double_t *fitparams,
		Double_t *fiterrors, Double_t *ChiSqr, Int_t *NDF,bool verbose) {
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

	sprintf(FunName, "fLangauFixedNoise_%s", his->GetName());

	TF1 *ffitold = (TF1*) gROOT->GetListOfFunctions()->FindObject(FunName);
	if (ffitold)
		delete ffitold;

	TF1 *ffit = new TF1(FunName, langaufun, fitrange[0], fitrange[1], 4);
	ffit->SetParameters(startvalues);
	ffit->SetParNames("Width", "MP", "Area", "GSigma");
	ffit->FixParameter(3,startvalues[3]);

	for (i = 0; i < 4; i++) {
		ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
	}
	ffit->FixParameter(3,startvalues[3]);
	ffit->SetLineColor(kBlue);
	if(verbose)
		his->Fit(FunName, "RB+");
	else
	his->Fit(FunName, "RQB+"); // fit within specified range, use ParLimits, do not plot

	ffit->GetParameters(fitparams); // obtain fit parameters
	for (i = 0; i < 4; i++) {
		fiterrors[i] = ffit->GetParError(i); // obtain fit parameter errors
	}
	ChiSqr[0] = ffit->GetChisquare(); // obtain chi^2
	NDF[0] = ffit->GetNDF(); // obtain ndf

	return (ffit); // return fit function

}

Int_t LandauGaussFit::langaupro(Double_t *params, Double_t &maxx, Double_t &FWHM) {

	// Searches for the location (x value) at the maximum of the
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


TF1* LandauGaussFit::doLandauGaussFitFixedNoise(TH1F* inputHisto, Float_t noise, bool verbose){
	if (inputHisto == 0)
			return 0;
		if (verbose) cout << "Do Landau Gauss Fit for " << inputHisto->GetName() <<" with Fixed Noise "<<noise<< endl;
		float min= inputHisto->GetXaxis()->GetXmin();
		float max = inputHisto->GetXaxis()->GetXmax();
		TF1* landau = new TF1("landau","landau",min, max);
		landau->SetLineColor(kRed);
		inputHisto->Fit(landau,"QN");

		Double_t fr[2];
		Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
		//set range
		fr[0] = 0.5 * inputHisto->GetMean();
		fr[1] = 3.0 * inputHisto->GetMean();
		//low end
		pllo[0] = 0.1*landau->GetParameter(2);
		pllo[1] = 0.4*landau->GetParameter(1);
		pllo[2] = 0.1* inputHisto->Integral();
		pllo[3] = noise;
		//High end
		plhi[0] = 5.0*landau->GetParameter(2);
		plhi[1] = 10*landau->GetParameter(1);
		plhi[2] = 1000* inputHisto->Integral();
		plhi[3] = noise;
		//startValue
		sv[0] = landau->GetParameter(2);
		sv[1] = inputHisto->GetBinCenter(inputHisto->GetMaximumBin());
		sv[2] = inputHisto->Integral();
		sv[3] = noise;
		Double_t chisqr;
		Int_t ndf;
		TF1 *fitsnr = langaufitFixedNoise(inputHisto, fr, sv, pllo, plhi, fp, fpe, &chisqr,&ndf,verbose);

		Double_t SNRPeak, SNRFWHM;
		langaupro(fp, SNRPeak, SNRFWHM);
		return fitsnr;
}

TF1* LandauGaussFit::doLandauGaussFit(TH1F* inputHisto,bool verbose) {
	if (inputHisto == 0)
		return 0;
	if (verbose) cout << "Do Landau Gauss Fit for " << inputHisto->GetName() << endl;
	float min= inputHisto->GetXaxis()->GetXmin();
	float max = inputHisto->GetXaxis()->GetXmax();
	TF1* landau = new TF1("landau","landau",min, max);
	landau->SetLineColor(kRed);
	inputHisto->Fit(landau,"QN");

	Double_t fr[2];
	Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
	//set range
	fr[0] = 0.5 * inputHisto->GetMean();
	fr[1] = 3.0 * inputHisto->GetMean();
	//low end
	pllo[0] = 0.1*landau->GetParameter(2);
	pllo[1] = 0.4*landau->GetParameter(1);
	pllo[2] = 0.1* inputHisto->Integral();
	pllo[3] = 0.5;
	//High end
	plhi[0] = 5.0*landau->GetParameter(2);
	plhi[1] = 10*landau->GetParameter(1);
	plhi[2] = 1000* inputHisto->Integral();
	plhi[3] = 1000;
	//startValue
	sv[0] = landau->GetParameter(2);
	sv[1] = inputHisto->GetBinCenter(inputHisto->GetMaximumBin());
	sv[2] = inputHisto->Integral();
	sv[3] = 5.0;
	Double_t chisqr;
	Int_t ndf;
	TF1 *fitsnr = langaufit(inputHisto, fr, sv, pllo, plhi, fp, fpe, &chisqr,&ndf,verbose);

	Double_t SNRPeak, SNRFWHM;
	langaupro(fp, SNRPeak, SNRFWHM);
	return fitsnr;
}
