#pragma once

enum class dfElevatorStatus {
	HOLD,		// elevator is not animated
	MOVE,		// is moving
	WAIT,		// is waiting at a stop
	TERMINATED	// the elevator cannot be activated anymore
};

enum class dfElevatorType {
	INV,		// moving up
	BASIC,		// moving down
	MOVE_FLOOR,
	CHANGE_LIGHT,
	MOVE_CEILING,
	MORPH_SPIN1,
	MORPH_MOVE1,
	MORPH_SPIN2,
	DOOR
};
