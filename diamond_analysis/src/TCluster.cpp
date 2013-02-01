/*
 * TCluster.cpp
 *
 *  Created on: 21.11.2011
 *      Author: bachmair
 */

#include "../include/TCluster.hh"
ClassImp(TCluster);

TCluster::TCluster(int nEvent,UChar_t det, int seedSigma,int hitSigma,UInt_t nChannels,Float_t cmNoise) {
	clusterChannel.clear();
	clusterSignal.clear();
	clusterADC.clear();
	clusterSignalInSigma.clear();
	clusterChannelScreened.clear();
	clusterPedMean.clear();
	clusterPedSigma.clear();
	clusterPedMeanCMN.clear();
	clusterPedSigmaCMN.clear();
	numberOfSeeds=0;
	numberOfHits=0;
	numberOfNoHits=0;
	maximumSignal=0;
	maxChannel=0;
	charge=0;
	this->seedSigma=seedSigma;
	this->hitSigma=hitSigma;
	verbosity=0;
	revisionNumber=TCLUSTER_REVISION();
	isChecked=false;
	isSaturated=false;
	isLumpy=false;
	isGoldenGate=false;
	hasBadChannel=false;
	this->det=det;
	this->eventNumber=nEvent;
	mode=highest2Centroid;
	this->nChannels=nChannels;
	this->cmNoise = cmNoise;
	if (verbosity>2) cout<<"new TCluster of event "<<nEvent<<" of detector "<<(int)det<<" Common mode Noise "<<cmNoise<<endl;
}

TCluster::~TCluster() {
}


TCluster::TCluster(const TCluster& rhs){
	//TODO WIe gehe ich vor mit den ganzen TObject kram???
	verbosity=rhs.verbosity;
	if(verbosity)cout<<"copy TCluster for det  "<<(int)rhs.det<<" "<<rhs.checkClusterForSize()<<endl;
	clusterChannel.clear();
	clusterSignal.clear();
	clusterADC.clear();
	clusterSignalInSigma.clear();
	clusterChannelScreened.clear();
	clusterPedMean.clear();
	clusterPedSigma.clear();
	clusterPedMeanCMN.clear();
	clusterPedSigmaCMN.clear();
	cmNoise=rhs.cmNoise;
	for(UInt_t i=0;i<rhs.checkClusterForSize();i++){
		clusterSignal.push_back(rhs.clusterSignal.at(i));
		clusterChannel.push_back(rhs.clusterChannel.at(i));
		clusterADC.push_back( rhs.clusterADC.at(i));
		clusterSignalInSigma.push_back( rhs.clusterSignalInSigma.at(i));
		clusterPedMean.push_back(rhs.clusterPedMean.at(i));
		clusterPedSigma.push_back(rhs.clusterPedSigma.at(i));
		clusterPedMeanCMN.push_back(rhs.clusterPedMeanCMN.at(i));
		clusterPedSigmaCMN.push_back(rhs.clusterPedSigmaCMN.at(i));
	}
	for(UInt_t i=0;i<rhs.clusterChannelScreened.size();i++)
		clusterChannelScreened.push_back(rhs.clusterChannelScreened.at(i));
	this->numberOfSeeds=rhs.numberOfSeeds;
	numberOfHits=rhs.numberOfHits;
	numberOfNoHits=rhs.numberOfNoHits;
	seedSigma=rhs.seedSigma;
	hitSigma=rhs.hitSigma;
	isSaturated=rhs.isSaturated;
	isGoldenGate=rhs.isGoldenGate;
	isLumpy=rhs.isLumpy;
	isChecked=rhs.isChecked;
	hasBadChannel=rhs.hasBadChannel;
	mode=rhs.mode;
	verbosity=rhs.verbosity;
	charge=rhs.charge;
	maximumSignal=rhs.maximumSignal;
	maxChannel=rhs.maxChannel;
	revisionNumber=rhs.revisionNumber;
	nChannels=rhs.nChannels;
	det=rhs.det;
	eventNumber=rhs.eventNumber;

}

