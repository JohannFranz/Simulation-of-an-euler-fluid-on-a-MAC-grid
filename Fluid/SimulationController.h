#pragma once
#include "glm.hpp"
#include "../Definitions.h"
class BaseGrid;

class SimulationController
{

public:
					SimulationController(uint32_t countX, uint32_t countY, uint32_t countZ);
					SimulationController(uint32_t countX, uint32_t countY);
					~SimulationController(void);

	void			update(UpdateStruct& data);
	void			reset();

	uint32_t		getVelocityUCount();
	uint32_t		getVelocityVCount();
	float*			getVelocityU();
	float*			getVelocityV();

	float*			getDensity();

	uint32_t		getCellCountHorizontal();
	uint32_t		getCellCountVertical();

private:
	void			project(UpdateStruct& data);
	void			projectJacobi(UpdateStruct& data);
	void			projectGaussSeidel(UpdateStruct& data);

	void			addForces(UpdateStruct& data);
	void			addInk(UpdateStruct& data);

	float			getDivergence(const uint32_t i, const uint32_t j, const float* velocityU, const float* velocityV);

	const uint32_t	m_CellCountX;
	const uint32_t	m_CellCountY;
	const uint32_t	m_CellCountZ;
	const uint32_t	m_CellCount;

	const uint32_t	m_RealCountX;
	const uint32_t	m_RealCountY;
	const uint32_t	m_RealCountZ;

	BaseGrid*		m_Grid;

	float*			m_Divergence;
	float*			m_LinSolutionOld;
	float*			m_LinSolutionNew;
};

