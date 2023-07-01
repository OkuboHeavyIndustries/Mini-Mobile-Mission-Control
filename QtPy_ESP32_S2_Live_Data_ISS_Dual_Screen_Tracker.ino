//1st July 2023
//QtPy ESP32 S2 Live Data ISS Dual Screen Tracker by Okubo Heavy Industries based on
//3D_Cube for Arduino OLED module by Colin Ord, 9/1/2015

/*Buy me a coffee!
Bitcoin: 19H3zFF4W3zUZ3jAdjmiDNNLs8Ja46M6AD
ETH: 0xD656DB37b61ac30Fa1e16a3162719FE417b231C8
*/

#include <U8g2lib.h>
#include <AioP13.h>
#include <TinyGPS++.h> 
#include "big_globe_coords.h"
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiClientSecure.h>

// Enter your WiFi SSID and password
char ssid[] = "Your_SSID";             // your network SSID (name)
char pass[] = "Your_SSID_Password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                      // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

#define SERVER "celestrak.org"
#define PATH   "/NORAD/elements/gp.php?CATNR=25544&FORMAT=tle" //this is the address for the Two Line Element (TLE) of the International Space Station

char tleName [30];
char tlel1 [80];
char tlel2 [80];

String tlenamestring;
String tle1string;
String tle2string;

int tlenamelength;

bool downloadcheck = false;

String filedatastring;

// Initialize the SSL client library
// with the IP address and port of the server
// that you want to connect to (port 443 is default for HTTPS):
WiFiClientSecure client;

//ISS Glyph
#define iss_width  9
#define iss_height 9
static unsigned char iss_bits[] = {
 0x83,0xff,0x83,0xff,0xbb,0xff,0x93,0xff,0xff,0xff,0x93,0xff,
 0xbb,0xff,0x93,0xff,0x83,0xff};

//The following lines will need changing depending on your screen type!
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED_2(U8G2_R0, A3, A2, /* reset=*/ U8X8_PIN_NONE);

//globe variables
float tx, nx, p;
float ty, ny, py;
float rot, rotx, roty, rotz, rotxx, rotyy, rotzz, rotxxx, rotyyy, rotzzz;
int i; //0 to 360
int fl, scale; //focal length
int wireframe[600][3];
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


float MM; //mean motion from the 2nd line of the TLE

int SMA; //the Semi major axis

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

static const uint32_t GPSBaud = 38400; //BE-280 GPS baud rate is 38400. BN-280 GPS baud rate is 9600

int xlon;
int ylat;

int xlonb;
int ylatb;

int orb;
int gpschar;

//int radius = 74;  //ISS orbital radius - should be about 67
// below radius is calculated using TLE
int radius;// = SMA+10; //add a fudge of +10 to distinguish between ground level and low Earth orbit.  Only one or two pixel diiference otherwise 

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
  Serial.begin(GPSBaud);
  
 u8g2.begin();
 OLED_2.begin();

 u8g2.firstPage();
 OLED_2.firstPage();
 do {
 } while ( u8g2.nextPage() );

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi");
  printWifiStatus();

  client.setInsecure(); // don't use a root cert

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(SERVER, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET " PATH " HTTP/1.1");
    client.println("Host: " SERVER);
    client.println("Connection: close");
    client.println();
  }

 
}

uint32_t bytes = 0;

