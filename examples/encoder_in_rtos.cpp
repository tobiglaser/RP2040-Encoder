#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

//#define DEBUG_IRQ_DURATION_PIN 11

#include "quadrature.h"

#define ever ;;

/*
    In this example it is one tasks duty, 
    to print the encoder value whenever it changes.
    The other tasks job is to simulate a rotating encoder
    with intervalls of SIM_INTERVAL milliseconds.
    
    Bridge pins 13 and 15 and pins 12 and 14 for this to work.

    Try changing the resolution parameter to count less changes.
*/

const int ENCODER1_DATA_PIN = 14;
const int ENCODER1_CLK_PIN = 15;

const int SIM_DATA_PIN = 12;
const int SIM_CLK_PIN = 13;
const int SIM_INTERVAL = 20;//ms

void simulatedEncoder( void * pvParameters )
{
    stdio_init_all();
    
    gpio_init(SIM_DATA_PIN);
    gpio_init(SIM_CLK_PIN);
    gpio_set_dir(SIM_DATA_PIN, GPIO_OUT);
    gpio_put(SIM_DATA_PIN, 0);
    gpio_set_dir(SIM_CLK_PIN, GPIO_OUT);
    gpio_put(SIM_CLK_PIN, 0);

    for(ever)
    {
        gpio_put(SIM_DATA_PIN, 1);
        printf("+1\n");
        vTaskDelay(SIM_INTERVAL);

        gpio_put(SIM_CLK_PIN, 1);
        printf("+1\n");
        vTaskDelay(SIM_INTERVAL);

        gpio_put(SIM_DATA_PIN, 0);
        printf("+1\n");
        vTaskDelay(SIM_INTERVAL);

        gpio_put(SIM_CLK_PIN, 0);
        printf("+1\n");
        vTaskDelay(SIM_INTERVAL);
    }
}

void encoderTask ( void * pvParameters )
{
    Quadrature_encoder<ENCODER1_DATA_PIN, ENCODER1_CLK_PIN> encoder1 = Quadrature_encoder<ENCODER1_DATA_PIN, ENCODER1_CLK_PIN>();
    encoder1.begin(pull_direction::up, resolution::full);

    int countBefore=0;
    for(ever)
    {
        int count = encoder1.count();
        if (countBefore != count)
        {
            countBefore = count;
            printf("%d\n", count);
        }
    }
}

int main() {


BaseType_t xReturned;
TaskHandle_t xHandle = NULL;
/* Create the task, storing the handle. */
    xReturned = xTaskCreate(
                    simulatedEncoder,/* Function that implements the task. */
                    "Sim task",      /* Text name for the task. */
                    512,             /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle );   

BaseType_t yReturned;
TaskHandle_t yHandle = NULL;
/* Create the task, storing the handle. */
    yReturned = xTaskCreate(
                    encoderTask,     /* Function that implements the task. */
                    "Encoders task", /* Text name for the task. */
                    512,             /* Stack size in words, not bytes. */
                    ( void * ) 1,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &yHandle );

    vTaskStartScheduler();
    while(1)
    {
        configASSERT(0);    /* We should never get here */
    }

}