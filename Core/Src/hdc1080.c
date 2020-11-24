#include "hdc1080.h"
/**
  * @brief    hdc1080读寄存器函数
  *
  * @param  ctx         设备句柄
  *         reg         寄存器地址
  *         data        传出数据buffer
  *         len         读取字节数
  * @retval             状态值
  *
  */
int32_t hdc1080_read_reg(dev_ctx_t* ctx, uint8_t reg, uint8_t* data, uint16_t len)
{
    int32_t ret;
    ret = ctx->read_reg(ctx->handle, reg, data, len);
    return ret;
}

/**
  * @brief    hdc1080写寄存器函数
  *
  * @param  ctx         设备句柄
  *         reg         寄存器地址
  *         data        要写的数据buffer
  *         len         写入字节数
  * @retval             状态值
  *
  */
int32_t hdc1080_write_reg(dev_ctx_t* ctx, uint8_t reg, uint8_t* data, uint16_t len)
{
    int32_t ret;
    ret = ctx->write_reg(ctx->handle, reg, data, len);
    return ret;
}

/**
  * @brief    hdc1080数据接收函数
  *
  * @param  ctx         设备句柄
  *         dev_addr    IIC器件地址
  *         data        传出数据buffer
  *         len         读取字节数
  * @retval             状态值
  *
  */
int32_t hdc1080_read(dev_ctx_t* ctx, uint16_t dev_addr, uint8_t* data, uint16_t len)
{
    int32_t ret;
    ret = ctx->read(ctx->handle, dev_addr, data, len);
    return ret;
}

/**
  * @brief    hdc1080数据发送函数
  *
  * @param  ctx         设备句柄
  *         dev_addr    IIC器件地址
  *         data        要写的数据buffer
  *         len         写入字节数
  * @retval             状态值
  *
  */
int32_t hdc1080_write(dev_ctx_t* ctx, uint16_t dev_addr, uint8_t* data, uint16_t len)
{
    int32_t ret;
    ret = ctx->write(ctx->handle, dev_addr, data, len);
    return ret;
}

extern void platform_delay(uint32_t ms);

/**
  * @brief    hdc1080温湿度读取函数
  *
  * @param  ctx         设备句柄
  *         data        温湿度数据传出buffer
  * @retval             状态值
  *
  */
int32_t hdc1080_data_get(dev_ctx_t* ctx, hdc1080_data_t* data)
{
    int32_t ret = 0;
    hdc1080_mode_t mode;
    uint8_t rb[4] = {0};
    uint8_t wb = 0;
    ret |= hdc1080_mode_get(ctx, &mode);
    if(ret == 0 && mode == HDC1080_TOH_ACQUIRED)
    {
        wb = HDC1080_REG_TMP;
        ret |= hdc1080_write(ctx, HDC1080_ADDR, &wb, 1);
        platform_delay(HDC1080_CNV_TIME);
        ret |= hdc1080_read(ctx, HDC1080_ADDR, rb, 2);
        if(ret == 0)
        {
            data->tmp_raw = (uint16_t)rb[0] << 8 | (uint16_t)rb[1];
            data->tmp = ((data->tmp_raw)/65536.0)*165.0-40.0;
        }
        wb = HDC1080_REG_HUM;
        ret |= hdc1080_write(ctx, HDC1080_ADDR, &wb, 1);
        platform_delay(HDC1080_CNV_TIME);
        ret |= hdc1080_read(ctx, HDC1080_ADDR, rb, 2);
        if(ret == 0)
        {
            data->hum_raw = (uint16_t)rb[0] << 8 | (uint16_t)rb[1];
            data->hum = (data->hum_raw)/65536.0;
        }
    }
    else if(ret == 0 && mode == HDC1080_TAH_ACQUIRED)
    {
        wb = HDC1080_REG_TMP;
        ret |= hdc1080_write(ctx, HDC1080_ADDR, &wb, 1);
        platform_delay(HDC1080_CNV_TIME);
        ret |= hdc1080_read(ctx, HDC1080_ADDR, rb, 4);
        if(ret == 0)
        {
            data->tmp_raw = (uint16_t)rb[0] << 8 | (uint16_t)rb[1];
            data->hum_raw = (uint16_t)rb[2] << 8 | (uint16_t)rb[3];
            data->tmp = ((data->tmp_raw)/65536.0)*165.0-40.0;
            data->hum = (data->hum_raw)/65536.0;
        }
        else
        {
            data->tmp = 0;
            data->hum = 0;
            data->tmp_raw = 0;
            data->hum_raw = 0;
        }
    }
    return ret;
}

/**
  * @brief    hdc1080软复位函数
  *
  * @param  ctx         设备句柄
  *         val         复位操作数可以是 HDC1080_SOFT_RST, HDC1080_NORMAL
  * @retval             状态值
  *
  */
int32_t hdc1080_soft_rst(dev_ctx_t* ctx, hdc1080_rst_t val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(ret == 0)
    {
        cfg.rst = (uint8_t)val;
        ret = hdc1080_write_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    }
    return ret;
}

/**
  * @brief    hdc1080加热器开启状态获取函数
  *
  * @param  ctx         设备句柄
  *         val         传出的加热器状态
  * @retval             状态值
  *
  */
int32_t hdc1080_heater_get(dev_ctx_t* ctx, hdc1080_heater_t* val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(cfg.heat == HDC1080_HEATER_DISABLE)
    {
        *val = HDC1080_HEATER_DISABLE;
    }
    else if(cfg.heat == HDC1080_HEATER_ENABLE)
    {
        *val = HDC1080_HEATER_ENABLE;
    }
    return ret;
}

