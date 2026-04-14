//#pragma once
//#include "SDL_Graphics.h"
//#include "SDL.h"
//#include "Engine.h"
//#include <SDL_image.h>
//#include <SDL_ttf.h>
//#include <math.h>
//#include <IScene.h>
//#include "RectI.h"
//#include "Editor.h"
//
//#if !SDL_VERSION_ATLEAST(2,0,17)
//#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
//#endif
//
//buki::SDL_Graphics::SDL_Graphics()
//	: m_Gfx(nullptr)
//	, m_Window(nullptr)
//	, g_TextureBuffer(nullptr)
//	, m_WindowWidth(0)
//	, m_WindowHeight(0)
//	, scale(METRES_TO_PIXELS)
//	, scaleFixed(METRES_TO_PIXELS)
//{
//}
//
//bool buki::SDL_Graphics::InitBackend()
//{
//	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
//		buki::Engine::GetInstance().Log().LogSdlError();
//		return false;
//	}
//	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
//	return true;
//}
//
//bool buki::SDL_Graphics::Initialize(const std::string& title, int w, int h)
//{
//	int _x SDL_WINDOWPOS_CENTERED;
//	int _y = SDL_WINDOWPOS_CENTERED;
//	Uint32 _windowFlag = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
//	Uint32 _rendererFlag = (SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
//	m_WindowWidth = w;
//	m_WindowHeight = h;
//
//	Editor* editor = Engine::GetInstance().TheEditor();
//
//	if (editor)
//	{
//		m_Window = editor->GetGameWindow();
//		m_Gfx = editor->GetGameRenderer();
//	}
//	else
//	{
//		m_Window = SDL_CreateWindow(title.c_str(), _x, _y, (int)w, (int)h, _windowFlag);
//		if (!m_Window)
//		{
//			buki::Engine::GetInstance().Log().LogSdlError();
//			return false;
//		}
//		buki::Engine::GetInstance().Log().LogSuccess("Window initialised");
//		m_Gfx = SDL_CreateRenderer(m_Window, -1, _rendererFlag);
//		if (!m_Gfx)
//		{
//			buki::Engine::GetInstance().Log().LogSdlError();
//			return false;
//		}
//		buki::Engine::GetInstance().Log().LogSuccess("Renderer initialised");
//		int blendmode = SDL_SetRenderDrawBlendMode(m_Gfx, SDL_BlendMode::SDL_BLENDMODE_BLEND);
//		if (blendmode != 0)
//		{
//			buki::Engine::GetInstance().Log().LogSdlError();
//			return false;
//		}
//	}
//
//	TTF_Init();
//	camera->width = w;
//	camera->height = h;
//
//	return true;
//}
//
//void buki::SDL_Graphics::Shutdown()
//{
//	for (std::map<size_t, SDL_Texture*>::iterator it = m_TextureCache.begin(); it != m_TextureCache.end(); ++it)
//	{
//		if (it->second != nullptr)
//		{
//			SDL_DestroyTexture(it->second);
//			it->second = nullptr;
//		}
//	}
//	delete& m_TextureCache;
//
//	for (std::map<size_t, TTF_Font*>::iterator it = m_FontCache.begin(); it != m_FontCache.end(); ++it)
//	{
//		if (it->second != nullptr)
//		{
//			TTF_CloseFont(it->second);
//			it->second = nullptr;
//		}
//	}
//
//	TTF_Quit();
//	delete& m_FontCache;
//	delete camera;
//	camera = nullptr;
//
//	if (g_TextureBuffer != nullptr)
//	{
//		SDL_DestroyTexture(g_TextureBuffer);
//		g_TextureBuffer = nullptr;
//	}
//
//	if (m_Gfx != nullptr)
//	{
//		SDL_DestroyRenderer(m_Gfx);
//		m_Gfx = nullptr;
//	}
//
//	if (m_Window != nullptr)
//	{
//		SDL_DestroyWindow(m_Window);
//		m_Window = nullptr;
//	}
//
//
//}
//
//void buki::SDL_Graphics::SetColor(const Color& color)
//{
//	SDL_SetRenderDrawColor(m_Gfx, color.r, color.g, color.b, color.a);
//}
//
//void buki::SDL_Graphics::Clear()
//{
//	SetColor(Color::PINK);
//	SDL_RenderClear(m_Gfx);
//}
//
//void buki::SDL_Graphics::Present()
//{
//	RectF fDestRect{ 0.0f,0.0f, (float)m_WindowWidth, (float)m_WindowHeight };
//
//	SDL_Rect destRect{ static_cast<int>(fDestRect.x), static_cast<int>(fDestRect.y), static_cast<int>(fDestRect.w), static_cast<int>(fDestRect.h) };
//	//SDL_RenderSetLogicalSize(m_Gfx, static_cast<int>(fDestRect.w), static_cast<int>(fDestRect.h));
//	SDL_RenderPresent(m_Gfx);
//}
//
//void buki::SDL_Graphics::DrawRect(float x, float y, float w, float h, const Color& color)
//{
//	RectF camDst;
//	camDst.x = x * scale;
//	camDst.y = y * scale;
//	camDst.w = w * scale;
//	camDst.h = h * scale;
//	DestinationToCamera(&camDst);
//
//	float _w = camDst.w * 0.5f;
//	float _h = camDst.h * 0.5f;
//
//	SDL_Rect _rect{ 0 };
//	_rect.x = static_cast<int>(camDst.x - _w);
//	_rect.y = static_cast<int>(camDst.y - _h);
//	_rect.w = static_cast<int>(camDst.w);
//	_rect.h = static_cast<int>(camDst.h);
//	SetColor(color);
//	SDL_RenderDrawRect(m_Gfx, &_rect);
//}
//
//void buki::SDL_Graphics::DrawRect(float x, float y, float w, float h, float angle, const Color& color)
//{
//	RectF camDst;
//	camDst.x = x * scale;
//	camDst.y = y * scale;
//	camDst.w = w * scale;
//	camDst.h = h * scale;
//	DestinationToCamera(&camDst);
//
//	std::vector<SDL_FPoint> corners = { {-camDst.w,-camDst.h}, {camDst.w, -camDst.h}, {camDst.w,camDst.h}, {-camDst.w, camDst.h} };
//	std::vector<SDL_FPoint> rotatedCorners;
//
//	float angleSin = sin(angle);
//	float angleCos = cos(angle);
//
//	SDL_FPoint textCord = { 1.0f, 1.0f };
//	SDL_Color sdlColor = { color.r, color.g, color.b, color.a };
//
//	for each(auto corner in corners)
//	{
//		SDL_FPoint newCorner;
//		newCorner.x = corner.x * angleCos - corner.y * angleSin;
//		newCorner.y = corner.x * angleSin + corner.y * angleCos;
//		newCorner.x += camDst.x;
//		newCorner.y += camDst.y;
//		rotatedCorners.push_back(SDL_FPoint{ newCorner.x, newCorner.y });
//	}
//	DrawLine(rotatedCorners[0].x, rotatedCorners[0].y, rotatedCorners[1].x, rotatedCorners[1].y, color);
//	DrawLine(rotatedCorners[1].x, rotatedCorners[1].y, rotatedCorners[2].x, rotatedCorners[2].y, color);
//	DrawLine(rotatedCorners[2].x, rotatedCorners[2].y, rotatedCorners[3].x, rotatedCorners[3].y, color);
//	DrawLine(rotatedCorners[3].x, rotatedCorners[3].y, rotatedCorners[0].x, rotatedCorners[0].y, color);
//}
//
//void buki::SDL_Graphics::DrawRect(const RectF& rect, const Color& color)
//{
//	/*RectF camDst;
//	camDst.x = rect.x * scale;
//	camDst.y = rect.y * scale;
//	camDst.w = rect.w * scale;
//	camDst.h = rect.h * scale;
//	DestinationToCamera(&camDst);
//
//	float _w = rect.w * 0.5f;
//	float _h = rect.h * 0.5f;
//	SDL_Rect _rect{ 0 };
//	_rect.x = static_cast<int>((rect.x - _w) * scale);
//	_rect.y = static_cast<int>((rect.y - _h) * scale);
//	_rect.h = static_cast<int>(rect.h * scale);
//	_rect.w = static_cast<int>(rect.w * scale);
//	SetColor(color);
//	SDL_RenderDrawRect(m_Gfx, &_rect);*/
//}
//
//void buki::SDL_Graphics::FillRect(float x, float y, float w, float h, const Color& color)
//{
//	RectF camDst;
//	camDst.x = x * scale;
//	camDst.y = y * scale;
//	camDst.w = w * scale;
//	camDst.h = h * scale;
//	DestinationToCamera(&camDst);
//
//	float _w = camDst.w * 0.5f;
//	float _h = camDst.h * 0.5f;
//	SDL_Rect _rect{ 0 };
//	_rect.x = static_cast<int>((camDst.x - _w));
//	_rect.y = static_cast<int>((camDst.y - _h));
//	_rect.h = static_cast<int>(camDst.h);
//	_rect.w = static_cast<int>(camDst.w);
//	SetColor(color);
//	SDL_RenderFillRect(m_Gfx, &_rect);
//}
//
//void buki::SDL_Graphics::FillRect(float x, float y, float w, float h, float angle, const Color& color)
//{
//	RectF camDst;
//	camDst.x = x * scale;
//	camDst.y = y * scale;
//	camDst.w = w * scale;
//	camDst.h = h * scale;
//	DestinationToCamera(&camDst);
//
//	std::vector<SDL_FPoint> corners = { {-camDst.w,-camDst.h}, {camDst.w, -camDst.h}, {camDst.w,camDst.h}, {-camDst.w, camDst.h} };
//	std::vector<SDL_FPoint> rotatedCorners;
//
//	float angleSin = sin(angle);
//	float angleCos = cos(angle);
//
//	SDL_FPoint textCord = { 1.0f, 1.0f };
//	SDL_Color sdlColor = { color.r, color.g, color.b, color.a };
//
//	for each(auto corner in corners)
//	{
//		SDL_FPoint newCorner;
//		newCorner.x = corner.x * angleCos - corner.y * angleSin;
//		newCorner.y = corner.x * angleSin + corner.y * angleCos;
//		newCorner.x += camDst.x;
//		newCorner.y += camDst.y;
//		rotatedCorners.push_back(SDL_FPoint{ newCorner.x, newCorner.y });
//	}
//
//	std::vector<SDL_Vertex> triangles{
//		SDL_Vertex{ rotatedCorners[0], sdlColor, textCord },
//		SDL_Vertex{ rotatedCorners[1], sdlColor, textCord },
//		SDL_Vertex{ rotatedCorners[2], sdlColor, textCord },
//		SDL_Vertex{ rotatedCorners[0], sdlColor, textCord },
//		SDL_Vertex{ rotatedCorners[2], sdlColor, textCord },
//		SDL_Vertex{ rotatedCorners[3], sdlColor, textCord }
//	};
//
//	SDL_RenderGeometry(m_Gfx, nullptr, triangles.data(), (int)triangles.size(), nullptr, 0);
//}
//
//void buki::SDL_Graphics::FillRect(const RectF& rect, const Color& color)
//{
//	FillRect(rect.x, rect.y, rect.w, rect.h, color);
//}
//
//void buki::SDL_Graphics::DrawLine(float x1, float y1, float x2, float y2, const Color& color)
//{
//	int _x1 = static_cast<int>(x1);
//	int _x2 = static_cast<int>(x2);
//	int _y1 = static_cast<int>(y1);
//	int _y2 = static_cast<int>(y2);
//	SetColor(color);
//	SDL_RenderDrawLine(m_Gfx, _x1, _y1, _x2, _y2);
//}
//
//size_t buki::SDL_Graphics::LoadTexture(const std::string& filename)
//{
//	const size_t id = std::hash<std::string>()(filename);
//	if (m_TextureCache.count(id) > 0)
//	{
//		return id;
//	}
//
//	SDL_Texture* texture = IMG_LoadTexture(m_Gfx, filename.c_str());
//	if (texture != nullptr)
//	{
//		m_TextureCache[id] = texture;
//		return id;
//	}
//	buki::Engine::GetInstance().Log().LogError("did not load image");
//	return -1;
//}
//
//void buki::SDL_Graphics::DrawTexture(size_t id, const RectI& src, const RectF& dst, double angle, const Flip& flip, const Color& color)
//{
//	SDL_Texture* _texture = m_TextureCache[id];
//
//	RectF camDst = dst;
//	camDst.x *= scale;
//	camDst.y *= scale;
//	camDst.w *= scale;
//	camDst.h *= scale;
//	DestinationToCamera(&camDst);
//
//	SDL_Rect source{ 0 };
//	source.x = src.x;
//	source.y = src.y;
//	source.h = src.h;
//	source.w = src.w;
//
//	SDL_Rect dest{ 0 };
//	dest.x = static_cast<int>(camDst.x);
//	dest.y = static_cast<int>(camDst.y);
//	dest.h = static_cast<int>(camDst.h);
//	dest.w = static_cast<int>(camDst.w);
//
//	SetColor(color);
//
//	SDL_RendererFlip _flip;
//	if (flip.h)
//	{
//		_flip = SDL_FLIP_HORIZONTAL;
//	}
//	else if (flip.v)
//	{
//		_flip = SDL_FLIP_VERTICAL;
//	}
//	else
//	{
//		_flip = SDL_FLIP_NONE;
//	}
//
//	SDL_RenderCopyEx(m_Gfx, _texture, &source, &dest, Vector2::GetRotationDegree(angle), nullptr, _flip);
//}
//
//void buki::SDL_Graphics::DrawTexture(size_t id, const RectI& src, const RectF& dst, const Color& color)
//{
//	SDL_Texture* _texture = m_TextureCache[id];
//
//	RectF camDst = dst;
//	camDst.x *= scale;
//	camDst.y *= scale;
//	camDst.w *= scale;
//	camDst.h *= scale;
//	DestinationToCamera(&camDst);
//
//	SDL_Rect source{ 0 };
//	source.x = src.x;
//	source.y = src.y;
//	source.h = src.h;
//	source.w = src.w;
//
//	SDL_Rect dest{ 0 };
//	dest.x = static_cast<int>(camDst.x);
//	dest.y = static_cast<int>(camDst.y);
//	dest.h = static_cast<int>(camDst.h);
//	dest.w = static_cast<int>(camDst.w);
//
//	SetColor(color);
//
//	SDL_RenderCopyEx(m_Gfx, _texture, &source, &dest, 0.0, nullptr, SDL_FLIP_NONE);
//}
//
//void buki::SDL_Graphics::DrawTexture(size_t id, const RectF& dst, const Color& color)
//{
//	SDL_Texture* _texture = m_TextureCache[id];
//	RectF camDst = dst;
//	camDst.x *= scale;
//	camDst.y *= scale;
//	camDst.w *= scale;
//	camDst.h *= scale;
//	DestinationToCamera(&camDst);
//	SDL_Rect dstrect{ 0 };
//	dstrect.x = static_cast<int>(dst.x);
//	dstrect.y = static_cast<int>(dst.y);
//	dstrect.h = static_cast<int>(dst.h);
//	dstrect.w = static_cast<int>(dst.w);
//
//	SetColor(color);
//
//	SDL_RenderCopyEx(m_Gfx, _texture, nullptr, &dstrect, 0, nullptr, SDL_FLIP_NONE);
//}
//
//void buki::SDL_Graphics::DrawTexture(size_t id, const Color& color)
//{
//	SDL_Texture* _texture = m_TextureCache[id];
//	SDL_Rect _rect{ 0 };
//	_rect.x = 0;
//	_rect.y = 0;
//	GetTextureSize(id, &_rect.w, &_rect.h);
//	SetColor(color);
//
//	SDL_RenderCopyEx(m_Gfx, _texture, nullptr, &_rect, 0, nullptr, SDL_FLIP_NONE);
//}
//
//void buki::SDL_Graphics::DrawTiledTexture(size_t id, std::vector<std::vector<RectI>> destinationsSRCs, const Vector2 tileSize, const Vector2 Pos, const Color& color)
//{
//	SDL_Texture* _texture = m_TextureCache[id];
//
//	// PIXEL SIZE OF EACH TILE
//	int textureWidth, textureHeight;
//	GetTextureSize(id, &textureWidth, &textureHeight);
//	RectF scaledRect = { Pos.x * scale, Pos.y * scale, tileSize.x * scale, tileSize.y * scale };
//	DestinationToCamera(&scaledRect);
//
//	SDL_Rect dest{ 0 };
//	dest.x = static_cast<int>(scaledRect.x);
//	dest.y = static_cast<int>(scaledRect.y);
//	dest.h = static_cast<int>(scaledRect.h);
//	dest.w = static_cast<int>(scaledRect.w);
//	SetColor(color);
//	for (size_t i = 0; i < destinationsSRCs.size(); i++)
//	{
//		for (size_t j = 0; j < destinationsSRCs[i].size(); j++)
//		{
//			RectI source = destinationsSRCs[i][j];
//			SDL_Rect src{ source.x, source.y, source.w, source.h };
//			SDL_RenderCopyEx(m_Gfx, _texture, &src, &dest, 0.0, nullptr, SDL_FLIP_NONE);
//
//			dest.x += dest.w;
//		}
//		dest.y += dest.h;
//	}
//}
//
//void buki::SDL_Graphics::GetTextureSize(size_t id, int* w, int* h)
//{
//	if (m_TextureCache.count(id) > 0)
//	{
//		SDL_Texture* _tex = m_TextureCache[id];
//		SDL_QueryTexture(_tex, nullptr, nullptr, w, h);
//	}
//	else
//	{
//		*w = 0;
//		*h = 0;
//	}
//}
//
//size_t buki::SDL_Graphics::LoadFont(const std::string& filename, int fontSize)
//{
//	const size_t id = std::hash<std::string>()(filename + std::to_string(fontSize));
//	if (m_FontCache.count(id) > 0)
//	{
//		return id;
//	}
//	TTF_Font* _font = TTF_OpenFont(filename.c_str(), fontSize);
//	if (_font != nullptr)
//	{
//		m_FontCache[id] = _font;
//		return id;
//	}
//	return -1;
//}
//
//void buki::SDL_Graphics::DrawString(const std::string& text, size_t fontId, float x, float y, const Color& color)
//{
//	SDL_Color _color = { color.r, color.g, color.b, color.a };
//	float w, h;
//	GetTextSize(text, fontId, &w, &h);
//	RectF dest = { (x - (w / 2)) * scale, (y - (h / 2)) * scale, w * scale, h * scale };
//	DestinationToCamera(&dest);
//	SDL_Rect _dst = { (int)dest.x, (int)dest.y, (int)dest.w, (int)dest.h };
//	if (m_FontCache.count(fontId) > 0)
//	{
//		TTF_Font* _font = m_FontCache[fontId];
//		SDL_Surface* _surface = TTF_RenderText_Solid(_font, text.c_str(), _color);
//
//		SDL_Texture* textureBuffer = SDL_CreateTextureFromSurface(m_Gfx, _surface);
//		SDL_RenderCopy(m_Gfx, textureBuffer, nullptr, &_dst);
//		SDL_FreeSurface(_surface);
//		SDL_DestroyTexture(textureBuffer);
//	}
//}
//
//void buki::SDL_Graphics::DrawString(const std::string& text, size_t fontId, float x, float y, float w, float h, const Color& color)
//{
//	SDL_Color _color = { color.r, color.g, color.b, color.a };
//	RectF dest = { (x - (w / 2)) * scale, (y - (h / 2)) * scale, w * scale, h * scale };
//	DestinationToCamera(&dest);
//	SDL_Rect _dst = { (int)dest.x, (int)dest.y, (int)dest.w, (int)dest.h };
//	if (m_FontCache.count(fontId) > 0)
//	{
//		TTF_Font* _font = m_FontCache[fontId];
//		SDL_Surface* _surface = TTF_RenderText_Solid(_font, text.c_str(), _color);
//
//		SDL_Texture* textureBuffer = SDL_CreateTextureFromSurface(m_Gfx, _surface);
//		SDL_RenderCopy(m_Gfx, textureBuffer, nullptr, &_dst);
//		SDL_FreeSurface(_surface);
//		SDL_DestroyTexture(textureBuffer);
//	}
//}
//
//void buki::SDL_Graphics::GetTextSize(const std::string& text, size_t fontId, float* w, float* h)
//{
//	*w = 0.0f;
//	*h = 0.0f;
//	int _w, _h;
//	if (m_FontCache.count(fontId) > 0)
//	{
//		TTF_Font* _font = m_FontCache[fontId];
//		TTF_SizeText(_font, text.c_str(), &_w, &_h);
//		*w = (float)_w / scale;
//		*h = (float)_h / scale;
//	}
//}
//
//void buki::SDL_Graphics::GetTextSizeFixed(const std::string& text, size_t fontId, float* w, float* h)
//{
//	*w = 0.0f;
//	*h = 0.0f;
//	int _w, _h;
//	if (m_FontCache.count(fontId) > 0)
//	{
//		TTF_Font* _font = m_FontCache[fontId];
//		TTF_SizeText(_font, text.c_str(), &_w, &_h);
//		*w = (float)_w / scaleFixed;
//		*h = (float)_h / scaleFixed;
//	}
//}
//
//void buki::SDL_Graphics::DrawTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3)
//{
//	Vector2 position1, position2, position3, size1, size2, size3;
//	Color color1, color2, color3;
//
//	vertex1.GetData(position1 * scale, color1, size1);
//	vertex2.GetData(position2 * scale, color2, size2);
//	vertex3.GetData(position3 * scale, color3, size3);
//
//	DrawLine(position1.x, position1.y, position2.x, position2.y, color1);
//	DrawLine(position2.x, position2.y, position3.x, position3.y, color1);
//	DrawLine(position3.x, position3.y, position1.x, position1.y, color1);
//}
//
//void buki::SDL_Graphics::FillTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3)
//{
//	Vector2 position1, position2, position3, size1, size2, size3;
//	Color color1, color2, color3;
//
//	vertex1.GetData(position1 * scale, color1, size1);
//	vertex2.GetData(position2 * scale, color2, size2);
//	vertex3.GetData(position3 * scale, color3, size3);
//
//	const std::vector<SDL_Vertex> verts =
//	{
//		{ SDL_FPoint{ position1.x, position1.y }, SDL_Color{ color1.r, color1.g, color1.b, color1.a }, SDL_FPoint{ size1.x, size1.y }, },
//		{ SDL_FPoint{ position2.x, position2.y }, SDL_Color{ color2.r, color2.g, color2.b, color2.a }, SDL_FPoint{ size2.x, size2.y }, },
//		{ SDL_FPoint{ position3.x, position3.y }, SDL_Color{ color3.r, color3.g, color3.b, color3.a }, SDL_FPoint{ size3.x, size3.y }, },
//	};
//
//	SDL_RenderGeometry(m_Gfx, nullptr, verts.data(), static_cast<int>(verts.size()), nullptr, 0);
//}
//
//void buki::SDL_Graphics::DrawCircle(float x, float y, float r, float angle, const Color& color)
//{
//	float _x = x * scale;
//	float _y = y * scale;
//	float _r = r * scale;
//	DestinationToCamera(&_x, &_y);
//	const int triangleCount = 32;
//	//int triangleCount = (int)((64.0f * (_r * 50.0f)) / 100.0f);
//	//if (triangleCount < 16) triangleCount = 16;
//	const float offsetAngle = 6.28319f / triangleCount;
//
//
//	std::vector<SDL_FPoint> temp;
//	// positions
//	for (int i = 0; i < triangleCount; i++)
//	{
//		float currentAngle = (offsetAngle * i) + angle;
//		float cx = _r * cos(currentAngle);
//		float sy = _r * sin(currentAngle);
//		SDL_FPoint point = SDL_FPoint{ cx + _x, sy + _y };
//		temp.push_back(point);
//	}
//	// triangles
//	for (int i = triangleCount; i > 1; i--)
//	{
//		int first = i - 1;
//		int second = i - 2;
//		DrawLine(temp[first].x, temp[first].y, temp[second].x, temp[second].y, color);
//	}
//	const int firstPoint = triangleCount - 1;
//	DrawLine(temp[0].x, temp[0].y, temp[firstPoint].x, temp[firstPoint].y, color);
//}
//
//void buki::SDL_Graphics::FillCircle(float x, float y, float r, float angle, const Color& color)
//{
//	FillPolygon(x, y, r, angle, 32, color);
//	//FillPolygon(x, y, r, angle, (int)((64.0f * (r * 50.0f)) / 100.0f), color);
//}
//
//void buki::SDL_Graphics::DrawPolygon(float x, float y, float r, int sides, const Color& color)
//{
//	float _x, _y, _r;
//	_x = x * scale;
//	_y = y * scale;
//	_r = r * scale;
//
//	SDL_Color polygonColor = SDL_Color{ color.r, color.g, color.b, color.a };
//	SDL_FPoint textureSize = SDL_FPoint{ 1.0f, 1.0f };
//
//	std::vector<SDL_Vertex> verts = std::vector<SDL_Vertex>();
//
//	float fullCircle = PI * 2.0f;
//
//	float tx = _x;
//	float ty = _y;
//	float lastX = static_cast<float>(_x + (_r - 1) * sin((fullCircle / sides) * 0));
//	float lastY = static_cast<float>(_y - _r * cos((fullCircle / sides) * 0));
//	float firstX = lastX;
//
//	SDL_Vertex centerVertex = SDL_Vertex{ SDL_FPoint{ _x, _y }, polygonColor, textureSize };
//	for (int i = 1; i <= sides + 1; i++)
//	{
//		tx = static_cast<float>(_x + _r * sin((fullCircle / sides) * i));
//		ty = static_cast<float>(_y - _r * cos((fullCircle / sides) * i));
//
//		DrawLine(lastX, lastY, tx, ty, color);
//
//		lastX = tx;
//		lastY = ty;
//	}
//
//
//}
//
//void buki::SDL_Graphics::FillPolygon(float x, float y, float r, float angle, int sides, const Color& color)
//{
//
//	RectF camDst;
//	camDst.x = x * scale;
//	camDst.y = y * scale;
//	camDst.w = r * scale;
//	camDst.h = r * scale;
//	DestinationToCamera(&camDst);
//
//	int _sides = sides;
//	if (sides < 16) _sides = 16;
//	const SDL_Color polygonColor = SDL_Color{ color.r, color.g, color.b, color.a };
//	const SDL_FPoint textureSize = SDL_FPoint{ 0.0f };
//
//	std::vector<SDL_Vertex> verts = std::vector<SDL_Vertex>();
//
//	float offsetAngle = 6.28319f / _sides;
//
//	int triangleCount = _sides - 2;
//
//	std::vector<SDL_FPoint> temp;
//	// positions
//	for (int i = 0; i < _sides; i++)
//	{
//		float currentAngle = (offsetAngle * i) + angle;
//		float cx = camDst.w * cos(currentAngle);
//		float sy = camDst.w * sin(currentAngle);
//
//		temp.push_back(SDL_FPoint{ cx + camDst.x, sy + camDst.y });
//	}
//	// triangles
//	for (int i = 0; i < triangleCount; i++)
//	{
//		verts.push_back(SDL_Vertex{ temp[0], polygonColor, textureSize });
//		verts.push_back(SDL_Vertex{ temp[i + 1], polygonColor, textureSize });
//		verts.push_back(SDL_Vertex{ temp[i + 2], polygonColor, textureSize });
//	}
//
//	SDL_RenderGeometry(m_Gfx, nullptr, verts.data(), static_cast<int>(verts.size()), nullptr, 0);
//	DrawLine(temp[0].x, temp[0].y, camDst.x, camDst.y, Color::RED);
//}
//
//void buki::SDL_Graphics::DrawPoint(float x, float y, const Color& color)
//{
//	//const int _x = static_cast<int>(x * scale);
//	//const int _y = static_cast<int>(y * scale);
//	const int _x = static_cast<int>(x);
//	const int _y = static_cast<int>(y);
//	SDL_SetRenderDrawColor(m_Gfx, color.r, color.g, color.b, color.a);
//	SDL_RenderDrawPoint(m_Gfx, _x, _y);
//}
//
//void buki::SDL_Graphics::GetWindowSizeScreen(int* w, int* h)
//{
//	SDL_GetWindowSize(m_Window, w, h);
//}
//
//void buki::SDL_Graphics::GetWindowSize(float* w, float* h)
//{
//	int _w, _h;
//	SDL_GetWindowSize(m_Window, &_w, &_h);
//	*w = (float)_w / scale;
//	*h = (float)_h / scale;
//}
//
//uint32_t buki::SDL_Graphics::GetWindowID()
//{
//	return SDL_GetWindowID(m_Window);
//}
//
//void buki::SDL_Graphics::GetRenderScale(float* scaleX, float* scaleY)
//{
//	//SDL_RenderGetScale(m_Gfx, scaleX, scaleY);
//}
//
//void buki::SDL_Graphics::SetRenderScale(float scaleX, float scaleY)
//{
//	//SDL_RenderSetScale(m_Gfx, scaleX, scaleY);
//}
//
//void buki::SDL_Graphics::SetCameraPosition(const Vector2 _pos)
//{
//	camera->position = _pos;
//}
//
//buki::Camera* buki::SDL_Graphics::GetCamera()
//{
//	return camera;
//}
//
//void buki::SDL_Graphics::DestinationToCamera(RectF* _rect)
//{
//	Vector2 camPos = camera->position * scale;
//	Vector2 camSize = Vector2(camera->width, camera->height);
//	_rect->x -= camPos.x - (camSize.x * 0.5f);
//	_rect->y -= camPos.y - (camSize.y * 0.5f);
//}
//
//void buki::SDL_Graphics::DestinationToCamera(SDL_FPoint* _point)
//{
//	Vector2 camPos = camera->position * scale;
//	Vector2 camSize = Vector2(camera->width, camera->height);
//	_point->x -= camPos.x - (camSize.x * 0.5f);
//	_point->y -= camPos.y - (camSize.y * 0.5f);
//}
//
//void buki::SDL_Graphics::DestinationToCamera(float* _x, float* _y)
//{
//	Vector2 camPos = camera->position * scale;
//	Vector2 camSize = Vector2(camera->width, camera->height);
//	*_x -= camPos.x - (camSize.x * 0.5f);
//	*_y -= camPos.y - (camSize.y * 0.5f);
//}
//
//void buki::SDL_Graphics::AddScale(float _scale)
//{
//	scale += _scale;
//}
//
//void buki::SDL_Graphics::SubScale(float _scale)
//{
//	if (scale - _scale > 0.0f)
//	{
//		scale -= _scale;
//	}
//}
//
//void buki::SDL_Graphics::GetScale(float* _scale)
//{
//	*_scale = scale;
//}
//
//void buki::SDL_Graphics::GetScaleFixed(float* _scale)
//{
//	*_scale = scaleFixed;
//}
//
//void buki::SDL_Graphics::SetScale(float _scale)
//{
//	if (_scale > 0.0f)
//	{
//		scale = _scale;
//	}
//}
//
//void buki::SDL_Graphics::ResetScale()
//{
//	scale = scaleFixed;
//}