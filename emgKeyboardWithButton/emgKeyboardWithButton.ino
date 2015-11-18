/*Globals*/

const int rightFoot = A1;  // Analog input pin for Left foot EMG
const int leftFoot = A2;  // Analog input pin for Left foot EMG

const int redLED = 11; //Digital input pin 
const int greenLED = 10; //Digital input pin 
const int blueLED = 9; //Digital input pin 

/*Set up as button interrupt on Pin 2 (interrupt 0)*/
const int button = 0; //Digital input pin 

/*Setup for left and right emg values*/
volatile int emgValueLeft = 0;
volatile int emgValueRight = 0;

/* Setup for baseline emg reading */
float low = 475.00;
float high = 625.00;

/*Current state of the Arduino
          Red: Off
          Blue: Test
          Green: On                  */

volatile unsigned int current_state = 1;

/*Setup software debounching of the button*/
volatile long long timeout = 150; // .15 seconds
volatile long long last_change_time = 0;


/*Setup testing timer*/
volatile unsigned long long testingTimer = 0;
volatile unsigned long long pressed = 0;


void setup()
{
  // initialize serial commuications at 9600 bps:
  Serial.begin(9600);
  Keyboard.begin();
  /*Increase analogRead() sampling rate by
       running with high speed clock       */
  bitClear(ADCSRA, ADPS0);
  bitClear(ADCSRA, ADPS1);
  bitSet(ADCSRA, ADPS2);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  attachInterrupt(button, next, RISING); // digital pin 2 // next
}

void loop()
{
  switch (current_state) {
    //Red LED: Off and no function
    case 1:
      setColor(255, 0, 0);  // red
      break;
    /*BLue LED: Testing with functionality of blinking lights
     *  Blinks Aqua when left foot is activated 
     *  and Purple when right foot is activated
     */
    case 2:
      setColor(0, 0, 255); // blue
      emgValueLeft = analogRead(leftFoot);
      emgValueRight = analogRead(rightFoot);
      //If left foot activated blink aqua
      if(emgValueLeft >= high || emgValueLeft <= low) {
        setColor(0, 0, 0);
        delay(300);
        setColor(0, 255, 255);
        delay(300);
        setColor(0, 0, 0);
        delay(300);
      }
      //If right foot activated blink purple
      if(emgValueRight >= high || emgValueRight <= low) {
        setColor(0, 0, 0);
        delay(300);
        setColor(255, 0, 255);
        delay(300);
        setColor(0, 0, 0);
        delay(300);       
      }
      break;
    /* Green LED: Keyboard mode
     *  Writes left arrow when left foot is activated and 
     *  Write right arrow when right foot is activated
     */
    case 3:
      setColor(0, 255, 0);  // green
      while(current_state == 3) {
        testingTimer = micros();
        emgValueLeft = analogRead(leftFoot);
        emgValueRight = analogRead(rightFoot);
         //If left foot activated
        if(emgValueLeft >= high || emgValueLeft <= low) {
           Keyboard.write('l');
        }
        //If right foot activated
        if(emgValueRight >= high || emgValueRight <= low) {
          Keyboard.write('r');
        }
        
      }
      break;

      
  }
}

void setColor(int red, int green, int blue) {
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
  analogWrite(redLED, red);
  analogWrite(greenLED, green);
  analogWrite(blueLED, blue);
}

void next() {
  if ((current_state > 0 && ((millis() - last_change_time) > timeout)) || last_change_time == 0) {
    if (current_state >= 3) {
      current_state = 1;
    }
    else {
      current_state++;
    }
    last_change_time = millis();
  }
}


