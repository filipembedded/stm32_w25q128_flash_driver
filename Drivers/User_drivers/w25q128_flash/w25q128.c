#include "main.h"
#include "w25q128.h"

#define W25Q128_SPI_TIMEOUT 100

void W25Q128_ChipSelect(W25Q128_TypeDef *w25q128)
{
    HAL_GPIO_WritePin(w25q128->cs_port, w25q128->cs_pin, GPIO_PIN_RESET);
}

void W25Q128_ChipDeselect(W25Q128_TypeDef *w25q128)
{
    HAL_GPIO_WritePin(w25q128->cs_port, w25q128->cs_pin, GPIO_PIN_SET);
}

void W25Q128_SPIWrite(W25Q128_TypeDef *w25q128, uint8_t *data, uint16_t len, 
                                                            uint32_t timeout)
{
    HAL_SPI_Transmit(w25q128->hspi, data, len, timeout);
}

void W25Q128_SPIRead(W25Q128_TypeDef *w25q128, uint8_t *data, uint16_t len, 
                                                            uint32_t timeout)
{
    HAL_SPI_Receive(w25q128->hspi, data, len, timeout);
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

    W25Q128_SPIWrite(w25q128, t_data, 2, W25Q128_SPI_TIMEOUT);

    W25Q128_ChipDeselect(w25q128);

    W25Q128_DelayMs(100);
}

uint32_t W25Q128_ReadID(W25Q128_TypeDef *w25q128, W25Q128_ID_TypeDef id)
{
    uint8_t t_data;
    uint8_t r_data[3];
    uint32_t result_id;

    switch (id)
    {
        case ID_JEDEC:
            t_data = INST_JEDEC_ID;
            W25Q128_ChipSelect(w25q128);
            W25Q128_SPIWrite(w25q128, &t_data, 1, W25Q128_SPI_TIMEOUT);
            W25Q128_SPIRead(w25q128, r_data, 3, W25Q128_SPI_TIMEOUT);
            W25Q128_ChipDeselect(w25q128);

            // MFN_ID : MEM_ID : CAPACITY_ID
            result_id = ((r_data[0] << 16) | (r_data[1] << 8) | (r_data[2])); 
            break;

        default:
            break;
    }
    return result_id;
}

W25Q128_StatusTypeDef W25Q128_Read(W25Q128_TypeDef *w25,
                                    uint32_t start_page,
                                    uint8_t offset,
                                    uint32_t size,
                                    uint8_t *r_data)
{
    uint8_t t_data[4];
    uint32_t mem_addr = (start_page * 256) + offset;

    t_data[0] = INST_READ_DATA;
    t_data[1] = (mem_addr >> 16) & 0xFF; // MSB of 24-bit memory address
    t_data[2] = (mem_addr >> 8) & 0xFF;
    t_data[3] = (mem_addr) & 0xFF;       // LSB of 24-bit memory address

    W25Q128_ChipSelect(w25);
    W25Q128_SPIWrite(w25, t_data, sizeof(t_data), W25Q128_SPI_TIMEOUT);
    W25Q128_SPIRead(w25, r_data, size, W25Q128_SPI_TIMEOUT);
    W25Q128_ChipDeselect(w25);

    return W25Q128_SUCCESS;
}

W25Q128_StatusTypeDef W25Q128_FastRead(W25Q128_TypeDef *w25,
                                        uint32_t start_page,
                                        uint8_t offset,
                                        uint32_t size,
                                        uint8_t *r_data)
{
uint8_t t_data[5];
uint32_t mem_addr = (start_page * 256) + offset;

t_data[0] = INST_FAST_READ;
t_data[1] = (mem_addr >> 16) & 0xFF; // MSB of 24-bit memory address
t_data[2] = (mem_addr >> 8) & 0xFF;
t_data[3] = (mem_addr) & 0xFF;       // LSB of 24-bit memory address
t_data[4] = 0x00;

W25Q128_ChipSelect(w25);
W25Q128_SPIWrite(w25, t_data, sizeof(t_data), W25Q128_SPI_TIMEOUT);
W25Q128_SPIRead(w25, r_data, size, W25Q128_SPI_TIMEOUT);
W25Q128_ChipDeselect(w25);

return W25Q128_SUCCESS;
}