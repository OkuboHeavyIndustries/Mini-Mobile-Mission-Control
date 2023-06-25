//25th June 2023
//QtPy 3D Globe Dual Screen ISS Tracker by Okubo Heavy Industries based on
//3D_Cube for Arduino OLED module by Colin Ord, 9/1/2015

#include <U8g2lib.h>
#include <AioP13.h>
#include <TinyGPS++.h> 
#include "globe_coords.h"
#include <Time.h>
#include "ArduinoLowPower.h"
#include <Adafruit_NeoPixel.h>



//The following lines will need changing depending on your screen type!
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED_2(U8G2_R0, 3, 2, /* reset=*/ U8X8_PIN_NONE);

//globe variables
float tx, nx, p;
float ty, ny, py;
float rot, rotx, roty, rotz, rotxx, rotyy, rotzz, rotxxx, rotyyy, rotzzz;
int i; //0 to 360
int fl, scale; //focal length
int wireframe[250][2];
float vector;

int yangle;
int zangle;
float yrot;
float zrot;
int w;

float scalefactor = 0;

int originx = 64;
int originy = 32; //32

int fd = 0; //0=orthographic

//Satellite variables

/* copy the updated information below from https://celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=tle
ISS (ZARYA)             
1 25544U 98067A   23175.81366287  .00019375  00000+0  34217-3 0  9993
2 25544  51.6423 290.7258 0004694  72.8718  63.6102 15.50361685402972
*/
//update the TLE constants regularly or the calculated position will be wrong
const char *tleName = "ISS (ZARYA)";
const char *tlel1   = "1 25544U 98067A   23175.81366287  .00019375  00000+0  34217-3 0  9993";
const char *tlel2   = "2 25544  51.6423 290.7258 0004694  72.8718  63.6102 15.50361685402972";

const char  *pcMyName = "OHI";    // Observer name


double       dfreqRX  = 137.100;     // Nominal downlink frequency
double       dfreqTX  = 437.800;     // Nominal uplink frequency


double       dSatLAT  = 0;           // Satellite latitude
double       dSatLON  = 0;           // Satellite longitude
double       dSatAZ   = 0;           // Satellite azimuth
double       dSatEL   = 0;           // Satellite elevation

double       dSatLATb  = 0;           // Satellite latitude
double       dSatLONb  = 0;           // Satellite longitude
double       dSatAZb   = 0;           // Satellite azimuth
double       dSatELb   = 0;           // Satellite elevation


double       dSunLAT  = 0;           // Sun latitude
double       dSunLON  = 0;           // Sun longitude
double       dSunAZ   = 0;           // Sun azimuth
double       dSunEL   = 0;           // Sun elevation

char         acBuffer[20];            // Buffer for ASCII time

static const uint32_t GPSBaud = 38400;

int xlon;
int ylat;

int xlonb;
int ylatb;

int orb;
int gpschar;

int radius = 74;  //ISS orbital radius - should be about 69

double lat_val, lng_val, alt_m_val;
uint8_t hr_val, min_val, sec_val, hr_val_jp, sats_val;
bool loc_valid, alt_valid, time_valid, sats_valid;
double year_val;
double month_val;
double day_val;
double dMyLAT;
double dMyLON;
double dMyALT;

char time_string[32];

TinyGPSPlus gps;

Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL);

void setup(void)
{

  Serial1.begin(GPSBaud);
  
 u8g2.begin();

 u8g2.firstPage();
 do {
 } while ( u8g2.nextPage() );

 OLED_2.begin();
}

void draw_vertices(void)
{
 u8g2.drawPixel (rotxxx, rotyyy);
}

