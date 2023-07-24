#include "bsp_scan.h"
#include "gw_gray.h"
#include "main.h"

#define BSP_SCANNER_GROUP1_I2C_DEV  (&bsp_i2c_dev_scanner)
#define BSP_SCANNER_GROUP1_I2C_ADDR (0b10011110)
#define BSP_SCANNER_GROUP2_I2C_DEV  (&bsp_i2c_dev_scanner)
#define BSP_SCANNER_GROUP2_I2C_ADDR (0b10011000)

bsp_scanner_group_t bsp_scanner_groups[BSP_SCANNER_GROUP_COUNT] = {
    {.dev = BSP_SCANNER_GROUP1_I2C_DEV, .i2c_addr = BSP_SCANNER_GROUP1_I2C_ADDR},
    {.dev = BSP_SCANNER_GROUP2_I2C_DEV, .i2c_addr = BSP_SCANNER_GROUP2_I2C_ADDR}
};

bool bsp_scanner_group_update(bsp_scanner_group_t *group){
    
    static uint8_t digital_data;
    static uint8_t analog_data[BSP_SCANNER_COUNT];
    bsp_i2c_master_read_reg(group->dev, group->i2c_addr, GW_GRAY_DIGITAL_MODE, &digital_data, 1);
    group->digital_byte = ~digital_data;
    for(int i=0;i<BSP_SCANNER_COUNT;i++){
        group->datas[i].digital = (digital_data & 1) == 0;
        digital_data = digital_data >> 1;
    }

    #if BSP_SCANNER_ENABLE_READ_ANALOG
    static uint8_t norm = 0xFF;
    bsp_i2c_master_write_reg(group->dev, group->i2c_addr, GW_GRAY_ANALOG_NORMALIZE, &norm, 1);
    bsp_i2c_master_read_reg(group->dev, group->i2c_addr, GW_GRAY_ANALOG_MODE, &analog_data, BSP_SCANNER_COUNT);
    for(int i=0;i<BSP_SCANNER_COUNT;i++){
        group->datas[i].analog = analog_data[i];
    }
    #endif
    return true;
}