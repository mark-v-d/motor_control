# result=trace_decode(serial, varargin)
#
# Decode data from trace port
#
# Protocol in ARM DDI 0403D ID021310
#
# result	data from trace_deserialize
# varargin	Optional data, sampled at srate
#
function result=trace_decode(serial, varargin)
	#
	# Detect all packets after a pause and start decoding from there
	#
	result=[];
	start=find(diff(serial.time)>10.5*serial.dt)+1;
	while length(start)
		next_start=[];
		for first_byte=unique(serial.data(start))'
			next_x=[];
			start_x=intersect(start,find(serial.data==first_byte));
			if first_byte==0
				[result, next_x]=...
					sync_packet(result,serial,start_x);
			elseif first_byte==0x05
				[result, next_x]=event_counter_packet(result,
					serial,start_x);
			elseif first_byte==0x0e
				[result, next_x]=exception_packet(result,
					serial,start_x);
			elseif first_byte==0x17
				[result, next_x]=PC_sample_packet(result,
					serial,start_x);
			elseif bitand(first_byte,4)
				[result, next_x]=data_trace_packet(result,
					serial,start_x);
			else
				[result, next_x]=itm_packet(result,serial,...
					start_x);
			end
			next_start=union(next_start,next_x);
			if 0 && length(next_start)>10
				n=next_start(1:10)
			end
		end
		start=next_start;
		start=start(find(start<length(serial.time)));
	end
	result=sort_struct(result);
	result=main_exception(result);

	#
	# Serial communication info
	#
	if isfield(serial,"q")
		result.serial.byte.time=serial.time;
		result.serial.byte.data=serial.data;
		result.serial.quality.time=serial.time;
		result.serial.quality.data=serial.q;
	else
		result=rmfield(result,"serial");
	end

	#
	# Scope data
	#
	for x=1:length(varargin)
		if !length(inputname(x+1))
			name=sprintf("%d",x);
		else
			name=sprintf("%s",inputname(x+1));
		end

		idx=[0,find(diff(double(varargin{x})))]+1;
		result.scope.(name).time=idx(:);
		result.scope.(name).data=varargin{x}(idx);
	end
endfunction

function [result next_start]=sync_packet(result,serial,start)
	valid=ones(size(start));
	next_start=start;
	while sum(valid) && next_start+1<length(serial.time)
		valid.*=serial.data(next_start)==0;
		valid.*=(serial.time(next_start+1)-serial.time(next_start))<...
			10.5*serial.dt;
		next_start+=valid;
	end

	valid.*=serial.data(next_start)==0x80;
	invalid=serial.time(next_start(find(!valid)));
	result.serial.error.time(end+1:end+length(invalid))=invalid;
	next_start=next_start(find(valid))+1;
	result.serial.sync.time(end+1:end+length(next_start))=...
		serial.time(next_start);
endfunction

function [result start data next_start]=validate_timing(result,serial,start)
	plen=single(bitand(serial.data(start(1)),3));
	if plen==0
		result.serial.empty.time(end+1:end+length(start))=...
			serial.time(start);
		n=min(length(serial.time),start+1);
		next_valid=(serial.time(n)-serial.time(start))...
			<10.5*serial.dt;
		next_start=start(find(next_valid))+1;
		data=[];
		return;
	elseif plen==3
		plen=4;
	end

	start=start(find(start+1+plen<length(serial.time)));

	# Validate packet timing and collect data
	valid=ones(size(start));
	next_start=start+valid;
	for x=0:plen-1
		valid.*=(serial.time(start+x+1)-serial.time(start+x))< ...
			10.5*serial.dt;
		next_start+=valid;
		data(x+1,:)=serial.data(start+x+1);
	end
	next_valid=(serial.time(start+plen+1)-serial.time(start+plen)) ...
		<10.5*serial.dt;
	next_start=next_start(find(next_valid.*valid));

	# ignored packets
	invalid=serial.time(start(find(!valid)));
	result.serial.error.time(end+1:end+length(invalid))=invalid;

	data=data(:,find(valid));
	data=typecast(data,sprintf("uint%d",8*plen));
	start=start(find(valid));
endfunction

function [result next_start]=event_counter_packet(result,serial,start)
	[result start data next_start]=validate_timing(result,serial,start);

	name={"CPICNT","EXCCNT","SLEEPCNT","LSUCNT","FOLDCNT","POSTCNT"};
	for x=1:5
		e=start(find(bitget(serial.data(start+1),x)));
		if(length(e))
			result.event.(name{x}).time(end+1:end+length(e))=e;
		end
	end
endfunction

