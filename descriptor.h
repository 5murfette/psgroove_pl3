/*
    PSGroove Exploit port for PL3 by 5murf

    *This code uses parts of PSFreedom code by Youness Alaoui (KaKaRoTo)*

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "pl3/shellcode_egghunt.h"

#define MAGIC_NUMBER		0x50, 0x53, 0x46, 0x72, 0x65, 0x65, 0x64, 0x6d

#if defined (FIRMWARE_3_41)
#include "pl3/default_payload_3_41.h"
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xe7, 0x20
#elif defined (FIRMWARE_3_15)
#include "pl3/default_payload_3_15.h"
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x33, 0xda, 0x10
#elif defined (FIRMWARE_3_01)
#include "pl3/default_payload_3_01.h"
#define RTOC_TABLE		0x80, 0x00, 0x00, 0x00, 0x00, 0x32, 0x06, 0x40
#else
#error "You must specify the target firmware." \
  " define a supported FIRMWARE_X_YZ in config.h and recompile."
#endif /* FIRMWARE_X_YZ */


#ifdef USE_JIG
#define default_shellcode shellcode_egghunt

#if defined (FIRMWARE_3_41)
#define default_payload default_payload_3_41
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xee
#define SHELLCODE_ADDR_LOW	0x70
#elif defined (FIRMWARE_3_15)
#define default_payload default_payload_3_15
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xde
#define SHELLCODE_ADDR_LOW	0x30
#elif defined (FIRMWARE_3_01)
#define default_payload default_payload_3_01
#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xFB
#define SHELLCODE_ADDR_LOW	0xC8
#endif /* FIRMWARE_X_YZ */

#define SHELLCODE_PAGE		0x80, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00
#define SHELLCODE_DESTINATION	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW
#define SHELLCODE_PTR 		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x08
#define SHELLCODE_ADDRESS	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x18

#define PORT1_NUM_CONFIGS	4

#else /* USE_JIG */

#include "pl3/dump_lv2.h"

#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x4D, 0x00
//#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x5B, 0x00
//#define SHELLCODE_ADDR_HIGH	0x80, 0x00, 0x00, 0x00, 0x00, 0x45, 0x00
#define SHELLCODE_ADDR_LOW	0x00

#define SHELLCODE_PAGE		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW
#define SHELLCODE_DESTINATION	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x20
#define SHELLCODE_PTR 		SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x28
#define SHELLCODE_ADDRESS	SHELLCODE_ADDR_HIGH, SHELLCODE_ADDR_LOW + 0x38

#define PORT1_NUM_CONFIGS	100

#endif /* USE_JIG */


const uint8_t PROGMEM jig_response[64] = {
  SHELLCODE_PTR,
  SHELLCODE_ADDRESS,
  RTOC_TABLE,
  SHELLCODE
};


const uint8_t PROGMEM HUB_Device_Descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x09, 0x00, 0x01, 0x08,
	0xAA, 0xAA, 0xCC, 0xCC, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM HUB_Config_Descriptor[] = {
	// Config
	0x09, 0x02, 0x19, 0x00, 0x01, 0x01, 0x00, 0xe0,
	0x32,
	// Interface
	0x09, 0x04, 0x00, 0x00, 0x01, 0x09, 0x00, 0x00,
	0x00,
	// Endpoint (interrupt in)
	0x07, 0x05, 0x81, 0x03, 0x01, 0x00, 0x0c,
};

const uint8_t PROGMEM HUB_Hub_Descriptor[] = {
	0x09, 0x29, 0x06, 0xa9, 0x00, 0x32, 0x64, 0x00,
	0xff,
};

const uint8_t PROGMEM port1_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x00, PORT1_NUM_CONFIGS
};

const uint8_t PROGMEM port1_short_config_descriptor[] = {
	0x09, 0x02, 0x00, 0x0f, 0x01, 0x00, 0x00, 0x80,
};

const uint8_t PROGMEM port1_config_descriptor[] = {
	0x09, 0x02, 0x12, 0x00, 0x01, 0x00, 0x00, 0x80, 0xFA, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xFE, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	MAGIC_NUMBER,
#ifndef USE_JIG
	SHELLCODE_PTR,
	SHELLCODE_ADDRESS,
	RTOC_TABLE,
#endif
	PAYLOAD
} ;



const uint8_t PROGMEM port2_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM port2_config_descriptor[] = {
	// config
	0x09, 0x02, 0x16, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x02,
	0x00,
	// extra
	0x04, 0x21, 0xb4, 0x2f,
};

const uint8_t PROGMEM port3_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x02,
};

const uint8_t PROGMEM port3_config_descriptor[] = {
	0x09, 0x02, 0x4d, 0x0a, 0x01, 0x01, 0x00, 0x80, 0x01, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04,
	0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01,
	0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00,
	0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09,
	0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe,
	0x00, 0xfe, 0x01, 0x02, 0x00, 0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
};


const uint8_t PROGMEM port4_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xAA, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x03,
};

const uint8_t PROGMEM port4_config_descriptor_1[] = {
	// config
	0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00,
};

const uint8_t PROGMEM port4_short_config_descriptor_2[] = {
	// config
	0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
};

const uint8_t PROGMEM port4_config_descriptor_2[] = {
	// config
	0x09, 0x02, 0x00, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00,
};

const uint8_t PROGMEM port4_config_descriptor_3[] = {
	0x09, 0x02, 0x30, 0x00, 0x01, 0x01, 0x00, 0x80, 0x01,
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02, 0x00,
	0x3e, 0x21, 0x00, 0x00, 0x00, 0x00, 
	MAGIC_NUMBER, /* magic number to look for in the start of the page */
	SHELLCODE_PAGE, /* Initial data search ptr */
	SHELLCODE_DESTINATION, /* destination ptr for heap structure (jig response) */
};

const uint8_t PROGMEM port5_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0x4c, 0x05, 0xeb, 0x02, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM port5_config_descriptor[] = {
	// config
	0x09, 0x02, 0x20, 0x00, 0x01, 0x00, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x02, 0xff, 0x00, 0x00,
	0x00,
	// endpoint
	0x07, 0x05, 0x02, 0x02, 0x08, 0x00, 0x00,
	// endpoint
	0x07, 0x05, 0x81, 0x02, 0x08, 0x00, 0x00,
};

const uint8_t PROGMEM port6_device_descriptor[] = {
	0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08,
	0xaa, 0xaa, 0xc0, 0xde, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01,
};

const uint8_t PROGMEM port6_config_descriptor[] = {
	// config
	0x09, 0x02, 0x12, 0x00, 0x01, 0x01, 0x00, 0x80,
	0x01,
	// interface
	0x09, 0x04, 0x00, 0x00, 0x00, 0xfe, 0x01, 0x02,
	0x00,
};
