#ifndef _GLES1_STATE_H_
#define _GLES1_STATE_H_

#if defined(GLX)
	#include "../../Context/Linux/GLee.h"
#elif defined(PANDORA) || defined(ANDROID)
	#include <GLES/gl.h>
#endif

#include "../RenderState.h"

namespace GLESGAE
{
	class Matrix4;
	class Camera;
	class GLES1State : public RenderState
	{
		public:
			GLES1State();
			~GLES1State();
			
			/// Set the Camera.
			void setCamera(Camera* const camera);
			
			/// Sets the current texture matrix
			void setTextureMatrix(Matrix4* const matrix);
			
			/// Toggle Texturing
			void setTexturingEnabled(const bool isEnabled);
			
			/// Toggle Alpha Blending
			void setAlphaBlendingEnabled(const bool isEnabled);
			
			/// Toggle Vertex Positions
			void setVertexPositionsEnabled(const bool isEnabled);
			
			/// Toggle Vertex Colours
			void setVertexColoursEnabled(const bool isEnabled);
			
			/// Toggle Vertex Normals
			void setVertexNormalsEnabled(const bool isEnabled);
			
			/// Set Blending Function
			void setBlendingFunction(const GLenum source, const GLenum destination);
			
			/// Is Texturing Enabled
			bool isTexturingEnabled() const { return mTexturing; }
			
			/// Is Alpha Blending Enabled
			bool isAlphaBlendingEnabled() const { return mAlphaBlending; }
			
			/// Is Vertex Positions Enabled
			bool isVertexPositionsEnabled() const { return mVertexPositions; }
			
			/// Is Vertex Colours Enabled
			bool isVertexColoursEnabled() const { return mVertexColours; }
			
			/// Is Vertex Normals Enabled
			bool isVertexNormalsEnabled() const { return mVertexNormals; }
			
		private:
			// No Copying
			GLES1State(const GLES1State&);
			GLES1State& operator=(const GLES1State&);
			
			Camera* mCamera;
			Matrix4* mTextureMatrix;
			bool mTexturing;
			bool mAlphaBlending;
			bool mVertexPositions;
			bool mVertexColours;
			bool mVertexNormals;
	};
}

#endif
