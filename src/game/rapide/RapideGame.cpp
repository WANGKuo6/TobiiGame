#include "RapideGame.h"
#include "../src/MusicManager.h"
#include "../src/Window.h"
#include "../src/GazeObject.h"
#include "../src/InputManager.h"
#include "../src/Constants.h"
game::rpg::RapideGame::RapideGame(core::Window* window, core::CursorMovementManager* cmm, std::string name, Config* config, core::MusicManager* musicManager) :
	plates_(window, config, { 100, 150, 100, 100 }),
	levelNb(1),
	createPlateInterval(config->get_int("game_rapide_create_plate_interval", 2000)),
	plateNb(0),
	clickNb(0),
	loseNb(0),
	plateCreated(0),
	shrinkTime(200), //$(ProjectDir);
	gameState(PRE_GAME),
	Game(window, cmm, name, config, musicManager)
{
	winSound = musicManager->loadSFX("ressources/games/Rapide/winSound.wav");
	loseSound = musicManager->loadSFX("ressources/games/Rapide/loseSound.mp3");
	losePlateSound = musicManager->loadSFX("ressources/sound/passedSound.mp3");
	clickPlateSound = musicManager->loadSFX("ressources/games/Rapide/clickPlate.mp3");
}
void game::rpg::RapideGame::shrinkPlates(core::coord cursorPos)
{

	if (shrinkTime.isTimePassed()) {
		logger_->debug({ "plates start to shrink" });
		plates_.shrink(cursorPos, config_->get_int("game_rapide_plate_disapear_time", 500)/200);
		shrinkTime.reset();
	};

	plates_.update(cursorPos);
}

