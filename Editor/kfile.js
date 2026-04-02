let project = new Project("Editor")

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE");
project.isStaticLib = true;

project.addIncludeDir("./Engine/includes")
project.addIncludeDir("./Editor/includes")
project.addFiles('src/**','includes/**')

project.addIncludeDir('./vendor/imgui-docking/backends')
project.addIncludeDir("./vendor/imgui-docking");

project.addFile('./vendor/imgui-docking/*.h')
project.addFile('./vendor/imgui-docking/*.cpp')

// project.addFile('./vendor/imgui-docking/imgui.h')
// project.addFile('./vendor/imgui-docking/imgui.cpp')

project.addFile('./vendor/imgui-docking/backends/imgui_impl_sdl2.h')
project.addFile('./vendor/imgui-docking/backends/imgui_impl_sdl2.cpp')

project.addFile('./vendor/imgui-docking/backends/imgui_impl_opengl3.h')
project.addFile('./vendor/imgui-docking/backends/imgui_impl_opengl3.cpp')

project.addFile('./vendor/SDL2/include/SDL_opengl.h')
project.addFile('./vendor/SDL2/include/SDL_opengl.cpp')

project.addFile('./vendor/imgui-docking/backends/imgui_impl_opengl3_loader.h')

resolve(project);