void draw_wireframe(void)
{
 
 u8g2.drawLine(wireframe[0][0], wireframe[0][1], wireframe[1][0], wireframe[1][1]);
 u8g2.drawLine(wireframe[1][0], wireframe[1][1], wireframe[2][0], wireframe[2][1]);
 u8g2.drawLine(wireframe[2][0], wireframe[2][1], wireframe[3][0], wireframe[3][1]);
 u8g2.drawLine(wireframe[3][0], wireframe[3][1], wireframe[0][0], wireframe[0][1]);//greenland
 u8g2.drawLine(wireframe[4][0], wireframe[4][1], wireframe[5][0], wireframe[5][1]); 
 u8g2.drawLine(wireframe[5][0], wireframe[5][1], wireframe[6][0], wireframe[6][1]); 
 u8g2.drawLine(wireframe[6][0], wireframe[6][1], wireframe[4][0], wireframe[4][1]);//iceland
 u8g2.drawLine(wireframe[7][0], wireframe[7][1], wireframe[8][0], wireframe[8][1]);
 u8g2.drawLine(wireframe[8][0], wireframe[8][1], wireframe[9][0], wireframe[9][1]);
 u8g2.drawLine(wireframe[9][0], wireframe[9][1], wireframe[10][0], wireframe[10][1]);
 u8g2.drawLine(wireframe[10][0], wireframe[10][1], wireframe[11][0], wireframe[11][1]);
 u8g2.drawLine(wireframe[11][0], wireframe[11][1], wireframe[12][0], wireframe[12][1]);
 u8g2.drawLine(wireframe[12][0], wireframe[12][1], wireframe[13][0], wireframe[13][1]);
 u8g2.drawLine(wireframe[13][0], wireframe[13][1], wireframe[14][0], wireframe[14][1]);
 u8g2.drawLine(wireframe[14][0], wireframe[14][1], wireframe[15][0], wireframe[15][1]);
 u8g2.drawLine(wireframe[15][0], wireframe[15][1], wireframe[16][0], wireframe[16][1]);
 u8g2.drawLine(wireframe[16][0], wireframe[16][1], wireframe[17][0], wireframe[17][1]);
 u8g2.drawLine(wireframe[17][0], wireframe[17][1], wireframe[18][0], wireframe[18][1]);
 u8g2.drawLine(wireframe[18][0], wireframe[18][1], wireframe[19][0], wireframe[19][1]);
 u8g2.drawLine(wireframe[19][0], wireframe[19][1], wireframe[20][0], wireframe[20][1]);
 u8g2.drawLine(wireframe[20][0], wireframe[20][1], wireframe[21][0], wireframe[21][1]);
 u8g2.drawLine(wireframe[21][0], wireframe[21][1], wireframe[7][0], wireframe[7][1]);//Britain
 u8g2.drawLine(wireframe[22][0], wireframe[22][1], wireframe[23][0], wireframe[23][1]);
 u8g2.drawLine(wireframe[23][0], wireframe[23][1], wireframe[24][0], wireframe[24][1]);
 u8g2.drawLine(wireframe[24][0], wireframe[24][1], wireframe[25][0], wireframe[25][1]);
 u8g2.drawLine(wireframe[25][0], wireframe[25][1], wireframe[26][0], wireframe[26][1]);
 u8g2.drawLine(wireframe[26][0], wireframe[26][1], wireframe[22][0], wireframe[22][1]);//Ireland

 for (w = 27; w < 84; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[84][0], wireframe[84][1], wireframe[37][0], wireframe[27][1]);//Europe, Asia, Africa

 for (w = 85; w < 117; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[117][0], wireframe[117][1], wireframe[85][0], wireframe[85][1]);//Americas

 for (w = 118; w < 125; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[125][0], wireframe[125][1], wireframe[118][0], wireframe[118][1]);//Australia

 for (w = 126; w < 128; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[128][0], wireframe[128][1], wireframe[126][0], wireframe[126][1]);//Madagascar

 for (w = 129; w < 133; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[133][0], wireframe[133][1], wireframe[129][0], wireframe[129][1]);//Japan

 for (w = 134; w < 136; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[136][0], wireframe[136][1], wireframe[134][0], wireframe[134][1]);//Borneo

 for (w = 137; w < 140; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[140][0], wireframe[140][1], wireframe[137][0], wireframe[137][1]);//New Zealand

 for (w = 141; w < 152; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[152][0], wireframe[152][1], wireframe[141][0], wireframe[141][1]);//Antarctica

 for (w = 154; w < 156; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[156][0], wireframe[156][1], wireframe[154][0], wireframe[154][1]);//Cuba

 for (w = 157; w < 159; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[159][0], wireframe[159][1], wireframe[157][0], wireframe[157][1]);//Dominica 

 for (w = 161; w < 164; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
 }
 u8g2.drawLine(wireframe[164][0], wireframe[164][1], wireframe[161][0], wireframe[161][1]);//Dominica 

 for (w = 170; w < 189; w++){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]); //ISS Orbital Path
 }

  u8g2.drawCircle(originx,originy,65*scalefactor);
  u8g2.setFont(u8g2_font_u8glib_4_tr);
  u8g2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders

  u8g2.drawCircle(wireframe[169][0],wireframe[169][1],4);
  u8g2.drawDisc(wireframe[169][0],wireframe[169][1],2);
  if (wireframe[169][1] >32){
  u8g2.setCursor(wireframe[169][0],wireframe[169][1]-6);
  }
  else {
    u8g2.setCursor(wireframe[169][0],wireframe[169][1]+10);
  }
  u8g2.print("ISS");
    
}