/**
 * the class assignment Function
 * @param src
 * @return
 */
TCluster & TCluster::operator =(const TCluster & src)
{
	hitSigma = src.hitSigma;
	seedSigma= src.seedSigma;
	isSaturated= src.isSaturated;
	isLumpy=src.isLumpy;
	isGoldenGate=src.isGoldenGate;
	isChecked=src.isChecked;
	hasBadChannel=src.hasBadChannel;
	mode=src.mode;
	numberOfHits=src.numberOfHits;
	numberOfNoHits=src.numberOfNoHits;
	numberOfSeeds=src.numberOfSeeds;
	verbosity=src.verbosity;
	charge=src.charge;
	maxChannel=src.maxChannel;
	maximumSignal=src.maximumSignal;
	revisionNumber=src.revisionNumber;
	nChannels=src.nChannels;
	det=src.det;
	eventNumber=src.eventNumber;
	cmNoise= src.cmNoise;
	for(UInt_t i=0;i<src.clusterChannel.size();i++)
		clusterChannel.push_back(src.clusterChannel.at(i));
	for(UInt_t i=0;i<src.clusterSignal.size();i++)
		clusterSignal.push_back(src.clusterSignal.at(i));
	for(UInt_t i=0;i<src.clusterADC.size();i++)
		clusterADC.push_back(src.clusterADC.at(i));
	for(UInt_t i=0;i<src.clusterSignalInSigma.size();i++)
		clusterSignalInSigma.push_back(src.clusterSignalInSigma.at(i));
	for(UInt_t i=0;i<src.clusterPedMean.size();i++)
		clusterPedMean.push_back(src.clusterPedMean.at(i));
	for(UInt_t i=0; i<src.clusterPedSigma.size();i++)
		clusterPedSigma.push_back(src.clusterPedSigma.at(i));
	for(UInt_t i=0;i<src.clusterPedMeanCMN.size();i++)
		clusterPedMeanCMN.push_back(src.clusterPedMeanCMN.at(i));
	for(UInt_t i=0; i<src.clusterPedSigmaCMN.size();i++)
		clusterPedSigmaCMN.push_back(src.clusterPedSigmaCMN.at(i));
	for(UInt_t i=0;i<src.clusterChannelScreened.size();i++)
		clusterChannelScreened.push_back(src.clusterChannelScreened.at(i));
	return *this;
}

/**
 * checks if all deques with the data of the cluster do have the same size
 * @return the size of the cluster if all sizes are the same, else 0
 */
UInt_t TCluster::checkClusterForSize() const{
	UInt_t nChannel=clusterChannel.size();
	UInt_t nAdc = clusterADC.size();
	UInt_t nPedMean = clusterPedMean.size();
	UInt_t nPedMeanCMN = clusterPedMeanCMN.size();
	UInt_t nPedSigma = clusterPedSigma.size();
	UInt_t nPedSigmaCMN = clusterPedSigmaCMN.size();

	bool retVal = (nChannel==nAdc);
	retVal = retVal && nAdc==nPedMean;
	retVal = retVal && nPedMean==nPedMeanCMN;
	retVal = retVal && nPedMeanCMN==nPedSigma;
	retVal = retVal && nPedSigma==nPedSigmaCMN;
	if(retVal==true)
		return nAdc;
	else
		return 0;
}

/**
 *
 * @param ch
 * @param signal
 * @param signalInSigma
 * @param adcValue
 * @param bSaturated
 * @param screened
 * @todo: bbSaturated not yet used....
 */

