// only one core for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseTye_t app_cpu = 0; // BaseType_t is an int
#else
  static const BaseType_t app_cpu = 1;
#endif

// Task handles if needed
static TaskHandle_t task1 = NULL; // TaskHandle_t is a void *
static TaskHandle_t task2 = NULL;

// create a buffer for receiving messages from USB
const unsigned int max_buffer_length = 7; // maximum length is 6 plus null termination
static char buffer_arr[max_buffer_length];

// communication variable - no need to make it thread safe since a read or write function to an int is per se atomic
unsigned int blinkyTime = 1000; // ms
bool valueChanged = true;

// led pin on esp32
int led_pin = 2;

// helper functions
void printToSerialInteraction(char* msg) {
  // print the change message to the serial console
  if (NULL != msg) {
    const char* constMsg = msg;
    Serial.println();
    int len = strlen(constMsg);
    for (int i = 0; i < len; i++) {
      Serial.print(constMsg[i]);
    }
    Serial.println();
  }
}

unsigned int readoutBuffer() {
  // readout serial receive buffer
  uint32_t pos = 0;
  while (0 < Serial.available()) {
    char inByte = Serial.read();
    if ('\n' != inByte) { // assume \n termination from the serial input - could be adjusted within the serial monitor of the ardurino IDE
      buffer_arr[pos] = inByte;
    }
    pos++;
  }
  // add null termination to the string
  buffer_arr[pos] = '\n';
  unsigned int result = atoi(buffer_arr);
  return result;
}

uint32_t readSerialInput(unsigned int & newBlinkyVal) { // TODO: change return value to status code
  /*
    Return 0 if it could read out something and something valid, else 1 as a status code
  */
  uint32_t availBytes = Serial.available();
  if (availBytes > 0 && availBytes < max_buffer_length) { // assume \n termination of the input and therefore do not use the max_buffer_length - 1
    // there is something to read out
    newBlinkyVal = readoutBuffer();
    return 0;
  } else {
    // invalid readout of the serial receive buffer
    while(0 < Serial.available()) { // clear buffer for the next message if there is something within the buffer
      Serial.read();
    }
    return 1;
  }
}

// task functions
void blinkyFunction(void* params) {
  pinMode(led_pin, OUTPUT);
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(blinkyTime / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(blinkyTime / portTICK_PERIOD_MS);
  }
}

void serialInputFunction(void* params) {
  // create printout message
  char* successMsg = "Changed blinky successful to ";
  char* errorMsg = "Invalid change request. Please insert a valid unsigned integer value";
  char* msgToUse;

  // setup serial communication
  Serial.begin(300);
  unsigned int newBlinkyVal = 0;
  unsigned int oldBlinkyVal = 0;
  
  while(1) {
    msgToUse = NULL;
    // check and validate value change
    if (readSerialInput(newBlinkyVal)) {
      msgToUse = successMsg;
      // change the value
      blinkyTime = newBlinkyVal;
      oldBlinkyVal = newBlinkyVal;
    } else {
      msgToUse = errorMsg;
    } 
    // print and sleep until the next iteration of the task
    if (oldBlinkyVal != newBlinkyVal) {
      printToSerialInteraction(msgToUse); // TODO: Add parameter for the value to print
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  
  // create blinky task
  xTaskCreatePinnedToCore(
    blinkyFunction,
    "Blinky",
    1024,
    NULL,
    1,
    &task1,
    app_cpu
  );

  // create blinky task
  xTaskCreatePinnedToCore(
    serialInputFunction,
    "Serial",
    1024,
    NULL,
    2,
    &task2,
    app_cpu
  );

}

void loop() {
  // put your main code here, to run repeatedly:
}
