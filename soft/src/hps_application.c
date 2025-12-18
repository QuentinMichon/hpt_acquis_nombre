/*****************************************************************************************
 * HEIG-VD
 * Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
 * School of Business and Engineering in Canton de Vaud
 *****************************************************************************************
 * REDS Institute
 * Reconfigurable Embedded Digital Systems
 *****************************************************************************************
 *
 * File                 : hps_application.c
 * Author               : Quentin Michon, Dorian Kury
 * Date                 : 05.12.2025
 *
 * Context              : ARE lab
 *
 *****************************************************************************************
 * Brief: Conception d'une interface évoluée sur le bus Avalon avec la carte DE1-SoC
 *
 *****************************************************************************************
 * Modifications :
 * Ver    Date        Student      Comments
 * 
 *
*****************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "axi_lw.h"

#include "pio_function.h"

int __auto_semihosting;

int err_cum = 0;

bool key_pressed(int key_number);
bool key_holding(int key_number);
const char *read_status_str(uint32_t status);


bool holding = false;

int main(void){
    
    printf("Laboratoire: Conception d'une interface évoluée \n");
 
    uint32_t board_id =  AXI_LW_REG(0x000000);
    printf("Board ID = 0x%08X\n", board_id);
    
    uint32_t interface_id = get_user_id();
    printf("Interface ID = 0x%08X\n", interface_id);
    
    
    //init
    init_leds();
    init_nbr();
    
    while (1){

        //copie des valeur des sw sur les leds en continu...
        uint32_t sw = get_switchs();
        put_leds(sw);

        //selection de la fréquence avec sw98 !
        uint32_t mask = (3 << 8);
        uint32_t delay = (sw & mask) >> 8;
        write_delay_gen(delay);
        
        //mode de généraation avec sw7
        mask = (1 << 7);
        uint32_t mode = (sw & mask) >> 7;
        //printf("mode : %d \n", mode);
        write_mode_gen(mode);

        // set/clear acquisition fiable
        if (sw & (1 << 0)) {
        	set_safe_mode(true);
        } else {
        	set_safe_mode(false);
        }
        
        if (key_pressed(0)){ //réf
            //init les nombres
            init_nbr();
        }
        
        if (key_pressed(1)){ //réf
            //générer un nouveal ensemble de 4 nombres (ssi mode manuel est selectionné)
            if (get_mode_gen() == 0) //mode_gen 0 = manuel !
                new_nbr();
        }
        
        if (key_holding(2)){

        	//printf("reg safe mode : %d\n", read_safe_mode());
        	//printf("status before snap : %d\n", read_status());

        	// demander le maintiens
        	if(read_safe_mode()) {
        		// on prend une photo
        		//printf("take snap\n");
        		take_snap();
        		// on attend que la photo sois prise
        		while(read_status() == 0b10){
        			//printf("status : %d\n", read_status());
        		}
        	}

            //lecture successive des 4 nombres
            uint32_t n0 = get_nbr_value(0);
            uint32_t n1 = get_nbr_value(1);
            uint32_t n2 = get_nbr_value(2);
            uint32_t n3 = get_nbr_value(3);
            
            //vérif : 
            uint32_t sum = n0 + n1 + n2;
            bool equal = (sum == n3);
            
            if (equal){
                
                printf("%-3s : status: %-7s, somme: %-7lu, nbr_a: %-7lu, nbr_b: %-7lu, nbr_c: %-7lu, nbr_d: %-7lu, \n",
                       "OK",
                       read_status_str(read_status()),sum, n0, n1, n2, n3);
            }
            
            else {
                printf("%-3s : status: %-7s, somme: %-7lu, nbr_a: %-7lu, nbr_b: %-7lu, nbr_c: %-7lu, nbr_d: %-7lu, \n",
                    "ER",
                       read_status_str(read_status()),sum,n0, n1, n2, n3);
        
                printf("ER : nombre d'erreur cumulée : %d \n", err_cum);
                ++err_cum;
            }
        }
    }
}

/* -------------------------------------------------- */
// fonction pour "presser un bouton"
bool key_pressed(int key_number) {
    static bool prev_state[4] = {false, false, false, false};
    bool current = Key_read(key_number);
    bool rising = (!prev_state[key_number] && current); //ne returne true que quand current = true et previous est false -> n'arrive que quand on commance à appuyer
    prev_state[key_number] = current;
    return rising;
}

bool key_holding(int key_number) {
    return Key_read(key_number);
}

const char *read_status_str(uint32_t status){
    uint32_t s = status &= 0x3; //garder que les deux derniers bits

    if (s==0)
        return "00";
    else if (status==1)
        return "01";
    else if (status==2)
        return "10";
    else
        return "11";
}
