/*
 * Receive_FW_UART.h
 *
 *  Created on: Apr 27, 2024
 *      Author: quang
 */

#ifndef INC_RECEIVE_FW_UART_H_
#define INC_RECEIVE_FW_UART_H_

/**************************************************************************/
/*                         Private DataTypes                              */
/**************************************************************************/

/**************************************************************************/
/*                         UART Protocol                                  */
/**************************************************************************/
typedef uint8_t ReceiveFWUartType;
#define RX_IDLE 					0x00U
#define RX_ACCEPT_UPDATE 			0x01U
#define RX_RECEIVED_HEADER 			0x02U
#define RX_RECEIVED_PACKET 			0x04U
#define RX_UPDATE_PACKET_PROGRESS 	0x05U
#define RX_DONE_INSTALL_FW			0x06U
#define RX_END_STATE	  			0x0BU
#define RX_DENY_UPDATE 	  			0x0CU

/**************************************************************************/
/*                        LoRa SPI Protocol                               */
/**************************************************************************/
#define LoRa_SYNCONFIG	0x07U
#define LoRa_PROVIDE_FW 0x08U
#define LoRa_SENDFW		0x09U
#define LoRa_ERROR      0x0AU


/**************************************************************************/
/*                         Private Macros                                 */
/**************************************************************************/
/*ESP To GATEWAY NOTIFY NEW FIRMWARE */
#define NEW_UPDATE_REQUEST	  		0x01U

/* ERROR HANDLE MESSEGE GATEWAY TO ESP*/
#define SYSTEM_STATE_UNDEFINE 		0x02U
#define GATEWAY_BUSY		  		0x03U
#define INVALID_REQUEST 	  		0x04U
/* Sequence GATEWAY with ESP*/
#define NEW_UPDATE_REQUEST_ACCEPT 	0x05U
#define NEW_UPDATE_REQUEST_DENY		0x06U
#define ESP_SEND_HEADER_FLAG  		0x07U
#define HEADER_FLAG_RECEIVED  		0x08U
#define HEADER_FLAG_INVALID  		0x09U
#define HEADER_FLAG_FW_INFO			0x0CU
#define GATEWAY_ACCEPT_PACKET 		0x0AU
#define ESP_SEND_NEXT_PACKET  		0x0BU
/*CONFIRM DOWNLOADING DONE*/
#define ESP_DOWNLOAD_DONE    		0x20U
#define PACKET_1024bytes	 		1024U
#define PACKET_112bytes				112U
#define PACKET_64bytes				64U
#define HEADER_CONFIG_SIZE 	 		32U

/**************************************************************************/
/*                         Prototype Funcion                              */
/**************************************************************************/
void ReceiveFWUpdate_Init(void);
void ReceiveFWUpdate_MainFunc(void);
#endif /* INC_RECEIVE_FW_UART_H_ */