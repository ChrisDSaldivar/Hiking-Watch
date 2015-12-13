//******************************************************************************
//
// Last Modified:   12/13/15
//
// Programmer:      Chris Saldivar
//
// Description:     Sketch for a hiking watch
//
// Input:           Reads from external clock, barometer and gyroscope
//
// Output:          Outputs time, temperature, altitude and direction to OLED
//
//
//                          BSD license
//******************************************************************************

#include <SPI.h>
#include <Wire.h>
#include <DS3231.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_BMP280.h>

DS3231 Clock;

bool h12 = false;
bool PM  = false;


void printDirection (float h);

// C++ style stream insertion
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

// Barometer 
Adafruit_BMP280 bme; // I2C

// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_RESET);

// compass
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);



#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   {        

  // Start the I2C interface
  Wire.begin();
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();

  // Clear the buffer.
  display.clearDisplay();

  Serial.println(F("BMP280 test"));
  
  if (!bme.begin()) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  mag.enableAutoRange(true);
  
  // Initialise the sensor 
  if(!mag.begin())
  {
    // There was a problem detecting the LSM303 ... check your connections 
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
}

void loop() {
  int h = Clock.getHour(h12, PM);
  // converts 24 hour clock to 12 hour clock
  h = h > 12 ? h-12 : h == 0 ? 12 : h;

  /* Get a new sensor event */ 
  sensors_event_t event; 
  mag.getEvent(&event);
  
  float Pi = 3.14159;
  
  // Calculate the angle of the vector y,x
  float heading = (atan2(event.magnetic.y,event.magnetic.x) * 180) / Pi;
  
  // Normalize to 0-360
  if (heading < 0)
  {
    heading = 360 + heading;
  }

  // Write temperature to the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(2);
  display.setCursor(85, 0);
  display << bme.readTemperature()*(9/5)+32 << "^F"; // temp in degrees F

  // Write direction to display
  display.setCursor(0, 0);
  printDirection(heading);

  // Write the time to display
  display.setTextSize(2);
  display.setCursor(43, 9);
  display << h << ":" << Clock.getMinute();

  // Write the current altitude to the display
  display.setTextSize(1);
  display.setCursor(8, 25);
  display << "Altitude: " << bme.readAltitude(1013.25) << " m";
  display.display();
  
}

/**
 * @brief get operands from stack
 *
 * @details Uses table from: https://en.wikipedia.org/wiki/Points_of_the_compass#32_cardinal_points
 *  
 * @param   h    current heading; 360 degrees
 *
 * @return     One of the 32 cardinal points
 */
void printDirection (float h){
    if (h >= 5.63  && h <= 16.87)
        display << "NbE";
    else if (h >= 16.88 && h <= 28.12)
        display << "NNE";
    else if (h >= 28.13 && h <= 39.37)
        display << "NEbN";
    else if (h >= 39.38 && h <= 50.62)
        display << "NE";
    else if (h >= 50.63 && h <= 61.87)
        display << "NEbE";
    else if (h >= 61.88 && h <= 73.12)
        display << "ENE";
    else if (h >= 73.13 && h <= 84.37)
        display << "EbN";
    else if (h >= 84.38 && h <= 95.62)
        display << "E";
    else if (h >= 95.63 && h <= 106.87)
        display << "EbS";
    else if (h >= 106.88 && h <= 118.12)
        display << "ESE";
    else if (h >= 118.13 && h <= 129.37)
        display << "SEbE";
    else if (h >= 129.38 && h <= 140.62)
        display << "SE";
    else if (h >= 140.63 && h <= 151.87)
        display << "SEbS";
    else if (h >= 151.88 && h <= 163.12)
        display << "SSE";
    else if (h >= 163.13 && h <= 174.37)
        display << "SbE";
    else if (h >= 174.38 && h <= 185.62)
        display << "S";
    else if (h >= 185.63 && h <= 196.87)
        display << "SbW";
    else if (h >= 196.88 && h <= 208.12)
        display << "SSW";
    else if (h >= 208.13 && h <= 219.37)
        display << "SWbS";
    else if (h >= 219.38 && h <= 230.62)
        display << "SW";
    else if (h >= 230.63 && h <= 241.87)
        display << "SWbW";
    else if (h >= 241.88 && h <= 253.12)
        display << "WSW";
    else if (h >= 253.13 && h <= 264.37)
        display << "WbS";
    else if (h >= 264.38 && h <= 275.62)
        display << "W";
    else if (h >= 275.63 && h <= 286.87)
        display << "WbN";
    else if (h >= 286.88 && h <= 298.12)
        display << "WNW";
    else if (h >= 298.13 && h <= 309.37)
        display << "NWbW";
    else if (h >= 309.38 && h <= 320.62)
        display << "NW";
    else if (h >= 320.63 && h <= 331.87)
        display << "NWbN";
    else if (h >= 331.88 && h <= 343.12)
        display << "NNW";
    else if (h >= 343.13 && h <= 354.37)
        display << "NbW";
    else
        display << "N";
}
