#pragma once

#include <string>
#include <vector>
#include <glm/vec4.hpp>

#include "../framework/fwHUD.h"
#include "../framework/fwHUDelement.h"
#include "../framework/fwTextureAtlas.h"

#include "gaComponent.h"

namespace GameEngine
{

	class UI_widget;
	class UI_tab;

	class UI : public fwHUD, public gaComponent
	{
	protected:
		UI_widget* m_root;
		Framework::TextureAtlas* m_textures=nullptr;
		bool m_dirty = true;
		void onMouseDown(gaMessage*);															// dipatch mouse actions to widgets

	public:
		UI(const std::string& name, bool m_visible=true);
		inline void root(UI_widget* widget) { m_root = widget; };
		void draw(void)  override;																// draw the GUI
		static void draw_widget(const glm::vec4& position_size);
		void dispatchMessage(gaMessage*) override;												// receive and dispatch controller messages
	};

	class UI_widget
	{
	protected:
		std::string m_name;
		glm::vec4 m_position_size;
		glm::vec4 m_screen_position_size;
		std::vector<UI_widget*> m_widgets;
		bool m_visible = true;
		bool m_draw = false;

	public:
		UI_widget(const std::string& name, const glm::vec4& position, bool visible = true);
		void add(UI_widget* widget);															// add a widget on the panel
		void update(const glm::vec4&);															// update screen position
		virtual void draw(void);																// draw the GUI
		inline void visible(bool b) { m_visible = b; };
		inline bool visible(void) { return m_visible; };
		inline UI_widget* widget(uint32_t i) { return m_widgets[i]; };
	};

	class UI_picture : public UI_widget
	{
		glm::vec4 m_textureIndex;
	public:
		UI_picture(const std::string& name, const glm::vec4& panel, const glm::vec4& textureIndex, bool visible=true);
		void draw(void) override;																// draw the GUI
	};

	class UI_tab : public UI_widget
	{
	public:
		UI_tab(const std::string& name, const glm::vec4& panel, const glm::vec4& button);
	};

	class UI_button : public UI_picture
	{
		glm::vec4 m_texture_on;		
		glm::vec4 m_texture_off;
	public:
		UI_button(const std::string& name, const glm::vec4& position, const glm::vec4& textureOnIndex, const glm::vec4& textureOffIndex, bool visible = true);
	};
}