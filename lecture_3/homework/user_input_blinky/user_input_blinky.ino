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
  unsigned int idx = 0;
  while(1) {
    msgToUse = NULL;
    // check and validate value change
    if (Serial.available() > 0) {
      char tmpVal = Serial.read();
      if (tmpVal != '\n' && idx < max_buffer_length) {
        buffer_arr[idx] = tmpVal;
        idx++;
      } else if (tmpVal == '\n' && idx < max_buffer_length) {
        // nullterminate the string
        buffer_arr[idx] = '\0'; 
        // convert to int
        printToSerialInteraction("Update blinky time to:");
        printToSerialInteraction(buffer_arr);
        blinkyTime = atoi(buffer_arr);
        // reset
        memset(buffer_arr, 0, max_buffer_length);
        idx = 0;
      } else {
        printToSerialInteraction("Your input was too long. Please try again!");
        // reset
        memset(buffer_arr, 0, max_buffer_length);
        idx = 0;
      }

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
