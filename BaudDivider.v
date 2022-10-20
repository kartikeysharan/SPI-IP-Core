module BaudDivider (clk, reset, enable, brd, baud_out);
	input clk, reset, enable;
	input[31:0] brd;
	output reg baud_out;
	reg clk_out;
	reg [31:0] count;		
	reg [31:0] match;

	always @ (posedge clk)
	begin
		if(reset || !enable)
			begin
				clk_out <= 1'b0;
				baud_out <= 1'b0;
				count <= 32'b0;
				match <= brd;
			end
		else
			begin
				clk_out <= !clk_out;
				count <= count + 32'b10000000;
				if(count[31:7] == match[31:7])
					begin
						match <= match + brd;
						baud_out <= !baud_out;
					end
			end
	end
endmodule
