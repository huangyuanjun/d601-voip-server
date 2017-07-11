#include <stdio.h>
#include <stdlib.h>  /* malloc, free, rand */

#include "vproc_common.h"
#include "zl380twolf_config.h"
#include "vprocTwolf_access.h"
#include "zl380twolf_firmware.h"

#if 1
#include "vproc_common.c"
#include "vprocTwolf_access.c"
#include "zl380twolf_config.c"
#include "zl380twolf_firmware.c"
#endif
#include "codec_set.h"


#define MAX_WORDS_FOR_MULTIWORD_ACCESS_TEST  125
#include <android/log.h>
#define  LOG_TAG    "myjni_led"
//#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

VprocStatusType LoadFwrConfig_Alt(uint8 mode);


pthread_mutex_t WRITE_INFO = PTHREAD_MUTEX_INITIALIZER; 
unsigned char WRITE_INFOINFOOPEN(void)
{
			pthread_mutex_lock(&WRITE_INFO);

	return 0;
} /* VpSysEnterCritical() */
unsigned char WRITE_INFOINFOCLOSE(void)
{		
			pthread_mutex_unlock(&WRITE_INFO);
    return 0;
} /* VpSysExitCritical() */


#define F_W_FILE_PATH "/system/bin/ZLS38040App_v1.1.6.s3"
#define C_F_G_FILE_PATH "/system/bin/ZLS38040voip.cr2"
#define	SAVE_IMAGE_TO_FLASH
#define SAVE_CFG_TO_FLASH
/*quick test*/

uint16 numElements;

dataArr *pCr2Buf;
/* fseekNunlines() -- The firmware file is an ascii text file.
 * the information from fseek will not be usefull
 * this is our own fseek equivalent
 */
static unsigned long fseekNunlines(FILE *BOOT_FD) {
    uint32 line_count = 0;
    int c;

    while ( (c=fgetc(BOOT_FD)) != EOF ) {
        if ( c == '\n' )
            line_count++;
    }
    return line_count;
}

/* readCfgFile() use this function to
 * Read the Voice processing cr2 config file into RAM
 * filepath -- pointer to the location where to find the file
 * pCr2Buf -- the actual firmware data array will be pointed to this buffer
 */
static int readCfgFile(char *filepath) {
	unsigned int reg[2], val[2], len;
	uint8 done = 0;
	uint16 index = 0;
	FILE *BOOT_FD;
	char *s;
	char line[512] = "";


	BOOT_FD = fopen(filepath, "rb");
	if (BOOT_FD != NULL) {
		len = fseekNunlines(BOOT_FD);
		if (len <= 0) {
		   	printf("Error: file is not of the correct format...\n");
		   	return -1;
		}
		//printf("fileLength = %u\n", len);
        /*start at the beginning of the file*/
        //fseek(BOOT_FD, 0, SEEK_SET);

		/* allocate memory to contain the reg and val:*/
		pCr2Buf = (dataArr *) malloc(len*sizeof(dataArr));
		if (pCr2Buf == NULL) {
            printf ("not enough memory to allocate %u bytes.. ", len*sizeof(dataArr));
            return -1;
		}

	    rewind(BOOT_FD);
        /*read and format the data accordingly*/
        numElements  = 0;
        do {
		   	 s = fgets(line, 512, BOOT_FD);
		   	 if (line[0] == ';') {
			 	 continue;
			 } else
			 if (s != NULL) {
			 	 numElements++;
			 	 sscanf(line, "%x %c %x", reg, s, val);
			  	 pCr2Buf[index].reg = reg[0];
			  	 pCr2Buf[index].value = val[0];
			    // printf("pCr2Buf[%d].reg pCr2Buf[%d].value  = 0x%04x\t0x%04x\n", index, index, pCr2Buf[index].reg, pCr2Buf[index].value);
			  	 index++;
			  } else { done = 1;}

        } while (done == 0);

        fclose(BOOT_FD);
		//printf ("size of pCr2Buf = %u bytes.. \n", len*sizeof(pCr2Buf));
	} else {
	  	printf("Error: can't open file\n");
	}
	return 0;
}

int reset_dsp(void)
{
	 int status = -1;
	 status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
//	 printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~��¼���\n");
	 return status;
}




int codec_write_reg(int addr,int numwords,int value)
{    
	     int val[1];
		 int status = -1;
		 val[0] = value;
		WRITE_INFOINFOOPEN();
			status  = VprocTwolfHbiWrite(addr, numwords, val);
		//	LOGD("***************************%d**********%x*",status,value);
		    if (status != VPROC_STATUS_SUCCESS)
				{
				   printf("Error %d:VprocTwolfHbiWrite()\n", status);
			       VprocTwolfHbiCleanup();
				   WRITE_INFOINFOCLOSE();
				   return -1;
			    }
		WRITE_INFOINFOCLOSE();
		return 0;
}

