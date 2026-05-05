const path = require("path");
let project = new Project("Game");

project.setCStd("c17");
project.setCppStd("c++17");

project.kore = false;
project.isStaticLib = false;

project.addIncludeDir("./Game/includes");

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

project.addIncludeDir("./vendor/json-develop/single_include");

project.addFile("src/**");
project.addFile("includes/**");

project.addIncludeDir("./vendor/glm");

project.setDebugDir("../Deployment");

resolve(project);