#include "sys.h"
  /**************************************************************************
���ߣ�ƽ��С��֮��
ע�⣺����Ŀ�ǲο�ƽ��С��֮�ҵĴ����д�ģ�����ԭ������Ϣ����ԭ�������ء�
**************************************************************************/
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //����ң����صı���
u8 Flag_Stop=1,Flag_Show=1; 
int Encoder_Left,Encoder_Right;             //���ұ��������������
int Moto1,Moto2; //���PWM���� 
int Temperature;                            //��ʾ�¶�
int Voltage;                                //��ص�ѹ������صı���
float Angle_Balance,Gyro_Balance,Gyro_Turn; //ƽ����� ƽ�������� ת��������
u32 Distance;                               //���������
int main(void)
{ 
	Stm32_Clock_Init(9);            //ϵͳʱ������
	delay_init(72);                 //��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
	LED_Init();                     //��ʼ���� LED ���ӵ�Ӳ���ӿ�
	LED=1;
	KEY_Init();                     //������ʼ��
	OLED_Init();                    //OLED��ʼ��
	uart_init(72,115200);           //��ʼ������1
	uart3_init(9600);            //����3��ʼ��
	Adc_Init();                     //ADC��ʼ��
	MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ ��Ƶ���Է�ֹ�����Ƶʱ�ļ����
	Encoder_Init_TIM2();            //��ʼ��������1 
	Encoder_Init_TIM4();            //��ʼ��������2 
	IIC_Init();                     //ģ��IIC��ʼ��
  MPU6050_initialize();           //=====MPU6050��ʼ��	
	DMP_Init();                     //��ʼ��DMP     
  Timer1_Init(49,7199);           //=====5MS��һ���жϷ��������жϷ�������control.c
	while(1)
		{
//*****************������̬���ƶ�Ҫ�ڶ�ʱ�жϷ������н��У�whileѭ��ִֻ��һЩ����Ҫ�Ĺ�����oled��ʾ��
			
					delay_ms(500);//��ʱ�������ݴ���Ƶ�ʣ�ȷ��ͨ�ŵ��ȶ�
					//Temperature=Read_Temperature();  //===��ȡMPU6050�����¶ȴ��������ݣ����Ʊ�ʾ�����¶ȡ�	
					oled_show(); //===��ʾ����
					delay_ms(100);	//��ʱ�������ݴ���Ƶ�ʣ�ȷ��ͨ�ŵ��ȶ�			
		}
}
