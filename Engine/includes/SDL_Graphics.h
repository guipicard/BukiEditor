#pragma once
#include "IGraphics.h"
#include <map>
#include <Camera.h>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_FPoint;
typedef struct _TTF_Font TTF_Font;

namespace buki {

	struct Camera;
	struct SDL_Graphics : public IGraphics
	{
		SDL_Graphics();
		virtual bool Initialize(const std::string& title, int w, int h) override;
		virtual void Shutdown() override;
		virtual void SetColor(const Color& color) override;
		virtual void Clear() override;
		virtual void Present() override;
		virtual void DrawRect(float x, float y, float w, float h, const Color& color) override;
		virtual void DrawRect(float x, float y, float w, float h, float angle, const Color& color) override;
		virtual void DrawRect(const RectF& rect, const Color& color) override;
		virtual void FillRect(float x, float y, float w, float h, const Color& color) override;
		virtual void FillRect(float x, float y, float w, float h, float angle, const Color& color) override;
		virtual void FillRect(const RectF& rect, const Color& color) override;
		virtual void DrawLine(float x1, float y1, float x2, float y2, const Color& color) override;
		virtual size_t LoadTexture(const std::string& filename) override;
		virtual void DrawTexture(size_t id, const RectI& src, const RectF& dst, double angle, const Flip& flip, const Color& color) override;
		virtual void DrawTexture(size_t id, const RectI& src, const RectF& dst, const Color& color) override;
		virtual void DrawTexture(size_t id, const RectF& dst, const Color& color) override;
		virtual void DrawTexture(size_t id, const Color& color) override;
		virtual void DrawTiledTexture(size_t id, std::vector<std::vector<RectI>> destinationsSRCs, const Vector2 tileSize, const Vector2 Pos, const Color& color) override;
		virtual void GetTextureSize(size_t id, int* w, int* h) override;
		virtual size_t LoadFont(const std::string& filename, int fontSize) override;
		virtual void DrawString(const std::string& text, size_t fontId, float x, float y, const Color& color) override;
		virtual void DrawString(const std::string& text, size_t fontId, float x, float y, float w, float h, const Color& color) override;
		virtual void GetTextSize(const std::string& text, size_t fontId, float* w, float* h) override;
		virtual void GetTextSizeFixed(const std::string& text, size_t fontId, float* w, float* h) override;
		virtual void DrawTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3) override;
		virtual void FillTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3) override;
		virtual void DrawCircle(float x, float y, float r, float angle, const Color& color) override;
		virtual void FillCircle(float x, float y, float r, float angle, const Color& color) override;
		virtual void DrawPolygon(float x, float y, float r, int sides, const Color& color) override;
		virtual void FillPolygon(float x, float y, float r,float angle, int sides, const Color& color) override;
		virtual void DrawPoint(float x, float y, const Color& color) override;
		virtual void GetWindowSizeScreen(int* w, int* h) override;
		virtual void GetWindowSize(float* w, float* h) override;
		virtual uint32_t GetWindowID() override;
		virtual void GetRenderScale(float* scaleX, float* scaleY) override;
		virtual void SetRenderScale(float scaleX, float scaleY) override;
		virtual void SetCameraPosition(const Vector2 _pos) override;
		virtual Camera* GetCamera() override;
		void DestinationToCamera(RectF* _rect);
		void DestinationToCamera(SDL_FPoint* _point);
		void DestinationToCamera(float* _x, float* _y);
		virtual void AddScale(float _scale) override;
		virtual void SubScale(float _scale) override;
		virtual void GetScale(float* _scale) override;
		virtual void GetScaleFixed(float* _scale) override;
		virtual void SetScale(float _scale) override;
		virtual void ResetScale() override;

		virtual void InitializeImGui() override;
		virtual bool SetImGui() override;
		virtual void DrawImGui() override;


	private:
		SDL_Window* m_Window;
		int m_WindowWidth;
		int m_WindowHeight;
		SDL_Renderer* m_Gfx;
		SDL_Texture* g_TextureBuffer;
		Camera* camera = new Camera();
		std::map<size_t, SDL_Texture*> &m_TextureCache = *new std::map<size_t, SDL_Texture*>;
		std::map<size_t, TTF_Font*> &m_FontCache = *new std::map<size_t, TTF_Font*>;
		float scale;
		const float scaleFixed;

		float main_scale = 0;
		bool show_demo_window = true;
		bool show_another_window = false;
		float imguiColor[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
	};

	
}
