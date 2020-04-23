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
		PrintError("Window�����Ѵ������޷��ٴδ���", "");
		return SDL_FALSE;
	}

	SDL_Window* _pWindow = SDL_CreateWindow(strTitle.c_str(), WindowRect.x, WindowRect.y, WindowRect.w, WindowRect.h, nWindowFlags_);
	if (!_pWindow)
	{
		PrintError("����Window����", SDL_GetError());
		return SDL_FALSE;
	}

	// ��SDL_Windowָ��ת������ָ��
	m_pSharedWindow = CREATE_SHARED_WINDOW(_pWindow);
	_pWindow = nullptr;

	if (m_pSharedRenderer)
	{
		PrintError("Renderer�Ѿ��������޷��ٴδ���", "");
		return SDL_FALSE;
	}

	SDL_Renderer* _pRenderer = SDL_CreateRenderer(GET_RAW_POINT(m_pSharedWindow), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (!_pRenderer)
	{
		PrintError("����Renderer����", SDL_GetError());
		return SDL_FALSE;
	}

	m_pSharedRenderer = CREATE_SHARED_RENDERER(_pRenderer);
	_pRenderer = nullptr; // ��ָ�븳ֵ������ָ���Ҫ��ʱ��ֵΪnullptr
	
	return SDL_TRUE;
}

bool GWindow::RegisterElement(ElementPtr pElement_)
{
	if (std::find(m_ElementsList.cbegin(), m_ElementsList.cend(), pElement_) != m_ElementsList.cend())
		return SDL_FALSE;

	m_ElementsList.push_back(pElement_);

	// ��Elementע�ᵽWindowʱ����Ҫ��Window�е�Rendererָ�봫��Element������Element��ͼ����Ⱦ
	// ͬʱ��Ҫ��Windowָ��Ҳ����ȥ������Ԫ�صĻ���
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

		// ��Elementע�ᵽWindowʱ����Ҫ��Window�е�Rendererָ�봫��Element������Element��ͼ����Ⱦ
		// ͬʱ��Ҫ��Windowָ��Ҳ����ȥ������Ԫ�صĻ���
		pElement_[i]->m_pSharedRenderer = this->m_pSharedRenderer;
		pElement_[i]->m_pSharedWindow = this->m_pSharedWindow;
	}

	return _bIsAllAdd;
}

void GWindow::Render()
{
	auto _pRenderer = GET_RAW_POINT(m_pSharedRenderer); // ע�⣬����Destrpy��ָ�룬��������ָ���ʧЧ
	if (0 != SDL_SetRenderDrawColor(_pRenderer, m_BackgroundColour.r, m_BackgroundColour.g, m_BackgroundColour.b, m_BackgroundColour.a))
	{
		PrintError("���ñ���ɫ����", SDL_GetError());
		return;
	}

	if (0 != SDL_RenderClear(_pRenderer))
	{
		PrintError("����������", SDL_GetError());
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
