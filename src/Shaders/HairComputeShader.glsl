#version 460 core
#define MAX_VERTICES_PER_STRAND 50
#define FTL 0
#define VOLUMES 1
#define COLLISIONS 2

layout (local_size_x = 128) in;

layout (std430, binding = 0) buffer HairPosition {
	float positions[][3];
};

layout (std430, binding = 1) buffer HairVelocity {
	float velocities[][3];
};

layout (std430, binding = 2) buffer volumeDensity {
	float volumeDensities[10][10][10];
};

layout (std430, binding = 3) buffer volumeVelocity {
	float volumeVelocities[10][10][10][3];
};

struct HairData {
	uint particlesPerStrand;
	uint strandCount;
	float particleMass;
	float segmentLength;
};

struct Force {
	vec4 wind;
	float gravity;
};

uniform float headRadius;
uniform uint state;
uniform Force force;
uniform HairData hairData;
uniform float deltaTime;
uniform float runningTime;
uniform float velocityDampingCoefficient = 1.0;
uniform float frictionCoefficient = 0.0;

vec3 followTheLeader(in vec3 leaderParticlePosition, in vec3 proposedParticlePosition, inout vec3 positionCorrectionVector) 
{
	const vec3 direction = normalize(proposedParticlePosition - leaderParticlePosition);
	vec3 fixedPosition = leaderParticlePosition + (direction * hairData.segmentLength);
	positionCorrectionVector = fixedPosition - proposedParticlePosition;
	return fixedPosition;
}

vec3 generateGravityForce() 
{
	return hairData.particleMass * vec3(0.0, force.gravity, 0.0);
}

vec3 generateWindForce(in vec3 particlePosition) 
{
	if (vec3(force.wind) == vec3(0.0)) 
	{
		return force.wind.w * normalize(vec3(
					sin(runningTime + particlePosition.z * 20.0),
                    cos(deltaTime * particlePosition.y * 5.0),
                    sin(runningTime + particlePosition.x * 30.0)

			   ));
	} 
	else
	{
		return normalize(vec3(force.wind)) * force.wind.w;
	}
}

vec3 integrateExplicitEuler(in vec3 forces, in vec3 particlePosition, in vec3 particleVelocity)
{
	const vec3 acceleration = forces / hairData.particleMass;
	return (particlePosition + (particleVelocity * deltaTime) + (acceleration * deltaTime * deltaTime));
}

vec3 updateVelocity(in vec3 oldPosition, in vec3 newPosition) 
{
	return ((newPosition - oldPosition) / deltaTime);
}

// Very useful article: https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/interpolation/introduction
vec3 interpolateVelocity(in vec3 particlePosition)
{
	particlePosition += 5;
	ivec3 flooredCoords = ivec3(floor(particlePosition));

	// Upper limit of the regular voxel grid, flooring to 3
	if (flooredCoords.x >= 9) flooredCoords.x = 8;
	if (flooredCoords.y >= 9) flooredCoords.y = 8;
	if (flooredCoords.z >= 9) flooredCoords.z = 8;

	vec3 voxelVertexVelocities[2][2][2];
	for (uint i = 0; i < 2; ++i)
	{
		for (uint j = 0; j < 2; ++j)
		{
			for (uint k = 0; k < 2; ++k)
			{
				voxelVertexVelocities[i][j][k].x = volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][0];
				voxelVertexVelocities[i][j][k].y = volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][1];
				voxelVertexVelocities[i][j][k].z = volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][2];
				voxelVertexVelocities[i][j][k] /= volumeDensities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k];
			}
		}
	}

	// Trilinear interpolation
	const float tx = abs(particlePosition.x - flooredCoords.x);
	const float ty = abs(particlePosition.y - flooredCoords.y);
	const float tz = abs(particlePosition.z - flooredCoords.z);

	// 4 linear interpolations on x-axis
	const vec3 xPoint1 = tx * voxelVertexVelocities[0][0][0] + (1 - tx) * voxelVertexVelocities[1][0][0];
	const vec3 xPoint2 = tx * voxelVertexVelocities[0][0][1] + (1 - tx) * voxelVertexVelocities[1][0][1];
	const vec3 xPoint3 = tx * voxelVertexVelocities[0][1][0] + (1 - tx) * voxelVertexVelocities[1][1][0];
	const vec3 xPoint4 = tx * voxelVertexVelocities[0][1][1] + (1 - tx) * voxelVertexVelocities[1][1][1];

	// 2 linear interpolations on y-axis
	const vec3 yPoint1 = ty * xPoint1 + (1 - ty) * xPoint3;
	const vec3 yPoint2 = ty * xPoint2 + (1 - ty) * xPoint4;

	// 1 linear interpolation on z-axis
	const vec3 zPoint = tz * yPoint1 + (1 - tz) * yPoint2;

	return zPoint;
}

