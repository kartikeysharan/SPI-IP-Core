module FIFO (
output reg[31:0] DataOut,																			//Data output
output Full, Empty, 
output OV,																	//Status outputs
output reg[3:0] ReadPtr,
output reg[3:0] WritePtr,
input [31:0] DataIn,																							//Data input
input Read, Write, Clock, Reset, ClearOV										//Control inputs
);
reg[31:0] Stack [15:0];
reg[4:0] PtrDiff;																								//Pointer difference	
reg OVFound;
reg OVReset;																				//Storage array	
assign Empty = (PtrDiff == 1'b0)?1'b1:1'b0;								//Empty?
assign Full = (PtrDiff >= 5'd16)?1'b1:1'b0;									//Full?
assign OV = (OVFound)?1'b1:1'b0;									//Overflow?
initial DataOut <= 32'b0;												//Clear data out buffer
initial ReadPtr <= 1'b0;												//Clear read pointer
initial WritePtr<= 1'b0;												//Clear write pointer
initial PtrDiff <= 1'b0;													//Clear pointer difference
initial OVReset <= 1'b0;
initial OVFound <= 1'b0;

always @ (*)
	begin
		if (Reset)
			OVFound <= 1'b0;
		else if (OVReset)
			OVFound <= 1'b1;
		else if (ClearOV)
		begin
			if(OVFound)
				OVFound <= 1'b0;
		end
	end
	
always @ (posedge Clock, posedge Reset) begin		//Data transfers

	if (Reset) begin																	//Test for Clear
					ReadPtr <= 1'b0;												//Clear read pointer
					WritePtr <= 1'b0;												//Clear write pointer
					PtrDiff <= 1'b0;													//Clear pointer difference
					OVReset <= 1'b0;
	end
	else begin														//Begin read or write operations
	
			if(OVReset)
				OVReset <= 1'b0;
				
			if (Read && !Empty) begin
				DataOut = Stack [ReadPtr];	//Transfer data to output
				ReadPtr <= ReadPtr + 1'b1;		//Update read pointer
				PtrDiff <= PtrDiff - 1'b1;				//update pointer difference
			end
			
			if(Write) begin
			
				if (PtrDiff <= 5'd15) begin
					Stack [WritePtr] = DataIn;		//If not full store data in stack
					WritePtr <= WritePtr + 1'b1;		//Update write pointer
					PtrDiff <= PtrDiff + 1'b1;				//Update pointer difference
				end
				
				else if (Full) begin 						//Check for full
					if (!OV) 								//Check for OV
						Stack [WritePtr] = DataIn;		//If full no ov store data in stack
					OVReset <= 1'b1;
				end
				
			end
			
	end
end
endmodule
