const path = require("path");
let project = new Project("Game");

project.setCStd("c17");
project.setCppStd("c++17");

project.kore = false;
project.isStaticLib = false;

project.addIncludeDir("./Game/includes");
project.addIncludeDir("./Game/includes/Shapes");
project.addIncludeDir("./Game/includes/AngryBird");
project.addIncludeDir("./Game/includes/Platformer");

project.addIncludeDir("./Engine/includes");
project.addIncludeDir("./Engine/includes/Core");
project.addIncludeDir("./Engine/includes/Platform");
project.addIncludeDir("./Engine/includes/Graphics");

project.addIncludeDir("./vendor/json-develop/single_include");

project.addFile("src/**");
project.addFile("includes/**");
project.addFile("includes/Shapes/**");
project.addFile("includes/Angrybird/**");
project.addFile("includes/Platformer/**");

project.addIncludeDir("./vendor/glm");

project.setDebugDir("../Deployment");

resolve(project);