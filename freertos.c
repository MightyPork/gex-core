/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "tasks/sched_queue.h"
#include "stacksmon.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/

#define STACK_MAIN 160
#define STACK_MSG 230
#define STACK_LP 180
#define STACK_HP 150

osThreadId tskMainHandle;
uint32_t mainTaskBuffer[ STACK_MAIN ];
osStaticThreadDef_t mainTaskControlBlock;

osThreadId tskMsgHandle;
uint32_t msgTaskBuffer[ STACK_MSG ];
osStaticThreadDef_t msgTaskControlBlock;

osThreadId tskSchedLPHandle;
uint32_t schedLowBuffer[ STACK_LP ];
osStaticThreadDef_t schedLowControlBlock;

osThreadId tskSchedHPHandle;
uint32_t schedHighBuffer[ STACK_HP ];
osStaticThreadDef_t schedHighControlBlock;

osMessageQId queSchedLPHandle;
uint8_t myQueue01Buffer[ LP_SCHED_CAPACITY * sizeof( struct sched_que_item ) ];
osStaticMessageQDef_t myQueue01ControlBlock;

osMessageQId queSchedHPHandle;
uint8_t myQueue02Buffer[ HP_SCHED_CAPACITY * sizeof( struct sched_que_item ) ];
osStaticMessageQDef_t myQueue02ControlBlock;

osMessageQId queRxDataHandle;
uint8_t myQueue03Buffer[ RX_QUE_CAPACITY * sizeof( struct rx_que_item ) ];
osStaticMessageQDef_t myQueue03ControlBlock;

osMutexId mutTinyFrameTxHandle;
osStaticMutexDef_t myMutex01ControlBlock;

osSemaphoreId semVcomTxReadyHandle;
osStaticSemaphoreDef_t myBinarySem01ControlBlock;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void TaskMain(void const * argument);
extern void TaskSchedLP(void const * argument);
extern void TaskSchedHP(void const * argument);
extern void TaskMessaging(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  stackmon_register("Main", mainTaskBuffer, sizeof(mainTaskBuffer));
  stackmon_register("Job Queue Low", schedLowBuffer, sizeof(schedLowBuffer));
  stackmon_register("Job Queue High", schedHighBuffer, sizeof(schedHighBuffer));
  stackmon_register("Messaging", msgTaskBuffer, sizeof(msgTaskBuffer));
  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of mutTinyFrameTx */
  osMutexStaticDef(mutTinyFrameTx, &myMutex01ControlBlock);
  mutTinyFrameTxHandle = osMutexCreate(osMutex(mutTinyFrameTx));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of semVcomTxReady */
  osSemaphoreStaticDef(semVcomTxReady, &myBinarySem01ControlBlock);
  semVcomTxReadyHandle = osSemaphoreCreate(osSemaphore(semVcomTxReady), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  xSemaphoreGive(semVcomTxReadyHandle);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of tskMain */
  osThreadStaticDef(tskMain, TaskMain, osPriorityHigh, 0, STACK_MAIN, mainTaskBuffer, &mainTaskControlBlock);
  tskMainHandle = osThreadCreate(osThread(tskMain), NULL);

  /* definition and creation of tskSchedLP */
  osThreadStaticDef(tskSchedLP, TaskSchedLP, osPriorityLow, 0, STACK_LP, schedLowBuffer, &schedLowControlBlock);
  tskSchedLPHandle = osThreadCreate(osThread(tskSchedLP), NULL);

  /* definition and creation of tskSchedHP */
  osThreadStaticDef(tskSchedHP, TaskSchedHP, osPriorityAboveNormal, 0, STACK_HP, schedHighBuffer, &schedHighControlBlock);
  tskSchedHPHandle = osThreadCreate(osThread(tskSchedHP), NULL);

  /* definition and creation of TaskMessaging */
  osThreadStaticDef(tskMsg, TaskMessaging, osPriorityNormal, 0, STACK_MSG, msgTaskBuffer, &msgTaskControlBlock);
  tskMsgHandle = osThreadCreate(osThread(tskMsg), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of queSchedLP */
  osMessageQStaticDef(queSchedLP, LP_SCHED_CAPACITY, struct sched_que_item, myQueue01Buffer, &myQueue01ControlBlock);
  queSchedLPHandle = osMessageCreate(osMessageQ(queSchedLP), NULL);

  /* definition and creation of queSchedHP */
  osMessageQStaticDef(queSchedHP, HP_SCHED_CAPACITY, struct sched_que_item, myQueue02Buffer, &myQueue02ControlBlock);
  queSchedHPHandle = osMessageCreate(osMessageQ(queSchedHP), NULL);

  /* definition and creation of queRxData */
  osMessageQStaticDef(queRxData, RX_QUE_CAPACITY, struct rx_que_item, myQueue03Buffer, &myQueue03ControlBlock);
  queRxDataHandle = osMessageCreate(osMessageQ(queRxData), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* TaskMain function */
__weak void TaskMain(void const * argument)
{

  /* USER CODE BEGIN TaskMain */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END TaskMain */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
