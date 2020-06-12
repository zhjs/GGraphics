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
		
		// ��GWindow��Ϊ��Ԫ����Ϊ�ڽ�Elementע�ᵽWindow��ʱ����Ҫ��Window���Renderer����Element�����ֲ��뱩¶���ⲿ
		// ��Window�ڻ��Ƶ�ʱ��Ҳ�����Element��Update()����
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

		// GElement��Ϊ�����࣬�������ʵ��Update()��������Update���������Ÿ��û�
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
		bool m_bReDraw = true; // �Ƿ���Ҫ���»���Texture
		Uint8 m_nAntiAliasing = 0; // ����ݵȼ���Ϊ0��ʾ�رտ����

		// �����壬��������Rect(һ�����ھ��λ���ͼ�ε�Ԫ��)����һ������������Ľṹ��(һ��������Ԫ��)
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








	// һЩ�����Ķ���
	using ElementPtr = std::shared_ptr<GElement>;
}





#endif