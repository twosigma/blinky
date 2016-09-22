/*
 * Copyright (c) 2016, Blinkinlabs, LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Blinkinlabs, LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mma8653.h"

#include <Arduino.h>
#include <Wire.h>
//#include <stdint.h>

#define MMA8653_ADDRESS    0x1D

#define STATUS             0x00    // Real time status
#define OUT_X_MSB          0x01    // [7:0] are 8 MSBs of 10-bit real-time sample
#define OUT_X_LSB          0x02    // [7:6] are 2 LSBs of 10-bit real-time sample
#define OUT_Y_MSB          0x03    // [7:0] are 8 MSBs of 10-bit real-time sample
#define OUT_Y_LSB          0x04    // [7:6] are 2 LSBs of 10-bit real-time sample
#define OUT_Z_MSB          0x05    // [7:0] are 8 MSBs of 10-bit real-time sample
#define OUT_Z_LSB          0x06    // [7:6] are 2 LSBs of 10-bit real-time sample

#define SYSMOD             0x0B    // Current System Mode
#define INT_SOURCE         0x0C    // Interrupt status
#define WHO_AM_I           0x0D    // Device ID (0x5A)
#define XYZ_DATA_CFG       0x0E    // Dynamic Range Settings

#define PL_STATUS          0x10    // Landscape/Portrait orientation status
#define PL_CFG             0x11    // Landscape/Portrait configuration.
#define PL_COUNT           0x12    // Landscape/Portrait debounce counter
#define PL_BF_ZCOMP        0x13    // Back/Front, Z-Lock Trip threshold
#define PL_THS_REG         0x14    // Portrait to Landscape Trip angle
#define FF_MT_CFG          0x15    // Freefall/Motion functional block configuration
#define FF_MT_SRC          0x16    // Freefall/Motion event source register
#define FF_MT_THS          0x17    // Freefall/Motion threshold register
#define FF_MT_COUNT        0x18    // Freefall/Motion debounce counter

#define ASLP_COUNT         0x29    // Counter setting for Auto-SLEEP/WAKE
#define CTRL_REG1          0x2A    // Data Rates, ACTIVE Mode.
#define CTRL_REG2          0x2B    // Sleep Enable, OS Modes, RST, ST
#define CTRL_REG3          0x2C    // Wake from Sleep, IPOL, PP_OD
#define CTRL_REG4          0x2D    // Interrupt enable register
#define CTRL_REG5          0x2E    // Interrupt pin (INT1/INT2) map
#define OFF_X              0x2F    // X-axis offset adjust
#define OFF_Y              0x30    // Y-axis offset adjust
#define OFF_Z              0x31    // Z-axis offset adjust


#define XYZ_DATA_CFG_2G    0x00
#define XYZ_DATA_CFG_4G    0x01
#define XYZ_DATA_CFG_8G    0x02

#define CTRL_REG1_ACTIVE   0x01                            // Full-scale selection
#define CTRL_REG1_F_READ   0x02                            // Fast Read Mode
#define CTRL_REG1_DR(n)    (uint8_t)(((n) & 0x07) << 3)    // Data rate selection
#define CTRL_REG1_ASLP_RATE(n) (uint8_t)(((n) & 0x03) << 6)// Auto-WAKE sample frequency

#define CTRL_REG2_ST       0x80                            // Self-Test Enable
#define CTRL_REG2_RST      0x40                            // Software Reset
#define CTRL_REG2_SMODS(n) (uint8_t)(((n) & 0x03) << 3)    // SLEEP mode power scheme selection
#define CTRL_REG2_SLPE     0x02                            // Auto-SLEEP enable
#define CTRL_REG2_MODS(n)  (uint8_t)(((n) & 0x03))         // ACTIVE mode power scheme selection

#define CTRL_REG3_PP_OD         0x01
#define CTRL_REG3_IPOL          0x02
#define CTRL_REG3_WAKE_FF_MT    0x08
#define CTRL_REG3_WAKE_LNDPRT   0x20

#define CTRL_REG4_INT_EN_DRDY   0x01
#define CTRL_REG4_INT_EN_FF_MT  0x04
#define CTRL_REG4_INT_EN_LNDPRT 0x10
#define CTRL_REG4_INT_EN_ASLP   0x80

#define CTRL_REG5_INT_CFG_DRDY   0x01
#define CTRL_REG5_INT_CFG_FF_MT  0x04
#define CTRL_REG5_INT_CFG_LNDPRT 0x10
#define CTRL_REG5_INT_CFG_ASLP   0x80


void MMA8653::setup() {
  // Set up the I2C peripheral
  Wire.begin();

  // Reset the device, to put it into a known state.
  Wire.beginTransmission(MMA8653_ADDRESS);
  Wire.write(CTRL_REG2);
  Wire.write(CTRL_REG2_RST);
  Wire.endTransmission();

  delay(1); // Allow the device to reset

  // Check that we're talking to the right kind of device
  Wire.beginTransmission(MMA8653_ADDRESS);
  Wire.write(WHO_AM_I);
  Wire.endTransmission(false);

  Wire.requestFrom(MMA8653_ADDRESS, 1);
  while(Wire.available()) {
    Wire.read();
    // TODO: Test if this is equal to 0x5A?
  }

  // Configure for 8G sensitivity
  Wire.beginTransmission(MMA8653_ADDRESS);
  Wire.write(XYZ_DATA_CFG);
  Wire.write(XYZ_DATA_CFG_8G);
  Wire.endTransmission();


  // Enable data ready interrupt on interrput pin 1
  Wire.beginTransmission(MMA8653_ADDRESS);
  Wire.write(CTRL_REG4);
  Wire.write(CTRL_REG4_INT_EN_DRDY);
  Wire.endTransmission();

  Wire.beginTransmission(MMA8653_ADDRESS);
  Wire.write(CTRL_REG5);
  Wire.write(CTRL_REG5_INT_CFG_DRDY);
  Wire.endTransmission();

  // Put in fast-read mode, with 800Hz output rate, and activate
  Wire.beginTransmission(MMA8653_ADDRESS);
  Wire.write(CTRL_REG1);
  Wire.write(CTRL_REG1_ACTIVE | CTRL_REG1_F_READ | CTRL_REG1_DR(0));
  Wire.endTransmission();
}

bool MMA8653::getXYZ(float& X, float& Y, float& Z) {

    // TODO: We're assuming that we are in 8G, 8 bit mode
    // 1/16*9.8m/s^2
#define factor (.6125)

    Wire.beginTransmission(MMA8653_ADDRESS);
    Wire.write(STATUS);
    Wire.endTransmission(false);

    Wire.requestFrom(MMA8653_ADDRESS, 4);

    if(Wire.available()) {
        Wire.read();
    }
    if(Wire.available()) {
        Y = factor*(int8_t)Wire.read();
    }
    if(Wire.available()) {
        X = factor*(int8_t)Wire.read();
    }
    if(Wire.available()) {
        Z = factor*(int8_t)Wire.read();
    }

    return true;
}


