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
*路径目的
*初始化默认配置
*作者：jocker
*date:2016-12-1
*/
#define CODEC_F_W_FILE_PATH "/system/bin/ZLSApp_v1.1.6.s3"
#define CODEC_C_F_G_FILE_PATH "/system/bin/ZLSvoipcfg.cr2"


/*
*路径作用
*这部分宏定义主要用于
*1、系统默认音频配置的路径定义
*作者：jocker
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
*宏功能：
*这部分定于主要用于
*1、新增配置的引用路径不同设备路径有差异
*作者：jocker
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
char cfgPATHtemp[20];   //字符串临时变量，用于接收新配置的路径
char cfgPATH[100];       //新配置路径，用于保存路径  "cfgPATHtemp" + NEW_XXXXXXXXXXXX
int hascfgflg = 0;       //是否有新配置存在，0为不存在；1为存在
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

	pstn2mute(2); //取消静音mic1

	return ;
}

void alarm_mute1(int val)
{
		pstn2mute(2); //取消静音mic1
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
*功能：根据codec_route值切换声音状态
*作者：jocker
*日期：2016-12-1
******************************************************************************************************/
void codec_send_cmd(int codec_route)
{

	//usleep(100000);
	reset_dsp();

	switch (codec_route)
	{
    //喇叭外放模式
	case 0:
				printf("-----------喇叭外放模式-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
				if (is_pstn_calling == 1)
					{
						printf("-----------喇叭外放模式，但是此时正在进行模拟通话。不做处理--------------\n");
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
	//免提通话模式
	case 1:
				printf("-----------免提通话模式-----------codec_route = %d-----------------------\n",codec_route);
							if (old_route == 9 || old_route == 15 )
							{
								//fix me 免提通道切换
							printf("-----------切换到免提通话模式-----------codec_route = %d-----------------------\n",codec_route);
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
							printf("-----------配置免提通话模式参数-----------codec_route = %d-----------------------\n",codec_route);
							}

							incall_model();
								
				break;
	//喇叭响铃模式
	case 2:
				printf("-----------喇叭响铃模式-----------codec_route = %d-----------------------\n",codec_route);
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
	//喇叭网络通话模式
	case 3:
							printf("-----------喇叭网络通话模式-----------codec_route = %d-----------------------\n",codec_route);
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

							printf("-----------配置喇叭网络通话模式参数-----------codec_route = %d-----------------------\n",codec_route);
							
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
								printf("-----------喇叭外放模式，但是此时正在进行模拟通话。不做处理--------------\n");
								break;
							}
							if (old_route == 0 )
							{
								printf("-----------声音通道切换到耳机-----------codec_route = %d-----------------------\n",codec_route);
								//fix me 免提通道切换
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
								//fix me 免提通道切换
								printf("-----------原本在免提通话模式，现在只切通道-----------codec_route = %d-----------------------\n",codec_route);
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
								printf("-----------原本就在耳机模式，现在啥也不做-----------codec_route = %d-----------------------\n",codec_route);
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
							printf("-----------其他模式切到耳机通话模式-----------codec_route = %d-----------------------\n",codec_route);
							
							}
							incall_model();
				break;
	//HEADPHONE_RINGTONE_ROUTE
	case 10:
				printf("-----------HEADPHONE_RINGTONE_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
								//fix me 切换声音通道
				break;
	//SPEAKER_HEADPHONE_NORMAL_ROUTE
	case 11:
				printf("-----------SPEAKER_HEADPHONE_NORMAL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
								//fix me 切换声音通道
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
								//fix me 切换声音通道
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
								//fix me 切换声音通道
							if (old_route == 17 || old_route == 3 )
							{
								//fix me 免提通道切换
								 pstn2mute(3);
								speaker_off();
								 pstn2mute(2);
								printf("-----------其他通话切换到耳机网络通话模式-----------codec_route = %d-----------------------\n",codec_route);
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
								printf("-----------其他模式切换到耳机网络通话模式-----------codec_route = %d-----------------------\n",codec_route);
							}
				break;

	//耳机外放模式
	case 14:
			//	printf("-----------耳机外放模式-----------codec_route = %d-----------------------\n",codec_route);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
				printf("-----------耳机外放模式------old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (is_pstn_calling == 1)
							{
								printf("-----------喇叭外放模式，但是此时正在进行模拟通话。不做处理--------------\n");
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

	//耳机通话模式
	case 15:
				/*printf("-----------耳机通话模式-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 1 )
							{
								//fix me 免提通道切换
								printf("-----------原来就是免提通话模式，只做通道切换-----------codec_route = %d-----------------------\n",codec_route);
							}else if(old_route == 9){
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								
								printf("-----------原来就是耳机通话模式，啥也不做-----------codec_route = %d-----------------------\n",codec_route);
							}else{
							
								printf("-----------其他模式切换到耳机通话模式-----------codec_route = %d-----------------------\n",codec_route);
							}*/
				printf("-----------HEADPHONE_INCALL_ROUTE-----------codec_route = %d-----------------------\n",codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 1 )
							{
								//fix me 免提通道切换
								printf("-----------原本在免提通话模式，现在只切通道-----------codec_route = %d-----------------------\n",codec_route);
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
								printf("-----------原本就在耳机模式，现在啥也不做-----------codec_route = %d-----------------------\n",codec_route);
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
							printf("-----------其他模式切到耳机通话模式-----------codec_route = %d-----------------------\n",codec_route);
							
							}
							incall_model();

				break;
	//耳机铃声模式
	case 16:
				printf("-----------耳机铃声模式-----------codec_route = %d-----------------------\n",codec_route);
								 pstn2mute(3);
								speaker_on();
								 pstn2mute(2);
								//lowd_cfg(SPEAKER_PSTN_ROUTE);
				break;
	
	//耳机网络通话模式
	case 17:	
				/*printf("-----------耳机网络通话模式------old_route = %d -----codec_route = %d-----------------------\n",old_route,codec_route);
							//	lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 13 || old_route == 3 )
							{
								printf("-----------其他通话切换到耳机通话模式-----------codec_route = %d-----------------------\n",codec_route);
								//fix me 免提通道切换
							}else{
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								printf("-----------其他模式切换到耳机通话模式-----------codec_route = %d-----------------------\n",codec_route);
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
	
	//主咪录音模式
	case 21:	
		//		printf("-----------主咪录音模式-----------codec_route = %d-----------------------\n",codec_route);
				printf("主咪录音-----old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
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
	
	//耳机录音模式
	case 22:
			//	printf("-----------耳机录音模式-----------codec_route = %d-----------------------\n",codec_route);
				printf("耳机录音-----old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
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

	//蓝牙录音模式
	case 23:
			//	printf("-----------蓝牙录音模式-----------codec_route = %d-----------------------\n",codec_route);
				break;

	//外放结束
	case 24:
			//	printf("-----------外放结束-----------codec_route = %d-----------------------\n",codec_route);
				break;

	//录音结束
	case 25:
	 	//		printf("-----------录音结束-----------codec_route = %d-----------------------\n",codec_route);

				printf("录音结束-----old_route = %d-----codec_route = %d-----------------------\n",old_route,codec_route);
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
	//通话结束
	case 26:
				printf("-----------通话结束-----------codec_route = %d-----------------------\n",codec_route);
						//	lowd_cfg(SPEAKER_PSTN_ROUTE);
						if (old_route == 1)  //原来是免提通话，先在切回免提音乐模式
							{
								//fix me 免提通道切换
								printf("-----------原来是免提通话，先在切回免提音乐模式---------codec_route = %d-----------------------\n",codec_route);
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
							}else if (old_route == 9){   //原来是耳机通话，先在切回耳机音乐模式
								//lowd_cfg(SPEAKER_VOIP_ROUTE);
								printf("-----------原来是耳机通话，先在切回耳机音乐模式---------codec_route = %d-----------------------\n",codec_route);
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

	//网络通话结束
	case 27:
				printf("-----------网络通话结束-----------codec_route = %d-----------------------\n",codec_route);
							//lowd_cfg(SPEAKER_PSTN_ROUTE);
							if (old_route == 26)
							{

								
							}else{
							
								if (old_route == 3)  //原来是免提网络通话，先在切回免提音乐模式
								{
									//fix me 免提通道切换
									printf("-----------原来是免提网络通话，先在切回免提音乐模式---------codec_route = %d-----------------------\n",codec_route);
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
								}else if (old_route == 13 || old_route == 17 ){   //原来是耳机网络通话，先在切回耳机音乐模式
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
									printf("-----------原来是耳机网络通话，先在切回耳机音乐模式--------codec_route = %d-----------------------\n",codec_route);
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
							printf("-----------当前状态模拟电话免提通话----------\n");
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
							printf("-----------当前状态模拟电话手柄通话----------\n");
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
								//设置声音
								printf("-----------当前状态模拟电话通话，设置声音大小 0----------\n");
								set_speaker(0);
							}else{
								//保存当前声音大小待下次通话时使用
								pstn_val = 0;
								printf("-----------当前状态模拟电话未通话，设置声音大小 0----------\n");
							}
							
		break;
		case 34:						//PSTN VAL 1
							printf("-----------PSTN VAL 1--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//设置声音
								printf("-----------当前状态模拟电话通话，设置声音大小 1----------\n");
								set_speaker(1);
							}else{
								//保存当前声音大小待下次通话时使用
								pstn_val = 1;
								printf("-----------当前状态模拟电话未通话，设置声音大小 1----------\n");
							}
							
		break;
		case 35:						//PSTN VAL 2
							printf("-----------PSTN VAL 2--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//设置声音
								printf("-----------当前状态模拟电话通话，设置声音大小 2----------\n");
								set_speaker(2);
							}else{
								//保存当前声音大小待下次通话时使用
								pstn_val = 2;
								printf("-----------当前状态模拟电话未通话，设置声音大小 2----------\n");
							}
		break;
		case 36:						//PSTN VAL 3
							printf("-----------PSTN VAL 3--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//设置声音
								printf("-----------当前状态模拟电话通话，设置声音大小 3----------\n");
								set_speaker(3);
							}else{
								//保存当前声音大小待下次通话时使用
								pstn_val = 3;
								printf("-----------当前状态模拟电话未通话，设置声音大小 3----------\n");
							}
		break;
		case 37:						//PSTN MUTE
							printf("-----------PSTN MUTE--------codec_route = %d-----------------------\n",codec_route);
							is_pstn_mute = 1;
							if (is_pstn_calling == 1)
							{
								//设置声音
								if (is_pstn_hold == 1)
								{
									pstn2mute(3);
									printf("-----------当前状态模拟电话通话，MUTE ，但是之前已处于HOLD状态----------\n");
								}else{
									pstn2mute(1);
									printf("-----------当前状态模拟电话通话，MUTE 状态----------\n");
								}

								
							}else{
								//pstn未通话，不做处理
								
							}
		break;
		case 38:						//PSTN NO MUTE
							printf("-----------PSTN NO MUTE--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//设置声音
								is_pstn_mute = 0;
								if (is_pstn_hold == 1)
								{
									pstn2mute(3);
									printf("-----------当前状态模拟电话通话， NO MUTE ，但是之前已处于HOLD状态----------\n");
								}else{
								
									pstn2mute(2);
									printf("-----------当前状态模拟电话通话，NO MUTE----------\n");
								}
								
							}else{
								//pstn未通话，不做处理
								
							}
		break;
		case 39:						//PSTN HOLD
							printf("-----------/PSTN HOLD--------codec_route = %d-----------------------\n",codec_route);
							is_pstn_hold = 1;
							if (is_pstn_calling == 1)
							{
								//设置声音
								pstn2mute(3);
								printf("-----------当前状态模拟电话通话，HOLD状态----------\n");
							}else{
								//pstn未通话，不做处理
								
							}
		break;
		case 40:						//PSTN NO HOLD
							printf("-----------/PSTN NO HOLD--------codec_route = %d-----------------------\n",codec_route);
							if (is_pstn_calling == 1)
							{
								//设置声音
								is_pstn_hold = 0;
								if (is_pstn_mute == 1)
								{
									pstn2mute(1);
									printf("-----------当前状态模拟电话通话， NO HOLD ，但是之前已处于MUTE状态----------\n");
								}else{
									pstn2mute(2);
									printf("-----------当前状态模拟电话通话，NO HOLD状态----------\n");
								}
								
							}else{
								//pstn未通话，不做处理
								
							}
		break;
		case 41:						//PSTN OFF
							printf("-----------/PSTN OFF--------codec_route = %d-----------------------\n",codec_route);
								is_pstn_calling = 0;
								pstn2mute(3);
								/*
								*fix me   查询耳机状态。。。。。。。。。。。。
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
				printf("-----------其他模式-----------codec_route = %d-----------------------\n",codec_route);
			
	}

}

/*************************************************************************************************************
*socket子线程void *client_pthread(void *new_fd)	
*
*功能：与连接上的客户端通讯
*接收客户端发送的数据并将其解析封装然后控制声音route状态
*作者：jocker
*日期：2016-12-01
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
*新配置测试及加载
*功能：
*1、测试当前机器类型
*2、根据机器类型去检测是否有可供更新的配置
*3、获取最新配置
*4、将最新配置路径提供给相关函数
*5、检测当前声音服务是否有更新，如果有就将其更新
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
*定时器服务函数
*定时监测是否有新配置更新
*********************************************************************/
void alarmHandle(int val)
{
		//pstn2mute(2); //取消静音mic1
	check_codeccfg_file();
	return ;
}
/********************************************************************
*定时器启动函数
*配置定时器并指定定时循环周期 time_delay*1S
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
*检测设备是否烧录过codec固件
*若没有测烧录对应固件
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
	init_codec();   //初始化codec设备io
	init_speaker();  //初始化声音切换节点io
	//init_callled();
	
	check_initcfg();  //检测是否烧录过固件
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
	timer_delay1(5);    //定时每五秒一个周期检测是否有配置更新
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