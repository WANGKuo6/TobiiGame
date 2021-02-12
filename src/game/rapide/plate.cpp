#include "plate.h"
#include "../src/Constants.h"


game::rpg::Plate::Plate(int index, core::Window* window, core::MusicManager* musicManager, int x, int y, int triggerTime,std::string selector) :
	PlateBasic(window, musicManager, triggerTime, " ", 32, BLACK_COLOR, "ressources/games/remember/NumberSprites.png", selector,{ x, y, 100, 100 }),
	passedTime_(5000),
	index_(index)
{
	isPassedTime_ = false;
	
	SDL_Rect cropRect = { 0, 3 * 250, 250, 250 };
	setCropRect(cropRect);
	
}
bool game::rpg::Plate::isPassedStayTime()
{
	if (!is_active_)
	{
		return false;
	}

	if (passedTime_.isTimePassed()) {
		passedTime_.reset();
		isPassedTime_ = true;
	}

	return isPassedTime_;
}
		

///  The plate will shrink each time by 1/speed itself 
void game::rpg::Plate::autoShrink(core::coord cursorPosition,int speed) {
	if (speed <= 0) {
		speed = 5; 
	}
	SDL_Rect origin_size = getImageRect();
	SDL_Rect label_rect_resize = { origin_size.x + origin_size.w / (2* speed),
		origin_size.y + origin_size.h / (2* speed),
		origin_size.w - origin_size.w / speed,
		origin_size.h - origin_size.h / speed };
	setImageRect(label_rect_resize);
	window_->updateTexturePosition(texture_, cropRect_, &image_rect_);
}
void game::rpg::Plate::selected() {
	updateSelector("ressources/SelectedGameButtonBackground.png");
	SDL_Rect position = getImageRect();
	window_->updateTexturePosition(selector_, &position);

}

int game::rpg::Plate::getIndex()
{
	return index_;
}
