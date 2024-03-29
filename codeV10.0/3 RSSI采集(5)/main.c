
#include <iocc2530.h>
#include "hal_mcu.h"
#include "hal_assert.h"
#include "hal_board.h"

#include "hal_rf.h"
#include "basic_rf.h"
#include <stdio.h>
#include <string.h>

#define RF_CHANNEL            25      // 2.4 GHz RF channel


#define PAN_ID                0x2007
#define SEND_ADDR             0x2530
#define RECV_ADDR             0x2520

#define NODE_TYPE             2         //0:接收节点，！0：发送节点

static basicRfCfg_t basicRfConfig;

#pragma optimize=none      
void DelaymS (unsigned int dly)                   
{
	unsigned int i;
	for ( ; dly>0; dly--)
	{
		for (i=0; i<1032; i++);        
	}
}



//int uWaveDistance(void);
void dht11_update(char* buf);
void rfSendData(void)
{
    uint8 pTxData[20];
    uint8 ret;
    // Keep Receiver off when not needed to save power
    basicRfReceiveOff();
    //sprintf(pTxData,"distince:%u CM.", uWaveDistance());
    printf("goto while...\r\n");
    // Main loop
    while (TRUE) {
       printf("in while...\r\n");
       dht11_update(pTxData);
       printf("get data ok...\r\n");
       ret = basicRfSendPacket(RECV_ADDR, pTxData, sizeof pTxData);       
       if (ret == SUCCESS) {
          printf("send msg ok!!!\r\n");
          hal_led_on(1);
          halMcuWaitMs(100);
          hal_led_off(1);
          halMcuWaitMs(900); 
       } else {
          printf("send msg error!!!\r\n");
          hal_led_on(1);
          halMcuWaitMs(1000);
          hal_led_off(1);
       }
       
    }
}

void rfRecvData(void)
{
  uint8 pRxData[128];
  int rlen;
  
   basicRfReceiveOn();

    // Main loop
    while (TRUE) {
        while(!basicRfPacketIsReady());
        rlen = basicRfReceive(pRxData, sizeof pRxData, NULL);
        if(rlen > 0) {
          pRxData[rlen] = 0;
            
          printf("%s rssi: %d\r\n", (char *)pRxData, basicRfGetRssi());
            
        }
    }
}

//void uWaveInit(void);
void dht11_io_init(void);
void main(void)
{
    halMcuInit();

    hal_led_init();
    
    hal_uart_init();
    
    //Uart0Init(0, 0); 
    printf("s rssi: d\r\n");
    //Uart0Init(unsigned char StopBits,unsigned char Parity)
    if (FAILED == halRfInit()) {
        HAL_ASSERT(FALSE);
    }

    // Config basicRF
    basicRfConfig.panId = PAN_ID;
    basicRfConfig.channel = RF_CHANNEL;
    basicRfConfig.ackRequest = TRUE;
#ifdef SECURITY_CCM
    basicRfConfig.securityKey = key;
#endif

    
    // Initialize BasicRF
#if NODE_TYPE
    basicRfConfig.myAddr = SEND_ADDR;
#else
    basicRfConfig.myAddr = RECV_ADDR; 
#endif
    
    if(basicRfInit(&basicRfConfig)==FAILED) {
      HAL_ASSERT(FALSE);
    }
    
#if NODE_TYPE
    //uWaveInit();
    dht11_io_init();
    rfSendData();
#else
    rfRecvData();   
#endif
}
