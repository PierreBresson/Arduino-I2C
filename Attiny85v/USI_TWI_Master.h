#define SYS_CLK   1000.0  

#define T2_TWI    5 		
#define T4_TWI    4 	

#define SIGNAL_VERIFY		

#define TWI_READ_BIT  0       
#define TWI_ADR_BITS  1       
#define TWI_NACK_BIT  0    

#define USI_TWI_NO_DATA             0x08  
#define USI_TWI_DATA_OUT_OF_BOUND   0x09  
#define USI_TWI_UE_START_CON        0x07  
#define USI_TWI_UE_STOP_CON         0x06  
#define USI_TWI_UE_DATA_COL         0x05  
#define USI_TWI_NO_ACK_ON_DATA      0x02  
#define USI_TWI_NO_ACK_ON_ADDRESS   0x01  
#define USI_TWI_MISSING_START_CON   0x03  
#define USI_TWI_MISSING_STOP_CON    0x04  
#define USI_TWI_BAD_MEM_READ	    0x0A  

#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PORTB0
#define PORT_USI_SCL        PORTB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2

#define TRUE  1
#define FALSE 0


void          USI_TWI_Master_Initialise( void );
unsigned char USI_TWI_Start_Random_Read( unsigned char * , unsigned char );
unsigned char USI_TWI_Start_Read_Write( unsigned char * , unsigned char );
unsigned char USI_TWI_Get_State_Info( void );
