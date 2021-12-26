#pragma once

/**
 * List of messages
 */
namespace DarkForces {
	enum Message {
		TRIGGER = 1024,
		GOTO_STOP,			// 1025: move to the next stop
		GOTO_STOP_FORCE,	// 1026: jump directly to the stop
		DONE,
		ADD_SHIELD,
		ADD_ENERGY,
		DYING,				// 1030: the object is dying but not yet dead
		DEAD,				// 1031: the object is dead
		STATE,				// 1032: change the state of the object unless it is already on that state
		FORCE_STATE,		// 1033: change the state of the object
		PICK_RIFLE_AND_BULLETS,
		ADD_BATTERY,
		EVENT,				// 1036: send events to sectors
		CHANGE_WEAPON,		// 1037: activate a new weapon
		FIRE,				// 1038: fire a bullet from a gun
		START_FIRE,			// pull the trigger
		STOP_FIRE,			// stop firing
		SatNav_Wait,		// ticks for the DF satnav
		SET_ANIM,			// define the display of an object
		ANIM_START,			// WAX animation is starting
		ANIM_NEXT_FRAME,	// WAX animation start the next frame
		ANIM_LASTFRAME,		// the animation loop reached the end
		ANIM_END,			// WAX animation reached the end and is stopping
		ROTATE,				// rotate objects in degrees
		ANIM_PAUSE,			// let the animation loop or not
		ANIM_VUE,			// load an animation VUE for 3D objects
		DROP_ITEM			// entity shall drop an item
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