void loop(void)
{

  if (downloadcheck == false){
    
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    filedatastring = client.readStringUntil('\n');
    //char c = client.read();
    Serial.println(filedatastring); //Check getting data from web
    if (filedatastring.startsWith("ISS")) { //Store TLE data when we get to it
       tlenamestring = filedatastring;
       tlenamestring.trim();
       tlenamelength = tlenamestring.length();
       tle1string = client.readStringUntil('\n');  
       tle2string = client.readStringUntil('\n');
       tlenamestring.toCharArray(tleName, tlenamestring.length()+1); // Store TLE data in char arrays
       tle1string.toCharArray(tlel1, tle1string.length()+1); 
       tle2string.toCharArray(tlel2, tle2string.length()+1); 

// Check we have TLE data stored correctly
       Serial.print("TLE Name ");
       Serial.println(tleName);
       Serial.print("TLE Line 1 ");
       Serial.println(tlel1);
       Serial.print("TLE Line 2 ");
       Serial.println(tlel2);
    
    
    bytes++;
      }
    }
     
  
  
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    Serial.print("Read "); Serial.print(bytes); Serial.println(" bytes");

    downloadcheck = true; //make sure we only download data once
  }

    MM = atof(tlel2+52); //Extract the mean motion from the 2nd line of the TLE
    SMA = (cbrt((sq((86400/MM)/(2*PI)))*398600)/100); //Calculate the Semi major axis
    radius = SMA+10; //add a fudge of +10 to distinguish between ground level and low Earth orbit.  Only one or two pixel diiference otherwise.
    //check extracted MM and calculation of SMA
    Serial.println(MM);
    Serial.println(SMA);

  }




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

globe_vertices[368][0] = radius * cos (dSatLAT*PI/180) * cos (-dSatLON*PI/180);
globe_vertices[368][1] = radius * cos (dSatLAT*PI/180) * sin (-dSatLON*PI/180);
globe_vertices[368][2] = radius * sin (dSatLAT*PI/180);

//Full orbit vertices
orb = 369; //first orbital vertice
for (int delta = -50; delta < 50; delta = delta + 5) { //do calculations for t-50 minutes to t+50 minutes in 5 minute intevals
P13DateTime MyTime2(year_val, month_val, day_val, hr_val, (min_val + delta) , sec_val); // Set start time for the prediction
P13Observer MyQTH(pcMyName, dMyLAT, dMyLON, dMyALT);              // Set observer coordinates
P13Satellite MySAT2(tleName, tlel1, tlel2);                        // Create ISS data from TLE
//MyTime.ascii(acBuffer);             // Get time for prediction as ASCII string
MySAT2.predict(MyTime2);              // Predict ISS for specific time
MySAT2.latlon(dSatLATb, dSatLONb);     // Get the rectangular coordinates

//convert lat and lon to cartesian coordinates
globe_vertices[orb][0] = radius * cos (dSatLATb*PI/180) * cos (-dSatLONb*PI/180);
globe_vertices[orb][1] = radius * cos (dSatLATb*PI/180) * sin (-dSatLONb*PI/180);
globe_vertices[orb][2] = radius * sin (dSatLATb*PI/180);

orb = orb + 1;
}

//picture loop

 yangle = 270;//270 Can't quite get my head round why these numbers work but they do!
 zangle = 90;//90
 
 for (int angle = 0; angle <= 360; angle = angle + 3) { //spin the globe 360º in 3 degree increments
  u8g2.firstPage();
  do {
   for (int i = 0; i < globe_cnt; i++) {  //go through the vertices in globe_coords.h one at a time

     
     rot = angle * 0.0174532; //0.0174532 = one degree
     yrot = yangle * 0.0174532; //0.0174532 = one degree
     zrot = zangle * 0.0174532; //0.0174532 = one degree
     
//rotateY
    rotz = globe_vertices[i][2] * cos(yrot) - globe_vertices[i][0] * sin(yrot);
    rotx = globe_vertices[i][2] * sin(yrot) + globe_vertices[i][0] * cos(yrot);
    roty = globe_vertices[i][1];
//rotateX
    rotyy = roty * cos(rot) - rotz * sin(rot);
    rotzz = roty * sin(rot) + rotz * cos(rot);
    rotxx = rotx;
//rotateZ
    rotxxx = rotxx * cos(zrot) - rotyy * sin(zrot);
    rotyyy = rotxx * sin(zrot) + rotyy * cos(zrot);
    rotzzz = rotzz;

//orthographic projection
    rotxxx = rotxxx * scalefactor + originx;
    rotyyy = rotyyy * scalefactor + originy;

//store new vertices values for wireframe drawing
    wireframe[i][0] = rotxxx;
    wireframe[i][1] = rotyyy;
    wireframe[i][2] = rotzzz;

     draw_vertices();  //we can skip this but we lose Hawaii!
     
   }

   
   
    draw_wireframe();
  if (scalefactor < 0.4) scalefactor = scalefactor + 0.005; //Globe has a radius of 64 so 0.4 scale factor keeps whole World on screen
  
   } while (u8g2.nextPage());
 }

 //second screen
  //I'm sure there is a better way to get this to update more frequently but I can't figure it out
  //I keep getting both screens data displaying on both screens at once.  For the time being this updates just once a rotation
    
    OLED_2.firstPage();
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
        
      OLED_2.nextPage();
}