/**
  * @brief    hdc1080加热器操作函数
  *
  * @param  ctx         设备句柄
  *         val         传入的加热器操作数，可以是HDC1080_HEATER_DISABLE, HDC1080_HEATER_ENABLE
  * @retval             状态值
  *
  */
int32_t hdc1080_heater_set(dev_ctx_t* ctx, hdc1080_heater_t val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(ret == 0)
    {
        cfg.heat = (uint8_t)val;
        ret = hdc1080_write_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    }
    return ret;
}

/**
  * @brief    hdc1080内部电压状态获取函数
  *
  * @param  ctx         设备句柄
  *         val         传出的电压状态
  * @retval             状态值
  *
  */
int32_t hdc1080_voltage_get(dev_ctx_t* ctx, hdc1080_voltage_t* val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(cfg.btst == HDC1080_G_2V8)
    {
        *val = HDC1080_G_2V8;
    }
    else if(cfg.btst == HDC1080_L_2V8)
    {
        *val = HDC1080_L_2V8;
    }
    return ret;
}

/**
  * @brief    hdc1080采样模式获取函数
  *
  * @param  ctx         设备句柄
  *         val         传出的采样模式
  * @retval             状态值
  *
  */
int32_t hdc1080_mode_get(dev_ctx_t* ctx, hdc1080_mode_t* val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(cfg.mode == HDC1080_TOH_ACQUIRED)
    {
        *val = HDC1080_TOH_ACQUIRED;
    }
    else if(cfg.mode == HDC1080_TAH_ACQUIRED)
    {
        *val = HDC1080_TAH_ACQUIRED;
    }
    return ret;
}

/**
  * @brief    hdc1080模式设置函数
  *
  * @param  ctx         设备句柄
  *         val         传入的模式参数，可以是HDC1080_TOH_ACQUIRED, HDC1080_TAH_ACQUIRED
  * @retval             状态值
  *
  */
int32_t hdc1080_mode_set(dev_ctx_t* ctx, hdc1080_mode_t val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(ret == 0)
    {
        cfg.mode = (uint8_t)val;
        ret = hdc1080_write_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    }
    return ret;
}

/**
  * @brief    hdc1080温湿度数据结果位宽获取函数
  *
  * @param  ctx         设备句柄
  *         val         传出的数据结果位宽
  * @retval             状态值
  *
  */
int32_t hdc1080_th_res_get(dev_ctx_t* ctx, hdc1080_th_res_t* val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(cfg.tres == HDC1080_TMP_14BIT)
    {
        val->tmp_res = HDC1080_TMP_14BIT;
    }
    else
    {
        val->tmp_res = HDC1080_TMP_11BIT;
    }
    
    if(cfg.hres == HDC1080_HUM_14BIT)
    {
        val->hum_res = HDC1080_HUM_14BIT;
    }
    else if(cfg.hres == HDC1080_HUM_11BIT)
    {
        val->hum_res = HDC1080_HUM_11BIT;
    }
    else
    {
        val->hum_res = HDC1080_HUM_8BIT;
    }
    return ret;
}

/**
  * @brief    hdc1080温湿度数据结果位宽设置函数
  *
  * @param  ctx         设备句柄
  *         val         传入的数据结果位宽，温度可以是HDC1080_TMP_14BIT, HDC1080_TMP_11BIT
  *                                                                                                                      湿度可以是HDC1080_HUM_8BIT HDC1080_HUM_11BIT HDC1080_HUM_14BIT
  * @retval             状态值
  *
  */
int32_t hdc1080_th_res_set(dev_ctx_t* ctx, hdc1080_th_res_t val)
{
    int32_t ret;
    hdc1080_cfg_t cfg;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    if(ret == 0)
    {
        cfg.tres = (uint8_t)val.tmp_res;
        cfg.hres = (uint8_t)val.hum_res;
        ret = hdc1080_write_reg(ctx, HDC1080_REG_CFG, (uint8_t*)&cfg, 2);
    }
    return ret;
}

/**
  * @brief    hdc1080温湿度传感器串号查询函数
  *
  * @param  ctx         设备句柄
  *         val         传出的串号
  * @retval             状态值
  *
  */
int32_t hdc1080_sid_get(dev_ctx_t* ctx, uint64_t* val)
{
    int32_t ret;
    uint64_t fsid, msid, lsid;
    ret = hdc1080_read_reg(ctx, HDC1080_REG_FSID, (uint8_t*)&fsid, 2);
    ret = hdc1080_read_reg(ctx, HDC1080_REG_MSID, (uint8_t*)&msid, 2);
    ret = hdc1080_read_reg(ctx, HDC1080_REG_LSID, (uint8_t*)&lsid, 2);
    *val = (fsid << 25) + (msid << 9) + (lsid >> 7);
    return ret;
}

/**
  * @brief    hdc1080温湿度传感器制造商标识查询函数
  *
  * @param  ctx         设备句柄
  *         val         传出的制造商标
  * @retval             状态值
  *
  */
int32_t hdc1080_mid_get(dev_ctx_t* ctx, uint16_t* val)
{
    int32_t ret;
    uint8_t tmp[2];
    ret = hdc1080_read_reg(ctx, HDC1080_REG_MID, tmp, 2);
    *val = ((uint16_t)tmp[0]<<8)|((uint16_t)tmp[1]);
    return ret;
}

/**
  * @brief    hdc1080温湿度传感器设备标识查询函数
  *
  * @param  ctx         设备句柄
  *         val         传出的设备ID
  * @retval             状态值
  *
  */
int32_t hdc1080_did_get(dev_ctx_t* ctx, uint16_t* val)
{
    int32_t ret;
    uint8_t tmp[2];
    ret = hdc1080_read_reg(ctx, HDC1080_REG_DID, tmp, 2);
    *val = ((uint16_t)tmp[0]<<8)|((uint16_t)tmp[1]);
    return ret;
}
