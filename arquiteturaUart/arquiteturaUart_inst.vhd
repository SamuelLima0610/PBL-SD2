	component arquiteturaUart is
		port (
			clk_clk                        : in  std_logic := 'X'; -- clk
			uart_0_external_connection_rxd : in  std_logic := 'X'; -- rxd
			uart_0_external_connection_txd : out std_logic         -- txd
		);
	end component arquiteturaUart;

	u0 : component arquiteturaUart
		port map (
			clk_clk                        => CONNECTED_TO_clk_clk,                        --                        clk.clk
			uart_0_external_connection_rxd => CONNECTED_TO_uart_0_external_connection_rxd, -- uart_0_external_connection.rxd
			uart_0_external_connection_txd => CONNECTED_TO_uart_0_external_connection_txd  --                           .txd
		);

