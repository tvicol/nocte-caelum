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

#include "StelQGLGLSLShader.hpp"
#include "StelQGL2Renderer.hpp"

#include <stdint.h>

#define ALIGN_UNIFORM_SIZE(x) (((x) + (sizeof ((StelQGLGLSLShader *)0)->uniformStorage[0]-1)) / sizeof ((StelQGLGLSLShader *)0)->uniformStorage[0])

int StelQGLGLSLShader::UNIFORM_SIZES[UniformType_max] =
	{0,
	ALIGN_UNIFORM_SIZE(sizeof(float)),
	ALIGN_UNIFORM_SIZE(sizeof(Vec2f)),
	ALIGN_UNIFORM_SIZE(sizeof(Vec3f)),
	ALIGN_UNIFORM_SIZE(sizeof(Vec4f)),
	ALIGN_UNIFORM_SIZE(sizeof(Mat4f)),
	ALIGN_UNIFORM_SIZE(sizeof(bool)),
	ALIGN_UNIFORM_SIZE(sizeof(int))};

StelQGLGLSLShader::StelQGLGLSLShader(StelQGL2Renderer* renderer, bool internal) 
	: StelGLSLShader()
	, renderer(renderer)
	, program(NULL)
	, state(State_Unlocked)
	, bound(false)
	, useUnprojectedPosition_(false)
	, internal(internal)
	, uniformStorageUsed(0)
	, uniformCount(0)
	, uniformStorageStackSize(0)
{
	renderer->getStatistics()[SHADERS_CREATED] += 1.0;
	if(!addVertexShader("DefaultProjector",
		"vec4 project(in vec4 v){return v;}\n"))
	{
		qWarning() << "Failed to add default projection vertex shader: " << log();
	}
}

void StelQGLGLSLShader::bind()
{
	Q_ASSERT_X(!bound && state == State_Built, Q_FUNC_INFO,
	           "Trying to bind a non-built or already bound shader");
	if(!internal){renderer->bindCustomShader(this);}
	bound = true;
}

void StelQGLGLSLShader::release()
{
	Q_ASSERT_X(bound && state == State_Built, Q_FUNC_INFO,
	           "Trying to release an unbound shader");

	clearUniforms();
	// The reference is passed to ensure the custom shader used is really this one.
	if(!internal){renderer->releaseCustomShader(this);}
	bound = false;
}

QGLShader* StelQGLGLSLShader::createVertexShader(const QString& source)
{
	Q_ASSERT_X(state == State_Unlocked || state == State_Modified, Q_FUNC_INFO,
	           "Can't add a vertex shader to a built shader program");
	state = State_Modified;
	QGLShader* shader = new QGLShader(QGLShader::Vertex, renderer->getGLContext());
	aggregatedLog += shader->log();

	if(shader->compileSourceCode(source))
	{
		return shader;
	}
	else
	{
		delete shader;
		return NULL;
	}
}

bool StelQGLGLSLShader::addVertexShader(const QString& source)
{
	QGLShader* shader = createVertexShader(source);
	if(NULL != shader)
	{
		defaultVertexShaders.append(shader);
	}
	return shader != NULL;
}

bool StelQGLGLSLShader::addVertexShader(const QString& name, const QString& source)
{
	Q_ASSERT_X(!namedVertexShaders.contains(name), Q_FUNC_INFO,
	           "Trying to add a vertex shader with the same name twice");
	QGLShader* shader = createVertexShader(source);
	if(NULL != shader)
	{
		OptionalShader value;
		value.enabled = true;
		value.shader = shader;
		namedVertexShaders.insert(name, value);
	}
	return shader != NULL;
}

bool StelQGLGLSLShader::addFragmentShader(const QString& source)
{
	Q_ASSERT_X(state == State_Unlocked || state == State_Modified, Q_FUNC_INFO,
	           "Can't add a fragment shader to a built shader program");

	state = State_Modified;

	QGLShader* shader = new QGLShader(QGLShader::Fragment, renderer->getGLContext());
	const bool success = shader->compileSourceCode(source);
	aggregatedLog += shader->log();
	if(success)
	{
		defaultFragmentShaders.append(shader);
	}
	else
	{
		delete shader;
	}
	return success;
}

QGLShaderProgram* StelQGLGLSLShader::getProgramFromCache()
{
	// Add up pointers to used shaders to get the ID.
	uintptr_t id = 0;
	foreach(QGLShader* shader, defaultVertexShaders)
	{
		id += reinterpret_cast<uintptr_t>(shader);
	}
	foreach(OptionalShader shader, namedVertexShaders)
	{
		id += shader.enabled ? reinterpret_cast<uintptr_t>(shader.shader) : 0;
	}
	foreach(QGLShader* shader, defaultFragmentShaders)
	{
		id += reinterpret_cast<uintptr_t>(shader);
	}

	// If no such program in cache, return NULL
	return programCache.value(id, NULL);
}

// See programCache doc comments to see how caching works here.
bool StelQGLGLSLShader::build()
{
	// Unlocked - i.e. not Modified
	if(state == State_Unlocked && program != NULL)
	{
		state = State_Built;
		return true;
	}

	QGLShaderProgram* cached = getProgramFromCache();
	// No matching program in cache, need to link a new program.
	if(cached == NULL)
	{
		uintptr_t id = 0;
		QGLShaderProgram* newProgram = new QGLShaderProgram(renderer->getGLContext());

        QString vertSource;

        // Add all the shaders to the program.
        foreach(QGLShader* shader, defaultVertexShaders)
		{
            vertSource.append(shader->sourceCode());
            //if(!newProgram->addShader(shader)) {goto FAILED;}
			id += reinterpret_cast<uintptr_t>(shader);
		}
        foreach(OptionalShader shader, namedVertexShaders)
        {
            if(shader.enabled)
            {
                vertSource.append("\n" + shader.shader->sourceCode());
                //if(!newProgram->addShader(shader.shader)) {goto FAILED;}
                id += reinterpret_cast<uintptr_t>(shader.shader);
            }
        }

        // OpenGL ES 2.0 (Adreno on Sailfish) does not like linking multiple vertex shaders, so we join the source to a single shader
        QGLShader* vertShader = new QGLShader(QGLShader::Vertex, renderer->getGLContext());
        vertShader->compileSourceCode(vertSource);
        aggregatedLog += vertShader->log();
        if(!newProgram->addShader(vertShader)) {goto FAILED;}

        foreach(QGLShader* shader, defaultFragmentShaders)
		{
            if(!newProgram->addShader(shader)) {goto FAILED;}
			id += reinterpret_cast<uintptr_t>(shader);
		}
        Q_ASSERT_X(id > 0, Q_FUNC_INFO, "Trying to build() a StelQGLGLSLShader "
		           "but no vertex or fragment shaders were added");

		// Link the program.
		if(!newProgram->link()) {goto FAILED;}

		aggregatedLog += "Built successfully";
		// Add the program to the cache, and set the current program to it.
		programCache.insert(id, newProgram);
		program = newProgram;
		state = State_Built;
		return true;

		// And here I present to you a viable application of the fabled GOTO statement and a label.
		// (good way to do error recovery in plan C and when using return codes, BTW)
FAILED:
		aggregatedLog += newProgram->log();
		delete newProgram;
		return false;
	}

	program = cached;
	state = State_Built;
	return true;
}
