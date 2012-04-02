#include "URreceiver.h"

// Pin mappings
#define PWR_LED 13
#define ACT_LED 12
#define IR_IN 2
#define PORT_ADDR2 5
#define PORT_ADDR1 6
#define PORT_ADDR0 4
#define DATA 3
#define CLK 7

// Ports
#define ADDR_PORT PORTD
#define ADDR_DDR DDRD
#define ADDR_SHIFT 4
#define ADDR_MASK 0b01110000

#define NUM_OUTPUTS 4

// Config
#define UPDATE_RATE 500


IRrecv irrecv(IR_IN);
decode_results results;

int output[4] = { 0 };

void setup() {
	int i;

	pinMode(PWR_LED, OUTPUT);
	pinMode(ACT_LED, OUTPUT);
	digitalWrite(PWR_LED, HIGH);
	digitalWrite(ACT_LED, LOW);

	ADDR_DDR |= ADDR_MASK;
	pinMode(DATA, OUTPUT);
	digitalWrite(DATA, LOW);
	pinMode(CLK, OUTPUT);
	digitalWrite(CLK, LOW);

	Serial.begin(115200);
	irrecv.enableIRIn();

	for ( i = 0; i < NUM_OUTPUTS; i += 1 ) {
		toggleOutput(i);
	}
}


void loop() {

	if ( Serial.available() ) {
		toggleOutput(Serial.read());
		while ( Serial.available() ) {
			Serial.read();
		}
	}

	if ( irrecv.decode(&results) != 0 ) {
		writeCode();
		delay(UPDATE_RATE);
		irrecv.resume();
	}
}


int portify(int n) {
	int x0, x1, x2;
	int r = 0;

	x0 = (n & 0b0001) >> 0;
	x1 = (n & 0b0010) >> 1;
	x2 = (n & 0b0100) >> 2;

	r += x0 * pow(2, PORT_ADDR0);
	r += x1 * pow(2, PORT_ADDR1);
	r += x2 * pow(2, PORT_ADDR2);

	return r;
}


void toggleOutput(char b) {
	int addr = b & 0b00111111;
	int value = (b >> 6) & 0b01;
	int toggle = (b >> 7) & 0b01;


	// Set output value
	if ( toggle ) {
		if ( value ) {
			output[addr] = ! output[addr];
		}
	} else {
		output[addr] = value;
	}

	// Set value on data pin
	digitalWrite(DATA, output[addr]);
	// Clear address pins
	ADDR_PORT &= ~ADDR_MASK;
	// Set address pins
	ADDR_PORT |= portify(addr);//  (addr >> ADDR_SHIFT) & ADDR_MASK;
	// Cycle clock
	digitalWrite(CLK, LOW);
	digitalWrite(CLK, HIGH);
	//delay(1);
}


void writeCode() {
	uint8_t a[4];
	int i;
	long value;
	value = results.value;
	for ( i = 3; i >= 0; i -= 1 ) {
		a[i] = value & 0x00FFL;
		value >>= 8;
	}
	Serial.write(a, 4);
	Serial.flush();
}