void TCluster::addChannel(UInt_t ch, Float_t pedMean, Float_t pedSigma, Float_t pedMeanCMN, Float_t pedSigmaCMN, Int_t adcValue, bool bSaturated,bool isScreened){
//void TCluster::addChannel(UInt_t ch, Float_t signal,Float_t signalInSigma,UShort_t adcValue, bool bSaturated,bool screened){
	if(ch>TPlaneProperties::getNChannels((UInt_t)this->det)||ch!=ch||pedMean!=pedMean||pedSigma!=pedSigma||pedMeanCMN!=pedMeanCMN||pedSigmaCMN!=pedSigmaCMN||adcValue!=adcValue){
		cerr<<"The channel which should be added is not VALID! you cannot add a channel "<<ch<<" at det "<<det<<endl;
		return;
	}
	Float_t signal = adcValue - pedMean;
	Float_t signalInSigma= signal/pedSigma;
	Float_t signalCMN = adcValue - pedMeanCMN - this->cmNoise;
	Float_t signalInSigmaCMN= signal/pedSigma;
	if(verbosity>2)cout<<"("<<ch<<"/"<<signal<<"/"<<signalInSigma<<")";
	this->isSaturated=this->isSaturated||bSaturated;
	if(signalInSigma>seedSigma)
		numberOfSeeds++;
	else if(signalInSigma>hitSigma)
		numberOfHits++;
	else{
		//cerr<<"No valid channel added to cluster:"<<ch<<" "<<signal<<" "<<signalInSigma<<" "<<seedSigma<<" "<<hitSigma<<(signalInSigma>hitSigma)<<endl;
		numberOfNoHits++;
	}
	if(signal>maximumSignal){
		maximumSignal=signal;
		maxChannel=ch;
	}
	if(signalInSigma>hitSigma) charge+=signal;
	if(this->checkClusterForSize()>0&&ch<clusterChannel.front()){
		clusterChannel.push_front(ch);
		clusterSignal.push_front(signal);
		clusterSignalInSigma.push_front(signalInSigma);
		clusterSignalCMN.push_back(signalCMN);
		clusterSignalInSigmaCMN.push_back(signalInSigmaCMN);
		clusterADC.push_front(adcValue);
		this->clusterChannelScreened.push_front(isScreened);
		clusterPedMean.push_front(pedMean);
		clusterPedMeanCMN.push_front(pedMeanCMN);
		clusterPedSigma.push_front(pedSigma);
		clusterPedSigmaCMN.push_front(pedSigmaCMN);
	}
	else{
		clusterChannel.push_back(ch);
		clusterSignal.push_back(signal);
		clusterSignalInSigma.push_back(signalInSigma);
		clusterSignalCMN.push_back(signalCMN);
		clusterSignalInSigmaCMN.push_back(signalInSigmaCMN);
		clusterADC.push_back(adcValue);
		clusterPedMean.push_back(pedMean);
		clusterPedMeanCMN.push_back(pedMeanCMN);
		clusterPedSigma.push_back(pedSigma);
		clusterPedSigmaCMN.push_back(pedSigmaCMN);
		this->clusterChannelScreened.push_back(isScreened);
	}
	if(verbosity>2)cout<<flush;

}
Float_t TCluster::getPosition(calculationMode_t mode,TH1F *histo){
	if(mode==maxValue)
		return this->getHighestSignalChannel();
	else if(mode==chargeWeighted)
		return this->getChargeWeightedMean();
	else if(mode==highest2Centroid)
		return this->getHighest2Centroid();
	else if(mode==eta)
		return this->getEtaPostion();
	else if(mode == corEta&&histo==0){
		cerr<<"mode = cor Eta, but histo =0, "<<endl;
//		this->Print();
		return this->getEtaPostion();
	}
	else if(mode == corEta&&histo!=0)
		return this->getPositionCorEta(histo);

	return 0;//todo;
}

UInt_t TCluster::size()
{
	return numberOfSeeds+numberOfHits;
}

UInt_t TCluster::seedSize(){
	return numberOfSeeds;
}
void TCluster::clear(){
	numberOfSeeds=0;
	numberOfHits=0;
	//cluster.clear();
}
bool TCluster::isLumpyCluster(){
	if (!isChecked)
			checkCluster();
	return isLumpy;
}

