const path = require('path');
const fs = require('fs');

let project = new Project('imgui-docking');

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE");
project.isStaticLib = true;

project.addFiles('*.h');
project.addFiles('*.cpp');

project.addIncludeDir('./vendor/imgui-docking/backends/')
project.addIncludeDir('./vendor/imgui-docking/')

project.addFile('backends/imgui.cpp')
project.addFile('backends/imgui.h')

project.addFile('backends/imgui_impl_sdl3.cpp')
project.addFile('backends/imgui_impl_sdl3.h')

project.addFile('backends/imgui_impl_opengl3.cpp')
project.addFile('backends/imgui_impl_opengl3.h')

project.addFile('backends/imgui_impl_opengl3_loader.h')

resolve(project);

//