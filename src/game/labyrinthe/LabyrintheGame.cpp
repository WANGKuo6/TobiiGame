#include "LabyrintheGame.h"
#include "../src/Button.h"
#include "../src/Utils.h"
#include "../src/InputManager.h"
#include "../src/Constants.h"
#include "../src/GazeObject.h"
#include "../src/ratio.h"

game::lg::LabyrintheGame::LabyrintheGame(core::Window* window, core::CursorMovementManager* cmm, std::string name, Config* config, core::MusicManager* musicManager) :
	Game(window, cmm, name, config, musicManager),
	frameCounter_(config->get_int("game_remember_frame_rate", -1)),
	previousWindowSize_({ window_->get_width(), window_->get_height() }),
	levelNb(1),
	currentIndex(-1),
	haveFailed(false),
	gameState(LABY_PRE_GAME)
{
	
	winSound = musicManager->loadSFX("ressources/games/Labyrinthe/winSound.wav");
	loseSound = musicManager->loadSFX("ressources/games/Labyrinthe/loseSound.mp3");
}

game::lg::LabyrintheGame::~LabyrintheGame()
{
}


void game::lg::LabyrintheGame::play()
{
	logger_->info("Play game {}", getName());

	core::fx::Button preGameSkip(window_, musicManager, config_->get_int("button_trigger_default", 1000), "Passer >", 36, WHITE_COLOR, "ressources/buttonBackground.png", { 320, 100, 180, 70 });
	core::fx::Button continueButton(window_, musicManager, config_->get_int("button_trigger_default", 1000), "Continuer", 36, WHITE_COLOR, "ressources/buttonBackground.png", { 320, 100, 200, 70 });

	core::fx::Button placeStart(window_, musicManager, config_->get_int("button_trigger_default", 1000), "PositionStart", 36, WHITE_COLOR, "ressources/buttonBackground.png", { 0, 50, 200, 200 });
	core::TimeCounter startGameCounter(config_->get_int("game_Labyrinthe_start_counter", 6000));

	core::fx::Label timeToStartGameLabel(window_, "", 128, { window_->get_width()/2 - 16,200 }, { 0x29,0xb6,0xf6,0xFF });

	core::fx::Label pageTitle(window_, "Description", 64, { 20,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label levelLabel(window_, "Niveau", 64, { 20,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label levelNumberLabel(window_, "1", 64, { 240,20 }, { 0x21,0x21,0x21,0xFF });
	core::fx::Label timeCounterLabel(window_, "", 64, { 20,window_->get_height() - 100 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label endGameLabel(window_, "", 128, { window_->get_width() / 3,window_->get_height() / 3 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label gameName(window_, name_, 64, { 370,20 }, { 0x21,0x21,0x21,0xFF });
	core::fx::Label move(window_, "please move to this rectangle", 64, { 20,20 }, { 0x29,0xb6,0xf6,0xFF });
	core::fx::Label descriptionLabel(window_, core::utils::readText("ressources/games/Labyrinthe/description.txt"), 24, { 20,150 }, { 0x21,0x21,0x21,0xFF });
	core::fx::Button quitGame(window_, musicManager, config_->get_int("button_trigger_default", 1000), "X", 36, WHITE_COLOR, "ressources/buttonBackground.png", { 100,100, 65, 65 });
	core::InputManager* inputManager = core::InputManager::getInstance();
	core::coord cursorPos;
	core::GazeObject gaze_object(window_);
	

	gameState = LABY_PRE_GAME;
	haveFailed = false;

	statisticsManager->load();


	do
	{
		SDL_Rect rect = { 0, 300 + levelNb * 20, 100, 200 - levelNb * 20 };
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
		case LABY_WIN_GAME:
			continueButton.update(cursorPos);
			if (continueButton.isButtonPressed()) {
				levelNb = (levelNb) % 3 + 1;
				gameState = LABY_INIT_GAME;
			}
				

			
			endGameLabel.setContent("Victoire !", { 0x29,0xb6,0xf6,0xFF });
			endGameLabel.update(cursorPos);
			SDL_SetRenderDrawColor(window_->get_renderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);
			
			
			break;
		case LABY_LOSE_GAME:
			continueButton.update(cursorPos);
			if (continueButton.isButtonPressed())
				gameState = LABY_INIT_GAME;

			endGameLabel.setContent("Defaite ! :(", { 0xE5,0x39,0x35,0xFF });
			endGameLabel.update(cursorPos);
			SDL_SetRenderDrawColor(window_->get_renderer(), 255, 255, 255, SDL_ALPHA_OPAQUE);
			
			break;
		case LABY_PRE_GAME:
			leveling = 0;
			descriptionLabel.update(cursorPos);
			pageTitle.update(cursorPos);
			gameName.update(cursorPos);
			preGameSkip.update(cursorPos);
			// display the game description only if it's needed
			if (preGameSkip.isButtonPressed()) {
				gameState = LABY_INIT_GAME;
			}
			break;
		case LABY_INIT_GAME:
			SDL_SetRenderDrawColor(window_->get_renderer(), 255, 255, 255, 255);
			// reset current game
			//initCases
			logger_->info("{} level {} : Initialize new game", getName(), levelNb);
			//initLabyrinthe();
			currentIndex = -1;
			haveFailed = false;
			gameState = LABY_START_GAME;
			levelNumberLabel.setText(std::to_string(levelNb));
			startGameCounter.reset();
			break;
		case LABY_START_GAME:
			SDL_SetRenderDrawColor(window_->get_renderer(), 255, 255, 255, 255);
			SDL_RenderClear(window_->get_renderer());
			SDL_SetRenderDrawColor(window_->get_renderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
			move.update(cursorPos);
			SDL_RenderDrawRect(window_->get_renderer(), &rect);
			if (startGameCounter.isTimePassed()) {
				startGameCounter.reset();
				gameState = LABY_PLAY_GAME;
			}
			else {
				timeToStartGameLabel.setText(std::to_string(startGameCounter.getRemainingTime() / 1000));
				core::ratio ra;
				ra.x = 1;
				ra.y = 1;
				timeToStartGameLabel.update(cursorPos, { 0,0,0,0 }, ra);
				placeStart.update(cursorPos);
			}
			break;
		case LABY_PLAY_GAME:
			/* Select the color for drawing. It is set to red here. */
			
				SDL_SetRenderDrawColor(window_->get_renderer(), 255, 255, 255, 255);
				/* Clear the entire screen to our selected color. */
				SDL_RenderClear(window_->get_renderer());
				timeCounterLabel.setText(std::to_string(startGameCounter.getRemainingTime() / 1000));
				SDL_SetRenderDrawColor(window_->get_renderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
				

				int i = 0;
				int x1 = 340;
				int y1 = 550;
				
				for (i; i < window_->get_width(); i += 2) {
					int  x = x1 - 200 * sin(3.14*i / 180 / 5) + levelNb * 20;
					int y = y1 - 200 * sin(3.14*i / 180 / 5);
					SDL_SetRenderDrawColor(window_->get_renderer(), 150, 0, 0, SDL_ALPHA_OPAQUE);
					SDL_RenderDrawLine(window_->get_renderer(), i - 2, x, i, y);
				}
				if (!core::utils::isInside(cursorPos.x, cursorPos.y, x1, y1, levelNb))
				{
					gameState = LABY_LOSE_GAME;

					statisticsManager->increment("number_of_games", 1);
					statisticsManager->increment("level_loose", 1);
				}
				if (core::utils::isInside(cursorPos.x, cursorPos.y, x1, y1, levelNb) && cursorPos.x >= window_->get_width() - 50) {
					gameState = LABY_WIN_GAME;
					statisticsManager->increment("level_win", 1);
					statisticsManager->increment("number_of_games", 1);
				}

		

			levelLabel.update(cursorPos);
			levelNumberLabel.update(cursorPos);

			levelNumberLabel.setText(std::to_string(levelNb));

			break;
		}

		//Update gaze position
		gaze_object.update(cursorPos);

		// update the window when all the texture are displayed
		window_->update();

	} while (true);
	statisticsManager->flush();
}

	



	