void loop(void)
{

while (Serial1.available()>0){  
  gpschar = Serial1.read(); //read raw gps data to gpschar
  gps.encode(gpschar);      // extract useful info from raw gps data
  }

lat_val = gps.location.lat();  /* Get latitude data */
loc_valid = gps.location.isValid(); /* Check if valid location data is available */
lng_val = gps.location.lng(); /* Get longtitude data */
alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
hr_val = gps.time.hour(); /* Get hour */
min_val = gps.time.minute();  /* Get minutes */
sec_val = gps.time.second();  /* Get seconds */
time_valid = gps.time.isValid();  /* Check if valid time data is available */
year_val = (gps.date.year());
month_val = (gps.date.month());
day_val = (gps.date.day());
sats_valid = gps.satellites.isValid();
sats_val = gps.satellites.value();

if (hr_val < 15) { hr_val_jp = hr_val + 9;  // convert UTC to Japan time zone
     }
     else {hr_val_jp = hr_val -15;
     }

dMyLAT = gps.location.lat();
dMyLON = gps.location.lng();
dMyALT = gps.altitude.meters();

//P13Sun Sun;                                                       // Create object for the sun
P13DateTime MyTime(year_val, month_val, day_val, hr_val, min_val, sec_val); // Set start time for the prediction
P13Observer MyQTH(pcMyName, dMyLAT, dMyLON, dMyALT);              // Set observer coordinates
P13Satellite MySAT(tleName, tlel1, tlel2);                        // Create ISS data from TLE
MyTime.ascii(acBuffer);             // Get time for prediction as ASCII string
MySAT.predict(MyTime);              // Predict ISS for specific time
MySAT.latlon(dSatLAT, dSatLON);     // Get the rectangular coordinates
MySAT.elaz(MyQTH, dSatEL, dSatAZ);  // Get azimut and elevation for MyQTH
globe_vertices[169][0] = radius * cos (dSatLAT*PI/180) * cos (-dSatLON*PI/180);
globe_vertices[169][1] = radius * cos (dSatLAT*PI/180) * sin (-dSatLON*PI/180);
globe_vertices[169][2] = radius * sin (dSatLAT*PI/180);

//Full orbit vertices
orb = 170;
for (int delta = -50; delta < 50; delta = delta + 5) {
P13DateTime MyTime2(year_val, month_val, day_val, hr_val, (min_val + delta) , sec_val); // Set start time for the prediction
P13Observer MyQTH(pcMyName, dMyLAT, dMyLON, dMyALT);              // Set observer coordinates
P13Satellite MySAT2(tleName, tlel1, tlel2);                        // Create ISS data from TLE
//MyTime.ascii(acBuffer);             // Get time for prediction as ASCII string
MySAT2.predict(MyTime2);              // Predict ISS for specific time
MySAT2.latlon(dSatLATb, dSatLONb);     // Get the rectangular coordinates

globe_vertices[orb][0] = radius * cos (dSatLATb*PI/180) * cos (-dSatLONb*PI/180);
globe_vertices[orb][1] = radius * cos (dSatLATb*PI/180) * sin (-dSatLONb*PI/180);
globe_vertices[orb][2] = radius * sin (dSatLATb*PI/180);

orb = orb + 1;
}

//picture loop

 yangle = 270;//270 Can't quite get my head round why these numbers work but they do!
 zangle = 90;//90
 
 for (int angle = 0; angle <= 360; angle = angle + 3) {
  u8g2.firstPage();
  do {
   for (int i = 0; i < globe_cnt; i++) {

     
     rot = angle * 0.0174532; //0.0174532 = one degree
     yrot = yangle * 0.0174532; //0.0174532 = one degree
     zrot = zangle * 0.0174532; //0.0174532 = one degree
     
//rotateY
    rotz = globe_vertices[i][2] * cos(yrot) - globe_vertices[i][0] * sin(yrot);
    rotx = globe_vertices[i][2] * sin(yrot) + globe_vertices[i][0] * cos(yrot);
    roty = globe_vertices[i][1];
//rotateX
    rotyy = roty * cos(rot) - rotz * sin(rot);
    //rotzz = roty * sin(rot) + rotz * cos(rot);
    rotxx = rotx;
//rotateZ
    rotxxx = rotxx * cos(zrot) - rotyy * sin(zrot);
    rotyyy = rotxx * sin(zrot) + rotyy * cos(zrot);
    //rotzzz = rotzz;

//orthographic projection
    rotxxx = rotxxx * scalefactor + originx;
    rotyyy = rotyyy * scalefactor + originy;

//store new vertices values for wireframe drawing
    wireframe[i][0] = rotxxx;
    wireframe[i][1] = rotyyy;
    //wireframe[i][2] = rotzzz;

     draw_vertices();
     
   }

   
   
    draw_wireframe();
  if (scalefactor < 0.4) scalefactor = scalefactor + 0.005;
 


 
   } while (u8g2.nextPage());
 }

 //second screen
    OLED_2.firstPage();
    do {
    //OLED_2.clearBuffer();

    OLED_2.setFont(u8g2_font_u8glib_4_tr);    // u8g2 font 
    OLED_2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders
    OLED_2.drawLine(0, 9, 128,9);
    OLED_2.drawLine(0, 27, 128,27);
    OLED_2.drawStr(2, 7, " MISSION CONTROL ");
    
    OLED_2.drawLine(0, 54, 128,54);
    OLED_2.drawStr(2, 61, "OKUBO HEAVY INDUSTRIES");

    if (hr_val < 15) { hr_val_jp = hr_val + 9;  // convert UTC to Japan time zone
     }
     else {hr_val_jp = hr_val -15;
     }

     
     if (!loc_valid)
      {          
      
        OLED_2.drawStr(3, 16, "LAT : ********");
        
        OLED_2.drawStr(60, 16, "LON : ********");
        
       }
       else
       {
          OLED_2.drawStr(3, 16, "LAT :");
          OLED_2.setCursor(22, 16);
          OLED_2.println(lat_val, 6);
          
          
          OLED_2.drawStr(60, 16, "LON :");
          OLED_2.setCursor(79, 16);
          OLED_2.println(lng_val, 6);
          
        }
        if (!alt_valid)
        {
          
          OLED_2.drawStr(3, 24, "ALT : ********");
        }
        else
        {
           
          OLED_2.drawStr(3, 24, "ALT :");
          OLED_2.setCursor(22, 24);
          OLED_2.println(alt_m_val, 2);   
        }
        
        if (!sats_valid)
        {
          
          OLED_2.drawStr(60, 24, "GPS LOCK : **");
        }
        else
        {
         
          OLED_2.drawStr(60, 24, "GPS LOCK :");
          OLED_2.setCursor(100, 24);
          OLED_2.println(sats_val, 1);   
        }
        
        if (!time_valid)
        {
          OLED_2.drawStr(74, 7, "Time : ********");
         
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d:%02d:%02d \n", hr_val_jp, min_val, sec_val);
          OLED_2.setCursor(74, 7);
          OLED_2.print(time_string);    
        }
  
        
        OLED_2.drawStr(3,52,"UTC:");
        OLED_2.setCursor(22,52);
        OLED_2.println(acBuffer);
        
        OLED_2.drawStr(3,40,"Lat:");
        OLED_2.setCursor(22,40);
        OLED_2.println(dSatLAT,4);
        OLED_2.drawStr(60,40,"  Lon: ");
        OLED_2.setCursor(80,40);
        OLED_2.println(dSatLON,4);

        OLED_2.drawStr(3,46,"AZ:");
        OLED_2.setCursor(22,46);
        OLED_2.println(dSatAZ,2);

        OLED_2.drawStr(60,46,"EL:");
        OLED_2.setCursor(80,46);
        OLED_2.println(dSatEL,2);

        OLED_2.drawStr(3,34,"SAT:");
        OLED_2.setCursor(22,34);
        OLED_2.println(tleName);
         
          
    }while (OLED_2.nextPage());

}

void clearOLED(){
    u8g2.firstPage();  
    do {
    } while( u8g2.nextPage() );

}
