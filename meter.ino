// Based on code from https://microcontrollerslab.com/water-flow-sensor-pinout-interfacing-with-arduino-measure-flow-rate/

const int SENSOR_PIN = 2;
/* YF-S201 water Flow sensor code for Arduino */
const int PULSES_PER_LITER = 450; // https://www.seeedstudio.com/blog/2020/05/11/how-to-use-water-flow-sensor-with-arduino/

// TODO: 
// Add time of start
// total pulses for entire shower
// time of last pulse
// end of shower output 10s after last pulse

volatile int  Pulse_Count;
unsigned int  Liter_per_hour;
unsigned long Current_Time, Loop_Time;

void setup()
{ 
   pinMode(SENSOR_PIN, INPUT);
   Serial.begin(9600); 
	 // Most blog posts about this sensor use interrupt 0, but that is for the Arduino Uno.
	 // The Micro has additional digitial interrupt pins, so we use digitalPinToInterrupt(2) instead.
	 // More info here https://www.arduino.cc/en/Reference/AttachInterrupt
   attachInterrupt(digitalPinToInterrupt(2), Detect_Rising_Edge, RISING);
                                     
   Current_Time = millis();
   Loop_Time = Current_Time;
} 

void loop ()    
{
   Current_Time = millis();
   if(Current_Time >= (Loop_Time + 1000))
   {
      Loop_Time = Current_Time;
      Liter_per_hour = (Pulse_Count * 60 / 7.5);
      Pulse_Count = 0;
      Serial.print(Liter_per_hour, DEC);
      Serial.println(" Liter/hour");
   }
}
void Detect_Rising_Edge ()
{ 
   Pulse_Count++;
} 