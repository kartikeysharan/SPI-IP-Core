// SPI IP Example
// SPI Verilog Implementation (spi.v)
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: DE1-SoC Board

// Hardware configuration:
// GPIO Port:
//   GPIO_1[31-0] is used as a general purpose GPIO port
// HPS interface:
//   Mapped to offset of 0 in light-weight MM interface aperature
//-----------------------------------------------------------------------------

module spi2 (clk, reset, address, byteenable, chipselect, writedata, readdata, write, read, cs0, cs1, cs2, cs3, rx, tx, clock, LED);

    // Clock, reset, and interrupt
    input   clk, reset;

    // Avalon MM interface (8 word aperature)
    input             read, write, chipselect;
    input [2:0]       address;
    input [3:0]       byteenable;
    input [31:0]      writedata;
    output reg [31:0] readdata;
	 output reg [9:0] LED;
    
    // spi interface
    input  rx;
	 output cs0, cs1, cs2, cs3, tx, clock;
	 wire rxfo, rxff, rxfe, txfo, txff, txfe;
	 reg[31:0] int_clr_ov;
	 reg clr_ov_tx;
	 reg clr_ov_rx;
	 
    // internal    
	 wire [31:0] data;
    reg [31:0] latch_data;
    reg [31:0] status;
    reg [31:0] control;
    reg [31:0] BRD;
    
    // register map
    // ofs  fn
    //   0  data (r/w)
    //   4  status (RXFO, RXFF, RXFE, TXFO, TXFF, TXFE)
    //   8  control 
    //  12  BRD (IBRD/FBRD)
    
    // register numbers
    parameter DATA_REG     = 2'b00;
    parameter STATUS_REG   = 2'b01;
    parameter CONTROL_REG  = 2'b10;
    parameter BRD_REG      = 2'b11;
    
    // read register
    always @ (*)
    begin
        if (read & chipselect)
            case (address)
                DATA_REG: 
                   readdata = data;
                STATUS_REG:
                    readdata = {25'b0, txfe, txff, txfo, rxfe, rxff, rxfo};
                CONTROL_REG: 
                    readdata = control;
                BRD_REG: 
                    readdata = BRD;
            endcase
        else
            readdata = 32'b1;
    end        

    // write register
    always @ (posedge clk or posedge reset)
    begin
        if (reset)
        begin
            latch_data[31:0] <= 32'b0;
            status <= 32'b0;
            control <= 32'b0;
            BRD <= 32'b0;
        end
        else
        begin
            if (write & chipselect)
            begin
                case (address)
                    DATA_REG: 
                        latch_data <= writedata;
                   STATUS_REG: 
                        int_clr_ov <= writedata;
                    CONTROL_REG: 
                        control <= writedata;
                    BRD_REG: 
                        BRD <= writedata;
                endcase
            end
				else
					int_clr_ov <= 32'b0;
        end
    end
	 
	 
	 // write to clear tx and clear rx
    always @ (posedge clk)
	 
	 begin
		if (int_clr_ov[3] == 1)
			clr_ov_tx = 1;
		else
			clr_ov_tx = 0;		
		
		if (int_clr_ov[0] == 1)
			clr_ov_rx = 1;
		else
			clr_ov_rx = 0;		
		
	 end		
	 
wire writetxfifo;
wire readtxfifo;
wire [31:0] txfifo2txserial;
wire [31:0] serial2rxfifo;
wire BRDclk;
wire readTXrequest;
wire writeRXrequest;
	 
BaudDivider baudgen 
(
	.clk(clk),
	.enable(control[15]),
	.reset(reset),
	.brd(BRD),
	.baud_out(BRDclk)
);

edgeDetector WriteTXfifo
(
	.reset(reset),
	.signal(write),
	.clk(clk),
	.pulse(writetxfifo)
);

edgeDetector ReadTXfifo
(
	.reset(reset),
	.signal(readTXrequest),
	.clk(clk),
	.pulse(readtxfifo)
);

edgeDetector WriteRXfifo
(
	.reset(reset),
	.signal(writeRXrequest),
	.clk(clk),
	.pulse(writerxfifo)
);

edgeDetector ReadRXfifo
(
	.reset(reset),
	.signal(read),
	.clk(clk),
	.pulse(readrxfifo)
);

assign Write = (write & chipselect & (address == DATA_REG) & writetxfifo);
assign Read = (read & chipselect & (address == DATA_REG) & readrxfifo);

FIFO txfifo
(
	.DataOut(txfifo2txserial), 
	.DataIn(writedata),
	.Full(txff),
	.Empty(txfe),
	.OV(txfo),
	.Read(readtxfifo), 
	.Write(Write),
	.Clock(clk),
	.Reset(reset), 
	.ClearOV(clr_ov_tx)
);

FIFO rxfifo
(
	.DataOut(data), 
	.DataIn(serial2rxfifo),
	.Full(rxff),
	.Empty(rxfe),
	.OV(rxfo),
	.Read(Read), 
	.Write(writerxfifo),
	.Clock(clk),
	.Reset(reset), 
	.ClearOV(clr_ov_rx)
);

transmitter serializer 
(
	.clock(clk), 
	.reset(reset), 
	.brdClk(BRDclk), 
	.TXEmpty(txfe), 
	.DataIn(txfifo2txserial),
	.DataOuttoRXFifo(serial2rxfifo),
	.wordSize(control[4:0]),
	.mode0(control[17:16]), 
	.mode1(control[19:18]),
	.mode2(control[21:20]), 
	.mode3(control[23:22]),
	.cs_select(control[14:13]),
	.cs0_enable(control[9]), 
	.cs1_enable(control[10]), 
	.cs2_enable(control[11]), 
	.cs3_enable(control[12]), 
	.chip_enable(control[15]),
	.cs0_auto(control[5]), 
	.cs1_auto(control[6]), 
	.cs2_auto(control[7]), 
	.cs3_auto(control[8]),
	.sc0(cs0), 
	.sc1(cs1), 
	.sc2(cs2), 
	.sc3(cs3), 
	.tx(tx), 
	.rx(rx),
	.sysclk(clock),
	.requestTXread(readTXrequest),
	.requestRXwrite(writeRXrequest)
);
endmodule
