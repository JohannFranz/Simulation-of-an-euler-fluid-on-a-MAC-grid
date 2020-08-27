#define _USE_MATH_DEFINES


#include "SimulationController.h"
#include "Grid3D.h"
#include "Grid2D.h"


SimulationController::SimulationController(uint32_t countX, uint32_t countY, uint32_t countZ)
: m_CellCountX(countX)
, m_CellCountY(countY)
, m_CellCountZ(countZ)
, m_CellCount(countX*countY*countZ)
, m_RealCountX(countX+2)
, m_RealCountY(countY+2)
, m_RealCountZ(countZ+2)
{
	//m_Grid = new Grid3D(countX, countY, countZ);
}

SimulationController::SimulationController(uint32_t countX, uint32_t countY)
: m_CellCountX(countX)
, m_CellCountY(countY)
, m_CellCountZ(0)
, m_CellCount(countX*countY)
, m_RealCountX(countX+2)
, m_RealCountY(countY+2)
, m_RealCountZ(0)
{
	m_Grid = new Grid2D(countX, countY);
	m_Grid->initDensity();
	m_Grid->initTemperature();
	m_Grid->initVelocityField();
	m_Grid->initGridPositions();

	m_Divergence = new float[m_RealCountX*m_RealCountY];
	m_LinSolutionNew = new float[m_RealCountX*m_RealCountY];
	m_LinSolutionOld = new float[m_RealCountX*m_RealCountY];
}

SimulationController::~SimulationController(void)
{
	delete m_Grid;
}

void SimulationController::reset()
{
	m_Grid->reset();
}

void SimulationController::update(UpdateStruct& data)
{
	//advect always first bevause advection needs a divergence-free velocity field
	m_Grid->advectCentralQuantities(data);
	m_Grid->advectVelocities(data);
	

	if (data.addInk) addInk(data);
	if (data.addForce) addForces(data);

	//project always as the last step so in the next update step 
	//advection can be called on a divergence-free velocity field
	if (data.projectMode != ProjectMode::noProject)	project(data);
	
	m_Grid->switchQuantities();
}

void SimulationController::project(UpdateStruct& data)
{
	memset(m_Divergence, 0, sizeof(float) * m_RealCountX*m_RealCountY);
	memset(m_LinSolutionNew, 0, sizeof(float) * m_RealCountX*m_RealCountY);
	memset(m_LinSolutionOld, 0, sizeof(float) * m_RealCountX*m_RealCountY);

	float* velocityU = m_Grid->getVelocityBackupU();
	float* velocityV = m_Grid->getVelocityBackupV();
	
	for (uint32_t j = 1; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			m_Divergence[i + j*m_RealCountX] =	- (velocityU[i + j*(m_RealCountX+1) + 1] - velocityU[i + j*(m_RealCountX+1)]
												+ velocityV[i + (j+1)*m_RealCountX] - velocityV[i + j*m_RealCountX]);
			//m_LinSolutionNew[i + j*m_RealCountX] = m_Divergence[i + j*m_RealCountX] * 0.25f / m_CellCountX;
			//m_LinSolutionNew[i + j*m_RealCountX] = 0.0f;
		}
	}

	if (data.projectMode == ProjectMode::useJacobi) projectJacobi(data);
	else projectGaussSeidel(data);

	uint32_t velocityIndex = 0, presCentralIndex = 0, presHorizIndex = 0, presVertIndex = 0;
	//1. subtract pressure from u component
	for (uint32_t j = 0; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			velocityIndex = i + j*(m_RealCountX+1);
			presCentralIndex = i+j*m_RealCountX;
			presHorizIndex = presCentralIndex - 1;
			velocityU[velocityIndex] -= (float) data.timeDelta * (m_LinSolutionNew[presCentralIndex] - m_LinSolutionNew[presHorizIndex]);
		}
	}

	//2. subtract pressure from v component
	for (uint32_t j = 1; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			velocityIndex = i + j*m_RealCountX;
			presCentralIndex = velocityIndex;
			presVertIndex = presCentralIndex-m_RealCountX;
			velocityV[velocityIndex] -= (float) data.timeDelta * (m_LinSolutionNew[presCentralIndex] - m_LinSolutionNew[presVertIndex]);
		}
	}
}

void SimulationController::projectJacobi(UpdateStruct& data)
{
	float preIteration;
	float* temp;
	
	for (uint32_t iterations = 0; iterations < data.solverIterations; iterations++)
	{
		temp = m_LinSolutionNew;
		m_LinSolutionNew = m_LinSolutionOld;
		m_LinSolutionOld = temp;
		for (uint32_t j = 1; j <= m_CellCountY; j++)
		{
			for (uint32_t i = 1; i <= m_CellCountX; i++)
			{
				preIteration =	m_LinSolutionOld[i + j*m_RealCountX + 1] 
								+ m_LinSolutionOld[i + j*m_RealCountX - 1]  
								+ m_LinSolutionOld[i + (j+1)*m_RealCountX] 
								+ m_LinSolutionOld[i + (j-1)*m_RealCountX];
				m_LinSolutionNew[i + j*m_RealCountX] = 0.25f * (m_Divergence[i + j*m_RealCountX] + preIteration);
			}
		}
	}
}

void SimulationController::projectGaussSeidel(UpdateStruct& data)
{
	float preIteration;
	
	for (uint32_t iterations = 0; iterations < data.solverIterations; iterations++)
	{
		for (uint32_t j = 1; j <= m_CellCountY; j++)
		{
			for (uint32_t i = 1; i <= m_CellCountX; i++)
			{
				preIteration =	m_LinSolutionNew[i + j*m_RealCountX + 1] 
								+ m_LinSolutionNew[i + j*m_RealCountX - 1]  
								+ m_LinSolutionNew[i + (j+1)*m_RealCountX] 
								+ m_LinSolutionNew[i + (j-1)*m_RealCountX];
				m_LinSolutionNew[i + j*m_RealCountX] = 0.25f * (m_Divergence[i + j*m_RealCountX] + preIteration);
			}
		}
	}
}



float SimulationController::getDivergence(const uint32_t i, const uint32_t j, const float* velocityU, const float* velocityV)
{
	float divergence = velocityU[i + j*(m_CellCountX+1) + 1] - velocityU[i + j*(m_CellCountX+1)];
	divergence += (velocityV[i + (j+1)*m_CellCountX] - velocityV[i + j*m_CellCountX]);
	
	return divergence;
}

void SimulationController::addInk(UpdateStruct& data)
{
	m_Grid->addDensity(data);
}

void SimulationController::addForces(UpdateStruct& data)
{
	m_Grid->addVelocity(data);
}

uint32_t SimulationController::getVelocityUCount()
{
	return m_Grid->getVelocityUCount();
}

uint32_t SimulationController::getVelocityVCount()
{
	return m_Grid->getVelocityVCount();
}

float* SimulationController::getVelocityU()
{
	return m_Grid->getVelocityU();
}

float* SimulationController::getVelocityV()
{
	return m_Grid->getVelocityV();
}

float* SimulationController::getDensity()
{
	return m_Grid->getDensity();
}

uint32_t SimulationController::getCellCountHorizontal()
{
	return m_RealCountX;
}

uint32_t SimulationController::getCellCountVertical()
{
	return m_RealCountY;
}