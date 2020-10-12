#include "dfWAX.h"

#include <algorithm>

#include "../dfFileGOB.h"
#include "../dfFileSystem.h"
#include "../dfFrame.h"

const float WAX_WORLDSIZE_X = 110.0f;
const float WAX_WORLDSIZE_Y = 110.0f;

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

// create runtime classes
const uint32_t g_wax_class = GameEngine::Model::m_modelClasses++;

dfWAX::dfWAX(dfFileSystem* fs, dfPalette* palette, const std::string& name) :
	dfModel(name, g_wax_class, true)
{
	int size;
	m_data = fs->load(DF_SPRITES_GOB, name, size);
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

			_dfWaxState* _state = (_dfWaxState*)((unsigned char*)m_data + table->WAXES[i]);

			dfWaxState* state = new dfWaxState;
			state->animations.resize(32);
			state->m_Wwidth = _state->Wwidth;
			state->m_Wheight = _state->Wheight;
			state->m_FrameRate = _state->FrameRate;

			m_states[i] = state;

			// extract all angles
			for (auto j = 0; j < 32; j++) {
				if (_state->SEQS[j] > 0) {
					int seq_offset = _state->SEQS[j];
					dfWaxAnimation* animation = nullptr;

					if (m_animations.count(seq_offset) > 0) {
						animation = m_animations[seq_offset];
					}
					else {
						m_animations[seq_offset] = animation = new dfWaxAnimation;
						animation->frames.resize(32);

						//extract all valid frames
						_dfWaxSequence* seq = (_dfWaxSequence*)((unsigned char*)m_data + seq_offset);

						animation->frames.resize(32);

						for (auto k = 0; k < 32; k++) {
							if (seq->FRAMES[k] > 0) {
								animation->m_nbframes++;
								int offset = seq->FRAMES[k];
								dfFrame* frame = nullptr;

								if (m_frames.count(offset) > 0) {
									frame = m_frames[k];
								}
								else {
									m_frames[offset] = frame = new dfFrame(m_data, seq->FRAMES[k], palette, this);

									// get the size and position of the sprite (max of all frame sizes)
									if (m_width < frame->m_width) {
										m_width = frame->m_width;
										m_insertX = frame->m_InsertX;
									}
									if (m_height < frame->m_height) {
										m_height = frame->m_height;
										m_insertY = frame->m_InsertY;
									}

									// maximum size (in pixel) of the current animation
									if (animation->m_size.x < frame->m_width) {
										animation->m_size.x = frame->m_width;
										animation->m_insert.x = (int)frame->m_InsertX;
									}
									if (animation->m_size.y < frame->m_height) {
										animation->m_size.y = frame->m_height;
										animation->m_insert.y = (int)frame->m_InsertY;
									}
								}

								animation->frames[k] = frame;
							}
						}
					}
					state->animations[j] = animation;

					// maximum size (in pixel) of the current state
					if (state->m_size.x < animation->m_size.x) {
						state->m_size.x = animation->m_size.x;
						state->m_insert.x = animation->m_insert.x;
					}
					if (state->m_size.y < animation->m_size.y) {
						state->m_size.y = animation->m_size.y;
						state->m_insert.y = animation->m_insert.y;
					}
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

	// resize the frames for the new target size
	for (auto frame : m_frames) {
		frame.second->targetSize(m_width, m_height);
	}

	// level side of the sprite depend on texture size (pixel) / 32 * (Wwidth / 65536)
	float widthFactor = WAX_WORLDSIZE_X / (m_Wwidth / 65536.0f);
	float heightFactor = WAX_WORLDSIZE_Y / (m_Wheight / 65536.0f);

	// create a model space AABB boundingbox for each state
	for (auto i = 0; i < m_nbStates; i++) {
		dfWaxState* state = m_states[i];

		glm::vec2 size_gl = glm::vec2(state->m_size.x / widthFactor, state->m_size.y / heightFactor);
		glm::vec2 insert_gl = glm::vec2(state->m_insert.x / widthFactor, (-state->m_insert.y - state->m_size.y - 1) / heightFactor);

		state->m_cylinder.set(
			-size_gl.x / 2, insert_gl.y, -size_gl.x / 2,
			size_gl.x / 2, size_gl.y + insert_gl.y, size_gl.x / 2);
	}

	// Update the object size
	// fix the position of the quad based on the insert_Y
	m_size_gl = glm::vec2(m_width / widthFactor, m_height / heightFactor);
	m_insert_gl = glm::vec2(m_insertX / widthFactor, (-m_insertY - m_height - 1) / heightFactor);

	updateBoundingBox();
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
void dfWAX::spriteModel(GLmodel &model, int id)
{
	SpriteModel* sm = &model.models[id];

	// fix the position of the quad based on the insert_Y
	sm->size = m_size_gl;
	sm->insert = m_insert_gl;

	sm->world = glm::vec2(0.5, 1);
	sm->states = glm::i16vec2(0, 0);

	// Compress the WAX tables into flat tables
	sm->states.x = model.stIndex;

	std::map<dfWaxAnimation*, int> anglesIndexes;

	for (auto state = 0; state < 32; state++) {
		if (m_states[state]) {
			model.indexes[model.stIndex++].x = model.atIndex;

			for (auto angle = 0; angle < 32; angle++) {
				dfWaxAnimation* wa = m_states[state]->animations[angle];
				if (wa) {
					if (anglesIndexes.count(wa) == 0) {
						anglesIndexes[wa] = model.ftIndex;

						model.indexes[model.atIndex++].y = model.ftIndex;

						for (auto frame = 0; frame < 32; frame++) {
							dfFrame* fme = wa->frames[frame];
							if (fme) {
								model.indexes[model.ftIndex++].z = fme->m_textureID;
							}
						}
					}
					else {
						model.indexes[model.atIndex++].y = anglesIndexes[wa];
					}
				}
			}
		}
	}

	dfModel::spriteModel(model, id);
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
	dfWaxState* angles = m_states[state];
	dfWaxAnimation* animation = angles->animations[0];	// animation of state XX viewed under angle 0

	if ((frame + 1 ) < animation->m_nbframes) {
		return frame + 1;	// next frame
	}

	return -1;	// cycle back to frame 0 but warn the object first
}

/**
 * get the specific AABB from a state
 */
const fwAABBox& dfWAX::bounding(dfState state)
{
	return m_states[(int)state]->m_cylinder;
}

dfWAX::~dfWAX()
{
	delete m_data;

	for (auto state : m_states) {
		delete state;
	}

	for (auto frames : m_frames) {
		delete frames.second;
	}

	for (auto animation : m_animations) {
		delete animation.second;
	}
}
