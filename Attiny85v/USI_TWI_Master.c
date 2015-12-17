#define F_CPU 8000000UL  

#include "USI_TWI_Master.h"

#include <avr/interrupt.h>     
#include <util/delay.h>
#include <avr/io.h>
#include <avr/iotn85.h>


unsigned char USI_TWI_Start_Transceiver_With_Data( unsigned char * , unsigned char );
unsigned char USI_TWI_Master_Transfer( unsigned char );
unsigned char USI_TWI_Master_Stop( void );
unsigned char USI_TWI_Master_Start( void );



union  USI_TWI_state
{
  unsigned char errorState;        
  struct
  {
    unsigned char addressMode         : 1;
    unsigned char masterWriteDataMode : 1;
	unsigned char memReadMode		  : 1;
    unsigned char unused              : 5;
  }; 
}   USI_TWI_state;



void USI_TWI_Master_Initialise( void )
{
  PORT_USI |= (1<<PIN_USI_SDA);           
  PORT_USI |= (1<<PIN_USI_SCL);           
  
  DDR_USI  |= (1<<PIN_USI_SCL);           
  DDR_USI  |= (1<<PIN_USI_SDA);           
  
  USIDR    =  0xFF;                       
  USICR    =  (0<<USISIE)|(0<<USIOIE)|                            
              (1<<USIWM1)|(0<<USIWM0)|                            
              (1<<USICS1)|(0<<USICS0)|(1<<USICLK)|                
              (0<<USITC);
  USISR   =   (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      
              (0x0<<USICNT0);                                     
}



unsigned char USI_TWI_Get_State_Info( void )
{
  return ( USI_TWI_state.errorState );                            
}



unsigned char USI_TWI_Start_Random_Read( unsigned char *msg, unsigned char msgSize)
{
  *(msg) &= ~(TRUE<<TWI_READ_BIT);	
  USI_TWI_state.errorState = 0;
  USI_TWI_state.memReadMode = TRUE;
  
  return (USI_TWI_Start_Transceiver_With_Data( msg, msgSize));
}



unsigned char USI_TWI_Start_Read_Write( unsigned char *msg, unsigned char msgSize)
{
	USI_TWI_state.errorState = 0;				
  
	return (USI_TWI_Start_Transceiver_With_Data( msg, msgSize));
}



unsigned char USI_TWI_Start_Transceiver_With_Data( unsigned char *msg, unsigned char msgSize)
{
  unsigned char const tempUSISR_8bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      
                                 (0x0<<USICNT0);                                    
  unsigned char const tempUSISR_1bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|      
                                 (0xE<<USICNT0); 									

	unsigned char *savedMsg;
	unsigned char savedMsgSize; 

  USI_TWI_state.addressMode = TRUE;			

#ifdef PARAM_VERIFICATION
  if(msg > (unsigned char*)RAMEND)
  {
    USI_TWI_state.errorState = USI_TWI_DATA_OUT_OF_BOUND;
    return (FALSE);
  }
  if(msgSize <= 1)                                 
  {
    USI_TWI_state.errorState = USI_TWI_NO_DATA;
    return (FALSE);
  }
#endif

#ifdef NOISE_TESTING                               
  if( USISR & (1<<USISIF) )
  {
    USI_TWI_state.errorState = USI_TWI_UE_START_CON;
    return (FALSE);
  }
  if( USISR & (1<<USIPF) )
  {
    USI_TWI_state.errorState = USI_TWI_UE_STOP_CON;
    return (FALSE);
  }
  if( USISR & (1<<USIDC) )
  {
    USI_TWI_state.errorState = USI_TWI_UE_DATA_COL;
    return (FALSE);
  }
#endif

  if ( !(*msg & (1<<TWI_READ_BIT)) )                
  {
    USI_TWI_state.masterWriteDataMode = TRUE;
  }

	savedMsg = msg;
	savedMsgSize = msgSize;

	if ( !USI_TWI_Master_Start( ))
  {
	return (FALSE);                           
  }

  do
  {
    if (USI_TWI_state.addressMode || USI_TWI_state.masterWriteDataMode)
    {
      PORT_USI &= ~(1<<PIN_USI_SCL);                
      USIDR     = *(msg++);                       
      USI_TWI_Master_Transfer( tempUSISR_8bit );    

      DDR_USI  &= ~(1<<PIN_USI_SDA);               
      if( USI_TWI_Master_Transfer( tempUSISR_1bit ) & (1<<TWI_NACK_BIT) ) 
      {
        if ( USI_TWI_state.addressMode )
          USI_TWI_state.errorState = USI_TWI_NO_ACK_ON_ADDRESS;
        else
          USI_TWI_state.errorState = USI_TWI_NO_ACK_ON_DATA;
        return (FALSE);
      }
	  
	  if ((!USI_TWI_state.addressMode) && USI_TWI_state.memReadMode)
	  {
		msg = savedMsg;					
		*(msg) |= (TRUE<<TWI_READ_BIT);  
		USI_TWI_state.errorState = 0;
		USI_TWI_state.addressMode = TRUE;	
		msgSize = savedMsgSize;				

		if ( !USI_TWI_Master_Start( ))
		{
			USI_TWI_state.errorState = USI_TWI_BAD_MEM_READ;
			return (FALSE);                           
		}
	  }
	  else
	  {
		USI_TWI_state.addressMode = FALSE;            
	  }
    }

    else
    {
      DDR_USI   &= ~(1<<PIN_USI_SDA);               
      *(msg++)  = USI_TWI_Master_Transfer( tempUSISR_8bit );

      if( msgSize == 1)                           
      {
        USIDR = 0xFF;                             
      }
      else
      {
        USIDR = 0x00;                             
      }
      USI_TWI_Master_Transfer( tempUSISR_1bit );   
    }
  }while( --msgSize);                             
  
  if (!USI_TWI_Master_Stop())
  {
	return (FALSE);                           
	}

  return (TRUE);
}



unsigned char USI_TWI_Master_Transfer( unsigned char temp )
{
  USISR = temp;                                     
                                                    
  temp  =  (0<<USISIE)|(0<<USIOIE)|                 
           (1<<USIWM1)|(0<<USIWM0)|                
           (1<<USICS1)|(0<<USICS0)|(1<<USICLK)|    
           (1<<USITC);                              
  do
  { 
	_delay_us(T2_TWI);
    USICR = temp;                          
    while( !(PIN_USI & (1<<PIN_USI_SCL)) );
	_delay_us(T4_TWI);
    USICR = temp;                          
  }while( !(USISR & (1<<USIOIF)) );        
  
	_delay_us(T2_TWI);
  temp  = USIDR;                           
  USIDR = 0xFF;                            
  DDR_USI |= (1<<PIN_USI_SDA);             

  return temp;                             
}



unsigned char USI_TWI_Master_Start( void )
{
  PORT_USI |= (1<<PIN_USI_SCL);                     
  while( !(PORT_USI & (1<<PIN_USI_SCL)) );        
  _delay_us(T2_TWI);

  PORT_USI &= ~(1<<PIN_USI_SDA);                   
	_delay_us(T4_TWI);                         
  PORT_USI &= ~(1<<PIN_USI_SCL);                    
  PORT_USI |= (1<<PIN_USI_SDA);                     

#ifdef SIGNAL_VERIFY
  if( !(USISR & (1<<USISIF)) )
  {
    USI_TWI_state.errorState = USI_TWI_MISSING_START_CON;  
    return (FALSE);
  }
#endif
  return (TRUE);
}



unsigned char USI_TWI_Master_Stop( void )
{
  PORT_USI &= ~(1<<PIN_USI_SDA);           
  PORT_USI |= (1<<PIN_USI_SCL);            
  while( !(PIN_USI & (1<<PIN_USI_SCL)) );  
	_delay_us(T4_TWI);
  PORT_USI |= (1<<PIN_USI_SDA);           
	_delay_us(T2_TWI);
  
#ifdef SIGNAL_VERIFY
  if( !(USISR & (1<<USIPF)) )
  {
    USI_TWI_state.errorState = USI_TWI_MISSING_STOP_CON;    
    return (FALSE);
  }
#endif

  return (TRUE);
}
