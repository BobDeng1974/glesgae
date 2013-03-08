#include "OGLState.h"

#include "../../Camera.h"
#include "../../Shader.h"
#include "../../ShaderUniformUpdater.h"
#include "../../Material.h"
#include "../../Texture.h"
#include "../../../Maths/Matrix4.h"

#include <cassert>

using namespace GLESGAE;

const HashString aPositionHS("a_position");
const HashString aColourHS("a_colour");
const HashString aNormalHS("a_normal");
const HashString aTexCoord0HS("a_texCoord0");
const HashString aTexCoord1HS("a_texCoord1");
const HashString aCustom0HS("a_custom0");
const HashString aCustom1HS("a_custom1");
const HashString aCustom2HS("a_custom2");

OGLState::OGLState()
: mCamera(0)
, mTextureMatrix(0)
, mTexturing(false)
, mAlphaBlending(false)
, mCurrentShader(0)
, a_position(GL_INVALID_VALUE)
, a_colour(GL_INVALID_VALUE)
, a_normal(GL_INVALID_VALUE)
, a_texCoord0(GL_INVALID_VALUE)
, a_texCoord1(GL_INVALID_VALUE)
, a_custom0(GL_INVALID_VALUE)
, a_custom1(GL_INVALID_VALUE)
, a_custom2(GL_INVALID_VALUE)
, mLastTexture(0)
, mLastTextureUnit(GL_INVALID_VALUE)
, mUniformUpdaters()
{
}

OGLState::~OGLState()
{
}

void OGLState::setCamera(Camera* const camera)
{
	if (mCamera != camera) {
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(camera->getProjectionMatrix().getData());
		
		glMatrixMode(GL_MODELVIEW);
		Matrix4& viewMatrix(camera->getViewMatrix());
		viewMatrix(0U, 2U) = -viewMatrix(0U, 2U);
		viewMatrix(1U, 2U) = -viewMatrix(1U, 2U);
		viewMatrix(2U, 2U) = -viewMatrix(2U, 2U);
		viewMatrix(3U, 2U) = -viewMatrix(3U, 2U);
		glLoadMatrixf(viewMatrix.getData());
		
		mCamera = camera;
	}
}

void OGLState::setTextureMatrix(Matrix4* const matrix)
{
	if (mTextureMatrix != matrix) {
		glMatrixMode(GL_TEXTURE);
		glLoadMatrixf(matrix->getData());
		
		glMatrixMode(GL_MODELVIEW);
		
		mTextureMatrix = matrix;
	}
}

void OGLState::setTexturingEnabled(const bool isEnabled)
{
	if (true == isEnabled) {
		if (false == mTexturing) {
			glEnable(GL_TEXTURE_2D);
			mTexturing = true;
		}
	} else {
		if (true == mTexturing) {
			glDisable(GL_TEXTURE_2D);
			mTexturing = false;
		}
	}
}

void OGLState::setAlphaBlendingEnabled(const bool isEnabled)
{
	if (true == isEnabled) {
		if (false == mAlphaBlending) {
			glEnable(GL_BLEND);
			mAlphaBlending = true;
		}
	} else {
		if (true == mAlphaBlending) {
			glDisable(GL_BLEND);
			mAlphaBlending = false;
		}
	}
}

void OGLState::setBlendingFunction(const GLenum source, const GLenum destination)
{
	glBlendFunc(source, destination);
}

void OGLState::setFullBlendingFunction(const GLenum sourceRGB, const GLenum destinationRGB
									,const GLenum sourceAlpha, const GLenum destinationAlpha)
{
	glBlendFuncSeparate(sourceRGB, destinationRGB, sourceAlpha, destinationAlpha);
}

void OGLState::addUniformUpdater(const HashString uniformName, ShaderUniformUpdater* const updater)
{
	// should probably check and assert if we already have something for this uniform name.
	mUniformUpdaters[uniformName] = updater;
}

void OGLState::bindShader(Shader* const shader)
{
	if (mCurrentShader != shader) {
		mCurrentShader = shader;
		glUseProgram(shader->getProgramId());
		resetAttributes();
		
		const std::vector<std::pair<HashString, GLint> >& attributes(shader->getAttributeArray());
		for (std::vector<std::pair<HashString, GLint> >::const_iterator itr(attributes.begin()); itr < attributes.end(); ++itr)
			glEnableVertexAttribArray(itr->second);
	}
}

void OGLState::resetAttributes()
{
	a_position = GL_INVALID_VALUE;
	a_colour = GL_INVALID_VALUE;
	a_normal = GL_INVALID_VALUE;
	a_texCoord0 = GL_INVALID_VALUE;
	a_texCoord1 = GL_INVALID_VALUE;
	a_custom0 = GL_INVALID_VALUE;
	a_custom1 = GL_INVALID_VALUE;
	a_custom2 = GL_INVALID_VALUE;

	if (mCurrentShader != 0) {
		a_position = mCurrentShader->getAttribute(aPositionHS);
		a_colour = mCurrentShader->getAttribute(aColourHS);
		a_normal = mCurrentShader->getAttribute(aNormalHS);
		a_texCoord0 = mCurrentShader->getAttribute(aTexCoord0HS);
		a_texCoord1 = mCurrentShader->getAttribute(aTexCoord1HS);
		a_custom0 = mCurrentShader->getAttribute(aCustom0HS);
		a_custom1 = mCurrentShader->getAttribute(aCustom1HS);
		a_custom2 = mCurrentShader->getAttribute(aCustom2HS);
	}

	if (GL_INVALID_VALUE != a_position)
		glEnableVertexAttribArray(a_position);
	if (GL_INVALID_VALUE != a_colour)
		glEnableVertexAttribArray(a_colour);
	if (GL_INVALID_VALUE != a_normal)
		glEnableVertexAttribArray(a_normal);
	if (GL_INVALID_VALUE != a_texCoord0)
		glEnableVertexAttribArray(a_texCoord0);
	if (GL_INVALID_VALUE != a_texCoord1)
		glEnableVertexAttribArray(a_texCoord1);
	if (GL_INVALID_VALUE != a_custom0)
		glEnableVertexAttribArray(a_custom0);
	if (GL_INVALID_VALUE != a_custom1)
		glEnableVertexAttribArray(a_custom1);
	if (GL_INVALID_VALUE != a_custom2)
		glEnableVertexAttribArray(a_custom2);
}

void OGLState::updateUniforms(Material* const material, Matrix4* const transform)
{
	std::vector<std::pair<HashString, GLint> > uniforms(mCurrentShader->getUniformArray());
	for (std::vector<std::pair<HashString, GLint> >::iterator itr(uniforms.begin()); itr < uniforms.end(); ++itr)
		mUniformUpdaters[itr->first]->update(itr->second, mCamera, material, transform);
}

void OGLState::updateTextures(Material* const material)
{
	const unsigned int textureCount(material->getTextureCount());

	for (unsigned int currentTexture(0U); currentTexture < textureCount; ++currentTexture) {
		GLenum currentTextureUnit(GL_TEXTURE0 + currentTexture);
		if (currentTextureUnit != mLastTextureUnit) {
			glActiveTexture(currentTextureUnit);
			mLastTextureUnit = currentTextureUnit;
		}
		
		Texture* const texture(material->getTexture(currentTexture));
		if (mLastTexture != texture) {
			mLastTexture = texture;
			glBindTexture(GL_TEXTURE_2D, texture->getId());
		}
	}
}

