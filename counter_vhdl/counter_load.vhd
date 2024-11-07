library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.all;

entity counter_load is
    generic (
        NUM_COUNTERS : positive := 8;  -- Total number of counters available
        COUNTER_WIDTH : positive := 8   -- Width of each counter (matching up_counter)
    );
    port (
        clk         : in  std_logic;
        rst         : in  std_logic;
        -- Input to control how many counters are active (binary encoded)
        active_cnt  : in  std_logic_vector(7 downto 0)  -- Supports up to 256 counters
    );
end counter_load;

architecture rtl of counter_load is
    -- Constants
    type counter_array is array (natural range <>) of std_logic_vector(COUNTER_WIDTH-1 downto 0);
    signal counter_outputs : counter_array(0 to NUM_COUNTERS-1);
    signal counter_enables : std_logic_vector(0 to NUM_COUNTERS-1);
    constant MAX_COUNT : std_logic_vector(COUNTER_WIDTH-1 downto 0) := (others => '1');
    signal at_limit_signals : std_logic_vector(0 to NUM_COUNTERS-1);

begin
    enable_proc: process(active_cnt)
        variable active_counters : integer;
    begin
        active_counters := to_integer(unsigned(active_cnt));
        -- Clear all enables first
        counter_enables <= (others => '0');       
--        if active_counters > NUM_COUNTERS then
--            active_counters := NUM_COUNTERS;
--        end if;
        
--        for i in counter_enables'range loop
--           if i < active_counters then
--                counter_enables(i) <= '1';
--            else
--                counter_enables(i) <= '0';
--            end if;
--        end loop;
--    end process enable_proc;
        -- Enable only the specified number of counters
        for i in 0 to NUM_COUNTERS-1 loop
            if i < active_counters then
                counter_enables(i) <= '1';
            end if;
        end loop;
    end process enable_proc;

    -- Use direct entity instantiation
    COUNTER_GEN: for i in 0 to NUM_COUNTERS-1 generate
        counter_inst: entity work.up_counter
            generic map (
                g_COUNTER_BITS  => COUNTER_WIDTH,
                g_INITIAL_VALUE => 0
            )
            port map (
                CLK       => clk,
                RSTN      => not rst,
                CEN       => counter_enables(i),
                LIMIT     => MAX_COUNT,
                PLOAD     => '0',
                PLOAD_VAL => (others => '0'),
                COUNT     => counter_outputs(i),
                AT_LIMIT  => at_limit_signals(i)
            );
    end generate;
    
end rtl;
