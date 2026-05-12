let project = new Project("Box2D");

project.setCStd("c17");
project.setCppStd("c++20");

project.kore = false;
project.isStaticLib = true;
project.addDefine("KINC_STATIC_COMPILE");

project.addFiles("src/**");
project.addFiles("include/**");
project.addIncludeDir("./vendor/Box2D/include");

resolve(project);