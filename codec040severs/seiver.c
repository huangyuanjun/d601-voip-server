#include<sys/types.h>  
#include<sys/socket.h>  
#include<stdio.h>  
#include<sys/un.h>  
#include<unistd.h>  
#include<stdlib.h>  
#include <stddef.h>  
#include <pthread.h>  
#include <sys/ioctl.h>
#include <sys/time.h>
#include <signal.h>
pthread_t tid;
#define SERVER_NAME "@codec040_socket"  
#include "./codec/codec_set.c"
#include "./powerspeaker/powerspeaker.c"


/*
*·��Ŀ��
*��ʼ��Ĭ������
*���ߣ�jocker
*date:2016-12-1
*/
#define CODEC_F_W_FILE_PATH "/system/bin/ZLSApp_v1.1.6.s3"
#define CODEC_C_F_G_FILE_PATH "/system/bin/ZLSvoipcfg.cr2"


/*
*·������
*�ⲿ�ֺ궨����Ҫ����
*1��ϵͳĬ����Ƶ���õ�·������
*���ߣ�jocker
*date :2016-11-28
*/
#define SPEAKER_NORMAL_ROUTE "/system/bin/SPEAKER_NORMAL_ROUTE.cr2"
#define SPEAKER_INCALL_ROUTE "/system/bin/SPEAKER_INCALL_ROUTE.cr2"
#define SPEAKER_VOIP_ROUTE "/system/bin/SPEAKER_VOIP_ROUTE.cr2"
#define SPEAKER_PSTN_ROUTE "/system/bin/SPEAKER_PSTN_ROUTE.cr2"
#define HEADPHONE_PSTN_ROUTE "/system/bin/HEADPHONE_PSTN_ROUTE.cr2"
#define MAIN_MIC_CAPTURE_ROUTE "/system/bin/MAIN_MIC_CAPTURE_ROUTE.cr2"
#define HEADSET_MIC_CAPTURE_ROUTE "/system/bin/HEADSET_MIC_CAPTURE_ROUTE.cr2"

#define SPEAKER_RINGTONE_ROUTE "/system/bin/SPEAKER_RINGTONE_ROUTE.cr2"
#define HEADPHONE_NORMAL_ROUTE "/system/bin/HEADPHONE_NORMAL_ROUTE.cr2"

#define HEADPHONE_INCALL_ROUTE "/system/bin/HEADPHONE_INCALL_ROUTE.cr2"

#define HEADPHONE_RINGTONE_ROUTE "/system/bin/HEADPHONE_RINGTONE_ROUTE.cr2"

#define HEADPHONE_VOIP_ROUTE "/system/bin/HEADPHONE_VOIP_ROUTE.cr2"


/***************************************************************************
*
*
*�깦�ܣ�
*�ⲿ�ֶ�����Ҫ����
*1���������õ�����·����ͬ�豸·���в���
*���ߣ�jocker
*date:2016-11-29
****************************************************************************/
#define NEW_SPEAKER_NORMAL_ROUTE  "SPEAKER_NORMAL_ROUTE.cr2"
#define NEW_SPEAKER_INCALL_ROUTE  "SPEAKER_INCALL_ROUTE.cr2"
#define NEW_SPEAKER_VOIP_ROUTE  "SPEAKER_VOIP_ROUTE.cr2"
#define NEW_SPEAKER_PSTN_ROUTE  "SPEAKER_PSTN_ROUTE.cr2"
#define NEW_HEADPHONE_PSTN_ROUTE  "HEADPHONE_PSTN_ROUTE.cr2"
#define NEW_MAIN_MIC_CAPTURE_ROUTE  "MAIN_MIC_CAPTURE_ROUTE.cr2"
#define NEW_HEADSET_MIC_CAPTURE_ROUTE  "HEADSET_MIC_CAPTURE_ROUTE.cr2"
#define NEW_SPEAKER_RINGTONE_ROUTE  "SPEAKER_RINGTONE_ROUTE.cr2"
#define NEW_HEADPHONE_NORMAL_ROUTE  "HEADPHONE_NORMAL_ROUTE.cr2"
#define NEW_HEADPHONE_INCALL_ROUTE  "HEADPHONE_INCALL_ROUTE.cr2"
#define NEW_HEADPHONE_RINGTONE_ROUTE  "HEADPHONE_RINGTONE_ROUTE.cr2"
#define NEW_HEADPHONE_VOIP_ROUTE  "HEADPHONE_VOIP_ROUTE.cr2"
char cfgPATHtemp[20];   //�ַ�����ʱ���������ڽ��������õ�·��
char cfgPATH[100];       //������·�������ڱ���·��  "cfgPATHtemp" + NEW_XXXXXXXXXXXX
int hascfgflg = 0;       //�Ƿ��������ô��ڣ�0Ϊ�����ڣ�1Ϊ����
int voip_calling = 0;
int voice_route = 0;
#define HEADSET_MODDEL  1
#define SPEAKER_MODDEL  2
#define BLUETOOTH_SOC_MIC_CAPTURE_ROUTE "/system/bin/BLUETOOTH_SOC_MIC_CAPTURE_ROUTE.cr2"



