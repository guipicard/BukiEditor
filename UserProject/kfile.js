const path = require("path");
let project = new Project("UserProject");

project.setCStd("c17");
project.setCppStd("c++20");

project.kore = false;
project.isStaticLib = true;
project.addDefine("KINC_STATIC_COMPILE");

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
project.addIncludeDir("./vendor/glm");

project.addFile("src/**");
project.addFile("includes/**");

project.setDebugDir("../Deployment");

resolve(project);