#include "GLogs.h"
#include "GElement.h"
#include <algorithm>
#include <cctype>
#include <iterator>

using namespace GLogs;

namespace GGraphis
{

	ElementPtr CreateElement(const std::string& strElemType_)
	{
		std::string _strUpperString;
		std::transform(strElemType_.begin(), strElemType_.end(), std::back_inserter(_strUpperString), std::toupper);
		if (_strUpperString == "RECT")
		{
			return ElementPtr(new GRect);
		}
		else if (_strUpperString == "LINE")
		{
			return ElementPtr(new GLine);
		}
		else if (_strUpperString == "PICTURE")
		{
			return ElementPtr(new GPicture);
		}
		else
		{
			PrintError("����Ԫ�ش���", "û�д�����Ԫ�صĶ��壬�޷�����");
			return nullptr;
		}
	}



	/************ GElement ***************/

/*
// 	GElement::GElement(const GElement& Element_)
// 	{
// 		this->m_pSharedTexture = Element_.m_pSharedTexture;
// 		this->m_pSharedRenderer = Element_.m_pSharedRenderer;
// 		this->m_pSharedWindow = Element_.m_pSharedWindow;
// 		this->m_Shape = Element_.m_Shape;
// 		this->m_bShow = Element_.m_bShow;
// 		this->m_Colour = Element_.m_Colour;
// 		this->m_nAngle = Element_.m_nAngle;
// 		this->m_bReDraw = Element_.m_bReDraw;
// 		this->m_nAlpha = Element_.m_nAlpha;
// 		this->m_nAntiAliasing = Element_.m_nAntiAliasing;
// 	}
*/

	void GElement::GetSize(float& nWidth_, float& nHeight_) const
	{
		nWidth_ = m_Shape.m_Rect.w;
		nHeight_ = m_Shape.m_Rect.h;
	}

	void GElement::SetSize(float nWidth_, float nHeight_)
	{
		m_Shape.m_Rect.w = nWidth_;
		m_Shape.m_Rect.h = nHeight_;
	}

	void GElement::GetPosition(float& nX_, float& nY_) const
	{
		nX_ = m_Shape.m_Rect.x;
		nY_ = m_Shape.m_Rect.y;
	}

	void GElement::SetPosition(float nX_, float nY_)
	{
		m_Shape.m_Rect.x = nX_;
		m_Shape.m_Rect.y = nY_;
	}

	bool GElement::RenderCopy()
	{
		if (!SetAlpha(m_nAlpha))
		{
			PrintError("����͸���ȳ���", SDL_GetError());
			return false;
		}

		int _nReturn = 0;
		if (SDL_fabsf(m_nAngle) < 1.0E-8)
		{
			_nReturn = SDL_RenderCopyF(
				GET_RAW_POINT(m_pSharedRenderer),
				GET_RAW_POINT(m_pSharedTexture),
				nullptr,
				&m_Shape.m_Rect);
		}
		else
		{
			_nReturn = SDL_RenderCopyExF(
				GET_RAW_POINT(m_pSharedRenderer),
				GET_RAW_POINT(m_pSharedTexture),
				nullptr,
				&m_Shape.m_Rect,
				m_nAngle,
				&SDL_FPoint({m_Shape.m_Rect.x, m_Shape.m_Rect.y}),
				SDL_FLIP_HORIZONTAL);
		}

		return (_nReturn == 0);
	}

	bool GElement::SetAlpha(Uint8 nAlpha_)
	{
		auto _pTexture = GET_RAW_POINT(m_pSharedTexture);
		if (0 != SDL_SetTextureBlendMode(_pTexture, SDL_BLENDMODE_BLEND))
			return false;

		if (0 != SDL_SetTextureAlphaMod(_pTexture, nAlpha_))
			return false;

		return true;
	}

	void GElement::AntiAliasing()
	{
		if (m_nAntiAliasing == 0)
			return;

		if (!m_pSharedTexture)
		{
			SDL_assert(false);
			PrintError("TextureΪ�գ��޷�ִ��anti-aliasing", "");
			return;
		}


	}

	/************ GElement ***************/

	


	/************ GRect ***************/

