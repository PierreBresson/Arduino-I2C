#define MPL115A2_ADDRESS                       (0x60)   

#define MPL115A2_REGISTER_PRESSURE_MSB         (0x00)
#define MPL115A2_REGISTER_PRESSURE_LSB         (0x01)
#define MPL115A2_REGISTER_TEMP_MSB             (0x02)
#define MPL115A2_REGISTER_TEMP_LSB             (0x03)
#define MPL115A2_REGISTER_A0_COEFF_MSB         (0x04)
#define MPL115A2_REGISTER_A0_COEFF_LSB         (0x05)
#define MPL115A2_REGISTER_B1_COEFF_MSB         (0x06)
#define MPL115A2_REGISTER_B1_COEFF_LSB         (0x07)
#define MPL115A2_REGISTER_B2_COEFF_MSB         (0x08)
#define MPL115A2_REGISTER_B2_COEFF_LSB         (0x09)
#define MPL115A2_REGISTER_C12_COEFF_MSB        (0x0A)
#define MPL115A2_REGISTER_C12_COEFF_LSB        (0x0B)
#define MPL115A2_REGISTER_STARTCONVERSION      (0x12)

void Adafruit_MPL115A2_init();
void Adafruit_MPL115A2_begin(void);
float Adafruit_MPL115A2_getPressure(void);
float Adafruit_MPL115A2_getTemperature(void);
void Adafruit_MPL115A2_getPT(float *P, float *T);


float _mpl115a2_a0 ;
float _mpl115a2_b1 ;
float _mpl115a2_b2 ;
float _mpl115a2_c12;

void Adafruit_MPL115A2_readCoefficients(void);

