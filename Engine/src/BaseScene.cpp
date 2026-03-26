#include "BaseScene.h"
#include "Engine.h"
#include "Entity.h"
#include "Text.h"

buki::BaseScene::BaseScene()
{
}

buki::BaseScene::~BaseScene()
{

}

buki::Entity* buki::BaseScene::Instantiate(const std::string _name)
{
	return buki::Engine::GetInstance().World().Create(_name);
}

void buki::BaseScene::OnStart()
{
}

void buki::BaseScene::OnStop()
{

}
