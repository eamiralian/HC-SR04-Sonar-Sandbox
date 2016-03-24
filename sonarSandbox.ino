#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define trigPin 11
#define echoPin 12
#define ledPin 10
#define numSensorVals 128 // Display Width

int loopNum = 0;
int sensorVals[numSensorVals];

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // OLED initialization
  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // Initialize the array of sensor values for the graph to display
  for (int i=0; i < numSensorVals; i++){
    sensorVals[i] = 0;
  }

  // Clear the buffer, display test text
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Startup...");
  display.display();
}

void loop() {
  /* Circularly record sensor values into the array so 
  that we have one for each vertical line on the display, 
  overwriting the oldest value with each new value. */
  long distance = getSonarDistance();
  sensorVals[loopNum] = distance;
  displayGraph();
  ++loopNum;
  if (loopNum > 127) { loopNum = 0; }

  // Turn the LED on if there's a detection within a certain distance
  if (distance < 6) {  
    digitalWrite(ledPin,HIGH);
  } else {
    digitalWrite(ledPin,LOW);
  }
}

long getSonarDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5); // originally 10
  digitalWrite(trigPin, LOW);
  long microSecondDuration = pulseIn(echoPin, HIGH);
  long distance = durationToCm(microSecondDuration);
  return distance;
}

long durationToCm(long microSecondDuration) {
  return (microSecondDuration/2) / 29.1;
}

long durationToIn(long microSecondDuration) {
  return (microSecondDuration/2) / 74;
}

void displayGraph(){
  /* 
   *  FastVLine draws from the top down, so we'll use a white screen that we can draw black lines 
   *  from the top down varying in length by detection distance.
   *  This will have the end effect of displaying white lines (from the bottom up) representing how close the detection is to the sensor
   */
  display.clearDisplay();
  display.fillScreen(1);
  for (int16_t i = 0; i < numSensorVals; i++) {
    // loopNum is always the index of the last recorded sensor value, which we always want 
    // to display at the far right of the screen (numSensorVals-1, assuming every line on the display is used for the graph).

    // The sensor value to use for the current line in the graph is the index of the last recorded sensor val minus the distance to the position that val will be displayed at
    int idx = loopNum - ((numSensorVals-1) - i);
    // Rollover negative values
    if (idx < 0) {idx = numSensorVals - (idx * -1);}
    
    display.drawFastVLine(i, 0, sensorVals[idx], 0);
  }
  display.display();
}


