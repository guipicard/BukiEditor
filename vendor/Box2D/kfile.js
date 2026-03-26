const path = require('path');
const fs = require('fs');

let project = new Project('Box2D');

project.setCStd('c17')
project.setCppStd('c++17')

// project.addDefine("/experimental:c11atomics")

project.kore = false;
project.addDefine("KINC_STATIC_COMPILE");
project.isStaticLib = true;

project.addFiles('src/**');
project.addFiles('include/**');

project.addIncludeDir('./vendor/Box2D/include')

resolve(project);
