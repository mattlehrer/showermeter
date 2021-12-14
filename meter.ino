// Based on code from https://microcontrollerslab.com/water-flow-sensor-pinout-interfacing-with-arduino-measure-flow-rate/

const int SENSOR_PIN = 2;
/* YF-S201 water Flow sensor code for Arduino */
const int PULSES_PER_LITER = 450; // https://www.seeedstudio.com/blog/2020/05/11/how-to-use-water-flow-sensor-with-arduino/

volatile int Pulse_Count, Total_Pulse_Count;
unsigned int Liter_per_hour;
unsigned int Shower_Seconds, Shower_Minutes;
unsigned long Current_Time, Loop_Time, Start_Time, Most_Recent_Pulse_Time;

void setup()
{
	pinMode(SENSOR_PIN, INPUT);
	Serial.begin(9600);
	// Most blog posts about this sensor use interrupt 0, but that is for the Arduino Uno.
	// The Micro has additional digital interrupt pins, so we use digitalPinToInterrupt(2) instead.
	// More info here https://www.arduino.cc/en/Reference/AttachInterrupt
	attachInterrupt(digitalPinToInterrupt(2), Detect_Rising_Edge, RISING);

	Current_Time = millis();
	Start_Time = Current_Time;
	Loop_Time = Current_Time;
	Most_Recent_Pulse_Time = 0;
	Total_Pulse_Count = 0;
}

void loop()
{
	Current_Time = millis();
	if (Current_Time >= (Loop_Time + 1000))
	{
		Loop_Time = Current_Time;
		Liter_per_hour = (Pulse_Count * 60 / 7.5);
		Pulse_Count = 0;
		Serial.print(Liter_per_hour, DEC);
		Serial.println(" Liter/hour");
	}

	// Check for end of shower
	if (Most_Recent_Pulse_Time > 0 && Current_Time >= (Most_Recent_Pulse_Time + 10000))
	{
		Serial.println("10 seconds elapsed since last pulse");
		Serial.println("Full shower statistics:");
		Serial.print("Total pulses: ");
		Serial.println(Total_Pulse_Count);
		Serial.print("Total liters: ");
		float totalLiters = (float)Total_Pulse_Count / (float)PULSES_PER_LITER;
		Serial.print(totalLiters);
		Serial.println("L");

		Shower_Seconds = (Most_Recent_Pulse_Time - Start_Time) / 1000;
		Shower_Minutes = Shower_Seconds / 60;

		Serial.print("Total time: ");
		Serial.print(Shower_Minutes);
		Serial.print(":");
		if (Shower_Seconds % 60 < 10)
		{
			Serial.print("0");
		}
		Serial.println(Shower_Seconds % 60);

		// Reset all variables for next shower
		Start_Time = millis();
		Pulse_Count = 0;
		Most_Recent_Pulse_Time = 0;
	}
}
void Detect_Rising_Edge()
{
	Pulse_Count++;
	Total_Pulse_Count++;
	Most_Recent_Pulse_Time = millis();
}