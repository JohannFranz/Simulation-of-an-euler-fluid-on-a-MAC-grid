#include "VelocityRenderer.h"
#include "../Fluid/SimulationController.h"
#include "IOManager.h"

#include <string>
#include <QtGui/QOpenGLShaderProgram>
#include <QTGui/QOpenGLFunctions>

VelocityRenderer::VelocityRenderer(SimulationController* simCon)
: m_SimCon(simCon)
{
	
}	

void VelocityRenderer::init()
{
	m_ShaderProgram = new QOpenGLShaderProgram();

	const char* vertexShaderSourceCode = IOManager::readFile("Rendering/Shader/vDefault.vsh");
	m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSourceCode);

	const char* fragmentShaderSourceCode = IOManager::readFile("Rendering/Shader/fVelocity.fsh");
	m_ShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSourceCode);

	m_ShaderProgram->link();
	bool linked = m_ShaderProgram->isLinked();

	m_ShaderProgram->bind();

	m_VelUCountID = m_ShaderProgram->uniformLocation("velocityUCount");
	m_VelVCountID = m_ShaderProgram->uniformLocation("velocityVCount");
	m_VelUID = m_ShaderProgram->uniformLocation("velocityU");
	m_VelVID = m_ShaderProgram->uniformLocation("velocityV");

	m_VelocityVCount = m_SimCon->getVelocityVCount();
	m_VelocityUCount = m_SimCon->getVelocityUCount();

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

void VelocityRenderer::render(QOpenGLFunctions *f)
{
	m_VboScreenPos->bind();
	m_ShaderProgram->bind();
	m_ShaderProgram->enableAttributeArray("vPosition");
	
	m_ShaderProgram->setUniformValue(m_VelUCountID, m_VelocityUCount);
	m_ShaderProgram->setUniformValue(m_VelVCountID, m_VelocityVCount);
	m_ShaderProgram->setUniformValueArray(m_VelUID, (GLfloat*) m_SimCon->getVelocityU(), m_VelocityUCount*(m_VelocityVCount-1), 1);
	m_ShaderProgram->setUniformValueArray(m_VelVID, (GLfloat*) m_SimCon->getVelocityV(), m_VelocityVCount*(m_VelocityUCount-1), 1);
	
	f->glDrawArrays(GL_TRIANGLES, 0, 6);

	m_ShaderProgram->disableAttributeArray("vPosition");
	m_ShaderProgram->release();
	m_VboScreenPos->release();
}

