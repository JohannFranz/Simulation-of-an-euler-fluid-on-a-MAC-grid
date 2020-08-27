#pragma once
#include "BaseGrid.h"

using glm::vec3;

class Grid3D : public BaseGrid
{
public:

					Grid3D(uint32_t countX, uint32_t countY, uint32_t countZ);
	virtual			~Grid3D(void);

	void			initTemperature();
	void			initVelocityField();
	void			initGridPositions();

	//void			update() override;
	void			advectVelocities(UpdateStruct& data) override;
	void			switchQuantities() override;

private:
	uint32_t		m_CellCountZ;

	vec3*			m_Velocity;
	vec3*			m_VelocityPrev;
	vec3*			m_Position;
};

