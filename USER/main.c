#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		150  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void led0_task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		150  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void led1_task(void *pvParameters);

//任务优先级
#define RUNSTATE_TASK_PRIO		4
//任务堆栈大小	
#define RUNSTATE_STK_SIZE 		256
//任务句柄
TaskHandle_t RUNSTATETask_Handler;
//任务函数
void runstate_task(void *pvParameters);




//创建一个发送到串口1的队列
#define MESSAGE_Q_NUM   4   	//发送数据的消息队列的数量 
QueueHandle_t Usart1_Queue;   		//按键值消息队列句柄







int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}
 
//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LED0任务
		Usart1_Queue=xQueueCreate(MESSAGE_Q_NUM,sizeof(u8));        //创建消息Key_Queue
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);        
    //浮点测试任务
    xTaskCreate((TaskFunction_t )runstate_task,     
                (const char*    )"runstate_task",   
                (uint16_t       )RUNSTATE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RUNSTATE_TASK_PRIO,
                (TaskHandle_t*  )&RUNSTATETask_Handler);  
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//LED0任务函数 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(1);
    }
}   

//LED1任务函数
void led1_task(void *pvParameters)
{
	int j=0;
	float float_num=0.00;
    while(1)
    {	
				j++;
        LED1=~LED1;
				float_num+=1;
        vTaskDelay(1);
			
    }
}

//任务状态
void runstate_task(void *pvParameters)
{
  char *temp;
	char *temp1;
	temp=( char * )pvPortMalloc(2000);
	//temp1=( char * )pvPortMalloc(20000);
	while(1)
	{
	
	//	vTaskList(temp1);
		vTaskGetRunTimeStats(temp);
	//	printf("%s\r\n",temp1);
		printf("%s\r\n",temp);
    vTaskDelay(1000);
	}
}