bool TCluster::isGoldenGateCluster(){
	if (!isChecked)
		checkCluster();
	return this->isGoldenGate;
}

bool TCluster::hasSaturatedChannels(){
  for(UInt_t cl=0;cl<this->clusterADC.size();cl++){
    if (getAdcValue(cl)>=TPlaneProperties::getMaxSignalHeight(this->det)) return true;
  }
  return false;
	return isSaturated;//todo
}
Float_t TCluster::getCharge(bool useSmallSignals){
//	if(useSmallSignals)
		return getCharge(1000,useSmallSignals);
//	return charge;
}

/**
 * @param nClusterEntries
 * @param useSmallSignals
 * @return
 */
Float_t TCluster::getCharge(UInt_t nClusterEntries,bool useSmallSignals){
	if(nClusterEntries==0)return 0;
	Float_t clusterCharge=getSignalOfChannel(this->getHighestSignalChannel());
	bool b2ndHighestStripIsBigger = (getHighestSignalChannel()-this->getHighest2Centroid()<0);
	for(UInt_t nCl=1;nCl<nClusterEntries&&nCl<this->checkClusterForSize();nCl++){
		if(b2ndHighestStripIsBigger){
			if(nCl%2==1){
				if (useSmallSignals||isHit(getClusterPosition(getHighestSignalChannel()+(nCl/2)+1)))
					clusterCharge=clusterCharge + this->getSignalOfChannel(getHighestSignalChannel()+(nCl/2)+1);
			}
			else{
				if (useSmallSignals||isHit(getClusterPosition(getHighestSignalChannel()-(nCl/2))))
					clusterCharge=clusterCharge + this->getSignalOfChannel(getHighestSignalChannel()-(nCl/2));
			}
		}
		else{
			if(nCl%2==1){

				if (useSmallSignals||isHit(getClusterPosition(getHighestSignalChannel()-(nCl/2)-1)))
					clusterCharge+=this->getSignalOfChannel(getHighestSignalChannel()-(nCl/2)-1);
			}
			else{
				if (useSmallSignals||isHit(getClusterPosition(getHighestSignalChannel()+(nCl/2))))
					clusterCharge+=this->getSignalOfChannel(getHighestSignalChannel()+(nCl/2));
			}
		}

	}
	return clusterCharge;
}

/**
 *
 * @return channel no of highest Signal
 */
UInt_t TCluster::getHighestSignalChannel()
{
	return maxChannel;
}

UInt_t TCluster::getClusterSize(){
	return this->checkClusterForSize();
}

Float_t TCluster::getChargeWeightedMean(bool useNonHits){
	Float_t sum=0;
	Float_t charged=0;
	for(UInt_t cl=0;cl<this->checkClusterForSize();cl++){
		if(useNonHits||isHit(cl)||checkClusterForSize()<4){//todo anpassen
			//todo . take at least second biggest hit for =charge weighted mean
			sum+=getChannel(cl)*getSignal(cl);//kanalnummer*signalNummer
			charged+=getSignal(cl);;//signal
		}
	}
	if (charged>0)
		return sum/charged;
	return -1;
}


void TCluster::setPositionCalulation(calculationMode_t mode){
this->mode=mode;
}


void TCluster::checkCluster(){
	this->checkForGoldenGate();
	this->checkForLumpyCluster();
	isChecked=true;
}

void TCluster::checkForGoldenGate(){
	if(verbosity>10)cout<< "check for golden gate cluster!: "<<endl;
	this->isGoldenGate=false;
	if(this->checkClusterForSize()<=2)
		return;
	if(verbosity>10)cout<<"clSize:"<<checkClusterForSize()<<" "<<clusterChannel.size()<<flush;
	int previousSeed=-1;
	for(UInt_t i=0;i<this->checkClusterForSize()&&!isGoldenGate;i++){
		if(getSNR(i)>seedSigma){
			if( previousSeed!=-1 && previousSeed+1!=(int)getChannel(i) )
				isGoldenGate=true;

			previousSeed=(int)getChannel(i);
		}
	}

}

