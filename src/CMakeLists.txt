cmake_minimum_required (VERSION 3.8)

add_executable(JumperBoy 
"jumper_boy.cpp" 
"gb.cpp"
"bus.cpp"
"cpu/cpu.cpp" 
"cpu/cpu_registers.cpp" 
"cpu/instructions.cpp"
"cpu/interrupt.cpp" 
"cpu/cpu_fetch.cpp"
"cpu/cpu_execute.cpp"
"cpu/debugging.cpp" 
"ram.cpp" 
"io/io.cpp" 
"io/timer.cpp" 
"io/joypad.cpp" 
"io/input_handler.cpp" 
"cartridge/cartridge.cpp" 
"cartridge/mbc_commons.cpp" 
"cartridge/mbc.cpp" 
"ppu/ppu.cpp" 
"ppu/ppu_pipeline.cpp"  
"ppu/pixel_transfer.cpp" 
"ppu/vmem.cpp" 
"ppu/lcd.cpp" 
"ppu/dma.cpp"
"apu/apu.cpp" 

"apu/apu_mixing.cpp"
"ui/user_interface.cpp" 
"ui/user_visuals.cpp" 
"ui/user_inputs.cpp" 
"../libs/tfd/tinyfiledialogs.cpp" 
   
  "apu/channel.cpp" 
  "apu/square_channel.cpp" 
  "apu/noise_channel.cpp" 
  "apu/wave_channel.cpp")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")

  target_link_libraries(JumperBoy ${SDL2_LIBRARIES})

elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")

  target_link_libraries(JumperBoy ${SDL2_LIBRARIES})

endif()
