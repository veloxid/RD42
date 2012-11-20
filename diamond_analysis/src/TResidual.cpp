/*
 * TResidual.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: bachmair
 */

#include "../include/TResidual.hh"

using namespace std;
/** Constructor of TResidual
 * initialise all the variables
 */
TResidual::TResidual(bool bTest){
	this->resetResidual();
	this->SetTestResidual(bTest);
}

/** aut-generated destructor
 * up to now it is making nothig
 */
TResidual::~TResidual() {

}

void TResidual::resetResidual(){
	resXMean=0;
	resXSigma=0;
	resYMean=0;
	resYSigma=0;
	sumRx=0;
	sumRy=0;
	sumVx=0;
	sumVy=0;
	sumV2x=0;
	sumV2y=0;
	sumVRx=0;
	sumVRy=0;
	nUsedTracks=0;
}

/** Prints the results of the ResidualCalulation
 * prints all details of the residual calculation
 * @param level how far to intent the output
 */
void TResidual::Print(UInt_t level){

	cout<<"\n\n";
	if(isTestResidual()){cout<<TCluster::Intent(level)<<"residual is a testresidual"<<endl;}
	else{
		cout<<TCluster::Intent(level)<<"residual results of "<<nUsedTracks<<" used Tracks"<<endl;
		cout<<TCluster::Intent(level)<<"\tX: "<<getXMean()<<"+/-"<<getXSigma()<<"\t"<<sumRx<<" "<<sumVx<<" "<<sumV2x<<" "<<sumVRx<<endl;
		cout<<TCluster::Intent(level)<<"\tY: "<<getYMean()<<"+/-"<<getYSigma()<<"\t"<<sumRy<<" "<<sumVy<<" "<<sumV2y<<" "<<sumVRy<<endl;
		cout<<TCluster::Intent(level)<<"\t Xoff: "<<getXOffset()<<"\tPhiXoff: "<<getPhiXOffset()<<endl;
		cout<<TCluster::Intent(level)<<"\t Yoff: "<<getYOffset()<<"\tPhiYoff: "<<getPhiYOffset()<<endl;
	}
	cout<<"\n\n\n"<<endl;
}

/**
 *
 * @param deltaX
 * @param predX
 * @param deltaY
 * @param predY
 */
void TResidual::addDataPoint(Float_t deltaX, Float_t predX, Float_t deltaY, Float_t predY)
{
	this->resYMean += deltaY;
	this->resYSigma += deltaY*deltaY;
	this->resXMean += deltaX;
	this->resXSigma +=deltaX*deltaX;
	this->sumRx+=deltaX;
	this->sumRy+=deltaY;
	this->sumVx+=predY;
	this->sumVy+=predX;
	this->sumV2x+=predY*predY;//todo check
	this->sumV2y+=predX*predX;//todo check
	this->sumVRx+=predY*deltaX;
	this->sumVRy+=predX*deltaY;
	this->nUsedTracks++;
}

/**
 *
 * @return
 */
Float_t TResidual::getXSigma()
{
	if(bTestResidual) return (100000);
	if (nUsedTracks!=0)
		return (TMath::Sqrt(this->resXSigma / (Double_t)this->nUsedTracks - getXMean()*getXMean()));
	return ( N_INVALID);
}



Float_t TResidual::getYSigma()
{
	if(bTestResidual) return (100000);
	if (nUsedTracks!=0)
		return (TMath::Sqrt(this->resYSigma / (Double_t)this->nUsedTracks - getYMean()*getYMean()));
	return (N_INVALID);
}

Float_t TResidual::getXOffset()
{
	Float_t variableDif= (nUsedTracks * sumV2x - sumVx * sumVx);
	if (variableDif!=0)
		return (-(sumRx * sumV2x - sumVRx * sumVx) / variableDif);
	return (N_INVALID);
}



Float_t TResidual::getYOffset()
{
	Float_t variableDif= (nUsedTracks * sumV2y - sumVy * sumVy);
	if (variableDif!=0)
		return (-(sumRy * sumV2y - sumVRy * sumVy) / variableDif);
	return (N_INVALID);
}

Float_t TResidual::getPhiXOffset()
{
	Float_t variableDif=(nUsedTracks * sumV2y - sumVy * sumVy);
	if(variableDif!=0)
		return (TMath::ATan((nUsedTracks* sumVRy - sumRy * sumVy) / variableDif));
	return (N_INVALID);
}


