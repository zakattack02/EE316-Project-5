library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL; -- Use numeric_std for arithmetic and conversion

entity PWM_Controller is
    generic (
        PWM_PERIOD : natural := 50000000; -- 20ms period at 50 MHz clock
        MIN_DUTY_CYCLE : natural := 1250000; -- 2.5% duty cycle at 50 MHz clock
        MAX_DUTY_CYCLE : natural := 6250000; -- 12.5% duty cycle at 50 MHz clock
        DATA_WIDTH : integer := 8 -- Width of SRAM_data
    );
    Port (
        clk : in STD_LOGIC;
        rst : in STD_LOGIC;
        potentiometer_value : in STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0); -- Input from potentiometer
        isEqual : out STD_LOGIC
    );
end PWM_Controller;

architecture Behavioral of PWM_Controller is
    signal counter_reg : STD_LOGIC_VECTOR(31 downto 0) := (others => '0');
    signal duty_cycle : STD_LOGIC_VECTOR(DATA_WIDTH - 1 downto 0); -- Updated duty cycle signal
begin
    process(clk, rst)
begin
    if rst = '1' then
        counter_reg <= (others => '0'); -- Reset the counter to zero
        duty_cycle <= (others => '0'); -- Initialize duty cycle to minimum
    elsif rising_edge(clk) then
        -- Calculate duty cycle based on potentiometer value
        duty_cycle <= std_logic_vector(to_unsigned((to_integer(unsigned(potentiometer_value)) * (MAX_DUTY_CYCLE - MIN_DUTY_CYCLE) + 32) / 64, duty_cycle'length));
        -- Counter logic
        if counter_reg = std_logic_vector(to_unsigned(PWM_PERIOD - 1, counter_reg'length)) then
            counter_reg <= (others => '0'); -- Reset when reaching maximum value
        else
            counter_reg <= std_logic_vector(unsigned(counter_reg) + 1); -- Increment the counter
        end if;
    end if;
end process;



   process(clk)
begin
    if rising_edge(clk) then
        if unsigned(counter_reg(31 downto DATA_WIDTH)) = unsigned(duty_cycle) then
            isEqual <= '1'; -- Compare counter output with duty cycle
        else
            isEqual <= '0';
        end if;
    end if;
end process;

        
end Behavioral;
