const path = require("path");
let project = new Project("Game");

project.setCStd("c17");
project.setCppStd("c++17");

project.kore = false;
project.isStaticLib = false;

project.addIncludeDir("./Game/includes");
project.addIncludeDir("./Engine/includes");
project.addIncludeDir("./vendor/json-develop/single_include");

project.addFile("src/main.cpp");
project.addFile("includes/SceneTest.h");
project.addFile("src/SceneTest.cpp");
project.addFile("includes/OpenGlTest.h");
project.addFile("src/OpenGlTest.cpp");
project.addFile("includes/AnimTest.h");
project.addFile("src/AnimTest.cpp");

project.addIncludeDir("./vendor/glm");

project.setDebugDir("../Deployment");



resolve(project);