#include "dfVOC.h"

#include "dfFileSystem.h"
#include "../alEngine/alSound.h"

#pragma pack(push)
#pragma pack(1)
struct vocHeader {
	char magic[19];
	char eof[3];
	short version;
	short validation;
};

enum vocType {
	TERMINATOR = 0,
	SOUND_DATA = 1,
	SOUND_CONT = 2,
	SILENCE = 3,
	MARKER = 4,
	ASCII = 5,
	REPEAT = 6,
	END_REPEAT= 7
};

struct vocDataBlock {
	unsigned char type;
	unsigned char size[3];
	char info[1];
};

enum vocCodec {
	UNSIGNED_PCM_8 = 0x00,	// This is right now the only supported codec. Unsigned 8 bit values
	ADPCM_4_8 = 0x01,		// 4 bits to 8 bits Creative ADPCM
	ADPCM_3_8 = 0x02,		// 3 bits to 8 bits Creative ADPCM (AKA 2.6 bits)
	ADPCM_2_8 = 0x03,		// 2 bits to 8 bits Creative ADPCM
	SIGNED_PCM_16 = 0x04,	// 16 bits signed PCM
	ALAW = 0x06,			// Logarithmic PCM (using A-Law)
	ULAW = 0x07,			// Logarithmic PCM (using mU-Law)
	ADPCM_4_16 = 0x2000		// 4 bits to 16 bits Creative ADPCM (only valid in block type 0x09 BlockType.SOUND_DATA_NF)
};

struct vocSound {
	char type;
	unsigned char size[3];
	unsigned char sample_rate;
	unsigned char codec;
	char data[1];
};

struct vocSilence {
	char type;
	unsigned char size[3];
	unsigned short length;
	unsigned char sample_rate;
};

struct vocRepeat {
	char type;
	unsigned char size[3];
	unsigned short repetition;
};

struct vocMarker {
	char type;
	unsigned char size[3];
	unsigned short markerID;
};

struct vocCount {
	char type;
	unsigned char size[3];
	unsigned short count;
};
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
struct VocHeader
{
	unsigned char  desc[20];
	unsigned short datablockOffset;
	unsigned short version;
	unsigned short id;
};

struct VocBlockHeader
{
	unsigned char blocktype;
	unsigned char size[3];
	unsigned char sr;
	unsigned char pack;
};
#pragma pack(pop)

enum BlockType
{
	VOC_TERMINATOR = 0,
	VOC_SOUND_DATA = 1,
	VOC_SOUND_CONTINUE = 2,
	VOC_SILENCE = 3,
	VOC_MARKER = 4,
	VOC_ASCII = 5,
	VOC_REPEAT = 6,
	VOC_END_REPEAT = 7
};

enum VocCodec
{
	CODEC_8BITS = 0,
	CODEC_4BITS = 1,
	CODEC_2_6BITS = 2,
	CODEC_2_BITS = 3,
};

static void debug(unsigned char* buffer, int len)
{
	unsigned char* end = buffer + len;

	// Read the header.
	VocHeader* header = (VocHeader*)buffer;
	buffer += sizeof(VocHeader);

	// Parse blocks.
	while (buffer < end)
	{
		const BlockType type = BlockType(*buffer); buffer++;
		// Break if this is the final block.
		// TODO: Figure out what type = 170 means; for now abort.
		if (type == VOC_TERMINATOR || type > VOC_END_REPEAT) { break; }
		// All other blocks have a 3 byte size (up to 16MB).
		const unsigned long blockLen = buffer[0] | (buffer[1] << 8u) | (buffer[2] << 16u);
		buffer += 3;
		// Block parsing.
		switch (type)
		{
		case VOC_SOUND_DATA:
		{
			float sampleRate = 1000000 / (256 - (float)buffer[0]);
			unsigned char  codec = buffer[1];
			unsigned char* soundData = &buffer[2];
		} break;
		case VOC_SOUND_CONTINUE:
		{
			unsigned char* soundData = &buffer[2];
		} break;
		case VOC_SILENCE:
		{
			unsigned short silenceLen = *((unsigned short*)buffer);
			float sampleRate = 1000000 / (256 - (float)buffer[2]);
		} break;
		case VOC_MARKER:
		{
			unsigned short markerId = *((unsigned short*)buffer);
		} break;
		case VOC_ASCII:
		{
			// The string is ignored.
		} break;
		case VOC_REPEAT:
		{
			unsigned short repeatCount = *((unsigned short*)buffer);
		} break;
		case VOC_END_REPEAT:
		{
		} break;
		};
		// Move to the next block.
		buffer += blockLen;
	}
}

dfVOC::dfVOC(dfFileSystem* fs, const std::string& file)
{
	int size;
	char *data = fs->load(DF_SOUNDS_GOB, file, size);
	if (data == nullptr) {
		std::cerr << "dfVOC::dfVOC cannot load " << file << std::endl;
		return;
	}

	vocHeader* header = (vocHeader*)data;
	if (strncmp(header->magic, "Creative Voice File", 19) != 0) {
		std::cerr << "dfVOC::dfVOC incorrect VOC file " << file << std::endl;
		delete data;
		return;
	}

	vocDataBlock* current = (vocDataBlock*)((char *)data + sizeof(vocHeader));

	debug((unsigned char*)data, size);
	int position = sizeof(vocHeader);
	int start = 0;
	int len = sizeof(vocHeader);

	while (len < size && current->type != vocType::TERMINATOR && current->type < 8) {
		// All other blocks have a 3 byte size (up to 16MB).
		int blockLen = current->size[0] + current->size[1] * 256 + current->size[2] * 65536;

		switch (current->type) {
		case vocType::SOUND_DATA: {
			vocSound* sound = (vocSound*)current;
			if (sound->codec != vocCodec::UNSIGNED_PCM_8) {
				std::cerr << "dfVOC::dfVOC unsupported code " << file << std::endl;
				delete data;
				return;
			}

			m_sampleRate = 1000000 / (256 - (int)sound->sample_rate);

			int soundLen = blockLen - 2;	// remove the samplerate/codec
			m_pcm8.resize(start + soundLen);
			memcpy(&m_pcm8[start], &sound->data[0], soundLen);
			start += soundLen;
			break;
		}

		case vocType::SOUND_CONT:
			m_pcm8.resize(start + blockLen);
			memcpy(&m_pcm8[start], &current->info[0], blockLen);
			start += blockLen;
			break;

		case vocType::SILENCE:
			m_pcm8.resize(start + blockLen);
			memset(&m_pcm8[start], 128, blockLen);
			start += blockLen;
			break;

		case vocType::REPEAT: {
			vocRepeat* repeat = (vocRepeat*)current;
			m_repeat = repeat->repetition;
			break;
		}

		case vocType::END_REPEAT:
			break;

		case vocType::MARKER: {
			vocMarker* marker = (vocMarker*)current;
			m_markerID = marker->markerID;
			break;
		}

		}

		current = (vocDataBlock*)((char*)current + blockLen + 4);
		len += blockLen + 4;
	}

	delete data;
}

/**
 * build and return an openAL buffer
 */
alSound* dfVOC::sound(void)
{
	if (m_sound) {
		return m_sound;
	}

	m_sound = new alSound(AL_FORMAT_MONO8, (ALvoid *)&m_pcm8[0], m_pcm8.size(), m_sampleRate, m_repeat == 0Xffff);

	return m_sound;
}

dfVOC::~dfVOC()
{
	if (m_sound) {
		delete m_sound;
	}
}
