/**
 * @file tca8418.c
 * @brief TCA8418 driver implementation
 * @details This file contains the implementation of the TCA8418 driver,
 *          supporting key status and configuration.
 * @author Cengiz Sinan Kostakoglu
 * @version 1.0
 * @date 2025-06-08
 */

#include "tca8418.h"

/* TCA8418 I2C Address */
#define TCA8418_ADDRESS 0x34

/* Register addresses */
#define CFG             0x01 //< Configuration Register
#define INT_STAT        0x02 //< Interrupt Status Register
#define KEY_LCK_EC      0x03 //< Key Lock AND Event Counter Register
#define KEY_EVENT_A     0x04 //< Key Event A Register
#define KEY_EVENT_B     0x05 //< Key Event B Register
#define KEY_EVENT_C     0x06 //< Key Event C Register
#define KEY_EVENT_D     0x07 //< Key Event D Register
#define KEY_EVENT_E     0x08 //< Key Event E Register
#define KEY_EVENT_F     0x09 //< Key Event F Register
#define KEY_EVENT_G     0x0A //< Key Event G Register
#define KEY_EVENT_H     0x0B //< Key Event H Register
#define KEY_EVENT_I     0x0C //< Key Event I Register
#define KEY_EVENT_J     0x0D //< Key Event J Register
#define KP_LCK_TIMER    0x0E //< Keypad Lock 1 to Lock 2 Timer
#define UNLOCK1         0x0F //< Unlock 1 Register
#define UNLOCK2         0x10 //< Unlock 2 Register
#define GPIO_INT_STAT1  0x11 //< GPIO Interrupt Status 1 Register
#define GPIO_INT_STAT2  0x12 //< GPIO Interrupt Status 2 Register
#define GPIO_INT_STAT3  0x13 //< GPIO Interrupt Status 3 Register
#define GPIO_DAT_STAT1  0x14 //< GPIO Data Status 1 Register
#define GPIO_DAT_STAT2  0x15 //< GPIO Data Status 2 Register
#define GPIO_DAT_STAT3  0x16 //< GPIO Data Status 3 Register
#define GPIO_DAT_OUT1   0x17 //< GPIO Data Output 1 Register
#define GPIO_DAT_OUT2   0x18 //< GPIO Data Output 2 Register
#define GPIO_DAT_OUT3   0x19 //< GPIO Data Output 3 Register
#define GPIO_INT_EN1    0x1A //< GPIO Interrupt Enable 1 Register
#define GPIO_INT_EN2    0x1B //< GPIO Interrupt Enable 2 Register
#define GPIO_INT_EN3    0x1C //< GPIO Interrupt Enable 3 Register
#define KP_GPIO1        0x1D //< Keypad or GPIO Selection 1 Register
#define KP_GPIO2        0x1E //< Keypad or GPIO Selection 2 Register
#define KP_GPIO3        0x1F //< Keypad or GPIO Selection 3 Register
#define GPIO_EM1        0x20 //< GPIO Event Mode 1 Register
#define GPIO_EM2        0x21 //< GPIO Event Mode 2 Register
#define GPIO_EM3        0x22 //< GPIO Event Mode 3 Register
#define GPIO_DIR1       0x23 //< GPIO Direction 1 Register
#define GPIO_DIR2       0x24 //< GPIO Direction 2 Register
#define GPIO_DIR3       0x25 //< GPIO Direction 3 Register
#define GPIO_INT_LVL1   0x26 //< GPIO Edge/Level Detect 1 Register
#define GPIO_INT_LVL2   0x27 //< GPIO Edge/Level Detect 2 Register
#define GPIO_INT_LVL3   0x28 //< GPIO Edge/Level Detect 3 Register
#define DEBOUNCE_DIS1   0x29 //< Debounce Disable 1 Register
#define DEBOUNCE_DIS2   0x2A //< Debounce Disable 2 Register
#define DEBOUNCE_DIS3   0x2B //< Debounce Disable 3 Register
#define GPIO_PULL1      0x2C //< GPIO Pull-up Disable 1 Register
#define GPIO_PULL2      0x2D //< GPIO Pull-up Disable 2 Register
#define GPIO_PULL3      0x2E //< GPIO Pull-up Disable 3 Register

/* Change this according to your I2C handle declared in main.c */
extern I2C_HandleTypeDef hi2c1; 

/**
 * @brief Read data from TCA8418 register(s)
 * @param reg Register address to read from
 * @param data Pointer to store read data
 * @param length Number of bytes to read
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
static inline HAL_StatusTypeDef TCA8418_ReadRegister(uint8_t reg, uint8_t *data, uint16_t length){
    return HAL_I2C_Mem_Read(&hi2c1, (TCA8418_ADDRESS << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}   

/**
 * @brief Write data to TCA8418 register(s)
 * @param reg Register address to write to
 * @param data Pointer to data to write
 * @param length Number of bytes to write
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
static inline HAL_StatusTypeDef TCA8418_WriteRegister(uint8_t reg, uint8_t *data, uint16_t length){
    return HAL_I2C_Mem_Write(&hi2c1, (TCA8418_ADDRESS << 1), reg, I2C_MEMADD_SIZE_8BIT, data, length, HAL_MAX_DELAY);
}

/**
 * @brief Configure TCA8418 for keypad and GPIO operation
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 * @note This function configures the TCA8418 for keypad operation with pins ROW0 and COL6:0.(7 keys in total)   
 *       It also configures other unused pins as GPIO inputs with pull-up enabled without interrupts.
 */
