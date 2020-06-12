#ifndef __GELEMENT_H__
#define __GELEMENT_H__

#include "../../GSDL/src/GSDL.h"
#include <memory>


using namespace GSDL;

namespace GGraphis
{


	class GElement
	{
	public:
		
		// 将GWindow作为友元，因为在将Element注册到Window中时，需要将Window里的Renderer传给Element，但又不想暴露给外部
		// 且Window在绘制的时候，也会调用Element的Update()函数
		friend class GWindow;


		GElement()
		{
			SDL_memset(&m_Shape, 0, sizeof(m_Shape));
		}

		GElement(const GElement& Element_);

		virtual ~GElement() {}


		virtual void GetSize(float& nWidth_, float& nHeight_) const;
		virtual void SetSize(float nWidth_, float nHeight_);
		
		virtual void GetPosition(float& nX_, float& nY_) const;
		virtual void SetPosition(float nX_, float nY_);

		float GetAngle() const { return m_nAngle; }
		void SetAngle(float nAngle_) { m_nAngle = nAngle_; }

		SDL_Colour GetColour() const { return m_Colour; }
		void SetColour(const SDL_Colour& Colour_) { m_Colour = Colour_; }

		void Clone(GElement& Element_);

		bool GetShow() const { return m_bShow; }
		void SetShow(bool bIsShow_) { m_bShow = bIsShow_; }

		void SetOpacity(Uint8 nOpacity_) { m_nAlpha = nOpacity_; }
		Uint8 GetOpacity() const { return m_nAlpha; }

		void SetAntiAliasing(Uint8 nAntiAliasingLevel_) { m_nAntiAliasing = nAntiAliasingLevel_; }
	
		

	protected:

		// GElement作为纯虚类，子类必须实现Update()函数，且Update函数不开放给用户
		virtual bool Update() { return false; }

		bool RenderCopy();
		bool SetAlpha(Uint8 nAlpha_);
		void AntiAliasing();

	protected:
		TextureSharedPtr m_pSharedTexture;
		RendererSharedPtr m_pSharedRenderer;
		WindowSharedPtr m_pSharedWindow;
		SDL_Colour m_Colour = { 0xFF, 0xFF, 0xFF, 0xFF };
		float m_nAngle = 0.0f;
		Uint8 m_nAlpha = 0xFF;
		bool m_bShow = true;
		bool m_bReDraw = true; // 是否需要重新绘制Texture
		Uint8 m_nAntiAliasing = 0; // 抗锯齿等级，为0表示关闭抗锯齿

		// 联合体，里面存放了Rect(一般用于矩形或者图形等元素)或者一个包含两个点的结构体(一般用于线元素)
		union
		{
			SDL_FRect m_Rect;
			struct
			{
				SDL_FPoint Start;
				SDL_FPoint End;
			} m_Points;
		} m_Shape;

	};

	class GRect : public GElement
	{
	public:
		GRect() : GElement() {}
		GRect(const GRect& Rect_) : GElement(Rect_) {}
		virtual ~GRect() {}

	protected:

		virtual bool Update() override;
	};

	class GLine : public GElement
	{
	public:
		GLine() :GElement() {}
		GLine(const GLine& Line_) : GElement(Line_) {}
		virtual ~GLine() {}

		virtual void GetSize(float& nWidth_, float& nHeight_) const override;
		virtual void SetSize(float nWidth_, float nHeight_) override;

	protected:
		virtual bool Update() override;

	};

	class GPicture : public GElement
	{
		GPicture() : GElement() {}
		GPicture(const GPicture& Picture_) : GElement(Picture_) {}
		virtual ~GPicture() {}

	protected:
		virtual bool Update() override;
	};








	// 一些别名的定义
	using ElementPtr = std::shared_ptr<GElement>;
}





#endif