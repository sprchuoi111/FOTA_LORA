/*
 * LORA_IF.c
 *
 *  Created on: Mar 25, 2024
 *      Author: quang
 */

#include "SX1278_if.h"
#include "SX1278.h"


struct AES_ctx ctx;

static uint8_t AES_CBC_128_Key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
static uint8_t AES_CBC_128_IV[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
uint8_t LORA_IF_GetFragment_Firmware(SX1278_t *module , uint8_t* buffer , uint8_t no , uint8_t ret , uint32_t timeout , uint8_t length){
	ret = SX1278_LoRaEntryRx(module, length, timeout);
		HAL_Delay(100);
		ret = SX1278_LoRaRxPacket(module);
		if ( ret > 0 ) {
			ret = SX1278_read(module, (uint8_t*) buffer, ret);
			if(buffer[0] == ADDR_MASTER && buffer[2] == FL_FRAGMENT_FIRMWARE){
				no = buffer[1];
				return FL_FRAGMENT_FIRMWARE ;
			}
		}
	    return 0;
}
uint8_t LORA_IF_GetData_Frame(SX1278_t *module , uint32_t unicast_address,uint8_t* buffer_resp , uint8_t ret , uint32_t timeout , uint8_t length , uint8_t ACK_resp ){
	uint32 local_u32timeout = 0;
	uint32 address_node_req = 0;
	ret = SX1278_LoRaRxPacket(module);
	if ( ret > 0 ) {
		// Replace Receive Led hear
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		ret = SX1278_read(module, (uint8_t*) buffer_resp, ret);
		AES_init_ctx_iv(&ctx, AES_CBC_128_Key, AES_CBC_128_IV);
		AES_CTR_xcrypt_buffer(&ctx, (uint8_t*) buffer_resp, 16);
		address_node_req = (buffer_resp[0]<<SHIFT_24_BIT)|(buffer_resp[1]<<SHIFT_16_BIT)
									   |(buffer_resp[2]<<SHIFT_8_BIT)|(buffer_resp[3]<<SHIFT_0_BIT);
		if(address_node_req == unicast_address && buffer_resp[4] == ACK_resp)
			return 1;
	}
	else return 0;

}
//LoRa_Return_t LORA_IF_Stransmit_Data_Frame(SX1278_t *module , uint8_t* buffer , uint8_t ret , uint32_t timeout , uint8_t length ){
//    ret = SX1278_LoRaEntryTx(module, length , timeout);
//	ret = SX1278_LoRaTxPacket(module, (uint8_t*) buffer, length, timeout);
//	return LORA_OKE;
//}
LoRa_Return_t LORA_IF_Stransmit_Request(SX1278_t *module , uint8_t *buffer_req , uint8_t* buffer_resp ,
		uint8_t ret ,uint8_t ACK_req , uint8_t ACK_resp ){
	buffer_req[2] = ACK_req;
	AES_init_ctx_iv(&ctx, AES_CBC_128_Key, AES_CBC_128_IV);
	AES_CTR_xcrypt_buffer(&ctx, (uint8_t*) buffer_req, 16);
	//init to TX mode
	ret = SX1278_LoRaEntryTx(module, SIZE_BUFFER_16BYTES, MAX_TIME_OUT);
	ret = SX1278_LoRaTxPacket(module, (uint8_t*) buffer_req, SIZE_BUFFER_16BYTES, MAX_TIME_OUT);
	/*Read the first Frame */
	 if(ret >0){
		//Replace Blink Send hear
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
		return LORA_OKE;
	 }
	 return LORA_TIMEOUT;
}

LoRa_Return_t LORA_IF_Stransmit_Fragment_Firmware(SX1278_t *module ,uint8_t* buffer_flashing_data ){
	uint8_t counter = 0;
	while(1){
	 ret = SX1278_LoRaEntryTx(module, SIZE_BUFFER_80BYTES, MAX_TIME_OUT);
	 ret = SX1278_LoRaTxPacket(module, (uint8_t*) buffer_flashing_data, SIZE_BUFFER_80BYTES, MAX_TIME_OUT);
	 if(ret){
		 HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
		 HAL_Delay(WAIT_PACKET_SEND);
		/*Read the first Frame */
//		switch(local_u8Check_Code = LORA_IF_GetData_Frame(module ,(uint8_t*) buffer_resp , ret , MAX_TIME_OUT , SIZE_BUFFER_16BYTES)){
//			case(MCU_ACKNOWLEDGE_FINISHING):
//				buffer_packet[2] = buffer_resp[2];
		 return LORA_OKE ;
//			case(MCU_IMAGE_CRC_NOT_CORRECT):
//				return LORA_FLASHING_ERROR;
	 }
//		}
	 else{
		 counter++;
		 if(counter == MAX_TRY_REQ){
			 return LORA_ERROR;
		 }
	 }

	}
}

uint8_t LORA_IF_GetData_End_Frame(SX1278_t *module, uint8_t *rxBuffer, uint32_t unicast_addr , uint8_t length, uint32_t timeout){
	uint32 local_u32timeout = 0;
	uint32_t local_u32addrNode_req;
	memset(rxBuffer , 0xff ,112);
	AES_init_ctx_iv(&ctx, AES_CBC_128_Key, AES_CBC_128_IV);
	ret = SX1278_LoRaRxPacket(module);
	if ( ret > 0 ) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
		ret = SX1278_read(module, (uint8_t*) rxBuffer, ret);
		AES_CTR_xcrypt_buffer(&ctx, (uint8_t*) rxBuffer, length);
		//convert buffer to address_node_req
		local_u32addrNode_req = (rxBuffer[0] << SHIFT_24_BIT) |(rxBuffer[1] << SHIFT_16_BIT)
											|(rxBuffer[2] << SHIFT_8_BIT) | (rxBuffer[3] << SHIFT_0_BIT);
		if(local_u32addrNode_req == ADDRESS__MAC_NODE_1 ||local_u32addrNode_req == ADDRESS__MAC_NODE_2 || local_u32addrNode_req == ADDRESS__MAC_NODE_3 )
			//return flag
			return rxBuffer[4];
	}
}
