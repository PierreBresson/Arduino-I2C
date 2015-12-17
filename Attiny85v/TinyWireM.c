#include "USI_TWI_Master.h"
#include "TinyWireM.h"

#include <stdbool.h>



uint8_t USI_Buf[USI_BUF_SIZE];             
uint8_t USI_BufIdx = 0;                    
uint8_t USI_LastRead = 0;                  
uint8_t USI_BytesAvail = 0;               




void TinyWireM_begin(){
  USI_TWI_Master_Initialise();          
}



void TinyWireM_beginTransmission(uint8_t slaveAddr){
  USI_BufIdx = 0; 
  USI_Buf[USI_BufIdx] = (slaveAddr<<TWI_ADR_BITS) | USI_SEND; 
}



void TinyWireM_send(uint8_t data){
  if (USI_BufIdx >= USI_BUF_SIZE) return;        
  USI_BufIdx++;                                   
  USI_Buf[USI_BufIdx] = data;
}



uint8_t TinyWireM_endTransmission(){ 
  bool xferOK = false;
  uint8_t errorCode = 0;
  xferOK = USI_TWI_Start_Read_Write(USI_Buf,USI_BufIdx+1); 
  USI_BufIdx = 0;
  if (xferOK) {
    return 0;
  } else {                                  
    errorCode = USI_TWI_Get_State_Info();
    return errorCode;
  }
}



uint8_t TinyWireM_requestFrom(uint8_t slaveAddr, uint8_t numBytes){ 
  bool xferOK = false;
  uint8_t errorCode = 0;
  USI_LastRead = 0;
  USI_BytesAvail = numBytes; 
  numBytes++;                
  USI_Buf[0] = (slaveAddr<<TWI_ADR_BITS) | USI_RCVE;   
  xferOK = USI_TWI_Start_Read_Write(USI_Buf,numBytes); 

  if (xferOK){
   return 0;
  } else {                                  
    errorCode = USI_TWI_Get_State_Info(); 
    return errorCode;
  }
}



uint8_t TinyWireM_receive(){
  USI_LastRead++;    
  return USI_Buf[USI_LastRead];
}



uint8_t TinyWireM_available(){
  return USI_BytesAvail - (USI_LastRead); 
}

