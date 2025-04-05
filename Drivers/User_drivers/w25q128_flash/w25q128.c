#include "main.h"
#include "w25q128.h"



void W25Q128_ChipSelect(W25Q128_TypeDef *w25q128)
{
    HAL_GPIO_WritePin(w25q128->cs_port, w25q128->cs_pin, GPIO_PIN_SET);
}

void W25Q128_ChipDeselect(W25Q128_TypeDef *w25q128)
{
    HAL_GPIO_WritePin(w25q128->cs_port, w25q128->cs_pin, GPIO_PIN_RESET);
}

void W25Q128_SPIWrite(W25Q128_TypeDef *w25q128, uint8_t *data, uint16_t len, uint32_t timeout)
{
    HAL_SPI_Transmit(&w25q128->hspi, data, len, timeout);
}

void W25Q128_SPIRead(W25Q128_TypeDef *w25q128, uint8_t *data, uint16_t len, uint32_t timeout)
{
    HAL_SPI_Receive(&w25q128->hspi, data, len, timeout);
}

void W25Q128_DelayMs(uint32_t delay_ms)
{
    HAL_Delay(delay_ms);
}

void W25Q128_Reset(W25Q128_TypeDef *w25q128)
{
    uint8_t t_data[2];

    t_data[0] = INST_ENABLE_RESET;
    t_data[1] = INST_RESET_DEVICE;

    W25Q128_ChipSelect(w25q128);

    W25Q128_SPIWrite(w25q128, t_data, 2, 100);

    W25Q128_ChipDeselect(w25q128);

    W25Q128_DelayMs(100);
}

uint32_t W25Q128_ReadID(W25Q128_TypeDef *w25q128, W25Q128_ID_TypeDef id)
{
    uint8_t t_data;
    uint8_t r_data[3];

    switch (id)
    {
        case ID_JEDEC:
            t_data = INST_JEDEC_ID;
            W25Q128_ChipSelect(w25q128);
            W25Q128_SPIWrite(w25q128, &t_data, 1, 100);
            W25Q128_SPIRead(w25q128, r_data, 3, 100);
            W25Q128_ChipDeselect(w25q128);

            // MFN_ID : MEM_ID : CAPACITY_ID
            return ((r_data[0] << 16) | (r_data[1] << 8) | (r_data[2])); 

        default:
            break;
    }
}