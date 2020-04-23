#include "GLogs.h"
#include "GWindow.h"

using namespace GGraphis;
using namespace GLogs;


GWindow::~GWindow()
{
}


bool GWindow::Create(const std::string& strTitle, const SDL_Rect& WindowRect, Uint32 nWindowFlags_)
{
	if (m_pSharedWindow)
	{
		PrintError("Window窗口已创建，无法再次创建", "");
		return SDL_FALSE;
	}

	SDL_Window* _pWindow = SDL_CreateWindow(strTitle.c_str(), WindowRect.x, WindowRect.y, WindowRect.w, WindowRect.h, nWindowFlags_);
	if (!_pWindow)
	{
		PrintError("创建Window出错", SDL_GetError());
		return SDL_FALSE;
	}

	// 将SDL_Window指针转成智能指针
	m_pSharedWindow = CREATE_SHARED_WINDOW(_pWindow);
	_pWindow = nullptr;

	if (m_pSharedRenderer)
	{
		PrintError("Renderer已经创建，无法再次创建", "");
		return SDL_FALSE;
	}

	SDL_Renderer* _pRenderer = SDL_CreateRenderer(GET_RAW_POINT(m_pSharedWindow), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (!_pRenderer)
	{
		PrintError("创建Renderer出错", SDL_GetError());
		return SDL_FALSE;
	}

	m_pSharedRenderer = CREATE_SHARED_RENDERER(_pRenderer);
	_pRenderer = nullptr; // 裸指针赋值给智能指针后要及时赋值为nullptr
	
	return SDL_TRUE;
}

bool GWindow::RegisterElement(ElementPtr pElement_)
{
	if (std::find(m_ElementsList.cbegin(), m_ElementsList.cend(), pElement_) != m_ElementsList.cend())
		return SDL_FALSE;

	m_ElementsList.push_back(pElement_);

	// 将Element注册到Window时候，需要将Window中的Renderer指针传给Element，用以Element作图和渲染
	// 同时需要把Window指针也传过去，用于元素的绘制
	pElement_->m_pSharedRenderer = this->m_pSharedRenderer;
	pElement_->m_pSharedWindow = this->m_pSharedWindow;
	return SDL_TRUE;
}


bool GWindow::RegisterElements(ElementPtr* pElement_, Uint32 nSize_)
{
	bool _bIsAllAdd = SDL_FALSE;
	for (Uint32 i = 0; i < nSize_; ++i)
	{
		if (std::find(m_ElementsList.cbegin(), m_ElementsList.cend(), pElement_[i]) == m_ElementsList.cend())
		{
			_bIsAllAdd = SDL_FALSE;
			continue;
		}

		m_ElementsList.push_back(pElement_[i]);

		// 将Element注册到Window时候，需要将Window中的Renderer指针传给Element，用以Element作图和渲染
		// 同时需要把Window指针也传过去，用于元素的绘制
		pElement_[i]->m_pSharedRenderer = this->m_pSharedRenderer;
		pElement_[i]->m_pSharedWindow = this->m_pSharedWindow;
	}

	return _bIsAllAdd;
}

void GWindow::Render()
{
	auto _pRenderer = GET_RAW_POINT(m_pSharedRenderer); // 注意，不能Destrpy此指针，否则智能指针会失效
	if (0 != SDL_SetRenderDrawColor(_pRenderer, m_BackgroundColour.r, m_BackgroundColour.g, m_BackgroundColour.b, m_BackgroundColour.a))
	{
		PrintError("设置背景色出错", SDL_GetError());
		return;
	}

	if (0 != SDL_RenderClear(_pRenderer))
	{
		PrintError("清除画板出错", SDL_GetError());
		return;
	}

	for (const auto& Element_ : m_ElementsList)
	{
		if (Element_->GetShow())
		{
			Element_->Update();
		}
	}

	SDL_RenderPresent(_pRenderer);
}

void GWindow::Run(GFunc pCallbackFunc_, void* pParameters_)
{
	bool _bQuit = SDL_FALSE;
	while (!_bQuit)
	{
		while (SDL_PollEvent(&m_Event))
		{
			if (m_Event.type == SDL_QUIT)
			{
				_bQuit = SDL_TRUE;
				break;
			}

			if (pCallbackFunc_)
				pCallbackFunc_(pParameters_);
		}

		

		Render();

		SDL_Delay(2);
	}
	
}
