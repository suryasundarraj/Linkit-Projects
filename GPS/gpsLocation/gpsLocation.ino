/*
 * GPS LOCATION TRACKER
 * Tracks the exact location of the user and prints on the serial monitor
 * -Surya Igor
 */
//Importing the GPS Library supported by the Linkit-One-SDK
#include <LGPS.h>

//Obtainging the GPS Response as info
gpsSentenceInfoStruct info;
char buff[256];

//Structure to Obtain the GPS Latitude and Longitude
struct gpsLatLon{
  float latitude,longitude;
  char latitude_dir,longitude_dir;  
}gpsLocation;

//Structure to Obtain the UTC Time and GPS Fix
struct gpsData{
  int hour,minute,second;
  int fix,num;
}MyGPSPos;

/*******************************************************************************************************
 Function Name            : getComma
 Description              : Search for the comma
 Parameters               : num,str
 *******************************************************************************************************/
static unsigned char getComma(unsigned char num,const char *str){
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++){
    if(str[i] == ',')
      j++;
    if(j == num)
      return i + 1; 
    }
  return 0; 
}

/*******************************************************************************************************
 Function Name            : getFloatNumber
 Description              : converts the char buffer into float
 Parameters               : charBuffer
 *******************************************************************************************************/
static float getFloatNumber(const char *charBuffer){
  char buf[10];
  unsigned char i;
  float rev;

  i=getComma(1, charBuffer);
  i = i - 1;
  strncpy(buf, charBuffer, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

/*******************************************************************************************************
 Function Name            : getIntNumber
 Description              : converts the char buffer into int
 Parameters               : charBuffer
 *******************************************************************************************************/
static float getIntNumber(const char *charBuffer){
  char buf[10];
  unsigned char i;
  float rev;
  i=getComma(1, charBuffer);
  i = i - 1;
  strncpy(buf, charBuffer, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

/*******************************************************************************************************
 Function Name            : parse_data
 Description              : Parse the GPGGA data into the Lat Lon Values[Google Readable]
 Parameters               : GPGGAstr
 *******************************************************************************************************/
void parse_data(const char* GPGGAstr){
  if(GPGGAstr[0] == '$'){
    int tmp;
    tmp = getComma(1, GPGGAstr);
    MyGPSPos.hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    MyGPSPos.minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    MyGPSPos.second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');

    //get time
    sprintf(buff, "UTC timer : %2d-%2d-%2d", MyGPSPos.hour, MyGPSPos.minute, MyGPSPos.second);
    Serial1.println(buff);
    //get lat/lon coordinates
    float latitudetmp;
    float longitudetmp;
    tmp = getComma(2, GPGGAstr);
    latitudetmp = getFloatNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitudetmp = getFloatNumber(&GPGGAstr[tmp]);
    // need to convert format
    convertCoords(latitudetmp, longitudetmp, gpsLocation.latitude, gpsLocation.longitude);
    //get lat/lon direction
    tmp = getComma(3, GPGGAstr);
    gpsLocation.latitude_dir = (GPGGAstr[tmp]);
    tmp = getComma(5, GPGGAstr);    
    gpsLocation.longitude_dir = (GPGGAstr[tmp]);
    
    sprintf(buff, "Latitude : %10.4f%c \t Longitude : %10.4f%c", gpsLocation.latitude, gpsLocation.latitude_dir, gpsLocation.longitude, gpsLocation.longitude_dir);
    Serial1.println(buff); 
    
    //get GPS fix quality
    tmp = getComma(6, GPGGAstr);
    MyGPSPos.fix = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "GPS fix quality : %d", MyGPSPos.fix);
    Serial1.println(buff);   
    //get satellites in view
    tmp = getComma(7, GPGGAstr);
    MyGPSPos.num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "Number of Satellites : %d", MyGPSPos.num);
    Serial1.println(buff); 
  }
  else{
    Serial1.println("No GPS data"); 
  }
}

/*******************************************************************************************************
 Function Name            : convertCoords
 Description              : converts lat and lon value (degrees-mins-secs) to true decimal-degrees
 Parameters               : latitude, longitude, lat_return, lon_return
 *******************************************************************************************************/
void convertCoords(float latitude, float longitude, float &lat_return, float &lon_return){
  int lat_deg_int = int(latitude/100);    //extract the first 2 chars to get the latitudinal degrees
  int lon_deg_int = int(longitude/100);   //extract first 3 chars to get the longitudinal degrees
  // must now take remainder/60
  //this is to convert from degrees-mins-secs to decimal degrees
  // so the coordinates are "google mappable"
  //remove the degrees part of the coordinates - so we are left with only minutes-seconds part of the coordinates
  float latitude_float = latitude - lat_deg_int * 100;    
  float longitude_float = longitude - lon_deg_int * 100;
  //add back on the degrees part, so it is decimal degrees     
  lat_return = lat_deg_int + latitude_float / 60 ;      
  lon_return = lon_deg_int + longitude_float / 60 ;
}

/*******************************************************************************************************
 Function Name            : setup
 Description              : Initalize the Settings and Power on GPS 
 Parameters               : none
 *******************************************************************************************************/
void setup() {
  //Start your serial monitor with 115200 BAUD
  Serial1.begin(115200);
  //Initialize the GPS 
  LGPS.powerOn();
  Serial1.println("LGPS Power on, and waiting ..."); 
  delay(3000);
}

/*******************************************************************************************************
 Function Name            : loop
 Description              : Continuously Scan the GPS Co-ordinates and receives the location
 Parameters               : none
 *******************************************************************************************************/
void loop() {
  // put your main code here, to run repeatedly:
  Serial1.println("LGPS loop");
  //Obtain the GPS data 
  LGPS.getData(&info);
  //Serial1.println((char*)info.GPGGA); 
  parse_data((const char*)info.GPGGA);
  //wait for 2 Seconds each time
  delay(2000);
}

//End of the Program
