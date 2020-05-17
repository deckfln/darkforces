#pragma once

enum dfDifficulty {
    DF_EASY,
    DF_MEDIUM,
    DF_HARD
};

/**
 * Status of the game
 */
class dfGame
{
    int m_difficulty = 0;
public:
    dfGame();
    int difficulty(void) { return m_difficulty; };
    ~dfGame();
};

extern dfGame Game;