int TCluster::getHitSigma() const
{
    return hitSigma;
}

int TCluster::getSeedSigma() const
{
    return seedSigma;
}

void TCluster::setHitSigma(int hitSigma)
{
    this->hitSigma = hitSigma;
}

void TCluster::setSeedSigma(int seedSigma)
{
    this->seedSigma = seedSigma;
}

bool TCluster::isScreened()
{
	bool isOneChannelScreened=false;
	for(UInt_t cl;cl<clusterChannelScreened.size();cl++)
		isOneChannelScreened+=isScreened(cl);
	isOneChannelScreened+=((this->getSmallestChannelNumber()==0)||this->getHighestChannelNumber()==nChannels-1);
	return isOneChannelScreened;
}

bool TCluster::isScreened(UInt_t cl)
{
	if(cl<this->clusterChannelScreened.size())
		return clusterChannelScreened.at(cl);
	else{
		cout<<"tried to get isScreend for not valid channel from cluster:"<<cl<<endl;
		return true;
	}

}


UInt_t TCluster::getHighestHitClusterPosition()
{
	UInt_t maxCh = this->getHighestSignalChannel();
	UInt_t clPos;
	for(clPos=0;maxCh!=this->getChannel(clPos)&&clPos<size();clPos++){
	}
	if(maxCh==getChannel(clPos))
		return clPos;
	else return 9999;
}

Float_t TCluster::getHighest2Centroid()
{
	if(getClusterSize()==0)return 0;
	UInt_t maxCh = this->getHighestSignalChannel();
	UInt_t clPos;
	for(clPos=0;maxCh!=this->getChannel(clPos)&&clPos<size();clPos++){
	}

	Float_t retVal;
	UInt_t channel=getChannel(clPos);
	Float_t signal=getSignal(clPos);
	Float_t nextChannelSignal;
	UInt_t nextChannel;
	if(this->getSignal(clPos-1)<getSignal(clPos+1)){
		nextChannel=getChannel(clPos+1);
		nextChannelSignal=getSignal(clPos+1);
//		cout<<"r"<<flush;
	}
	else{
		nextChannel=getChannel(clPos-1);
		nextChannelSignal=getSignal(clPos-1);
//		cout<<"l"<<flush;
	}
	retVal=(channel*signal+nextChannel*nextChannelSignal)/(signal+nextChannelSignal);
//	cout<<signal<<"*"<<channel<<"+"<<nextChannelSignal<<"*"<<nextChannel<<":"<<retVal<<" "<<flush;
	return retVal;
}

/**
 * todo: ueberpruefen
 */
void TCluster::checkForLumpyCluster(){
	if(verbosity>10)cout<<"check is lumpy:"<<endl;
	this->isLumpy=false;
	UInt_t clSize = checkClusterForSize();
	if(this->checkClusterForSize()<=2)
		return;//for lumpy cluster at least 3 hits are needed
	bool isfalling;
	Float_t lastSeed;
	if(verbosity>10)cout<<" Do Loop: "<<endl;
	for(UInt_t i=0;i<clSize&&!isLumpy;i++){
		float signal = getSignal(i);
		float snr = getSNR(i);
		if(verbosity>10)cout<<" "<<i<<flush;
			if(getSNR(i)>seedSigma){
				if(verbosity>10)cout<<" - found seed with SNR "<<getSNR(i)<<flush;
				if(lastSeed<getSignal(i)&&!isfalling){
					lastSeed=getSignal(i);
					if(verbosity>10)cout<<", found new las seed with signal "<< getSignal(i)<<flush;
				}
				if(lastSeed<getSignal(i)&&isfalling){
					isLumpy=true;
					if(verbosity>10)cout<<", is LUMPY"<<flush;
				}
				else{
					isfalling=true;
					if(verbosity>10)cout<<", is falling"<<flush;
				}
			}
			if(verbosity>10)cout<<endl;
	}
	if(verbosity>10)cout<<"isLumpyCluster: "<<isLumpy<<endl;
}

