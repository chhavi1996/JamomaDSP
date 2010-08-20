/* 
 * 33-Pole Halfband filter built up from a 2-path allpass structure
 * Copyright © 2010, Tim Place
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#include "TTHalfbandLinear33.h"

#define thisTTClass			TTHalfbandLinear33
#define thisTTClassName		"halfband.linear.33"
#define thisTTClassTags		"audio, processor, filter, lowpass, highpass"

#ifdef TT_PLATFORM_WIN
#include <Algorithm>
#endif

TT_AUDIO_CONSTRUCTOR,
	mP0Delay(NULL),
	mP1Delay(NULL),
	mF0(NULL),
	mF1(NULL),
	mF2(NULL),
	mF3(NULL),
	mF4(NULL)
{
	TTUInt16	initialMaxNumChannels = arguments;
	TTErr		err;

	addAttributeWithSetter(Mode, kTypeSymbol);		
	addMessage(Clear);
	addMessageWithArgument(updateMaxNumChannels);

	err = TTObjectInstantiate(TT("delay"), (TTObjectPtr*)&mP0Delay, initialMaxNumChannels);
	
	err = TTObjectInstantiate(TT("allpass.1a"), (TTObjectPtr*)&mP1Delay, initialMaxNumChannels);
	err = TTObjectInstantiate(TT("allpass.1b"), (TTObjectPtr*)&mF0, initialMaxNumChannels);
	err = TTObjectInstantiate(TT("allpass.1b"), (TTObjectPtr*)&mF1, initialMaxNumChannels);
	err = TTObjectInstantiate(TT("allpass.2b"), (TTObjectPtr*)&mF2, initialMaxNumChannels);
	err = TTObjectInstantiate(TT("allpass.2b"), (TTObjectPtr*)&mF3, initialMaxNumChannels);
	err = TTObjectInstantiate(TT("allpass.2b"), (TTObjectPtr*)&mF4, initialMaxNumChannels);

	setAttributeValue(TT("MaxNumChannels"),	initialMaxNumChannels);
	setAttributeValue(TT("Mode"), TT("lowpass"));
	
	mP0Delay->setAttributeValue(TT("DelayMaxInSamples"), 16);
	mP0Delay->setAttributeValue(TT("DelayInSamples"), 16);
	
	mP1Delay->setAttributeValue(TT("Alpha"), 0.0);
	mF0->setAttributeValue(TT("Alpha"), 0.832280776);
	mF1->setAttributeValue(TT("Alpha"), -0.421241137);
	mF2->setAttributeValue(TT("C1"), 0.67623706);
	mF2->setAttributeValue(TT("C2"), 0.23192313);
	mF3->setAttributeValue(TT("C1"), 0.00359228);
	mF3->setAttributeValue(TT("C2"), 0.19159423);
	mF4->setAttributeValue(TT("C1"), -0.59689082);
	mF4->setAttributeValue(TT("C2"), 0.18016931);
}


TTHalfbandLinear33::~TTHalfbandLinear33()
{
	TTObjectRelease((TTObjectPtr*)&mP0Delay);

	TTObjectRelease((TTObjectPtr*)&mP1Delay);
	TTObjectRelease((TTObjectPtr*)&mF0);
	TTObjectRelease((TTObjectPtr*)&mF1);
	TTObjectRelease((TTObjectPtr*)&mF2);
	TTObjectRelease((TTObjectPtr*)&mF3);
	TTObjectRelease((TTObjectPtr*)&mF4);
}


TTErr TTHalfbandLinear33::updateMaxNumChannels(const TTValue& oldMaxNumChannels)
{
	// TODO: update internal filters
	Clear();
	return kTTErrNone;
}


TTErr TTHalfbandLinear33::Clear()
{
	// TODO: update internal filters
	return kTTErrNone;
}


TTErr TTHalfbandLinear33::setMode(const TTValue& newValue)
{
	TTSymbolPtr newMode = newValue;
	
	if (newMode == TT("highpass")) {
		mMode = TT("highpass");
		setCalculateMethod(calculateHighpass);
		setProcessMethod(processHighpass);
	}
	else { // lowpass
		mMode = TT("lowpass");
		setCalculateMethod(calculateLowpass);
		setProcessMethod(processLowpass);
	}
	return kTTErrNone;
}


inline void TTHalfbandLinear33::filterKernel(const TTFloat64& input, TTFloat64& outputPath0, TTFloat64& outputPath1, TTPtrSizedInt channel)
{
	TTFloat64 temp1, temp2;
	
	mP0Delay->calculateNoInterpolation(input, outputPath0, channel);

	mP1Delay->calculateValue(input,		temp2,			channel);
	mF0->calculateValue(temp2,			temp1,			channel);
	mF1->calculateValue(temp1,			temp2,			channel);
	mF2->calculateValue(temp2,			temp1,			channel);
	mF3->calculateValue(temp1,			temp2,			channel);
	mF4->calculateValue(temp2,			outputPath1,	channel);
}


TTErr TTHalfbandLinear33::calculateLowpass(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt channel)
{
	TTFloat64 outputFromPath0 = 0;
	TTFloat64 outputFromPath1;
	
	filterKernel(x, outputFromPath0, outputFromPath1, channel);
	y = (outputFromPath0 + outputFromPath1) * 0.5;
	return kTTErrNone;
}


TTErr TTHalfbandLinear33::calculateHighpass(const TTFloat64& x, TTFloat64& y, TTPtrSizedInt channel)
{
	TTFloat64 outputFromPath0;
	TTFloat64 outputFromPath1;
	
	filterKernel(x, outputFromPath0, outputFromPath1, channel);
	y = (outputFromPath0 - outputFromPath1) * 0.5;
	return kTTErrNone;
}


TTErr TTHalfbandLinear33::processLowpass(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs)
{
	TT_WRAP_CALCULATE_METHOD(calculateLowpass);
}


TTErr TTHalfbandLinear33::processHighpass(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs)
{
	TT_WRAP_CALCULATE_METHOD(calculateHighpass);
}
