library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.all;

entity up_counter is
    generic (
        --! Number of bits used for the COUNT output
        g_COUNTER_BITS      : natural := 8;

        --! Value that COUNT will be set to on reset
        g_INITIAL_VALUE     : natural := 0
    );
    port (
        --! Clock
        CLK             : in    std_logic;

        --! Reset, activelow
        RSTN            : in     std_logic;

        -----------------------------ControlInterface---------------------------

        --! Clock enable, up_counter increments only when this is '1'
        CEN             : in     std_logic;

        --! Count limit, will reset to initial value when count is equal to this value
        LIMIT           : in     std_logic_vector(g_COUNTER_BITS - 1 downto 0) := ( others=> '1');


        -----------------------------LoadingInterface---------------------------

        --! Pulse load, when '1' COUNT is set to PLOAD_VAL on then ext clock cycle
        PLOAD           : in     std_logic := '0';

        --! Value to load when PLOAD is asserted
        PLOAD_VAL       : in     std_logic_vector(g_COUNTER_BITS - 1 downto 0) := (others => '0');

        --! Current count value (unsigned)
        COUNT           : out    std_logic_vector(g_COUNTER_BITS - 1 downto 0);

        --! Pulse when the count reaches the limit
        AT_LIMIT        : out    std_logic
 );
 end entity up_counter;

 architecture rtl of up_counter is
    -- Internal counter signal
    signal count_int : unsigned(g_COUNTER_BITS - 1 downto 0);
    
begin
    -- Counter process
    count_proc: process(CLK)
    begin
        if rising_edge(CLK) then
            if RSTN = '0' then
                -- Reset condition
                count_int <= to_unsigned(g_INITIAL_VALUE, g_COUNTER_BITS);
                AT_LIMIT <= '0';
            else
                if PLOAD = '1' then
                    -- Load value
                    count_int <= unsigned(PLOAD_VAL);
                    AT_LIMIT <= '0';
                elsif CEN = '1' then
                    -- Normal counting operation
                    if count_int = unsigned(LIMIT) then
                        count_int <= to_unsigned(g_INITIAL_VALUE, g_COUNTER_BITS);
                        AT_LIMIT <= '1';
                    else
                        count_int <= count_int + 1;
                        AT_LIMIT <= '0';
                    end if;
                end if;
            end if;
        end if;
    end process count_proc;

    -- Output assignment
    COUNT <= std_logic_vector(count_int);
    
end architecture rtl;