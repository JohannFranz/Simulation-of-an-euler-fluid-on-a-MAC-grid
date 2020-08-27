#include "DensityRenderer.h"
#include "../Fluid/SimulationController.h"
#include "../IOManager.h"

#include <string>
#include <QtGui/QOpenGLShaderProgram>
#include <QTGui/QOpenGLFunctions>

DensityRenderer::DensityRenderer(SimulationController* simCon)
: m_SimCon(simCon), m_Color(1.0f, 1.0f, 1.0f, 1.0f)
{
}	

void DensityRenderer::init()
{
	m_ShaderProgram = new QOpenGLShaderProgram();

	const char* vertexShaderSourceCode = IOManager::readFile("Rendering/Shader/vDefault.vsh");
	m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCode);

	const char* fragmentShaderSourceCode = IOManager::readFile("Rendering/Shader/fCentralQuantity.fsh");
	m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCode);

	m_ShaderProgram->link();
	bool linked = m_ShaderProgram->isLinked();

	m_ShaderProgram->bind();

	m_CellCountHorizontalID = m_ShaderProgram->uniformLocation("cellCountHoriz");
	m_CellCountVerticalID = m_ShaderProgram->uniformLocation("cellCountVert");
	m_DensityID = m_ShaderProgram->uniformLocation("q");
	m_ColorID = m_ShaderProgram->uniformLocation("color");

	m_CellCountHorizontal = m_SimCon->getCellCountHorizontal();
	m_CellCountVertical = m_SimCon->getCellCountVertical();

	float* vertices = new float[12]{-1.0f, -1.0f, 
									1.0f, -1.0f, 
									-1.0f, 1.0f, 
									-1.0f, 1.0f, 
									1.0f, -1.0f, 
									1.0f, 1.0f};

	m_VboScreenPos = new QOpenGLBuffer();
	m_VboScreenPos->create();
	m_VboScreenPos->bind();
	m_VboScreenPos->setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_VboScreenPos->allocate(vertices, 12*sizeof(GLfloat));

	m_ShaderProgram->enableAttributeArray("vPosition");
	m_ShaderProgram->setAttributeArray(0, GL_FLOAT, 0, 2);

	m_ShaderProgram->release();
	m_VboScreenPos->release();
}

void DensityRenderer::render(QOpenGLFunctions *f)
{
	m_VboScreenPos->bind();
	m_ShaderProgram->bind();
	m_ShaderProgram->enableAttributeArray("vPosition");
	
	m_ShaderProgram->setUniformValue(m_CellCountHorizontalID, m_CellCountHorizontal);
	m_ShaderProgram->setUniformValue(m_CellCountVerticalID, m_CellCountVertical);
	m_ShaderProgram->setUniformValueArray(m_DensityID, (GLfloat*) m_SimCon->getDensity(), sizeof(float)*m_CellCountHorizontal*m_CellCountVertical, 1);
	m_ShaderProgram->setUniformValue(m_ColorID, m_Color.x, m_Color.y, m_Color.z, m_Color.w);
	
	f->glDrawArrays(GL_TRIANGLES, 0, 6);

	m_ShaderProgram->disableAttributeArray("vPosition");
	m_ShaderProgram->release();
	m_VboScreenPos->release();
}

void DensityRenderer::setColor(glm::vec4 color)
{
	m_Color = color;
}