module task2_top (
	SW,
	HEX0_3,
	HEX4_7,
	HEX8_9
	);

	input [9:0] SW;
	output [6:0] HEX0_3;
	output [6:0] HEX4_7;
	output [6:0] HEX8_9;
	
	hex_to_7seg SEG0(SW[3:0],SW[7:4],SW[9:8],HEX0_3,HEX4_7,HEX8_9);

endmodule
