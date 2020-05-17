#include "dfWAX.h"

#include <algorithm>

#include "../dfFileGOB.h"
#include "../dfFileSystem.h"
#include "../dfFME.h"

enum {
	DF_WAX_ENEMY_MOVE = 0,
	DF_WAX_ENEMY_ATTACK = 1,
	DF_WAX_ENEMY_DIE_FROM_PUNCH = 2,
	DF_WAX_ENEMY_DIE_FROM_SHOT = 3,
	DF_WAX_ENEMY_LIE_DEAD = 4,
	DF_WAX_ENEMY_STAY_STILL = 5,
	DF_WAX_ENEMY_FOLLOW_PRIMARY_ATTACK = 6,
	DF_WAX_ENEMY_SECONDARY_ATTACK = 7,
	DF_WAX_ENEMY_FOLLOW_SECONDARY_ATTACK = 8,
	DF_WAX_ENEMY_JUMP = 9,
	DF_WAX_ENEMY_INJURED = 12,
	DF_WAX_ENEMY_SPECIAL = 13
};

enum {
	DF_WAX_REMOTE_MOVE = 0,
	DF_WAX_REMOTE_STAY_STILL = 1,
	DF_WAX_REMOTE_DIE = 2,
DF_WAX_REMOTE_DIE1 = 3
};

enum {
	DF_WAX_SCENERY_NORMAL = 0,
	DF_WAX_SCENERY_ATTACK = 1
};

enum {
	DF_WAX_BARREL_NORMAL = 0,
	DF_WAX_BARREL_EXPLODE = 1
};

#pragma pack(push)
struct _dfWaxTable {
	long Version;	// constant = 0x00100100 
	long Nseqs;		// number of SEQUENCES 
	long Nframes;	// number of FRAMES 
	long Ncells;	// number of CELLS 
	long Xscale;	// unused 
	long Yscale;	// unused 
	long XtraLight;	// unused 
	long pad4;		// unused 
	long WAXES[32];	// pointers to WAXES
					// = different action or state
};

struct _dfWaxState {
	long Wwidth;	// World Width 
	long Wheight;	// World Height 
	long FrameRate;	// Frames per second 
	long Nframes;	// unused = 0 
	long pad2;		// unused = 0 
	long pad3;		// unused = 0 
	long pad4;		// unused = 0 
	long SEQS[32];	// pointers to SEQUENCES 
					// = views from different angles in degrees 0, 11.25, 22.50....348.7
};

struct _dfWaxSequence {
	long pad1;		// unused = 0 
	long pad2;		// unused = 0 
	long pad3;		// unused = 0 
	long pad4;		// unused = 0 
	long FRAMES[32];// pointers to FRAMES
					// = the animation frames
};

struct _dfWaxFrame {
	long InsertX;	// Insertion point, X coordinate
					// Negative values shift the cell left
					// Positive values shift the cell right 
	long InsertY;	// Insertion point, Y coordinate
					// Negative values shift the cell up
					// Positive values shift the cell down 
	long Flip;		// 0 = not flipped
					// 1 = flipped horizontally 
	long Cell;		// pointer to CELL
					// = single picture 
	long UnitWidth;	// Unused 
	long UnitHeight;// Unused 
	long pad3;		// Unused 
	long pad4;		// Unuse
};
#pragma pack(pop)

