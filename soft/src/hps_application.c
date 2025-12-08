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


int main(void){
    
    printf("Laboratoire: Conception d'une interface évoluée \n");
 
    uint32_t board_id =  AXI_LW_REG(0x000000);
    printf("Board ID = 0x%08X\n", board_id);
    
    uint32_t interface_id = get_user_id();
    printf("Interface ID = 0x%08X\n", interface_id);
    
    
    //init
    init_leds();

    
    while (1){
        //copie des valeur des sw sur les leds en continu...
        uint32_t sw = get_switchs();
        put_leds(sw);

        //selection de la fréquence avec sw98 !
        uint32_t mask = (3 << 8);
        write_delay_gen(sw & mask);
        
        //mode de généraation avec sw7
        mask = (1 << 7);
        write_mode_gen(mask & sw);
        
        //acquisition fiable avec sw0 (partie 2) 
        //TODO
        
        if (key_pressed(0)){ //réf
           //init les nombres
            init_nbr(true);
        }
        
        if (key_pressed(1)){ //réf
            //générer un nouveal ensemble de 4 nombres (ssi mode manuel est selectionné)
            if (get_mode_gen() == 1)
                new_nbr(true);
        }
        
        if (key_pressed(2)){ //TODO : key2 actif... //réf
            //lecture successive des 4 nombres
            //vérif : 
            uint32_t sum = get_nbr_value(0) + get_nbr_value(1) + get_nbr_value(1);
            bool equal = (sum == get_nbr_value(3));
            
            if (equal){
                printf("OK : status: %d, nbr_a: %d, nbr_b: %d, nbr_c: %d, nbr_d: %d, \n",
                       read_status(), get_nbr_value(0), get_nbr_value(1), get_nbr_value(2), get_nbr_value(3)); //TODO : vérifier sous quelle forme afficher status 
            }
            
            else {
                printf("ER : status: %d, nbr_a: %d, nbr_b: %d, nbr_c: %d, nbr_d: %d, \n",
                     read_status(), get_nbr_value(0), get_nbr_value(1), get_nbr_value(2), get_nbr_value(3)); //TODO : vérifier sous quelle forme afficher status
        
                printf("ER : nombre d'erreur cumulée : %d \n", err_cum);
                ++err_cum;
            }
        }
    }
}

// fonction pour "presser un bouton"
bool key_pressed(int key_number) {
    static bool prev_state[4] = {false, false, false, false};
    bool current = Key_read(key_number);
    bool rising = (!prev_state[key_number] && current); //ne returne true que quand current = true et previous est false -> n'arrive que quand on commance à appuyer
    prev_state[key_number] = current;
    return rising;
}