void game::rpg::RapideGame::getCordForPlate(int* x, int* y) {
	bool ok = false;
	while (!ok) {
		ok = true;
		*x = core::utils::randomInt(plates_.getProperties().x, plates_.getProperties().w);
		*y = core::utils::randomInt(plates_.getProperties().y, plates_.getProperties().h);

		// check position validity
		plateNb = plates_.getObjects().size();
		for (int j = 0; j < plateNb && ok; j++)
		{
			auto rect = plates_.getObjects()[j]->getImageRect();
			//logger_->info("case i {} x = {} y= {} w= {} h={}", j, rect.x,rect.y,rect.w,rect.h);
			if (core::utils::isIntersect(*x, *y, rect) || core::utils::isIntersect(*x + rect.w, *y + rect.h, rect)
				|| core::utils::isIntersect(*x + rect.w, *y, rect) || core::utils::isIntersect(*x, *y + rect.h, rect))
			{
				ok = false;
			}
		}
		if (!ok)
		{
			continue;
		}
	}

}
void game::rpg::RapideGame::play() {
	core::fx::Label levelLabel(window_, "level", 52, { 20,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label levelNumberLabel(window_, "0", 52, { 140,20 }, { 0x21,0x21,0x21,0xFF });
	core::fx::Label clickedPlateLabel(window_, "Click", 52, { 220,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label clickNumberLabel(window_, "0", 52, { 360,20 }, { 0x21,0x21,0x21,0xFF });
	core::fx::Label losePlateLabel(window_, "Lose", 52, { 440,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label loseNumberLabel(window_, "0", 52, { 580,20 }, { 0x21,0x21,0x21,0xFF });
	core::TimeCounter startGameCounter(config_->get_int("game_rapide_start_counter", 6000));
	core::fx::Label timeToStartGameLabel(window_, "", 128, { window_->get_width() / 2 - 16,200 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label endGameLabel(window_, "", 128, { window_->get_width() / 3,window_->get_height() / 3 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Button continueButton(window_, musicManager, config_->get_int("button_trigger_default", 1000), "Continuer", 36, { 255,255,255 }, "ressources/buttonBackground.png", { 320, 100, 200, 70 });
	core::fx::Button quitGame(window_, musicManager, config_->get_int("button_trigger_default", 1000), "X", 36, { 255,255,255 }, "ressources/buttonBackground.png", { 100,100, 65, 65 });
	core::fx::Label descriptionLabel(window_, core::utils::readText("ressources/games/rapide/description.txt"), 24, { 20,150 }, { 0x21,0x21,0x21,0xFF });
	core::fx::Label pageTitle(window_, "Description", 64, { 20,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Button preGameSkip(window_, musicManager, config_->get_int("button_trigger_default", 1000), "Passer >", 36, WHITE_COLOR, "ressources/buttonBackground.png", { 320, 100, 180, 70 });
	core::fx::Label gameName(window_, name_, 64, { 370,20 }, { 0x21,0x21,0x21,0xFF });
	core::InputManager* inputManager = core::InputManager::getInstance();
	core::GazeObject gaze_object(window_);
	core::coord cursorPos;
	gameState = PRE_GAME;

	statisticsManager->load();

	statisticsManager->increment("number_of_games", 1);


	do {

		cursorPos = cmm_->get_cursor_position();
		quitGame.update(cursorPos);
		bool quit = inputManager->quitRequested() || quitGame.isButtonPressed();

		if (quit)
		{
			logger_->info("Quit game {}", getName());
			break;
		}
		switch (gameState)
		{
		case PRE_GAME:
			descriptionLabel.update(cursorPos);
			pageTitle.update(cursorPos);
			gameName.update(cursorPos);
			preGameSkip.update(cursorPos);

			// display the game description only if it's needed
			if (preGameSkip.isButtonPressed()) {
				gameState = INIT_GAME;
			}

			break;
		case WIN_GAME:
			continueButton.update(cursorPos);
			if (continueButton.isButtonPressed()) {
				gameState = INIT_GAME;
				levelNb = (levelNb) % 5 + 1;
			}
			
			endGameLabel.setContent("Victoire !", { 0x29,0xb6,0xf6,0xFF });
			endGameLabel.update(cursorPos, { 0,0,0,0 }, plates_.getResizeRatio());
			break;
		case LOSE_GAME:
			continueButton.update(cursorPos);
			if (continueButton.isButtonPressed()) {
				gameState = INIT_GAME;
				levelNb = 1;
			}
		
			endGameLabel.setContent("Defaite !", { 0xE5,0x39,0x35,0xFF });
			endGameLabel.update(cursorPos);
			break;
		case INIT_GAME:
			// reset current game
			plateNb = 0;
			clickNb = 0;
			loseNb = 0;
			plates_.clearPlates();
			gameState = START_GAME;

			startGameCounter.reset();
			break;
		case START_GAME:
			// ask the user to watch a certain space
			if (startGameCounter.isTimePassed()) {
				startGameCounter.reset();
				gameState = PLAY_GAME;
				logger_->info("{} level {} : Visible phase passed", getName(), levelNb);
			}
			else {
				timeToStartGameLabel.setText(std::to_string(startGameCounter.getRemainingTime() / 1000));
				timeToStartGameLabel.update(cursorPos, { 0,0,0,0 }, plates_.getResizeRatio());
			}

			break;
		case PLAY_GAME:
			levelLabel.update(cursorPos);
			levelNumberLabel.update(cursorPos);

			clickedPlateLabel.update(cursorPos);
			clickNumberLabel.update(cursorPos);
			losePlateLabel.update(cursorPos);
			loseNumberLabel.update(cursorPos);

			updatePlates(cursorPos);
			clickNumberLabel.setText(std::to_string(clickNb));
			loseNumberLabel.setText(std::to_string(loseNb));
			levelNumberLabel.setText(std::to_string(levelNb));

			createPlateOverTime();
			shrinkPlates(cursorPos);

			break;

		
		}


		//Update gaze position
		gaze_object.update(cursorPos);
		window_->update();
	} while (true);

	statisticsManager->flush();
}

void game::rpg::RapideGame::updatePlates(core::coord cursorPos) {
	plates_.update(cursorPos);
	int res = plates_.checkObject();

	if (res == 0) {
		clickNb++;
		statisticsManager->increment("plates_pushed", 1);
		musicManager->playSFX(clickPlateSound);
	}
	else if (res == 1) {
		loseNb++;
		statisticsManager->increment("plates_loose", 1);
		musicManager->playSFX(losePlateSound);
	}
	if (clickNb >= levelNb * 5) {
		gameState = WIN_GAME;
		statisticsManager->increment("level_win", 1);
		statisticsManager->setMaxOf("max_level", levelNb);
		
		musicManager->playSFX(winSound);
	}
	if (loseNb >= 10 - levelNb) {
		gameState = LOSE_GAME;
		statisticsManager->increment("level_loose", 1);
		musicManager->playSFX(loseSound);
	}
}

void game::rpg::RapideGame::createPlateOverTime() {
	if (createPlateInterval.isTimePassed()) {
		int x = 0;
		int y = 0;
		getCordForPlate(&x, &y);
		logger_->debug("case{} created at x = {} y= {} ", plateCreated, x, y);

		auto plate = new Plate(plateCreated, window_, musicManager, x, y, config_->get_int("game_rapide_plate_trigger_time", 500), "ressources/GameButtonBackground.png");
		plateCreated++;
		plates_.add(plate);
		/// flush the number in  plates_
		plateNb = plates_.getSize();
		createPlateInterval.reset();
	}
}

game::rpg::RapideGame::~RapideGame()
{

}