/**
 * checks if an entry in the cluster is above the seed SNR
 * @param cl
 * @return
 */
bool TCluster::isSeed(UInt_t cl){
	if(cl<checkClusterForSize())
		return (getSNR(cl)>this->seedSigma);
	return false;
}
/**
 * checks if an entry in the cluster is above the hit SNR
 * Could be either a hit or a seed
 * @param cl
 * @return
 */
bool TCluster::isHit(UInt_t cl){
	if(cl<checkClusterForSize())
		return (getSNR(cl)>this->hitSigma);
	return false;
}

UInt_t TCluster::getSmallestChannelNumber()
{
	if(clusterChannel.size()>0){
		return clusterChannel.front();
	}
	return 0;
}

/**
 * highest channel number of cluster...
 * goes to end of cluster and looks what is the channel no
 * @return highest channel number of cluster
 */
UInt_t TCluster::getHighestChannelNumber()
{

	if(clusterChannel.size()>0){
		return clusterChannel.back();
	}
	return 0;
}


Float_t TCluster::getHighestSignal(){
	Float_t signal=0;
	UInt_t highestSignalClusterPos;
	UInt_t highestSignalChannelPos;
	for(UInt_t clPos=0;clPos<checkClusterForSize();clPos++){
		if(getSignal(clPos)>signal){
			signal=getSignal(clPos);
			highestSignalClusterPos=clPos;
		}
	}
	highestSignalChannelPos=getChannel(highestSignalClusterPos);
	if(maximumSignal!=signal){
		cout<<"maximumSignal "<<maximumSignal<<" and highest signal "<<signal<<" does not match... Something is wrong:";
		cout<<"highestSignalChannelPos: "<<highestSignalChannelPos<<"\thighesSignalClusterPos: "<<highestSignalClusterPos<<" "<<getHighestHitClusterPosition()<<endl;

	}

	return this->maximumSignal;
}



UInt_t TCluster::getClusterPosition(UInt_t channelNo){
	if(channelNo<this->getSmallestChannelNumber()&&channelNo>this->getHighestSignalChannel()){
		if(verbosity)cout<<"ChannelNo does not match..."<<endl;
		return 9999;
	}
	UInt_t clPos;
	for(clPos=0;clPos<checkClusterForSize()&&getChannel(clPos)!=channelNo;clPos++){}
	return clPos;
}

Float_t TCluster::getSignalOfChannel(UInt_t channel)
{
	if(channel<this->getSmallestChannelNumber()&&channel>this->getHighestSignalChannel()) return 0;
	UInt_t clPos;
	for(clPos=0;clPos<checkClusterForSize()&&getChannel(clPos)!=channel;clPos++){};
	if(clPos<getClusterSize()){//TODO
		Float_t signal = getSignal(clPos);
//		if (signal>0)
		return signal;
	}
//	cout << "TCluster::getSignalOfChannel: clPos>=getClusterSize()\treturn 0" << endl;
	return 0;
}


Float_t TCluster::getSignal(UInt_t clusterPos, bool cmnCorrected)
{
	if(clusterPos<checkClusterForSize()){
		 Int_t adc = getAdcValue(clusterPos);
		 Float_t pedMean = getPedestalMean(clusterPos,cmnCorrected);
		 Float_t signal = (Float_t)adc - pedMean;
		 if (cmnCorrected) signal -= getCMN();
//		 if(!cmnCorrected && signal != getSignal(clusterPos))
//			 cout<< "signal clalulated: "<<signal<<" other:"<<getSignal(clusterPos)<<endl;
		 return signal;
	}
	else {
		if(verbosity)cout<<"clusterPos "<<clusterPos<<" bigger than clusterSize"<<checkClusterForSize()<<endl;
		return 0;
	}
}


