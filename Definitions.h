#pragma once

//Linear Interpolation
#define LI(a, b, alpha) ((1.0f - alpha)*a + alpha*b) 

//Bilinear Interpolation
#define BI(a0, a1, b0, b1, alpha, beta) ((1.0f - beta)*LI(a0, a1, alpha) + beta*LI(b0, b1, alpha))

//Trilinear Interpolation
//#define TI(a0, a1, a2, a3, b0, b1, b2, b3, alpha, beta, gamma) ()

//integrate x milliseconds
const double	MIN_TIME_DELTA_IN_MILLISEC = 100.0/3.0;
const double	MIN_TIME_DELTA_IN_SEC = 0.1;
const double	MAX_TIME_DELTA_IN_SEC = 0.1;

const float		MINIMUM_VALUE = 0.001f;
const float		MAXIMUM_VALUE = 10.0f;


enum FluidMode
{
	showDensities,
	showVelocities
};

enum IntegrationMode
{
	useEuler,
	useRK
};

enum ProjectMode
{
	noProject,
	useJacobi,
	useGaussSeidel
};

struct UpdateStruct
{
	bool addInk;
	bool addForce;
	
	glm::vec2 mousePos; 
	glm::vec2 mouseMove;
	
	double timeDelta;

	FluidMode fluidMode;
	IntegrationMode intMode;
	ProjectMode projectMode;
	uint32_t solverIterations;

	float splatRadiusDensity;// = 0.005f;
	float splatRadiusVelocity;// = 0.005f;

	float dissipationDensity;// = 0.99f;
	float dissipationVelocity;// = 0.99f;

	glm::vec4 color;

	float velocityMultiplicator;// = 1000.0f;
};