#pragma once

#include <string>
#include <cstdint>
#include "Memory.h"
#include "BukiContainers.h"

namespace buki
{
	class IPlatform;
	struct Texture2D;
	class RenderTarget;
	class Camera2D;

	/*struct TextureDesc
	{
		int width = 0;
		int height = 0;
		int channels = 4;
		const void* pixels = nullptr;
	};*/

	/*struct SpriteDrawCommand
	{
		Ref<Texture2D> texture;
		float x = 0.0f;
		float y = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
		float rotationRadians = 0.0f;
		Color tint{};
	};*/

	

	class IGraphics
	{
	public:
		virtual ~IGraphics() = default;

		virtual bool Initialize(IPlatform& platform) = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Resize(int width, int height) = 0;

		virtual void DrawQuad(float x, float y,
			float width, float height,
			float rotationRadians,
			float originX, float originY,
			float u0, float v0, float u1, float v1,
			float r, float g, float b, float a) = 0;

		virtual std::uint32_t CreateTextureRGBA8(const void* pixels, int width, int height) = 0;
		virtual void DestroyTexture(std::uint32_t textureId) = 0;
		virtual void DrawTexturedQuad(std::uint32_t textureId,
			float x, float y, float width, float height,
			float rotationRadians,
			float originX, float originY,
			float u0, float v0, float u1, float v1,
			float r, float g, float b, float a) = 0;
		virtual void SetCameraPosition(float x, float y) = 0;
		virtual void ResetCamera() = 0;

		virtual void DrawSprite(std::uint32_t textureId,
			float x, float y,
			float width, float height,
			const RectF& sourceRectPixels,
			float textureWidth, float textureHeight,
			float rotationRadians,
			float originX, float originY,
			float r, float g, float b, float a,
			bool flipX, bool flipY) = 0;

		virtual void DrawSprite(const Texture2D& texture,
			float x, float y,
			float width, float height,
			const RectF& sourceRectPixels,
			float rotationRadians,
			float originX, float originY,
			float r, float g, float b, float a,
			bool flipX, bool flipY) = 0;

	};
}


// =========================OLD CODE, NOT USED ANYMORE, KEPT FOR REFERENCE=========================

