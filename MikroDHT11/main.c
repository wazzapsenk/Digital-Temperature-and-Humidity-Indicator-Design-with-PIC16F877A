
#include <xc.h>
#include <stdint.h>

#include "supporing_cfile/lcd.h"

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator) -Osilatör Seçim bitleri (HS osilatör)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled) Watchdog -Timer Etkinle?tirme biti (WDT devre d???)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled) Power-up -Timer Etkinle?tirme biti (PWRT etkin)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled) Brown-out -Reset Etkinle?tirme biti (BOR etkin)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming) -Dü?ük Voltaj (Tek Besleme) Devre ?çi Seri Programlama Etkinle?tirme biti (RB3 dijital I / O'dur, MCLR üzerindeki HV programlama için kullan?lmal?d?r)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off) -Veri EEPROM Bellek Kodu Koruma biti (Veri EEPROM kod korumas? kapal?)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control) -Flash Program Belle?i Yazmay? Etkinle?tirme bitleri (Yazma korumas? kapal?; tüm program belle?ine EECON kontrolü ile yaz?labilir)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off) -Flash Program? Bellek Kodu Koruma biti (Kod korumas? kapal?)


/*
 Program Flow related definition - Program Ak???yla ilgili tan?m
 */
#define DHT11_Data_Pin   PORTDbits.RD5
#define DHT11_Data_Pin_Direction  TRISDbits.TRISD5
#define FIRST_LINE 0x80
#define SECOND_LINE 0xC0


#define _XTAL_FREQ 20000000 //20 Mhz

unsigned char Check_bit, Temp_byte_1, Temp_byte_2, RH_byte_1, RH_byte_2;
unsigned char Nem, RH, ozet ;

//Dht11 related definition - Dht11 ile ilgili tan?m

void dht11_init();
void find_response();
char read_dht11();

// System related definitions -Sistemle ilgili tan?mlar

void system_init(void);
void introduction_screen(void);
void clear_screen(void);

void main() { 
 system_init();
 
 while(1){
     __delay_ms(800);
    dht11_init();
    find_response();
    if(Check_bit == 1){
        RH_byte_1 = read_dht11();
        RH_byte_2 = read_dht11();
        Temp_byte_1 = read_dht11();
        Temp_byte_2 = read_dht11();
        ozet = read_dht11();
        if(ozet == ((RH_byte_1+RH_byte_2+Temp_byte_1+Temp_byte_2) & 0XFF)){
            Nem = Temp_byte_1;
            RH = RH_byte_1;                        
            lcd_com (0x80);                   
            lcd_puts("Temp: ");
            //lcd_puts("                ");
            lcd_data(48 + (( Nem / 10) % 10));
            lcd_data(48 + ( Nem % 10));
            lcd_data(0xDF);
            lcd_puts("C   ");
            lcd_com (0xC0);            
            lcd_puts("Nem: ");
             //lcd_puts("                ");
            lcd_data(48 + ((RH / 10) % 10));
            lcd_data(48 + (RH % 10));
            lcd_puts("%  ");
            }
        else{
            lcd_puts("Hata ozetini kontrol et");
        }
    }
    else {
        clear_screen();
        lcd_com (0x80);
        lcd_puts("Error!");
        lcd_com (0xC0);
        lcd_puts("Yanit yok");
     }
    __delay_ms(1000);
 }
}

/*
 * This will initialize the dht22 sensor. - Bu, dht22 sensörünü baslatacaktir.
 */

void dht11_init(){
 DHT11_Data_Pin_Direction= 0; //Configure RD0 as output - RD0'? ç?kt? olarak yap?land?r?n
 DHT11_Data_Pin = 0; //RD0 sends 0 to the sensor - RD0, sensöre 0 gönderir
 __delay_ms(18);
 DHT11_Data_Pin = 1; //RD0 sends 1 to the sensor - RD0 sensöre 1 gönderir
 __delay_us(30);
 DHT11_Data_Pin_Direction = 1; //Configure RD0 as input - RD0'? giri? olarak yap?land?r?n
 }

/*
 * This will find the dht22 sensor is working or not. - Bu, dht22 sensörünün çal???p çal??mad???n? bulacakt?r.
 */

 void find_response(){
 Check_bit = 0;
 __delay_us(40);
 if (DHT11_Data_Pin == 0){
 __delay_us(80);
 if (DHT11_Data_Pin == 1){
    Check_bit = 1;
 }     
 __delay_us(50);}
 }
 
 /*
 This Function is for read dht22. - Bu Fonksiyon, dht22'yi okumak içindir.
 */
 
 char read_dht11(){
 char data, for_count;
 for(for_count = 0; for_count < 8; for_count++){
     while(!DHT11_Data_Pin); 
    __delay_us(30);
    if(DHT11_Data_Pin == 0){
        data&= ~(1<<(7 - for_count)); //Clear bit (7-b) - Net bit 7-b
    }
    else{
        data|= (1 << (7 - for_count)); //Set bit (7-b) - Biti 7-b ayarla
        while(DHT11_Data_Pin);
    } //Wait until PORTD.F0 goes LOW - PORTD.F0  durumu LOW a gelene kadar bekleyin.
    }
 return data;
 }
 
 void system_init(){
     TRISB = 0; // LCD pins set to out. -LCD pimleri d??ar?ya ayarlanm??.
     lcd_init();
     introduction_screen();
     //dht11_init();
 }
 
 /*
 This Function is for Clear screen without command. - Bu ??lev, komut olmadan ekran? temizle içindir.
 */

void clear_screen(void){
    lcd_com(FIRST_LINE);
    lcd_puts("                ");
    lcd_com(SECOND_LINE);
    lcd_puts("                "); 
}

 
 /*
 This Function is for playing introduction. - Bu ??lev, giri? yapmak içindir.
 */

void introduction_screen(void){
    lcd_com(FIRST_LINE);
    lcd_puts("Welcome to");
    lcd_com(SECOND_LINE);
    lcd_puts("circuit Digest");
    __delay_ms(1000);
    __delay_ms(1000);
    clear_screen();
    lcd_com(FIRST_LINE);
    lcd_puts("DHT11 Sensor");
    lcd_com(SECOND_LINE);
    lcd_puts("with PIC16F877A");
    __delay_ms(1000);
    __delay_ms(1000);
}