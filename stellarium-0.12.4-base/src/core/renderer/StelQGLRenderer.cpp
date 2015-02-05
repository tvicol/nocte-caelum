/*
 * Stellarium
 * Copyright (C) 2012 Ferdinand Majerech
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include <cmath>

#ifdef _MSC_BUILD
#define round(dbl) dbl >= 0.0 ? (int)(dbl + 0.5) : ((dbl - (double)(int)dbl) <= -0.5 ? (int)dbl : (int)(dbl - 0.5))
#endif

#include "StelQGLRenderer.hpp"
#include "StelLocaleMgr.hpp"
#include "QSettings"

void StelQGLRenderer::bindTextureBackend
	(StelTextureBackend* const textureBackend, const int textureUnit)
{
	invariant();
	StelQGLTextureBackend* qglTextureBackend =
		dynamic_cast<StelQGLTextureBackend*>(textureBackend);
	Q_ASSERT_X(qglTextureBackend != NULL, Q_FUNC_INFO,
	           "Trying to bind a texture created by a different renderer backend");

	QSettings* conf = StelApp::getInstance().getSettings();

	const TextureStatus status = qglTextureBackend->getStatus();
	if(status == TextureStatus_Uninitialized)
	{
		qglTextureBackend->startAsynchronousLoading();
	}
	// Ignore the texture if we don't have enough texture units
	// or if texture unit is nonzero and we don't support multitexturing.
	if(textureUnit >= textureUnitCount)
	{
		invariant();
		return;
	}
	if(status == TextureStatus_Loaded)
	{
		if(currentlyBoundTextures[textureUnit] != qglTextureBackend)
		{
			qglTextureBackend->bind(textureUnit);
			currentlyBoundTextures[textureUnit] = qglTextureBackend;
		}
	}
	else
	{
		// by default placeholder is hide but its can be enabled for debugging
		bool debugMode = false;
		if (conf->value("debug/texture_placeholder_flag", false).toBool())
			debugMode = true;

		getPlaceholderTexture(debugMode)->bind(textureUnit);
		currentlyBoundTextures[textureUnit] = getPlaceholderTexture(debugMode);

	}
	invariant();
}

void StelQGLRenderer::destroyTextureBackend(StelTextureBackend* const textureBackend)
{
	invariant();
	StelQGLTextureBackend* qglTextureBackend =
		dynamic_cast<StelQGLTextureBackend*>(textureBackend);
	Q_ASSERT_X(qglTextureBackend != NULL, Q_FUNC_INFO,
	           "Trying to destroy a texture created by a different renderer backend");

	if(textureBackend->getName().isEmpty())
	{
		delete textureBackend;
		invariant();
		return;
	}
	textureCache.remove(qglTextureBackend);
	invariant();
}

StelTextureBackend* StelQGLRenderer::createTextureBackend
	(const QString& filename, const TextureParams& params, const TextureLoadingMode loadingMode)
{
	invariant();
	const QString fullPath = filename.startsWith("http://") 
		? filename 
		: glFileSystemTexturePath(filename, pvrSupported);

	if(fullPath.isEmpty())
	{
		QImage image;
		// Texture in error state will be returned (loaded from NULL image), and
		// when bound, the placeholder texture will be used.
		qWarning() << "createTextureBackend failed: file \"" << QDir::toNativeSeparators(filename) << "\" not found.";
		return StelQGLTextureBackend::constructFromImage(this, QString(), params, image);
	}

	StelQGLTextureBackend* cached = textureCache.get(fullPath, loadingMode);
	if(NULL != cached)
	{
		invariant();
		return cached;
	}

	StelQGLTextureBackend* result = NULL;
	//Synchronous (not in a separate thread) loading.
	if(loadingMode == TextureLoadingMode_Normal)
	{
		//Load from a normal image.
		if(!fullPath.endsWith(".pvr"))
		{
			QImage image = QImage(fullPath);
			if(image.isNull())
			{
				// Texture in error state will be returned (loaded from NULL image), and
				// when bound, the placeholder texture will be used.
				qWarning() << "createTextureBackend failed: found image file \"" << QDir::toNativeSeparators(fullPath)
				           << "\" but failed to load image data. ";
			}
			//Uploads to GL
			result = StelQGLTextureBackend::constructFromImage(this, fullPath, params, image);
		}
		//Load from PVR (compressed textures on PowerVR (mobile)).
		else
		{
			result = StelQGLTextureBackend::constructFromPVR(this, fullPath, params);
		}
	}
	else
	{
		Q_ASSERT_X(loadingMode == TextureLoadingMode_Asynchronous || 
		           loadingMode == TextureLoadingMode_LazyAsynchronous,
		           Q_FUNC_INFO, 
		           "Unknown texture loading mode");
		result = StelQGLTextureBackend::constructAsynchronous(this, fullPath, params);
		if(loadingMode != TextureLoadingMode_LazyAsynchronous)
		{
			//Web or file
			result->startAsynchronousLoading();
		}
	}

	Q_ASSERT_X(result != NULL, Q_FUNC_INFO, "Result texture backend was not set");

	textureCache.add(result);
	invariant();
	return result;
}

StelTextureBackend* StelQGLRenderer::createTextureBackend
	(QImage& image, const TextureParams& params)
{
	invariant();
	return StelQGLTextureBackend::constructFromImage(this, QString(), params, image);
}

StelTextureBackend* StelQGLRenderer::createTextureBackend
	(const void* data, const QSize size, const TextureDataFormat format,
	 const TextureParams& params)
{
	invariant();
	return StelQGLTextureBackend::fromRawData(this, data, size, format, params);
}


void StelQGLRenderer::renderFrame(StelRenderClient& renderClient)
{
    invariant();
	recentFrames.update();
	clearFrameStatistics();
	if(previousFrameEndTime < 0.0)
	{
		previousFrameEndTime = StelApp::getTotalRunTime();
	}

    //TODO: refactor StelN9 code to separate Renderer class

    viewport.setDefaultPainter(renderClient.getPainter(), glContext);

    bool draw = true;

    if (skipNumFrames > 0)
    {
        if (!skipFrameFbo)
        {
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
            format.setMipmap(false);
            format.setTextureTarget(GL_TEXTURE_2D);
            format.setInternalTextureFormat(GL_RGBA);
            skipFrameFbo = new QOpenGLFramebufferObject(viewport.getViewportSize(), format);
        }


        if (currentSkipFrameNum == 0)
        {
            //qDebug() << "Draw FBO";
            draw = true;
            skipFrameFbo->bind();
        }
        else
        {
            //qDebug() << "Skip frame";
            draw = false;
        }
        if (++currentSkipFrameNum > skipNumFrames)
            currentSkipFrameNum = 0;
    }

    if (draw)
    {
        glClearColor(0., 0., 0., 0.);
        glClear(GL_COLOR_BUFFER_BIT);

        //makeGLContextCurrent();

        // Stellarium N9:
        // tested FBOs on Jolla, performance sucks; does not justify frame rate control

        viewport.startFrame();
        while (true)
        {
            const bool doneDrawing = !renderClient.drawPartial();
            if(doneDrawing)
            {
                newFrame = true;
                viewport.finishFrame();
                break;
            }
        }
        drawWindow(renderClient.getViewportEffect());
    }

    if (skipNumFrames > 0)
    {
        if (draw)
            skipFrameFbo->release();

        //glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skipFrameFbo->texture());

        renderQuadProgram->bind();
        renderQuadProgram->setUniformValue("tex", 0);

        renderQuadVbo->bind();
        renderQuadProgram->enableAttributeArray(0);
        renderQuadProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2);
        renderQuadVbo->release();

        QSize size = viewport.getViewportSize();
        glViewport(0, 0, size.width(), size.height());
        glDisable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindTexture(GL_TEXTURE_2D, 0);
        renderQuadProgram->disableAttributeArray(0);
        renderQuadProgram->release();
    }

	viewport.setDefaultPainter(NULL, glContext);


    previousFrameEndTime = StelApp::getTotalRunTime();
	invariant();
}

void StelQGLRenderer::drawWindow(StelViewportEffect* const effect)
{
	// At this point, FBOs have been released (if using FBOs), so we're drawing 
	// directly to the screen. The StelViewportEffect::drawToViewport call 
	// actually draws puts the rendered result onto the viewport.
	invariant();

	//Warn about any GL errors.
	checkGLErrors("drawWindow() start");
	
	//Effects are ignored when FBO is not supported.
	//That might be changed for some GPUs, but it might not be worth the effort.
	
	viewport.prepareToDrawViewport();

	if(NULL == effect)
	{
        // StelN9: we never use viewport FBO
		// If using FBO, we still need to put it on the screen.
		if(viewport.useFBO())
		{
			StelTextureNew* screenTexture = getViewportTexture();
			const QSize size = screenTexture->getDimensions();

			glDisable(GL_BLEND);			
			setGlobalColor(Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
			screenTexture->bind();
			drawTexturedRect(0, 0, size.width(), size.height());
			delete screenTexture;
		}
		// If not using FBO, the result is already drawn to the screen.
	}
	else
	{
		effect->drawToViewport(this);
	}

	viewport.disablePainting();

	checkGLErrors("drawWindow() end");
	invariant();
}

void StelQGLRenderer::drawTextGravityHelper
	(const TextParams& params, QPainter& painter, const int baseX, const int baseY, StelProjectorP projector)
{
	const Vec2f viewportCenter = projector->getViewportCenterAbsolute();

	const float dx = baseX - viewportCenter[0];
	const float dy = baseY - viewportCenter[1];
	const float d  = std::sqrt(dx * dx + dy * dy);
	
	// Cull the text if it's too far away to be visible in the screen.
	if (d > qMax(projector->getViewportXywh()[3], projector->getViewportXywh()[2]) * 2)
	{
		return;
	}

	const QString string  = params.string_;
	const int charCount   = string.length();
	const float charWidth = painter.fontMetrics().width(string) / charCount;
	float theta           = std::atan2(dy - 1, dx);
	const float psi       = qMin(5.0, std::atan2(charWidth, d + 1) * 180.0f / M_PI);
	const float xVc       = viewportCenter[0] + params.xShift_;
	const float yVc       = viewportCenter[1] + params.yShift_;
	const QString lang    = StelApp::getInstance().getLocaleMgr().getAppLanguage();

	const bool leftToRight = !QString("ar fa ur he yi ckb").contains(lang);
	// Draw each character separately
	for (int i = 0; i < charCount; ++i)
	{
		const int charIndex = leftToRight ? i : charCount - 1 - i;
		const float x       = d * std::cos(theta) + xVc;
		const float y       = d * std::sin(theta) + yVc;
		const float angle   = 90.0f + theta * 180.0f / M_PI;

		drawText(TextParams(x, y, string[charIndex]).angleDegrees(angle));
	
		// Compute how much the character contributes to the angle 
		const float width = painter.fontMetrics().width(string[charIndex]);
		theta += psi * M_PI / 180.0 * (1 + (width - charWidth) / charWidth);
	}
}

void StelQGLRenderer::drawText(const TextParams& params)
{
	statistics[TEXT_DRAWS] += 1.0;
	StelQGLTextureBackend* currentTexture = currentlyBoundTextures[0];

	if(params.string_.length() == 0)
	{
		return;
	}

	viewport.enablePainting();
	if(currentFontSet)
	{
		viewport.setFont(currentFont);
	}
	QPainter* painter = viewport.getPainter();
	Q_ASSERT_X(NULL != painter, Q_FUNC_INFO, 
	           "Trying to draw text but painting is disabled");

	QFontMetrics fontMetrics = painter->fontMetrics();

	StelProjectorP projector = NULL == params.projector_
	                         ? StelApp::getInstance().getCore()->getProjection2d() 
	                         : params.projector_;
	Vec3f win;
	if(params.doNotProject_) 
	{
		win = params.position_;
	}
	else if(!projector->project(params.position_, win))
	{
		viewport.disablePainting();
		return;
	}

	const int x = round(win[0]);
	const int y = round(win[1]);

	// Avoid drawing if outside viewport.
	// We do a worst-case approximation as getting exact text dimensions is expensive.
	// We also account for rotation by assuming the worst case in bot X and Y 
	// (culling with a rotating rectangle would be expensive)
	const int cullDistance = 
		std::max(fontMetrics.height(), params.string_.size() * fontMetrics.maxWidth());
	const Vec4i viewXywh = projector->getViewportXywh();
	const int viewMinX = viewXywh[0];
	const int viewMinY = viewXywh[1];
	const int viewMaxX = viewMinX + viewXywh[2];
	const int viewMaxY = viewMinY + viewXywh[3];

	if(y + cullDistance < viewMinY || y - cullDistance > viewMaxY ||
	   x + cullDistance < viewMinX || x - cullDistance > viewMaxX)
	{
		viewport.disablePainting();
		return;
	}

	if(projector->useGravityLabels() && !params.noGravity_)
	{
		drawTextGravityHelper(params, *painter, x, y, projector);
		return;
	}
	
	const int pixelSize   = painter->font().pixelSize();
	// Strings drawn by drawText() can differ by text, font size, or the font itself.
	const QByteArray hash = params.string_.toUtf8() + QByteArray::number(pixelSize) + 
	                        painter->font().family().toUtf8();
	StelQGLTextureBackend* textTexture = textTextureCache.object(hash);

	// No texture in cache for this string, need to draw it.
	if (NULL == textTexture) 
	{
		const QRect extents = fontMetrics.boundingRect(params.string_);

		// Width and height of the text. 
		// Texture width/height is required to be at least equal to this.
		//
		// Both X and Y need to be at least 1 so we don't create an empty image 
		// (doesn't work with textures)
		const int requiredWidth  = std::max(1, extents.width() + 1 + static_cast<int>(0.02f * extents.width()));
		const int requiredHeight = std::max(1, extents.height());
		// Create temporary image and render text into it

		// QImage is used solely to reuse existing QGLTextureBackend constructor 
		// function. QPixmap could be used as well (not sure which is faster, 
		// needs profiling)
		QImage image = areNonPowerOfTwoTexturesSupported() 
		             ? QImage(requiredWidth, requiredHeight, QImage::Format_ARGB32_Premultiplied) 
		             : QImage(StelUtils::smallestPowerOfTwoGreaterOrEqualTo(requiredWidth), 
		                      StelUtils::smallestPowerOfTwoGreaterOrEqualTo(requiredHeight),
		                      QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter fontPainter(&image);
		fontPainter.setFont(painter->font());
		fontPainter.setRenderHints(QPainter::TextAntialiasing, true);
		fontPainter.setPen(Qt::white);

		// The second argument ensures the text is positioned correctly even if 
		// the image is enlarged to power-of-two.
		fontPainter.drawText(-extents.x(), 
		                     image.height() - requiredHeight - extents.y(), 
		                     params.string_);

		textTexture = StelQGLTextureBackend::constructFromImage
			(this, QString(), TextureParams().filtering(TextureFiltering_Linear), image);
		const QSize size = textTexture->getDimensions();
		if (!(textTexture->getStatus() == TextureStatus_Loaded))
		{
			qWarning() << "Texture error: " << textTexture->getErrorMessage();
			Q_ASSERT_X(false, Q_FUNC_INFO, "Failed to construct a text texture");
		}
		textTextureCache.insert(hash, textTexture, 4 * size.width() * size.height());
	}

	// Even if NPOT textures are not supported, we always draw the full rectangle 
	// of the texture. The extra space is fully transparent, so it's not an issue.

	// Shortcut variables to calculate the rectangle.
	const QSize size   = textTexture->getDimensions();
	const float w      = size.width();
	const float h      = size.height();
	const float xShift = params.xShift_;
	const float yShift = params.yShift_;

	const float angleDegrees = 
		params.angleDegrees_ + 
		(params.noGravity_ ? 0.0f : projector->getDefaultAngleForGravityText());
	// Zero out very small angles.
	// 
	// (this could also be used to optimize the case with zero angled
	//  to avoid sin/cos if needed)
	const bool  angled = std::fabs(angleDegrees) >= 1.0f * M_PI / 180.f;
	const float cosr   = angled  ? std::cos(angleDegrees * M_PI / 180.0) : 1.0f;
	const float sinr   = angled  ? std::sin(angleDegrees * M_PI / 180.0) : 0.0f;

	// Corners of the (possibly rotated) texture rectangle.
	const Vec2f ne(round(x + cosr * xShift       - sinr * yShift),
	               round(y + sinr * xShift       + cosr * yShift));
	const Vec2f nw(round(x + cosr * (w + xShift) - sinr * yShift),
	               round(y + sinr * (w + xShift) + cosr * yShift));
	const Vec2f se(round(x + cosr * xShift       - sinr * (h + yShift)),
	               round(y + sinr * xShift       + cosr * (h + yShift)));
	const Vec2f sw(round(x + cosr * (w + xShift) - sinr * (h + yShift)),
	               round(y + sinr * (w + xShift) + cosr * (h + yShift)));

	// Construct the text vertex buffer if it doesn't exist yet, otherwise clear it.
	if(NULL == textBuffer)
	{
		textBuffer = createVertexBuffer<TexturedVertex>(PrimitiveType_TriangleStrip);
	}
	else
	{
		textBuffer->unlock();
		textBuffer->clear();
	}

	textBuffer->addVertex(TexturedVertex(ne, Vec2f(0.0f, 0.0f)));
	textBuffer->addVertex(TexturedVertex(nw, Vec2f(1.0f, 0.0f)));
	textBuffer->addVertex(TexturedVertex(se, Vec2f(0.0f, 1.0f)));
	textBuffer->addVertex(TexturedVertex(sw, Vec2f(1.0f, 1.0f)));
	textBuffer->lock();

	// Draw.
	const BlendMode oldBlendMode = blendMode;
	setBlendMode(BlendMode_Alpha);
	textTexture->bind(0);
	drawVertexBuffer(textBuffer);
	setBlendMode(oldBlendMode);

	// Reset user-bound texture.
	if(NULL != currentTexture)
	{
		currentTexture->bind(0);
	}
	viewport.disablePainting();
}


void StelQGLRenderer::drawRectInternal
	(const bool textured, const float x, const float y, const float width, 
	 const float height, const float angle)
{
	statistics[RECT_DRAWS] += 1.0;
	// Could be improved by keeping the vertex buffer as a data member,
	// or even caching all rectangle draws to the same buffer and drawing them 
	// at once at the end of the frame.
	
	Vec2f ne, nw, se, sw;

	// Faster path for angles that are zero or extremely small.
	if(abs(angle) < 0.1)
	{
		ne = Vec2f(x, y);
		nw = Vec2f(x + width, y);
		se = Vec2f(x, y + height);
		sw = Vec2f(x + width, y + height);
	}
	// Need to rotate the rectangle (around its center).
	else
	{
		const float cosr       = std::cos(angle / 180 * M_PI);
		const float sinr       = std::sin(angle / 180 * M_PI);
		const float halfWidth  = width  * 0.5;
		const float halfHeight = height * 0.5;
		const Vec2f center(x + halfWidth, y + halfHeight);

		const float widthCos  = halfWidth  * cosr;
		const float heightCos = halfHeight * cosr;
		const float widthSin  = halfWidth  * sinr;
		const float heightSin = halfHeight * sinr;

		ne = center + Vec2f(-widthCos + heightSin, -widthSin - heightCos);
		nw = center + Vec2f(widthCos  + heightSin, widthSin  - heightCos);
		se = center + Vec2f(-widthCos - heightSin, -widthSin + heightCos);
		sw = center + Vec2f(widthCos  - heightSin, widthSin  + heightCos);
	}
	
	// Prepare a vertex buffer for the rectangle and draw it.
	if(textured)
	{
		if(NULL == texturedRectBuffer)
		{
			texturedRectBuffer = 
				createVertexBuffer<TexturedVertex>(PrimitiveType_TriangleStrip);
		}
		else
		{
			texturedRectBuffer->unlock();
			texturedRectBuffer->clear();
		}

		texturedRectBuffer->addVertex(TexturedVertex(ne, Vec2f(0.0f , 0.0f)));
		texturedRectBuffer->addVertex(TexturedVertex(nw, Vec2f(1.0f , 0.0f)));
		texturedRectBuffer->addVertex(TexturedVertex(se, Vec2f(0.0f , 1.0f)));
		texturedRectBuffer->addVertex(TexturedVertex(sw, Vec2f(1.0f , 1.0f)));

		texturedRectBuffer->lock();
		drawVertexBuffer(texturedRectBuffer);
	}
	else
	{
		if(NULL == plainRectBuffer)
		{
			plainRectBuffer = 
				createVertexBuffer<VertexP2>(PrimitiveType_TriangleStrip);
		}
		else
		{
			plainRectBuffer->unlock();
			plainRectBuffer->clear();
		}

		plainRectBuffer->addVertex(VertexP2(ne));
		plainRectBuffer->addVertex(VertexP2(nw));
		plainRectBuffer->addVertex(VertexP2(se));
		plainRectBuffer->addVertex(VertexP2(sw));

		plainRectBuffer->lock();
		drawVertexBuffer(plainRectBuffer);
	}
}

void StelQGLRenderer::drawRect
	(const float x, const float y, const float width, const float height, const float angle)
{
	drawRectInternal(false, x, y, width, height, angle);
}

void StelQGLRenderer::drawTexturedRect
	(const float x, const float y, const float width, const float height, const float angle)
{
	drawRectInternal(true, x, y, width, height, angle);
}


void StelQGLRenderer::createRenderQuadProgram()
{
    float values[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };

    renderQuadVbo = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    renderQuadVbo->create();
    renderQuadVbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
    renderQuadVbo->bind();
    renderQuadVbo->allocate(values, 8 * sizeof(float));
    renderQuadVbo->release();

    renderQuadProgram = new QOpenGLShaderProgram();

    if (!renderQuadProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            "attribute highp vec4 vertex;"
            "varying highp vec2 coords;"
            "void main()"
            "{"
            "    gl_Position = vertex;"
            "    coords = vertex.xy * 0.5 + vec2(0.5);"
            "}"))
    {
        qDebug() << "Error: " << renderQuadProgram->log();
    }

    if (!renderQuadProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            "varying vec2 coords;"
            "uniform sampler2D tex;"
            "void main()"
            "{"
            "    gl_FragColor = texture2D(tex, coords);"
            "}"))
    {
        qDebug() << "Error: " << renderQuadProgram->log();
    }

    renderQuadProgram->bindAttributeLocation("vertex", 0);
    renderQuadProgram->setUniformValue("tex", 0);
    renderQuadProgram->link();

}
