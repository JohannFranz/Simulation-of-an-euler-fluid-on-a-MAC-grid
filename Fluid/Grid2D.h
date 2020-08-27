#pragma once

#include "BaseGrid.h"

using glm::vec2;

class Grid2D : public BaseGrid
{

struct QuantityData
{
	uint32_t i;
	uint32_t j;
	vec2 particlePos;
	float* quantity;
	float* prevQuantity;
	int indexIncrease;
	float centralQuantity;
};

public:
					Grid2D(uint32_t cellCountX, uint32_t cellCountY);
	virtual			~Grid2D(void);

	void			initDensity() override;
	void			initTemperature() override;
	void			initVelocityField() override;
	void			initGridPositions() override;

	void			addDensity(UpdateStruct& data) override;
	void			addVelocity(UpdateStruct& data) override;
	void			advectVelocities(UpdateStruct& data) override;
	void			advectCentralQuantities(UpdateStruct& data) override;

	void			reset() override;
	void			switchQuantities() override;

	float*			getVelocityU() override;
	float*			getVelocityV() override;
	float*			getVelocityBackupU() override;
	float*			getVelocityBackupV() override;
	uint32_t		getVelocityUCount()	override;
	uint32_t		getVelocityVCount()	override;

private:
	void			advectCentralQuantity(QuantityData& qData, UpdateStruct& uData);
	void			advectVelocityU(uint32_t i, uint32_t j, UpdateStruct& data);
	void			advectVelocityV(uint32_t i, uint32_t j, UpdateStruct& data);

	void			getVelocityForCentralPosition(vec2& velocity, uint32_t x, uint32_t y);
	void			getVelocityForGridFaceX(vec2& velocity, uint32_t i, uint32_t j);
	void			getVelocityForGridFaceY(vec2& velocity, uint32_t i, uint32_t j);

	float			getPrevParticleQuantity(QuantityData& data);
	float			getPrevParticleVelocityU(const vec2& particlePos, uint32_t i, uint32_t j);
	float			getPrevParticleVelocityV(const vec2& particlePos, uint32_t i, uint32_t j);
	
	void			getIndicesForCell(const vec2& pos, uint32_t& i, uint32_t& j);
	void			getCentralPosition(vec2& centralPos, uint32_t i, uint32_t j);
	inline float	getCentralQuantity(uint32_t i, uint32_t j, float* quantity);
	
	float			lerpHorizontalQuantity(QuantityData& data);
	float			lerpVerticalQuantity(QuantityData& data);

	glm::vec2		integrateParticlePosition(UpdateStruct& data, vec2& position, vec2& velocity);

	float*			m_Velocity_U;
	float*			m_Velocity_U_Prev;
	float*			m_Velocity_U_Backup;
	
	float*			m_Velocity_V;
	float*			m_Velocity_V_Prev;
	float*			m_Velocity_V_Backup;

	uint32_t		m_CenterPositionOffset;
	vec2*			m_Position;
};

