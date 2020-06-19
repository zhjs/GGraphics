#define _USE_MATH_DEFINES
#include "src/GWindow.h"
#include "GLogsCommon.h"

using namespace GGraphis;
using namespace GLogs;
using namespace GSDL;

void CreateCircleData(const SDL_FPoint& CenterPoint_, float nRadius_, int nDivides_, SDL_FPoint*& Points_)
{
	double _nDeltAngle = 2 * M_PI / nDivides_;
	Points_ = new SDL_FPoint[nDivides_];

	int _nIndex = 0;
	for (double _nAngle = 0.0; _nAngle <= 2 * M_PI && _nIndex < nDivides_; _nAngle += _nDeltAngle)
	{
		double _nx = CenterPoint_.x + nRadius_ * cos(_nAngle);
		double _ny = CenterPoint_.y + nRadius_ * sin(_nAngle);
		Points_[_nIndex++] = { static_cast<float>(_nx), static_cast<float>(_ny) };
	}
}

struct pp
{
	ElementPtr pe;
	SDL_Event* pEvent;
};

void fs(void* p_)
{
	auto _pe = static_cast<pp*>(p_);
	if (!_pe)
	{
		SDL_assert(_pe);
		return;
	}

	auto _pEvent = _pe->pEvent;
	SDL_assert(_pEvent);

	if (_pEvent && _pEvent->key.type == SDL_KEYDOWN)
	{

		auto _pElem = _pe->pe;
		auto _nalpha = _pElem->GetOpacity();

		if (_pEvent->key.keysym.sym == SDLK_w && _nalpha < 0xFF - 10)
			_pElem->SetOpacity(_nalpha + 10);
		else if (_pEvent->key.keysym.sym == SDLK_s && _nalpha > 10)
			_pElem->SetOpacity(_nalpha - 10);
		else if (_pEvent->key.keysym.sym == SDLK_a)
			_pElem->SetAngle(_pElem->GetAngle() + 2);
		else if (_pEvent->key.keysym.sym == SDLK_d)
			_pElem->SetAngle(_pElem->GetAngle() - 2);
	}
}

int main(int argc, char* argv[])
{
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
	GSDL_Init();

	GWindow _win;
	_win.Create("test", {100, 100, 500, 500});
	_win.SetBackgroundColour({0x0, 0x8E, 0x20, 0xFF });


	auto _pelem = CreateElement("Rect");
	_pelem->SetSize(150, 150);
	_pelem->SetPosition(150, 150);
	_pelem->SetColour({ 0xFF, 0xCE, 0x0, 0xFF });
	_pelem->SetAngle(0);
	_pelem->SetOpacity(255);
	_win.RegisterElement(_pelem);

	pp _pe;
	_pe.pe = _pelem;
	_pe.pEvent = _win.GetEvent();

	_pelem = CreateElement("Line");
	_pelem->SetColour({ 0xFF, 0xCE, 0x0, 0xFF });
	_pelem->SetPosition(100, 300);
	_pelem->SetSize(10, 300);
	_win.RegisterElement(_pelem);

	_win.Run(fs, &_pe);

	GSDL_Quit();
	return 0;
}