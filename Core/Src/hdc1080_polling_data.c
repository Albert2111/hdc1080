#include <string.h>
#include <stdio.h>
#include "hdc1080.h"

#define STM32L4XX

#if defined(STM32L4XX)
#include "stm32l4xx_hal.h"
#include "i2c.h"
#endif

#define SENSOR_BUS hi2c2
#define BOOT_TIME 15

static int32_t platform_read_reg(void* handle, uint8_t reg, uint8_t* pbuf, uint16_t len);
static int32_t platform_write_reg(void* handle, uint8_t reg, uint8_t* pbuf, uint16_t len);
static int32_t platform_read(void* handle, uint16_t dev_addr, uint8_t* pbuf, uint16_t len);
static int32_t platform_write(void* handle, uint16_t dev_addr, uint8_t* pbuf, uint16_t len);
void platform_delay(uint32_t ms);
static void platform_init(void);

void example_main_hdc1080()
{
    dev_ctx_t stm32dev;
    uint16_t mid;
    uint16_t did;
    hdc1080_voltage_t voltage;
    hdc1080_th_res_t th_res;
    hdc1080_data_t th_data = {0};

    stm32dev.handle = &SENSOR_BUS;
    stm32dev.read_reg = platform_read_reg;
    stm32dev.write_reg = platform_write_reg;
    stm32dev.read = platform_read;
    stm32dev.write = platform_write;

    platform_init();

    hdc1080_mid_get(&stm32dev, &mid);
    if(mid != HDC1080_MID)
    {
        printf("mid fail\r\n");
        while(1);
    }
    else
    {
        printf("MID = 0x%X\r\n", mid);
    }
    
    hdc1080_did_get(&stm32dev, &did);
    if(did != HDC1080_DID)
    {
        printf("did fail\r\n");
        while(1);
    }
    else
    {
        printf("DID = 0x%X\r\n", did);
    }

    hdc1080_soft_rst(&stm32dev, HDC1080_SOFT_RST);

    platform_delay(BOOT_TIME);
    hdc1080_voltage_get(&stm32dev, &voltage);
    if(voltage == HDC1080_G_2V8)
    {
        printf("Battery voltage > 2.8V\r\n");
    }
    else
    {
        printf("Battery voltage < 2.8V\r\n");
    }

    hdc1080_mode_set(&stm32dev, HDC1080_TAH_ACQUIRED);

    th_res.tmp_res = HDC1080_TMP_14BIT;
    th_res.hum_res = HDC1080_HUM_14BIT;
    hdc1080_th_res_set(&stm32dev, th_res);

    hdc1080_data_get(&stm32dev, &th_data);
    printf("Temperature = %.2f, Humidity = %.2f\r\n", th_data.tmp, th_data.hum);
}

static int32_t platform_read_reg(void* handle, uint8_t reg, uint8_t* pbuf, uint16_t len)
{
     if (handle == &SENSOR_BUS)
     {
         HAL_I2C_Mem_Read(handle, HDC1080_ADDR << 1, reg,
                 I2C_MEMADD_SIZE_8BIT, pbuf, len, 1000);
     }
     return 0;
}

static int32_t platform_write_reg(void* handle, uint8_t reg, uint8_t* pbuf, uint16_t len)
{
     if (handle == &SENSOR_BUS)
     {
         HAL_I2C_Mem_Write(handle, HDC1080_ADDR << 1, reg,
                 I2C_MEMADD_SIZE_8BIT, pbuf, len, 1000);
     }
     return 0;
}

static int32_t platform_read(void* handle, uint16_t dev_addr, uint8_t* pbuf, uint16_t len)
{
    if (handle == &SENSOR_BUS)
    {
        HAL_I2C_Master_Receive(handle, dev_addr << 1, pbuf, len, 0xffff);
    }
    return 0;
}

static int32_t platform_write(void* handle, uint16_t dev_addr, uint8_t* pbuf, uint16_t len)
{
    if (handle == &SENSOR_BUS)
    {
        HAL_I2C_Master_Transmit(handle, dev_addr << 1, pbuf, len, 0xffff);
    }
    return 0;
}

void platform_delay(uint32_t ms)
{
    HAL_Delay(ms);
}

static void platform_init(void)
{
//
}
