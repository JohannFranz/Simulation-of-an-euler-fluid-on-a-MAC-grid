#pragma once
#include "glm.hpp"
#include "../Definitions.h"

class BaseGrid
{
public:
	virtual				~BaseGrid(void);

	virtual void		advectVelocities(UpdateStruct& data) = 0;
	virtual void		advectCentralQuantities(UpdateStruct& data) = 0;
	virtual void		addDensity(UpdateStruct& data) = 0;
	virtual void		addVelocity(UpdateStruct& data) = 0;

	virtual void		reset() = 0;
	virtual void		switchQuantities() = 0;
	
	//virtual void		subtractPressureGradient(Eigen::VectorXd& pressure, double dt) = 0;

	virtual void		initDensity() = 0;
	virtual void		initTemperature() = 0;
	virtual void		initVelocityField() = 0;
	virtual void		initGridPositions() = 0;

	float*				getDensity();
	float*				getPressure();
	float*				getTemperature();

	virtual float*		getVelocityU() = 0;
	virtual float*		getVelocityV() = 0;
	virtual float*		getVelocityBackupU() = 0;
	virtual float*		getVelocityBackupV() = 0;
	virtual uint32_t	getVelocityUCount() = 0;
	virtual uint32_t	getVelocityVCount() = 0;

	uint32_t			getHorizontalCellCount();
	uint32_t			getVerticalCellCount();

protected:
						BaseGrid(uint32_t cellCountX, uint32_t cellCountY);
	float				getLinearWeight(float start, float end);

	float*				m_Pressure;
	//float*				m_PressurePrev;

	float*				m_Temperature;
	float*				m_TemperaturePrev;

	float*				m_Density;
	float*				m_DensityPrev;

	const uint32_t		m_CellCountX;
	const uint32_t		m_CellCountY;
	const uint32_t		m_RealCountX;
	const uint32_t		m_RealCountY;

	float				m_Width;
};

