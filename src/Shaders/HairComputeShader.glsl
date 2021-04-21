#version 460 core
#define MAX_VERTICES_PER_STRAND 50

layout (local_size_x = 128) in;

layout (std430, binding = 1) buffer HairPositions {
	float positions[];
};

layout (std430, binding = 2) buffer HairVelocities {
	float velocities[];
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

uniform Force force;
uniform HairData hairData;
uniform float deltaTime;
uniform float runningTime;
uniform float velocityDampingCoefficient = 1.0;

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
		return 	force.wind.w * vec3(
					sin(runningTime + particlePosition.z * 20.0), 
					cos(deltaTime * particlePosition.y * 5.0), 
					sin(runningTime + particlePosition.x * 30.0)
				);
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

vec3 addVelocityCorrection(in vec3 currentParticleVelocity, in vec3 nextParticleCorrectionVector)
{
	return (currentParticleVelocity - velocityDampingCoefficient * (nextParticleCorrectionVector / deltaTime));
}

void main(void)
{
	if (gl_GlobalInvocationID.x > hairData.strandCount * hairData.particlesPerStrand * 3)
		return; 

	vec3 particlePositions[MAX_VERTICES_PER_STRAND];
	vec3 particleVelocities[MAX_VERTICES_PER_STRAND];

	uint offset = gl_GlobalInvocationID.x * hairData.particlesPerStrand * 3;

	for (uint i = 0; i < hairData.particlesPerStrand; ++i)
	{
		const uint floatOffset = offset + i * 3;
		// Positions
		particlePositions[i].x = positions[floatOffset];
		particlePositions[i].y = positions[floatOffset + 1];
		particlePositions[i].z = positions[floatOffset + 2];

		// Velocities
		particleVelocities[i].x = velocities[floatOffset];
		particleVelocities[i].y = velocities[floatOffset + 1];
		particleVelocities[i].z = velocities[floatOffset + 2];
	}

	/*
	* Updates first particle outside the following update loop
	* because of veloctity correction updates which are starting from particle 2 
	* so the goal is to avoid checking if (i > 1)
	*/
	uint i = 1;
	vec3 positionCorrectionVector[MAX_VERTICES_PER_STRAND];
	vec3 forces = vec3(0.0);
	forces = generateWindForce(particlePositions[i]);
	forces += generateGravityForce();

	vec3 proposedPosition = integrateExplicitEuler(forces, particlePositions[i], particleVelocities[i]);
	proposedPosition = followTheLeader(particlePositions[i - 1], proposedPosition, positionCorrectionVector[i]);	
	particleVelocities[i] = updateVelocity(particlePositions[i], proposedPosition);
	particlePositions[i] = proposedPosition;

	for (i = 2; i < hairData.particlesPerStrand; ++i) 
	{
		forces = generateWindForce(particlePositions[i]);
		forces += generateGravityForce();

		proposedPosition = integrateExplicitEuler(forces, particlePositions[i], particleVelocities[i]);
		proposedPosition = followTheLeader(particlePositions[i - 1], proposedPosition, positionCorrectionVector[i]);	
		particleVelocities[i] = updateVelocity(particlePositions[i], proposedPosition);
		particlePositions[i] = proposedPosition;
		particleVelocities[i - 1] = addVelocityCorrection(particleVelocities[i - 1], positionCorrectionVector[i]);
	}

	for (uint i = 0; i < hairData.particlesPerStrand; ++i)
	{
		positions[offset] = particlePositions[i].x;
		positions[offset+1] = particlePositions[i].y;
		positions[offset+2] = particlePositions[i].z;

		velocities[offset++] = particleVelocities[i].x;
		velocities[offset++] = particleVelocities[i].y;
		velocities[offset++] = particleVelocities[i].z;
	}
}
