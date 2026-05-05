const path = require("path");
let project = new Project("Editor");

project.setCStd("c17");
project.setCppStd("c++17");

project.kore = false;
project.isStaticLib = false;

project.addIncludeDir("./Editor/includes");
project.addIncludeDir("./vendor/imgui-docking");
project.addIncludeDir("./vendor/imgui-docking/backends");
project.addIncludeDir("./vendor/SDL3-3.4.4/include");
project.addIncludeDir("./vendor/glm");
project.addIncludeDir("./vendor/json-develop/single_include");

project.addIncludeDir("./UserProject/includes");

project.addIncludeDir("./Engine/includes");
project.addIncludeDir("./Engine/includes/Components");
project.addIncludeDir("./Engine/includes/Components/Animation");
project.addIncludeDir("./Engine/includes/Core");
project.addIncludeDir("./Engine/includes/Graphics");
project.addIncludeDir("./Engine/includes/MetaData");
project.addIncludeDir("./Engine/includes/Platform");
project.addIncludeDir("./Engine/includes/Types");
project.addIncludeDir("./Engine/includes/Utils");

project.addIncludeDir("./vendor/glad/include");
project.addFile("../vendor/glad/src/glad.c");

project.addFile("src/*.cpp");
project.addFile("includes/*.h");

project.setDebugDir("../Deployment");

resolve(project);