// Based on code from https://microcontrollerslab.com/water-flow-sensor-pinout-interfacing-with-arduino-measure-flow-rate/

const int SENSOR_PIN = 2;
/* YF-S201 water Flow sensor code for Arduino */
const int PULSES_PER_LITER = 450; // https://www.seeedstudio.com/blog/2020/05/11/how-to-use-water-flow-sensor-with-arduino/

volatile int  Pulse_Count;
unsigned int  Liter_per_hour;
unsigned long Current_Time, Loop_Time;

void setup()
{ 
   pinMode(SENSOR_PIN, INPUT);
   Serial.begin(9600); 
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