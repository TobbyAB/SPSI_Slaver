/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-22     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include "drv_spi.h"
#include <string.h>
#include "AX5043.h"
#include "Radio_Decoder.h"
#include "Radio_Encoder.h"
#include "Flashwork.h"
#include "pin_config.h"
#include "Radio_Drv.h"

#define DBG_TAG "RF_DE"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

uint8_t device_select = 0;
uint32_t counter_433 = 0;
uint32_t counter_4068 = 0;
void counter_send(uint8_t rf)
{
    char *buf = rt_malloc(64);
    if(rf)
    {
        sprintf(buf,"A{rf is %d,counter is %ld}",rf,counter_433);
        Normal_send(&rf_433,buf,strlen(buf));
    }
    else
    {
        sprintf(buf,"A{rf is %d,counter is %ld}",rf,counter_4068);
        Normal_send(&rf_4068,buf,strlen(buf));
    }
    rt_free(buf);
}
void rf4068_solve(uint8_t *rx_buffer,uint8_t rx_len)
{
    sscanf((const char *)&rx_buffer[2],"{rf is %d,counter is %ld}",&device_select,&counter_4068);
    File_Output(device_select,counter_4068);
    counter_send(0);
}
void rf433_solve(uint8_t *rx_buffer,uint8_t rx_len)
{
    sscanf((const char *)&rx_buffer[2],"{rf is %d,counter is %ld}",&device_select,&counter_433);
    File_Output(device_select,counter_433);
    counter_send(1);
}
void rf433_rx_callback(uint8_t *rx_buffer,uint8_t rx_len)
{
    //LOG_I("RX 433 is %s\r\n",rx_buffer);
    switch(rx_buffer[1])
    {
    case 'A':
        rf433_solve(rx_buffer,rx_len);
        break;
    }
}
void rf4068_rx_callback(uint8_t *rx_buffer,uint8_t rx_len)
{
    //LOG_I("RX 4068 is %s\r\n",rx_buffer);
    switch(rx_buffer[1])
    {
    case 'A':
        rf4068_solve(rx_buffer,rx_len);
        break;
    }
}