vec3 correctFtlVelocity(in vec3 currentParticleVelocity, in vec3 nextParticleCorrectionVector) 
{
	const vec3 correctedVelocity = currentParticleVelocity - velocityDampingCoefficient * (nextParticleCorrectionVector / deltaTime);
	return correctedVelocity;
}

vec3 addHairFriction(in vec3 currentParticleVelocity, in vec3 particlePosition)
{
	const vec3 frictionVelocity = (1.0 - frictionCoefficient) * currentParticleVelocity + frictionCoefficient * interpolateVelocity(particlePosition);
	return frictionVelocity;
}

void fillVolumes(in vec3 particlePositions[MAX_VERTICES_PER_STRAND], in vec3 particleVelocities[MAX_VERTICES_PER_STRAND]) 
{
	for (uint index = 0; index < hairData.particlesPerStrand; ++index)
	{
		// Adding 2 to linearly map [-2,2] range to [0,4] range
		const vec3 particlePosition = particlePositions[index] + 5.0;
		const vec3 particleVelocity = particleVelocities[index];
		ivec3 flooredCoords = ivec3(floor(particlePosition));
		if (flooredCoords.x == 9) flooredCoords.x = 8;
		if (flooredCoords.y == 9) flooredCoords.y = 8;
		if (flooredCoords.z == 9) flooredCoords.z = 8;

		for (uint i = 0; i < 2; ++i)
		{
			for (uint j = 0; j < 2; ++j)
			{
				for (uint k = 0; k < 2; ++k)
				{
					float densityWeight = (1.0 - abs(particlePosition.x - flooredCoords.x - i)) * (1.0 - abs(particlePosition.y - flooredCoords.y - j)) * (1.0 - abs(particlePosition.z - flooredCoords.z - k));
					volumeDensities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k] += densityWeight;
					volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][0] += densityWeight * particleVelocity.x;
					volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][1] += densityWeight * particleVelocity.y;
					volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][2] += densityWeight * particleVelocity.z;
				}
			}
		}
	}
}

void resolveBodyCollision(inout vec3 particlePosition) {
	if (length(particlePosition) < headRadius + 0.02f) 
		particlePosition = normalize(particlePosition) * (headRadius + 0.02f);
}

void main(void)
{
	if (gl_GlobalInvocationID.x > hairData.strandCount)
		return; 

	vec3 particlePositions[MAX_VERTICES_PER_STRAND];
	vec3 particleVelocities[MAX_VERTICES_PER_STRAND];

	uint offset = gl_GlobalInvocationID.x * hairData.particlesPerStrand;

	for (uint i = 0; i < hairData.particlesPerStrand; ++i)
	{
		const uint particleOffset = offset + i;
		// Positions
		particlePositions[i].x = positions[particleOffset][0];
		particlePositions[i].y = positions[particleOffset][1];
		particlePositions[i].z = positions[particleOffset][2];

		// Velocities
		particleVelocities[i].x = velocities[particleOffset][0];
		particleVelocities[i].y = velocities[particleOffset][1];
		particleVelocities[i].z = velocities[particleOffset][2];
	}

	switch (state)
	{
		case FTL:
			vec3 forces, proposedPosition;
			vec3 positionCorrectionVector[MAX_VERTICES_PER_STRAND];
			for (uint i = 1; i < hairData.particlesPerStrand; ++i) 
			{
				forces = generateWindForce(particlePositions[i]);
				forces += generateGravityForce();

				proposedPosition = integrateExplicitEuler(forces, particlePositions[i], particleVelocities[i]);
				resolveBodyCollision(proposedPosition);
				proposedPosition = followTheLeader(particlePositions[i - 1], proposedPosition, positionCorrectionVector[i]);	
				particleVelocities[i] = updateVelocity(particlePositions[i], proposedPosition);
				particlePositions[i] = proposedPosition;
			}

			uint i;
			for (i = 1; i < hairData.particlesPerStrand - 1; ++i)
			{
				particleVelocities[i] = correctFtlVelocity(particleVelocities[i], positionCorrectionVector[i + 1]);
			}
			particleVelocities[i] = correctFtlVelocity(particleVelocities[i], vec3(0.0));
			break;

		case VOLUMES:
			fillVolumes(particlePositions, particleVelocities);
			break;

		case COLLISIONS:
			for (i = 1; i < hairData.particlesPerStrand; ++i)
			{
				particleVelocities[i] = addHairFriction(particleVelocities[i], particlePositions[i]);
			}
			break;
	}

	for (uint i = 0; i < hairData.particlesPerStrand; ++i)
	{
		positions[offset][0] = particlePositions[i].x;
		positions[offset][1] = particlePositions[i].y;
		positions[offset][2] = particlePositions[i].z;

		velocities[offset][0] = particleVelocities[i].x;
		velocities[offset][1] = particleVelocities[i].y;
		velocities[offset][2] = particleVelocities[i].z;

		++offset;
	}
}
