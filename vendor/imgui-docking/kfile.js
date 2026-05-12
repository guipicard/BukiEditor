let project = new Project("imgui-docking");

project.setCStd("c17");
project.setCppStd("c++20");

project.kore = false;
project.isStaticLib = true;
project.addDefine("KINC_STATIC_COMPILE");

project.addIncludeDir("./vendor/imgui-docking");
project.addIncludeDir("./vendor/imgui-docking/backends");
project.addIncludeDir("./vendor/SDL3-3.4.4/include");

project.addFile("*.h");
project.addFile("*.cpp");
project.addFile("backends/imgui_impl_sdl3.h");
project.addFile("backends/imgui_impl_sdl3.cpp");
project.addFile("backends/imgui_impl_opengl3.h");
project.addFile("backends/imgui_impl_opengl3.cpp");
project.addFile("backends/imgui_impl_opengl3_loader.h");

resolve(project);