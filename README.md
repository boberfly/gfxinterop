# gfxinterop

This is just an experiment with OpenGL + Vulkan interop. Actually it is using BGFX on the OpenGL side and The-Forge on the Vulkan side.

Currently it will launch BGFX cubes sample without a flag, and "--bgfxintf" will launch The-Forge's planets example. Eventually this will combine once I implement interop.

https://stackoverflow.com/questions/38907764/how-to-render-to-opengl-from-vulkan
https://github.com/jherico/Vulkan/tree/cpp/examples/glinterop
https://github.com/airlied/mesa/commit/6ce6914571af8d4934623d14b8de156d5f1279dd
https://www.khronos.org/registry/OpenGL/extensions/NV/NV_draw_vulkan_image.txt