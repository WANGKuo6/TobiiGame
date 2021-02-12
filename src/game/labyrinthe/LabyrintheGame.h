#pragma once
#ifndef LABYRINTHEGAME_GAME_INCLUDED
#define LABYRINTHEGAME_GAME_INCLUDED


#include "../src/Game.h"
#include "../src/ratio.h"
#include "../src/coord.h"
#include "../src/Frame.h"

/// The game status
enum LabyrintheGameState
{
	LABY_WIN_GAME, /// This mean that the user have wined the current level
	LABY_LOSE_GAME, /// This mean that the user have loosed the current level
	LABY_PRE_GAME, /// This state represent the explication status where the game is explained
	LABY_INIT_GAME, /// This represent the initialization of the level according levelNb and leveling
	LABY_PLAY_GAME, /// In this state, the game display different textures according to the level and check the current status
	LABY_START_GAME
};

namespace game {

	/// The remember game package
	namespace lg {

		/// The class for the Labyrinthe game
		///This game is about no touch the bord of the texture. 
		/// You have different game status in this game (see RememberGameState).
		/// As you can see, this class implements the Game interface with the play method in order to be used by the program
		class LabyrintheGame : public core::Game
		{
		public:

			/// Build a game with all the needed classes (Window CursorMovementManager, ...)
			/// The game name is provided by the 'name' argument
			LabyrintheGame(core::Window* window, core::CursorMovementManager* cmm, std::string name, Config* config, core::MusicManager* musicManager);
			~LabyrintheGame();


			/// Implementation of the Game play
			void play() override;

			bool checkCollide();
		private:

			/// Init the position of the cases on the screen
			/// This initialize the 'Case's objects and a set of gameObject with a label inside representing the "numbered cases"
			/// Each cases is assigned to a random position and can't be over another
			//void initLabyrinthe();
			

			/// This check if the cursor has touch the bord of the texture or win. 
			/// If one has been, it check if the game is loosed or wined by comparing currentIndex with the case index
			void checkGameState();

			/// The sound to be played when the game is wined
			Mix_Chunk* winSound;
			/// The sound to be played when the game is loosed
			Mix_Chunk* loseSound;

			SDL_Renderer* renderer;

			/// The frame counter of the game
			core::FrameCounter frameCounter_;

			/// The previous size of the window (for resizing purposes)
			core::coord previousWindowSize_;


			/// The current level number
			int levelNb;

			/// The current difficulty index难度指数
			int leveling;

			/// The current index of the last cases clicked
			/// The next expected index to be clicked is currentIndex+1
			int currentIndex;

			/// Store if the used have failed the current level
			bool haveFailed;

			/// The current game state
			LabyrintheGameState gameState;
		};
	}
}

#endif // LABYRINTHEGAME_GAME_INCLUDED