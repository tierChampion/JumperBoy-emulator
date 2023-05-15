cmake_minimum_required (VERSION 3.8)

add_executable(JumperBoy 
"jumper_boy.cpp" 
"cartridge.cpp" 
"../libs/tfd/tinyfiledialogs.cpp" 
"bus.cpp"
"cpu/cpu.cpp" 
"cpu/instructions.cpp"
"gb.cpp"
"cpu/cpu_fetch.cpp"
"cpu/cpu_execute.cpp"
"cpu/cpu_registers.cpp" 
"ram.cpp" 
"ui/user_interface.cpp" 
"io/io.cpp" 
"cpu/interrupt.cpp" 
"io/joypad.cpp" 
"io/input_handler.cpp" 
"cpu/debugging.cpp" 
"io/timer.cpp" 
"apu/apu.cpp" 
"ppu/ppu.cpp" 
"ppu/vmem.cpp" 
"ppu/lcd.cpp" 
"ui/user_inputs.cpp" 
"ui/user_visuals.cpp" 
"ppu/dma.cpp"
"ppu/ppu_pipeline.cpp" "ppu/ppu_fifo.cpp" "ppu/pixel_transfer.cpp")

target_link_libraries(JumperBoy ${SDL2_LIBRARIES})

