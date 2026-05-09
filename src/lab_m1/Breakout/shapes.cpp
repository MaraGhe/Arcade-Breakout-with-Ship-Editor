#include "lab_m1/Breakout/shapes.h"

Mesh* shapes::rectangle(glm::vec3 bottomLeftCorner, float length, float height,
    const std::string& name, glm::vec3 color, bool fill)
{
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(bottomLeftCorner, color),
        VertexFormat(bottomLeftCorner + glm::vec3(length, 0, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(length, height, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(0, height, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* shapes::start_button(glm::vec3 bottomLeftCorner, float length, float height,
    const std::string& name, bool fill, bool validBuild)
{
    glm::vec3 color;
    if (validBuild)
        color = glm::vec3(0, 1, 0);
    else
        color = glm::vec3(1, 0, 0);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(bottomLeftCorner, color),
        VertexFormat(bottomLeftCorner + glm::vec3(length, 0, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(length / 2, height / 2, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(length, height, 0), color),
        VertexFormat(bottomLeftCorner + glm::vec3(0, height, 0), color)
    };

    Mesh* button = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4 };

    if (!fill) {
        button->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
    }

    button->InitFromData(vertices, indices);
    return button;
}

Mesh* shapes::circle(glm::vec3 center, float radius, float segments, 
    const std::string& name, glm::vec3 color, bool fill)
{
    Mesh* circle = new Mesh(name);
    std::vector<VertexFormat> vertices{};
    std::vector<unsigned int> indices{};

    if (fill)
    {
        vertices.push_back(VertexFormat(center, color));
        indices.push_back(0);
    }

    for (int i = 0; i < segments; i++)
    {
        float angle = i * (RADIANS(360) / segments);
        vertices.push_back(VertexFormat(
            center + glm::vec3(radius * sin(angle), radius * cos(angle), 0), color));

        if (fill)
            indices.push_back(i + 1);
        else
            indices.push_back(i);
    }

    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else
    {
        circle->SetDrawMode(GL_TRIANGLE_FAN);
        indices.push_back(1);
    }
    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* shapes::heart(glm::vec3 bottomLeftCorner, float length, float height,
    const std::string& name, glm::vec3 color, bool fill)
{
    Mesh* heart = new Mesh(name);
    std::vector<VertexFormat> vertices{};
    std::vector<unsigned int> indices{};

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(0.5f * length, 0, 0), color));
    indices.push_back(0);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(length, 2.0f / 5.0f * height, 0), color));
    indices.push_back(1);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(length, 4.0f / 5.0f * height, 0), color));
    indices.push_back(2);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(5.0f / 6.0f * length, height, 0), color));
    indices.push_back(3);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(4.0f / 6.0f * length, height, 0), color));
    indices.push_back(4);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(0.5f * length, 4.0f / 5.0f * height, 0), color));
    indices.push_back(5);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(2.0f / 6.0f * length, height, 0), color));
    indices.push_back(6);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(1.0f / 6.0f * length, height, 0), color));
    indices.push_back(7);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(0, 4.0f / 5.0f * height, 0), color));
    indices.push_back(8);

    vertices.push_back(VertexFormat(bottomLeftCorner + glm::vec3(0, 2.0f / 5.0f * height, 0), color));
    indices.push_back(9);

    if (!fill) {
        heart->SetDrawMode(GL_LINE_LOOP);
    }
    else
    {
        heart->SetDrawMode(GL_TRIANGLE_FAN);
        indices.push_back(1);
    }
    heart->InitFromData(vertices, indices);
    return heart;
}