/*
	SPEAKER_NORMAL_ROUTE = 0,
    SPEAKER_INCALL_ROUTE, // 1
    SPEAKER_RINGTONE_ROUTE,
    SPEAKER_VOIP_ROUTE,



    HEADPHONE_NORMAL_ROUTE, // 8
    HEADPHONE_INCALL_ROUTE,
    HEADPHONE_RINGTONE_ROUTE,
    SPEAKER_HEADPHONE_NORMAL_ROUTE,
    SPEAKER_HEADPHONE_RINGTONE_ROUTE,
    HEADPHONE_VOIP_ROUTE,

    HEADSET_NORMAL_ROUTE, // 14
    HEADSET_INCALL_ROUTE,
    HEADSET_RINGTONE_ROUTE,
    HEADSET_VOIP_ROUTE,

    MAIN_MIC_CAPTURE_ROUTE, // 21
    HANDS_FREE_MIC_CAPTURE_ROUTE,
    BLUETOOTH_SOC_MIC_CAPTURE_ROUTE,

    PLAYBACK_OFF_ROUTE, // 24
    CAPTURE_OFF_ROUTE,
    INCALL_OFF_ROUTE,
    VOIP_OFF_ROUTE,
    MAX_ROUTE, // 31
*/
int codec_route = -1;
int codec_value = -1;
int old_route = -1;
int codec040_route = -1;
int codec040_temo_route =-1;
int pstn_val = 3;
int is_pstn_calling = -1;
int is_pstn_hold = -1;
int is_pstn_mute = -1;


void alarm_mute(int val)
{

	pstn2mute(2); //ȡ������mic1

	return ;
}

void alarm_mute1(int val)
{
		pstn2mute(2); //ȡ������mic1
	return ;
}

void mute_delay1(int time_delay)
{
    struct itimerval timeStruct, timeOldVal;

    timeStruct.it_value.tv_sec = 0;
    timeStruct.it_value.tv_usec = 500000;
	timeStruct.it_interval.tv_sec = 0;
    timeStruct.it_interval.tv_usec = 0;
	 signal(SIGALRM, alarm_mute);
    setitimer(ITIMER_REAL, &timeStruct, NULL);
}


void mute_delay(int time_delay)
{
    struct itimerval timeStruct, timeOldVal;

    timeStruct.it_value.tv_sec = time_delay;
    timeStruct.it_value.tv_usec = 0;
	timeStruct.it_interval.tv_sec = 0;
    timeStruct.it_interval.tv_usec = 0;
	 signal(SIGALRM, alarm_mute1);
    setitimer(ITIMER_REAL, &timeStruct, NULL);
}



