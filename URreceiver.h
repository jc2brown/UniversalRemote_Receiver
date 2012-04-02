#ifndef RemoteRcv_H_
#define RemoteRcv_H_

#include "Arduino.h"

#include "IRremote/IRremote.h"


#ifdef __cplusplus
	extern "C" {
#endif
	void loop();
	void setup();
#ifdef __cplusplus
	}
#endif

// Prototypes


void toggleOutput(char addr);
void writeCode();



#endif


