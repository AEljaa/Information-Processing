module task1_top (
	SW,
	HEX8
	);

	input [3:0] SW;
	output [6:0] HEX8;
	
	hex_to_7seg(HEX8,SW[3:0]);

endmodule
