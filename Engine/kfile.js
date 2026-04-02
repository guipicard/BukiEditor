const path = require('path');
const fs = require('fs');
let project = new Project('Engine');

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE")
project.isStaticLib = true;

project.addDefine("BUKI_EDITOR")

process.argv.push("--sdl2")
project.addProvider = function(proj, isRoot=false)
{
    // -- PATH FOR VLD
    // C:\Program Files (x86)\Visual Leak Detector\lib
    // C:\Program Files (x86)\Visual Leak Detector\include
    // C:\Program Files (x86)\Visual Leak Detector\bin
    const sdl2 = process.argv.indexOf("--sdl2") >= 0;
    if (sdl2)
    {
        if (!isRoot)
            {
                proj.addDefine("USE_SDL")
                proj.addIncludeDir(path.resolve("./vendor/SDL2/include"))
            }
            fs.copyFileSync("./vendor/SDL2/lib/SDL2.dll", "./Deployment/SDL2.dll");
            fs.copyFileSync("./vendor/SDL2/lib/SDL2_image.dll", "./Deployment/SDL2_image.dll")
            fs.copyFileSync("./vendor/SDL2/lib/SDL2_ttf.dll", "./Deployment/SDL2_ttf.dll")
            fs.copyFileSync("./vendor/SDL2/lib/SDL2_mixer.dll", "./Deployment/SDL2_mixer.dll")
            proj.addLib("./vendor/SDL2/lib/SDL2")
            proj.addLib("./vendor/SDL2/lib/SDL2main")
            proj.addLib("./vendor/SDL2/lib/SDL2_image")
            proj.addLib("./vendor/SDL2/lib/SDL2_ttf")
            proj.addLib("./vendor/SDL2/lib/SDL2_mixer")
    }
};

project.addIncludeDir("./vendor/Visual Leak Detector 2019/include")
project.addLib("./vendor/Visual Leak Detector 2019/lib/Win64/vld")

project.addIncludeDir(path.resolve("./vendor/json-develop/single_include"))

project.addIncludeDir(path.resolve("./Engine/includes"))

project.addIncludeDir(path.resolve("./Editor/includes"))

project.addIncludeDir("./vendor/Box2D/include/box2d")

project.addFiles('src/**','includes/**')

resolve(project)