/**
 */
Float_t TCluster::getSNR(UInt_t clusterPos, bool cmnCorrected)
{

	if(clusterPos<checkClusterForSize()){
		Float_t signal= getSignal(clusterPos,cmnCorrected);
		Float_t sigma = getPedestalSigma(clusterPos,cmnCorrected);
		return signal/sigma;
	}
	else return -1;

}
/**
 */
Float_t TCluster::getPedestalMean(UInt_t clusterPos, bool cmnCorrected)
{

	if(clusterPos<checkClusterForSize())
		if( cmnCorrected)
			return this->clusterPedMeanCMN.at(clusterPos);
		else
			return this->clusterPedMean.at(clusterPos);
	else return -1;

}


Float_t TCluster::getPedestalSigma(UInt_t clusterPos,bool cmnCorrected)
{
	if(clusterPos<checkClusterForSize())
		if( cmnCorrected)
			return this->clusterPedSigmaCMN.at(clusterPos);
		else
			return this->clusterPedSigma.at(clusterPos);
	else return -1;

}

Int_t TCluster::getAdcValue(UInt_t clusterPos)
{
	if(clusterPos<checkClusterForSize())
		return this->clusterADC.at(clusterPos);
	else return 0;
}

Float_t TCluster::getEta()
{
	if (checkClusterForSize() < 2) return -1;
	UInt_t clPosHighest = getHighestHitClusterPosition();
	UInt_t clPos2ndHighest = getHighestSignalNeighbourClusterPosition(getHighestHitClusterPosition());
	UInt_t leftClPos = 0;
	UInt_t rightClPos = 0;
	if (clPosHighest < clPos2ndHighest) {
		leftClPos = clPosHighest;
		rightClPos = clPos2ndHighest;
	}
	else {
		leftClPos = clPos2ndHighest;
		rightClPos = clPosHighest;
	}
	Float_t sumSignal = (getSignal(leftClPos)+getSignal(rightClPos));
	if(sumSignal==0||getSignal(rightClPos)==0)
		return -1;
	return getSignal(rightClPos) / sumSignal;
}

Float_t TCluster::getEtaPostion(){
	if (checkClusterForSize() < 3) return -1;
	UInt_t clPosHighest = getHighestHitClusterPosition();
	UInt_t clPos2ndHighest = getHighestSignalNeighbourClusterPosition(getHighestHitClusterPosition());
	UInt_t leftClPos = 0;
	UInt_t rightClPos = 0;
	if (clPosHighest < clPos2ndHighest) {
		leftClPos = clPosHighest;
		rightClPos = clPos2ndHighest;
	}
	else {
		leftClPos = clPos2ndHighest;
		rightClPos = clPosHighest;
	}
	UInt_t leftChannel = this->getChannel(leftClPos);
	Float_t eta = getEta();
	if(eta==0)return -9999;
	return eta+leftChannel;
}

Float_t TCluster::getPositionCorEta(TH1F* histo){
	if (checkClusterForSize() < 3) return -1;
	if(histo==0) return this->getEtaPostion();
	UInt_t clPosHighest = getHighestHitClusterPosition();
	UInt_t clPos2ndHighest = getHighestSignalNeighbourClusterPosition(getHighestHitClusterPosition());
	UInt_t leftClPos = 0;
	UInt_t rightClPos = 0;

	Float_t eta = getEta();
	if(verbosity)cout<<"get Position Cor Eta: "<<eta<<" ";
	if (clPosHighest < clPos2ndHighest) {
		leftClPos = clPosHighest;
		rightClPos = clPos2ndHighest;
		if(verbosity)cout<<"leftHighest "<<flush;
	}
	else {
		leftClPos = clPos2ndHighest;
		rightClPos = clPosHighest;
		if(verbosity)cout<<"rightHighest "<<flush;
	}
	if(eta<=0)
		return getEtaPostion();
	Float_t corEta= getValueOfHisto(eta,histo);
	UInt_t leftChannelNo= getChannel(leftClPos);
	if(verbosity)	cout<<leftChannelNo<<" + "<<corEta<<" = "<<leftChannelNo+corEta;
	return leftChannelNo+corEta;
}
/**
 * todo: return value 5000 check if that makes sense
 * @param clusterPos
 * @return
 */
