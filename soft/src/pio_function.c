/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : pio_function.c
 * Author               : Dorian Kury | Quentin Michon
 * Date                 : 
 *
 * Context              : ARE lab
 *
 *****************************************************************************************
 * Brief: Pio function
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 
 *
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "pio_function.h"

//equation
//number a-b-c-d

//internal function
uint32_t get_nbr_x(int nbr){
    if (nbr > 3) {
        printf("ERROR : get_nbr_code() and get_nbr_value -> wrong arg \n");
    }
    
    if (nbr == 0){
            return BASE_ADD(NBR_A);
    } else if(nbr == 1){
        return BASE_ADD(NBR_B);
    } else if (nbr == 2) {
        return BASE_ADD (NBR_C);
    } else if (nbr==3) {
        return BASE_ADD(NBR_D);
    }
}

uint32_t get_nbr_code(uint32_t nbr){
    
    volatile uint32_t mask = (3 << 22);
    volatile uint32_t number32 = get_nbr_x(nbr);
    volatile uint32_t code = (number32 & mask) >> 22;

   
    //printf("nbr_code : %lu \n", code);

    return code;
}

uint32_t get_nbr_value(uint32_t nbr){
    
    uint32_t mask = (1u << 22) - 1u; //garder que les bits 0 à 21
    uint32_t number32 = get_nbr_x(nbr);
    
    uint32_t number = number32 & mask;

    //printf("nbr_value : %lu \n", number);
    
    return number;
}

//status functions
uint32_t read_status(){
    uint32_t mask = (0b11);
    return BASE_ADD(STATUS) & mask;
}
    
void new_nbr(){
	BASE_ADD(STATUS) = (1 << 4);
}
    
void init_nbr(){
    BASE_ADD(STATUS) = (1 << 0);
}

//gen_function
uint32_t get_mode_gen(){
    uint32_t mask = (1 << 4);
    uint32_t gen = BASE_ADD(GEN);
    
    uint32_t mode = (gen & mask) >> 4;
    
    
    return (mode);
}

uint32_t get_delay_gen(){
    uint32_t mask = (3);
    uint32_t gen = BASE_ADD(GEN);
    
    return (mask & gen);
}

void write_mode_gen(uint32_t mode){ //ici
    uint32_t mask = (1 << 4);
    uint32_t gen = BASE_ADD(GEN);
       
    if (mode) 
        gen |= mask;
    else
        gen &= ~mask;
    
    BASE_ADD(GEN) = gen;
    
}

void write_delay_gen(uint32_t delay){ //ici
    uint32_t gen = BASE_ADD(GEN);
    
    gen &= ~0x3;
    gen |= (delay & 0x3);
    
    BASE_ADD(GEN) = gen;
}

uint32_t get_user_id(){
    return BASE_ADD(USER_ID);
}    

uint32_t get_buttons(){
    uint32_t mask = (0b1111);
    return (BASE_ADD(BUTTONS) & mask); 
}

uint32_t get_switchs(){
    uint32_t mask = (1u << 10) - 1u;
    return (BASE_ADD(SWITCHS) & mask);
}

uint32_t get_leds(){
    uint32_t mask = (1u << 10) - 1u;
    return (BASE_ADD(LEDS) & mask);
}

void put_leds(uint32_t leds){
    BASE_ADD(LEDS) = leds;
}

void init_leds(){
    BASE_ADD(LEDS) = 0;
}










//-----

// --fonctions internes--
//insérer des valeurs dans un register
void insert_value(volatile uint32_t *addr, uint32_t start_bit, uint32_t length, uint32_t value) {
        uint32_t mask = (length >= 32) ? 0xFFFFFFFFu : ((1u << length) - 1u); // mask de 1 eg length = 3 -> 0b0000 0111 ; -1 permet de passe de 1000 à 0111
        mask <<= start_bit; // le décaller eg start_bit = 3 -> 0b0000 0000 0011 1000
        *addr = (*addr & ~mask) | ((value << start_bit) & mask); // efface les bits, puis insère la valeur du mask
}

//lire des valeurs d'un restistre
uint32_t read_value(volatile uint32_t *addr, uint32_t start_bit, uint32_t length) {
    uint32_t mask = (length >= 32) ? 0xFFFFFFFFu : ((1u << length) - 1u); // mask de 1 eg length = 3 -> 0b0000 0111
    
    return (*addr >> start_bit) & mask;
}

// --INIT--

void Switchs_init(void) {
   insert_value(&PIO0_REG(PIO_DIRECTION), SW_FIRST_BIT, SW_LENGTH, 0x0);
}
    
void Leds_init(void) {
    insert_value(&PIO0_REG(PIO_DIRECTION), LED_FIRST_BIT, LED_LENGTH, 0x3FF);
    insert_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH, 0x0);
}
        
    
void Keys_init(void) {
    insert_value(&PIO0_REG(PIO_DIRECTION), KEY_FIRST_BIT, KEY_LENGTH, 0x0);
}

void Segs7_init(void) {
    insert_value(&PIO1_REG(PIO_DIRECTION), 0, 32, 0xFFFFFFFF),
        
    //hex0 et hex1 en off. (inverstion des polarités)
    Seg7_write_hex(0, 0x0); //7F == tout les bits à 1 sur les 7 bits
    Seg7_write_hex(1, 0x0);
    
    //hex2 : segment du haut allumé
    Seg7_write_hex(2, 0x0); //7F pour que sout soit étteinds, -1 pour enelver b0;
    
    //hex3 : afficher un zéro
    Seg7_write_hex(3, 0x0);
}


//--IO--


uint32_t Switchs_read(void){
    return read_value(&PIO0_REG(PIO_DATA), SW_FIRST_BIT, SW_LENGTH);
}

void Leds_write(uint32_t value){   
    insert_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH, value);
}

void Leds_set(uint32_t maskleds) {
    uint32_t actual_leds = read_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH);
    uint32_t new_leds = actual_leds | maskleds;
    insert_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH, new_leds);
}
    

void Leds_clear(uint32_t maskleds) {
    uint32_t actual_leds = read_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH);
    uint32_t new_leds = actual_leds & ~maskleds;
    insert_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH, new_leds);
}

void Leds_toggle(uint32_t maskleds){
    uint32_t actual_leds = read_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH);
    uint32_t new_leds = actual_leds ^ (maskleds & LED_MASK); //toggle d'un uniquement
    insert_value(&PIO0_REG(PIO_DATA), LED_FIRST_BIT, LED_LENGTH, new_leds);
}

    //correction pour labo
bool Key_read(int key_number){
    if (key_number < 0 || key_number >= (int)KEY_LENGTH) return false; //sécurité
        
    uint32_t buttons = get_buttons();
    uint32_t mask = 1 << key_number;
    uint32_t bit = buttons & mask;
    return bit ? false : true; 
}


    

void Seg7_write(int seg7_number, uint32_t value){
    if (seg7_number < 0 || seg7_number > 3) return; //sécurité
    if (value > 0x7F) return; //sécurité
    
    insert_value(&PIO1_REG(PIO_DATA), seg7_number*HEX_LENGTH, HEX_LENGTH, value);
}


void Seg7_write_hex(int seg7_number, uint32_t value){
    if (seg7_number < 0 || seg7_number > 3) return; // sécurité
    if (value > 0xF) return; //sécurité

    
    uint32_t pattern = HEX_LUT[value];
    Seg7_write(seg7_number, pattern);
}




        

    
    
    

