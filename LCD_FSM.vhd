library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity LCD_FSM is
    Generic (Constant CntMax : integer:= 62499);  --74997 this is for 5ms effective clock 

    Port ( clock             : in std_logic; -- board clock
           iData             : in std_logic_vector(7 downto 0);  
           --this will be 9 bits, the last bit (8) will be RS value
           rs                : in std_logic;
           iReset_n          : in std_logic;  
           iEna              : in std_logic;  
           oBusy             : out std_logic;         
           oDATA        : out std_logic_vector(7 downto 0);
           oEN          : out std_logic;  --(1-bit Enable)
           oRS          : out std_logic   --(1-bit Register_Select)
          );
end LCD_FSM;

architecture Behavioral of LCD_FSM is

--1> machine a is x2 faster than b
--
--3> 3a 1.6CPI
--Pulse_Zero_Rising is the default state and repeats when FSM not enabled

--the perspective of the en signal pulsing 

type stateType is (Zero_Rising, One, Zero_Falling);   --Ready, Set, Pulse, Done
    signal state    : stateType;	    
    signal cnt      : integer range 0 to CntMax;      --use CntMax
    --signal Bit_cnt  : integer;
	 signal clock_en : std_logic := '0';
    --signal Data     : std_logic_vector(7 downto 0); -- byte to send to display
    signal en_reg   : std_logic := '0';
    signal rs_reg   : std_logic := '0';
    signal data_reg   : std_logic_vector(7 downto 0) := (others => '0');
    --signal pulse_count : integer range 0 to 6; --how many times the pulse state has been checked

    
begin

Clock_Enable:
process(clock, iReset_n)
begin
  if iReset_n = '1' then 
	    cnt<=0;
	    clock_en<='0';
  elsif rising_edge(clock) then
	if cnt = CntMax then                           --Use CntMax 
		  clock_en <= '1';
		  cnt <=0;
	else
		  clock_en <= '0';
		  cnt <= cnt+1;
	end if;
end if;
end process;



oEN <= en_reg;
oRS <= rs_reg;
oDATA <= data_reg;

LCD_state_machine:
process(clock, iReset_n, clock_en)

begin
if iReset_n ='1' then    
    data_reg <= (others => '0');   --assumption when there is no data and this is the default power-on-reset
    rs_reg <= '0';
    
    en_reg <= '0';
    oBusy     <= '1';
    state     <= Zero_Rising;        
elsif rising_edge(clock) and clock_en = '1' then 
    case state is 
    
    
        when Zero_Rising =>
          if iEna = '0' then
            oBusy  <= '0';
            state <= Zero_Rising;
          else 
            oBusy <= '1';
                       
            data_reg <= iData(7 downto 0);  
            rs_reg <= rs;
            
            en_reg <= '0';  --5ms/3 low

            state <= One; 
          end if;       
          
          
        when One =>        
            en_reg <= '1';   --5ms/3 high
            state <= Zero_Falling;
              

        when Zero_Falling =>
            en_reg <= '0';  --5ms/3 low    
            oBusy <= '0'; 
            state <= Zero_Rising;                

          

        when others =>
            state <= Zero_Rising;
    end case;             
end if;
   
end process;

end Behavioral;