dfWAX::dfWAX(dfFileSystem* fs, dfPalette* palette, std::string& name) :
	dfModel(name)
{
	if (m_name == "REDLIT.WAX") {
		printf("dfWAX::dfWAX\n");
	}
	m_data = fs->load(DF_SPRITES_GOB, name);
	if (m_data == nullptr) {
		std::cerr << "dfWAX::dfWAX cannot load " << name << std::endl;
		return;
	}

	// extract all existing states of the object
	_dfWaxTable* table = (_dfWaxTable*)m_data;

	m_states.resize(32);

	for (auto i = 0; i < 32; i++) {
		if (table->WAXES[i] > 0) {
			m_nbStates++;

			_dfWaxState* state = (_dfWaxState*)((unsigned char*)m_data + table->WAXES[i]);

			dfWaxAngles* angles = new dfWaxAngles;
			angles->animations.resize(32);
			angles->m_Wwidth = state->Wwidth;
			angles->m_Wheight = state->Wheight;
			angles->m_FrameRate = state->FrameRate;

			m_states[i] = angles;

			// extract all angles
			for (auto j = 0; j < 32; j++) {
				if (state->SEQS[j] > 0) {
					int seq_offset = state->SEQS[j];
					dfWaxAnimation* animation = nullptr;

					if (m_animations.count(seq_offset) > 0) {
						animation = m_animations[seq_offset];
					}
					else {
						m_animations[seq_offset] = animation = new dfWaxAnimation;
						animation->frames.resize(32);

						//extract all valid frames
						_dfWaxSequence* seq = (_dfWaxSequence*)((unsigned char*)m_data + seq_offset);

						animation = new dfWaxAnimation;
						animation->frames.resize(32);

						for (auto k = 0; k < 32; k++) {
							if (seq->FRAMES[k] > 0) {
								animation->m_nbframes++;
								int offset = seq->FRAMES[k];
								dfFME* frame = nullptr;

								if (m_frames.count(offset) > 0) {
									frame = m_frames[k];
								}
								else {
									m_frames[offset] = frame = new dfFME(m_data, seq->FRAMES[k], palette, true);
									this->m_insertX = frame->m_InsertX;
									this->m_insertY = frame->m_InsertY;
								}

								animation->frames[k] = frame;
							}
						}
					}
					angles->animations[j] = animation;

				}
			}
		}
	}

	// get the default world factor
	for (auto i = 0; i < m_nbStates; i++) {
		long mx = m_states[i]->m_Wwidth;
		long my = m_states[i]->m_Wheight;

		m_Wwidth = std::max(mx, m_Wwidth);
		m_Wheight = std::max(my, m_Wheight);
	}

	// get the size of the sprite (max of all frame sizes)
	for (auto frame : m_frames) {
		int mx = frame.second->m_width;
		int my = frame.second->m_height;

		m_width = std::max(mx, m_width);
		m_height = std::max(my, m_height);
	}

	// update the fram target size
	for (auto frame : m_frames) {
		frame.second->targetSize(m_width, m_height);
	}
}

/**
 * Extract all frames in the WAX
 */
void dfWAX::getFrames(std::vector<dfBitmapImage*>& frames)
{
	for (auto frame : m_frames) {
		frames.push_back(frame.second);
	}
}

/**
 * Return the textureId of the current frame
 */
int dfWAX::textureID(int state, int frame)
{
	return m_states[state]->animations[0]->frames[frame]->m_textureID;
}

/**
 * update the spriteModel based on the model
 */
void dfWAX::spriteModel(SpriteModel *sm)
{
	// level side of the sprite depend on texture size (pixel) / 32 * (Wwidth / 65536)
	float widthFactor = 58.0f / (m_Wwidth / 65536.0f);
	float heightFactor = 40.0f / (m_Wheight / 65536.0f);

	sm->size = glm::vec2(m_width / widthFactor, m_height / heightFactor);
	sm->insert = glm::vec2(m_insertX / widthFactor, -m_insertY / heightFactor);

	sm->world = glm::vec2(0.5, 1);
	sm->textureID.r = (float)m_states[0]->animations[0]->frames[0]->m_textureID;
}

/**
 * Return the frame rate of a state
 */
int dfWAX::framerate(int state)
{
	return m_states[state]->m_FrameRate;
}

/**
 * Return the next frame of the state
 */
int dfWAX::nextFrame(int state, unsigned int frame)
{
	dfWaxAngles* angles = m_states[state];
	dfWaxAnimation* animation = angles->animations[0];	// animation of state XX viewed under angle 0

	if ((frame + 1 ) < animation->m_nbframes) {
		return frame + 1;	// next frame
	}

	return 0;	// cycle back to frame 0
}

dfWAX::~dfWAX()
{
	delete m_data;

	for (auto frames : m_frames) {
		delete frames.second;
	}

	for (auto animation : m_animations) {
		delete animation.second;
	}
}