function [result next_start]=exception_packet(result,serial,start)
	[result start data next_start]=validate_timing(result,serial,start);

	exception_number=unique(bitand(511,data))';
	for e=exception_number
		name=exception_name(e);
		index=find(bitand(511,data)==e);
		fun=(bitand(data(index),0x3000)/4096);
		fun=fun==1 | fun==3;
		result.exception.(name).time(end+1:end+length(index))=...
			serial.time(start(index));
		result.exception.(name).data(end+1:end+length(index))=fun;
	end
endfunction

function [result next_start]=itm_packet(result,serial,start)
	port=bitand(serial.data(start(1)),0xf8)/8;
	plen=bitand(serial.data(start(1)),3);

	[result start data next_start]=validate_timing(result,serial,start);
	if plen==0 || length(data)==0
		return;
	elseif plen==3
		plen=4;
	end

	pname=["q", num2str(port)];
	fname=sprintf("int%d",8*plen);
	result.itm.(fname).(pname).time(end+1:end+length(start))=...
		serial.time(start);
	result.itm.(fname).(pname).data(end+1:end+length(start))=data;

	if plen==4
		result.itm.float.(pname).time(end+1:end+length(start))=...
			serial.time(start);
		result.itm.float.(pname).data(end+1:end+length(start))=...
			typecast(data,"single");
	end
end

function [result next_start]=data_trace_packet(result,serial,start)
	# FIXME untested
	[result start data next_start]=validate_timing(result,serial,start);
	if !length(start)
		return;
	end

	packet_type=bitand(serial.data(start(1)),0xc0)/64;
	comparator=bitand(serial.data(start(1)),0x30)/16;
	plen=bitand(serial.data(start(1)),3);

	if plen==1
		data
	end

	if plen==0 || length(data)==0
		return;
	elseif plen==3
		plen=4;
	end

	pname=num2str(comparator);
	fname=sprintf("int%d",8*plen);
	result.trace.(fname).(pname).time(end+1:end+length(start))=...
		serial.time(start);
	result.trace.(fname).(pname).data(end+1:end+length(start))=data;

	if plen==4
		result.trace.float.(pname).time(end+1:end+length(start))=...
			serial.time(start);
		result.trace.float.(pname).data(end+1:end+length(start))=...
			typecast(data,"single");
	end
end

function [result next_start]=PC_sample_packet(result,serial,start)
	# FIXME untested
	[result start data next_start]=validate_timing(result,serial,start);
	if !length(start)
		return;
	end

	result.trace.PC.time(end+1:end+length(start))=...
		serial.time(start);
	result.trace.PC.data(end+1:end+length(start))=data;
end

