#pragma once

#include <string>
#include <vector>
#include <glm/vec4.hpp>

#include "../gaEngine/gaImage2D.h"
#include "../framework/fwTextureAtlas.h"

#include "gaComponent.h"

namespace GameEngine
{

	class UI_widget;
	class UI_tab;
	class UI_button;

	enum UI_message {
		click = 1
	} ;

	struct UI_def_button {
		std::string name;
		uint32_t imageIndex;
		uint32_t message;
		bool repeater;
		bool visible;
		glm::vec4 img_press_position_size;					// position and size of the 'pressed' image
		glm::vec4 img_release_position_size;				// position and size of the 'released' image
		fwTexture* texture;
		glm::vec2 translation;
		glm::vec2 scale;
	};

	class UI : public gaComponent
	{
	protected:
		UI_widget* m_root = nullptr;
		Framework::TextureAtlas* m_textures=nullptr;
		bool m_dirty = true;
		UI_widget* m_currentButton = nullptr;									// currently pressed button

		void onMouseDown(gaMessage*);											// dipatch mouse actions to widgets
		void onMouseUp(gaMessage*);												// dipatch mouse actions to widgets
		void onMouseMove(gaMessage*);											// dipatch mouse actions to widgets
		void onTimer(gaMessage*);												// 

	public:
		UI(const std::string& name, bool m_visible=true);
		inline void root(UI_widget* widget) { m_root = widget; };
		void receiveMessage(UI_widget* from, uint32_t imessage);				// receive a message from a widget
		void timer(bool b);														// (de)activate the timer
		void dispatchMessage(gaMessage*) override;								// receive and dispatch controller messages
	};

	/**
	 *
	 */
	class UI_widget: public GameEngine::Image2D
	{
	protected:
		glm::vec4 m_position_size = glm::vec4(0);
		glm::vec4 m_screen_position_size = glm::vec4(0);
		bool m_draw = false;
		UI_widget* m_parent = nullptr;
		UI* m_ui = nullptr;

		virtual void sendMessage(UI_widget* from, uint32_t imessage);	// send a message to the parent

	public:
		UI_widget(const std::string& name, const glm::vec2& scale,	const glm::vec2& translate, fwTexture* texture, bool visible = true);
		void add(UI_widget* widget);										// add a widget on the panel
		void update(const glm::vec4&);										// update screen position based on parent
		void update(void);													// update children screen position
		UI_widget* findWidgetAt(const glm::vec2&);							// find relative widget
		void link(UI* ui);													// link each widget to the top UI
		void addButtons(std::vector<struct UI_def_button>& buttons);		// add buttons to the widget using a template

		inline void draw(bool b) { m_draw = b; };
		UI_widget* widget(const std::string& name);							// get a specific widget

		inline virtual void onMouseDown(void) { };							// click on the widget
		inline virtual void onMouseUp(void) { };							// release the widget
		inline virtual void onMouseMove(void) { };							// get cursor movement when captured
		inline virtual void onEnterArea(void) { };							// cursor enters the widget area
		inline virtual void onExitArea(void) { };							// cursor leaves the widget area
		inline virtual void onTimer(void) { };								// timer when widget captured the mouse
	};

	/**
	 *
	 */
	class UI_container : public UI_widget
	{
	public:
		UI_container(const std::string& name, const glm::vec2& scale, const glm::vec2& translation, bool visible = true);
	};

	/**
	 *
	 */
	class UI_picture : public UI_widget
	{
	protected:
		glm::vec4 m_textureIndex;
	public:
		UI_picture(const std::string& name, 
			const glm::vec2& scale,
			const glm::vec2& translate,
			const glm::vec4& textureIndex, 
			fwTexture* texture, 
			bool visible=true);
		UI_picture(const std::string& name,
			const glm::vec4& position,
			const glm::vec4& textureIndex,
			fwTexture* texture,
			bool visible = true);
	};

	/**
	 *
	 */
	class UI_tab : public UI_widget
	{
		std::map<UI_button*, UI_widget*> m_tabs;							// list of button/panel
		UI_button* m_activeTab = nullptr;
		UI_widget* m_panel = nullptr;										// target panel
		void sendMessage(UI_widget* from, uint32_t imessageu) override;		// send a message to the parent
	public:
		UI_tab(const std::string& name, const glm::vec2& scale, const glm::vec2& translate);
		void tab(UI_button* current);										// force the current tab
		void addTab(UI_button* button, UI_widget* panel);					// add a tab : button + panel
		void setPanel(UI_widget* panel);									// define the display panel
	};

	/**
	 *
	 */
	class UI_button : public UI_picture
	{
		glm::vec4 m_texture_on;												// when button is pressed: position&size of the image
		glm::vec4 m_texture_off;											// when button is releases: position&size of the image
		uint32_t m_message = 0;												// message to send when the button is released of pressed with repeater
		bool m_repeater = false;											// send 'repeat' messages while pressed (like a scroll down button)

	public:
		UI_button(const std::string& name, 
			const glm::vec2& scale,
			const glm::vec2& translate,
			const glm::vec4& textureOnIndex,
			const glm::vec4& textureOffIndex, 
			fwTexture* texture,
			uint32_t message = 0,											// message sent when activated (none by default)
			bool repeater = false,											// does the button send messages while pressed ?
			bool visible = true);
		void onMouseDown(void) override;									// push the button
		void onMouseUp(void) override;										// release the button
		void onEnterArea(void) override;									// cursor enters the button area
		void onExitArea(void) override;										// cursor leaves the button area
		void onTimer(void) override;										// timer when widget captured the mouse

		void release(void);													// force the OFF image
		void press(void);													// force the ON image
	};

	/**
	 *
	 */
	class UI_ZoomPicture : public UI_picture
	{
		glm::ivec2 m_imageSize;
		glm::ivec2 m_viewPort;
		glm::ivec2 m_scroll;
		glm::vec4 m_size;			// global size of the picture

		float m_zoom = 1.0f;
	public:
		UI_ZoomPicture(const std::string& name, 
			const glm::vec2& scale, 
			const glm::vec2& translation,
			const glm::vec4& textureIndex,
			const glm::ivec2& imageSize, 
			const glm::ivec2& viewPort,
			fwTexture* texture,
			bool visible = true);
		void scrollUp(void);
		void scrollDown(void);
	};


}