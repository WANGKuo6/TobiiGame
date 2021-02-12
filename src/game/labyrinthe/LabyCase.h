#pragma once
#ifndef LABYRINTHEGAME_CASE_HPP
#define LABYRINTHEGAME_CASE_HPP

#include "../src/Button.h"


namespace game
{
	namespace laby
	{
		/// This class represent a case for the game called "LabyrintheGame"
		/// Each case is a button with a specific index. When the user "click" on a case,
		/// the game check if case clicked's index is the next to be clicked
		class Case : public core::fx::Button
		{
		public:

			/// Build a case with it's index and position (x and y)
			/// also provide the trigger time
			Case(int index, core::Window* window, core::MusicManager* musicManager, int x, int y, int triggerTime);
			~Case() = default;

			/// Return the index of the case
			int getIndex();
			

		private:

			/// Represent the index of the case (0 based)
			int index_;
		};
	}
}

#endif