	bool GRect::Update()
	{
		// �����ж��Ƿ���Ҫ���»���Texture���������Ҫ���»��ƣ���ֱ�ӷ���
		// �����Ҫ���»��ƣ���Texture����ָ������
		if (m_pSharedTexture)
		{
			if (!m_bReDraw)
			{
				if (!RenderCopy())
				{
					PrintError("��Texture��Ⱦ��Renderer�ϳ���", SDL_GetError());
					return false;
				}

				return true;
			}

			// ��Ҫ������Ⱦ���ƵĻ��������ָ���texture������
			m_pSharedTexture.reset();
		}

		// ����texture��ʹ��renderer�����Ͻ��л���
		m_pSharedTexture = CREATE_SHARED_TEXTURE_WITHPARAM(
			GET_RAW_POINT(m_pSharedRenderer),
			SDL_GetWindowPixelFormat(GET_RAW_POINT(m_pSharedWindow)),
			SDL_TEXTUREACCESS_TARGET,
			static_cast<int>(m_Shape.m_Rect.w + 4),
			static_cast<int>(m_Shape.m_Rect.h + 4)
		);

		if (!m_pSharedTexture)
		{
			PrintError("������ʱTexture����", SDL_GetError());
			return false;
		}

		// �˴�������m_pSharedRendererֵ�Ƿ�Ϊ�գ���Ϊֻ��ע�ᵽWindow��Window���ܵ���Update������
		// ����ע���ʱ��һ���Ὣm_pSharedRenderer��ֵ
		SDL_assert(m_pSharedRenderer != nullptr);
		auto _pRenderer = GET_RAW_POINT(m_pSharedRenderer);

		// ���ݹ������飬��ִ�����º���֮ǰ����Ҫ���SDL_RendererInfo���Ƿ���SDL_RENDERER_TARGETTEXTURE��ʶ
		bool _bSuccess = true;
		if (0 != SDL_SetRenderTarget(_pRenderer, GET_RAW_POINT(m_pSharedTexture)))
		{
			PrintError("�޷���Renderer��Ŀ������ΪTexture", SDL_GetError());
			return false;
		}
		else
		{
			if (0 != SDL_SetRenderDrawColor(_pRenderer, 255, 206, 0, 0))
			{
				PrintError("����Ԫ�ر���ɫ����", SDL_GetError());
				_bSuccess = false; // ����˲����������������������
			}

			SDL_RenderClear(_pRenderer);

			if (0 != SDL_SetRenderDrawColor(_pRenderer, m_Colour.r, m_Colour.g, m_Colour.b,m_Colour.a))
			{
				PrintError("����Ԫ�ر���ɫ����", SDL_GetError());
				_bSuccess = false; // ����˲����������������������
			}

			if (_bSuccess)
			{
				SDL_FRect _rect = { 2.0, 2.0, m_Shape.m_Rect.w, m_Shape.m_Rect.h };
				if (0 != SDL_RenderDrawRectF(_pRenderer, &_rect))
				{
					PrintError("ʹ��Renderer���Ʒ��γ���", SDL_GetError());
					_bSuccess = false;
				}

				_rect = { 3, 3, m_Shape.m_Rect.w - 2, m_Shape.m_Rect.h - 2 };
				if (0 != SDL_RenderDrawRectF(_pRenderer, &_rect))
				{
					PrintError("ʹ��Renderer���Ʒ��γ���", SDL_GetError());
					_bSuccess = false;
				}
			}
			
			// ��Renderer��Ŀ����ص�Window��
			if (0 != SDL_SetRenderTarget(_pRenderer, nullptr))
			{
				PrintError("�޷���ԭRenderer��Ŀ������ΪWindow", SDL_GetError());
				return false;
			}
		}

		if (!RenderCopy())
		{
			PrintError("��Texture��Ⱦ��Renderer�ϳ���", SDL_GetError());
			return false;
		}

		m_bReDraw = false;
		return _bSuccess;
	}




	/************ GRect ***************/



	
	/************ GLine ***************/

	bool GLine::Update()
	{
		// �˴�������m_pSharedRendererֵ�Ƿ�Ϊ�գ���Ϊֻ��ע�ᵽWindow��Window���ܵ���Update������
		// ����ע���ʱ��һ���Ὣm_pSharedRenderer��ֵ
		SDL_assert(m_pSharedRenderer != nullptr);
		if (0 != SDL_SetRenderDrawColor(GET_RAW_POINT(m_pSharedRenderer), m_Colour.r, m_Colour.g, m_Colour.b, m_Colour.a))
		{
			PrintError("����Ԫ�ر���ɫ����", SDL_GetError());
			return false;
		}

		const auto& _StartP = m_Shape.m_Points.Start;
		const auto& _EndP = m_Shape.m_Points.End;
		if (0 != SDL_RenderDrawLineF(GET_RAW_POINT(m_pSharedRenderer), _StartP.x, _StartP.y, _EndP.x, _EndP.y))
		{
			PrintError("ʹ��Renderer������������", SDL_GetError());
			return false;
		}

		return true;
	}

	// ��ʱ��������ģ���������ϸ���
	void GLine::GetSize(float& nWidth_, float& nHeight_) const
	{
		nWidth_ = SDL_fabsf(m_Shape.m_Points.End.x - m_Shape.m_Points.Start.x);
		nHeight_ = SDL_fabsf(m_Shape.m_Points.End.y - m_Shape.m_Points.Start.y);
	}

	void GLine::SetSize(float nWidth_, float nHeight_)
	{
		m_Shape.m_Points.End.x = nWidth_ + m_Shape.m_Points.Start.x;
		m_Shape.m_Points.End.y = nHeight_ + m_Shape.m_Points.Start.y;
	}

	/************ GLine ***************/

	








}