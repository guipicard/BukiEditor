let project = new Project("Game")

project.setCStd('c17')
project.setCppStd('c++17')

project.kore = false

project.addFile("Game/**")

project.setDebugDir("Deployment")

project.addIncludeDir("./Engine/includes");

project.addIncludeDir("./Game/includes");
project.addIncludeDir("./Game/includes/Platformer/PlayerStates");
project.addIncludeDir("./Game/includes/Angrybird");
project.addIncludeDir("./Game/includes/Platformer");
project.addIncludeDir("./Game/includes/Shapes");
project.addIncludeDir("./vendor/json-develop/single_include");

project.flatten()

const engineproj = await project.addProject("./Engine")
engineproj.addProvider(engineproj, false)
engineproj.addProvider(project, true)
const boxproj = await project.addProject("./vendor/Box2D")
engineproj.addProvider(boxproj, true)
const imguiproj = await project.addProject("./vendor/imgui-docking")
engineproj.addProvider(imguiproj, false)
const editorproj = await project.addProject("./Editor")
engineproj.addProvider(editorproj, false)

engineproj.parent = null;
boxproj.parent = null;
imguiproj.parent = null;
editorproj.parent = null;

resolve(project)