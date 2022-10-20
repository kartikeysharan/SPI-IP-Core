module BaudDivider (clock, reset,enable, brd, brd_out, baud_out, count, target);
	input clock, reset, enable;
	input[31:0] brd;
	output reg brd_out;
	output reg baud_out;
	output reg[31:0] count;
	output reg[31:0] target;
		always @(posedge clock)
			begin
				if (reset || !enable)
				begin
						brd_out <= 1'b0;
						baud_out <= 1'b0;
						count <= 32'b0;
						target <= brd;
				end
				else
				begin
					if (enable == 1)
					begin
						brd_out <= !brd_out;
						count <= count + 32'b10000000;
							if (count[31:7] == target[31:7])
							begin
								target <= target + brd;
								brd_out<= !brd_out;
							end
					end
				end
			end
endmodule
