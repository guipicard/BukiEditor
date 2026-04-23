const path = require('path');
const fs = require('fs');
let project = new Project('Engine');

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE")
project.isStaticLib = true;

project.addDefine("BUKI_EDITOR")

process.argv.push("--sdl3")
project.addProvider = function(proj, isRoot=false)
{
    // -- PATH FOR VLD
    // C:\Program Files (x86)\Visual Leak Detector\lib
    // C:\Program Files (x86)\Visual Leak Detector\include
    // C:\Program Files (x86)\Visual Leak Detector\bin



    const sdl2 = process.argv.indexOf("--sdl3") >= 0;
    if (sdl2)
    {
        if (!isRoot)
            {
                proj.addDefine("USE_SDL")
                proj.addIncludeDir(path.resolve("./vendor/SDL3-3.4.4/include"))
            }
            fs.copyFileSync("./vendor/SDL3-3.4.4/lib/x64/SDL3.dll", "./Deployment/SDL3.dll");
            // fs.copyFileSync("./vendor/SDL2/lib/SDL2_image.dll", "./Deployment/SDL2_image.dll")
            // fs.copyFileSync("./vendor/SDL2/lib/SDL2_ttf.dll", "./Deployment/SDL2_ttf.dll")
            fs.copyFileSync("./vendor/SDL3_mixer-devel-3.2.0-VC/SDL3_mixer-3.2.0/lib/x64/SDL3_mixer.dll", "./Deployment/SDL3_mixer.dll")
            proj.addLib("./vendor/SDL3-3.4.4/lib/x64/SDL3")
            proj.addLib("./vendor/SDL3_mixer-devel-3.2.0-VC/SDL3_mixer-3.2.0/lib/x64/SDL3_mixer")
            // proj.addLib("./vendor/SDL2/lib/SDL2main")
            // proj.addLib("./vendor/SDL2/lib/SDL2_image")
            // proj.addLib("./vendor/SDL2/lib/SDL2_ttf")
    }
};
// project.addIncludeDir("./vendor/Visual Leak Detector 2019/include")
// fs.copyFileSync("./vendor/Visual Leak Detector 2019/bin/Win64/vld_x64.dll", "./Deployment/vld_x64.dll");
// fs.copyFileSync("./vendor/Visual Leak Detector 2019/bin/Win64/vld_x64.pdb", "./Deployment/vld_x64.pdb");
// project.addLib("./vendor/Visual Leak Detector 2019/lib/Win64/vld")

project.addIncludeDir("./vendor/Box2D/include/box2d")

project.addIncludeDir(path.resolve("./vendor/json-develop/single_include"))

project.addIncludeDir("./vendor/glm")

project.addIncludeDir("./vendor/glad/include")

project.addIncludeDir("./vendor/stb-master")

project.addIncludeDir("./Engine/includes")
project.addIncludeDir("./Engine/includes/Core")
project.addIncludeDir("./Engine/includes/Platform")
project.addIncludeDir("./Engine/includes/Graphics")
project.addIncludeDir("./Engine/includes/Editor")
project.addIncludeDir("./Engine/includes/Runtime")

project.addFiles('src/**')
project.addFiles(path.resolve('./vendor/glad/src/**.c'))
project.addFiles('includes/**')
project.addFiles("includes/Core/**")
project.addFiles("includes/Platform/**")
project.addFiles("includes/Graphics/**")
project.addFiles("includes/Editor/**")
project.addFiles("includes/Runtime/**")

resolve(project)