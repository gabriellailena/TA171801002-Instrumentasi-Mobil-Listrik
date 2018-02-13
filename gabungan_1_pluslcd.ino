// Libraries
/*******library GPS***************/
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <TinyGPS.h>


/**********Temperatur Library**************/
#include <OneWire.h>
#include <DallasTemperature.h>


/**********Librari kirim data**************/
#include <Wire.h>
#include <EasyTransferI2C.h>

/*******library sdCard*************/
#include <SPI.h>
#include <SD.h> //include the SD library

/***********************************Inisialisasi GPS*****************************************/
TinyGPS gps;
SoftwareSerial ss(4, 3);
// Offset hours from gps time (UTC)
const int offset = 7;   // Central European 
time_t prevDisplay = 0; // when the digital clock was displayed

bool connect_gps;
float flat, flon;
unsigned long age;
int Year;
byte Month, Day, Hour, Minute, Second;

/***********************************Inisialisasi sdCard*****************************************/

bool sdInitSuccess = true; //card init status
File myFile;


/*************************Inisialisasi I2C Communication*****************************************/
// Create object
EasyTransferI2C ET; 

struct SEND_DATA_STRUCTURE{
  // Variables to be sent are declared here
  // THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int battSoC;
  int carSpeed;
  double motorSpeed;
  float motorCurrent;
  float battTemp1;
  float battTemp2;
  int distTravelled;
  float motorTemp;
};

// Name the group of data
SEND_DATA_STRUCTURE mydata;

// Define slave i2c address
#define I2C_SLAVE_ADDRESS 9

/*************************Inisiasi Sensor Temperatur*****************************************/
// Data wire is plugged into pin 2 on the Arduino
int ONE_WIRE_BUS = 6;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


/*************************Inisiasi Sensor Tegangan DC*****************************************/
int batMonPin1 = A4;    // input pin for the divider
int val1 = 0;       // variable for the A/D value
float pinVoltage1 = 0; // variable to hold the calculated voltage
float batteryVoltage1 = 0;
float ratio1 = 5.94903;  // Change this to match the MEASURED ration of the circuit

/*
int batMonPin2 = A5;    // input pin for the divider
int val2 = 0;       // variable for the A/D value
float pinVoltage2 = 0; // variable to hold the calculated voltage
float batteryVoltage2 = 0;
float ratio2 = 3.075;  // Change this to match the MEASURED ration of the circuit
*/

/*************************Inisiasi Sensor Arus DC*****************************************/
const int analogIn = A0;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
int RawValue= 0;
int ACSoffset = 2500; 
double Voltage = 0;
double Amps = 0;

/*************************Inisiasi Sensor Arus AC*****************************************/
const unsigned int numReadings = 200; //samples to calculate Vrms.
int readingsVClamp[numReadings];    // samples of the sensor SCT-013-000
int readingsGND[numReadings];      // samples of the virtual ground
float SumSqGND = 0;            
float SumSqVClamp = 0;
float total = 0; 
  
int PinVClamp = A2;    // Sensor SCT-013-000
int PinVirtGND = A1;   // virtual ground
 
/**************************Inisiasi Tachogenerator***************************************/
int PIN_Tacho = A3;    // input pin for the divider
int Val_Tacho = 0;       // variable for the A/D value
float Tacho_Voltage = 0; // variable to hold the calculated voltage
float Tacho_Voltage_Real = 0;
float Ratio_Tacho = 3.125475285;  // Change this to match the MEASURED ration of the circuit

const double ktcg = 0.019580104; // Koefisien Tachogenerator
const double pi = 22/7;
double tacho_rpm;
double motor_rpm;
double tacho_rad;

 
 /********************************************************Setup*****************************************************************/
