// Use only 1 core for learning purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// configure blinky pin
static const int led_pin = 2;

/* functions to use in the main setup and loop */
void toggleLED(void* parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // make the pin an output pin
  pinMode(led_pin, OUTPUT);

  // create task to run the blinky function
  xTaskCreatePinnedToCore(
    toggleLED,    // function with signature BaseType_t (*TaskHookFunction_t)( void * ) that contains the task execution instructions 
    "ToggleLED",  // name of the task
    1024,         // stack size
    NULL,         // parameters to the function (needs to be casted as void*)
    1,            // task priority
    NULL,         // task handle - this can be handed over to other task to check this tasks status, memory usage etc from other tasks
    app_cpu       // core to pin the task to (only esp32 specific/espressive FreeRTOS)
  );

  // In vanilla FreeRTOS we would need to call vTaskStartScheduler() after we setup our tasks. This is not
  // needed with the adurino IDE, since this function is called prior to the void setup() and void loop() function
  // because they are also started as separate FreeRTOS tasks under the hood of the adurino abstraction 
}

void loop() {
  // put your main code here, to run repeatedly:

}
