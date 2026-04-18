/* 
 * ------------------------------------------------------------------------------
 * Reads from an analog channel with time stamping
 * This code is used with Analog_PyArduino_DAQ_therm_Driver.py, which will
 * read serial data and create a data file of (time,temperature)
 * 
 * ME 144L Version 1.0 (Spring 2026)
 * Comments: To use serial monitor, use the send box and input "f,50"
 * This will output data in serial monitor (free run)
 * To log data, use the Python code above.
 * ------------------------------------------------------------------------------
 */

const int PIN = A0;          // Read analog voltage from voltage divider circuit
unsigned long timer = 0;    // used to check current time [microseconds]
long loopTime = 0;       // default time between updates, but will be set in python Code [microseconds]
bool initLooptime = false;  // boolean (T/F) to check if loop time has already been set
bool stopProgram = false;

int analogVal = 0;          // variable to store A0 input [int]
float Rth = 0.0;          // thermistor resistance variable
float TthK = 0.0;         // temperature in Kelvin from Steinhart-Hart equation
float TthC = 0.0;         // temperature in Celsius (for output)
// Steinhart-Hart coefficients for NTC B3380-10K thermistor
float A = 0.00083028;
float B = 0.00026174;
float C = 0.000000146815;
float Ro = 10000.0; // value of the output resistor in ohms - can update for your setup

void setup() {
  Serial.begin(19200);         // Being serial comms and set Baud rate
  timer = micros();             // start timer
}
 
void loop() {
  if (Serial.available() > 0) {       // if data is available
    String str = Serial.readStringUntil('\n');
    readFromPC(str); 
  }
  if (initLooptime && !stopProgram) // once loop time has been initialized
  {
    // initLooptime 
    timeSync(loopTime);   // sync up time to mach data rate
    analogVal = analogRead(PIN); // get analog data from pin
    float Rth = 0; // NOTE: you need to put in the voltage-to-resistance relation here
    // Steinhart-Hart Equation (uses given coefficients)
    float TthK = 1.0 / (A + (B * log(Rth)) + (C * pow(log(Rth), 3)));
    // Convert Kelvin to Celsius
    float TthC = TthK - 273.15;
  
    unsigned long currT = micros();  // get current time
    // Send data over serial line to computer
    sendToPC(&currT);
    sendToPC(&TthC);
  }
  else if (initLooptime && stopProgram)
  {
    // also free run
    analogVal = analogRead(PIN); // get analog data from pin
    float Rth = 0;
    // Steinhart-Hart Equation
    float TthK = 1.0 / (A + (B * log(Rth)) + (C * pow(log(Rth), 3)));
    // Convert Kelvin to Celsius
    float TthC = TthK - 273.15;
    Serial.print(TthC);
    Serial.print('\n');
  }
}

/*
 * Timesync calculates the time the arduino needs to wait so it 
 * outputs data at the specified rate
 * Input: deltaT - the data transfer period in microseconds
 */
void timeSync(unsigned long deltaT)
{
  unsigned long currTime = micros();  // get current time
  long timeToDelay = deltaT - (currTime - timer); // calculate how much time to delay for [us]
  
  if (timeToDelay > 5000) // if time to delay is large 
  {
    // Split up delay commands into delay(milliseconds)
    delay(timeToDelay / 1000);

    // and delayMicroseconds(microseconds)
    delayMicroseconds(timeToDelay % 1000);
  }
  else if (timeToDelay > 0) // If time to delay is positive and small
  {
    // Use delayMicroseconds command
    delayMicroseconds(timeToDelay);
  }
  else
  {
      // timeToDelay is negative or zero so don't delay at all
  }
  timer = currTime + timeToDelay;
}


void readFromPC(const String input)
{
  int commaIndex = input.indexOf(',');
  char command = input.charAt(commaIndex - 1);
  String data = input.substring(commaIndex + 1);    
  int rate = 0;
  switch(command)
  {
    case 'r':
      // rate command
      rate = data.toInt();
      loopTime = 1000000/rate;         // set loop time in microseconds to 1/frequency sent
      initLooptime = true;             // no longer check for data
      timer = micros();
      break;
    case 's':
      // stop command
      stopProgram = true;
      break;
    case 'f':
      // free run
      initLooptime = true;
      stopProgram = true;
      break;
    default:
    // Otherwise, do nothing
      break;
  
  }

}

// ------------------------------------------------------------------------------------------------------------
// Send Data to PC: Methods to send different types of data to PC
// ------------------------------------------------------------------------------------------------------------

void sendToPC(int* data)
{
  byte* byteData = (byte*)(data);
  Serial.write(byteData, 2);
}

void sendToPC(float* data)
{
  byte* byteData = (byte*)(data);
  Serial.write(byteData, 4);
}
 
void sendToPC(double* data)
{
  byte* byteData = (byte*)(data);
  Serial.write(byteData, 4);
}

void sendToPC(unsigned long* data)
{
  byte* byteData = (byte*)(data);
  Serial.write(byteData, 4);
}