function result=exception_name(num)
	irq_names={
	"Reset",
	"NMI",
	"HardFault",
	"MemManage",
	"BusFault",
	"UsageFault",
	"0",
	"0",
	"0",
	"0",
	"SVC",
	"DebugMon",
	"0",
	"PendSV",
	"SysTick",
	"WWDG",
	"PVD",
	"TAMPER_STAMP",
	"RTC_WKUP",
	"FLASH",
	"RCC",
	"EXTI0",
	"EXTI1",
	"EXTI2_TS",
	"EXTI3",
	"EXTI4",
	"DMA1_Channel1",
	"DMA1_Channel2",
	"DMA1_Channel3",
	"DMA1_Channel4",
	"DMA1_Channel5",
	"DMA1_Channel6",
	"DMA1_Channel7",
	"ADC1_2",
	"CAN1_TX",
	"CAN1_RX0",
	"CAN1_RX1",
	"CAN1_SCE",
	"EXTI9_5",
	"TIM1_BRK_TIM15",
	"TIM1_UP_TIM16",
	"TIM1_TRG_COM_TIM17",
	"TIM1_CC",
	"TIM2",
	"TIM3",
	"0",
	"I2C1_EV",
	"I2C1_ER",
	"0",
	"0",
	"SPI1",
	"0",
	"USART1",
	"USART2",
	"USART3",
	"EXTI15_10",
	"RTC_Alarm",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"TIM6_DAC1",
	"TIM7_DAC2",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"COMP2",
	"COMP4_6",
	"0",
	"HRTIM1_Master",
	"HRTIM1_TIMA",
	"HRTIM1_TIMB",
	"HRTIM1_TIMC",
	"HRTIM1_TIMD",
	"HRTIM1_TIME",
	"HRTIM1_FLT",
	"HRTIM1_TIMF",
	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"FPU"
	};

	# STM32G474
	irq_names={
		"Reset_Handler",
		"NMI_Handler",
		"HardFault_Handler",
		"MemManage_Handler",
		"BusFault_Handler",
		"UsageFault_Handler",
		"0",
		"0",
		"0",
		"0",
		"SVC_Handler",
		"DebugMon_Handler",
		"0",
		"PendSV_Handler",
		"SysTick_Handler",
		"WWDG",
		"PVD_PVM",
		"RTC_TAMP_LSECSS",
		"RTC_WKUP",
		"FLASH",
		"RCC",
		"EXTI0",
		"EXTI1",
		"EXTI2",
		"EXTI3",
		"EXTI4",
		"DMA1_Channel1",
		"DMA1_Channel2",
		"DMA1_Channel3",
		"DMA1_Channel4",
		"DMA1_Channel5",
		"DMA1_Channel6",
		"DMA1_Channel7",
		"ADC1_2",
		"USB_HP",
		"USB_LP",
		"FDCAN1_IT0",
		"FDCAN1_IT1",
		"EXTI9_5",
		"TIM1_BRK_TIM15",
		"TIM1_UP_TIM16",
		"TIM1_TRG_COM_TIM17",
		"TIM1_CC",
		"TIM2",
		"TIM3",
		"TIM4",
		"I2C1_EV",
		"I2C1_ER",
		"I2C2_EV",
		"I2C2_ER",
		"SPI1",
		"SPI2",
		"USART1",
		"USART2",
		"USART3",
		"EXTI15_10",
		"RTC_Alarm",
		"USBWakeUp",
		"TIM8_BRK",
		"TIM8_UP",
		"TIM8_TRG_COM",
		"TIM8_CC",
		"ADC3",
		"FMC",
		"LPTIM1",
		"TIM5",
		"SPI3",
		"UART4",
		"UART5",
		"TIM6_DAC",
		"TIM7_DAC",
		"DMA2_Channel1",
		"DMA2_Channel2",
		"DMA2_Channel3",
		"DMA2_Channel4",
		"DMA2_Channel5",
		"ADC4",
		"ADC5",
		"UCPD1",
		"COMP1_2_3",
		"COMP4_5_6",
		"COMP7",
		"HRTIM1_Master",
		"HRTIM1_TIMA",
		"HRTIM1_TIMB",
		"HRTIM1_TIMC",
		"HRTIM1_TIMD",
		"HRTIM1_TIME",
		"HRTIM1_FLT",
		"HRTIM1_TIMF",
		"CRS",
		"SAI1",
		"TIM20_BRK",
		"TIM20_UP",
		"TIM20_TRG_COM",
		"TIM20_CC",
		"FPU",
		"I2C4_EV",
		"I2C4_ER",
		"SPI4",
		"0",
		"FDCAN2_IT0",
		"FDCAN2_IT1",
		"FDCAN3_IT0",
		"FDCAN3_IT1",
		"RNG",
		"LPUART1",
		"I2C3_EV",
		"I2C3_ER",
		"DMAMUX_OVR",
		"QUADSPI",
		"DMA1_Channel8",
		"DMA2_Channel6",
		"DMA2_Channel7",
		"DMA2_Channel8",
		"CORDIC",
		"FMAC"
	};

	# XMC4400
	irq_names={
		"Reset_Handler",
		"NMI_Handler",
		"HardFault_Handler",
		"MemManage_Handler",
		"BusFault_Handler",
		"UsageFault_Handler",
		"0",
		"0",
		"0",
		"0",
		"SVC_Handler",
		"DebugMon_Handler",
		"0",
		"PendSV_Handler",
		"SysTick_Handler",
		"SCU_0_IRQHandler",
		"ERU0_0_IRQHandler",
		"ERU0_1_IRQHandler",
		"ERU0_2_IRQHandler",
		"ERU0_3_IRQHandler",
		"ERU1_0_IRQHandler",
		"ERU1_1_IRQHandler",
		"ERU1_2_IRQHandler",
		"ERU1_3_IRQHandler",
		"0",
		"0",
		"0",
		"PMU0_0_IRQHandler",
		"0",
		"VADC0_C0_0_IRQHandler",
		"VADC0_C0_1_IRQHandler",
		"VADC0_C0_2_IRQHandler",
		"VADC0_C0_3_IRQHandler",
		"VADC0_G0_0_IRQHandler",
		"VADC0_G0_1_IRQHandler",
		"VADC0_G0_2_IRQHandler",
		"VADC0_G0_3_IRQHandler",
		"VADC0_G1_0_IRQHandler",
		"VADC0_G1_1_IRQHandler",
		"VADC0_G1_2_IRQHandler",
		"VADC0_G1_3_IRQHandler",
		"VADC0_G2_0_IRQHandler",
		"VADC0_G2_1_IRQHandler",
		"VADC0_G2_2_IRQHandler",
		"VADC0_G2_3_IRQHandler",
		"VADC0_G3_0_IRQHandler",
		"VADC0_G3_1_IRQHandler",
		"VADC0_G3_2_IRQHandler",
		"VADC0_G3_3_IRQHandler",
		"DSD0_0_IRQHandler",
		"DSD0_1_IRQHandler",
		"DSD0_2_IRQHandler",
		"DSD0_3_IRQHandler",
		"DSD0_4_IRQHandler",
		"DSD0_5_IRQHandler",
		"DSD0_6_IRQHandler",
		"DSD0_7_IRQHandler",
		"DAC0_0_IRQHandler",
		"DAC0_1_IRQHandler",
		"CCU40_0_IRQHandler",
		"CCU40_1_IRQHandler",
		"CCU40_2_IRQHandler",
		"CCU40_3_IRQHandler",
		"CCU41_0_IRQHandler",
		"CCU41_1_IRQHandler",
		"CCU41_2_IRQHandler",
		"CCU41_3_IRQHandler",
		"CCU42_0_IRQHandler",
		"CCU42_1_IRQHandler",
		"CCU42_2_IRQHandler",
		"CCU42_3_IRQHandler",
		"CCU43_0_IRQHandler",
		"CCU43_1_IRQHandler",
		"CCU43_2_IRQHandler",
		"CCU43_3_IRQHandler",
		"CCU80_0_IRQHandler",
		"CCU80_1_IRQHandler",
		"CCU80_2_IRQHandler",
		"CCU80_3_IRQHandler",
		"CCU81_0_IRQHandler",
		"CCU81_1_IRQHandler",
		"CCU81_2_IRQHandler",
		"CCU81_3_IRQHandler",
		"POSIF0_0_IRQHandler",
		"POSIF0_1_IRQHandler",
		"POSIF1_0_IRQHandler",
		"POSIF1_1_IRQHandler",
		"HRPWM_0_IRQHandler",
		"HRPWM_1_IRQHandler",
		"HRPWM_2_IRQHandler",
		"HRPWM_3_IRQHandler",
		"CAN0_0_IRQHandler",
		"CAN0_1_IRQHandler",
		"CAN0_2_IRQHandler",
		"CAN0_3_IRQHandler",
		"CAN0_4_IRQHandler",
		"CAN0_5_IRQHandler",
		"CAN0_6_IRQHandler",
		"CAN0_7_IRQHandler",
		"USIC0_0_IRQHandler",
		"USIC0_1_IRQHandler",
		"USIC0_2_IRQHandler",
		"USIC0_3_IRQHandler",
		"USIC0_4_IRQHandler",
		"USIC0_5_IRQHandler",
		"USIC1_0_IRQHandler",
		"USIC1_1_IRQHandler",
		"USIC1_2_IRQHandler",
		"USIC1_3_IRQHandler",
		"USIC1_4_IRQHandler",
		"USIC1_5_IRQHandler",
		"0",
		"0",
		"0",
		"0",
		"0",
		"0",
		"LEDTS0_0_IRQHandler",
		"0",
		"FCE0_0_IRQHandler",
		"GPDMA0_0_IRQHandler",
		"0",
		"USB0_0_IRQHandler",
		"ETH0_0_IRQHandler",
		"0",
		"0"
	};


	if num==0
		result="main";
	elseif(num<=0 || num>length(irq_names))
		result=num2str(num);
	else
		result=irq_names{num};
	end
endfunction

function r=sort_struct(r)
	if isfield(r,"time") && isfield(r,"data")
		[r.time i]=sort(r.time);
		r.data=r.data(i);
	elseif isfield(r,"time")
		r.time=sort(r.time);
	elseif isstruct(r)
		for name=fieldnames(r)'
			r.(name{1})=sort_struct(r.(name{1}));
		end
	end
endfunction

function r=main_exception(r)
	if !isfield(r,"exception") || !isfield(r.exception,"main")
		return;
	end
	for n=setdiff(fieldnames(r.exception),"main")'
		x=r.exception.(n{1}).time;
		r.exception.main.time(end+1:end+length(x))=x;
		r.exception.main.data(end+1:end+length(x))=0;
	end
	[r.exception.main.time index]=sort(r.exception.main.time);
	r.exception.main.data=r.exception.main.data(index);
	x=find(diff(r.exception.main.data));
	index=unique([x x+1]);
	r.exception.main.time=r.exception.main.time(index);
	r.exception.main.data=r.exception.main.data(index);
endfunction
