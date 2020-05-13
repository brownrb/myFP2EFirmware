/*
 Repeat timer example
 This example shows how to use hardware timer in ESP32. The timer calls onTimer
 function every second. The timer can be stopped with button attached to PIN 0
 (IO0).
 This example code is in the public domain.
 */


#define DIRPIN        32
#define STEPPIN       33
#define ENABLEPIN     14

#define MS1           27
#define MS2           26
#define MS3           25

#define LED 2

// Stop button is attached to PIN 0 (IO0)
#define BTN_STOP_ALARM    0

volatile int StepCount = 0;
bool flag = false;
bool toggle = false;

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;




void IRAM_ATTR onTimer()
{
  // Increment the counter and set the time of ISR
//  portENTER_CRITICAL_ISR(&timerMux);

  if (StepCount) 
  {
    digitalWrite(STEPPIN, 0);
    toggle = (toggle == true) ? false : true;
    digitalWrite(LED, toggle);  
    digitalWrite(STEPPIN, toggle);
    StepCount--;
//  portEXIT_CRITICAL_ISR(&timerMux);
    flag = true;
    digitalWrite(STEPPIN, 1);    
  }
  else
  {
    if (flag == true)
    {
      flag = false;
    // Give a semaphore that we can check in the loop
      xSemaphoreGiveFromISR(timerSemaphore, NULL);
    }
  }
}



void setup()
{ 
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(ENABLEPIN, OUTPUT);
  pinMode(DIRPIN, OUTPUT);
  pinMode(STEPPIN, OUTPUT);
  digitalWrite(ENABLEPIN, 1);

  digitalWrite(LED, 1);
  delay(500);
  digitalWrite(LED, 0);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);



/* STEP8
  digitalWrite(MS1, 1);
  digitalWrite(MS2, 1);
  digitalWrite(MS3, 0);
*/
//STEP16
  digitalWrite(MS1, 0);
  digitalWrite(MS2, 0);
  digitalWrite(MS3, 1);

  digitalWrite(ENABLEPIN, 0);  

  byte cosArray[90];

  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000, true);  // 50 ms

  // Start an alarm
  timerAlarmEnable(timer);
  

  StepCount = 200*16;


  for(int i; i < 90; i++)
  {
    cosArray[i] =(byte) ((cos(2*i *3.14159265/180)+1)*100);
    Serial.print(2*i);
    Serial.print(":  ");
    Serial.println(cosArray[i]);
  }

}


void loop() 
{
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE)
  {
    Serial.println("Semaphore fired up");
    timerEnd(timer);
    delay(1000);
    StepCount = 200*16;
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);    
    timerAlarmWrite(timer, 500, true);  // 50 ms    
    timerAlarmEnable(timer);
  }
}



/*
void loop() 
{
  // If Timer has fired
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE)
  {
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
    // Print it
    Serial.print("onTimer no. ");
    Serial.print(isrCount);
    Serial.print(" at ");
    Serial.print(isrTime);
    Serial.println(" ms");
  }

  // If button is pressed
  if (digitalRead(BTN_STOP_ALARM) == LOW) 
  {
    // If timer is still running
    if (timer) 
    {
      // Stop and free timer
      timerEnd(timer);
      timer = NULL;
    }
  }
}
*/