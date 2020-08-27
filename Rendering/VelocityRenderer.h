#pragma once
#include "glm.hpp"
#include <QTGui/QOpenGLBuffer>

class SimulationController;
class ShaderProgram;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

class VelocityRenderer
{
public:
							VelocityRenderer(SimulationController* simCon);
							~VelocityRenderer(void){};

	void					render(QOpenGLFunctions *f);
	void					init();

private:
	QOpenGLBuffer*			m_VelocityBufferU;
	QOpenGLBuffer*			m_VelocityBufferV;
	QOpenGLBuffer*			m_VboScreenPos;

	GLuint					m_VelUCountID;
	GLuint					m_VelVCountID;
	GLuint					m_VelUID;
	GLuint					m_VelVID;

	uint32_t				m_VelocityVCount;
	uint32_t				m_VelocityUCount;

	SimulationController*	m_SimCon;
	
	QOpenGLShaderProgram*	m_ShaderProgram;
};

