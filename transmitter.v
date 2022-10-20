module transmitter
(
	input clock, reset, brdClk, TXEmpty, 
	input [31:0] DataIn,
	input [4:0] wordSize,
	input [1:0] mode0, mode1, mode2, mode3,
	input [1:0] cs_select,
	input cs0_enable, cs1_enable, cs2_enable, cs3_enable, chip_enable,
	input cs0_auto, cs1_auto, cs2_auto, cs3_auto,
	output reg sc0, sc1, sc2, sc3, tx, sysclk,
	input rx,
	output reg requestTXread,
	output reg requestRXwrite,
	output reg [31:0] DataOuttoRXFifo
);

	parameter Idle = 2'b00;
	parameter cs_assert = 2'b01;
	parameter TX_bits = 2'b10;
	
	reg[1:0] phase;
	reg last_brd;
	reg cs_auto;
	reg[4:0] counter;
	reg assertCS;
	reg debug;
						
always @ (*) begin

	case (cs_select)
	
		2'b00: cs_auto = cs0_auto;
		2'b01: cs_auto = cs1_auto;
		2'b10: cs_auto = cs2_auto;
		2'b11: cs_auto = cs3_auto;			
		
	endcase
	
end

	
always @ (*) begin

	sc0 = 1'b1;
	sc1 = 1'b1;
	sc2 = 1'b1;
	sc3 = 1'b1;
	
	if (chip_enable) begin
	
		case (cs_select)
		
			2'b00: begin
				if(cs0_auto == 1'b1 && phase == Idle) 
					sc0 = 1'b1;
				else if(cs0_auto == 1'b0 && cs0_enable == 1'b1)
					sc0 = 1'b0;
				else if(cs0_auto == 1'b1 && assertCS == 1'b1 && phase != Idle)
					sc0 = 1'b0;
			end
			
			2'b01: begin
				if(cs1_auto == 1'b1 && phase == Idle) 
					sc1 = 1'b1;
				else if(cs1_auto == 1'b0 && cs1_enable == 1'b1)
					sc1 = 1'b0;
				else if(cs1_auto == 1'b1 && assertCS == 1'b1 && phase != Idle)
					sc1 = 1'b0;
			end
			
			2'b10: begin
				if(cs2_auto == 1'b1 && phase == Idle) 
					sc2 = 1'b1;
				else if(cs2_auto == 1'b0 && cs2_enable == 1'b1)
					sc2 = 1'b0;
				else if(cs2_auto == 1'b1 && assertCS == 1'b1 && phase != Idle)
					sc2 = 1'b0;
			end
			
			2'b11: begin
				if(cs3_auto == 1'b1 && phase == Idle) 
					sc3 = 1'b1;
				else if(cs3_auto == 1'b0 && cs3_enable == 1'b1)
					sc3 = 1'b0;
				else if(cs3_auto == 1'b1 && assertCS == 1'b1 && phase != Idle)
					sc3 = 1'b0;
			end
		endcase
	end

end

always @ (*) begin
	
	if(phase == 2'b00) begin
	
		if(cs_select == 2'b00)
			sysclk = mode0[1];
		else if (cs_select == 2'b01)
			sysclk = mode1[1];
		else if (cs_select == 2'b10)
			sysclk = mode2[1];
		else if (cs_select == 2'b11)
			sysclk = mode3[1];

	end
	
	else if(phase == 2'b10) begin
	
		tx = DataIn[counter];
	
		if(cs_select == 2'b00)
			sysclk = brdClk ^ mode0[0] ^ mode0[1];
		else if (cs_select == 2'b01)
			sysclk = brdClk ^ mode1[0] ^ mode1[1];
		else if (cs_select == 2'b10)
			sysclk = brdClk ^ mode2[0] ^ mode2[1];
		else if (cs_select == 2'b11)
			sysclk = brdClk ^ mode3[0] ^ mode3[1];
			
	end

end

always @ (posedge clock) begin

last_brd <= brdClk;

end

always @ (posedge clock) begin

	requestTXread <= 1'b0;
	requestRXwrite <= 1'b0;

	if (reset) begin
		phase <= Idle;
		requestTXread <= 1'b0;
		requestRXwrite <= 1'b0;
		counter <= 5'b0;
	end
	
	else if (last_brd != brdClk) begin
	
	
	 debug <= 1'b1;
	 
		if (chip_enable) begin	
		
			if (brdClk) begin
			
				case (phase)
					2'b00: begin
					
							assertCS <= 1'b0;
							if (TXEmpty) begin
								phase <= Idle;
							end
							else if (!TXEmpty && cs_auto) begin
								requestTXread <= 1'b1;
								phase <= cs_assert;
							end
							
					end
					2'b01: assertCS <= 1'b1;
					
					2'b10: begin
					
							if(counter >0) begin
								phase <= TX_bits;
							end
							
					end
					
				endcase
			end
			
			else begin
				
					if(phase == 2'b00) begin
					
							if (!TXEmpty && !cs_auto) begin
								phase <= TX_bits;
								requestTXread <= 1'b1;
								counter <= wordSize;
							end
					end
					if(phase == 2'b01) begin
							
							if (assertCS) begin
								phase <= TX_bits;
								counter <= wordSize;
							end
					end
							
					if(phase == 2'b10) begin
					
							if(counter > 0) begin
								DataOuttoRXFifo[counter] <= rx;
								counter <= counter -1'b1;
							end
							else if (counter == 0) begin
								phase <= Idle;
								DataOuttoRXFifo[counter] <= rx;
								requestRXwrite <= 1'b1;
							end
					end
					
			end
		end
		else
			phase <= Idle;
	end
end				

endmodule
	
	
	
	
	
	