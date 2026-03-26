let project = new Project('Box2D');

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE");
project.isStaticLib = true;

project.addFiles('src/**');
project.addFiles('include/**');

project.addIncludeDir('./Box2D/include')

project.flatten()

console.log("box2d")
resolve(project);

// https://github.com/Kode/kmake/blob/main/lib/kmake/Project.js#L3