//#pragma once
//#include <cstdlib>
//#include <vector>
//#include <string>
//#include <functional>
//#include "RectF.h"
//#include "Color.h"
//#include "Flip.h"
//#include "Vertex.h"
//
//struct SDL_Window;
//
//namespace buki {
//	struct Camera;
//	struct RectI;
//	using DrawFunc = std::function<void(bool* b, bool* bA)>;
//	struct IGraphics
//	{
//		virtual bool InitBackend() = 0; // Permet de changer le backend graphique utilisé par l'interface.
//
//		virtual bool Initialize(const std::string& title, int w, int h) = 0; // Initialise le système graphique avec un titre de fenêtre et des dimensions spécifiées.
//
//		virtual void Shutdown() = 0; // Ferme et nettoie le système graphique.
//
//		virtual void SetColor(const Color& color) = 0; // Définit la couleur actuelle pour les opérations de dessin.
//
//		virtual void Clear() = 0; // Efface le buffer d'affichage actuel.
//
//		virtual void Present() = 0; // Présente le buffer actuel à l'écran.
//
//		virtual void DrawRect(float x, float y, float w, float h, const Color& color) = 0; // Dessine un rectangle avec des dimensions et une couleur spécifiées.
//
//		virtual void DrawRect(float x, float y, float w, float h, float angle, const Color& color) = 0; // Dessine un rectangle avec des dimensions et une couleur spécifiées avec angle.
//
//		virtual void DrawRect(const RectF& rect, const Color& color) = 0; // Dessine un rectangle en utilisant un objet RectF et une couleur spécifiée.
//
//		virtual void FillRect(float x, float y, float w, float h, const Color& color) = 0; // Remplit un rectangle avec une couleur.
//
//		virtual void FillRect(float x, float y, float w, float h, float angle, const Color& color) = 0; // Remplit un rectangle avec une couleur et un angle.
//
//		virtual void FillRect(const RectF& rect, const Color& color) = 0; // Remplit un rectangle défini par un RectF avec une couleur spécifiée.
//
//		virtual void DrawLine(float x1, float y1, float x2, float y2, const Color& color) = 0; // Dessine une ligne entre deux points avec une couleur spécifiée.
//
//		virtual size_t LoadTexture(const std::string& filename) = 0; // Charge une texture à partir d'un fichier et retourne son identifiant.
//
//		virtual void DrawTexture(size_t id, const RectI& src, const RectF& dst, double angle, const Flip& flip, const Color& color) = 0; // Dessine une texture avec des options de transformation.
//
//		virtual void DrawTexture(size_t id, const RectI& src, const RectF& dst, const Color& color) = 0;
//
//		virtual void DrawTexture(size_t id, const RectF& dst, const Color& color) = 0; // Dessine une texture dans un rectangle de destination spécifié.
//
//		virtual void DrawTexture(size_t id, const Color& color) = 0; // Dessine une texture avec une couleur spécifiée.
//
//		virtual void DrawTiledTexture(size_t id, std::vector<std::vector<RectI>> destinationsSRCs, const Vector2 tileSize, const Vector2 Pos, const Color& color) = 0; // Dessine une texture en la répétant pour remplir un rectangle de destination spécifié.
//
//		virtual void GetTextureSize(size_t id, int* w, int* h) = 0; // Récupère les dimensions d'une texture.
//
//		virtual size_t LoadFont(const std::string& filename, int fontSize) = 0; // Charge une police à partir d'un fichier et retourne son identifiant.
//
//		virtual void DrawString(const std::string& text, size_t fontId, float x, float y, const Color& color) = 0; // Dessine une chaîne de caractères avec une police et une couleur spécifiées.
//
//		virtual void DrawString(const std::string& text, size_t fontId, float x, float y, float w, float h, const Color& color) = 0; // Dessine une chaîne de caractères avec une police et une couleur spécifiées.
//
//		virtual void GetTextSize(const std::string& text, size_t fontId, float* w, float* h) = 0; // Calcule les dimensions d'une chaîne de texte rendue.
//		virtual void GetTextSizeFixed(const std::string& text, size_t fontId, float* w, float* h) = 0; // Calcule les dimensions d'une chaîne de texte rendue.
//
//		virtual void DrawTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3) = 0; // Dessine un triangle avec 3 points et une couleur spécifiés.
//
//		virtual void FillTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3) = 0; // Dessine un triangle avec 3 points et une couleur spécifiés.
//
//		virtual void DrawCircle(float x, float y, float r, float angle, const Color& color) = 0; // Dessine un cercle avec un rayon et une couleur spécifiés.
//
//		virtual void FillCircle(float x, float y, float r, float angle, const Color& color) = 0; // Remplit un cercle avec un rayon et une couleur spécifiés.
//
//		virtual void DrawPolygon(float x, float y, float r, int sides, const Color& color) = 0; // Dessine un polygon avec un rayon, une couleur et un nb de cotés spécifiés.
//
//		virtual void FillPolygon(float x, float y, float r, float angle, int sides, const Color& color) = 0; // Remplit un polygon avec un rayon, une couleur et un nb de cotés spécifiés.
//
//		virtual void DrawPoint(float x, float y, const Color& color) = 0; // Dessine un point à une position spécifiée.
//
//		virtual void GetWindowSize(float* w, float* h) = 0; // Récupère les dimensions de la fenêtre de rendu.
//
//		virtual void GetWindowSizeScreen(int* w, int* h) = 0; // Récupère les dimensions de la fenêtre de rendu au monde.
//
//		virtual uint32_t GetWindowID() = 0; // returns the id of the window
//
//		virtual void GetRenderScale(float* scaleX, float* scaleY) = 0; // retourne l'échelle du rendu.
//
//		virtual void SetRenderScale(float scaleX, float scaleY) = 0; // Définit l'échelle du rendu.
//
//		virtual void AddScale(float _scale) = 0;
//		virtual void SubScale(float _scale) = 0;
//		virtual void GetScale(float* _scale) = 0;
//		virtual void SetScale(float _scale) = 0;
//		virtual void GetScaleFixed(float* _scale) = 0;
//		virtual void ResetScale() = 0;
//
//		virtual void SetCameraPosition(const Vector2 _pos) = 0;
//		virtual Camera* GetCamera() = 0;
//	};
//}