static inline HAL_StatusTypeDef TCA8418_KPConfig(void){
    HAL_StatusTypeDef status;
    uint8_t data = 0x01; // ROW0[0] = 1 -> Keypad row is selected
    status = TCA8418_WriteRegister(KP_GPIO1, &data, 1);
    if(status != HAL_OK){
        return status;
    }
    data = 0x7F; // COL6:0[6:0] = 1 -> Keypad columns are selected
    status = TCA8418_WriteRegister(KP_GPIO2, &data, 1);
    if(status != HAL_OK){
        return status;
    }   
    data = 0x01; // R7:1IE[7:1] = 0 -> Row7:1 interrupts are disabled
        status = TCA8418_WriteRegister(GPIO_INT_EN1, &data, 1);
    if(status != HAL_OK){
        return status;
    }
    data = 0x7F; // C7IE[7] = 0 -> Column7 interrupt is disabled
    status = TCA8418_WriteRegister(GPIO_INT_EN2, &data, 1);
    if(status != HAL_OK){
        return status;
    }
    data = 0x00; // C9:8IE[1:0] = 0 -> Column9:8 interrupts are disabled
    status = TCA8418_WriteRegister(GPIO_INT_EN3, &data, 1);
    if(status != HAL_OK){
        return status;
    }
    return HAL_OK;
}

/**
 * @brief Enable TCA8418 key events interrupt
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */ 
static inline HAL_StatusTypeDef TCA8418_EnableInterrupt(void){
    HAL_StatusTypeDef status;
    uint8_t data = 0x01; // KE_IEN[0] = 1
    status = TCA8418_WriteRegister(CFG, &data, 1);
    if(status != HAL_OK){
        return status;
    }
    return HAL_OK;
}

/**
 * @brief Initialize TCA8418 with key events interrupt enabled
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
HAL_StatusTypeDef TCA8418_Init(void){
    HAL_StatusTypeDef status;
    status = TCA8418_KPConfig();
    if(status != HAL_OK){
        return status;
    }
    status = TCA8418_EnableInterrupt();
    if(status != HAL_OK){
        return status;
    }
    return HAL_OK;
}

/**
 * @brief Read key events from TCA8418 FIFO
 * @param keyEvents Array to store key events (up to 10 events)
 * @param numEvents Pointer to store number of events read
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 * @note This function reads all pending key events from the FIFO.
 *       Each event is stored as a byte where:
 *       - Bits 6:0 indicate the key number (0-80 for keypad, 97-114 for GPIO)
 *       - Bit 7 indicates event type (0=release, 1=press)
 */
HAL_StatusTypeDef TCA8418_ReadKeyEvents(uint8_t *keyEvents, uint8_t *numEvents) {
    HAL_StatusTypeDef status;
    uint8_t intStatus;
    uint8_t eventCount;
    /* First check if there are any interrupts */
    status = TCA8418_ReadRegister(INT_STAT, &intStatus, 1);
    if(status != HAL_OK){
        return status;
    }
    /* Check if there are key events (KE_INT bit) */
    if(!(intStatus & 0x01)){
        *numEvents = 0;
        return HAL_OK; // No events
    }
    /* Read the event counter */
    status = TCA8418_ReadRegister(KEY_LCK_EC, &eventCount, 1);
    if(status != HAL_OK){
        return status;
    }
    /* Limit to maximum 10 events */
    if(eventCount > 10){
        eventCount = 10;
    }
    /* Read all events from FIFO */
    for(uint8_t i = 0; i < eventCount; i++){
        status = TCA8418_ReadRegister(KEY_EVENT_A, &keyEvents[i], 1);
        if(status != HAL_OK){
            return status;
        }
    }
    *numEvents = eventCount;
    /* Clear the interrupt by writing 1 to KE_INT bit */
    intStatus = 0x01;
    status = TCA8418_WriteRegister(INT_STAT, &intStatus, 1);
    if(status != HAL_OK){   
        return status;
    }
    return HAL_OK;
}

/**
 * @brief Lock TCA8418 keypad except POWER key
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 * @note This function disables all keypad columns except column 0 (POWER key)
 */
HAL_StatusTypeDef TCA8418_LockKeypad(void){
    HAL_StatusTypeDef status;
    uint8_t data;
    /* Enable only column 0 (POWER key) for keypad operation */
    data = 0x01; // COL0[0] = 1 -> Enable column 0 for keypad
    status = TCA8418_WriteRegister(KP_GPIO2, &data, 1);
    if(status != HAL_OK){
        return status;
    }   
    /* Disable interrupt for all columns except column 0 */
    data = 0x7F; // C7:1IE[7:1] = 0 -> Disable interrupts for columns 1-7
    status = TCA8418_WriteRegister(GPIO_INT_EN2, &data, 1);
    if(status != HAL_OK){
        return status;
    }   
    return HAL_OK;
}   

/**
 * @brief Unlock TCA8418 keypad
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 * @note This function enables all keypad columns (0-6) and their interrupts
 */
HAL_StatusTypeDef TCA8418_UnlockKeypad(void){
    HAL_StatusTypeDef status;
    uint8_t data;
    /* Enable all columns (0-6) for keypad operation */
    data = 0x7F; // COL6:0[6:0] = 1 -> Enable columns 0-6 for keypad
    status = TCA8418_WriteRegister(KP_GPIO2, &data, 1);
    if(status != HAL_OK){
        return status;
    }   
    /* Disable interrupt for column 7 */
    data = 0x7F; // C7IE[7] = 0 -> Disable interrupt for column 7
    status = TCA8418_WriteRegister(GPIO_INT_EN2, &data, 1);
    if(status != HAL_OK){
        return status;
    }
    return HAL_OK;
}       