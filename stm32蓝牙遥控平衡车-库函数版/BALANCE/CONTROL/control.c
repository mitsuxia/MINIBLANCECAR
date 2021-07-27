#include "control.h"	
#include "filter.h"	
  /**************************************************************************
���ߣ�ƽ��С��֮��
ע�⣺����Ŀ�ǲο�ƽ��С��֮�ҵĴ����д�ģ�����ԭ������Ϣ����ԭ�������ء�
**************************************************************************/
#define ZHONGZHI 3 
int Balance_Pwm,Velocity_Pwm,Turn_Pwm;
u8 Flag_Target;
float Acceleration_Z;   
int TIM1_UP_IRQHandler(void)  
{    
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{   
		  TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ                                      //===�����ʱ��1�жϱ�־λ		 
		  Flag_Target=!Flag_Target;
		  if(Flag_Target==1)                                       //5ms��ȡһ�������Ǻͼ��ٶȼƵ�ֵ�����ߵĲ���Ƶ�ʿ��Ը��ƿ������˲���Ч��
			{
			Get_Angle();                                    //===������̬	
			return 0;	
			}                                                        //10ms����һ�Σ�Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
			Encoder_Left=-Read_Encoder(2);                           //===��ȡ��������ֵ����Ϊ�����������ת��180�ȵģ����Զ�����һ��ȡ������֤�������һ��
			Encoder_Right=Read_Encoder(4);                           //===��ȡ��������ֵ
	  	Get_Angle();                                    //===������̬	
  	  Voltage=Get_battery_volt();                              //===��ȡ��ص�ѹ	                                                          
 			Balance_Pwm =balance(Angle_Balance,Gyro_Balance);        //===ƽ��PID����	
		  Velocity_Pwm=velocity(Encoder_Left,Encoder_Right);       //===�ٶȻ�PID����	 ��ס���ٶȷ�����������������С�����ʱ��Ҫ����������Ҫ���ܿ�һ�㣬��˼����׷����б���ơ�
 	    Turn_Pwm =turn(Encoder_Left,Encoder_Right,Gyro_Turn); //===ת��PID����     
 		  Moto1=Balance_Pwm+Velocity_Pwm-Turn_Pwm;                 //===�������ֵ������PWM
 	  	Moto2=Balance_Pwm+Velocity_Pwm+Turn_Pwm;                 //===�������ֵ������PWM
   		Xianfu_Pwm();                                            //===PWM�޷�
      if(Turn_Off(Angle_Balance)==0)                  //===����������쳣����˼�����û���������¹ʣ��ٸ�ֵ��PWM�ļĴ���CCR
 			Set_Pwm(Moto1,Moto2);                                    //===��ֵ��PWM�Ĵ���  
	}       	
	 return 0;	  
} 
 
/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int balance(float Angle,float Gyro)
{  
   float Bias,kp=-630,kd=-0.9 ;//300  580  1.1   -720  -1.3
	 int balance;
	 Bias=Angle-ZHONGZHI;              //===���ƽ��ĽǶ���ֵ �ͻ�е��� -0��ζ������������0�ȸ��� �������������5�ȸ��� �Ǿ�Ӧ�ü�ȥ5
	 balance=kp*Bias+Gyro*kd;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ����޸�Movement��ֵ�����磬�ĳ�-60��60�ͱȽ�����
��ڲ��������ֱ����������ֱ�����
����  ֵ���ٶȿ���PWM
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int velocity(int encoder_left,int encoder_right)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Encoder_Integral;
	  float kp=200,ki=0.9;    //150  0.7  -50  -0.2
	  static float last_Encoder;	
	  //=============ң��ǰ�����˲���=======================//
		if(1==Flag_Qian)	Movement=100/Flag_sudu;	             //===���ǰ����־λ��1 λ��Ϊ��250
		else if(1==Flag_Hou)	  Movement=-160/Flag_sudu;        //===������˱�־λ��1 λ��Ϊ��
	  else  Movement=0;	
    //=============�ٶ�PI������=======================//	
		Encoder_Least =(Encoder_Left+Encoder_Right)-0;  //===��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶȣ��˴�Ϊ�㣩 
		//Encoder *= 0.8;		                         //===һ�׵�ͨ�˲���       
		//Encoder += Encoder_Least*0.2;	             //===һ�׵�ͨ�˲���   
	  Encoder=Encoder_Least*0.1+last_Encoder*0.9;  //===һ�׵�ͨ�˲�������һ��д��������ʱ�����ȶ��ԱȽϺþ������֡�
		Encoder_Integral +=Encoder;                                  //===���ֳ�λ�� ����ʱ�䣺10ms
		Encoder_Integral=Encoder_Integral-Movement;                  //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>10000)  	Encoder_Integral=10000;         //===�����޷�
		if(Encoder_Integral<-10000)	Encoder_Integral=-10000;         //===�����޷�	
		Velocity=Encoder*kp+Encoder_Integral*ki; //===�ٶȿ���	
	  last_Encoder=Encoder;
		if(Turn_Off(Angle_Balance)==1||Flag_Stop==1)   Encoder_Integral=0;     //===����رպ��������
	  return Velocity;
	 
}

