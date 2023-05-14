// only one core for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseTye_t app_cpu = 0; // BaseType_t is an int
#else
  static const BaseType_t app_cpu = 1;
#endif

const char msg[] = "This is a test string that needs to be long enought to be interrupted by an astras.";

// handles to the tasks to have the ability to control the tasks from a third task
static TaskHandle_t task1 = NULL; // TaskHandle_t is a void *
static TaskHandle_t task2 = NULL;

void startTask1(void* params) {
  // determine the number of letters in the string
  int msg_len = strlen(msg);
  // printout to the terminal
  while(1) {
    Serial.println(); // carriage return
    for (int i = 0; i < msg_len; i++) {
      Serial.print(msg[i]);
    }
    Serial.println(); // carriage return
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void startTask2(void* params) {
  // This task should have a higher priority then the task1 so that is will preempt the task1 and print an astras
  while(1) {
    Serial.print('*');
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

// setup task - separate FreeRTOS task
void setup() {
  // make the serial IO baud rate very slow so we can see in which order everything is printed
  Serial.begin(300);

  // wait before starting the printout
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.print("-- FreeRTOS Task Scheduling Demo --");

  // print the control tasks priority
  Serial.print("Setup and loop task running on core ");
  Serial.print(xPortGetCoreID()); // not in vanilla FreeRTOS since FreeRTOS is single core on default - this is from the espressive SMP extension
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));

  // creating the tasks
  xTaskCreatePinnedToCore(
    startTask1,
    "Task1",
    1024,   // stack memory
    NULL,   // params
    1,      // task priority
    &task1, // task handle
    app_cpu // cpu assignment
  );

xTaskCreatePinnedToCore(
  startTask2,
  "Task2",
  1024,
  NULL,
  2,    // higher priority then task 1
  &task2,
  app_cpu
);

}

// This will be the third control task since setup is started under the hood as a separate task
void loop() {
  // suspend the higher priority task with the astras to habe the * printed not all 100 ms
  for (int i = 0; i < 3; i++) {
    vTaskSuspend(task2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(task2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }

  // after the three times of suspension, delete the text printer task to only print * after this
  if (task1 != NULL) {
    vTaskDelete(task1);
    task1 = NULL; // avoid dangaling pointer
  }

}
