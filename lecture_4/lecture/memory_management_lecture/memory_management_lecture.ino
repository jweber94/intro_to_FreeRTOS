/* Memory Management */

#if CONFIG_FREERTOS_UNICORE
 static const BaseType_t app_cpu = 0;
#else
 static const BaseType_t app_cpu = 1;
#endif

// task function
void testTask(void* param) {
  while(1) {
    int a = 1;
    int b[100];
    for (int i = 0; i < 100; i++) {
      b[i] = a + 1;
    }
    Serial.println(b[0]);

    // print remaining stack memory
    Serial.print("High water mark (words): ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL)); // NULL means this task/thread - a word is 8 bytes. We need to multiply by 4 to get bytes in 32bit system

    // print number of free heap bytes
    Serial.print("Heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    // allocating memory
    int* ptr = (int*) pvPortMalloc(1024*sizeof(int)); // sizeof(int) = 4 on a 32 bit system
    if (ptr == NULL) {
      Serial.print("Not enough heap available");
    } else {
      // prevent the compiler from optimizing out the heap memory allocation - we need to do something with our allocated memory
      for (int i = 0; i < 1024; i++) {
        ptr[i] = 3;
      }
    }
    
    Serial.print("Heap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());
    
    vPortFree(ptr); // Free the memory from the heap to avoid filling it up completely
    
    vTaskDelay(100 / portTICK_PERIOD_MS); // make the printout easier to read

  }
}

void setup() {
  // setup the baud rate for the serial interface
  Serial.begin(9600);
  vTaskDelay(1000 / portTICK_PERIOD_MS); // vTaskDelay takes the number of ticks, the task should wait - dividing by ms/ticks constant will result in the number of ticks that are needed for the number in milliseconds

  // setup mundane task
  xTaskCreatePinnedToCore(testTask,
                          "Test Task",
                          1500,
                          NULL,
                          1, 
                          NULL,
                          app_cpu
  );

}

void loop() {
  // put your main code here, to run repeatedly:

}
