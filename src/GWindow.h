#ifndef __GWINDOW_H__
#define __GWINDOW_H__

#include <string>
#include <list>


#include "../../GSDL/src/GSDL.h"
#include "GElement.h"


namespace GGraphis
{
	// 用户传递的回调函数
	using GFunc = void (*)(void*);



	class GWindow
	{
	public:
		GWindow():m_BackgroundColour{0xFF, 0xFF, 0xFF, 0xFF} {}
		virtual ~GWindow();

		bool Create(const std::string& strTitle = "this is a test",
						const SDL_Rect& WindowRect = SDL_Rect{100, 100, 100, 100},
						Uint32 nWindowFlags_ = SDL_WINDOW_ALLOW_HIGHDPI);

		SDL_Colour GetBackgroundColour() const { return m_BackgroundColour; }
		void SetBackgroundColour(const SDL_Colour& BackgroundColour_) { m_BackgroundColour = BackgroundColour_; }

		void Run(GFunc pCallbackFunc_, void* pParameters_);

		bool RegisterElement(ElementPtr pElement_);
		bool RegisterElements(ElementPtr* pElement_, Uint32 nSize_);

		bool RemoveElement(ElementPtr pElement_) { m_ElementsList.remove(pElement_); }
		bool RemoveElements();

		SDL_Event* GetEvent() { return &m_Event; }

		void Render();



	private:

		WindowSharedPtr m_pSharedWindow;
		RendererSharedPtr m_pSharedRenderer;
		std::list<ElementPtr> m_ElementsList;
		SDL_Colour m_BackgroundColour;
		SDL_Event m_Event;


	};




}




#endif
