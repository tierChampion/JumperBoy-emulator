cmake_minimum_required (VERSION 3.8)

add_executable(JumperBoy 
"jumper_boy.cpp" 
"cartridge.cpp" 
"tinyfiledialogs.cpp" 
"bus.cpp"
"cpu.cpp" 
"instructions.cpp"
"gb.cpp" )

target_link_libraries(JumperBoy ${SDL2_LIBRARIES})

