------------------------------------------------------------------------------------------
-- HEIG-VD ///////////////////////////////////////////////////////////////////////////////
-- Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
-- School of Business and Engineering in Canton de Vaud
------------------------------------------------------------------------------------------
-- REDS Institute ////////////////////////////////////////////////////////////////////////
-- Reconfigurable Embedded Digital Systems
------------------------------------------------------------------------------------------
--
-- File                 : avl_user_interface.vhd
-- Author               : 
-- Date                 : 04.08.2022
--
-- Context              : Avalon user interface
--
------------------------------------------------------------------------------------------
-- Description : 
--   
------------------------------------------------------------------------------------------
-- Dependencies : 
--   
------------------------------------------------------------------------------------------
-- Modifications :
-- Ver    Date        Engineer    Comments
-- 0.0    See header              Initial version

------------------------------------------------------------------------------------------

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
    
entity avl_user_interface is
  port(
    -- Avalon bus
    avl_clk_i           : in  std_logic;
    avl_reset_i         : in  std_logic;
    avl_address_i       : in  std_logic_vector(13 downto 0);
    avl_byteenable_i    : in  std_logic_vector(3 downto 0);
    avl_write_i         : in  std_logic;
    avl_writedata_i     : in  std_logic_vector(31 downto 0);
    avl_read_i          : in  std_logic;
    avl_readdatavalid_o : out std_logic;
    avl_readdata_o      : out std_logic_vector(31 downto 0);
    avl_waitrequest_o   : out std_logic;
    -- User interface
    button_i            : in  std_logic_vector(3 downto 0);
    switch_i            : in  std_logic_vector(9 downto 0);
    led_o               : out std_logic_vector(9 downto 0);
    -- Gen nombres
    nbr_a_i             : in  std_logic_vector(21 downto 0);
    nbr_b_i             : in  std_logic_vector(21 downto 0);
    nbr_c_i             : in  std_logic_vector(21 downto 0);
    nbr_d_i             : in  std_logic_vector(21 downto 0);
    cmd_init_o          : out std_logic;
    cmd_new_nbr_o       : out std_logic;
    auto_o              : out std_logic;
    delay_o             : out std_logic_vector(1 downto 0)
  );
end avl_user_interface;

architecture rtl of avl_user_interface is

    --| Components declaration |--------------------------------------------------------------
    
    --| Constants declarations |--------------------------------------------------------------
    
    --| Signals declarations   |--------------------------------------------------------------
	 -- BLOC READ
	 signal reg_readdata_s : std_logic_vector(31 downto 0);
	 signal cmd_readdatavalid_s : std_logic;
	 -- BLOC WRITE
	 signal reg_leds_soc_s : std_logic_vector(9 downto 0);
	 signal reg_status_s : std_logic_vector(1 downto 0);
	 signal reg_mode_gen : std_logic;
	 signal reg_delay_gen : std_logic_vector(1 downto 0);
	 signal cmd_new_nbr : std_logic;
	 signal cmd_init_nbr : std_logic;
	 signal reg_safe_s   : std_logic;  -- part 2 | used to set safe mode enable or not
	 signal cmd_take_snap : std_logic; -- part 2 | used to take a picture of a,b,c,d
	 --  BLOC safe
	 signal reg_nbr_a_s : std_logic_vector(21 downto 0);
	 signal reg_nbr_b_s : std_logic_vector(21 downto 0);
	 signal reg_nbr_c_s : std_logic_vector(21 downto 0);
	 signal reg_nbr_d_s : std_logic_vector(21 downto 0);
    signal we_eq_s     : std_logic;
	 signal flg_eq_en_s : std_logic;
	 -- MSS
	 signal mss_pres_s : std_logic_vector(1 downto 0);
	 signal mss_fut_s  : std_logic_vector(1 downto 0);
	 constant E_WAIT_SNAP_REQ : std_logic_vector(1 downto 0) := "00";
	 constant E_SAVE          : std_logic_vector(1 downto 0) := "01";
	 constant E_EQ_READABLE   : std_logic_vector(1 downto 0) := "10";
	 
	 
