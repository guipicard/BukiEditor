const path = require("path");
let project = new Project("Editor");

project.setCStd("c17");
project.setCppStd("c++17");
project.kore = false;

project.addDefine("KINC_STATIC_COMPILE");
project.addDefine("BUKI_EDITOR");

project.addIncludeDir("./Editor/includes");
project.addIncludeDir("./vendor/imgui-docking");
project.addIncludeDir("./vendor/imgui-docking/backends");
project.addIncludeDir("./vendor/SDL3-3.4.4/include");
project.addIncludeDir("./vendor/glm");
project.addIncludeDir("./vendor/json-develop/single_include");

project.addIncludeDir("./Engine/includes");
project.addIncludeDir("./Engine/includes/Core");
project.addIncludeDir("./Engine/includes/Platform");
project.addIncludeDir("./Engine/includes/Graphics");

project.addFile("src/*.cpp");
project.addFile("includes/*.h");

project.setDebugDir("../Deployment");

resolve(project);