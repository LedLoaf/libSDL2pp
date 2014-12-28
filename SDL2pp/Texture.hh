/*
  libSDL2pp - C++11 bindings/wrapper for SDL2
  Copyright (C) 2013-2014 Dmitry Marakasov <amdmi3@amdmi3.ru>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SDL2PP_TEXTURE_HH
#define SDL2PP_TEXTURE_HH

#include <string>

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_blendmode.h>

#include <SDL2pp/Optional.hh>
#include <SDL2pp/Rect.hh>
#include <SDL2pp/Config.hh>

struct SDL_Texture;

namespace SDL2pp {

class Renderer;
class RWops;
class Surface;

////////////////////////////////////////////////////////////
/// \brief Image stored in the graphics card memory that
///        can be used for fast drawing
///
/// \ingroup rendering
///
/// \headerfile SDL2pp/Texture.hh
///
////////////////////////////////////////////////////////////
class Texture {
private:
	SDL_Texture* texture_; ///< Contained SDL_Texture structure

public:
	////////////////////////////////////////////////////////////
	/// \brief SDL2pp::Texture lock
	/// \ingroup rendering
	///
	/// \details
	/// Textures with SDL_TEXTUREACCESS_STREAMING access mode may
	/// be locked, which provides (writeonly) access to their raw
	/// pixel data. This may be used to update texture contents.
	///
	/// This class represents the lock and controls its lifetime
	/// as the lock is released as soon as LockHandle is destroyed.
	///
	/// Usage example:
	/// \code
	/// {
	///     SDL2pp::Texture tex(SDL_PIXELFORMAT_RGB24,
	///                         SDL_TEXTUREACCESS_STREAMING,
	///                         256, 256);
	///     {
	///         // Lock the whole texture
	///         SDL2pp::Texture::LockHandle lock = tex.Lock();
	///
	///         unsigned char* start = static_cast<unsigned char*>(lock.GetPixels());
	///
	///         // note that we use lock.GetPitch(), not tex.GetWidth() here
	///         // as texture may have different dimensions in memory
	///         unsigned char* end = start + tex.GetHeight() * lock.GetPitch();
	///
	///         // fill the texture white
	///         std::fill(start, end, 255);
	///     }
	///     // At this point lock is released
	/// }
	/// \endcode
	///
	////////////////////////////////////////////////////////////
	class LockHandle {
		friend class Texture;
	private:
		Texture* texture_; ///< SDL2pp::Texture this lock belongs to
		void* pixels_;     ///< Pointer to raw pixel data of locked region
		int pitch_;        ///< Number of bytes in a row of pixel data, including padding between lines

	private:
		////////////////////////////////////////////////////////////
		/// \brief Create lock for specific SDL2pp::Texture
		///
		/// \param rect Specifies region to lock
		///
		/// \throws STL2pp::Exception
		///
		/// \see http://wiki.libsdl.org/SDL_LockTexture
		///
		////////////////////////////////////////////////////////////
		LockHandle(Texture* texture, const Optional<Rect>& rect);

	public:
		////////////////////////////////////////////////////////////
		/// \brief Create no-op lock
		///
		/// \details
		/// This may be initialized with real lock later with move
		/// assignment operator
		///
		////////////////////////////////////////////////////////////
		LockHandle();

		////////////////////////////////////////////////////////////
		/// \brief Destructor
		///
		/// \details
		/// Releases the lock
		///
		/// \see http://wiki.libsdl.org/SDL_UnlockTexture
		///
		////////////////////////////////////////////////////////////
		~LockHandle();

		////////////////////////////////////////////////////////////
		/// \brief Move constructor
		///
		/// \param other SDL2pp::Texture::LockHandle to move data from
		///
		////////////////////////////////////////////////////////////
		LockHandle(LockHandle&& other) noexcept;

		////////////////////////////////////////////////////////////
		/// \brief Move assignment operator
		///
		/// \param other SDL2pp::Texture::LockHandle to move data from
		///
		/// \returns Reference to self
		///
		////////////////////////////////////////////////////////////
		LockHandle& operator=(LockHandle&& other) noexcept;

		// Deleted copy constructor and assignment
		LockHandle(const LockHandle& other) = delete;
		LockHandle& operator=(const LockHandle& other) = delete;

		////////////////////////////////////////////////////////////
		/// \brief Get pointer to raw pixel data of locked region
		///
		/// \returns Pointer to raw pixel data of locked region
		///
		////////////////////////////////////////////////////////////
		void* GetPixels() const;

		////////////////////////////////////////////////////////////
		/// \brief Get pitch of locked pixel data
		///
		/// \returns Number of bytes in a row of pixel data, including
		///          padding between lines
		///
		////////////////////////////////////////////////////////////
		int GetPitch() const;
	};

public:
    ////////////////////////////////////////////////////////////
	/// \brief Construct from existing SDL_Texture structure
	///
	/// \param texture Existing SDL_Texture to manage
	///
	////////////////////////////////////////////////////////////
	Texture(SDL_Texture* texture);

	////////////////////////////////////////////////////////////
	/// \brief Create empty texture
	///
	/// \param renderer Rendering context to create texture for
	/// \param format One of the enumerated values in SDL_PixelFormatEnum
	/// \param access One of the enumerated values in SDL_TextureAccess
	/// \param w With of the texture in pixels
	/// \param h Height of the texture in pixels
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_CreateTexture
	///
	////////////////////////////////////////////////////////////
	Texture(Renderer& renderer, Uint32 format, int access, int w, int h);

#ifdef SDL2PP_WITH_IMAGE
	////////////////////////////////////////////////////////////
	/// \brief Create texture loading it via RWops
	///
	/// \param renderer Rendering context to create texture for
	/// \param rwops RWops used to access an image file
	///
	/// \throws SDL2pp::Exception
	///
	////////////////////////////////////////////////////////////
	Texture(Renderer& renderer, RWops& rwops);

	////////////////////////////////////////////////////////////
	/// \brief Create texture loading it from file
	///
	/// \param renderer Rendering context to create texture for
	/// \param filename Path to an image file
	///
	/// \throws SDL2pp::Exception
	///
	////////////////////////////////////////////////////////////
	Texture(Renderer& renderer, const std::string& filename);
#endif

	////////////////////////////////////////////////////////////
	/// \brief Create texture from surface
	///
	/// \param renderer Rendering context to create texture for
	/// \param surface Surface containing pixel data used to fill the texture
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_CreateTextureFromSurface
	///
	////////////////////////////////////////////////////////////
	Texture(Renderer& renderer, const Surface& surface);

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	/// \see http://wiki.libsdl.org/SDL_DestroyTexture
	///
	////////////////////////////////////////////////////////////
	virtual ~Texture();

	////////////////////////////////////////////////////////////
	/// \brief Move constructor
	///
	/// \param other SDL2pp::Texture object to move data from
	///
	////////////////////////////////////////////////////////////
	Texture(Texture&& other) noexcept;

	////////////////////////////////////////////////////////////
	/// \brief Move assignment
	///
	/// \param other SDL2pp::Texture object to move data from
	///
	/// \returns Reference to self
	///
	////////////////////////////////////////////////////////////
	Texture& operator=(Texture&& other) noexcept;

	// Deleted copy constructor and assignment
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;

	////////////////////////////////////////////////////////////
	/// \brief Get pointer to contained SDL_Texture structure
	///
	/// \returns Pointer to contained SDL_Texture structure
	///
	////////////////////////////////////////////////////////////
	SDL_Texture* Get() const;

	////////////////////////////////////////////////////////////
	/// \brief Update the given texture rectangle with new pixel data
	///
	/// \param rect Rect representing the area to update, or NullOpt to
	///             update the entire texture
	/// \param pixels Raw pixel data
	/// \param pitch Number of bytes in a row of pixel data, including
	///              padding between lines
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_UpdateTexture
	///
	////////////////////////////////////////////////////////////
	void Update(const Optional<Rect>& rect, const void* pixels, int pitch);

	////////////////////////////////////////////////////////////
	/// \brief Set the blend mode for a texture, used by SDL2pp::Renderer::Copy
	///
	/// \param blendMode SDL_BlendMode to use for texture blending
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_SetTextureBlendMode
	///
	////////////////////////////////////////////////////////////
	void SetBlendMode(SDL_BlendMode blendMode);

	////////////////////////////////////////////////////////////
	/// \brief Set an additional alpha value multiplied into render copy operations
	///
	/// \param alpha Source alpha value multiplied into copy operations
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_SetTextureAlphaMod
	///
	////////////////////////////////////////////////////////////
	void SetAlphaMod(Uint8 alpha = 255);

	////////////////////////////////////////////////////////////
	/// \brief Set an additional color value multiplied into render copy operations
	///
	/// \param r Red color value multiplied into copy operations
	/// \param g Green color value multiplied into copy operations
	/// \param b Blue color value multiplied into copy operations
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_SetTextureColorMod
	///
	////////////////////////////////////////////////////////////
	void SetColorMod(Uint8 r = 255, Uint8 g = 255, Uint8 b = 255);

	////////////////////////////////////////////////////////////
	/// \brief Lock texture for write-only pixel access
	///
	/// \param rect Rect representing area to lock for access
	///             (NullOpt to lock entire texture)
	///
	/// \return Lock handle used to access pixel data and to control lock lifetime
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_LockTexture
	///
	////////////////////////////////////////////////////////////
	LockHandle Lock(const Optional<Rect>& rect = NullOpt);

	////////////////////////////////////////////////////////////
	/// \brief Get texture format
	///
	/// \return Texture raw format
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_QueryTexture
	/// \see http://wiki.libsdl.org/SDL_QueryTexture#format
	///
	////////////////////////////////////////////////////////////
	Uint32 GetFormat() const;

	////////////////////////////////////////////////////////////
	/// \brief Get texture access mode
	///
	/// \return Texture access pattern
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_QueryTexture
	/// \see http://wiki.libsdl.org/SDL_TextureAccess
	///
	////////////////////////////////////////////////////////////
	int GetAccess() const;

	////////////////////////////////////////////////////////////
	/// \brief Get texture image width
	///
	/// \return Texture width in pixels
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_QueryTexture
	///
	////////////////////////////////////////////////////////////
	int GetWidth() const;

	////////////////////////////////////////////////////////////
	/// \brief Get texture image height
	///
	/// \return Texture height in pixels
	///
	/// \throws SDL2pp::Exception
	///
	/// \see http://wiki.libsdl.org/SDL_QueryTexture
	///
	////////////////////////////////////////////////////////////
	int GetHeight() const;
};

}

#endif
