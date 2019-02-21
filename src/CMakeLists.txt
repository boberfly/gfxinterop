add_executable( gfxinterop app.cpp app.h bgfx.cpp bgfx.h main.cpp tf.h tf.cpp fs_cubes.sc varying.def.sc vs_cubes.sc )
target_compile_definitions( gfxinterop PRIVATE VULKAN )
target_link_libraries( gfxinterop PUBLIC bx bgfx bimg glfw TFVulkan TFLinux TFImage TFCamera TFInput )
set_target_properties( gfxinterop PROPERTIES FOLDER "examples" )
target_include_directories( gfxinterop PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/The-Forge-Lite/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/The-Forge-Lite/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/The-Forge-Lite/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/The-Forge-Lite/include/Renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/The-Forge-Lite/src/Examples )

add_shader( vs_cubes.sc VERTEX   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders DX11_MODEL 5_0 GLSL 130 )
add_shader( fs_cubes.sc FRAGMENT OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders DX11_MODEL 5_0 GLSL 130 )

configure_debugging( gfxinterop WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR} )

install( TARGETS gfxinterop DESTINATION bin/ )
