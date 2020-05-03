#include "dfWAX.h"

#include "dfFileGOB.h"
#include "dfFileSystem.h"
#include "dfFME.h"

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
	m_name(name)
{
	m_data = fs->load(DF_SPRITES_GOB, name);
	if (m_data == nullptr) {
		std::cerr << "dfWAX::dfWAX cannot load " << name << std::endl;
		return;
	}

	_dfWaxTable* table = (_dfWaxTable*)m_data;

	m_states.resize(32);

	for (auto i = 0; i < 32; i++) {
		if (table->WAXES[i] > 0) {
			_dfWaxState* state = (_dfWaxState*)((unsigned char*)m_data + table->WAXES[i]);

			dfWaxAngles* angles = new dfWaxAngles;
			angles->m_animations.resize(32);
			m_states[i] = angles;

			// extract angles
			for (auto j = 0; j < 32; j++) {
				if (state->SEQS[j] > 0) {

					//extract frames
					_dfWaxSequence* seq = (_dfWaxSequence*)((unsigned char*)m_data + state->SEQS[j]);

					dfWaxAnimation* animations = new dfWaxAnimation;
					animations->m_frames.resize(32);
					angles->m_animations[j] = animations;

					for (auto k = 0; k < 32; k++) {
						if (seq->FRAMES[k] > 0) {

							// check for clones
							int offset = seq->FRAMES[k];
							dfFME* frame = nullptr;
							for (auto l = 0; l < k; l++) {
								if (seq->FRAMES[l] == offset) {
									// record a duplicate
									frame = animations->m_frames[k];
									frame->references(1);	// increase number of references
									break;
								}
							}

							if (frame == nullptr) {
								// extract new frame
								frame = new dfFME(m_data, seq->FRAMES[k], palette, true);
							}

							animations->m_frames[k] = frame;
						}
					}
				}
			}
		}
	}
}

dfWAX::~dfWAX()
{
	delete m_data;

	for (auto i = 0; i < 32; i++) {
		dfWaxAngles* angles = m_states[i];
		if (angles != nullptr) {

			// delete angles
			for (auto j = 0; j < 32; j++) {
				dfWaxAnimation* animations = angles->m_animations[j];
				if (animations != nullptr) {

					// delete frames
					for (auto k = 0; k < 32; k++) {
						dfFME* frame = animations->m_frames[k];
						if (frame != nullptr) {
							if (frame->references(-1) == 0) {
								delete frame;
							}
						}
					}
				}
			}
		}
	}
}