int pstn2mute(int cmd)
{
	int status = -1;
	switch (cmd)
	{
	case 1:
			status = codec_write_reg(0x02b0,1,0x0002);//��ͨ������mute����
			status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
			break;
	case 2:
			status = codec_write_reg(0x02b0,1,0x0007);//��ͨ������no mute����
			status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
			break;
	case 3:
			status = codec_write_reg(0x02b0,1,0x0000);//��ͨ������hlod����
			status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	default:
			status = codec_write_reg(0x02b0,1,0x0007);//��ͨ������no mute����
			status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	}

return status;
	

}




int call_vol(int vol)
{

	int status = -1;
	vol = vol/2;
	status = codec_write_reg(INCALL_VAL[vol][0],1,INCALL_VAL[vol][1]);
	status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	
	return status;
}





int set_speaker(int cmd)
{
	int status = -1;
	switch(cmd)
	{
		case 1:		
					printf("-------------------------1-------------------------\n");
					status = codec_write_reg(SPEAKER_REG,1,SPEAKER_VAL1);
					status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
					printf("-------------------------%x-------%x------%x------------\n",status,SPEAKER_REG,SPEAKER_VAL1);
		
					break;
		case 2: 
					printf("-------------------------2-------------------------\n");
					status = codec_write_reg(SPEAKER_REG,1,SPEAKER_VAL2);
					status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
					printf("-------------------------%x-------------------------\n",status);
					
					break;
		case 3: 
					printf("-------------------------3-------------------------\n");
					status = codec_write_reg(SPEAKER_REG,1,SPEAKER_VAL3);
					status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
					printf("-------------------------%x-------------------------\n",status);

			
					break;
		case 0: 
					printf("-------------------------0-------------------------\n");
					status = codec_write_reg(SPEAKER_REG,1,SPEAKER_VAL0);
					status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
				
					break;
		case 4: 
					printf("-------------------------0-------------------------\n");
					status = codec_write_reg(SPEAKER_REG,1,HAND_VAL3);
					status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
				
					break;
		default:
					status = codec_write_reg(SPEAKER_REG,1,SPEAKER_VAL3);
					status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	}
		return status;
}


int skype_model(void)
{
	int status = -1;
	int i;
	for(i = 0;i<70;i++)
	{
		//status = codec_write_reg(reg_skype[i],1,val_skype[i]);
		
		status = codec_write_reg(reg_new_skype[i],1,val_new_skype[i]);
	//	LOGD("******************%x**********%x***********",reg_new_skype[i],val_new_skype[i]);
	}
	status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	return status;
}

int pstn_model(void)
{
	int status = -1;
	int i;
	for(i = 0;i<70;i++)
	{
	//	status = codec_write_reg(reg_pstn[i],1,val_pstn[i]);
		status = codec_write_reg(reg_pstn_handfree[i],1,val_pstn_handfree[i]);
		//LOGD("******************%x**********%x***********",reg_pstn_handfree[i],val_pstn_handfree[i]);
		
	
	}
	status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	return status;
}


int pstn_headset(void)
{
	int status = -1;
	int i;
	for(i = 0;i<70;i++)
	{
	//	status = codec_write_reg(reg_pstn[i],1,val_pstn[i]);
//	LOGD("******************%x**********%x***********",reg_pstn_handfree[i],val_pstn_handfree[i]);
		status = codec_write_reg(reg_pstn_headset[i],1,val_pstn_headset[i]);
	//	LOGD("******************%x**********%x***********",reg_pstn_handfree[i],val_pstn_handfree[i]);
	
	}
	status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	return status;


}


int init_codec(void)
{
	 int status = -1;
   status = VprocTwolfHbiInit();
	
    if (status < 0)
    {
        perror("tw_spi_access open");
	
		VprocTwolfHbiCleanup();
		if(VprocTwolfHbiInit())
		return 0;
		else
        return -1;
		//LOGD("******************init failed**********%d***********\n",-1);
    }

		//LOGD("******************init succeed**********%d***********\n",-1);
	return 0;

}

