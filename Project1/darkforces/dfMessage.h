#pragma once

/**
 * List of messages
 */
namespace DarkForces {
	enum Message {
		TRIGGER = 1024,
		GOTO_STOP,
		DONE,
		ADD_SHIELD,
		ADD_ENERGY,
		HIT_BULLET,			// the object was hit by a bullet
		END_LOOP,			// the animation loop stopped
		DYING,				// the object is dying but not yet dead
		DEAD,				// the object is dead
		STATE,				// change the state of the object
		PICK_RIFLE_AND_BULLETS,
		ADD_BATTERY,
		EVENT,				// send events to sectors
		FIRE,				// fire a bullet from a gun
		START_FIRE,			// pull the trigger
		STOP_FIRE,			// stop firing
		SatNav_Wait,		// ticks for the DF satnav
		ANIM_START,			// WAX animation is starting
		ANIM_NEXT_FRAME,	// WAX animation start the next frame
		ANIM_END			// WAX animation reached the end and is stopping
	};

	enum MessageEvent {
		CROSSLINE_FRONT = 1,	// Cross line from front side 
		CROSSLINE_BACK = 2,		// Cross line from back side 
		ENTER_SECTOR = 4,		// Enter sector
		LEAVE_SECTOR = 8,		// Leave sector 
		NUDGE_FRONT_INSIDE = 16,// Nudge line from front side / Nudge sector from inside 
		NUDGE_BACK_OUTSIE = 32,	// Nudge line from back side / Nudge sector from outside 
		EXPLOSION = 64,			// Explosion 
		SHOOT = 128,			// Shoot or punch line(see entity_mask) 
		LAND = 512				// Land on floor of sector
	};

	enum class Keys {
		NONE = 0,
		RED = 1,
		BLUE = 2,
		YELLOW = 4
	};
}
