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
			PrintError("创建元素错误", "没有此类型元素的定义，无法创建");
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
			PrintError("设置透明度出错", SDL_GetError());
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
			PrintError("Texture为空，无法执行anti-aliasing", "");
			return;
		}


	}

	/************ GElement ***************/

	


	/************ GRect ***************/

	bool GRect::Update()
	{
		// 首先判断是否需要重新绘制Texture，如果不需要重新绘制，则直接返回
		// 如果需要重新绘制，则将Texture智能指针重置
		if (m_pSharedTexture)
		{
			if (!m_bReDraw)
			{
				if (!RenderCopy())
				{
					PrintError("将Texture渲染到Renderer上出错", SDL_GetError());
					return false;
				}

				return true;
			}

			// 需要重新渲染绘制的话，先清除指针对texture的引用
			m_pSharedTexture.reset();
		}

		// 创建texture，使用renderer在其上进行绘制
		m_pSharedTexture = CREATE_SHARED_TEXTURE_WITHPARAM(
			GET_RAW_POINT(m_pSharedRenderer),
			SDL_GetWindowPixelFormat(GET_RAW_POINT(m_pSharedWindow)),
			SDL_TEXTUREACCESS_TARGET,
			static_cast<int>(m_Shape.m_Rect.w + 4),
			static_cast<int>(m_Shape.m_Rect.h + 4)
		);

		if (!m_pSharedTexture)
		{
			PrintError("创建临时Texture出错", SDL_GetError());
			return false;
		}

		// 此处无需检测m_pSharedRenderer值是否为空，因为只有注册到Window后，Window才能调用Update函数，
		// 而在注册的时候，一定会将m_pSharedRenderer赋值
		SDL_assert(m_pSharedRenderer != nullptr);
		auto _pRenderer = GET_RAW_POINT(m_pSharedRenderer);

		// 根据官网建议，在执行以下函数之前，需要检测SDL_RendererInfo中是否有SDL_RENDERER_TARGETTEXTURE标识
		bool _bSuccess = true;
		if (0 != SDL_SetRenderTarget(_pRenderer, GET_RAW_POINT(m_pSharedTexture)))
		{
			PrintError("无法将Renderer的目标设置为Texture", SDL_GetError());
			return false;
		}
		else
		{
			if (0 != SDL_SetRenderDrawColor(_pRenderer, 255, 206, 0, 0))
			{
				PrintError("设置元素背景色出错", SDL_GetError());
				_bSuccess = false; // 如果此步出错，则下面就无需设置了
			}

			SDL_RenderClear(_pRenderer);

			if (0 != SDL_SetRenderDrawColor(_pRenderer, m_Colour.r, m_Colour.g, m_Colour.b,m_Colour.a))
			{
				PrintError("设置元素背景色出错", SDL_GetError());
				_bSuccess = false; // 如果此步出错，则下面就无需设置了
			}

			if (_bSuccess)
			{
				SDL_FRect _rect = { 2.0, 2.0, m_Shape.m_Rect.w, m_Shape.m_Rect.h };
				if (0 != SDL_RenderDrawRectF(_pRenderer, &_rect))
				{
					PrintError("使用Renderer绘制方形出错", SDL_GetError());
					_bSuccess = false;
				}

				_rect = { 3, 3, m_Shape.m_Rect.w - 2, m_Shape.m_Rect.h - 2 };
				if (0 != SDL_RenderDrawRectF(_pRenderer, &_rect))
				{
					PrintError("使用Renderer绘制方形出错", SDL_GetError());
					_bSuccess = false;
				}
			}
			
			// 将Renderer的目标设回到Window上
			if (0 != SDL_SetRenderTarget(_pRenderer, nullptr))
			{
				PrintError("无法还原Renderer的目标设置为Window", SDL_GetError());
				return false;
			}
		}

		if (!RenderCopy())
		{
			PrintError("将Texture渲染到Renderer上出错", SDL_GetError());
			return false;
		}

		m_bReDraw = false;
		return _bSuccess;
	}




	/************ GRect ***************/



	
	/************ GLine ***************/

	bool GLine::Update()
	{
		// 此处无需检测m_pSharedRenderer值是否为空，因为只有注册到Window后，Window才能调用Update函数，
		// 而在注册的时候，一定会将m_pSharedRenderer赋值
		SDL_assert(m_pSharedRenderer != nullptr);
		if (0 != SDL_SetRenderDrawColor(GET_RAW_POINT(m_pSharedRenderer), m_Colour.r, m_Colour.g, m_Colour.b, m_Colour.a))
		{
			PrintError("设置元素背景色出错", SDL_GetError());
			return false;
		}

		const auto& _StartP = m_Shape.m_Points.Start;
		const auto& _EndP = m_Shape.m_Points.End;
		if (0 != SDL_RenderDrawLineF(GET_RAW_POINT(m_pSharedRenderer), _StartP.x, _StartP.y, _EndP.x, _EndP.y))
		{
			PrintError("使用Renderer绘制线条出错", SDL_GetError());
			return false;
		}

		return true;
	}

	// 暂时是有问题的，后面再仔细设计
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