begin
    -- Output zone
	 avl_readdatavalid_o <= cmd_readdatavalid_s;
	 avl_readdata_o      <= reg_readdata_s;
	 
	 led_o <= reg_leds_soc_s;
	 
	 cmd_init_o <= cmd_init_nbr;
	 cmd_new_nbr_o <= cmd_new_nbr;
	 auto_o <= reg_mode_gen;
	 delay_o <= reg_delay_gen;
	 
	 reg_status_s <= '1' & flg_eq_en_s; -- part 2
	 
    -- Read access part
    read_access_p : process(avl_reset_i, avl_clk_i)
	 begin
	     if avl_reset_i = '1' then
		      -- reset des registres
		      reg_readdata_s <= (others => '0');
 		      cmd_readdatavalid_s <= '0';
		  elsif rising_edge(avl_clk_i) then 
		      cmd_readdatavalid_s <= '0'; -- fin de la pulse d'un coup de clock
				if avl_read_i = '1' then
				    case (to_integer(unsigned(avl_address_i))) is
					     when 0 =>
						      reg_readdata_s <= x"CAFEDECA";
								cmd_readdatavalid_s <= '1';
						  when 1 =>
						      reg_readdata_s <= (31 downto 4 => '0') & button_i;
								cmd_readdatavalid_s <= '1';
						  when 2 =>
						      reg_readdata_s <= (31 downto 10 => '0') & switch_i;
								cmd_readdatavalid_s <= '1'; 
						  when 3 =>
						      reg_readdata_s <= (31 downto 10 => '0') & reg_leds_soc_s;
								cmd_readdatavalid_s <= '1';
						  when 4 =>
						      reg_readdata_s <= (31 downto 2 => '0') & reg_status_s;
								cmd_readdatavalid_s <= '1';
						  when 5 =>
						      reg_readdata_s <= (31 downto 5 => '0') & reg_mode_gen & (3 downto 2 => '0') & reg_delay_gen; 
								cmd_readdatavalid_s <= '1';
						  when 6 =>
								reg_readdata_s <= (31 downto 1 => '0') & reg_safe_s;
								cmd_readdatavalid_s <= '1';
						  --when 7 =>
						      -- AVAILABLE FOR NEW FUNCTIONALITY
						  when 8 =>
								reg_readdata_s <= (31 downto 24 => '0') & "00" & reg_nbr_a_s;
								cmd_readdatavalid_s <= '1';
						  when 9 =>
								reg_readdata_s <= (31 downto 24 => '0') & "01" & reg_nbr_b_s;
								cmd_readdatavalid_s <= '1';
						  when 10 =>
								reg_readdata_s <= (31 downto 24 => '0') & "10" & reg_nbr_c_s;
								cmd_readdatavalid_s <= '1';
						  when 11 =>
								reg_readdata_s <= (31 downto 24 => '0') & "11" & reg_nbr_d_s;
								cmd_readdatavalid_s <= '1';
						  when others =>
						      -- RESERVED TO 15
						      -- THEN NOT USED
								if unsigned(avl_address_i) > 15 then 
								    reg_readdata_s <= X"AAAABBBB"; -- 1010'1010'1010'1010'0101'0101'0101'0101
									 cmd_readdatavalid_s <= '1'; -- permet de lire une constante qui indique RESERVED ou NOT USED
								else
									 reg_readdata_s <= X"3333CCCC"; -- 0011'0011'0011'0011'1100'1100'1100'1100
									 cmd_readdatavalid_s <= '1'; -- permet de lire une constante qui indique RESERVED ou NOT USED
								end if;
				    end case;
            end if;
        end if;
    end process;
	 
    -- Write access part
    write_access_p : process(avl_reset_i, avl_clk_i)
	 begin
	     if avl_reset_i = '1' then
		      cmd_new_nbr      <= '0';
				cmd_init_nbr     <= '1';              -- on initialise la fonction a 0+0+0=0
				reg_leds_soc_s   <= (others => '0');
				reg_mode_gen     <= '0';              -- mode manuel par defaut
				reg_delay_gen    <= (others => '0');
		  elsif rising_edge(avl_clk_i) then
		      cmd_new_nbr   <= '0';  -- fin de la pulse d'un coup de clock
			   cmd_init_nbr  <= '0';  -- fin de la pulse d'un coup de clock
				cmd_take_snap <= '0';  -- fin de la pulse d'un coup de clock
		  
		      if avl_write_i = '1' then
				    case(to_integer(unsigned(avl_address_i))) is
				        when 0 =>
				            -- NOT USED
				        when 1 =>
				            -- NOT USED
				        when 2 =>
				            -- NOT USED
		              when 3 =>
				            reg_leds_soc_s <= avl_writedata_i(9 downto 0);
						  when 4 =>
						      cmd_new_nbr  <= avl_writedata_i(4);
								cmd_init_nbr <= avl_writedata_i(0);
						  when 5 =>
						      reg_mode_gen  <= avl_writedata_i(4);
								reg_delay_gen <= avl_writedata_i(1 downto 0);
								
						  when 6  => 
						      reg_safe_s <= avl_writedata_i(0);
						  when 7  =>
						      cmd_take_snap <= '1';
						  --when 8  => NOT USED
						  --when 9  => NOT USED
						  --when 10 => NOT USED
						  --when 11 => NOT USED
						  
				        when others =>
								-- RESERVED TO 15
						      -- THEN NOT USED
								null;
                end case;
            end if;
        end if;
    end process;
	 -- Interface management

	 
	 
	 regs_snap_p : process(avl_reset_i, avl_clk_i)
	 begin
	     if avl_reset_i = '1' then
		      reg_nbr_a_s <= (others => '0');
				reg_nbr_b_s <= (others => '0');
				reg_nbr_c_s <= (others => '0');
				reg_nbr_d_s <= (others => '0');
	     elsif rising_edge(avl_clk_i) then
		      if we_eq_s = '1' OR reg_safe_s = '0' then
				    reg_nbr_a_s <= nbr_a_i;
					 reg_nbr_b_s <= nbr_b_i;
					 reg_nbr_c_s <= nbr_c_i;
					 reg_nbr_d_s <= nbr_d_i;
				end if;
		  end if;
    end process;
	 
	 -------------------------------- MSS -----------------------------
	 
	 process (reg_safe_s, cmd_take_snap, mss_pres_s)
	 begin
	     mss_fut_s   <= E_WAIT_SNAP_REQ;
		  we_eq_s     <= '0';
		  flg_eq_en_s <= '0';
		  
		  case mss_pres_s is
		      when E_WAIT_SNAP_REQ =>
				    if(cmd_take_snap = '1' AND reg_safe_s = '1') then
					     mss_fut_s <= E_SAVE;
					 else
					     mss_fut_s <= E_WAIT_SNAP_REQ;
				    end if;
					 
			   when E_SAVE =>
				    we_eq_s <= '1';
				    mss_fut_s <= E_EQ_READABLE;
					 
			   when E_EQ_READABLE =>
				    flg_eq_en_s <= '1';
					 if (cmd_take_snap = '1' AND reg_safe_s = '1') then
					     mss_fut_s <= E_SAVE;
					 elsif (cmd_take_snap = '0' AND reg_safe_s = '1') then
					     mss_fut_s <= E_EQ_READABLE;
				    else 
					     mss_fut_s <= E_WAIT_SNAP_REQ;
					 end if;
					 
		      when others =>
				    mss_fut_s <= E_WAIT_SNAP_REQ;
	     end case;
	 end process;
	 
	 
	 MSS: process (avl_clk_i, avl_reset_i)
	 begin 
	     if (avl_reset_i = '1') then
		      mss_pres_s <= E_WAIT_SNAP_REQ;
		  elsif rising_edge(avl_clk_i) then
		      mss_pres_s <= mss_fut_s;
		  end if;
	 end process;
end rtl; 