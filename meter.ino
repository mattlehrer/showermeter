// Based on code from https://microcontrollerslab.com/water-flow-sensor-pinout-interfacing-with-arduino-measure-flow-rate/

const int SENSOR_PIN = 2;
const int GREEN_LED_PIN = 4;
const int RED_LED_PIN = 5;

// specific to the YF-S201 water flow sensor
// via https://www.seeedstudio.com/blog/2020/05/11/how-to-use-water-flow-sensor-with-arduino/
const int PULSES_PER_LITER = 450;

const int LONG_SHOWER_THRESHOLD = 25; // divide by PULSES_PER_LITER to get liters

volatile int Pulse_Count_in_Last_Second, Total_Pulse_Count;
unsigned int Liter_per_hour;
unsigned int Shower_Seconds, Shower_Minutes;
unsigned long Current_Time, Loop_Time, Start_Time, Most_Recent_Pulse_Time;

void setup()
{
	pinMode(SENSOR_PIN, INPUT);
	pinMode(GREEN_LED_PIN, OUTPUT);
	pinMode(RED_LED_PIN, OUTPUT);
	Serial.begin(9600);
	// Most blog posts about this sensor use interrupt 0, but that is for the Arduino Uno.
	// The Micro has additional digital interrupt pins, so we use digitalPinToInterrupt(2) instead.
	// More info here https://www.arduino.cc/en/Reference/AttachInterrupt
	attachInterrupt(digitalPinToInterrupt(2), Detect_Rising_Edge, RISING);

	Most_Recent_Pulse_Time = 0;
	Total_Pulse_Count = 0;
	digitalWrite(GREEN_LED_PIN, LOW);
	digitalWrite(RED_LED_PIN, LOW);
}

void loop()
{
	Current_Time = millis();
	if (Current_Time >= (Loop_Time + 1000))
	{
		Loop_Time = Current_Time;

		if (Total_Pulse_Count > 0)
		{ // Only send data if a shower is currently active
			Serial.print(float(Pulse_Count_in_Last_Second) / 7.5);
			Serial.println(" Liter/min");
			Pulse_Count_in_Last_Second = 0;
		}

		if (Total_Pulse_Count > LONG_SHOWER_THRESHOLD)
		{ // this is now a long shower, switch from green to red LED
			digitalWrite(RED_LED_PIN, HIGH);
			digitalWrite(GREEN_LED_PIN, LOW);
		}
		else if (Total_Pulse_Count > 0)
		{
			digitalWrite(GREEN_LED_PIN, HIGH);
		}
	}

	// Check for end of shower
	if (Most_Recent_Pulse_Time > 0 && Current_Time >= (Most_Recent_Pulse_Time + 10000))
	{
		Serial.println("10 seconds have elapsed since last water flow reading");
		Serial.println("Full shower statistics:");
		Serial.print("Total pulses: ");
		Serial.println(Total_Pulse_Count);
		Serial.print("Total liters: ");
		// float Total_Liters = (float)Total_Pulse_Count / (float)PULSES_PER_LITER;
		// Serial.print(Total_Liters);
		Serial.print((float)Total_Pulse_Count / (float)PULSES_PER_LITER);
		Serial.println("L");

		Shower_Seconds = (Most_Recent_Pulse_Time - Start_Time) / 1000;
		Shower_Minutes = Shower_Seconds / 60;

		Serial.print("Total time: ");
		Serial.print(Shower_Minutes);
		Serial.print(":");
		// add 0 if single digit seconds
		if (Shower_Seconds % 60 < 10)
		{
			Serial.print("0");
		}
		Serial.println(Shower_Seconds % 60);
		Serial.println("*********");

		// Reset all variables for next shower
		Start_Time = 0;
		Most_Recent_Pulse_Time = 0;
		Pulse_Count_in_Last_Second = 0;
		Total_Pulse_Count = 0;
		digitalWrite(GREEN_LED_PIN, LOW);
		digitalWrite(RED_LED_PIN, LOW);
	}
}

void Detect_Rising_Edge()
{
	if (Total_Pulse_Count == 0)
	{ // if this is the first pulse of a shower, start the timer
		Start_Time = millis();
		Serial.println("*********");
		Serial.println("Starting a new shower");
	}
	Pulse_Count_in_Last_Second++;
	Total_Pulse_Count++;
	Most_Recent_Pulse_Time = millis();
}