#include "fwTextures.h"
#include <cassert>

#include "../include/stb_image.h"

fwTextures::fwTextures()
{
}

/**
 * create the textures from a list of files
 */
fwTextures::fwTextures(const int nbImages, const std::string* files) :
	m_nbImages(nbImages)
{
	myclass |= TEXTURES;
	m_images = new unsigned char*[nbImages]();

	int w, h, n;

	for (auto i = 0; i < nbImages; i++) {
		// load and generate the texture
		m_images[i] = stbi_load(files[i].c_str(), &w, &h, &n, 0);

		// check each image has the same size
		if (i == 0) {
			width = w;
			height = h;
			nrChannels = n;
		}
		else if (w != width || h != height || n != nrChannels) {
			std::cout << "fwTextures::fwTextures inconsistent size " << files[i] << std::endl;
			exit(-1);
		}
	}

	int size = width * height * nrChannels;
	m_data = new unsigned char[size * nbImages];

	for (auto i = 0; i < nbImages; i++) {
		memcpy(m_data + size * i, m_images[i], size);
	}

	// delete the list of images
	for (auto i = 0; i < m_nbImages; i++) {
		if (m_images[i]) {
			stbi_image_free(m_images[i]);
		}
	}
	delete m_images;
	m_images = nullptr;
}

/**
 * create the textures from a list of textures
 */
fwTextures::fwTextures(const std::list<fwTexture*> textures)
{
	myclass |= TEXTURES;
	m_nbImages = textures.size();

	textures.front()->get_info(&width, &height, &nrChannels);
	int size = width * height * nrChannels;
	m_data = new unsigned char[size * m_nbImages];

	auto i = 0;
	for (auto texture : textures) {
		int w, h, n;
		unsigned char* data = texture->get_info(&w, &h, &n);

		if (w != width || h != height || n != nrChannels) {
			std::cout << "fwTextures::fwTextures inconsistent size " << std::endl;
			exit(-1);
		}

		memcpy(m_data + size * i, m_data, size);

		i++;
	}
}

/**
 * reserve memory to upload individual images not yet here
 */
fwTextures::fwTextures(const int _nbImages, const int _width, const int _height, const int _format):
	fwTexture(_width, _height, _format),
	m_nbImages(_nbImages)
{
	myclass |= TEXTURES;
	int size = width * height * nrChannels;
	m_data = new unsigned char[size * m_nbImages];
}

/**
 * there is a mega buffer provided that stores all images in a continuous model
 */
fwTextures::fwTextures(const int _nbImages, const int _width, const int _height, const int _format, unsigned char *raw_data) :
	fwTexture(_width, _height, _format),
	m_nbImages(_nbImages)
{
	myclass |= TEXTURES;
	m_data = raw_data;
}

/**
 *
 */
void fwTextures::set_image(int image, fwTexture *texture)
{
	int w, h, n;

	assert(m_data != nullptr);

	if (image > m_nbImages) {
		std::cout << "fwTextures::set_image incorrect image " << image << std::endl;
		exit(-1);
	}

	int size = width * height * nrChannels;
	unsigned char* data = texture->get_info(&w, &h, &n);

	if (w != width || h != height || n != nrChannels) {
		std::cout << "fwTextures::fwTextures inconsistent size " << std::endl;
		exit(-1);
	}

	memcpy(m_data + size * image, data, size);
}

/**
 *
 */
unsigned char *fwTextures::get_info(int &_width, int &_height, int &_nrChannels, int &nrLayers)
{
	_width = width;
	_height = height;
	_nrChannels = nrChannels;
	nrLayers = m_nbImages;

	return m_data;
}

fwTextures::~fwTextures()
{
	if (m_images) {
	}

	if (m_data) {
		// delete the mega buffer
		delete m_data;
		m_data = nullptr;
	}
}