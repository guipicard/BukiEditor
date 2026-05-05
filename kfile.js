const exeNames = {Game: "Game", Editor: "Editor"};
const startupProject = exeNames.Editor;
const editorDependencies = {
    Game: ["UserProject", "Engine", "Box2D"],
    Editor: ["UserProject", "Engine", "Box2D", "imgui-docking"]
}


process.on("exit", () => {
    const sln = "./build/BukiSolution.sln";

    setDefaultStartupProject(sln, startupProject);

    setProjectDependenciesInSln(sln, editorDependencies);
});

const path = require("path");
const fs = require("fs");

let project = new Project("BukiSolution");
project.kore = false;


const gameproj = await project.addProject("./Game");
const userProject = await project.addProject("./UserProject");
const editorproj = await project.addProject("./Editor");
const engineproj = await project.addProject("./Engine");
const boxproj = await project.addProject("./vendor/Box2D");
const imguiproj = await project.addProject("./vendor/imgui-docking");

gameproj.addIncludeDir("./vendor/Visual Leak Detector 2019/include");
userProject.addIncludeDir("./vendor/Visual Leak Detector 2019/include");
engineproj.addIncludeDir("./vendor/Visual Leak Detector 2019/include");
boxproj.addIncludeDir("./vendor/Visual Leak Detector 2019/include");
imguiproj.addIncludeDir("./vendor/Visual Leak Detector 2019/include");
editorproj.addIncludeDir("./vendor/Visual Leak Detector 2019/include");

gameproj.parent = null;
userProject.parent = null;
engineproj.parent = null;
boxproj.parent = null;
imguiproj.parent = null;
editorproj.parent = null;

project.setDebugDir("./Deployment");


fs.copyFileSync("./libs/SDL3.dll", "./Deployment/SDL3.dll");
fs.copyFileSync("./libs/SDL3.lib", "./Deployment/SDL3.lib");
fs.copyFileSync("./libs/SDL3.pdb", "./Deployment/SDL3.pdb");

fs.copyFileSync("./libs/SDL3.dll", "./build/SDL3.dll");
fs.copyFileSync("./libs/SDL3.lib", "./build/SDL3.lib");
fs.copyFileSync("./libs/SDL3.pdb", "./build/SDL3.pdb");

fs.copyFileSync("./libs/SDL3_mixer.dll", "./Deployment/SDL3_mixer.dll");
fs.copyFileSync("./libs/SDL3_mixer.lib", "./Deployment/SDL3_mixer.lib");
fs.copyFileSync("./libs/SDL3_mixer.pdb", "./Deployment/SDL3_mixer.pdb");

fs.copyFileSync("./libs/SDL3_mixer.dll", "./build/SDL3_mixer.dll");
fs.copyFileSync("./libs/SDL3_mixer.lib", "./build/SDL3_mixer.lib");
fs.copyFileSync("./libs/SDL3_mixer.pdb", "./build/SDL3_mixer.pdb");

fs.copyFileSync("./libs/SDL3_ttf.dll", "./Deployment/SDL3_ttf.dll");
fs.copyFileSync("./libs/SDL3_ttf.lib", "./Deployment/SDL3_ttf.lib");
fs.copyFileSync("./libs/SDL3_ttf.pdb", "./Deployment/SDL3_ttf.pdb");

fs.copyFileSync("./libs/SDL3_ttf.dll", "./build/SDL3_ttf.dll");
fs.copyFileSync("./libs/SDL3_ttf.lib", "./build/SDL3_ttf.lib");
fs.copyFileSync("./libs/SDL3_ttf.pdb", "./build/SDL3_ttf.pdb");

fs.copyFileSync("./libs/vld_x64.dll", "./Deployment/vld_x64.dll");
fs.copyFileSync("./libs/vld.lib", "./Deployment/vld.lib");
fs.copyFileSync("./libs/vld_x64.pdb", "./Deployment/vld_x64.pdb");

fs.copyFileSync("./libs/vld_x64.dll", "./build/vld_x64.dll");
fs.copyFileSync("./libs/vld.lib", "./build/vld.lib");
fs.copyFileSync("./libs/vld_x64.pdb", "./build/vld_x64.pdb");

