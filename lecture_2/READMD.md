# Lecture 2

## Adurino IDE and FreeRTOS configuration of the ESP32 for SMP Supports
+ Adjustments by espressive that are made to have FreeRTOS support the dual core architecture:
    - https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html
+ FreeRTOS configuration of the ESP32 FreeRTOS by espressive
    - Look at: `C:\Users\jens1\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\tools\sdk\include\freertos\freertos\FreeRTOSConfig.h`

## Important Theory
+ SMP = Symetric Multi Processing
+ Tick-Timer:
    - Is one of the microcontrollers hardware timers (e.g. a quarz). This hardware-timer will interrupt the processor in a specific interval of time
    - The interval time of the hardware timer is called a ***tick***
    - In a RTOS, the scheduler runs at each tick in the beginning the determine which task/thread should run during that specific tick(-interval)

## FreeRTOS specific learnings
+ `vTaskDelay()`: Takes the number of clock-ticks that a task (aka thread) should sleep.
    - This will free the scheduler for that time and other tasks can run in this time slice
    - vTaskDelay takes the number of ticks that the task should sleep for
    
    - If we want to sleep for 500 ms, we need to divide the sleep time in milliseconds by the `portTICK_PERIOD_MS`
        * `portTICK_PERIOD_MS` contains the amount of time that one tick takes (in 1/sec = Hz)
        * Example: 0.5 [s] / (0.0000025 [s/tick]) = 200000 [tick]