#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Hardware includes. */
#include "XMC4500.h"
#include "system_XMC4500.h"

/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue. The 200ms value is converted
to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS ( 200 / portTICK_PERIOD_MS )

/* The number of items the queue can hold. This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH ( 1 )

/* Values passed to the two tasks just to check the task parameter
functionality. */
#define mainQUEUE_SEND_PARAMETER ( 0x1111UL )
#define mainQUEUE_RECEIVE_PARAMETER ( 0x22UL )

/* To toggle the single LED */
#define mainTOGGLE_LED() ( PORT3->OMR = 0x02000200 )

/*-----------------------------------------------------------*/

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask(void *pvParameters);
static void prvQueueSendTask(void *pvParameters);

/*
 * Called by main() to create the simply blinky style application if
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 */
void main_blinky(void);

/*
 * The hardware only has a single LED. Simply toggle it.
 */
extern void vMainToggleLED(void);

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

void main_blinky(void)
{
    /* Create the queue. */
    xQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(unsigned long));

    if (xQueue != NULL) {
        /* Start the two tasks as described in the comments at the top of this
           file. */
        xTaskCreate(prvQueueReceiveTask,        /* The function that implements 
                                                   the task. */
                    "Rx",       /* The text name assigned to the task -
                                   for debug only as it is not used by the
                                   kernel. */
                    configMINIMAL_STACK_SIZE,   /* The size of the 
                                                   stack to allocate to
                                                   the task. */
                    (void *)mainQUEUE_RECEIVE_PARAMETER,        /* The parameter
                                                                   passed to the
                                                                   task - just
                                                                   to check the
                                                                   functionality.
                                                                 */
                    mainQUEUE_RECEIVE_TASK_PRIORITY,    /* The priority assigned 
                                                           to the task. */
                    NULL);      /* The task handle is not required, 
                                   so NULL is passed. */

        xTaskCreate(prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE,
                    (void *)mainQUEUE_SEND_PARAMETER,
                    mainQUEUE_SEND_TASK_PRIORITY, NULL);

        /* Start the tasks and timer running. */
        vTaskStartScheduler();
    }

    /* If all is well, the scheduler will now be running, and the following
       line will never be reached. If the following line does execute, then
       there was insufficient FreeRTOS heap memory available for the idle and/or
       timer tasks to be created. See the memory management section on the
       FreeRTOS web site for more details. */
    for (;;) ;
}

/*-----------------------------------------------------------*/

static void prvQueueSendTask(void *pvParameters)
{
    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Check the task parameter is as expected. */
    configASSERT(((unsigned long)pvParameters) == mainQUEUE_SEND_PARAMETER);

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for (;;) {
        /* Place this task in the blocked state until it is time to run again.
           The block time is specified in ticks, the constant used
           converts ticks to ms. While in the Blocked state this task
           will not consume any CPU time. */
        vTaskDelayUntil(&xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS);

        /* Send to the queue - causing the queue receive task to unblock and
           toggle the LED. 0 is used as the block time so the sending
           operation will not block - it shouldn't need to block as the
           queue should always be empty at this point in the code. */
        xQueueSend(xQueue, &ulValueToSend, 0U);
    }
}

/*-----------------------------------------------------------*/

static void prvQueueReceiveTask(void *pvParameters)
{
    unsigned long ulReceivedValue;

    /* Check the task parameter is as expected. */
    configASSERT(((unsigned long)pvParameters) == mainQUEUE_RECEIVE_PARAMETER);

    for (;;) {
        /* Wait until something arrives in the queue - this task will block
           indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
           FreeRTOSConfig.h. */
        xQueueReceive(xQueue, &ulReceivedValue, portMAX_DELAY);

        /* To get here something must have been received from the queue, but
           is it the expected value? If it is, toggle the LED. */
        if (ulReceivedValue == 100UL) {
            mainTOGGLE_LED();
            ulReceivedValue = 0U;
        }
    }
}

/*-----------------------------------------------------------*/