/**************************************************************************
�������ܣ�ת��PD����
��ڲ��������ֱ����������ֱ�������Z��������
����  ֵ��ת�����PWM.7
��    �ߣ�ƽ��С��֮��
**************************************************************************/
int turn(int encoder_left,int encoder_right,float gyro)//ת�����
{
	  static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
	  float Turn_Amplitude=150/Flag_sudu,Kp=42,Kd=0;     //42 0
	  //=============ң��������ת����=======================//
  	if(1==Flag_Left||1==Flag_Right)                      //��һ������Ҫ�Ǹ�����תǰ���ٶȵ����ٶȵ���ʼ�ٶȣ�����С������Ӧ��
		{
			if(++Turn_Count==1)
			Encoder_temp=myabs(encoder_left+encoder_right);
			Turn_Convert=50/Encoder_temp;
			if(Turn_Convert<0.6)Turn_Convert=0.6;
			if(Turn_Convert>3)Turn_Convert=3;
		}	
	  else
		{
			Turn_Convert=0.9;
			Turn_Count=0;
			Encoder_temp=0;
		}			
		if(1==Flag_Left)	           Turn_Target-=Turn_Convert;
		else if(1==Flag_Right)	     Turn_Target+=Turn_Convert; 
		else Turn_Target=0;
	
    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===ת���ٶ��޷�
	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
		if(Flag_Qian==1||Flag_Hou==1)  Kd=1;        
		else Kd=0;   //ת���ʱ��ȡ�������ǵľ��� �е�ģ��PID��˼��
  	//=============ת��PD������=======================//
		  Turn=-Turn_Target*Kp -gyro*Kd;                 //===���Z�������ǽ���PD����
	  return Turn;
}

/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ���������PWM������PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int moto1,int moto2)
{
			if(moto1>0)			AIN2=1,			AIN1=0;
			else 	          AIN2=0,			AIN1=1;
			PWMA=myabs(moto1);
		  if(moto2>0)	BIN1=0,			BIN2=1;
			else        BIN1=1,			BIN2=0;
			PWMB=myabs(moto2);
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{	
	  int Amplitude=7100;    //===PWM������7200 ������7100
    if(Moto1<-Amplitude) Moto1=-Amplitude;	
		if(Moto1>Amplitude)  Moto1=Amplitude;	
	  if(Moto2<-Amplitude) Moto2=-Amplitude;	
		if(Moto2>Amplitude)  Moto2=Amplitude;		
	
}
/**************************************************************************
�������ܣ������޸�С������״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)//��������û�õ�
{	
	u8 tmp;
	tmp=click_N_Double(50); 
	if(tmp==1)Flag_Stop=~Flag_Stop;
	if(tmp==2)Flag_Show=~Flag_Show;
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������Ǻ͵�ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off(float angle)
{
	    u8 temp;
if(angle<-35||angle>35||Flag_Stop==1)
			{	                                                 //===��Ǵ���40�ȹرյ��
      temp=1;
			AIN1=0;                                            //===���������������¶ȹ���ʱ�رյ��
			AIN2=0;
			BIN1=0;
			BIN2=0;
      }
			else
      temp=0;
			Flag_Stop=0;	
      return temp;			
}


	
/**************************************************************************
�������ܣ���ȡ�Ƕ�
��ȡ�Ƕȵ��㷨���������˲������ü��ɣ�����̫��������㷨�����ص����PID���֡�
����  ֵ����
**************************************************************************/
//void Get_Angle(u8 way)
void Get_Angle()
{ 
	    float Accel_Y,Accel_X,Accel_Z,Gyro_Y,Gyro_Z;
			Gyro_Y=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_YOUT_L);    //��ȡY��������
			Gyro_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_GYRO_ZOUT_L);    //��ȡZ��������
		  Accel_X=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_XOUT_L); //��ȡX����ٶȼ�
	  	Accel_Z=(I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+I2C_ReadOneByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //��ȡZ����ٶȼ�
		  if(Gyro_Y>32768)  Gyro_Y-=65536;     //��������ת��  Ҳ��ͨ��shortǿ������ת��
			if(Gyro_Z>32768)  Gyro_Z-=65536;     //��������ת��
	  	if(Accel_X>32768) Accel_X-=65536;    //��������ת��
		  if(Accel_Z>32768) Accel_Z-=65536;    //��������ת��
			Gyro_Balance=-Gyro_Y;                                  //����ƽ����ٶ�
	   	Accel_Y=atan2(Accel_X,Accel_Z)*180/PI;                 //���������ļнǣ��õ��������ٶ�	
		  Gyro_Y=Gyro_Y/16.4;                                    //����������ת��	  	
			Kalman_Filter(Accel_Y,-Gyro_Y);                        //�������˲���ȡ��ѵĽǶ�ֵ
			
	    Angle_Balance=angle;                                   //����ƽ�����
			Gyro_Turn=Gyro_Z;                                      //����ת����ٶ�
			Acceleration_Z=Accel_Z;                                //===����Z����ٶȼ�	
}
/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����int
����  ֵ��unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}





