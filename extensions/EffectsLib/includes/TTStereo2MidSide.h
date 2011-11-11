/* 
 * TTBlue Stereo to mid-side convertion object
 * Copyright © 2011, Trond Lossius
 * 
 * License: This code is licensed under the terms of the "New BSD License"
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __TT_STEREO2MIDSIDE_H__
#define __TT_STEREO2MIDSIDE_H__

#include "TTDSP.h"


/**	TTStereo2MidSide is an exceptionally simple audio processor scales an input audio signal */
class TTStereo2MidSide : public TTAudioObject {
	TTCLASS_SETUP(TTStereo2MidSide)

    /**	Setter for the inherited maxNumChannels attribute.		*/
	TTErr updateMaxNumChannels(const TTValue& oldMaxNumChannels, TTValue&);
    
	/**	A standard audio processing method as used by TTBlue objects.*/
	TTErr processAudio(TTAudioSignalArrayPtr inputs, TTAudioSignalArrayPtr outputs);

	virtual TTErr test(TTValue& returnedTestInfo);

};


#endif // __TT_STEREO2MIDSIDE_H__