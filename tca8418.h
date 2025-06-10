/**
 * @file tca8418.h
 * @brief TCA8418 I2C keypad controller driver header
 * @details This header file contains the declarations and definitions for the
 *          TCA8418 I2C keypad controller driver, supporting various
 *          keypad operations.
 * @author Cengiz Sinan Kostakoglu
 * @version 1.0
 * @date 2025-06-08
 */

#ifndef __TCA8418_H__
#define __TCA8418_H__

#ifdef __cplusplus
extern "C" {
#endif

/* For uint8_t, uint16_t, uint32_t */
#include <stdint.h> 
/* For HAL functions */
#include "main.h"

/**
 * @brief Initialize TCA8418 with key events interrupt enabled
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
HAL_StatusTypeDef TCA8418_Init(void);

/**
 * @brief Read key events from TCA8418 FIFO
 * @param keyEvents Array to store key events (up to 10 events)
 * @param numEvents Pointer to store number of events read
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
HAL_StatusTypeDef TCA8418_ReadKeyEvents(uint8_t *keyEvents, uint8_t *numEvents);  

/**
 * @brief Lock TCA8418 keypad except POWER key
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
HAL_StatusTypeDef TCA8418_LockKeypad(void);

/**
 * @brief Unlock TCA8418 keypad
 * @return HAL_StatusTypeDef HAL_OK if successful, otherwise error code
 */
HAL_StatusTypeDef TCA8418_UnlockKeypad(void);

#ifdef __cplusplus
}
#endif

#endif