UInt_t TCluster::getChannel(UInt_t clusterPos)
{
	if(clusterPos<checkClusterForSize())
		return this->clusterChannel.at(clusterPos);
	else return 5000;
}


UInt_t TCluster::getHighestSignalNeighbourChannel(UInt_t channelNo)
{
	return getChannel(getHighestSignalNeighbourClusterPosition(getClusterPosition(channelNo)));
}

/**
 * gets the clusterposition of the highest signal neigboured to a given clusterposition
 * @param clPos position where should be looked next to
 * @return clusterPosition in cluster
 */
UInt_t TCluster::getHighestSignalNeighbourClusterPosition(UInt_t clPos)
{
	if (clPos>=checkClusterForSize() || clPos<0 || checkClusterForSize()<2) return 9999;
	if (getSignal(clPos-1) < getSignal(clPos+1)) return clPos+1;
	if (getSignal(clPos-1) > 0) return clPos-1;
	return 9999;
}

/**
 * small function to Intend cout-output;
 * @input level level of intention
 * @return string with level tabs
 * @todo move to "string stuff" class
 */
string TCluster::Intent(UInt_t level){
	stringstream output;
	output.str("");
	for(UInt_t i=0;i<level;i++){
		output<<"  ";
	}
	return output.str();
}
Float_t TCluster::getValueOfHisto(Float_t x, TH1F* histo){
	if(histo==0){
		cerr<<"Histo pointer = 0!"<<endl;
		return -999;
	}

	if(histo->IsZombie()){
		cerr<<"Histo is Zombie!"<<endl;
		return -999;
	}
	Float_t xmin = histo->GetXaxis()->GetXmin();
	Float_t xmax = histo->GetXaxis()->GetXmax();
	if(xmin<=x&&x<=xmax){
		Int_t bin = histo->FindBin(x);
		return histo->GetBinContent(bin);
	}
	cerr<<" x = "<<x<<" not in range ["<<xmin<<","<<xmax<<"]"<<endl;
	return -999;

}

void TCluster::Print(UInt_t level){
	cout<<Intent(level)<<"Cluster of Event "<<flush;
	cout<<eventNumber<<" in detector"<<(int)det<<" with "<<size()<<"/"<<checkClusterForSize()<<" Cluster entries"<<flush;
	for(UInt_t cl=0;cl<checkClusterForSize();cl++){
		if(this->isSeed(cl))
			cout<<"\t{"<<this->getChannel(cl)<<"|"<<this->getAdcValue(cl)<<"|"<<this->getSignal(cl)<<"|"<<this->getSNR(cl)<<"}"<<flush;
		else if(this->isHit(cl))
					cout<<"\t("<<this->getChannel(cl)<<"|"<<this->getAdcValue(cl)<<"|"<<this->getSignal(cl)<<"|"<<this->getSNR(cl)<<")"<<flush;
		else
					cout<<"\t["<<this->getChannel(cl)<<"|"<<this->getAdcValue(cl)<<"|"<<this->getSignal(cl)<<"|"<<this->getSNR(cl)<<"]"<<flush;
	}
	cout<<"\t||"<<this->getHighestSignalChannel()<<" "<<flush<<this->getHighest2Centroid()<<" "<<this->getChargeWeightedMean()<<" "<<this->getEtaPostion()<<" "<<this->getPositionCorEta();
	cout<<endl;
}