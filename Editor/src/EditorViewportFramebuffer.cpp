#include "EditorViewportFramebuffer.h"

#include <glad/GLAD.h>

namespace buki
{
	namespace EditorViewportFramebuffer
	{
		bool Initialize(EditorState& state)
		{
			if (state.sceneFramebuffer != 0)
			{
				return true;
			}

			glGenFramebuffers(1, &state.sceneFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, state.sceneFramebuffer);

			glGenTextures(1, &state.sceneColorTexture);
			glBindTexture(GL_TEXTURE_2D, state.sceneColorTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.sceneColorTexture, 0);

			glGenRenderbuffers(1, &state.sceneDepthRenderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, state.sceneDepthRenderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1, 1);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, state.sceneDepthRenderbuffer);

			const bool complete = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			state.sceneFramebufferWidth = 1;
			state.sceneFramebufferHeight = 1;

			return complete;
		}

		void Shutdown(EditorState& state)
		{
			if (state.sceneDepthRenderbuffer != 0)
			{
				glDeleteRenderbuffers(1, &state.sceneDepthRenderbuffer);
				state.sceneDepthRenderbuffer = 0;
			}

			if (state.sceneColorTexture != 0)
			{
				glDeleteTextures(1, &state.sceneColorTexture);
				state.sceneColorTexture = 0;
			}

			if (state.sceneFramebuffer != 0)
			{
				glDeleteFramebuffers(1, &state.sceneFramebuffer);
				state.sceneFramebuffer = 0;
			}

			state.sceneFramebufferWidth = 0;
			state.sceneFramebufferHeight = 0;
		}

		bool Resize(EditorState& state, int width, int height)
		{
			if (width <= 0 || height <= 0)
			{
				return false;
			}

			if (state.sceneFramebuffer == 0)
			{
				if (!Initialize(state))
				{
					return false;
				}
			}

			if (state.sceneFramebufferWidth == width && state.sceneFramebufferHeight == height)
			{
				return true;
			}

			state.sceneFramebufferWidth = width;
			state.sceneFramebufferHeight = height;

			glBindTexture(GL_TEXTURE_2D, state.sceneColorTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, state.sceneDepthRenderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, state.sceneFramebuffer);
			const bool complete = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return complete;
		}

		void Bind(const EditorState& state)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, state.sceneFramebuffer);
			glViewport(0, 0, state.sceneFramebufferWidth, state.sceneFramebufferHeight);
		}

		void Unbind()
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}