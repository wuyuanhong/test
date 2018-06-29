#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		150  
//������
TaskHandle_t LED0Task_Handler;
//������
void led0_task(void *pvParameters);

//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		150  
//������
TaskHandle_t LED1Task_Handler;
//������
void led1_task(void *pvParameters);

//�������ȼ�
#define RUNSTATE_TASK_PRIO		4
//�����ջ��С	
#define RUNSTATE_STK_SIZE 		256
//������
TaskHandle_t RUNSTATETask_Handler;
//������
void runstate_task(void *pvParameters);




//����һ�����͵�����1�Ķ���
#define MESSAGE_Q_NUM   4   	//�������ݵ���Ϣ���е����� 
QueueHandle_t Usart1_Queue;   		//����ֵ��Ϣ���о��







int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
	LED_Init();		        //��ʼ��LED�˿�
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}
 
//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����LED0����
		Usart1_Queue=xQueueCreate(MESSAGE_Q_NUM,sizeof(u8));        //������ϢKey_Queue
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);        
    //�����������
    xTaskCreate((TaskFunction_t )runstate_task,     
                (const char*    )"runstate_task",   
                (uint16_t       )RUNSTATE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RUNSTATE_TASK_PRIO,
                (TaskHandle_t*  )&RUNSTATETask_Handler);  
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//LED0������ 
void led0_task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(1);
    }
}   

//LED1������
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

//����״̬
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
