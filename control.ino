
#define ENCODEROUTPUT 390


#define EN2 D1
uint8_t PWM = D4;


const int ENCODER_A = D5; // Hall sensor A connected to pin 3 (external interrupt)


//The sample code for driving one way motor encoder
volatile long encoderValue = 0;

int interval = 100;
long previousMillis = 0;
long currentMillis = 0;

long rpm = 0;


int desiredValue; // Desired speed 80 RPM
int error = 0; 
float k_p = 0.3 , k_i = 0.8 , integral = 0;
int u;

void IRAM_ATTR updateEncoder()
{
  // Add encoderValue by 1, each time it detects rising signal
  // from hall sensor A
  encoderValue++;
}


void setup() {

  Serial.begin(9600);//Initialize the serial port
  analogWrite(PWM , 0);
  delay(5000); // wait for serial plotter
  // Enable motor driver
  pinMode( EN2 , OUTPUT);
  

  // Set encoder A pulse input
  pinMode(ENCODER_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoder, RISING);



  encoderValue = 0;
  previousMillis = millis();
  digitalWrite(EN2,HIGH);

}


void loop() {

  // Update RPM value on every second
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    updateRPM();
    Serial.print(rpm);
    Serial.print(" ");
    Serial.println(desiredValue);
    
    int adcValue = analogRead(A0);
    desiredValue = ( map(adcValue, 0, 1024, 0, 255) * 120) / 255;
    

    error = desiredValue - rpm;
    integral += error;
    u = k_p * error + k_i * integral;

    u = (u * 255/120);

    if( u < 0 ) u = 0;
    if( u > 255 ) u = 255;


    analogWrite(PWM , u);
    
  }

  
  
  

}


void updateRPM()
{
    previousMillis = currentMillis;

    // Revolutions per minute (RPM) =  (total encoder pulse in 1s / motor encoder output) x 60s
    rpm =  (encoderValue * 600/ ENCODEROUTPUT);

    // Only update display when there have readings
    if ( rpm > 0) {
      // Serial.print(desiredValue);
      // Serial.print("    ");
      // Serial.print(encoderValue);
      // Serial.print(" pulse / ");
      // Serial.print(ENCODEROUTPUT);
      // Serial.print(" pulse per rotation x 60 seconds = ");
      // Serial.println(desiredValue);
      // Serial.println(" RPM");
    }
    encoderValue = 0;
}

