let project = new Project("BukiEditor")

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false

project.addFile("Editor/**")

project.setDebugDir("Deployment")

project.addIncludeDir("../vendor/Box2D/include/box2d");
project.addIncludeDir("./Engine/includes");
project.addIncludeDir("./Editor/includes/Platformer/PlayerStates");
project.addIncludeDir("./json-develop/single_include");
project.addIncludeDir("./vendor/imgui-docking");

project.addIncludeDir("./Editor/includes");
project.addIncludeDir("./Editor/includes/Angrybird");
project.addIncludeDir("./Editor/includes/Platformer");
project.addIncludeDir("./Editor/includes/Shapes");


project.flatten()

const engineproj = await project.addProject("Engine")
engineproj.addProvider(project, true)
const boxproj = await project.addProject("./vendor/Box2D")
engineproj.addProvider(boxproj, true)
const imguiproj = await project.addProject("./vendor/imgui-docking")
engineproj.addProvider(imguiproj, false)

engineproj.parent = null;
boxproj.parent = null;
imguiproj.parent = null;

resolve(project)