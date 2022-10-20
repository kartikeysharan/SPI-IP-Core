module edgeDetector (input reset, input signal, input clk, output pulse);

	reg sig_dly;
	
	always @ (posedge clk) begin
		if(reset)
			sig_dly <= 1'b0;
		else
			sig_dly <= signal;
	end
   
	assign pulse = signal & ~sig_dly;
	
endmodule
