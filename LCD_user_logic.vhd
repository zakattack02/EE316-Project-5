library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.numeric_std.all;


entity LCD_user_logic is
GENERIC (
    CONSTANT cnt_max : integer := 62499); 
  PORT(
    clk          : IN     STD_LOGIC;      --system clock
    nreset       : in std_logic;
	 stateNumber  : in std_LOGIC_vector(1 downto 0);
	 iKey3		  : in std_logic;
	 iPWM_state_value : in integer range 0 to 4;
	 i16bitData   : in std_LOGIC_VECTOR(15 downto 0);
	 i8bitAddress : in std_LOGIC_VECTOR(7 downto 0);
    oDATA        : out std_logic_vector(7 downto 0);   --final 8bit data out
    oEN          : out std_logic;  -- final 1 bit Enable
    oRS          : out std_logic   -- final 1 bit Register Select
    
    );
  
end LCD_user_logic;
architecture Behavioral of LCD_user_logic is

TYPE state_type IS(start, ready, data_valid, busy_high, repeat); --needed states
signal state      : state_type;                   --state machine
signal reset_n    : STD_LOGIC;                    --active low reset
signal ena        : STD_LOGIC := '0';             --latch in data

signal busy       : STD_LOGIC;                    --indicates transaction in 

signal data0       : STD_LOGIC_VECTOR(7 DOWNTO 0); --data to write 
signal data1       : STD_LOGIC_VECTOR(7 DOWNTO 0); --data to write 
signal data2       : STD_LOGIC_VECTOR(7 DOWNTO 0); --data to write 
signal data3       : STD_LOGIC_VECTOR(7 DOWNTO 0); --data to write 
signal data_wr    : STD_LOGIC_VECTOR(7 DOWNTO 0); --data to write 

signal address_LCD0,address_LCD1 : std_LOGIC_vector(7 downto 0);
signal data_lcd0,data_lcd1,data_lcd2,data_lcd3 : std_LOGIC_vector(7 downto 0);

signal rs_in      : STD_LOGIC;                    -- temp register select
signal rs_in0      : STD_LOGIC;                    -- temp register select
signal rs_in1      : STD_LOGIC;                    -- temp register select
signal rs_in2      : STD_LOGIC;                    -- temp register select
signal rs_in3      : STD_LOGIC;                    -- temp register select

signal byteSel0    : integer range 0 to 100:=0;
signal byteSel1    : integer range 0 to 100:=0;
signal byteSel2    : integer range 0 to 300:=0;
signal byteSel3   : integer range 0 to 200:=0;

signal stateNumber_change : std_LOGIC_VECTOR(1 downto 0);
signal count 	  : unsigned(27 DOWNTO 0):=X"000000F";
signal sig_data     : std_logic_vector(7 downto 0);
signal sig_en     : std_logic;
signal sig_rs     : std_logic;

component EightBitToLCDLUT is
	PORT(
         iFourBit		:	in std_logic_vector(3 downto 0);
			oLCD_Data		:	out std_logic_vector(7 downto 0)
		);

end component;
component LCD_FSM is
    Generic (Constant CntMax : integer:= 62499);  -- (125 MHz/200 KHz) - 1 = 249
    Port ( clock             : in std_logic; -- board clock
           iData             : in std_logic_vector(7 downto 0);  
           rs                : in std_logic;
           iReset_n          : in std_logic;  
           iEna              : in std_logic;  
           oBusy             : out std_logic;         
           oDATA        : out std_logic_vector(7 downto 0);
           oEN          : out std_logic;  --(1-bit Enable)
           oRS          : out std_logic   --(1-bit Register_Select)
          );
end component;

begin

		oDATA <= sig_DATA;
		oEN	<= sig_EN;	
		oRS	<= sig_RS;	
		
		
process (stateNumber) is 			--- This prints current state of the 4 state machine onto the hexes
begin
	case stateNumber is
		when "00" => rs_in <= rs_in0;
		when "01" => rs_in <= rs_in1;
		when "10" => rs_in <= rs_in2;
		when "11" => rs_in <= rs_in3;
	end case;
end process;

