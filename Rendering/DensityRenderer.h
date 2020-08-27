#pragma once
#include "glm.hpp"
#include <QTGui/QOpenGLBuffer>

class SimulationController;
class ShaderProgram;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

class DensityRenderer
{
public:
							DensityRenderer(SimulationController* simCon);
							~DensityRenderer(void){};

	void					render(QOpenGLFunctions *f);
	void					init();

	void					setColor(glm::vec4 color);

private:
	QOpenGLBuffer*			m_DensityBuffer;
	QOpenGLBuffer*			m_VboScreenPos;

	GLuint					m_CellCountHorizontalID;
	GLuint					m_CellCountVerticalID;
	GLuint					m_DensityID;
	GLuint					m_ColorID;

	uint32_t				m_CellCountHorizontal;
	uint32_t				m_CellCountVertical;

	glm::vec4				m_Color;

	SimulationController*	m_SimCon;
	
	QOpenGLShaderProgram*	m_ShaderProgram;
};

