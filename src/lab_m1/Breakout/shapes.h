#include <vector>

#include "core/engine.h"
#include "core/gpu/mesh.h"
#include "utils/gl_utils.h"

namespace shapes
{
	Mesh* rectangle(glm::vec3 bottomLeftCorner, float length, float height,
		const std::string& name, glm::vec3 color, bool fill);

	Mesh* start_button(glm::vec3 bottomLeftCorner, float length, float height,
		const std::string& name, bool fill, bool validBuild);

	Mesh* circle(glm::vec3 center, float radius, float segments,
		const std::string& name, glm::vec3 color, bool fill);

	Mesh* heart(glm::vec3 bottom, float length, float height,
		const std::string& name, glm::vec3 color, bool fill);
}