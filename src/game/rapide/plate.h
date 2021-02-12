#ifndef RAPIDE_PLATE_HPP
#define RAPIDE_PLATE_HPP

#include "PlateBasic.h"
#include "../src/TimeCounter.h"
#include "../src/coord.h"
namespace game
{
	namespace rpg
	{
		/// This class represent a plate for the game called "Rapide"
		/// Each plate is a button with a specific index. When the user "click" on a case,the plate will disappear .
		/// and the plate will disappear automatically after a while
		class Plate : public core::fx::PlateBasic
		{
		public:

			/// Build a case with it's index and position (x and y)
			/// also provide the trigger time
			Plate(int index, core::Window* window, core::MusicManager* musicManager, int x, int y, int triggerTime,std::string selector);
			~Plate() =default;

			/// Return the index of the case
			int getIndex();
			bool isPassedStayTime(); 
			/// Automatically reduce the size of the plate on the screen
			void autoShrink(core::coord cursorPosition,int speed);
			/// update selector
			void selected();
		private:

			/// Represent the index of the case (0 based)
			int index_;
			/// 
			bool isPassedTime_;
			core::TimeCounter passedTime_;
		};
	}
}

#endif