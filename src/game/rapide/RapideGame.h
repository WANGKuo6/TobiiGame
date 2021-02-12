#pragma once
#ifndef RAPIDE_GAME_INCLUDED
#define RAPIDE_GAME_INCLUDED
#include "../src/Frame.h"
#include "../src/Game.h"
#include "../src/TimeCounter.h"
#include "plate.h"

/// The game status


namespace game {
	namespace rpg {
		enum RapideGameState
		{
			WIN_GAME, /// This mean that the user have wined the current level
			LOSE_GAME, /// This mean that the user have loosed the current level
			PRE_GAME, /// This state represent the explication status where the game is explained
			INIT_GAME, /// This represent the initialization of the level according levelNb and leveling
			START_GAME, /// In this state, the game display numberedCircles_
			PLAY_GAME /// In this state, the game display noNumberedCircles_ and check the current status
		};


		class RapideGame : public core::Game
		{
		public:
			RapideGame(core::Window* window, core::CursorMovementManager* cmm, std::string name, Config* config, core::MusicManager* musicManagerg);
			~RapideGame();
			void play() override;
			
			///This update the plates objects state's
			/// This is a for loop update on plates_
			void updatePlates(core::coord cursorPos);
			/// shrink all plates 
			void shrinkPlates(core::coord cursorPos);
			
			/// get a useable cord (x,y) for new plate
			void getCordForPlate(int* x,int* y);
			void createPlateOverTime();
		private:
			
			/// The plates objects. 
			core::fx::Frame<Plate> plates_;

			/// The diffculity level
			int levelNb;

			/// create a new plate each create_case_interval time 
			core::TimeCounter createPlateInterval;

			/// plates shrink for every shrinkTime
			core::TimeCounter shrinkTime;

			/// The number of cases objects in the noNumberedCircles_.
			int plateNb;

			///The number of cases has been created.
			int plateCreated;
			/// The current difficulty index
			int leveling;
			/// the number of plates that player has clicked 
			int clickNb;
			/// the number of plates that player has lost
			int loseNb;
			/// The sound to be played when the game is wined
			Mix_Chunk* winSound;
			/// The sound to be played when the game is loosed
			Mix_Chunk* loseSound;
			/// lose plate sound
			Mix_Chunk* losePlateSound;
			/// click plate sound
			Mix_Chunk* clickPlateSound;
			/// game states
			RapideGameState gameState;
		};

	}
}
#endif 