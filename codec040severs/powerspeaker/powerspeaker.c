#include <stdio.h>
#include <stdlib.h>  /* malloc, free, rand */
#include <sys/ioctl.h>
#include <linux/types.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
int speaker_fd =-1;
int headset_fd =-1;
int callled_fd =-1;
#include "powerspeaker.h"
#include <android/log.h>
//#define  LOG_TAG    "myjni_led"
//#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
int init_speaker(void)
{
	int status = 0;
    char *speaker_name = "/sys/class/leds/genesis-led/enable";

    speaker_fd = -1;
    speaker_fd = open(speaker_name, O_RDWR);
    if (speaker_fd == -1)
    {
        perror("unble to open /sys/class/leds/genesis-led/enable "
        " make sure the driver is installed in the kernel");
//LOGE("unble to open /sys/class/leds/genesis-led/enable "" make sure the driver is installed in the kernel\n");
        return -1;
    }
	//LOGE("open /sys/class/leds/genesis-led/enable succesed\n");
    return status;
}

int init_callled(void)
{
	int status = 0;
    char *callstatus_name = "/sys/class/leds/genesis-led/callstatus";

    callled_fd = -1;
    callled_fd = open(callstatus_name, O_RDWR);
    if (callled_fd == -1)
    {
        perror("unble to open /sys/class/leds/genesis-led/callstatus "
        " make sure the driver is installed in the kernel");
		//LOGE("unble to open /sys/class/leds/genesis-led/callstatus "" make sure the driver is installed in the kernel\n");
	printf("unble to open /sys/class/leds/genesis-led/callstatus\n");
        return -1;
    }
	//LOGE("open /sys/class/leds/genesis-led/callstatus succesed\n");
    return status;
}


int init_headset(void)
{
	int status = 0;
    char *headset_name = "/sys/class/leds/genesis-led/status";
	
    headset_fd = -1;
    headset_fd = open(headset_name, O_RDWR);
    if (headset_fd == -1)
    {
        perror("unble to open /sys/class/leds/genesis-led/status "
        " make sure the driver is installed in the kernel");
	//	LOGE("unble to open /sys/class/leds/genesis-led/status "" make sure the driver is installed in the kernel\n");
        return -1;
    }
	//LOGE("open /sys/class/leds/genesis-led/status succesed\n");
    return status;
}



void speakerclose(void)
{
    /*global file handle*/
    if (speaker_fd != -1) 
       close(speaker_fd);
    speaker_fd = -1;
}
void callledclose(void)
{
    /*global file handle*/
    if (callled_fd != -1) 
       close(callled_fd);
    callled_fd = -1;
}

void headsetclose(void)
{
    /*global file handle*/
    if (headset_fd != -1) 
       close(headset_fd);
    headset_fd = -1;
}


int speakerwrite(char *pData,short numBytes)
{
    int status = 0;
    if (speaker_fd < 0) {
        return -1;         
    } 
    status = write(speaker_fd, pData, numBytes);
    if (status < 0) {
        perror("microsemi_spis_tw_write driver");
	//	LOGE("*************WRIRE speaker_fd FAILED !!! pData[0] = %d*****return = %d **********",pData[0],status);
			if (speaker_fd < 0)
			{
				close(speaker_fd);
				init_speaker();
			    speakerwrite(pData, numBytes);
			}
    }
    return status;
}

int callledwrite(char *pData,short numBytes)
{
    int status = 0;
    if (callled_fd < 0) {
        return -1;         
    } 
    status = write(callled_fd, pData, numBytes);
    if (status < 0) {
        perror("microsemi_spis_tw_write driver");
		printf("WRIRE callled_fd FAILED\n");
	//	LOGE("*************WRIRE callled_fd FAILED !!! pData[0] = %d*****return = %d **********",pData[0],status);
			if (callled_fd < 0)
			{
				close(callled_fd);
				init_callled();
			    callledwrite(pData, numBytes);
			}
    }
    return status;
}


int headsetread(char *pData,short numBytes)
{
    int status = 0;
    if (headset_fd < 0) {
        return -1;         
    } 
	memset(pData, 0, sizeof(pData));
	lseek(headset_fd, 0, SEEK_SET);
    status = read(headset_fd, pData, numBytes);
    if (status < 0) {
        perror("microsemi_spis_tw_write driver");
	//	LOGE("*************READ headset_fd FAILED !!! pData[0] = %d*****return = %d **********",pData[0],status);
		if (headset_fd < 0)
			{
				close(headset_fd);
				init_headset();
				headsetread(pData, numBytes);
			}
    }
	//LOGD("******************%d*********************",pData[0]);
	//printf("******************%d*********************",pData[0]);
    return status;
}

int incall_model(void)
{
	char data[10];
	int status = -1;
	data[0] = '1';
    status = callledwrite(data,1);
	return status;
}
int endcall_model(void)
{
	char data[10];
	int status = -1;
	data[0] = '2';
    status = callledwrite(data,1);
	return status;
}

int inpstn_model(void)
{
	char data[10];
	int status = -1;
	data[0] = '3';
    status = callledwrite(data,1);
	return status;
}
int endpstn_model(void)
{
	char data[10];
	int status = -1;
	data[0] = '4';
    status = callledwrite(data,1);
	return status;
}
int istalking_on(void)
{
	char data[10];
	int status = -1;
	data[0] = '5';
    status = callledwrite(data,1);
	return status;
}



int speaker_on(void)
{
	char data[10];
	int status = -1;
	data[0] = '1';
	usleep(50000);
    status = speakerwrite(data,1);
	printf("22222222222222222222222  on   2222222222222222222222222222\n");
	usleep(50000);
	return status;
}
int speaker_off(void)
{
	char data[10];
	int status = -1;
	data[0] = '0';
	usleep(50000);
    status = speakerwrite(data,1);
	usleep(50000);
		printf("22222222222222222222222  off   2222222222222222222222222222\n");
	return status;
}
/*
int main (int argc, char** argv) {
  
	int ctlnum = 0;
	init_headset();
	char data[10];
	while(1)
	{
		//printf("�������״̬��������������С��");
		//scanf("%d",&ctlnum);
		//sprintf(data,"%d",ctlnum);
		
		memset(data, 0, sizeof(data));
	//	printf("+++++++++++++++++%d++++++++++++++++++++++",data[0]);
		headsetread(data,1);
		//	set_speaker(ctlnum);
		
	//	printf("---------------%d--------------",data[0]);
		usleep(50000);
	}
    printf("Test 3 - completed - PASS\n\n");       
    printf("\n");
    VprocTwolfHbiCleanup();
    

    return 0;
}*/