void setup() {

  
  Serial.begin(115200);      // open the serial port at 9600 baud
  
  /****************************************setup GPS********************************************************************/
  ss.begin(9600);
  
  /****************************************setup memori********************************************************************/
  pinMode(53, OUTPUT);
  SD.begin(53);
  
  /*
  if (!SD.begin(53)) { //using pin 53 (SS)
          Serial.println("Initialization failed!\n");
          sdInitSuccess = false; //failure
          return;
        }
   else {
          Serial.println("Intitialization success.");
          Serial.println();
          sdInitSuccess = true;
   }
   */
   

  /****************************************setup Initialize I2C communication********************************************************************/
    //Initialize I2C communication
   Wire.begin();
   
  //Start the library and pass in data details
  ET.begin(details(mydata), &Wire);

  /****************************************setup sensor suhu********************************************************************/
  sensors.begin();
  
  /****************************************setup sensor arus ac********************************************************************/
  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readingsVClamp[thisReading] = 0;
    readingsGND[thisReading] = 0;
  }
  /*******************************************************************************************************************************/

  
}

/*****************************************************main program*****************************************************************/
void loop() {  
  sensor_GPS();
  sensor_suhu();
  sensor_tegangan_dc();
  sensor_arus_dc();
  sensor_arus_ac();
  sensor_tacho(); 
  kirim_data();
  tampilan_serial();
  simpan_sdCard();
}

/****************************************GPS******************************************/
void sensor_GPS (){

  bool newData = false;
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    connect_gps = true;
    gps.f_get_position(&flat, &flon);
    gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);

    if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);
    }
    
    if (timeStatus()!= timeNotSet) {
      if (now() != prevDisplay) { //update the display only if the time has changed
        prevDisplay = now();
        
      }
    }
  }
  else {
    connect_gps = false;
  }
}

void tampilkan_serial_gps(){
  if (connect_gps){
        // digital clock display of the time
        Serial.print(hour());
        //printDigits(minute());
        Serial.print(":");
        if(minute() < 10)
        Serial.print('0');
        Serial.print(minute());
        
        //printDigits(second());
        Serial.print(":");
        if(second() < 10)
        Serial.print('0');
        Serial.print(second());
        Serial.print(" ");
        
        Serial.print(day());
        Serial.print("/");
        Serial.print(month());
        Serial.print("/");
        Serial.print(year());
        Serial.print("  ");

        Serial.print("LAT=");
        Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        Serial.print(" LON=");
        Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
        Serial.print(" ");
  }
  else{
  Serial.print("invalid gps");
  Serial.print(" ");
  }
}

void simpan_memori_gps(){
  if (connect_gps){
        // digital clock display of the time
        myFile.print(hour());
        //printDigits(minute());
        myFile.print(":");
        if(minute() < 10)
        myFile.print('0');
        myFile.print(minute());
        
        //printDigits(second());
        myFile.print(":");
        if(second() < 10)
        myFile.print('0');
        myFile.print(second());
        myFile.print(" ");
        
        myFile.print(day());
        myFile.print("/");
        myFile.print(month());
        myFile.print("/");
        myFile.print(year());
        myFile.print("  ");

        myFile.print("LAT=");
        myFile.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
        myFile.print(" LON=");
        myFile.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
        myFile.print(" ");
  }
  else{
  myFile.print("invalid gps");
  myFile.print(" ");
  }
}


/*****************************Sensor Suhu DSB18b20********************************/
void sensor_suhu(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  
  delay(200);
}


/*****************************Sensor tegangan DC********************************/
void sensor_tegangan_dc (){
  val1 = analogRead(batMonPin1);    // read the voltage on the divider  
  
  pinVoltage1 = val1 * 0.00488;    //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin.                                  
  batteryVoltage1 = pinVoltage1 * ratio1; //  Use the ratio calculated for the voltage divider
                                          //  to calculate the battery voltage
  delay(200);                  //  Slow it down
}

/*****************************Sensor Arus DC********************************/
void sensor_arus_dc() {
     RawValue = analogRead(analogIn);
     Voltage = (RawValue / 1024.0) * 5000; // Gets you mV
     Amps = ((Voltage - ACSoffset) / mVperAmp);
     
     delay(200); 
}