Inst_LCD_FSM: LCD_FSM
	GENERIC map(
		CntMax => cnt_max)
	port map(
		iReset_n	=> reset_n, 		
		clock	=> clk, 		
		iEna 	=> ena,			
		iData	=> data_wr,
		rs      => rs_in, 		
		oBusy 	=> busy,			
		oDATA	=> sig_DATA,
		oEN	=> sig_EN,	
		oRS	=> sig_RS	
		); 


reset_n <= nreset;

process(clk)
begin
if rising_edge(clk) then
stateNumber_change <= stateNumber;
end if;
end process;


process(clk, reset_n)
begin  
    if (stateNumber /= stateNumber_change) then  --R
        count <= X"000000F";
        byteSel0 <= 0;
		  byteSel1 <= 0;
		  byteSel2 <= 0;
		  byteSel3 <= 0;
        state <= start;


    elsif(clk'event and clk = '1') then 
 
  case state is 
  when start =>
	    if count /= X"0000000" then                         
		count   <= count - 1;	
		--reset_n <= '0';	
		state   <= start;
		ena 	<= '0';  
	else
		--reset_n <= '1'; 
		if stateNumber = "00" then
			data_wr <= data0;                --data to be written 
		elsif stateNumber = "01"  then
		   data_wr <= data1;  
		elsif stateNumber = "10" then
			data_wr <= data2;  
		elsif stateNumber =  "11" then
			data_wr <= data3;  
		end if;
			state   <= ready;
		
    end if;

  when ready =>		
	      if busy = '0' then
	      	ena     <= '1';
	      	state   <= data_valid;
	      end if;

  when data_valid =>                              --state for conducting this transaction
              if busy = '1' then  
        	ena     <= '0';
        	state   <= busy_high;
              end if;

  when busy_high => 
              if(busy = '0') then                -- busy just went low 
		      state <= repeat;
   	      end if;		     
  when repeat => 
  
			if stateNumber = "00" then				-- INIT MODE
          	if byteSel0 < 99 then
           	   byteSel0 <= byteSel0 + 1;
				else	 
           	   byteSel0 <= 22;           
         	end if;
			elsif stateNumber = "01" then			-- TESTING MODE
			   if byteSel1 < 24 then
           	   byteSel1 <= byteSel1 + 1;
				else	 
           	   byteSel1 <= 17;           
         	end if;
			elsif stateNumber = "10" then			-- PAUSE MODE
		      if byteSel2 < 200 then
           	   byteSel2 <= byteSel2 + 1;
				else	 
           	   byteSel2 <= 100;           
         	end if;
				
			else
				if iPWM_state_value = 0 then		-- 60 hz
					if byteSel3 < 49 then
						byteSel3 <= byteSel3 + 1;
					else	 
						byteSel3 <= 24;           
					end if;

				elsif iPWM_state_value = 1 then	-- 120 hz
					if byteSel3 < 79 then
						byteSel3 <= byteSel3 + 1;
					else	 
						byteSel3 <= 50;           
					end if;
	
				elsif iPWM_state_value = 2 then	 -- 1000hz
					if byteSel3 < 120 then
						byteSel3 <= byteSel3 + 1;
					else	 
						byteSel3 <= 80;           
					end if;
				end if;
			end if;
			
   	   state <= start; 
  when others => null;

  end case;   
  
  
end if;  

end process;      
------------------------------------------------------------------------------------------------------------------
process(byteSel3)   --LUT 8bit Data
 begin
    case byteSel3 is
       when 0  => data3 <=   X"38"; rs_in3 <=  '0';
       when 1  => data3 <=   X"38"; rs_in3 <=  '0';
       when 2  => data3 <=   X"38"; rs_in3 <=  '0';
       when 3  => data3 <=   X"38"; rs_in3 <=  '0';
       when 4  => data3 <=   X"38"; rs_in3 <=  '0';
       when 5  => data3 <=   X"38"; rs_in3 <=  '0';  
       when 6  => data3 <=   X"01"; rs_in3 <=  '0';
       when 7  => data3 <=   X"0C"; rs_in3 <=  '0';
       when 8  => data3 <=   X"06"; rs_in3 <=  '0';
       when 9  => data3 <=   X"80"; rs_in3 <=  '0';
       when 10  => data3 <=  X"50"; rs_in3 <=  '1'; --P
       when 11  => data3 <=  X"57"; rs_in3 <=  '1'; --W	
       when 12  => data3 <=  X"4D"; rs_in3 <=  '1'; --M
       when 13  => data3 <=  X"FE"; rs_in3 <=  '1'; --
       when 14  => data3 <=  X"47"; rs_in3 <=  '1'; --G
       when 15  => data3 <=  X"65"; rs_in3 <=  '1'; --e
       when 16  => data3 <=  X"6E"; rs_in3 <=  '1'; --n  
       when 17  => data3 <=  X"65"; rs_in3 <=  '1'; --e
       when 18  => data3 <=  X"72"; rs_in3 <=  '1'; --r
       when 19  => data3 <=  X"61"; rs_in3 <=  '1'; --a
       when 20  => data3 <=  X"74"; rs_in3 <=  '1'; --t  
       when 21  => data3 <=  X"69"; rs_in3 <=  '1'; --i
       when 22  => data3 <=  X"6F"; rs_in3 <=  '1'; --o
       when 23  => data3 <=  X"6E"; rs_in3 <=  '1'; --n  
		 
       when 24  => data3 <=  X"C0"; rs_in3 <=  '0'; -- Start OF Second Line
--		 when 25  => data3 <=  X"02"; rs_in3 <=  '0';-- 60 HZ START
		 when 26  => data3 <=  X"36"; rs_in3 <=  '1';
       when 27  => data3 <=  X"30"; rs_in3 <=  '1';
       when 28  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 29  => data3 <=  X"48"; rs_in3 <=  '1';
       when 30  => data3 <=  X"7A"; rs_in3 <=  '1';
		 when 31  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 32  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 33  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 34  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 35  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 36  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 37  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 38  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 39  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 40  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 41  => data3 <=  X"FE"; rs_in3 <=  '1';
		 
       when 50  => data3 <=  X"C0"; rs_in3 <=  '0';-- 120 Hz Start
--		 when 51  => data3 <=  X"02"; rs_in3 <=  '0';
       when 52  => data3 <=  X"31"; rs_in3 <=  '1';
       when 53  => data3 <=  X"32"; rs_in3 <=  '1';
       when 54  => data3 <=  X"30"; rs_in3 <=  '1';
       when 55  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 56  => data3 <=  X"48"; rs_in3 <=  '1';
       when 57  => data3 <=  X"7A"; rs_in3 <=  '1';
		 when 58  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 59  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 60  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 61  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 62  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 63  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 64  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 65  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 66  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 67  => data3 <=  X"FE"; rs_in3 <=  '1';
		 
		 
       when 80  => data3 <=  X"C0"; rs_in3 <=  '0'; -- 1kHz
--		 when 81  => data3 <=  X"02"; rs_in3 <=  '0';
		 when 82  => data3 <=  X"31"; rs_in3 <=  '1';
		 when 83  => data3 <=  X"30"; rs_in3 <=  '1';
		 when 84  => data3 <=  X"30"; rs_in3 <=  '1';
		 when 85  => data3 <=  X"30"; rs_in3 <=  '1';
		 when 86  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 87  => data3 <=  X"48"; rs_in3 <=  '1';
       when 88  => data3 <=  X"7A"; rs_in3 <=  '1';
		 when 89  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 90  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 91  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 92  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 93  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 94  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 95  => data3 <=  X"FE"; rs_in3 <=  '1';
		 when 96  => data3 <=  X"FE"; rs_in3 <=  '1';
       when 97  => data3 <=  X"FE"; rs_in3 <=  '1';
       when others => data3 <= X"38"; rs_in3 <= '0';
    end case;
end process;   
------------------------------------------------------------------------------------------------------------------
process(byteSel0)   --LUT 8bit Data
 begin
    case byteSel0 is
        when 0  => data0 <=   X"38"; rs_in0 <=  '0';
        when 1  => data0 <=   X"38"; rs_in0 <=  '0';
        when 2  => data0 <=   X"38"; rs_in0 <=  '0';
        when 3  => data0 <=   X"38"; rs_in0 <=  '0';
        when 4  => data0 <=   X"38"; rs_in0 <=  '0';
        when 5  => data0 <=   X"38"; rs_in0 <=  '0';
        when 6  => data0 <=   X"01"; rs_in0 <=  '0';
        when 7  => data0 <=   X"0C"; rs_in0 <=  '0';
        when 8  => data0 <=   X"06"; rs_in0 <=  '0';
        when 9  => data0 <=   X"80"; rs_in0 <=  '0';
        when 10  => data0 <=  X"28"; rs_in0 <=  '0'; --Set 4 Bit mode
        when 11  => data0 <=  X"40"; rs_in0 <=  '1'; --I
        when 12  => data0 <=  X"90"; rs_in0 <=  '1'; --I

        when 13  => data0 <=  X"60"; rs_in0 <=  '1'; --n	
        when 14  => data0 <=  X"E0"; rs_in0 <=  '1'; --n

        when 15  => data0 <=  X"60"; rs_in0 <=  '1'; --i
        when 16  => data0 <=  X"90"; rs_in0 <=  '1'; --i

        when 17  => data0 <=  X"70"; rs_in0 <=  '1'; --t
        when 18  => data0 <=  X"40"; rs_in0 <=  '1'; --t

	    when 50  => data0 <=  X"2E"; rs_in0 <=  '1'; --.
	    when 75  => data0 <=  X"2E"; rs_in0 <=  '1'; --.
	    when 95  => data0 <=  X"8C"; rs_in0 <=  '0'; --.
	    when 96  => data0 <=  X"FE"; rs_in0 <=  '1'; --.
	    when 97  => data0 <=  X"FE"; rs_in0 <=  '1'; --.
	    when 98  => data0 <=  X"FE"; rs_in0 <=  '1'; --.
	    when 99  => data0 <=  X"8C"; rs_in0 <=  '0'; --.

       when others => data0 <= X"38"; rs_in0 <= '0';
   end case;
end process;
------------------------------------------------------------------------------------------------------------------

process(byteSel1)   --LUT 8bit Data
 begin
    case byteSel1 is

       when 0  => data1 <=   X"38"; rs_in1 <=  '0';
       when 1  => data1 <=   X"38"; rs_in1 <=  '0';
       when 2  => data1 <=   X"38"; rs_in1 <=  '0';
       when 3  => data1 <=   X"38"; rs_in1 <=  '0';
       when 4  => data1 <=   X"38"; rs_in1 <=  '0';
       when 5  => data1 <=   X"38"; rs_in1 <=  '0';
       when 6  => data1 <=   X"01"; rs_in1 <=  '0';
       when 7  => data1 <=   X"0C"; rs_in1 <=  '0';
       when 8  => data1 <=   X"06"; rs_in1 <=  '0';
       when 9  => data1 <=   X"80"; rs_in1 <=  '0';
       when 10  => data1 <=  X"54"; rs_in1 <=  '1'; --T
       when 11  => data1 <=  X"65"; rs_in1 <=  '1'; --e	
       when 12  => data1 <=  X"73"; rs_in1 <=  '1'; --s
       when 13  => data1 <=  X"74"; rs_in1 <=  '1'; --t
       when 14  => data1 <=  X"69"; rs_in1 <=  '1'; --i
       when 15  => data1 <=  X"6E"; rs_in1 <=  '1'; --n
       when 16  => data1 <=  X"67"; rs_in1 <=  '1'; --g  
       when 17  => data1 <=  X"C0"; rs_in1 <=  '0'; -- Start OF Second Line
       when 18  => data1 <=  address_LCD1; rs_in1 <=  '1';
       when 19  => data1 <=  address_LCD0; rs_in1 <=  '1';
       when 20  => data1 <=  X"FE"; 		 rs_in1 <=  '1';
       when 21  => data1 <=  Data_lcd3; rs_in1 <=  '1';
       when 22  => data1 <=  Data_lcd2; rs_in1 <=  '1';
       when 23  => data1 <=  Data_lcd1; rs_in1 <=  '1';
       when 24  => data1 <=  Data_lcd0; rs_in1 <=  '1';

       when others => data1 <= X"38"; rs_in1 <= '0';
    end case;
end process;
------------------------------------------------------------------------------------------------------------------
process(byteSel2)   --LUT 8bit Data
 begin
    case byteSel2 is
       when 0  => data2 <=   X"38"; rs_in2 <=  '0';
       when 1  => data2 <=   X"38"; rs_in2 <=  '0';
       when 2  => data2 <=   X"38"; rs_in2 <=  '0';
       when 3  => data2 <=   X"38"; rs_in2 <=  '0';
       when 4  => data2 <=   X"38"; rs_in2 <=  '0';
       when 5  => data2 <=   X"38"; rs_in2 <=  '0';  
       when 6  => data2 <=   X"01"; rs_in2 <=  '0';
       when 7  => data2 <=   X"0C"; rs_in2 <=  '0';
       when 8  => data2 <=   X"06"; rs_in2 <=  '0';
       when 9  => data2 <=   X"80"; rs_in2 <=  '0';
       when 10  => data2 <=  X"50"; rs_in2 <=  '1'; --P
       when 11  => data2 <=  X"61"; rs_in2 <=  '1'; --a	
       when 12  => data2 <=  X"75"; rs_in2 <=  '1'; --u
       when 13  => data2 <=  X"73"; rs_in2 <=  '1'; --s
       when 14  => data2 <=  X"65"; rs_in2 <=  '1'; --e
       when 15  => data2 <=  X"64"; rs_in2 <=  '1'; --d
       when 16  => data2 <=  X"FE"; rs_in2 <=  '1'; --  
		 when 17  => data2 <=  X"5E"; rs_in2 <=  '1'; --  ^
		 when 18  => data2 <=  X"6F"; rs_in2 <=  '1'; --  o
		 when 19  => data2 <=  X"5E"; rs_in2 <=  '1'; --  ^

		 
		 
       when 30  => data2 <=  X"C0"; rs_in2 <=  '0'; -- Start OF Second Line
       when 31  => data2 <=  address_LCD1; rs_in2 <=  '1';
       when 32  => data2 <=  address_LCD0; rs_in2 <=  '1';
       when 33  => data2 <=  X"FE"; rs_in2 <=  '1';
       when 34  => data2 <=  Data_lcd3; rs_in2 <=  '1';
       when 35  => data2 <=  Data_lcd2; rs_in2 <=  '1';
       when 36  => data2 <=  Data_lcd1; rs_in2 <=  '1';
       when 37  => data2 <=  Data_lcd0; rs_in2 <=  '1';
		 
		 when 100 => data2 <=  X"88"; rs_in2 <=  '0';
		 when 101 => data2 <=  X"30"; rs_in2 <=  '1'; -- 0
		 
		 when 150 => data2 <=  X"88"; rs_in2 <=  '0';
		 when 151 => data2 <=  X"6F"; rs_in2 <=  '1'; -- o

       when others => data2 <= X"38"; rs_in2 <= '0';
    end case;
end process;

-----------------------------------------------------------------------------------------------------------------------
INST_ADDRESS1: EightBitToLCDLUT
	PORT map(
         iFourBit		=> i8bitAddress(7 downto 4),
			oLCD_Data	=> address_LCD1
		);
-----------------------------------------------------------------------------------------------------------------------
INST_ADDRESS0: EightBitToLCDLUT
	PORT map(
         iFourBit		=> i8bitAddress(3 downto 0),
			oLCD_Data	=> address_LCD0
		);
-----------------------------------------------------------------------------------------------------------------------
INST_DATA3: EightBitToLCDLUT
	PORT map(
         iFourBit		=> i16bitData(15 downto 12),
			oLCD_Data	=> Data_lcd3
		);
-----------------------------------------------------------------------------------------------------------------------
INST_DATA2: EightBitToLCDLUT
	PORT map(
         iFourBit		=> i16bitData(11 downto 8),
			oLCD_Data	=> Data_lcd2
		);
-----------------------------------------------------------------------------------------------------------------------
INST_DATA1: EightBitToLCDLUT
	PORT map(
         iFourBit		=> i16bitData(7 downto 4),
			oLCD_Data	=> Data_lcd1
		);
-----------------------------------------------------------------------------------------------------------------------
INST_DATA0: EightBitToLCDLUT
	PORT map(
         iFourBit		=> i16bitData(3 downto 0),
			oLCD_Data	=> Data_lcd0
		);

end Behavioral;
