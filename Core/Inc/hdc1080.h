#ifndef _HDC1080_H
#define _HDC1080_H

#ifdef __cplusplus
  extern "C" {
#endif

/* --------------------------Includes-------------------------- */
#include <stdint.h>

typedef int32_t(*dev_write_reg_ptr)(void*, uint8_t, uint8_t*, uint16_t);
typedef int32_t(*dev_read_reg_ptr)(void*, uint8_t, uint8_t*, uint16_t);
typedef int32_t(*dev_write_ptr)(void*, uint16_t, uint8_t*, uint16_t);
typedef int32_t(*dev_read_ptr)(void*, uint16_t, uint8_t*, uint16_t);

typedef struct
{
  void* handle;

  dev_read_reg_ptr read_reg;
  dev_write_reg_ptr write_reg;
  dev_read_ptr read;
  dev_write_ptr write;
}dev_ctx_t;

int32_t hdc1080_read_reg(dev_ctx_t* ctx, uint8_t reg, uint8_t* data, uint16_t len);
int32_t hdc1080_write_reg(dev_ctx_t* ctx, uint8_t reg, uint8_t* data, uint16_t len);
int32_t hdc1080_read(dev_ctx_t* ctx, uint16_t dev_addr, uint8_t* data, uint16_t len);
int32_t hdc1080_write(dev_ctx_t* ctx, uint16_t dev_addr, uint8_t* data, uint16_t len);

#define HDC1080_ADDR 0x40
#define HDC1080_CNV_TIME 20

#define HDC1080_REG_TMP 0x00
#define HDC1080_REG_HUM 0x01
typedef struct 
{
  uint16_t tmp_raw;
  uint16_t hum_raw;
  float tmp;
  float hum;
} hdc1080_data_t;

int32_t hdc1080_data_get(dev_ctx_t* ctx, hdc1080_data_t* data);


#define HDC1080_REG_CFG 0x02

typedef struct
{
    uint8_t hres     : 2;
    uint8_t tres     : 1;
    uint8_t btst     : 1;
    uint8_t mode     : 1;
    uint8_t heat     : 1;
    uint8_t notused2 : 1;
    uint8_t rst      : 1;
    uint8_t notused1 : 8;
} hdc1080_cfg_t;

typedef enum
{
  HDC1080_NORMAL   = 0,
  HDC1080_SOFT_RST = 1
} hdc1080_rst_t;

int32_t hdc1080_soft_rst(dev_ctx_t* ctx, hdc1080_rst_t val);


typedef enum
{
  HDC1080_HEATER_DISABLE = 0,
  HDC1080_HEATER_ENABLE  = 1
} hdc1080_heater_t;

int32_t hdc1080_heater_get(dev_ctx_t* ctx, hdc1080_heater_t* val);
int32_t hdc1080_heater_set(dev_ctx_t* ctx, hdc1080_heater_t val);


typedef enum
{
  HDC1080_G_2V8 = 0,
  HDC1080_L_2V8 = 1
} hdc1080_voltage_t;

int32_t hdc1080_voltage_get(dev_ctx_t* ctx, hdc1080_voltage_t* val);


typedef enum
{
  HDC1080_TOH_ACQUIRED = 0,
  HDC1080_TAH_ACQUIRED = 1
} hdc1080_mode_t;

int32_t hdc1080_mode_get(dev_ctx_t* ctx, hdc1080_mode_t* val);
int32_t hdc1080_mode_set(dev_ctx_t* ctx, hdc1080_mode_t val);


typedef struct 
{
  enum
  {
    HDC1080_TMP_14BIT = 0,
    HDC1080_TMP_11BIT = 1
  } tmp_res;
  enum
  {
    HDC1080_HUM_14BIT = 0,
    HDC1080_HUM_11BIT = 1,
    HDC1080_HUM_8BIT  = 1
  } hum_res;
} hdc1080_th_res_t;

int32_t hdc1080_th_res_get(dev_ctx_t* ctx, hdc1080_th_res_t* val);
int32_t hdc1080_th_res_set(dev_ctx_t* ctx, hdc1080_th_res_t val);


#define HDC1080_REG_FSID 0xFB
#define HDC1080_REG_MSID 0xFC
#define HDC1080_REG_LSID 0xFD

int32_t hdc1080_sid_get(dev_ctx_t* ctx, uint64_t* val);


#define HDC1080_MID 0x5449
#define HDC1080_REG_MID 0xFE

int32_t hdc1080_mid_get(dev_ctx_t* ctx, uint16_t* val);

#define HDC1080_DID 0x1050 
#define HDC1080_REG_DID 0xFF

int32_t hdc1080_did_get(dev_ctx_t* ctx, uint16_t* val);


#ifdef __cplusplus
}
#endif

#endif
