//Libraries
#include "U8glib.h"
#include <Wire.h>
#include <EasyTransferI2C.h>

//Global variables
char battSoC_conv[5];
char carSpeed_conv[5];
char motorSpeed_conv[5];
char battTemp1_conv[5];
char battTemp2_conv[5];
char distTravelled_conv[5];
char motorTemp_conv[5];

//Create object
EasyTransferI2C ET; 

struct RECEIVE_DATA_STRUCTURE{
  //Variables to be sent are declared here
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int battSoC;
  int carSpeed;
  double motorSpeed;
  float motorCurrent;
  float battTemp1;
  float battTemp2;
  int distTravelled;
  float motorTemp;
};

//Name the group of data
RECEIVE_DATA_STRUCTURE mydata;

//Define slave i2c address
#define I2C_SLAVE_ADDRESS 9

//Constants
#define motor_current_thres 10.0
#define motor_temp_thres 45.0
#define batt_SoC_thres 15

//Setup u8g object
U8GLIB_ST7920_128X64_4X u8g(13, 11, 10, U8G_PIN_NONE); // SPI Com: SCK = en = 13, MOSI = rw = 11, CS = di = 0

//Procedure to display icons and texts on GLCD
void draw(void) {  
  //Initialize fonts
  u8g.setFont(u8g_font_5x7);
  
  //Draw battery icon
  u8g.drawFrame( 0, 0, 12, 23 );
  u8g.drawBox( 2, 2, 8, 4 ); //top bar
  u8g.drawBox( 2, 7, 8, 4 ); //second bar
  u8g.drawBox( 2, 12, 8, 4 ); //third bar
  u8g.drawBox( 2, 17, 8, 4 ); //bottom bar
  
  //Battery SoC value
  u8g.drawStr( 14, 7, battSoC_conv);
  u8g.drawStr( 29, 7, "%");

  //Car speed
  u8g.setFont(u8g_font_6x13);
  u8g.drawStr( 33, 32, carSpeed_conv);
  u8g.setFont(u8g_font_5x7);
  u8g.drawStr( 33, 41, "km/h");

  //Motor speed
  u8g.setFont(u8g_font_6x13);
  u8g.drawStr( 73, 32, motorSpeed_conv);
  u8g.setFont(u8g_font_5x7);
  u8g.drawStr( 73, 41, "RPM");

  //Battery temperature #1
  u8g.drawStr( 0, 55, "Batt1(C)");
  u8g.drawStr( 0, 63, battTemp1_conv);
  u8g.drawStr( 21, 63, "C");
  
  //Battery temperature #2
  u8g.drawStr( 42, 55, "Batt2(C)");
  u8g.drawStr( 42, 63, battTemp2_conv);
  u8g.drawStr( 63, 63, "C");
  
  //Motor temperature
  u8g.drawStr( 87, 55, "Motor(C)");
  u8g.drawStr( 87, 63, motorTemp_conv);
  u8g.drawStr( 108, 63, "C"); 
}

void toggleWarning(void) {
  //Overcurrent - Red LED #1 turns ON
  if (mydata.motorCurrent > motor_current_thres) {
    digitalWrite(7, HIGH);  
  }

  //Motor Overheat - Red LED #2 turns ON
  if (mydata.motorTemp > motor_temp_thres) {
    digitalWrite(6, HIGH);
  }

  //Low Battery - Red LED #3 turns ON
  if (mydata.battSoC <= batt_SoC_thres) {
    digitalWrite(5, HIGH);
  }
  
}

//Handler function
void receive(int numBytes) {}

//Procedure to convert received data into displayable strings
void convertData(void) {
  itoa(mydata.battSoC, battSoC_conv, 10);
  itoa(mydata.carSpeed, carSpeed_conv, 10);
  dtostrf(mydata.motorSpeed, 3, 1, motorSpeed_conv);
  dtostrf(mydata.battTemp1, 3, 1, battTemp1_conv);
  dtostrf(mydata.battTemp2, 3, 1, battTemp2_conv);
  itoa(mydata.distTravelled, distTravelled_conv, 10);
  dtostrf(mydata.motorTemp, 3, 1, motorTemp_conv);
}

void setup(void) {
  //Initialize communication
  Wire.begin(I2C_SLAVE_ADDRESS);        //Join I2C bus (address optional for master)
  
  //Start the library and pass in data details
  ET.begin(details(mydata), &Wire);
  
  //Define handler function on receiving data
  Wire.onReceive(receive);
  Serial.begin(9600);
  
  //Assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white  
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity 
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }

  //Setup output pins
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);  
}

void loop(void) {
  //Check to see if a data packet has been received
  if (ET.receiveData()) {
    toggleWarning();
    convertData();

    //Display data on LCD
    u8g.firstPage();  
    do {
      draw();
    } while( u8g.nextPage() );

    //Rebuild the picture after some delay
    delay(200);
  }
}
