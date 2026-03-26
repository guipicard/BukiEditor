const path = require('path');
const fs = require('fs');
let project = new Project('Engine');

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE");
// project.addDefine("IMGUI_IMPL_OPENGL_ES2")
project.isStaticLib = true;

project.addIncludeDir("./vendor/Box2D/include/box2d");
project.addIncludeDir(path.resolve("./Engine/includes"));

project.addIncludeDir("./vendor/imgui-docking");
project.addIncludeDir('./vendor/imgui-docking/backends')

project.addProvider = function(proj, isRoot=false){
    // proj.addLib("C:/Program Files (x86)/Visual Leak Detector/lib/Win64/vld")
    // proj.addIncludeDir("C:/Program Files (x86)/Visual Leak Detector/include")
    // proj.addIncludeDir("./json-develop/single_include");
    proj.addLib("./vendor/Visual Leak Detector 2019/lib/Win64/vld")
    proj.addIncludeDir("./vendor/Visual Leak Detector 2019/include")
    proj.addIncludeDir("./vendor/json-develop/single_include");
    const sdl2 = true;//process.argv.indexOf("--sdl2") >= 0;
    if(sdl2){
        fs.copyFileSync("./vendor/SDL2/lib/SDL2.dll", "./Deployment/SDL2.dll");
        fs.copyFileSync("./vendor/SDL2/lib/SDL2_image.dll", "./Deployment/SDL2_image.dll");
        fs.copyFileSync("./vendor/SDL2/lib/SDL2_ttf.dll", "./Deployment/SDL2_ttf.dll");
        fs.copyFileSync("./vendor/SDL2/lib/SDL2_mixer.dll", "./Deployment/SDL2_mixer.dll");

        if(!isRoot){
            proj.addDefine("USE_SDL")
            proj.addIncludeDir(path.resolve("./vendor/SDL2/include"))
        }
        proj.addLib("./vendor/SDL2/lib/SDL2")
        proj.addLib("./vendor/SDL2/lib/SDL2main")
        proj.addLib("./vendor/SDL2/lib/SDL2_image")
        proj.addLib("./vendor/SDL2/lib/SDL2_ttf")
        proj.addLib("./vendor/SDL2/lib/SDL2_mixer")

        proj.addFile('/vendor/imgui-docking/*.h')
        proj.addFile('/vendor/imgui-docking/*.cpp')
        proj.addFile('/vendor/imgui-docking/backends/imgui.h')
        proj.addFile('/vendor/imgui-docking/backends/imgui.cpp')
        proj.addFile('/vendor/imgui-docking/backends/imgui_impl_sdl2.h')
        proj.addFile('/vendor/imgui-docking/backends/imgui_impl_sdl2.cpp')
        proj.addFile('/vendor/imgui-docking/backends/imgui_impl_opengl3.h')
        proj.addFile('/vendor/imgui-docking/backends/imgui_impl_opengl3.cpp')
        proj.addFile('/vendor/imgui-docking/backends/imgui_impl_opengl3_loader.h')
        proj.addFile('./vendor/imgui-docking/backends/imgui_impl_sdlrenderer2.cpp')
        proj.addFile('./vendor/imgui-docking/backends/imgui_impl_sdlrenderer2.h')
    }
    
};
project.addFiles('src/**','includes/**');

project.addProvider(project,false);

resolve(project);