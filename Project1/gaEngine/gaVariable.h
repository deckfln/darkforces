#pragma once

namespace GameEngine {
	namespace Variable {
		enum class Type {
			NONE,
			BOOL,
			INT32,
			FLOAT,
			VEC3,
			STRING,
			VAR,
			OBJECT,
			PTR
		};
	};
}