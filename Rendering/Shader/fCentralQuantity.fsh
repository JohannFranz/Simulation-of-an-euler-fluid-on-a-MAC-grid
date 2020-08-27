#version 430

layout(location = 0) out vec4 fColor;

in vec2 uv;

uniform int cellCountHoriz; //count of horizontal cells
uniform int cellCountVert; //count of vertical cells

uniform vec4 color;

uniform float q[16000];

//cell indices
int i, j;

//width and height in uv-coordinates
float width = 1.0f/float(cellCountHoriz); 
float height = 1.0f/float(cellCountVert);

//weight for linear interpolation
float alpha, beta;

//Quantity values for bilinear interpolation
float qCurrent, qHorizontal, qVertical, qDiagonal;

bool leftEdge = false, rightEdge = false, topEdge = false, bottomEdge = false;

float getQuantity(int i, int j)
{
	//check if fragment is left or right to center
	float iMid = float(i)*width + width*0.5f;
	
	//check if fragment is above or below to center
	float jMid = float(j)*height + height*0.5f;
	
	float col = 0.0f;
	
	int horizontalIndexIncrease, verticalIndexIncrease;
	if (uv.x > iMid)
	{
		horizontalIndexIncrease = 1;
		alpha = (uv.x - iMid) * float(cellCountHoriz);
	} else 
	{
		horizontalIndexIncrease = -1;
		alpha = -(uv.x - iMid) * float(cellCountHoriz);
	}
	
	if (uv.y > jMid)
	{
		verticalIndexIncrease = 1;
		beta = (uv.y - jMid) * float(cellCountVert);
	} else 
	{
		verticalIndexIncrease = -1;
		beta = -(uv.y - jMid) * float(cellCountVert);
	}
	
	qCurrent = q[i+j*cellCountHoriz];
	if ((horizontalIndexIncrease > 0 && rightEdge) ||
		(horizontalIndexIncrease < 0 && leftEdge))
	{
		qHorizontal = 0.0f;
	} 
	else qHorizontal = q[i+j*cellCountHoriz + horizontalIndexIncrease];
	
	if ((verticalIndexIncrease > 0 && topEdge) ||
		(verticalIndexIncrease < 0 && bottomEdge))
	{
		qVertical = 0.0f;
	} 
	else qVertical = q[i+(j+verticalIndexIncrease)*cellCountHoriz];
	
	if ((verticalIndexIncrease > 0 && topEdge) ||
	(verticalIndexIncrease < 0 && bottomEdge) ||
	(horizontalIndexIncrease > 0 && rightEdge) ||
	(horizontalIndexIncrease < 0 && leftEdge))
	{
		qDiagonal = 0.0f;
	} 
	else qDiagonal = q[i+(j+verticalIndexIncrease)*cellCountHoriz + horizontalIndexIncrease];
		
	float lin1 = mix(qCurrent, qHorizontal, alpha);
	float lin2 = mix(qVertical, qDiagonal, alpha);
	return mix(lin1, lin2, beta);
}

void main() {
	i = int(uv.x * (cellCountHoriz));
	j = int(uv.y * (cellCountVert));

	if (i == 0) leftEdge = true;
	if (i == cellCountHoriz) rightEdge = true;
	if (j == 0) bottomEdge = true;
	if (j == cellCountVert) topEdge = true;
	
	float quantity = getQuantity(i, j);
	
	fColor = vec4(color.xyz * quantity, 1.0f);
}