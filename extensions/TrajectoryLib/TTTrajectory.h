/* 
 *	Trajectory≈
 *	Trajectory generator/wrapper object for Jamoma
 *	Copyright © 2010 by Nils Peters
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_TRAJECTORY_H__
#define __TT_TRAJECTORY_H__

#include "TTDSP.h"


/**	A Generalized trajectory generator/wrapper object for Jamoma. */
class TTTrajectory : TTAudioObject {
	TTCLASS_SETUP(TTTrajectory)
	
protected:
	TTAudioObjectPtr		mActualTrajectoryObject;	///< The actual trajectory object that this object is currently wrapping
	TTFloat64				mA, mB, mC, mDeltaX, mDeltaY, mDeltaZ;
	TTSymbolPtr				mType;					///< The name of the current trajectory type
	TTUInt8					mMode;
	TTAudioObjectPtr		mPhasors[3];
	TTAudioSignalArrayPtr	mPhasorOutputSignals;
	
public:
	
	TTErr setType(const TTValue& newValue)
	{	
		TTSymbolPtr newType = newValue;
		TTErr		err = kTTErrNone;
		
		// if the type didn't change, then don't change the trajectory
		if (newType == mType)
			return kTTErrNone;
		
		mType = newType;
		err = TTObjectInstantiate(mType, &mActualTrajectoryObject, 2);			
		//if (!err) {
			// Now that we have our new filter, update it with the current state of the wrapper:
			//mActualFilterObject->setAttributeValue(TT("frequency"), mFrequency);
			//err = mActualFilterObject->setAttributeValue(TT("q"), mQ);
			//if (err == kTTErrInvalidAttribute)
			//	err = mActualFilterObject->setAttributeValue(TT("resonance"), mQ);
			//mActualFilterObject->setAttributeValue(TT("bypass"), this->attrBypass);
			//mActualFilterObject->setAttributeValue(kTTSym_sampleRate, sr);
		//}
		return err;
	}
	
	
	TTErr getTypes(TTValue& listOfTrajectoryTypesToReturn)
	{
		TTValue v;
		v.setSize(2);
		v.set(0, TT("trajectory"));
		v.set(1, TT("audio")); 
		return TTGetRegisteredClassNamesForTags(listOfTrajectoryTypesToReturn, v);
	}
	
	TTErr getCurrentAttributeNames(TTValue& listOfCurrentAttributesToReturn)
	{
		long		n;
		TTValue		names;
		TTSymbol*	aName;
		TTString	nameString;
		
		listOfCurrentAttributesToReturn.clear();
		mActualTrajectoryObject->getAttributeNames(names);
		n = names.getSize();
		for (int i=0; i<n; i++) {
			names.get(i, &aName);
			nameString = aName->getString();
			if (aName == TT("bypass") || aName == TT("mute") || aName == TT("maxNumChannels") || aName == TT("sampleRate"))
				continue;	
			listOfCurrentAttributesToReturn.append(aName);
		}
		if (listOfCurrentAttributesToReturn.getSize()==0)
			listOfCurrentAttributesToReturn.append(TT("NULL")); // in case there is no attribute, send a NULL value out
												   
		return kTTErrNone;
	}
	
	
	TTErr reset()
	{
		TTValue v;
		v.set(0,0.0);
		mPhasors[0]->setAttributeValue(TT("phase"),v);
		mPhasors[1]->setAttributeValue(TT("phase"),v);
		mPhasors[2]->setAttributeValue(TT("phase"),v);
		return kTTErrNone;
	}
	
	TTErr setA(const TTValue& newValue)
	{	
		mA = newValue;
		return mActualTrajectoryObject->setAttributeValue(TT("a"), mA);
	}
	
	
	TTErr setB(const TTValue& newValue)
	{	
		mB = newValue;
		return mActualTrajectoryObject->setAttributeValue(TT("b"), mB);
	}
	
	
	TTErr setC(const TTValue& newValue)
	{	
		mC = newValue;
		return mActualTrajectoryObject->setAttributeValue(TT("c"), mC);
	}
	
	
	TTErr setDeltaX(const TTValue& newValue)
	{	
		mDeltaX = newValue;
		return mActualTrajectoryObject->setAttributeValue(TT("deltaX"), mDeltaX);
	}
	
	
	TTErr setDeltaY(const TTValue& newValue)
	{	
		mDeltaY = newValue;
		return mActualTrajectoryObject->setAttributeValue(TT("deltaY"), mDeltaY);
	}
	
	
	TTErr setDeltaZ(const TTValue& newValue)
	{	
		mDeltaZ = newValue;
		return mActualTrajectoryObject->setAttributeValue(TT("deltaZ"), mDeltaZ);
	}
	
/*	
	TTErr updateMaxNumChannels(const TTValue& oldMaxNumChannels)
	{
		if (mActualFilterObject)
			return mActualFilterObject->setAttributeValue(kTTSym_maxNumChannels, maxNumChannels);
		else
			return kTTErrNone;
	}
	
*/		
	TTErr updateSampleRate(const TTValue& oldSampleRate)
	{
		mPhasors[0]->setAttributeValue(kTTSym_sampleRate, (uint)sr);
		mPhasors[1]->setAttributeValue(kTTSym_sampleRate, (uint)sr);
		mPhasors[2]->setAttributeValue(kTTSym_sampleRate, (uint)sr); 
		return mActualTrajectoryObject->setAttributeValue(kTTSym_sampleRate, (uint)sr);
	}

	TTErr setMode(const TTValue& newValue)
	{	
		mMode = newValue;
		return setProcessPointers();
	}
	
	TTErr setProcessPointers()
	{
		TTErr	err = kTTErrNone;
		
		if (mMode == 0) {
			err = setProcess((TTProcessMethod)&TTTrajectory::processAudioPhasorInternal);
		}
		else {
			err = setProcess((TTProcessMethod)&TTTrajectory::processAudioRampInternal);
		}
		return err;
	}
	
	TTErr processAudioPhasorInternal(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs)
	{
		// TODO: check if there are inputs, if so then use them, otherwise do as we do below

		mPhasorOutputSignals->allocAllWithVectorSize(outputs->getVectorSize());
		
		mPhasors[0]->process(mPhasorOutputSignals->getSignal(0));
		mPhasors[1]->process(mPhasorOutputSignals->getSignal(1));
		mPhasors[2]->process(mPhasorOutputSignals->getSignal(2));
		
		return mActualTrajectoryObject->process(mPhasorOutputSignals, outputs);
		
		//return mActualTrajectoryObject->process(inputs, outputs);
	}

	TTErr processAudioRampInternal(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs)
	{
		; // TODO: do something here
	}

};


#endif // __TT_TRAJECTORY_H__