void TResidual::calculateResidual(TPlaneProperties::enumCoordinate cor, vector<Float_t> *xPred, vector<Float_t> *deltaX, vector<Float_t> *yPred, vector<Float_t> *deltaY, TResidual resOld)
{
	this->clear();
	Float_t resxtest;
	Float_t resytest;
//	if(verbosity>2)cout<<"\tcalculate Residual "<<res_keep_factor<<endl;
//	if(verbosity>2)cout<<"\t"<<deltaX->size()<<" "<<deltaY->size()<<" "<< xPred->size()<<" "<<yPred->size()<<endl;
	for(UInt_t i=0;i<deltaX->size();i++){
		resxtest= TMath::Abs(deltaX->at(i)-resOld.getXMean())/resOld.getXSigma();
		resytest= TMath::Abs(deltaY->at(i)-resOld.getYMean())/resOld.getYSigma();

		//only add if restest is smaller than res_keep_factor
		if((cor==TPlaneProperties::X_COR)&&resxtest<res_keep_factor){
			this->addDataPoint(deltaX->at(i),xPred->at(i),deltaY->at(i),yPred->at(i));
		}//end if
		else if((cor==TPlaneProperties::X_COR)&&resytest<res_keep_factor){
			this->addDataPoint(deltaX->at(i),xPred->at(i),deltaY->at(i),yPred->at(i));
		}//end else if
		else if((cor==TPlaneProperties::XY_COR)&&resxtest<res_keep_factor&&resytest<res_keep_factor){
			this->addDataPoint(deltaX->at(i),xPred->at(i),deltaY->at(i),yPred->at(i));
		}//end else if

	}//end for loop
//	if(verbosity)cout<<"\n";
//	if(!resOld.isTestResidual()&&verbosity)printf("\tresidual with x:%1.2f+/-%1.2f and y:%1.2f+/-%1.2f\n",resOld.getXMean(),resOld.getXSigma(),resOld.getYMean(),resOld.getYSigma());
//	if(verbosity>0)	cout<<"\tused "<<residual.getUsedTracks()<<" Tracks"<<endl;
//	if(verbosity>0)	cout<<"\tX: "<<std::setprecision(4)<<residual.getXMean()<<"+/-"<<residual.getXSigma()<<endl;
//	if(verbosity>0)	cout<<"\tY: "<<residual.getYMean()<<"+/-"<<residual.getYSigma()<<"\n"<<endl;
	//set values
	this->SetTestResidual(false);
}

Float_t TResidual::getPhiYOffset()
{
	Float_t variableDif = (nUsedTracks * sumV2x - sumVx * sumVx);
	if(variableDif!=0)
		return (TMath::ATan(-(nUsedTracks * sumVRx - sumRx * sumVx) / variableDif));
	return (N_INVALID);
}

Float_t TResidual::getXMean()
{
	if(bTestResidual)
		return (0);
	if(nUsedTracks!=0)
		return  (this->resXMean/(Double_t)this->nUsedTracks);
	return (N_INVALID);
}

Float_t TResidual::getYMean()
{
	if(bTestResidual)
		return (0);
	if(nUsedTracks!=0)
		return  (this->resYMean/(Double_t)this->nUsedTracks);
	return (N_INVALID);
}


Float_t TResidual::getDeltaXMean()
{
	if(nUsedTracks!=0)
		return  (this->sumRx/(Double_t)this->nUsedTracks);
	return (N_INVALID);
}

Float_t TResidual::getDeltaYMean()
{
	if(nUsedTracks!=0)
		return  (this->sumRy/(Double_t)this->nUsedTracks);
	return (N_INVALID);
}


Float_t TResidual::getPredXMean()
{
	if(nUsedTracks!=0)
		return  (this->sumVy/(Double_t)this->nUsedTracks);
	return (N_INVALID);
}

Float_t TResidual::getPredYMean()
{
	if(nUsedTracks!=0)
		return  (this->sumVx/(Double_t)this->nUsedTracks);
	return (N_INVALID);
}

void TResidual::clear()
{
	this->bTestResidual=0;
	resXMean= resXSigma=resYMean=resYSigma=0;
	sumRx=sumRy=sumVx=sumVy=sumV2x=sumV2y=sumVRx=sumVRy=0;
	nUsedTracks=0;
}





//
//
//Float_t TResidual::getLinRegOffsetX()
//{
//	Float_t b=getDeltaXMean()-getLinRegSlopeX()*getPredYMean();
//	printf("\tgetLinRegOffsetX: %2.6f\n",b);
//	return b;
//}
//
//
//Float_t TResidual::getLinRegOffsetY()
//{
//	Float_t b=getDeltaYMean()-getLinRegSlopeY()*getPredXMean();
//	printf("\tgetLinRegOffsetY: %2.6f\n",b);
//	return b;
//
//}
//
//
//Float_t TResidual::getLinRegSlopeY()
//{
//	Float_t PredXMean=this->getPredXMean();
//	Float_t deltaYMean=this->getDeltaYMean();
//	Float_t SSxy=sumVRy-1.0/(Float_t)nUsedTracks*sumVy*sumRy;
//	Float_t SSxx=sumV2y-1.0/(Float_t)nUsedTracks*sumVy*sumVy;
//	Float_t m=SSxy/SSxx;
//	Float_t phi = TMath::ATan(m);
//	printf("\tgetDeltaYMean: %2.4f\tgetPredXMean: %2.4f\n",deltaYMean,PredXMean);
//	printf("\tgetLinRegSlopeY: %2.6f,\t phi:%2.6f\n",m,phi);
//	return m;
//}
//Float_t TResidual::getLinRegSlopeX()
//{
//	Float_t PredYMean=this->getPredYMean();
//	Float_t deltaXMean=this->getDeltaXMean();
//	Float_t SSxy=sumVRx-1.0/(Float_t)nUsedTracks*sumVx*sumRx;
//	Float_t SSxx=sumV2x-1.0/(Float_t)nUsedTracks*sumVx*sumVx;
//	Float_t m=SSxy/SSxx;
//	Float_t phi = TMath::ATan(m);
//	printf("\tgetDeltaXMean: %2.4f\tgetPredYMean: %2.4f\n",deltaXMean,PredYMean);
//	printf("\tgetLinRegSlopeX: %2.6f,\t phi:%2.6f\n",m,phi);
//	return m;
//}
//