int lowd_cfg_fw(char * codec_fw_path,char * codec_cfg_path)
{
	VprocStatusType status = VPROC_STATUS_SUCCESS;
    FILE *BOOT_FD;
    char line[256] = "";
	BOOT_FD = fopen(codec_fw_path, "rb");
	if (BOOT_FD == NULL) {
	    printf("Error: can't open file %s\n", codec_fw_path);
		sleep(1);
		//LOGD("******************fopenF_W_FILE_PATH**********%d***********\n",-1);
		return -1;
	}
	//Opening firmware file - done....

	status  = VprocTwolfHbiBootPrepare();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfHbiBootPrepare()\n", status);
        fclose(BOOT_FD);
        VprocTwolfHbiCleanup();
		//LOGD("******************VprocTwolfHbiBootPrepare**********%d***********\n",-1);
       return -1;
    }
	//Boot prepare - done....
    
    while (fgets(line, 256, BOOT_FD) != NULL) {
        status = VprocTwolfHbiBootMoreData(line);
        if (status == VPROC_STATUS_BOOT_LOADING_MORE_DATA) {
            continue;
        } else if (status == VPROC_STATUS_BOOT_LOADING_CMP) {
        
            break ;              
        } else if (status != VPROC_STATUS_SUCCESS) {        
            printf("Error %d:VprocTwolfHbiBootMoreData()\n", status);
            fclose(BOOT_FD);
            VprocTwolfHbiCleanup();
			//LOGD("******************VprocTwolfHbiBootMoreData**********%d***********\n",-1);
           return -1;
		 
        }
	}
	//Firmware data transfer - done....	
    fclose(BOOT_FD);

    status  = VprocTwolfHbiBootConclude();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfHbiBootConclude()\n", status);
        VprocTwolfHbiCleanup();
		//LOGD("******************VprocTwolfHbiBootConclude**********%d***********\n",-1);
       return -1;
    }

#ifdef SAVE_IMAGE_TO_FLASH
    printf("-- Saving firmware to flash....\n");
    status = VprocTwolfSaveImgToFlash();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfSaveImgToFlash()\n", status);
		//LOGD("******************VprocTwolfSaveImgToFlash**********%d***********\n",-1);
        VprocTwolfHbiCleanup();
       return -1;
    }
    printf("-- Saving firmware to flash....done\n");

#endif

	status  = VprocTwolfFirmwareStart();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfFirmwareStart()\n", status);
        VprocTwolfHbiCleanup();
		//LOGD("******************VprocTwolfFirmwareStart**********%d***********\n",-1);
		return -1;
    }

	//Loading the config file into the device RAM
    if (readCfgFile(codec_cfg_path) < 0){
	    printf("Error:read %s file\n", codec_cfg_path);
    }
	//Reading config file to host RAM - done....
	status  = VprocTwolfLoadConfig(pCr2Buf, numElements);
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfLoadConfig()\n", status);
        VprocTwolfHbiCleanup();
       return -1;
    }

#ifdef SAVE_CFG_TO_FLASH
   printf("-- Saving config to flash....\n");
   status = VprocTwolfSaveCfgToFlash();
   if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfSaveCfgToFlash()\n", status);
        VprocTwolfHbiCleanup();
		
        return -1;
   }
    printf("-- Saving config to flash....done\n");

#endif
   

  status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	return 0;
}

int lowd_cfg(char *CFG_FILE_PATH)
{
	VprocStatusType status = VPROC_STATUS_SUCCESS;
    FILE *BOOT_FD;
    char line[256] = "";
	
   //  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~������¼\n");
	//  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~��¼·��Ϊ��%s~~~~~~~~~~~~~~~~~~~\n",CFG_FILE_PATH);
	status  = VprocTwolfFirmwareStart();
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfFirmwareStart()\n", status);
        VprocTwolfHbiCleanup();
		//LOGD("******************VprocTwolfFirmwareStart**********%d***********\n",-1);
		return -1;
    }

	//Loading the config file into the device RAM
    if (readCfgFile(CFG_FILE_PATH) < 0){
	    printf("Error:read %s file\n", C_F_G_FILE_PATH);
    }
	//Reading config file to host RAM - done....
	status  = VprocTwolfLoadConfig(pCr2Buf, numElements);
    if (status != VPROC_STATUS_SUCCESS) {
        printf("Error %d:VprocTwolfLoadConfig()\n", status);
        VprocTwolfHbiCleanup();
       return -1;
    }
//  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~��¼���\n");
  status = codec_write_reg(0x0006,1,0x0002);//ò�����в�������Ҫ�ⲽ
	return 0;
}