void clearOLED(){
    u8g2.firstPage();  
    do {
    } while( u8g2.nextPage() );

}

void draw_vertices(void)
{
  if (rotzzz > 0){ //only draw the vertices if they are on the side of the Earth facing the screen
 u8g2.drawPixel (rotxxx, rotyyy);
  }
}

void draw_wireframe(void)
{

 for (w = 0; w < 4; w++){
  if (wireframe[w][2]>0){ //only draw the lines if they are on the side of the Earth facing the screen
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[4][2]>0){
 u8g2.drawLine(wireframe[4][0], wireframe[4][1], wireframe[0][0], wireframe[0][1]);//Greenland
 }

 for (w = 5; w < 8; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 
 if (wireframe[8][2]>0){
 u8g2.drawLine(wireframe[8][0], wireframe[8][1], wireframe[5][0], wireframe[5][1]);//Iceland
 }

 for (w = 9; w < 14; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[14][2]>0){
 u8g2.drawLine(wireframe[14][0], wireframe[14][1], wireframe[9][0], wireframe[9][1]);//Ireland
 }

 for (w = 15; w < 32; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[32][2]>0){
 u8g2.drawLine(wireframe[32][0], wireframe[32][1], wireframe[15][0], wireframe[15][1]);//Britain
 }

 for (w = 33; w < 97; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[97][2]>0){
 u8g2.drawLine(wireframe[97][0], wireframe[97][1], wireframe[99][0], wireframe[99][1]);//Europe, Asia, Africa
 }

  
 for (w = 99; w < 131; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[132][2]>0){
 u8g2.drawLine(wireframe[131][0], wireframe[131][1], wireframe[33][0], wireframe[33][1]);//Europe, Asia, Africa
 }

 for (w = 132; w < 242; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[242][2]>0){
 u8g2.drawLine(wireframe[242][0], wireframe[242][1], wireframe[132][0], wireframe[132][1]);//Americas
}

 for (w = 243; w < 247; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[247][2]>0){
 u8g2.drawLine(wireframe[247][0], wireframe[247][1], wireframe[243][0], wireframe[243][1]);//Madagascar
 }

 for (w = 248; w < 268; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[268][2]>0){
 u8g2.drawLine(wireframe[268][0], wireframe[268][1], wireframe[248][0], wireframe[248][1]);//Australia
 }

 for (w = 269; w < 271; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[271][2]>0){
 u8g2.drawLine(wireframe[271][0], wireframe[271][1], wireframe[269][0], wireframe[269][1]);//New Zealand North
 }

 for (w = 272; w < 276; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[276][2]>0){
 u8g2.drawLine(wireframe[276][0], wireframe[276][1], wireframe[272][0], wireframe[272][1]);//New Zealand South 
 }

 for (w = 277; w < 284; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[284][2]>0){
 u8g2.drawLine(wireframe[284][0], wireframe[284][1], wireframe[277][0], wireframe[277][1]);//Japan Hokkaido
 }

 for (w = 285; w < 295; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[295][2]>0){
 u8g2.drawLine(wireframe[295][0], wireframe[295][1], wireframe[285][0], wireframe[285][1]);//Japan Honshu
 }

 for (w = 296; w < 300; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[300][2]>0){
 u8g2.drawLine(wireframe[300][0], wireframe[300][1], wireframe[296][0], wireframe[296][1]);//Sahkalin
 }

 for (w = 301; w < 304; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[304][2]>0){
 u8g2.drawLine(wireframe[304][0], wireframe[304][1], wireframe[301][0], wireframe[301][1]);//Taiwan
 }

 for (w = 305; w < 308; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[308][2]>0){
 u8g2.drawLine(wireframe[308][0], wireframe[308][1], wireframe[305][0], wireframe[305][1]);//Phillipines
 }

 for (w = 309; w < 311; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[311][2]>0){
 u8g2.drawLine(wireframe[311][0], wireframe[311][1], wireframe[309][0], wireframe[309][1]);//Phillipines South
 }

 for (w = 312; w < 320; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[320][2]>0){
 u8g2.drawLine(wireframe[320][0], wireframe[320][1], wireframe[312][0], wireframe[312][1]);//Borneo
 }

 for (w = 321; w < 324; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[324][2]>0){
 u8g2.drawLine(wireframe[324][0], wireframe[324][1], wireframe[321][0], wireframe[321][1]);//Indonesia
 }

  for (w = 325; w < 327; w++){
    if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
    }
 }
 if (wireframe[327][2]>0){
 u8g2.drawLine(wireframe[327][0], wireframe[327][1], wireframe[325][0], wireframe[325][1]);//Indonesia - South
 }

 for (w = 328; w < 334; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[334][2]>0){
 u8g2.drawLine(wireframe[334][0], wireframe[334][1], wireframe[328][0], wireframe[328][1]);//Papua New Guinea
 }

 for (w = 335; w < 337; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[337][2]>0){
 u8g2.drawLine(wireframe[337][0], wireframe[337][1], wireframe[335][0], wireframe[335][1]);//Sri Lanka
 }

 for (w = 338; w < 344; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[344][2]>0){
 u8g2.drawLine(wireframe[344][0], wireframe[344][1], wireframe[338][0], wireframe[338][1]);//Cuba
 }

 for (w = 345; w < 348; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[348][2]>0){
 u8g2.drawLine(wireframe[348][0], wireframe[348][1], wireframe[345][0], wireframe[345][1]);//Dominica
 }

 for (w = 349; w < 366; w++){
  if (wireframe[w][2]>0){
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]);
  }
 }
 if (wireframe[366][2]>0){
 u8g2.drawLine(wireframe[366][0], wireframe[366][1], wireframe[349][0], wireframe[349][1]);//Antarctica
 }

  for (w = 369; w < 388; w++){
    if (wireframe[w][2]>-35){ //-35 is a fudge factor added so that the orbital path continues behind z=0 a little
  u8g2.drawLine(wireframe[w][0], wireframe[w][1], wireframe[w+1][0], wireframe[w+1][1]); //ISS orbital path
  }
  }

  u8g2.drawCircle(originx,originy,64*scalefactor);
  u8g2.setFont(u8g2_font_u8glib_4_tr);
  u8g2.drawFrame(0, 0, 128,64);  //setup fixed screen info and borders

  if (wireframe[368][2]>-30){ //-30 is a fudge factor added so that the ISS continues behind z=0 a little
  u8g2.drawXBM( wireframe[368][0]-4, wireframe[368][1]-4, iss_width, iss_height, iss_bits); //tiny ISS picture
  //u8g2.drawCircle(wireframe[368][0],wireframe[368][1],4);
  //u8g2.drawDisc(wireframe[368][0],wireframe[368][1],1);
  if (wireframe[368][1] >32){
  u8g2.setCursor(wireframe[368][0]+6,wireframe[368][1]-6);
  }
  else {
    u8g2.setCursor(wireframe[368][0]+6,wireframe[368][1]+10);
  }
  u8g2.print("ISS");
  }
} 

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
