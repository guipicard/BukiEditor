const path = require("path");
const fs = require("fs");

let project = new Project("Engine");

project.setCStd("c17");
project.setCppStd("c++20");

project.kore = false;
project.isStaticLib = true;

project.addDefine("KINC_STATIC_COMPILE");

function applySdl3(proj, isRoot = false) {
	if (!isRoot) {
		proj.addDefine("USE_SDL");
		proj.addIncludeDir(path.resolve("./vendor/SDL3-3.4.4/include"));
		proj.addIncludeDir(path.resolve("./vendor/SDL3_mixer-devel-3.2.0-VC/SDL3_mixer-3.2.0/include"));
        proj.addIncludeDir(path.resolve("./vendor/SDL3_ttf-devel-3.1.0-VC/SDL3_ttf-3.1.0/include"));
	}	
}

applySdl3(project, false);

project.addIncludeDir("./Engine/includes");
project.addIncludeDir("./Engine/includes/Components");
project.addIncludeDir("./Engine/includes/Components/Animation");
project.addIncludeDir("./Engine/includes/Core");
project.addIncludeDir("./Engine/includes/Graphics");
project.addIncludeDir("./Engine/includes/MetaData");
project.addIncludeDir("./Engine/includes/Platform");
project.addIncludeDir("./Engine/includes/Types");
project.addIncludeDir("./Engine/includes/Utils");

project.addIncludeDir("./vendor/box2d/include/box2d");
project.addIncludeDir("./vendor/glm");
project.addIncludeDir("./vendor/stb-master");
project.addIncludeDir("./vendor/json-develop/single_include");

project.addFiles("src/**");
project.addFiles("includes/**");

project.addIncludeDir("./vendor/glad/include");
project.addFile("../vendor/glad/src/glad.c");

project.setDebugDir("../Deployment");

resolve(project);

