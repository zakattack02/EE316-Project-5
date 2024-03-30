----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 03/30/2024 03:28:29 PM
-- Design Name: 
-- Module Name: UltraSonic - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity Ultrasonic_Sensor is
    Port (
        -- Inputs
        Trigger : in std_logic;
        Echo : in std_logic;
        Clock : in std_logic;
        Reset : in std_logic;
        -- Outputs
        LED_Red : out std_logic;
        LED_Yellow : out std_logic;
        LED_Green : out std_logic;
        Alarm : out std_logic;
        LCD_Distance : out std_logic_vector(15 downto 0);
        Buzzer_Frequency : out std_logic_vector(15 downto 0)
    );
end Ultrasonic_Sensor;

architecture Behavioral of Ultrasonic_Sensor is
    -- Constants for distance thresholds
    constant DISTANCE_1FT : integer := 30; -- Distance threshold for 1 foot (in microseconds)
    constant DISTANCE_2FT : integer := 60; -- Distance threshold for 2 feet (in microseconds)
    
    -- Internal signals
    signal Echo_Start : std_logic := '0';
    signal Echo_End : std_logic := '0';
    signal Echo_Width : integer := 0;
    signal Distance : integer := 0;
    signal Buzzer_Freq : integer := 1000; -- Initial frequency
    
begin
    -- Echo pulse detection process
    process(Reset, Clock)
    begin
        if Reset = '1' then
            Echo_Start <= '0';
            Echo_End <= '0';
            Echo_Width <= 0;
        elsif rising_edge(Clock) then
            if Echo = '1' then
                Echo_Start <= '1';
            elsif Echo = '0' and Echo_Start = '1' then
                Echo_End <= '1';
            end if;
        end if;
    end process;
    
    -- Distance calculation process
    process(Echo_End)
    begin
        if Echo_End = '1' then
            Distance <= Echo_Width / 70; -- Convert pulse width to distance in centimeters
        end if;
    end process;
    
    -- LED and alarm control process
    process(Distance)
    begin
        if Distance <= DISTANCE_1FT then
            LED_Red <= '1';
            LED_Yellow <= '0';
            LED_Green <= '0';
            Alarm <= '1';
        elsif Distance <= DISTANCE_2FT then
            LED_Red <= '0';
            LED_Yellow <= '1';
            LED_Green <= '0';
            Alarm <= '1';
            -- Adjust buzzer frequency
            Buzzer_Freq <= 1000 + (3000 - 1000) * (DISTANCE_1FT - Distance) / (DISTANCE_1FT - DISTANCE_2FT);
        else
            LED_Red <= '0';
            LED_Yellow <= '0';
            LED_Green <= '1';
            Alarm <= '0';
            Buzzer_Freq <= 1000; -- Reset buzzer frequency
        end if;
    end process;
    
    -- LCD display output
    process(Distance)
    begin
        -- Display distance on the first line of LCD
        LCD_Distance <= std_logic_vector(to_unsigned(Distance, LCD_Distance'length));
        -- Additional information on the second line (customize as needed)
        -- LCD_Info <= "Custom Info";
    end process;

    -- Buzzer frequency output (connect to buzzer module)
    -- Buzzer_Frequency <= std_logic_vector(to_unsigned(Buzzer_Freq, Buzzer_Frequency'length));
end Behavioral;