/*
int main (int argc, char** argv) {
  
	uint16 ctlnum = 0;


	init_codec();

	while(1)
	{
		printf("�������״̬��������������С��");
		scanf("%d",&ctlnum);
		if(ctlnum == 6){
				skype_model();
		}else if(ctlnum == 5)
		{
				pstn_model();
		}else{ 
			set_speaker(ctlnum);
		}
	}
    printf("Test 3 - completed - PASS\n\n");       
    printf("\n");
  //  VprocTwolfHbiCleanup();
    

    return 0;
}
*/
/*LoadFwrConfig_Alt - to load a converted *s3, *cr2 to c code into the device.
* Basically instead of loading the *.s3, *cr2 directly,
* use the tw_convert tool to convert the ascii hex fwr mage into code and compile 
* with the application
*
* input arg: mode:  0 - load both firmware and confing
*                   1 - load firmware only
*                   2 - load config only  
*/
VprocStatusType LoadFwrConfig_Alt(uint8 mode) {
		VprocStatusType status= VPROC_STATUS_SUCCESS;
        if ((mode == 0) || (mode == 1)) {
    		twFirmware st_Firmware;
    		st_Firmware.st_Fwr = (twFwr *)st_twFirmware;
    		st_Firmware.twFirmwareStreamLen = (uint16)firmwareStreamLen;
    		st_Firmware.execAddr  = (uint32)executionAddress;
    		st_Firmware.havePrgmBase = (uint8)haveProgramBaseAddress;
    		st_Firmware.prgmBase = (uint32)programBaseAddress;
                 
    		printf("\t1- Firmware boot loading started ....\n");
    
    		status  = VprocTwolfHbiBoot_alt(&st_Firmware);
    		if (status != VPROC_STATUS_SUCCESS) {
    			printf("Error %d:VprocTwolfHbiBoot()\n", status);
    			      
    			return -1;
    		}
    
    		printf("\t2- Loading the image to RAM....done\n");
#ifdef SAVE_IMAGE_TO_FLASH
    		printf("\t-- Saving firmware to flash....\n");
    		status = VprocTwolfSaveImgToFlash();
    		if (status != VPROC_STATUS_SUCCESS) {
    			printf("Error %d:VprocTwolfSaveImgToFlash()\n", status);
    			      
    			return status;
    		}
    		printf("\t-- Saving firmware to flash....done\n");
    
#endif
    
    		status  = VprocTwolfFirmwareStart();
    		if (status != VPROC_STATUS_SUCCESS) {
    			printf("Error %d:VprocTwolfFirmwareStart()\n", status);
    			      
    			return status;
    		}
        }
        if ((mode == 0) || (mode == 2)) {
    		printf("\t3- Loading the config file into the device RAM....\n");
    	
    		status  = VprocTwolfLoadConfig((dataArr *)st_twConfig, 
                                                    (uint16)configStreamLen);
    		if (status != VPROC_STATUS_SUCCESS) {
    			printf("Error %d:VprocTwolfLoadConfig()\n", status);
    			      
    			return status;
    		}
#ifdef SAVE_CFG_TO_FLASH
    	   printf("\t-- Saving config to flash....\n");
    	   status = VprocTwolfSaveCfgToFlash();
    	   if (status != VPROC_STATUS_SUCCESS) {
    			printf("Error %d:VprocTwolfSaveCfgToFlash()\n", status);
    			      
    			return status;
    	   }
    		printf("\t-- Saving config to flash....done\n");
    
#endif
        }
        {  /*Verify that the boot loading PASS or Fail*/
            uint16 val =0;

        	status  = VprocTwolfHbiRead(0x0022, 1, &val);
            if (status != VPROC_STATUS_SUCCESS) {
                printf("Error %d:VprocTwolfHbiRead()\n", status);
                VprocTwolfHbiCleanup();
                return -1;
            }
            if ((val == 38040) || 
                (val == 38050) || 
                (val == 38060) || 
                (val == 38080) ||
                (val == 38051) || 
                (val == 38041)) 
                 printf("\tDevice boot loading completed successfully...\n");
            else {
                 printf("\tDevice boot loading failed!!!...\n");
                 return VPROC_STATUS_FAILURE;
        	}
        }         
        
		/*Firmware reset - in order for the configuration to take effect
		 * NOTE:  The ZL38040 needs a soft reset for the uploaded configuration
		 *       to take effect. This soft-reset is sent below
		 *       if the ZL38040 is an I2S slave, if the I2S master is not stable
		 *       at the time of this reset, then that reset will not take effect. 
		 *       In that case the host has to to simply resend the reset 
		 *       command once the I2S master
		 *       is up and running and is at a stable state.
		 */
		status  = VprocTwolfReset(VPROC_RST_SOFTWARE);
		if (status != VPROC_STATUS_SUCCESS) {
			printf("Error %d:VprocTwolfReset()\n", status);
			      
			return status;
		}

		printf("\tDevice boot loading completed successfully...\n"); 
        return status;   
}