/***********************************Sensor arus ac*********************************************************/
void sensor_arus_ac() {
  unsigned int i=0;
  SumSqGND = 0;
  SumSqVClamp = 0;
  total = 0; 
   
  for (unsigned int i=0; i<numReadings; i++)
  {
    readingsVClamp[i] = analogRead(PinVClamp) - analogRead(PinVirtGND);
    delay(1); // 
  }
 
  //Calculate Vrms
  for (unsigned int i=0; i<numReadings; i++)
  {
    SumSqVClamp = SumSqVClamp + sq((float)readingsVClamp[i]);
 
  }
  total = sqrt(SumSqVClamp/numReadings);
  total= (total*(float)0.016); // Rburden=3300 ohms, LBS= 0,004882 V (5/1024)
                             // Transformer of 2000 laps (SCT-013-000).
                             // 5*24.93*2000/(3300*1024)= 2/3 (aprox)
  delay(200);  
}

/***********************************Tachogenerator*********************************************************/
void sensor_tacho(){
  Val_Tacho = analogRead(PIN_Tacho);    // read the voltage on the divider  
  
  
  Tacho_Voltage = Val_Tacho * 0.00488;
                                     //  Calculate the voltage on the A/D pin
                                    //  A reading of 1 for the A/D = 0.0048mV
                                    //  if we multiply the A/D reading by 0.00488 then 
                                    //  we get the voltage on the pin.                                  
                                    
                                    
  
  Tacho_Voltage_Real = Tacho_Voltage * Ratio_Tacho;
                                    //  Use the ratio calculated for the voltage divider
                                    //  to calculate the battery voltage
  

  tacho_rad = Tacho_Voltage_Real/ktcg;

  tacho_rpm = tacho_rad*60/(2*pi);
  
  motor_rpm = tacho_rpm*0.4542857;

  delay(200);
}


/***********************************Kirim Data*********************************************************/
void kirim_data(){
  //Copy variable values to struct data to be sent
   mydata.battSoC = 100;
   mydata.carSpeed = 75;
   mydata.motorSpeed = motor_rpm;
   mydata.motorCurrent = total;
   mydata.battTemp1 = sensors.getTempCByIndex(0);
   mydata.battTemp2 = sensors.getTempCByIndex(1);
   mydata.distTravelled = 1753;
   mydata.motorTemp = sensors.getTempCByIndex(2);
   delay(200);
    
  
  //Send data to receiver
 ET.sendData(I2C_SLAVE_ADDRESS);
}

/***********************************tampilan serial*********************************************************/
void tampilan_serial(){
  tampilkan_serial_gps();
  Serial.print(" Suhu_bat1: "); Serial.print(sensors.getTempCByIndex(0)); //0 refers to the first IC on the wire
  Serial.print("  Suhu_bat2: "); Serial.print(sensors.getTempCByIndex(1));
  Serial.print("  Suhu_motor: "); Serial.print(sensors.getTempCByIndex(2));

  Serial.print("\t  V_bat1: "); Serial.print(batteryVoltage1);

  Serial.print("\t Arus dc = "); Serial.print(Amps);

  Serial.print("\t  Arus ac= "); Serial.print(total);

  Serial.print("\t  RPM motor : "); Serial.println(motor_rpm);

  delay(200);
}

void simpan_sdCard(){
  if (sdInitSuccess) { //proceed only if card is initialized
        myFile = SD.open("TEST.txt", FILE_WRITE);
        
        if (myFile) {
          simpan_memori_gps();
          myFile.print(" Suhu_bat1: "); myFile.print(sensors.getTempCByIndex(0)); //0 refers to the first IC on the wire
          myFile.print("  Suhu_bat2: "); myFile.print(sensors.getTempCByIndex(1));
          myFile.print("  Suhu_motor: "); myFile.print(sensors.getTempCByIndex(2));
        
          myFile.print("\t  V_bat1: "); myFile.print(batteryVoltage1);
        
          myFile.print("\t Arus dc = "); myFile.print(Amps);
        
          myFile.print("\t  Arus ac= "); myFile.print(total);
        
          myFile.print("\t  RPM motor : "); myFile.println(motor_rpm); 

          myFile.close(); //this writes to the card
        }
        else { //else show error
          Serial.println("Error opening file.\n");
        }
  }
}


