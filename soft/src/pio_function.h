/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : pio_function.h
 * Author               : Dorian Kury
 * Date                 : 27.11.2025
 *
 * Context              : ARE lab
 *
 *****************************************************************************************
 * Brief: Header file for pio function
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 0.0    27.07.2022  ACS           Initial version.
 *
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "axi_lw.h"


// ACCESS MACROS from base address 0xFF21_0000
#define BASE_ADD(_x_)   *(volatile uint32_t *)(AXI_LW_HPS_FPGA_ZONE_DISPO + _x_) // _x_ is an offset with respect to the base address
#define AXI_LW_REG(_x_)   *(volatile uint32_t *)(AXI_LW_HPS_FPGA_BASE_ADD + _x_) // _x_ is an offset with respect to the base address


//offset
#define USER_ID     0x00
#define BUTTONS     0x04
#define SWITCHS     0x08
#define LEDS        0x0C
#define STATUS      0x10
#define GEN         0x14
#define NBR_A       0x20
#define NBR_B       0x24
#define NBR_C       0x28
#define NBR_D       0x2C

//fonctions

//number a-b-c-d
uint32_t get_nbr_code(uint32_t nbr);
uint32_t get_nbr_value(uint32_t nbr);

//status functions
uint32_t read_status();
void new_nbr(bool n);
void init_nbr(bool init);

//gen_function
uint32_t get_mode_gen();
uint32_t get_delay_gen();

void write_mode_gen(bool mode);
void write_delay_gen(uint32_t delay);

//USER_ID
uint32_t get_user_id(void);

//buttons
uint32_t get_buttons(void);

//switchs
uint32_t get_switchs(void);

//leds
uint32_t get_leds(void);
void put_leds(uint32_t leds);
void init_leds(void);



//------
// 0xFF200000 à 0xFF3FFFFF

// Base address
#define PIO_CORE0_BASE_ADD         0xFF200100u    // switch, leds et bouttons (KEY)
#define PIO_CORE1_BASE_ADD         0xFF200120u    // afficheurs 7 segement (HEX0, ...)

// ACCESS MACROS
#define PIO0_REG(_x_)   *(volatile uint32_t *)(PIO_CORE0_BASE_ADD + _x_) // _x_ is an offset with respect to the base address
#define PIO1_REG(_x_)   *(volatile uint32_t *)(PIO_CORE1_BASE_ADD + _x_)

// PIO Registers
// Offset pour aller registers du PIO (p.256)
#define PIO_DATA                0x0u
#define PIO_DIRECTION           0x4u

//mapping de PIO0
#define SW_FIRST_BIT     0u
#define SW_LENGTH       10u
#define SW_MASK         ((1u<<SW_LENGTH)-1u)

#define LED_FIRST_BIT   10u
#define LED_LENGTH      10u //car on veut travailler que sur les leds 7-0
#define LED_MASK        ((1u<<LED_LENGTH)-1u)

#define KEY_FIRST_BIT   20u
#define KEY_LENGTH       4u

//mapping de PIO1
#define HEX_LENGTH      7u
#define HEX2_MASK       0x1FC000u
#define HEX3_MASK       0xFE00000u

// table de correspondance entre exa et segments de l'afficheur
   static const uint8_t HEX_LUT[16] = {
        0x40, // 0 : 1000000
        0x79, // 1 : 1111001
        0x24, // 2 : 0100100
        0x30, // 3 : 0110000
        0x19, // 4 : 0011001
        0x12, // 5 : 0010010
        0x02, // 6 : 0000010
        0x78, // 7 : 1111000
        0x00, // 8 : 0000000
        0x10, // 9 : 0010000
        0x08, // A : 0001000
        0x03, // b : 0000011
        0x46, // C : 1000110 
        0x21, // d : 0100001 
        0x06, // E : 0000110
        0x0E  // F : 0001110
    };
    
    


//***************************//
//****** Init function ******//

// Swicths_init function : Initialize all Switchs in PIO core (SW9 to SW0)
void Switchs_init(void);

// Leds_init function : Initialize all Leds in PIO core (LED9 to LED0)
void Leds_init(void);

// Keys_init function : Initialize all Keys in PIO core (KEY3 to KEY0)
void Keys_init(void);

// Segs7_init function : Initialize all 7-segments display in PIO core (HEX3 to HEX0)
void Segs7_init(void);

//***********************************//
//****** Global usage function ******//

// Switchs_read function : Read the switchs value
// Parameter : None
// Return : Value of all Switchs (SW9 to SW0)
uint32_t Switchs_read(void);

// Leds_write function : Write a value to all Leds (LED9 to LED0)
// Parameter : "value"= data to be applied to all Leds
// Return : None
void Leds_write(uint32_t value);

// Leds_set function : Set to ON some or all Leds (LED9 to LED0)
// Parameter : "maskleds"= Leds selected to apply a set (maximum 0x3FF)
// Return : None
void Leds_set(uint32_t maskleds);

// Leds_clear function : Clear to OFF some or all Leds (LED9 to LED0)
// Parameter : "maskleds"= Leds selected to apply a clear (maximum 0x3FF)
// Return : None
void Leds_clear(uint32_t maskleds);

// Leds_toggle function : Toggle the curent value of some or all Leds (LED9 to LED0)
// Parameter : "maskleds"= Leds selected to apply a toggle (maximum 0x3FF)
// Return : None
void Leds_toggle(uint32_t maskleds);

// Key_read function : Read one Key status, pressed or not (KEY0 or KEY1 or KEY2 or KEY3)
// Parameter : "key_number"= select the key number to read, from 0 to 3
// Return : True(1) if key is pressed, and False(0) if key is not pressed
bool Key_read(int key_number);

// Seg7_write function : Write digit segment value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= digit segment value to be applied on the selected 7-segments (maximum 0x7F to switch ON all segments)
// Return : None
void Seg7_write(int seg7_number, uint32_t value);

// Seg7_write_hex function : Write an Hexadecimal value to one 7-segments display (HEX0 or HEX1 or HEX2 or HEX3)
// Parameter : "seg7_number"= select the 7-segments number, from 0 to 3
// Parameter : "value"= Hexadecimal value to be display on the selected 7-segments, form 0x0 to 0xF
// Return : None
void Seg7_write_hex(int seg7_number, uint32_t value);







