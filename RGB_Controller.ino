#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Ethernet.h>

//-----------------------------NeoPixel-Config-----------------------------------------
#define PIN 6 //Data Pin for Digital LEDs
#define PIXELS_N 100 //Pixel Lengh

//NeoPixel Builder
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(PIXELS_N, PIN,NEO_RGB+NEO_KHZ800);

//Data Pin
int TastPin = 2;

//Globale 
int Mode = 0;
int Delay = 100;
int R = 255;
int G = 255;
int B = 255;

//FadeModus_Seite
int x_Fade;
bool side = true;
//-----------------------------NeoPixel-Config-----------------------------------------


//--------------------------------Ethernet-Config---------------------------------------
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,178, 233);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

// Client variables 
char linebuf[80];
int charcount=0;
//----------------------------------Ethernet-Config---------------------------------------

//----------------------------------Status-Config-----------------------------------------
// Status
String Rainbow = "Off";
String Fade = "Off";
String Theater = "Off";
//----------------------------------Status-Config-----------------------------------------


void setup() { 
  pixel.begin();
  
  // Open serial communication at a baud rate of 9600
  Serial.begin(9600);
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

// Display dashboard page
void dashboardPage(EthernetClient &client) {
  client.println("<!DOCTYPE HTML><html><head>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>");                                                             
  client.println("<h3>Arduino RGB_Controller Web Server - <a href=\"/\">Refresh</a></h3>");
  
  // Generates buttons to control the RGB state
  // Creates the Internet Site
  //Rainbow
  client.println("<h4>Rainbow - State: " + Rainbow + "</h4>");
  if(Rainbow == "On")
  {
    client.println("<a href=\"/Status_0\"><button>"+Rainbow+"</button></a>");
  }
  else
  {
    client.println("<a href=\"/Status_2\"><button>"+Rainbow+"</button></a>");
  }     

  //Fade
  client.println("<h4>Fade - State: " + Fade + "</h4>");
  if(Fade == "On")
  {
    client.println("<a href=\"/Status_0\"><button>"+Fade+"</button></a>");
  }
  else
  {
    client.println("<a href=\"/Status_1\"><button>"+Fade+"</button></a>");
  } 

  //Theater
  client.println("<h4>Fade - State: " + Theater + "</h4>");
  if(Fade == "On")
  {
    client.println("<a href=\"/Status_0\"><button>"+Theater+"</button></a>");
  }
  else
  {
    client.println("<a href=\"/Status_1\"><button>"+Theater+"</button></a>");
  } 
  
  client.println("</body></html>"); 
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  //Nur Wenn sich jemand im Internet Verbindet--------------------------
  if (client) {
    Serial.println("new client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
       char c = client.read();
       //read char by char HTTP request
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          dashboardPage(client);
          break;
        }
        if (c == '\n') {

           //----- You can and Modes you Createt Here ------
          
          //Off
          if (strstr(linebuf,"GET /Status_0") > 0){
            Mode = 0;
            Rainbow = "Off";
            Fade = "Off";
            Theater = "Off";
            Serial.println("Changed: Modus= AUS");
          }
          //Fade
          else if (strstr(linebuf,"GET /Status_1") > 0){
            Mode = 1;
            Fade = "On";
            Rainbow = "Off";
            Theater = "Off";
            Serial.println("Changed: Modus = Fade");
          }
          //Rainbow
          else if (strstr(linebuf,"GET /Status_2") > 0){
            Mode = 2;
            Rainbow = "On";
            Fade = "Off";
            Theater = "Off";
            Serial.println("Changed: Modus = Rainbow");
          }
          //
          else if (strstr(linebuf,"GET /Status_3") > 0){
            Mode = 3;
            Rainbow = "Off";
            Fade = "Off";
            Theater = "On";
            Serial.println("Changed: Modus = Theater");
          }
          // you're starting a new line
          currentLineIsBlank = true;
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;          
        } 


        //-------------------------------------------------------
        
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
  //Nur Wenn sich jemand im Internet Verbindet--------------------------


  //Mode---------------------------------------------------------
    if(Mode == 0)
    {
      Off();
    }
    else if(Mode ==1)
    {
      FadeMode();
    }
    else if(Mode == 2)
    {
     rainbow();  
    }
    else if(Mode == 3)
    {
      theaterChaseRainbow();
    }
  //Mode---------------------------------------------------------
}



//Moduse
void Off()
{
  for(int i = 0;i<PIXELS_N;i++)
  {
    pixel.setPixelColor(i,pixel.Color(0,0,0));
  }
  pixel.show();
}

void FadeMode()
{
 if(side)
    {
      x_Fade++;

        for(int i = 0;i<PIXELS_N;i++)
         {
            pixel.setPixelColor(x_Fade,pixel.Color(B,R,G));//BRG
            delay(Delay);
         }
        pixel.show();
    }
    else if(!side)
    {
      x_Fade--;
      
       for(int i = 0;i<PIXELS_N;i++)
       {
        pixel.setPixelColor(x_Fade,pixel.Color(0,0,0));//BRG
        delay(Delay);
       }
       pixel.show();
    }

    if(x_Fade>=PIXELS_N)
    {
      side = false;  
    }
    else if(x_Fade<0)
    {
      side = true;
    }
}


void rainbow() 
{
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<pixel.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / pixel.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      pixel.setPixelColor(i, pixel.gamma32(pixel.ColorHSV(pixelHue)));
    }
    pixel.show(); // Update strip with new contents
  }
}

void theaterChaseRainbow() 
{
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      pixel.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<pixel.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / pixel.numPixels();
        uint32_t color = pixel.gamma32(pixel.ColorHSV(hue)); // hue -> RGB
        pixel.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      pixel.show();                // Update strip with new contents
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
