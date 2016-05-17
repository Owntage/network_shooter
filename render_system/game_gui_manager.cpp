#include "render_system.h"


void GameGuiManager::setWeaponUpdate(WeaponUpdate& weaponUpdate)
{
	this->weaponUpdate = weaponUpdate;
	if(weaponUpdate.state == WeaponUpdate::WeaponState::SHOOT)
	{
		shootBlink = 1.0f;
	}
}

void GameGuiManager::draw(sf::RenderTarget& renderTarget)
{

	//hp bar
	shape.setTexture(nullptr);
	sf::View targetView = renderTarget.getView();
	renderTarget.setView(view);
	shape.setSize(sf::Vector2f(160, 32));
	shape.setPosition(screenWidth / 2 - 164, -screenHeight / 2 + 4);
	shape.setFillColor(sf::Color(128, 0, 128, 255));
	renderTarget.draw(shape, sf::BlendAdd);

	//hp texture
	shape.setTexture(&getTexture(HP_TEXTURE));
	shape.setSize(sf::Vector2f(32, 32));
	shape.setFillColor(sf::Color::White);
	shape.setPosition(screenWidth / 2 - 200, -screenHeight / 2 + 4);
	renderTarget.draw(shape, sf::BlendAdd);

	//weapon texture
	shape.setTexture(&getTexture(weaponUpdate.weaponDef.weaponTexture));
	shape.setPosition(screenWidth / 2 - 200, -screenHeight / 2 + 40);
	renderTarget.draw(shape);

	//reload bar
	shape.setTexture(nullptr);
	float reloadValue = weaponUpdate.weaponData.timeSinceReload / weaponUpdate.weaponDef.reloadTime;
	reloadValue = std::min(reloadValue, 1.0f);
	shape.setSize(sf::Vector2f(160 * reloadValue, 16));
	shape.setPosition(screenWidth / 2 - 164, -screenHeight / 2 + 40);
	shape.setFillColor(sf::Color(0, 255, 255, 128));
	renderTarget.draw(shape, sf::BlendAdd);

	//bullets.
	float bulletSize = 160 - (weaponUpdate.weaponDef.bulletsPerHolder - 1) * 4;
	bulletSize /= weaponUpdate.weaponDef.bulletsPerHolder;
	int bullets = weaponUpdate.weaponDef.bulletsPerHolder - weaponUpdate.weaponData.shotsMade;
	shape.setSize(sf::Vector2f(bulletSize, 12));
	for(int i = 0; i < bullets; i++)
	{
		shape.setPosition(screenWidth / 2 - 164 + (bulletSize + 4) * i, -screenHeight / 2 + 60);
		renderTarget.draw(shape, sf::BlendAdd);
	}

	//blink
	shape.setSize(sf::Vector2f(screenWidth, screenHeight));
	shape.setPosition(-screenWidth / 2, -screenHeight / 2);
	shape.setTexture(nullptr);
	shape.setFillColor(sf::Color(192, 255, 255, 32.0f * shootBlink));
	renderTarget.draw(shape, sf::BlendAdd);
}

void GameGuiManager::onTimer()
{
	weaponUpdate.weaponData.timeSinceReload += 1.0f / 60.0f;
	weaponUpdate.weaponData.timeSinceShot += 1.0f / 60.0f;
	if(shootBlink > 0)
	{
		shootBlink -= 1.0f / 60.0f / weaponUpdate.weaponDef.period;
	}
}


sf::Texture& GameGuiManager::getTexture(std::string name)
{
	if(textures.find(name) == textures.end())
	{
		sf::Texture texture;
		texture.loadFromFile(name);
		textures[name] = texture;
	}
	return textures[name];
}
