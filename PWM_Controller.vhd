library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity PWM_Controller is
    Port ( clk : in STD_LOGIC;
           rst : in STD_LOGIC;
           state : in STD_LOGIC;
			  hertz : in std_LOGIC_VECTOR(15 downto 0);
           SRAM_data : in STD_LOGIC_VECTOR(7 downto 0);
           isEqual : out STD_LOGIC
         );
end entity PWM_Controller;

architecture Behavioral of PWM_Controller is
    signal counter_reg : STD_LOGIC_VECTOR(7 downto 0) := (others => '0');
	 signal counter_reg2 : STD_LOGIC_VECTOR(5 downto 0) := (others => '0');
	 signal counter_reg3 : STD_LOGIC_VECTOR(6 downto 0) := (others => '0');
																					-- Begin process for the PWM_Controller counter	 
begin
    process(clk, rst, state, hertz)
    begin
        if rst = '1' or state = '0' then
            counter_reg <= (others => '0'); 							-- Reset the counter to zero
				counter_reg2 <= (others => '0'); 
				counter_reg3 <= (others => '0');
        elsif rising_edge(clk) and hertz = X"0060" then
		     counter_reg <= counter_reg + 1; 							-- Increment the counter
            if counter_reg = "11111111" then
                counter_reg <= (others => '0');					   -- Reset when reaching maximum value
            end if;
				
--		 elsif rising_edge(clk) and hertz = X"0120"  then
--		     counter_reg3 <= counter_reg3 + 1; 						-- Increment the counter
--            if counter_reg3 = "1111111" then
--                counter_reg3 <= (others => '0');					 -- Reset when reaching maximum value
--            end if;
				
--		  elsif rising_edge(clk) and hertz = X"1000"  then
--		     counter_reg2 <= counter_reg2 + 1;							 -- Increment the counter
--            if counter_reg2 = "111111" then
--                counter_reg2 <= (others => '0');					 -- Reset when reaching maximum value
--            end if;
       end if;
 end process;
																					-- End process for the PWM_Controller counter

																					-- Begin comparator process for PWM_Controller   
    process(clk, rst, state, hertz)
    begin
        if rst = '1' or state = '0' then
            isEqual <= '0'; 												-- Reset the output to zero
        elsif rising_edge(clk) and hertz = X"0060"  then
            if counter_reg <= SRAM_data then
                isEqual <= '1'; 											-- Compare counter output with SRAM data input 
            else
                isEqual <= '0';
            end if;
--			 elsif rising_edge(clk) and hertz = X"0120" then
--            if counter_reg3 <= SRAM_data(7 downto 1) then
--                isEqual <= '1'; 											-- Compare counter output with SRAM data input 
--            else
--                isEqual <= '0';
--            end if;
--				   elsif rising_edge(clk) and hertz = X"1000" then
--            if counter_reg2 <= SRAM_data(7 downto 2) then
--                isEqual <= '1';										  -- Compare counter output with SRAM data input 
--            else
--                isEqual <= '0';
--            end if;
        end if;
    end process;
																					-- End comparator process for PWM_Controller

end Behavioral;