/********************************************************************************************************
*void codec_send_cmd(int codec_route)
*���ܣ�����codec_routeֵ�л�����״̬
*���ߣ�jocker
*���ڣ�2016-12-1
******************************************************************************************************/
void codec_send_cmd(int codec_route)
{

	//usleep(100000);
	reset_dsp();

	switch (codec_route)
	{
    //�������ģʽ
	case 0:
				printf("-----------�������ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
				if (is_pstn_calling == 1)
					{
						printf("-----------�������ģʽ�����Ǵ�ʱ���ڽ���ģ��ͨ������������--------------\n");
						break;
					}
						if (hascfgflg == 1)
						{
							memset(cfgPATH,0,100);
							sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
							lowd_cfg(cfgPATH);

						}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);

						}								
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
				break;
	//����ͨ��ģʽ
	case 1:
				printf("-----------����ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							if (old_route == 9 || old_route == 15 )
							{
								//fix me ����ͨ���л�
							printf("-----------�л�������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
								 if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_INCALL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								 lowd_cfg(SPEAKER_INCALL_ROUTE);
									}	
							}else{
									 if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_INCALL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_INCALL_ROUTE);
									}	
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
							printf("-----------��������ͨ��ģʽ����-----------codec_route = %d-----------------------\n",codec_route);
							}

							incall_model();
								
				break;
	//��������ģʽ
	case 2:
				printf("-----------��������ģʽ-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
									if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);
									}	
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
				break;
	//��������ͨ��ģʽ
	case 3:
							printf("-----------��������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);

								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);

							printf("-----------������������ͨ��ģʽ����-----------codec_route = %d-----------------------\n",codec_route);
							
				break;
	//EARPIECE_NORMAL_ROUTE
	case 4:
			//	printf("-----------EARPIECE_NORMAL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
				break;
	//EARPIECE_INCALL_ROUTE
	case 5:
			//	printf("-----------EARPIECE_INCALL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
				break;
	//EARPIECE_RINGTONE_ROUTE
	case 6:
			//	printf("-----------EARPIECE_RINGTONE_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
				break;
	//EARPIECE_VOIP_ROUTE
	case 7:
			//	printf("-----------EARPIECE_VOIP_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
				break;
	//HEADPHONE_NORMAL_ROUTE
	case 8:
				printf("-----------HEADPHONE_NORMAL_ROUTE------old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (is_pstn_calling == 1)
							{
								printf("-----------�������ģʽ�����Ǵ�ʱ���ڽ���ģ��ͨ������������--------------\n");
								break;
							}
							if (old_route == 0 )
							{
								printf("-----------����ͨ���л�������-----------codec_route = %d-----------------------\n",codec_route);
								//fix me ����ͨ���л�
								//speaker_on();
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);

							}else{
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
							}
				break;
	//HEADPHONE_INCALL_ROUTE
	case 9:
				printf("-----------HEADPHONE_INCALL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 1 )
							{
								//fix me ����ͨ���л�
								printf("-----------ԭ��������ͨ��ģʽ������ֻ��ͨ��-----------codec_route = %d-----------------------\n",codec_route);
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
								 if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_INCALL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								 lowd_cfg(HEADPHONE_INCALL_ROUTE);
									}
							}else if(old_route == 15){
								//
								printf("-----------ԭ�����ڶ���ģʽ������ɶҲ����-----------codec_route = %d-----------------------\n",codec_route);
							}else{
							 if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_INCALL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
							lowd_cfg(HEADPHONE_INCALL_ROUTE);
									}
							 pstn2mute(3);
							speaker_off();
							 pstn2mute(2);
							printf("-----------����ģʽ�е�����ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							
							}
							incall_model();
				break;
	//HEADPHONE_RINGTONE_ROUTE
	case 10:
				printf("-----------HEADPHONE_RINGTONE_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
								//fix me �л�����ͨ��
				break;
	//SPEAKER_HEADPHONE_NORMAL_ROUTE
	case 11:
				printf("-----------SPEAKER_HEADPHONE_NORMAL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
								//fix me �л�����ͨ��
								  if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								 lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
				break;
	//SPEAKER_HEADPHONE_RINGTONE_ROUTE
	case 12:
				printf("-----------SPEAKER_HEADPHONE_RINGTONE_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
								//fix me �л�����ͨ��
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
										lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
				break;

	//HEADPHONE_VOIP_ROUTE
	case 13:
				printf("-----------HEADPHONE_VOIP_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
								//fix me �л�����ͨ��
							if (old_route == 17 || old_route == 3 )
							{
								//fix me ����ͨ���л�
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
								printf("-----------����ͨ���л�����������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							}else{
									if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
								printf("-----------����ģʽ�л�����������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							}
				break;

	//�������ģʽ
	case 14:
			//	printf("-----------�������ģʽ-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
				printf("-----------�������ģʽ------old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (is_pstn_calling == 1)
							{
								printf("-----------�������ģʽ�����Ǵ�ʱ���ڽ���ģ��ͨ������������--------------\n");
								break;
							}
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(HEADPHONE_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);

				break;

	//����ͨ��ģʽ
	case 15:
				/*printf("-----------����ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 1 )
							{
								//fix me ����ͨ���л�
								printf("-----------ԭ����������ͨ��ģʽ��ֻ��ͨ���л�-----------codec_route = %d-----------------------\n",codec_route);
							}else if(old_route == 9){
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								
								printf("-----------ԭ�����Ƕ���ͨ��ģʽ��ɶҲ����-----------codec_route = %d-----------------------\n",codec_route);
							}else{
							
								printf("-----------����ģʽ�л�������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							}*/
				printf("-----------HEADPHONE_INCALL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 1 )
							{
								//fix me ����ͨ���л�
								printf("-----------ԭ��������ͨ��ģʽ������ֻ��ͨ��-----------codec_route = %d-----------------------\n",codec_route);
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_INCALL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								 lowd_cfg(HEADPHONE_INCALL_ROUTE);
									}
							}else if(old_route == 9){
								//
								printf("-----------ԭ�����ڶ���ģʽ������ɶҲ����-----------codec_route = %d-----------------------\n",codec_route);
							}else{
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_INCALL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
							lowd_cfg(HEADPHONE_INCALL_ROUTE);
									}
							 pstn2mute(3);
							speaker_off();
							 pstn2mute(2);
							printf("-----------����ģʽ�е�����ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							
							}
							incall_model();

				break;
	//��������ģʽ
	case 16:
				printf("-----------��������ģʽ-----------codec_route = %d-----------------------\n",codec_route);
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
				break;
	
	//��������ͨ��ģʽ
	case 17:	
				/*printf("-----------��������ͨ��ģʽ------old_route = %d -----codec_route = %d-----------------------\n",old_route,codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 13 || old_route == 3 )
							{
								printf("-----------����ͨ���л�������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
								//fix me ����ͨ���л�
							}else{
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								printf("-----------����ģʽ�л�������ͨ��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
							}*/
					printf("-----------HEADPHONE_VOIP_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(HEADPHONE_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
		
				break;
	
	//����¼��ģʽ
	case 21:	
		//		printf("-----------����¼��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
				printf("����¼��-----old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
								if (voip_calling==1)
								{
								}else{
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
								}
								 
				break;
	
	//����¼��ģʽ
	case 22:
			//	printf("-----------����¼��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
				printf("����¼��-----old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
							if (voip_calling == 1)
							{

							}else{
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(HEADPHONE_VOIP_ROUTE);
									}
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
							}

				break;

	//����¼��ģʽ
	case 23:
			//	printf("-----------����¼��ģʽ-----------codec_route = %d-----------------------\n",codec_route);
				break;

	//��Ž���
	case 24:
			//	printf("-----------��Ž���-----------codec_route = %d-----------------------\n",codec_route);
				break;

	//¼������
	case 25:
	 	//		printf("-----------¼������-----------codec_route = %d-----------------------\n",codec_route);

				printf("¼������-----old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
				if (old_route ==21)
				{
							 if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
							lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
				}else if (old_route ==22)
				{
							 if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
							lowd_cfg(HEADPHONE_VOIP_ROUTE);
									}
								pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
				}

				break;
	//ͨ������
	case 26:
				printf("-----------ͨ������-----------codec_route = %d-----------------------\n",codec_route);
						//	lowd_cfg(SPEAKER_PSTN_ROUTE);
						if (old_route == 1)  //ԭ��������ͨ���������л���������ģʽ
							{
								//fix me ����ͨ���л�
								printf("-----------ԭ��������ͨ���������л���������ģʽ---------codec_route = %d-----------------------\n",codec_route);
								//codec_route = 0 ;
								 pstn2mute(3);
							//	 sleep(1);
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								speaker_on();
								 pstn2mute(2);
							}else if (old_route == 9){   //ԭ���Ƕ���ͨ���������лض�������ģʽ
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								printf("-----------ԭ���Ƕ���ͨ���������лض�������ģʽ---------codec_route = %d-----------------------\n",codec_route);
								//codec_route = 8 ;
								 pstn2mute(3);
								speaker_off();
								 
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
								lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
								pstn2mute(2);
								
							}
							endcall_model();
						
				break;

	//����ͨ������
	case 27:
				printf("-----------����ͨ������-----------codec_route = %d-----------------------\n",codec_route);
							//lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 26)
							{

								
							}else{
							
								if (old_route == 3)  //ԭ������������ͨ���������л���������ģʽ
								{
									//fix me ����ͨ���л�
									printf("-----------ԭ������������ͨ���������л���������ģʽ---------codec_route = %d-----------------------\n",codec_route);
									// codec_route = 0 ;
									
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
									lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
									 pstn2mute(3);
									speaker_on();
									 pstn2mute(2);
								}else if (old_route == 13 || old_route == 17 ){   //ԭ���Ƕ�������ͨ���������лض�������ģʽ
									 pstn2mute(3);
									speaker_off();
									 pstn2mute(2);
								if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
									lowd_cfg(SPEAKER_VOIP_ROUTE);
									}
									printf("-----------ԭ���Ƕ�������ͨ���������лض�������ģʽ--------codec_route = %d-----------------------\n",codec_route);
									//	codec_route = 8 ;
										
								}
							
							}
							
				break;
		case 31:						//PSTN SPEAKER
								//printf("PSTN SPEAKER\n");
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
								//pstn_status = 1;
								pstn2mute(3);
							printf("-----------PSTN SPEAKER--------codec_route = %d-----------------------\n",codec_route);
							if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_PSTN_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
										lowd_cfg(SPEAKER_PSTN_ROUTE);
									}
							printf("-----------��ǰ״̬ģ��绰����ͨ��----------\n");
							is_pstn_calling = 1;
							//inpstn_model();
							// pstn2mute(3);
							set_speaker(pstn_val);
								 
								speaker_on();
								mute_delay1(1);
								 //pstn2mute(2);
								//speaker_on();
						//pstn2mute(2);

		break;
	    case 32:						//PSTN HEADSET
								//printf("PSTN HEADSET\n");
							//	lowd_cfg(HEADPHONE_PSTN_ROUTE);
							pstn2mute(3);
							printf("-----------PSTN HEADSET--------codec_route = %d-----------------------\n",codec_route);
							if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_PSTN_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
										lowd_cfg(HEADPHONE_PSTN_ROUTE);
									}
							printf("-----------��ǰ״̬ģ��绰�ֱ�ͨ��----------\n");
							is_pstn_calling = 1;
							set_speaker(pstn_val);
							//inpstn_model();
							speaker_off();
							pstn2mute(2);
								//pstn_status = 1;
		break;
		case 33:						//PSTN VAL 0
							printf("-----------PSTN VAL 0--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//��������
								printf("-----------��ǰ״̬ģ��绰ͨ��������������С 0----------\n");
								set_speaker(0);
							}else{
								//���浱ǰ������С���´�ͨ��ʱʹ��
								pstn_val = 0;
								printf("-----------��ǰ״̬ģ��绰δͨ��������������С 0----------\n");
							}
							
		break;
		case 34:						//PSTN VAL 1
							printf("-----------PSTN VAL 1--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//��������
								printf("-----------��ǰ״̬ģ��绰ͨ��������������С 1----------\n");
								set_speaker(1);
							}else{
								//���浱ǰ������С���´�ͨ��ʱʹ��
								pstn_val = 1;
								printf("-----------��ǰ״̬ģ��绰δͨ��������������С 1----------\n");
							}
							
		break;
		case 35:						//PSTN VAL 2
							printf("-----------PSTN VAL 2--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//��������
								printf("-----------��ǰ״̬ģ��绰ͨ��������������С 2----------\n");
								set_speaker(2);
							}else{
								//���浱ǰ������С���´�ͨ��ʱʹ��
								pstn_val = 2;
								printf("-----------��ǰ״̬ģ��绰δͨ��������������С 2----------\n");
							}
		break;
		case 36:						//PSTN VAL 3
							printf("-----------PSTN VAL 3--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//��������
								printf("-----------��ǰ״̬ģ��绰ͨ��������������С 3----------\n");
								set_speaker(3);
							}else{
								//���浱ǰ������С���´�ͨ��ʱʹ��
								pstn_val = 3;
								printf("-----------��ǰ״̬ģ��绰δͨ��������������С 3----------\n");
							}
		break;
		case 37:						//PSTN MUTE
							printf("-----------PSTN MUTE--------codec_route = %d-----------------------\n",codec_route);
							is_pstn_mute = 1;
							if (is_pstn_calling == 1)
							{
								//��������
								if (is_pstn_hold == 1)
								{
									pstn2mute(3);
									printf("-----------��ǰ״̬ģ��绰ͨ����MUTE ������֮ǰ�Ѵ���HOLD״̬----------\n");
								}else{
									pstn2mute(1);
									printf("-----------��ǰ״̬ģ��绰ͨ����MUTE ״̬----------\n");
								}

								
							}else{
								//pstnδͨ������������
								
							}
		break;
		case 38:						//PSTN NO MUTE
							printf("-----------PSTN NO MUTE--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//��������
								is_pstn_mute = 0;
								if (is_pstn_hold == 1)
								{
									pstn2mute(3);
									printf("-----------��ǰ״̬ģ��绰ͨ���� NO MUTE ������֮ǰ�Ѵ���HOLD״̬----------\n");
								}else{
								
									pstn2mute(2);
									printf("-----------��ǰ״̬ģ��绰ͨ����NO MUTE----------\n");
								}
								
							}else{
								//pstnδͨ������������
								
							}
		break;
		case 39:						//PSTN HOLD
							printf("-----------/PSTN HOLD--------codec_route = %d-----------------------\n",codec_route);
							is_pstn_hold = 1;
							if (is_pstn_calling == 1)
							{
								//��������
								pstn2mute(3);
								printf("-----------��ǰ״̬ģ��绰ͨ����HOLD״̬----------\n");
							}else{
								//pstnδͨ������������
								
							}
		break;
		case 40:						//PSTN NO HOLD
							printf("-----------/PSTN NO HOLD--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//��������
								is_pstn_hold = 0;
								if (is_pstn_mute == 1)
								{
									pstn2mute(1);
									printf("-----------��ǰ״̬ģ��绰ͨ���� NO HOLD ������֮ǰ�Ѵ���MUTE״̬----------\n");
								}else{
									pstn2mute(2);
									printf("-----------��ǰ״̬ģ��绰ͨ����NO HOLD״̬----------\n");
								}
								
							}else{
								//pstnδͨ������������
								
							}
		break;
		case 41:						//PSTN OFF
							printf("-----------/PSTN OFF--------codec_route = %d-----------------------\n",codec_route);
								is_pstn_calling = 0;
								pstn2mute(3);
								/*
								*fix me   ��ѯ����״̬������������������������
								*/
								if (voice_route == HEADSET_MODDEL)
								{
											if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_HEADPHONE_VOIP_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
										lowd_cfg(HEADPHONE_VOIP_ROUTE);
									}
									 pstn2mute(3);
									 speaker_off();
									 pstn2mute(2);
								}else if (voice_route == SPEAKER_MODDEL)
								{
									if (hascfgflg == 1)
									{
										memset(cfgPATH,0,100);
										sprintf(cfgPATH,"%s%s",cfgPATHtemp,NEW_SPEAKER_NORMAL_ROUTE);
										lowd_cfg(cfgPATH);
									}else{
										lowd_cfg(SPEAKER_NORMAL_ROUTE);
									}
								    pstn2mute(3);
								    speaker_on();
								    pstn2mute(2);
								}

								//endpstn_model();
								pstn2mute(2);
		break;
		case 42:						//VOIP reset cedec
							printf("-----------VOIP reset cedec--------codec_route = %d-----------------------\n",codec_route);
								reset_dsp();
		break;
		case 43:						//pstn flash 1000ms
							printf("-----------pstn flash 1000ms--------codec_route = %d-----------------------\n",codec_route);
								mute_delay(2);
		break;
		case 44:						//flash other time
							printf("-----------flash other time--------codec_route = %d-----------------------\n",codec_route);
								mute_delay(1);
		break;
	default:
				printf("-----------����ģʽ-----------codec_route = %d-----------------------\n",codec_route);
			
	}

}

/*************************************************************************************************************
*socket���߳�void *client_pthread(void *new_fd)	
*
*���ܣ��������ϵĿͻ���ͨѶ
*���տͻ��˷��͵����ݲ����������װȻ���������route״̬
*���ߣ�jocker
*���ڣ�2016-12-01
*************************************************************************************************************/

void *client_pthread(void *new_fd)	
{
	    int nread,client_sockfd=-1;
		char ch[10240]; 
		int ture =1;
		client_sockfd = (int)new_fd;
		while(ture>0)
	{
			if (client_sockfd < 0)
			{
				ture =0;

			}else{
				printf("%dclient %d connected\n",client_sockfd,new_fd);  
				nread = read(client_sockfd, &ch, 10);  
				 if(nread == 0){//client disconnected  
				   printf("client %d disconnected\n",client_sockfd);  
					 client_sockfd = -1;  
				 }else
					 {  
						if (ch[0] == 'R')
						{
							if (strlen(ch) < 3)
							{
								codec_route = ch[1] - 48;
								
							}else{
								codec_route = (ch[1] - 48)*10 + (ch[2] - 48);
							}
							reset_dsp();
							if (codec_route != old_route)
							{
										

									if ((codec_route ==3)|(codec_route==17))
									{
										voip_calling =1;
									}else if (codec_route == 27)
									{
										voip_calling =0;
									}
									if ((codec_route == 8)|(codec_route == 13)|(codec_route == 14)|(codec_route == 17)|(codec_route == 22))
									{
									voice_route = HEADSET_MODDEL;

									}else if ((codec_route == 0)|(codec_route == 2)|(codec_route == 3)|(codec_route == 11)|(codec_route == 12)|(codec_route == 16)|(codec_route == 21))
									{
									voice_route = SPEAKER_MODDEL;
									
									}
									if ((is_pstn_calling == 1)&&codec_route<31)
									{

									}else{
										codec_send_cmd(codec_route);
									}



									printf("++++++++++++old_route = %d +++++++++++++++++codec_route = %d+++++++++++++++++++++++++++\n",old_route,codec_route);
									old_route = codec_route;
									printf("---------------------old_route = %d ----------------------codec_route = %d---------------------------\n",old_route,codec_route);
	
								
								//codec040_route = codec_route;
							}
							

						}else if (ch[0] == 'V')
						{
							//printf("codec_value = %s\n",ch);
							if (strlen(ch) < 3)
							{
								codec_value = ch[1] - 48;
								
							}else{
								codec_value = (ch[1] - 48)*10 + (ch[2] - 48);
							}
							
							//codec_value = (ch[1] - 48)*100 + (ch[2] - 48)*10 + (ch[3] - 48);
							call_vol(codec_value);
							printf("codec_value = %d\n",codec_value); 
						}else{
			
							printf("codec_value = %s\n",ch);
			
						}

				}  

			}
	}
	pthread_exit(NULL);
	return 0;

}



void *codec_parameter(void *arg)	
{

		int true = 1;
		int pstn_status = -1;
		while(true)
		{

			//printf("pstn_status = %d\n",pstn_status);
			if (codec040_temo_route==codec040_route)
			{
				usleep(50);
			}else if (codec040_route > 30)
			{
					switch (codec040_route)
					{
					
					
					
					case 31:						//PSTN SPEAKER
								printf("PSTN SPEAKER\n");
								lowd_cfg(SPEAKER_PSTN_ROUTE);
								pstn_status = 1;

					break;
					case 32:						//PSTN HEADSET
								printf("PSTN HEADSET\n");
								lowd_cfg(HEADPHONE_PSTN_ROUTE);
								pstn_status = 1;
					break;
					case 33:						//PSTN VAL 0
					break;
					case 34:						//PSTN VAL 1
					break;
					case 35:						//PSTN VAL 2
					break;
					case 36:						//PSTN VAL 3
					break;
					case 37:						//PSTN MUTE
					break;
					case 38:						//PSTN NO MUTE
					break;
					case 39:						//PSTN HOLD
					break;
					case 40:						//PSTN NO HOLD
					break;
					case 41:						//PSTN OFF
									pstn_status = 0;
					break;
					default:
						usleep(50);
					
					}
			}else if(pstn_status == 0){
			
				switch (codec040_route)
				{
					case 0:                        //SPEAKER_NORMAL_ROUTE
								printf("SPEAKER_NORMAL_ROUTE\n");
								lowd_cfg(SPEAKER_NORMAL_ROUTE);
								printf("SPEAKER_NORMAL_ROUTE\n");
					break;
					case 1:							//SPEAKER_INCALL_ROUTE
								printf("SPEAKER_INCALL_ROUTE\n");
					break;
					case 2:							//SPEAKER_RINGTONE_ROUTE
								printf("SPEAKER_RINGTONE_ROUTE\n");
					break;
					case 3:							//SPEAKER_VOIP_ROUTE
								printf("SPEAKER_VOIP_ROUTE\n");
								skype_model();
					break;
					case 4:							//EARPIECE_NORMAL_ROUTE
								printf("EARPIECE_NORMAL_ROUTE\n");
					break;
					case 5:							//EARPIECE_INCALL_ROUTE
								printf("EARPIECE_INCALL_ROUTE\n");
					break;
					case 6:							//EARPIECE_RINGTONE_ROUTE
								printf("EARPIECE_RINGTONE_ROUTE\n");
					break;
					case 7:							//EARPIECE_VOIP_ROUTE
								printf("EARPIECE_VOIP_ROUTE\n");
								skype_model();
					break;
					case 8:							//HEADPHONE_NORMAL_ROUTE
								printf("HEADPHONE_NORMAL_ROUTE\n");
								lowd_cfg(HEADPHONE_NORMAL_ROUTE);
								printf("HEADPHONE_NORMAL_ROUTE\n");
					break;
					case 9:							//HEADPHONE_INCALL_ROUTE
								printf("HEADPHONE_INCALL_ROUTE\n");
					break;
					case 10:						//HEADPHONE_RINGTONE_ROUTE
								printf("HEADPHONE_RINGTONE_ROUTE\n");
					break;
					case 11:						//SPEAKER_HEADPHONE_NORMAL_ROUTE
								printf("SPEAKER_HEADPHONE_NORMAL_ROUTE\n");
					break;
					case 12:						//SPEAKER_HEADPHONE_RINGTONE_ROUTE
								printf("SPEAKER_HEADPHONE_RINGTONE_ROUTE\n");
					break;
					case 13:						//HEADPHONE_VOIP_ROUTE
								printf("HEADPHONE_VOIP_ROUTE\n");
								skype_model();
					break;
					case 14:						//HEADSET_NORMAL_ROUTE
								printf("HEADSET_NORMAL_ROUTE\n");
					break;
					case 15:						//HEADSET_INCALL_ROUTE
								printf("HEADSET_INCALL_ROUTE\n");
					break;
					case 16:						//HEADSET_RINGTONE_ROUTE
								printf("HEADSET_RINGTONE_ROUTE\n");
					break;
					case 17:						//HEADSET_VOIP_ROUTE
								printf("HEADSET_VOIP_ROUTE\n");
								skype_model();
					break;
					case 18:						//BLUETOOTH_NORMAL_ROUTE
								printf("BLUETOOTH_NORMAL_ROUTE\n");
					break;
					case 19:						//BLUETOOTH_INCALL_ROUTE
								printf("BLUETOOTH_INCALL_ROUTE\n");
					break;
					case 20:						//BLUETOOTH_VOIP_ROUTE
								printf("BLUETOOTH_VOIP_ROUTE\n");
								skype_model();
					break;
					case 21:						//MAIN_MIC_CAPTURE_ROUTE
								printf("MAIN_MIC_CAPTURE_ROUTE\n");
								lowd_cfg(MAIN_MIC_CAPTURE_ROUTE);
								printf("MAIN_MIC_CAPTURE_ROUTE\n");
					break;
					case 22:						//HANDS_FREE_MIC_CAPTURE_ROUTE
								printf("HANDS_FREE_MIC_CAPTURE_ROUTE\n");
								lowd_cfg(MAIN_MIC_CAPTURE_ROUTE);
								printf("HANDS_FREE_MIC_CAPTURE_ROUTE\n");
					break;
					case 23:						//BLUETOOTH_SOC_MIC_CAPTURE_ROUTE
								printf("BLUETOOTH_SOC_MIC_CAPTURE_ROUTE\n");
								//lowd_cfg();
					break;
					case 24:						//PLAYBACK_OFF_ROUTE
								printf("PLAYBACK_OFF_ROUTE\n");
					break;
					case 25:						//CAPTURE_OFF_ROUTE
								printf("CAPTURE_OFF_ROUTE\n");
					break;
					case 26:						//INCALL_OFF_ROUTE
								printf("INCALL_OFF_ROUTE\n");
					break;
					case 27:						//VOIP_OFF_ROUTE
								printf("VOIP_OFF_ROUTE\n");
					break;
					case 28:						//HDMI_NORMAL_ROUTE
								printf("HDMI_NORMAL_ROUTE\n");
					break;
					case 29:						//USB_NORMAL_ROUTE
								printf("USB_NORMAL_ROUTE\n");
					break;
					case 30:						//USB_CAPTURE_ROUTE
								printf("USB_CAPTURE_ROUTE\n");
					break;
					
					default:
						usleep(50000);

			
				}	
				codec040_temo_route = codec040_route;
				
			}
		
		}
		pthread_exit(NULL);
		return 0;

}

/***********************************************************************************************
*�����ò��Լ�����
*���ܣ�
*1�����Ե�ǰ��������
*2�����ݻ�������ȥ����Ƿ��пɹ����µ�����
*3����ȡ��������
*4������������·���ṩ����غ���
*5����⵱ǰ���������Ƿ��и��£�����оͽ������
*programmer:jocker
*date:2016-12-01
***********************************************************************************************/
int check_codeccfg_file(void)
{
		int ret = -1;
		//printf("111111111111111111111111111111111111111\n");
		if (access("/data/lcd7.cr2",F_OK)== 0)
			{
					ret = access("/sdcard/.codeccfg/codeccofig.cr2",F_OK);
					if (ret == 0)
					{
						system("busybox cp -r /sdcard/.codeccfg/lcd7  /data");
						system("busybox rm -r /sdcard/.codeccfg/lcd7");
					    system("busybox rm /sdcard/.codeccfg/codeccofig.cr2");
					}
				//printf("222222222222222222222222222222222222222222222\n");
					if ((access("/data/lcd7/HEADPHONE_INCALL_ROUTE.cr2",F_OK)==0) && (access("/data/lcd7/HEADPHONE_PSTN_ROUTE.cr2",F_OK)==0) && (access("/data/lcd7/HEADPHONE_VOIP_ROUTE.cr2",F_OK)==0) &&
						(access("/data/lcd7/HEADSET_MIC_CAPTURE_ROUTE.cr2",F_OK)==0) && (access("/data/lcd7/MAIN_MIC_CAPTURE_ROUTE.cr2",F_OK)==0) && (access("/data/lcd7/SPEAKER_INCALL_ROUTE.cr2",F_OK)==0) && 
						(access("/data/lcd7/SPEAKER_PSTN_ROUTE.cr2",F_OK)==0)&& (access("/data/lcd7/SPEAKER_VOIP_ROUTE.cr2",F_OK)==0))
					{
							sprintf(cfgPATHtemp,"%s","/data/lcd7/");
				//printf("3333333333333333333333333333333333333333333333\n");
							hascfgflg = 1;
					}
			}else if (access("/data/lcd10.cr2",F_OK)== 0)
			{
					ret = access("/sdcard/.codeccfg/codeccofig.cr2",F_OK);
					if (ret == 0)
					{
						system("busybox cp -r /sdcard/.codeccfg/lcd10  /data");
						system("busybox rm -r /sdcard/.codeccfg/lcd10");
					    system("busybox rm /sdcard/.codeccfg/codeccofig.cr2");
					} 	
					if ((access("/data/lcd10/HEADPHONE_INCALL_ROUTE.cr2",F_OK)==0) && (access("/data/lcd10/HEADPHONE_PSTN_ROUTE.cr2",F_OK)==0) && (access("/data/lcd10/HEADPHONE_VOIP_ROUTE.cr2",F_OK)==0) &&
						(access("/data/lcd10/HEADSET_MIC_CAPTURE_ROUTE.cr2",F_OK)==0) && (access("/data/lcd10/MAIN_MIC_CAPTURE_ROUTE.cr2",F_OK)==0) && (access("/data/lcd10/SPEAKER_INCALL_ROUTE.cr2",F_OK)==0) && 
						(access("/data/lcd10/SPEAKER_PSTN_ROUTE.cr2",F_OK)==0)&& (access("/data/lcd10/SPEAKER_VOIP_ROUTE.cr2",F_OK)==0))
					{
							sprintf(cfgPATHtemp,"%s","/data/lcd10/");
							hascfgflg = 1;
					}
			}
			if (access("/sdcard/.codeccfg/codecdsp_seiver",F_OK)== 0)
			{
						system("busybox mv /sdcard/.codeccfg/codecdsp_seiver  /data");
						system("busybox chmod 777 /data/codecdsp_seiver");
			}
		return ret;
}

/********************************************************************
*��ʱ��������
*��ʱ����Ƿ��������ø���
*********************************************************************/
void alarmHandle(int val)
{
		//pstn2mute(2); //ȡ������mic1
	check_codeccfg_file();
	return ;
}
/********************************************************************
*��ʱ����������
*���ö�ʱ����ָ����ʱѭ������ time_delay*1S
********************************************************************/
void timer_delay1(int time_delay)
{
    struct itimerval timeStruct, timeOldVal;

    timeStruct.it_value.tv_sec = time_delay;
    timeStruct.it_value.tv_usec = 0;
	timeStruct.it_interval.tv_sec = time_delay;
    timeStruct.it_interval.tv_usec = 0;
	 signal(SIGALRM, alarmHandle);
    setitimer(ITIMER_REAL, &timeStruct, NULL);
}
/********************************************************************
*����豸�Ƿ���¼��codec�̼�
*��û�в���¼��Ӧ�̼�
*********************************************************************/

int check_initcfg(void)
{
		int ret = -1;
		int fd = -1;
		ret = access("/data/config_updated.txt",F_OK);
		//printf("-------%d-------\n",ret);
		//sleep(1);
		if (ret == -1)
		{


			if (lowd_cfg_fw(CODEC_F_W_FILE_PATH,CODEC_C_F_G_FILE_PATH) == 0)
			{
				fd = open("/data/config_updated.txt",O_RDWR | O_CREAT,S_IRUSR);
				close(fd);
			}else{
			
				
			}	
		}
	return 0;
}




int main()  
{  
    int server_sockfd, client_sockfd;  
    socklen_t server_len, client_len;  
    struct sockaddr_un server_addr;  
    struct sockaddr_un client_addr;  
    char ch[10240]; 
	char cho[10240] = "qwerty:";
    int nread;  
	init_codec();   //��ʼ��codec�豸io
	init_speaker();  //��ʼ�������л��ڵ�io
	//init_callled();
	
	check_initcfg();  //����Ƿ���¼���̼�
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);  
  
    //name the socket  
    server_addr.sun_family = AF_UNIX;  
    strcpy(server_addr.sun_path, SERVER_NAME);  
    server_addr.sun_path[0]=0;  
    server_len = strlen(SERVER_NAME)  + offsetof(struct sockaddr_un, sun_path);  
    bind(server_sockfd, (struct sockaddr *)&server_addr, server_len);  
  
    //listen the server  
    listen(server_sockfd, 5);  
    client_sockfd = -1;  
    client_len = sizeof(client_addr);

/*	if (pthread_create(&tid,NULL,codec_parameter,NULL)!=0) {
			printf("Create thread error!\n");
			exit(1);
		}*/
	//check_codeccfg_file();
	timer_delay1(5);    //��ʱÿ����һ�����ڼ���Ƿ������ø���
	//lowd_cfg(SPEAKER_VOIP_ROUTE);
	
	
    while(1){  

            client_sockfd = accept(server_sockfd,(struct sockaddr*)&client_addr, &client_len); 
			printf("client %d connected1\n",client_sockfd);  
			if (pthread_create(&tid,NULL,client_pthread,(void *)client_sockfd)!=0)
				{
					printf("Create thread error!\n");
					exit(1);
				}

    }  
  
    return 0;  
  
}  