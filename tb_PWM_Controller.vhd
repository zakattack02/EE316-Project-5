LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY tb_PWM_Controller IS
END tb_PWM_Controller;

ARCHITECTURE behavior OF tb_PWM_Controller IS
    COMPONENT PWM_Controller
    PORT(
         clk : IN std_logic;
         rst : IN std_logic;
         potentiometer_value : IN std_logic_vector(7 downto 0);
         isEqual : OUT std_logic
        );
    END COMPONENT;

    --Inputs
    signal clk : std_logic := '0';
    signal rst : std_logic := '0';
    signal potentiometer_value : std_logic_vector(7 downto 0) := (others => '0');

    --Outputs
    signal isEqual : std_logic;

    -- Clock period definitions
    constant clk_period : time := 20 ns; -- Assuming a 50 MHz clock

BEGIN
    -- Instantiate the Unit Under Test (UUT)
    uut: PWM_Controller PORT MAP (
          clk => clk,
          rst => rst,
          potentiometer_value => potentiometer_value,
          isEqual => isEqual
        );

    -- Clock process definitions
    clk_process :process
    begin
        clk <= '0';
        wait for clk_period/2;
        clk <= '1';
        wait for clk_period/2;
    end process;

    -- Stimulus process
    stim_proc: process
    begin  
        -- Reset the system
        rst <= '1';
        wait for clk_period*10;
        rst <= '0';
       
        -- Test with different potentiometer values
        for i in 0 to 255 loop
            potentiometer_value <= std_logic_vector(to_unsigned(i, 8)); -- Simulate potentiometer input
            wait for clk_period*100; -- Increase wait time for more cycles
        end loop;
       
        wait;
    end process;

    -- Assertion for testing isEqual
    assert_proc: process(clk)
    begin
        if rising_edge(clk) then
            -- Example assertion: isEqual should be '1' when counter_reg equals duty_cycle
            -- Adjust this assertion based on your specific test conditions
            assert isEqual = '1' report "isEqual signal did not behave as expected" severity error;
        end if;
    end process;

END;