editorproj.addLib(path.resolve("./vendor/SDL3-3.4.4/lib/x64/SDL3"));
editorproj.addLib(path.resolve("./vendor/SDL3_mixer-devel-3.2.0-VC/SDL3_mixer-3.2.0/lib/x64/SDL3_mixer"));
editorproj.addLib(path.resolve("./vendor/SDL3_ttf-devel-3.1.0-VC/SDL3_ttf-3.1.0/lib/x64/SDL3_ttf"));
editorproj.addLib(path.resolve("./build/x64/Debug/Box2D"));
editorproj.addLib(path.resolve("./build/x64/Debug/imgui-docking"));
editorproj.addLib(path.resolve("./build/x64/Debug/Engine"));
editorproj.addLib(path.resolve("./build/x64/Debug/UserProject"));
editorproj.addLib("opengl32");

gameproj.addLib(path.resolve("./vendor/SDL3-3.4.4/lib/x64/SDL3"));
gameproj.addLib(path.resolve("./vendor/SDL3_mixer-devel-3.2.0-VC/SDL3_mixer-3.2.0/lib/x64/SDL3_mixer"));
gameproj.addLib(path.resolve("./vendor/SDL3_ttf-devel-3.1.0-VC/SDL3_ttf-3.1.0/lib/x64/SDL3_ttf"));
gameproj.addLib(path.resolve("./build/x64/Debug/Box2D"));
gameproj.addLib(path.resolve("./build/x64/Debug/Engine"));
gameproj.addLib(path.resolve("./build/x64/Debug/UserProject"));

resolve(project)


function setDefaultStartupProject(slnPath, projectName) {
    if (!fs.existsSync(slnPath)) {
        return;
    }

    let text = fs.readFileSync(slnPath, "utf8");

    const projectBlockRegex = /^Project\([^\r\n]+\)\s=\s"([^"]+)",\s"([^"]+)",\s"([^"]+)"[\s\S]*?^EndProject\r?$/gm;

    const blocks = [];
    let match;

    while ((match = projectBlockRegex.exec(text)) !== null) {
        blocks.push({
            name: match[1],
            block: match[0]
        });
    }

    if (blocks.length === 0) {
        return;
    }

    const startupIndex = blocks.findIndex(b => b.name === projectName);
    if (startupIndex <= 0) {
        return;
    }

    const startupBlock = blocks[startupIndex];
    const reordered = [startupBlock, ...blocks.slice(0, startupIndex), ...blocks.slice(startupIndex + 1)];

    let i = 0;
    text = text.replace(projectBlockRegex, () => reordered[i++].block);

    fs.writeFileSync(slnPath, text, "utf8");

    console.log("startup project is: " + projectName)
}

function setProjectDependenciesInSln(slnPath, dependencyMap) {
    if (!fs.existsSync(slnPath)) {
        return;
    }

    let text = fs.readFileSync(slnPath, "utf8");

    const projectRegex = /^Project\([^\r\n]+\)\s=\s"([^"]+)",\s"([^"]+)",\s"\{([^}]+)\}"([\s\S]*?)^EndProject\r?$/gm;

    const guidByName = {};
    let match;

    while ((match = projectRegex.exec(text)) !== null) {
        guidByName[match[1]] = `{${match[3].toUpperCase()}}`;
    }

    text = text.replace(projectRegex, (full, name, relPath, guid, body) => {
        const deps = dependencyMap[name];
        if (!deps || deps.length === 0) {
            return `Project("{CE5840DA-FF32-437D-BF35-25392F23E3AA}") = "${name}", "${relPath}", "{${guid.toUpperCase()}}"${body}EndProject`;
        }

        const cleanedBody = body.replace(
            /\r?\n\tProjectSection\(ProjectDependencies\)\s=\spostProject[\s\S]*?\r?\n\tEndProjectSection/g,
            ""
        );

        let depLines = "";
        for (const depName of deps) {
            const depGuid = guidByName[depName];
            if (depGuid) {
                depLines += `\n\t\t${depGuid} = ${depGuid}`;
            }
        }

        const depSection =
            `\n\tProjectSection(ProjectDependencies) = postProject` +
            depLines +
            `\n\tEndProjectSection`;

        return `Project("{CE5840DA-FF32-437D-BF35-25392F23E3AA}") = "${name}", "${relPath}", "{${guid.toUpperCase()}}"${cleanedBody}${depSection}\nEndProject`;
    });

    text = text.replace(
        /Project\("[^"]+"\)\s=\s"BukiSolution",[\\s\\S]*?\r?\n\tProjectSection\(ProjectDependencies\)\s=\spostProject[\s\S]*?\r?\n\tEndProjectSection/g,
        (match) => match.replace(
            /\r?\n\tProjectSection\(ProjectDependencies\)\s=\spostProject[\s\S]*?\r?\n\tEndProjectSection/,
            ""
        )
    );

    fs.writeFileSync(slnPath, text, "utf8");
}